//! Versioned Tuples for MVCC
//!
//! Each data row maintains multiple versions identified by transaction IDs
//! that created (xmin) and deleted (xmax) the version.

use std::collections::HashMap;
use serde::{Serialize, Deserialize};
use crate::types::DataValue;
use crate::mvcc::transaction::TransactionId;

/// Versioned tuple (row) with MVCC metadata
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct VersionedTuple {
    /// Primary key of this tuple
    pub primary_key: DataValue,
    /// Transaction ID that created this version
    pub xmin: TransactionId,
    /// Transaction ID that deleted this version (None if still visible)
    pub xmax: Option<TransactionId>,
    /// The actual data for this version
    pub data: HashMap<String, DataValue>,
}

impl VersionedTuple {
    /// Create a new versioned tuple
    pub fn new(primary_key: DataValue, data: HashMap<String, DataValue>, creating_txn: TransactionId) -> Self {
        Self {
            primary_key,
            xmin: creating_txn,
            xmax: None,
            data,
        }
    }

    /// Create a new version from an existing tuple (for updates)
    pub fn new_version(&self, new_data: HashMap<String, DataValue>, creating_txn: TransactionId) -> Self {
        Self {
            primary_key: self.primary_key.clone(),
            xmin: creating_txn,
            xmax: None,
            data: new_data,
        }
    }

    /// Mark this version as deleted by a transaction
    pub fn mark_deleted(&mut self, deleting_txn: TransactionId) {
        self.xmax = Some(deleting_txn);
    }

    /// Check if this version is currently visible to a transaction
    pub fn is_visible_to(&self, txn: &crate::mvcc::transaction::Transaction, txn_manager: &crate::mvcc::transaction::TransactionManager) -> bool {
        // Version must be created by a committed transaction
        if !txn_manager.is_transaction_visible(self.xmin, txn) {
            return false;
        }

        // If version was deleted, the deleting transaction must not be visible
        if let Some(xmax) = self.xmax {
            if txn_manager.is_transaction_visible(xmax, txn) {
                return false;
            }
        }

        true
    }

    /// Check if this version was created by a specific transaction
    pub fn created_by(&self, txn_id: TransactionId) -> bool {
        self.xmin == txn_id
    }

    /// Check if this version is the current (latest) version
    pub fn is_current(&self) -> bool {
        self.xmax.is_none()
    }

    /// Get the data for this version
    pub fn get_data(&self) -> &HashMap<String, DataValue> {
        &self.data
    }
}

/// Version chain for a single logical tuple
#[derive(Debug, Clone)]
pub struct TupleVersionChain {
    /// All versions of this tuple, ordered by xmin (creation time)
    pub versions: Vec<VersionedTuple>,
}

impl TupleVersionChain {
    /// Create a new version chain with the first version
    pub fn new(first_version: VersionedTuple) -> Self {
        Self {
            versions: vec![first_version],
        }
    }

    /// Add a new version to the chain
    pub fn add_version(&mut self, version: VersionedTuple) {
        // Mark the previous current version as replaced (deleted)
        if let Some(last) = self.versions.last_mut() {
            last.xmax = Some(version.xmin);
        }
        self.versions.push(version);
    }

    /// Delete the current version
    pub fn delete_current(&mut self, deleting_txn: TransactionId) {
        if let Some(current) = self.versions.last_mut() {
            current.mark_deleted(deleting_txn);
        }
    }

    /// Get the current (latest) version
    pub fn current_version(&self) -> Option<&VersionedTuple> {
        self.versions.last()
    }

    /// Find the visible version for a transaction
    pub fn visible_version(&self, txn: &crate::mvcc::transaction::Transaction, txn_manager: &crate::mvcc::transaction::TransactionManager) -> Option<&VersionedTuple> {
        // Find the most recent version visible to this transaction
        for version in self.versions.iter().rev() {
            if version.is_visible_to(txn, txn_manager) {
                return Some(version);
            }
        }
        None
    }

    /// Check if this tuple is visible to a transaction
    pub fn is_visible_to(&self, txn: &crate::mvcc::transaction::Transaction, txn_manager: &crate::mvcc::transaction::TransactionManager) -> bool {
        self.visible_version(txn, txn_manager).is_some()
    }

    /// Get all versions (for debugging/admin)
    pub fn all_versions(&self) -> &[VersionedTuple] {
        &self.versions
    }

    /// Clean up old versions that are no longer visible to any active transaction
    pub fn cleanup_old_versions(&mut self, active_txn_ids: &[TransactionId]) -> usize {
        let mut removed = 0;
        self.versions.retain(|version| {
            // Keep versions that are still potentially visible
            if version.is_current() {
                return true; // Current version is always needed
            }

            // Keep versions where the creating transaction is still active
            if active_txn_ids.contains(&version.xmin) {
                return true;
            }

            // Keep versions where the deleting transaction is still active
            if let Some(xmax) = version.xmax {
                if active_txn_ids.contains(&xmax) {
                    return true;
                }
            }

            // Version can be removed
            removed += 1;
            false
        });
        removed
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::mvcc::transaction::{TransactionManager, IsolationLevel};

    #[test]
    fn test_versioned_tuple_visibility() {
        let tm = TransactionManager::new();

        // Create transactions
        let txn1 = tm.begin_transaction(IsolationLevel::ReadCommitted).unwrap();
        let txn2 = tm.begin_transaction(IsolationLevel::ReadCommitted).unwrap();

        // Create a tuple
        let mut data = HashMap::new();
        data.insert("name".to_string(), DataValue::Text("Alice".to_string()));
        let tuple = VersionedTuple::new(DataValue::Integer(1), data, txn1.id);

        // Should be visible to txn1 (its creator)
        assert!(tuple.is_visible_to(&txn1, &tm));

        // Should not be visible to txn2 (read committed - not committed yet)
        assert!(!tuple.is_visible_to(&txn2, &tm));

        // Commit txn1
        tm.commit_transaction(txn1.id).unwrap();

        // Now should be visible to txn2
        assert!(tuple.is_visible_to(&txn2, &tm));
    }

    #[test]
    fn test_version_chain() {
        let tm = TransactionManager::new();

        let txn1 = tm.begin_transaction(IsolationLevel::ReadCommitted).unwrap();

        // Create initial version
        let mut data1 = HashMap::new();
        data1.insert("name".to_string(), DataValue::Text("Alice".to_string()));
        let initial_version = VersionedTuple::new(DataValue::Integer(1), data1, txn1.id);

        let mut chain = TupleVersionChain::new(initial_version);

        // Should have current version
        assert!(chain.current_version().is_some());
        assert_eq!(chain.versions.len(), 1);

        // Update with new version
        tm.commit_transaction(txn1.id).unwrap();
        let txn2 = tm.begin_transaction(IsolationLevel::ReadCommitted).unwrap();

        let mut data2 = HashMap::new();
        data2.insert("name".to_string(), DataValue::Text("Alice Updated".to_string()));
        let new_version = chain.current_version().unwrap().new_version(data2, txn2.id);
        chain.add_version(new_version);

        // Should have two versions now
        assert_eq!(chain.versions.len(), 2);
        assert!(chain.current_version().unwrap().created_by(txn2.id));
    }
}
