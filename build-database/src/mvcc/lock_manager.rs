//! Lock Manager for MVCC Concurrency Control
//!
//! Provides basic locking mechanisms to prevent conflicts in concurrent transactions.
//! Uses multi-version concurrency control to minimize locking requirements.

use std::collections::HashMap;
use parking_lot::RwLock;
use crate::core::{AuroraResult, AuroraError, ErrorCode};
use crate::mvcc::transaction::TransactionId;

/// Lock types for different operations
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum LockType {
    Shared,    // For reads
    Exclusive, // For writes
}

/// Lock request
#[derive(Debug, Clone)]
pub struct LockRequest {
    pub transaction_id: TransactionId,
    pub resource: String, // e.g., "table:users:pk:1"
    pub lock_type: LockType,
}

/// Lock holder information
#[derive(Debug, Clone)]
struct LockHolder {
    transaction_id: TransactionId,
    lock_type: LockType,
}

/// Lock manager for basic concurrency control
pub struct LockManager {
    /// Current locks held by transactions
    locks: RwLock<HashMap<String, Vec<LockHolder>>>,
    /// Wait queue for lock requests
    wait_queue: RwLock<HashMap<String, Vec<LockRequest>>>,
}

impl LockManager {
    /// Create a new lock manager
    pub fn new() -> Self {
        Self {
            locks: RwLock::new(HashMap::new()),
            wait_queue: RwLock::new(HashMap::new()),
        }
    }

    /// Acquire a lock (simplified - always grants for now)
    pub async fn acquire_lock(&self, request: LockRequest) -> AuroraResult<()> {
        let mut locks = self.locks.write();

        // Check for conflicts (simplified)
        if let Some(holders) = locks.get(&request.resource) {
            for holder in holders {
                if holder.transaction_id != request.transaction_id {
                    // Check if lock types conflict
                    if Self::locks_conflict(request.lock_type, holder.lock_type) {
                        // For now, just queue the request (simplified deadlock prevention)
                        let mut wait_queue = self.wait_queue.write();
                        wait_queue.entry(request.resource.clone())
                            .or_insert_with(Vec::new)
                            .push(request);
                        return Ok(()); // Queued, not blocked
                    }
                }
            }
        }

        // Grant the lock
        locks.entry(request.resource.clone())
            .or_insert_with(Vec::new)
            .push(LockHolder {
                transaction_id: request.transaction_id,
                lock_type: request.lock_type,
            });

        Ok(())
    }

    /// Release locks held by a transaction
    pub async fn release_locks(&self, transaction_id: TransactionId) {
        let mut locks = self.locks.write();
        let mut wait_queue = self.wait_queue.write();

        // Remove locks held by this transaction
        for holders in locks.values_mut() {
            holders.retain(|holder| holder.transaction_id != transaction_id);
        }

        // Clean up empty lock entries
        locks.retain(|_, holders| !holders.is_empty());

        // Process waiting requests (simplified)
        // In a real implementation, this would wake up waiting transactions
        for (_resource, requests) in wait_queue.iter_mut() {
            requests.retain(|req| req.transaction_id != transaction_id);
        }
    }

    /// Check if two lock types conflict
    fn locks_conflict(type1: LockType, type2: LockType) -> bool {
        match (type1, type2) {
            (LockType::Exclusive, _) | (_, LockType::Exclusive) => true,
            _ => false, // Shared locks don't conflict
        }
    }

    /// Get lock statistics
    pub fn stats(&self) -> LockStats {
        let locks = self.locks.read();
        let wait_queue = self.wait_queue.read();

        let total_locks = locks.values().map(|holders| holders.len()).sum();
        let waiting_requests = wait_queue.values().map(|requests| requests.len()).sum();

        LockStats {
            total_locks,
            locked_resources: locks.len(),
            waiting_requests,
        }
    }
}

/// Lock manager statistics
#[derive(Debug, Clone)]
pub struct LockStats {
    pub total_locks: usize,
    pub locked_resources: usize,
    pub waiting_requests: usize,
}
