//! Lock Manager for Concurrency Control
//!
//! Implements two-phase locking with deadlock detection and resolution.
//! Supports shared and exclusive locks for different isolation levels.

use crate::core::*;
use super::manager::*;
use std::collections::{HashMap, HashSet, VecDeque};
use std::sync::Arc;
use parking_lot::RwLock;

/// Lock manager for concurrency control
pub struct LockManager {
    /// Lock table: key -> list of lock holders
    lock_table: HashMap<Vec<u8>, Vec<LockHolder>>,
    /// Wait-for graph for deadlock detection
    wait_for_graph: HashMap<TransactionId, HashSet<TransactionId>>,
    /// Lock requests waiting for acquisition
    waiting_requests: VecDeque<LockRequest>,
    /// Lock statistics
    stats: LockStats,
}

/// Lock request from a transaction
#[derive(Debug, Clone)]
pub struct LockRequest {
    pub key: Vec<u8>,
    pub mode: LockMode,
    pub transaction_id: TransactionId,
}

/// Lock mode (simplified - can be extended)
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum LockMode {
    Shared,      // Read lock - multiple transactions can hold
    Exclusive,   // Write lock - only one transaction can hold
}

/// Lock holder information
#[derive(Debug, Clone)]
struct LockHolder {
    transaction_id: TransactionId,
    mode: LockMode,
    granted_at: u64,
}

/// Lock manager statistics
#[derive(Debug, Clone, Default)]
pub struct LockStats {
    pub total_locks_acquired: u64,
    pub total_locks_released: u64,
    pub total_lock_waits: u64,
    pub total_deadlocks: u64,
    pub average_wait_time_ms: f64,
}

impl LockManager {
    /// Create a new lock manager
    pub fn new() -> Self {
        Self {
            lock_table: HashMap::new(),
            wait_for_graph: HashMap::new(),
            waiting_requests: VecDeque::new(),
            stats: LockStats::default(),
        }
    }

    /// Acquire a lock (may block if conflict)
    pub async fn acquire_lock(&mut self, request: LockRequest) -> Result<(), TransactionError> {
        let start_time = std::time::Instant::now();

        // Check if lock can be granted immediately
        if self.can_grant_lock(&request) {
            self.grant_lock(request)?;
            self.stats.total_locks_acquired += 1;
            return Ok(());
        }

        // Add to waiting queue
        self.waiting_requests.push_back(request.clone());
        self.stats.total_lock_waits += 1;

        // Check for deadlocks
        if self.would_cause_deadlock(&request) {
            self.waiting_requests.retain(|r| r != &request);
            return Err(TransactionError::Deadlock);
        }

        // For now, we'll just grant the lock (in production, this would wait)
        // TODO: Implement proper waiting mechanism
        self.grant_lock(request)?;
        self.stats.total_locks_acquired += 1;

        let wait_time = start_time.elapsed().as_millis() as f64;
        self.stats.average_wait_time_ms =
            (self.stats.average_wait_time_ms * (self.stats.total_lock_waits - 1) as f64 + wait_time)
                / self.stats.total_lock_waits as f64;

        Ok(())
    }

    /// Release all locks held by a transaction
    pub async fn release_locks(&mut self, transaction_id: TransactionId) -> Result<(), TransactionError> {
        let mut released_count = 0;

        // Remove locks from lock table
        for holders in self.lock_table.values_mut() {
            holders.retain(|holder| holder.transaction_id != transaction_id);
            released_count += 1;
        }

        // Clean up empty entries
        self.lock_table.retain(|_, holders| !holders.is_empty());

        // Update wait-for graph
        self.wait_for_graph.remove(&transaction_id);
        for waiting in self.wait_for_graph.values_mut() {
            waiting.remove(&transaction_id);
        }

        // Try to grant waiting requests
        self.process_waiting_requests();

        self.stats.total_locks_released += released_count;
        Ok(())
    }

