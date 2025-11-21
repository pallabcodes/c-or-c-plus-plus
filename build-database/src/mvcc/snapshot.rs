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
    /// Isolation level for this snapshot
    pub isolation_level: crate::mvcc::transaction::IsolationLevel,
    /// Set of active transaction IDs at snapshot time
    pub active_transactions: HashSet<TransactionId>,
    /// Minimum transaction ID that committed after snapshot was taken
    pub xmin: TransactionId,
    /// Maximum transaction ID that committed before snapshot was taken
    pub xmax: TransactionId,
    /// Read set for Repeatable Read isolation (transactions read by this transaction)
    pub read_set: HashSet<TransactionId>,
    /// Write set for conflict detection in Serializable isolation
    pub write_set: HashSet<String>,
}

impl Snapshot {
    /// Create a new snapshot for a transaction
    pub fn new(transaction_id: TransactionId, isolation_level: crate::mvcc::transaction::IsolationLevel, txn_manager: &TransactionManager) -> Self {
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
            isolation_level,
            active_transactions,
            xmin: min_active,
            xmax: max_active,
            read_set: HashSet::new(),
            write_set: HashSet::new(),
        }
    }

    /// Check if a tuple version is visible in this snapshot
    pub fn is_tuple_visible(&self, xmin: TransactionId, xmax: Option<TransactionId>, txn_manager: &TransactionManager) -> bool {
        match self.isolation_level {
            crate::mvcc::transaction::IsolationLevel::ReadUncommitted => {
                self.is_tuple_visible_read_uncommitted(xmin, xmax, txn_manager)
            }
            crate::mvcc::transaction::IsolationLevel::ReadCommitted => {
                self.is_tuple_visible_read_committed(xmin, xmax, txn_manager)
            }
            crate::mvcc::transaction::IsolationLevel::RepeatableRead => {
                self.is_tuple_visible_repeatable_read(xmin, xmax, txn_manager)
            }
            crate::mvcc::transaction::IsolationLevel::Serializable => {
                self.is_tuple_visible_serializable(xmin, xmax, txn_manager)
            }
        }
    }

    /// Read Uncommitted visibility (sees uncommitted changes)
    fn is_tuple_visible_read_uncommitted(&self, xmin: TransactionId, xmax: Option<TransactionId>, _txn_manager: &TransactionManager) -> bool {
        // Can see all changes, even uncommitted ones
        // Only filter out our own aborted changes
        if xmin == self.transaction_id {
            return true;
        }

        // Don't show tuples deleted by our own transaction
        if let Some(delete_xid) = xmax {
            if delete_xid == self.transaction_id {
                return false;
            }
        }

        true
    }

    /// Read Committed visibility (sees only committed changes)
    fn is_tuple_visible_read_committed(&self, xmin: TransactionId, xmax: Option<TransactionId>, txn_manager: &TransactionManager) -> bool {
        // For Read Committed, we check visibility at the time of each individual read
        // Simplified: use snapshot timestamp as the read timestamp

        // Tuple must be created by a committed transaction visible at read time
        if let Some(txn) = txn_manager.get_transaction(xmin) {
            match txn.state {
                crate::mvcc::transaction::TransactionState::Committed => {
                    if let Some(commit_ts) = txn.commit_timestamp {
                        if commit_ts > self.snapshot_timestamp {
                            return false; // Committed after our snapshot
                        }
                    }
                }
                crate::mvcc::transaction::TransactionState::Active | crate::mvcc::transaction::TransactionState::InProgress => {
                    return false; // Not committed yet
                }
                crate::mvcc::transaction::TransactionState::Aborted => {
                    return false; // Aborted transaction
                }
            }
        }

        // If tuple was deleted, check if deleting transaction is committed and visible
        if let Some(delete_xid) = xmax {
            if let Some(delete_txn) = txn_manager.get_transaction(delete_xid) {
                match delete_txn.state {
                    crate::mvcc::transaction::TransactionState::Committed => {
                        if let Some(commit_ts) = delete_txn.commit_timestamp {
                            if commit_ts <= self.snapshot_timestamp {
                                return false; // Deleted by committed transaction visible at read time
                            }
                        }
                    }
                    crate::mvcc::transaction::TransactionState::Active | crate::mvcc::transaction::TransactionState::InProgress => {
                        // If the deleting transaction is still active, the tuple is not visible
                        // (though in practice we'd wait or handle this differently)
                        return false;
                    }
                    crate::mvcc::transaction::TransactionState::Aborted => {
                        // Deleting transaction aborted, so tuple is visible
                    }
                }
            }
        }

        true
    }

    /// Repeatable Read visibility (consistent snapshot)
    fn is_tuple_visible_repeatable_read(&self, xmin: TransactionId, xmax: Option<TransactionId>, txn_manager: &TransactionManager) -> bool {
        // For Repeatable Read, we maintain a consistent snapshot
        // We cannot see changes from transactions that committed after our snapshot was taken

        // Check if the creating transaction committed before our snapshot
        if let Some(txn) = txn_manager.get_transaction(xmin) {
            match txn.state {
                crate::mvcc::transaction::TransactionState::Committed => {
                    if let Some(commit_ts) = txn.commit_timestamp {
                        if commit_ts >= self.snapshot_timestamp {
                            return false; // Committed after our snapshot
                        }
                    }
                }
                crate::mvcc::transaction::TransactionState::Active | crate::mvcc::transaction::TransactionState::InProgress => {
                    return false; // Not committed yet
                }
                crate::mvcc::transaction::TransactionState::Aborted => {
                    return false; // Aborted transaction
                }
            }
        }

        // If tuple was deleted, check if deleting transaction committed before our snapshot
        if let Some(delete_xid) = xmax {
            if let Some(delete_txn) = txn_manager.get_transaction(delete_xid) {
                match delete_txn.state {
                    crate::mvcc::transaction::TransactionState::Committed => {
                        if let Some(commit_ts) = delete_txn.commit_timestamp {
                            if commit_ts < self.snapshot_timestamp {
                                return false; // Deleted by transaction that committed before our snapshot
                            }
                        }
                        // If delete committed after our snapshot, tuple is still visible
                    }
                    crate::mvcc::transaction::TransactionState::Active | crate::mvcc::transaction::TransactionState::InProgress => {
                        // Deleting transaction is still active, tuple is visible
                    }
                    crate::mvcc::transaction::TransactionState::Aborted => {
                        // Deleting transaction aborted, tuple is visible
                    }
                }
            }
        }

        true
    }

    /// Serializable visibility (Repeatable Read + conflict detection)
    fn is_tuple_visible_serializable(&self, xmin: TransactionId, xmax: Option<TransactionId>, txn_manager: &TransactionManager) -> bool {
        // Serializable uses the same visibility rules as Repeatable Read
        // but adds additional conflict detection during commit
        self.is_tuple_visible_repeatable_read(xmin, xmax, txn_manager)
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

    /// Record a read operation for conflict detection (Serializable isolation)
    pub fn record_read(&mut self, table_name: &str, row_key: &str) {
        if matches!(self.isolation_level, crate::mvcc::transaction::IsolationLevel::Serializable) {
            self.read_set.insert(format!("{}:{}", table_name, row_key).parse().unwrap_or(0));
        }
    }

    /// Record a write operation for conflict detection (Serializable isolation)
    pub fn record_write(&mut self, table_name: &str, row_key: &str) {
        if matches!(self.isolation_level, crate::mvcc::transaction::IsolationLevel::Serializable) {
            self.write_set.insert(format!("{}:{}", table_name, row_key));
        }
    }

    /// Check for serialization conflicts with another transaction
    pub fn has_serialization_conflict(&self, other_snapshot: &Snapshot) -> bool {
        if !matches!(self.isolation_level, crate::mvcc::transaction::IsolationLevel::Serializable) ||
           !matches!(other_snapshot.isolation_level, crate::mvcc::transaction::IsolationLevel::Serializable) {
            return false;
        }

        // Check for write-write conflicts (both transactions wrote to the same data)
        for write_item in &self.write_set {
            if other_snapshot.write_set.contains(write_item) {
                return true;
            }
        }

        // Check for write-read conflicts (one wrote what the other read)
        for write_item in &self.write_set {
            if other_snapshot.read_set.contains(&write_item.parse().unwrap_or(0)) {
                return true;
            }
        }

        for write_item in &other_snapshot.write_set {
            if self.read_set.contains(&write_item.parse().unwrap_or(0)) {
                return true;
            }
        }

        false
    }

    /// Check if transaction can commit under Serializable isolation
    pub fn can_commit_serializable(&self, active_snapshots: &[Snapshot]) -> bool {
        if !matches!(self.isolation_level, crate::mvcc::transaction::IsolationLevel::Serializable) {
            return true; // Non-serializable transactions always can commit
        }

        // Check for conflicts with all active transactions
        for snapshot in active_snapshots {
            if snapshot.transaction_id != self.transaction_id && self.has_serialization_conflict(snapshot) {
                return false;
            }
        }

        true
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
    pub fn create_snapshot(&self, transaction_id: TransactionId, isolation_level: crate::mvcc::transaction::IsolationLevel) -> Snapshot {
        Snapshot::new(transaction_id, isolation_level, &self.transaction_manager)
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
