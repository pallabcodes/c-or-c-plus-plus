//! MVCC Snapshots for Transaction Isolation
//!
//! Snapshots capture the database state at a point in time,
//! enabling repeatable read and serializable isolation levels.

use std::collections::HashSet;
use crate::mvcc::transaction::{TransactionId, TransactionManager};

/// Snapshot of database state at a point in time
#[derive(Debug, Clone)]
pub struct Snapshot {
    /// Transaction ID that created this snapshot
    pub transaction_id: TransactionId,
    /// Timestamp when snapshot was taken
    pub snapshot_timestamp: u64,
    /// Set of active transaction IDs at snapshot time
    pub active_transactions: HashSet<TransactionId>,
    /// Minimum transaction ID that committed after snapshot was taken
    pub xmin: TransactionId,
    /// Maximum transaction ID that committed before snapshot was taken
    pub xmax: TransactionId,
}

impl Snapshot {
    /// Create a new snapshot for a transaction
    pub fn new(transaction_id: TransactionId, txn_manager: &TransactionManager) -> Self {
        let snapshot_timestamp = txn_manager.current_timestamp();

        // Get active transactions at this time
        let active_transactions = txn_manager.active_transactions.read()
            .keys()
            .cloned()
            .collect();

        // Calculate xmin and xmax bounds
        // In a simplified implementation, we use transaction IDs as proxies for timestamps
        let min_active = active_transactions.iter().min().copied().unwrap_or(0);
        let max_active = active_transactions.iter().max().copied().unwrap_or(0);

        Self {
            transaction_id,
            snapshot_timestamp,
            active_transactions,
            xmin: min_active,
            xmax: max_active,
        }
    }

    /// Check if a tuple version is visible in this snapshot
    pub fn is_tuple_visible(&self, xmin: TransactionId, xmax: Option<TransactionId>, txn_manager: &TransactionManager) -> bool {
        // Tuple must be created by a transaction that committed before snapshot
        if !txn_manager.is_transaction_visible(xmin, &crate::mvcc::transaction::Transaction {
            id: self.transaction_id,
            state: crate::mvcc::transaction::TransactionState::Active,
            isolation_level: crate::mvcc::transaction::IsolationLevel::ReadCommitted,
            start_timestamp: self.snapshot_timestamp,
            commit_timestamp: None,
            snapshot: None,
        }) {
            return false;
        }

        // If tuple was deleted, the deleting transaction must not be visible
        if let Some(delete_xid) = xmax {
            if txn_manager.is_transaction_visible(delete_xid, &crate::mvcc::transaction::Transaction {
                id: self.transaction_id,
                state: crate::mvcc::transaction::TransactionState::Active,
                isolation_level: crate::mvcc::transaction::IsolationLevel::ReadCommitted,
                start_timestamp: self.snapshot_timestamp,
                commit_timestamp: None,
                snapshot: None,
            }) {
                return false;
            }
        }

        // Tuple must not be created by a transaction that was active at snapshot time
        if self.active_transactions.contains(&xmin) {
            return false;
        }

        // If deleted by a transaction active at snapshot time, it's not visible
        if let Some(delete_xid) = xmax {
            if self.active_transactions.contains(&delete_xid) {
                return false;
            }
        }

        true
    }

    /// Check if this snapshot can see a particular transaction's changes
    pub fn can_see_transaction(&self, txn_id: TransactionId, txn_manager: &TransactionManager) -> bool {
        // For snapshot isolation, we can see:
        // 1. Transactions that committed before snapshot was taken
        // 2. Our own transaction's changes

        if txn_id == self.transaction_id {
            return true; // Can always see own changes
        }

        if let Some(txn) = txn_manager.get_transaction(txn_id) {
            if let Some(commit_ts) = txn.commit_timestamp {
                return commit_ts < self.snapshot_timestamp;
            }
        }

        false
    }

    /// Export snapshot data for serialization/debugging
    pub fn export(&self) -> SnapshotData {
        SnapshotData {
            transaction_id: self.transaction_id,
            snapshot_timestamp: self.snapshot_timestamp,
            active_transaction_count: self.active_transactions.len(),
            xmin: self.xmin,
            xmax: self.xmax,
        }
    }
}

/// Serializable snapshot data
#[derive(Debug, Clone, serde::Serialize, serde::Deserialize)]
pub struct SnapshotData {
    pub transaction_id: TransactionId,
    pub snapshot_timestamp: u64,
    pub active_transaction_count: usize,
    pub xmin: TransactionId,
    pub xmax: TransactionId,
}

/// Snapshot manager for coordinating snapshots across transactions
pub struct SnapshotManager {
    transaction_manager: std::sync::Arc<TransactionManager>,
}

impl SnapshotManager {
    /// Create a new snapshot manager
    pub fn new(transaction_manager: std::sync::Arc<TransactionManager>) -> Self {
        Self { transaction_manager }
    }

    /// Create a snapshot for a transaction
    pub fn create_snapshot(&self, transaction_id: TransactionId) -> Snapshot {
        Snapshot::new(transaction_id, &self.transaction_manager)
    }

    /// Export all active snapshots (for monitoring/debugging)
    pub fn export_active_snapshots(&self) -> Vec<SnapshotData> {
        // In a real implementation, we'd track active snapshots
        // For now, return empty list
        vec![]
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::mvcc::transaction::{TransactionManager, IsolationLevel};

    #[test]
    fn test_snapshot_creation() {
        let tm = Arc::new(TransactionManager::new());

        // Create a transaction
        let txn = tm.begin_transaction(IsolationLevel::RepeatableRead).unwrap();

        // Create snapshot
        let sm = SnapshotManager::new(tm.clone());
        let snapshot = sm.create_snapshot(txn.id);

        assert_eq!(snapshot.transaction_id, txn.id);
        assert!(!snapshot.active_transactions.is_empty());
    }

    #[test]
    fn test_snapshot_visibility() {
        let tm = Arc::new(TransactionManager::new());

        // Create transactions
        let txn1 = tm.begin_transaction(IsolationLevel::ReadCommitted).unwrap();
        let txn2 = tm.begin_transaction(IsolationLevel::ReadCommitted).unwrap();

        // Create snapshot for txn2
        let sm = SnapshotManager::new(tm.clone());
        let snapshot = sm.create_snapshot(txn2.id);

        // txn1 should not be visible in snapshot (not committed)
        assert!(!snapshot.can_see_transaction(txn1.id, &tm));

        // Commit txn1
        tm.commit_transaction(txn1.id).unwrap();

        // Create new snapshot
        let snapshot2 = sm.create_snapshot(txn2.id);

        // Now txn1 should be visible
        assert!(snapshot2.can_see_transaction(txn1.id, &tm));
    }
}
