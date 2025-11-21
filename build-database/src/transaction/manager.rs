//! Transaction Manager
//!
//! Core transaction coordination with ACID guarantees and MVCC support.

use crate::core::*;
use super::mvcc::*;
use super::locking::*;
use super::logging::*;
use std::collections::HashMap;
use std::sync::Arc;
use parking_lot::RwLock;

/// Transaction manager for coordinating ACID operations
pub struct TransactionManager {
    /// Active transactions
    active_transactions: Arc<RwLock<HashMap<TransactionId, Transaction>>>,
    /// MVCC manager for version control
    mvcc_manager: Arc<RwLock<MVCCManager>>,
    /// Lock manager for concurrency control
    lock_manager: Arc<RwLock<LockManager>>,
    /// WAL manager for durability
    wal_manager: Arc<RwLock<WALManager>>,
    /// Next transaction ID
    next_txn_id: Arc<RwLock<u64>>,
    /// Transaction statistics
    stats: TransactionStats,
}

/// Transaction representation
#[derive(Debug, Clone)]
pub struct Transaction {
    pub id: TransactionId,
    pub status: TransactionStatus,
    pub isolation_level: IsolationLevel,
    pub start_time: u64,
    pub snapshot: Option<Snapshot>,
    pub modified_keys: Vec<Vec<u8>>,
    pub locks_held: Vec<LockRequest>,
}

/// Transaction status
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum TransactionStatus {
    Active,
    Committed,
    Aborted,
    Preparing,    // For distributed transactions
    Prepared,     // For distributed transactions
}

/// Transaction statistics
#[derive(Debug, Clone, Default)]
pub struct TransactionStats {
    pub total_transactions: u64,
    pub committed_transactions: u64,
    pub aborted_transactions: u64,
    pub average_duration_ms: f64,
    pub deadlock_count: u64,
    pub lock_wait_count: u64,
}

impl TransactionManager {
    /// Create a new transaction manager
    pub fn new() -> Self {
        Self {
            active_transactions: Arc::new(RwLock::new(HashMap::new())),
            mvcc_manager: Arc::new(RwLock::new(MVCCManager::new())),
            lock_manager: Arc::new(RwLock::new(LockManager::new())),
            wal_manager: Arc::new(RwLock::new(WALManager::new())),
            next_txn_id: Arc::new(RwLock::new(1)),
            stats: TransactionStats::default(),
        }
    }

    /// Begin a new transaction
    pub async fn begin_transaction(&mut self, isolation_level: IsolationLevel) -> Result<TransactionId, TransactionError> {
        let txn_id = TransactionId(*self.next_txn_id.read());
        *self.next_txn_id.write() += 1;

        // Create transaction snapshot for MVCC
        let snapshot = if matches!(isolation_level, IsolationLevel::Serializable | IsolationLevel::RepeatableRead) {
            Some(self.mvcc_manager.read().create_snapshot().await?)
        } else {
            None
        };

        let transaction = Transaction {
            id: txn_id,
            status: TransactionStatus::Active,
            isolation_level,
            start_time: std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_millis() as u64,
            snapshot,
            modified_keys: Vec::new(),
            locks_held: Vec::new(),
        };

        self.active_transactions.write().insert(txn_id, transaction);
        self.stats.total_transactions += 1;

        Ok(txn_id)
    }

    /// Commit a transaction
    pub async fn commit_transaction(&mut self, txn_id: TransactionId) -> Result<(), TransactionError> {
        let mut transactions = self.active_transactions.write();
        let mut mvcc = self.mvcc_manager.write();
        let mut wal = self.wal_manager.write();

        let transaction = transactions.get_mut(&txn_id)
            .ok_or(TransactionError::TransactionNotFound(txn_id))?;

        if transaction.status != TransactionStatus::Active {
            return Err(TransactionError::InvalidTransactionState(transaction.status.clone()));
        }

        // Acquire exclusive locks for modified keys (two-phase locking)
        for key in &transaction.modified_keys {
            let lock_request = LockRequest {
                key: key.clone(),
                mode: LockMode::Exclusive,
                transaction_id: txn_id,
            };
            self.lock_manager.write().acquire_lock(lock_request).await?;
            transaction.locks_held.push(lock_request);
        }

        // Log commit record to WAL
        let commit_record = LogRecord::Commit {
            transaction_id: txn_id,
            modified_keys: transaction.modified_keys.clone(),
        };
        wal.append_record(commit_record).await?;

        // Make changes visible in MVCC
        mvcc.commit_transaction(txn_id, transaction.snapshot.as_ref()).await?;

        // Release locks
        self.lock_manager.write().release_locks(txn_id).await?;

        transaction.status = TransactionStatus::Committed;
        self.stats.committed_transactions += 1;

        Ok(())
    }