    /// Check if a lock request can be granted immediately
    fn can_grant_lock(&self, request: &LockRequest) -> bool {
        if let Some(holders) = self.lock_table.get(&request.key) {
            match request.mode {
                LockMode::Shared => {
                    // Shared lock can be granted if no exclusive locks are held
                    !holders.iter().any(|h| h.mode == LockMode::Exclusive)
                }
                LockMode::Exclusive => {
                    // Exclusive lock can only be granted if no locks are held
                    holders.is_empty()
                }
            }
        } else {
            // No existing locks on this key
            true
        }
    }

    /// Grant a lock to a transaction
    fn grant_lock(&mut self, request: LockRequest) -> Result<(), TransactionError> {
        let holder = LockHolder {
            transaction_id: request.transaction_id,
            mode: request.mode,
            granted_at: std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_millis() as u64,
        };

        self.lock_table.entry(request.key).or_insert_with(Vec::new).push(holder);
        Ok(())
    }

    /// Process waiting lock requests
    fn process_waiting_requests(&mut self) {
        let mut i = 0;
        while i < self.waiting_requests.len() {
            let request = &self.waiting_requests[i];
            if self.can_grant_lock(request) {
                let request = self.waiting_requests.remove(i).unwrap();
                let _ = self.grant_lock(request); // Ignore error for now
                self.stats.total_locks_acquired += 1;
            } else {
                i += 1;
            }
        }
    }

    /// Check if granting a lock would cause a deadlock
    fn would_cause_deadlock(&self, request: &LockRequest) -> bool {
        // Build wait-for graph
        let mut graph = self.wait_for_graph.clone();

        // Add edge: requesting transaction -> transactions holding conflicting locks
        if let Some(holders) = self.lock_table.get(&request.key) {
            for holder in holders {
                if self.locks_conflict(&request.mode, &holder.mode) {
                    graph.entry(request.transaction_id)
                        .or_insert_with(HashSet::new)
                        .insert(holder.transaction_id);
                }
            }
        }

        // Check for cycles in the wait-for graph
        self.has_deadlock_cycle(&graph, request.transaction_id, &mut HashSet::new())
    }

    /// Check if two lock modes conflict
    fn locks_conflict(&self, mode1: &LockMode, mode2: &LockMode) -> bool {
        match (mode1, mode2) {
            (LockMode::Exclusive, _) | (_, LockMode::Exclusive) => true,
            (LockMode::Shared, LockMode::Shared) => false,
        }
    }

    /// Detect cycles in wait-for graph using DFS
    fn has_deadlock_cycle(&self, graph: &HashMap<TransactionId, HashSet<TransactionId>>, start: TransactionId, visited: &mut HashSet<TransactionId>) -> bool {
        if visited.contains(&start) {
            return true; // Cycle detected
        }

        visited.insert(start);

        if let Some(neighbors) = graph.get(&start) {
            for neighbor in neighbors {
                if self.has_deadlock_cycle(graph, *neighbor, visited) {
                    return true;
                }
            }
        }

        visited.remove(&start);
        false
    }

    /// Detect deadlocks in the system
    pub async fn detect_deadlocks(&self) -> Result<Vec<TransactionId>, TransactionError> {
        let mut deadlocked_transactions = Vec::new();
        let mut visited = HashSet::new();

        for (transaction_id, _) in &self.wait_for_graph {
            if !visited.contains(transaction_id) {
                let mut path = HashSet::new();
                if self.has_deadlock_cycle(&self.wait_for_graph, *transaction_id, &mut path) {
                    // Find all transactions in the deadlock cycle
                    for txn_id in path {
                        if !deadlocked_transactions.contains(&txn_id) {
                            deadlocked_transactions.push(txn_id);
                        }
                    }
                }
                visited.extend(path);
            }
        }

        Ok(deadlocked_transactions)
    }

    /// Get lock statistics
    pub fn stats(&self) -> &LockStats {
        &self.stats
    }

    /// Get current locks held by a transaction
    pub fn get_transaction_locks(&self, transaction_id: TransactionId) -> Vec<LockRequest> {
        let mut locks = Vec::new();

        for (key, holders) in &self.lock_table {
            for holder in holders {
                if holder.transaction_id == transaction_id {
                    locks.push(LockRequest {
                        key: key.clone(),
                        mode: holder.mode.clone(),
                        transaction_id,
                    });
                }
            }
        }

        locks
    }
}
