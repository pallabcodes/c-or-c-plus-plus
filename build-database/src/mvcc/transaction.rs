//! Transaction Management for MVCC
//!
//! Manages transaction lifecycle, IDs, and states for multi-version concurrency control.

use std::collections::HashMap;
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::Arc;
use parking_lot::RwLock;
use crate::core::{AuroraResult, AuroraError, ErrorCode};

/// Transaction ID type
pub type TransactionId = u64;

/// Transaction state
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum TransactionState {
    Active,
    Committed,
    Aborted,
    InProgress,
}

/// Isolation level for transactions
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum IsolationLevel {
    ReadUncommitted,
    ReadCommitted,
    RepeatableRead,
    Serializable,
}

impl Default for IsolationLevel {
    fn default() -> Self {
        IsolationLevel::ReadCommitted
    }
}

/// Transaction metadata
#[derive(Debug, Clone)]
pub struct Transaction {
    /// Unique transaction ID
    pub id: TransactionId,
    /// Transaction state
    pub state: TransactionState,
    /// Isolation level
    pub isolation_level: IsolationLevel,
    /// Start timestamp
    pub start_timestamp: u64,
    /// Commit timestamp (when committed)
    pub commit_timestamp: Option<u64>,
    /// Snapshot when transaction started (for repeatable read)
    pub snapshot: Option<crate::mvcc::snapshot::Snapshot>,
}

impl Transaction {
    /// Create a new transaction
    pub fn new(id: TransactionId, isolation_level: IsolationLevel, current_timestamp: u64) -> Self {
        Self {
            id,
            state: TransactionState::Active,
            isolation_level,
            start_timestamp: current_timestamp,
            commit_timestamp: None,
            snapshot: None,
        }
    }

    /// Check if transaction is active
    pub fn is_active(&self) -> bool {
        self.state == TransactionState::Active
    }

    /// Check if transaction is committed
    pub fn is_committed(&self) -> bool {
        self.state == TransactionState::Committed
    }

    /// Check if transaction is aborted
    pub fn is_aborted(&self) -> bool {
        self.state == TransactionState::Aborted
    }

    /// Check if transaction can be safely committed (conflict detection)
    pub fn can_commit(&self, txn_manager: &TransactionManager) -> bool {
        // For now, simple check - in a full implementation this would detect conflicts
        // with concurrent serializable transactions
        match self.isolation_level {
            IsolationLevel::Serializable => {
                // Check for conflicts in serializable isolation
                // This is a simplified version - real implementation would track read/write sets
                true // Assume no conflicts for now
            }
            _ => true, // Other isolation levels allow most commits
        }
    }

    /// Rollback transaction changes
    pub fn rollback(&mut self) {
        self.state = TransactionState::Aborted;
        log::info!("Rolled back transaction {}", self.id);
    }

    /// Get transaction age in milliseconds
    pub fn age_ms(&self) -> u64 {
        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_secs();

        (now - (self.start_timestamp / 1000)) * 1000
    }
}

/// Transaction manager for MVCC with ACID support
pub struct TransactionManager {
    /// Next transaction ID to assign
    next_txn_id: AtomicU64,
    /// Active transactions
    active_transactions: RwLock<HashMap<TransactionId, Arc<Transaction>>>,
    /// Committed transactions (for cleanup and statistics)
    committed_transactions: RwLock<Vec<Arc<Transaction>>>,
    /// Current timestamp (simplified - in real systems this would be more sophisticated)
    current_timestamp: AtomicU64,
    /// Lock manager for concurrency control
    lock_manager: Arc<LockManager>,
}

impl TransactionManager {
    /// Create a new transaction manager
    pub fn new() -> Self {
        Self {
            next_txn_id: AtomicU64::new(1),
            active_transactions: RwLock::new(HashMap::new()),
            committed_transactions: RwLock::new(Vec::new()),
            current_timestamp: AtomicU64::new(1),
            lock_manager: Arc::new(LockManager::new()),
        }
    }

    /// Begin a new transaction
    pub async fn begin_transaction(&self, isolation_level: IsolationLevel) -> AuroraResult<Arc<Transaction>> {
        let txn_id = self.next_txn_id.fetch_add(1, Ordering::SeqCst);
        let timestamp = self.current_timestamp.fetch_add(1, Ordering::SeqCst);

        let transaction = Arc::new(Transaction::new(txn_id, isolation_level, timestamp));

        // Store in active transactions
        {
            let mut active = self.active_transactions.write();
            active.insert(txn_id, transaction.clone());
        }

        log::info!("Started transaction {} with isolation level {:?}", txn_id, isolation_level);
        Ok(transaction)
    }