    /// Abort a transaction
    pub async fn abort_transaction(&mut self, txn_id: TransactionId) -> Result<(), TransactionError> {
        let mut transactions = self.active_transactions.write();
        let mut mvcc = self.mvcc_manager.write();
        let mut wal = self.wal_manager.write();

        let transaction = transactions.get_mut(&txn_id)
            .ok_or(TransactionError::TransactionNotFound(txn_id))?;

        // Log abort record to WAL
        let abort_record = LogRecord::Abort {
            transaction_id: txn_id,
            modified_keys: transaction.modified_keys.clone(),
        };
        wal.append_record(abort_record).await?;

        // Rollback changes in MVCC
        mvcc.rollback_transaction(txn_id).await?;

        // Release locks
        self.lock_manager.write().release_locks(txn_id).await?;

        transaction.status = TransactionStatus::Aborted;
        self.stats.aborted_transactions += 1;

        Ok(())
    }

    /// Read data within a transaction (MVCC-aware)
    pub async fn read_data(&self, txn_id: TransactionId, key: &[u8]) -> Result<Option<Vec<u8>>, TransactionError> {
        let transactions = self.active_transactions.read();
        let transaction = transactions.get(&txn_id)
            .ok_or(TransactionError::TransactionNotFound(txn_id))?;

        if transaction.status != TransactionStatus::Active {
            return Err(TransactionError::InvalidTransactionState(transaction.status.clone()));
        }

        // Use MVCC to read the appropriate version
        let mvcc = self.mvcc_manager.read();
        let snapshot = transaction.snapshot.as_ref();
        mvcc.read_version(key, snapshot, txn_id).await
    }

    /// Write data within a transaction
    pub async fn write_data(&mut self, txn_id: TransactionId, key: Vec<u8>, value: Vec<u8>) -> Result<(), TransactionError> {
        let mut transactions = self.active_transactions.write();
        let mut mvcc = self.mvcc_manager.write();
        let mut wal = self.wal_manager.write();

        let transaction = transactions.get_mut(&txn_id)
            .ok_or(TransactionError::TransactionNotFound(txn_id))?;

        if transaction.status != TransactionStatus::Active {
            return Err(TransactionError::InvalidTransactionState(transaction.status.clone()));
        }

        // Acquire write lock
        let lock_request = LockRequest {
            key: key.clone(),
            mode: LockMode::Exclusive,
            transaction_id: txn_id,
        };
        self.lock_manager.write().acquire_lock(lock_request.clone()).await?;
        transaction.locks_held.push(lock_request);

        // Log the write operation
        let write_record = LogRecord::Update {
            transaction_id: txn_id,
            key: key.clone(),
            old_value: None, // TODO: Track old value
            new_value: value.clone(),
        };
        wal.append_record(write_record).await?;

        // Create new version in MVCC
        mvcc.create_version(key.clone(), value, txn_id).await?;

        // Track modified keys
        if !transaction.modified_keys.contains(&key) {
            transaction.modified_keys.push(key);
        }

        Ok(())
    }

    /// Get transaction status
    pub fn get_transaction_status(&self, txn_id: TransactionId) -> Option<TransactionStatus> {
        self.active_transactions.read().get(&txn_id).map(|t| t.status.clone())
    }

    /// Get transaction statistics
    pub fn stats(&self) -> &TransactionStats {
        &self.stats
    }

    /// Perform deadlock detection and resolution
    pub async fn detect_deadlocks(&mut self) -> Result<Vec<TransactionId>, TransactionError> {
        let lock_manager = self.lock_manager.read();
        let deadlocked_txns = lock_manager.detect_deadlocks().await?;

        if !deadlocked_txns.is_empty() {
            self.stats.deadlock_count += 1;
            // TODO: Choose victim transaction and abort it
        }

        Ok(deadlocked_txns)
    }
}

/// Transaction operation errors
#[derive(Debug, thiserror::Error)]
pub enum TransactionError {
    #[error("Transaction not found: {0}")]
    TransactionNotFound(TransactionId),

    #[error("Invalid transaction state: {0:?}")]
    InvalidTransactionState(TransactionStatus),

    #[error("Lock acquisition failed: {0}")]
    LockFailed(String),

    #[error("MVCC operation failed: {0}")]
    MVCCError(String),

    #[error("WAL operation failed: {0}")]
    WALError(String),

    #[error("Deadlock detected")]
    Deadlock,

    #[error("Transaction timeout")]
    Timeout,
}
