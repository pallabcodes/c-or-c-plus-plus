//! MVCC Visibility Rules
//!
//! Implements the rules for determining which tuple versions are visible
//! to transactions based on isolation levels and MVCC metadata.

use crate::mvcc::transaction::{Transaction, TransactionManager, IsolationLevel};
use crate::mvcc::version::VersionedTuple;
use crate::mvcc::snapshot::Snapshot;

/// Visibility checker for MVCC
pub struct VisibilityChecker;

impl VisibilityChecker {
    /// Check if a tuple version is visible to a transaction
    pub fn is_tuple_visible(tuple: &VersionedTuple, transaction: &Transaction, txn_manager: &TransactionManager) -> bool {
        match transaction.isolation_level {
            IsolationLevel::ReadUncommitted => {
                Self::is_visible_read_uncommitted(tuple, transaction, txn_manager)
            }
            IsolationLevel::ReadCommitted => {
                Self::is_visible_read_committed(tuple, transaction, txn_manager)
            }
            IsolationLevel::RepeatableRead => {
                Self::is_visible_repeatable_read(tuple, transaction, txn_manager)
            }
            IsolationLevel::Serializable => {
                Self::is_visible_serializable(tuple, transaction, txn_manager)
            }
        }
    }

    /// Read Uncommitted: Can see all versions, even uncommitted ones
    fn is_visible_read_uncommitted(tuple: &VersionedTuple, _transaction: &Transaction, _txn_manager: &TransactionManager) -> bool {
        // Can see everything, including uncommitted changes
        // Note: This can lead to dirty reads
        true
    }

    /// Read Committed: Can only see committed versions
    fn is_visible_read_committed(tuple: &VersionedTuple, transaction: &Transaction, txn_manager: &TransactionManager) -> bool {
        // Tuple must be created by a committed transaction
        if !txn_manager.is_transaction_visible(tuple.xmin, transaction) {
            return false;
        }

        // If tuple was deleted, the deleting transaction must not be visible
        if let Some(xmax) = tuple.xmax {
            if txn_manager.is_transaction_visible(xmax, transaction) {
                return false;
            }
        }

        true
    }

    /// Repeatable Read: Can see committed versions as of snapshot time
    fn is_visible_repeatable_read(tuple: &VersionedTuple, transaction: &Transaction, txn_manager: &TransactionManager) -> bool {
        if let Some(snapshot) = &transaction.snapshot {
            return snapshot.is_tuple_visible(tuple.xmin, tuple.xmax, txn_manager);
        }

        // Fallback to read committed if no snapshot
        Self::is_visible_read_committed(tuple, transaction, txn_manager)
    }

    /// Serializable: Strictest isolation, prevents all anomalies
    fn is_visible_serializable(tuple: &VersionedTuple, transaction: &Transaction, txn_manager: &TransactionManager) -> bool {
        // For serializable, we use the same rules as repeatable read
        // but with additional conflict detection (simplified here)
        Self::is_visible_repeatable_read(tuple, transaction, txn_manager)
    }

    /// Check if a transaction can modify a tuple (for write operations)
    pub fn can_modify_tuple(tuple: &VersionedTuple, transaction: &Transaction, txn_manager: &TransactionManager) -> Result<(), VisibilityError> {
        // Check if tuple is currently visible to this transaction
        if !Self::is_tuple_visible(tuple, transaction, txn_manager) {
            return Err(VisibilityError::TupleNotVisible);
        }

        // Check if tuple is already being modified by another active transaction
        // This is a simplified check - in reality, we'd check for conflicting locks
        if let Some(xmax) = tuple.xmax {
            if let Some(conflicting_txn) = txn_manager.get_transaction(xmax) {
                if conflicting_txn.is_active() {
                    return Err(VisibilityError::TupleLocked);
                }
            }
        }

        Ok(())
    }

    /// Get the appropriate snapshot for a transaction
    pub fn create_snapshot_for_transaction(transaction: &mut Transaction, txn_manager: &TransactionManager) {
        match transaction.isolation_level {
            IsolationLevel::ReadUncommitted => {
                // No snapshot needed
            }
            IsolationLevel::ReadCommitted => {
                // No snapshot needed - each read gets current committed state
            }
            IsolationLevel::RepeatableRead | IsolationLevel::Serializable => {
                // Create snapshot for repeatable read
                use crate::mvcc::snapshot::SnapshotManager;
                let snapshot_manager = SnapshotManager::new(std::sync::Arc::new(txn_manager.clone()));
                let snapshot = snapshot_manager.create_snapshot(transaction.id);
                transaction.snapshot = Some(snapshot);
            }
        }
    }

    /// Check for potential conflicts in serializable isolation
    pub fn check_serializable_conflicts(_transaction: &Transaction, _other_transactions: &[&Transaction]) -> Vec<Conflict> {
        // Simplified conflict detection
        // In a real implementation, this would track read/write sets
        // and detect cycles in the conflict graph
        vec![]
    }
}

/// Visibility errors
#[derive(Debug, Clone)]
pub enum VisibilityError {
    TupleNotVisible,
    TupleLocked,
    SerializationFailure,
}

/// Conflict detected in serializable isolation
#[derive(Debug, Clone)]
pub struct Conflict {
    pub transaction_id: u64,
    pub conflicting_transaction_id: u64,
    pub conflict_type: ConflictType,
}

/// Types of conflicts in serializable isolation
#[derive(Debug, Clone)]
pub enum ConflictType {
    ReadWrite,
    WriteWrite,
    WriteSkew,
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::mvcc::transaction::{TransactionManager, IsolationLevel};
    use std::collections::HashMap;
    use crate::types::DataValue;

    #[test]
    fn test_read_committed_visibility() {
        let tm = TransactionManager::new();

        // Create transactions
        let txn1 = tm.begin_transaction(IsolationLevel::ReadCommitted).unwrap();
        let mut data = HashMap::new();
        data.insert("value".to_string(), DataValue::Integer(42));

        let tuple = VersionedTuple::new(DataValue::Integer(1), data, txn1.id);

        // Tuple should not be visible to other transactions (not committed)
        let txn2 = tm.begin_transaction(IsolationLevel::ReadCommitted).unwrap();
        assert!(!VisibilityChecker::is_tuple_visible(&tuple, &txn2, &tm));

        // Commit txn1
        tm.commit_transaction(txn1.id).unwrap();

        // Now tuple should be visible
        assert!(VisibilityChecker::is_tuple_visible(&tuple, &txn2, &tm));
    }

    #[test]
    fn test_repeatable_read_consistency() {
        let tm = TransactionManager::new();

        // Create transaction with repeatable read
        let mut txn = tm.begin_transaction(IsolationLevel::RepeatableRead).unwrap();

        // Create snapshot
        VisibilityChecker::create_snapshot_for_transaction(&mut txn, &tm);
        assert!(txn.snapshot.is_some());

        // Create and commit another transaction
        let txn2 = tm.begin_transaction(IsolationLevel::ReadCommitted).unwrap();
        tm.commit_transaction(txn2.id).unwrap();

        // The committed transaction should not be visible in repeatable read
        // (since it committed after the snapshot was taken)
        let snapshot = txn.snapshot.as_ref().unwrap();
        assert!(!snapshot.can_see_transaction(txn2.id, &tm));
    }
}