    /// Commit a transaction
    pub async fn commit_transaction(&self, txn_id: TransactionId) -> AuroraResult<()> {
        let mut active = self.active_transactions.write();

        if let Some(transaction) = active.get_mut(&txn_id) {
            if !transaction.is_active() {
                return Err(AuroraError::new(
                    ErrorCode::TransactionInvalidState,
                    format!("Transaction {} is not active", txn_id)
                ));
            }

            // Update transaction state
            let mut committed_txn = (**transaction).clone();
            committed_txn.state = TransactionState::Committed;
            committed_txn.commit_timestamp = Some(self.current_timestamp.fetch_add(1, Ordering::SeqCst));

            // Move to committed list
            let committed_arc = Arc::new(committed_txn);
            self.committed_transactions.write().push(committed_arc.clone());

            // Replace in active map
            *transaction = committed_arc;

            log::info!("Committed transaction {}", txn_id);
            Ok(())
        } else {
            Err(AuroraError::new(
                ErrorCode::TransactionNotFound,
                format!("Transaction {} not found", txn_id)
            ))
        }
    }

    /// Abort a transaction
    pub async fn abort_transaction(&self, txn_id: TransactionId) -> AuroraResult<()> {
        let mut active = self.active_transactions.write();

        if let Some(transaction) = active.get_mut(&txn_id) {
            if transaction.is_aborted() {
                return Ok(()); // Already aborted
            }

            // Update transaction state
            let mut aborted_txn = (**transaction).clone();
            aborted_txn.state = TransactionState::Aborted;

            // Replace in map
            *transaction = Arc::new(aborted_txn);

            log::info!("Aborted transaction {}", txn_id);
            Ok(())
        } else {
            Err(AuroraError::new(
                ErrorCode::TransactionNotFound,
                format!("Transaction {} not found", txn_id)
            ))
        }
    }

    /// Get transaction by ID
    pub fn get_transaction(&self, txn_id: TransactionId) -> Option<Arc<Transaction>> {
        let active = self.active_transactions.read();
        active.get(&txn_id).cloned()
    }

    /// Check if transaction is visible to another transaction
    pub fn is_transaction_visible(&self, txn_id: TransactionId, from_txn: &Transaction) -> bool {
        if let Some(txn) = self.get_transaction(txn_id) {
            match from_txn.isolation_level {
                IsolationLevel::ReadUncommitted => {
                    // Can see all versions, even uncommitted ones
                    true
                }
                IsolationLevel::ReadCommitted => {
                    // Can only see committed transactions
                    txn.is_committed()
                }
                IsolationLevel::RepeatableRead | IsolationLevel::Serializable => {
                    // Can see transactions that committed before this transaction started
                    if let Some(commit_ts) = txn.commit_timestamp {
                        commit_ts < from_txn.start_timestamp
                    } else {
                        false // Not committed yet
                    }
                }
            }
        } else {
            // Transaction not found - assume it's committed and visible
            // In a real system, we'd have a more sophisticated approach
            true
        }
    }

    /// Get current timestamp
    pub fn current_timestamp(&self) -> u64 {
        self.current_timestamp.load(Ordering::SeqCst)
    }

    /// Get statistics
    pub fn stats(&self) -> TransactionStats {
        let active = self.active_transactions.read();
        let committed = self.committed_transactions.read();

        let active_count = active.values().filter(|t| t.is_active()).count();
        let committed_count = committed.len() + active.values().filter(|t| t.is_committed()).count();
        let aborted_count = active.values().filter(|t| t.is_aborted()).count();

        TransactionStats {
            total_transactions: active.len(),
            active_transactions: active_count,
            committed_transactions: committed_count,
            aborted_transactions: aborted_count,
            current_timestamp: self.current_timestamp(),
        }
    }

    /// Cleanup old committed transactions (for memory management)
    pub fn cleanup_old_transactions(&self, max_age_ms: u64) {
        let mut committed = self.committed_transactions.write();
        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_secs() * 1000;

        committed.retain(|txn| {
            if let Some(commit_ts) = txn.commit_timestamp {
                let age = now - (commit_ts * 1000);
                age < max_age_ms
            } else {
                true // Keep if no commit timestamp
            }
        });
    }

    /// Get lock manager statistics
    pub fn lock_stats(&self) -> LockStats {
        self.lock_manager.stats()
    }
}

/// Transaction statistics
#[derive(Debug, Clone)]
pub struct TransactionStats {
    pub total_transactions: usize,
    pub active_transactions: usize,
    pub committed_transactions: usize,
    pub aborted_transactions: usize,
    pub current_timestamp: u64,
}