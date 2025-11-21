//! Multi-Version Concurrency Control (MVCC)
//!
//! Snapshot isolation without the vacuum operations that plague PostgreSQL.
//! Maintains multiple versions of data for concurrent access.

use crate::core::*;
use super::manager::*;
use std::collections::{HashMap, BTreeMap};
use std::sync::Arc;
use parking_lot::RwLock;

/// MVCC manager for version control and snapshot isolation
pub struct MVCCManager {
    /// Version chains for each key
    version_chains: HashMap<Vec<u8>, VersionChain>,
    /// Active snapshots
    snapshots: HashMap<TransactionId, Snapshot>,
    /// Global transaction counter
    transaction_counter: u64,
}

/// Version chain for a single key
#[derive(Debug, Clone)]
pub struct VersionChain {
    /// Versions ordered by transaction ID (descending - newest first)
    versions: BTreeMap<TransactionId, Version>,
}

/// Data version with metadata
#[derive(Debug, Clone)]
pub struct Version {
    pub value: Vec<u8>,
    pub created_by: TransactionId,
    pub committed_at: Option<u64>, // None if uncommitted
    pub deleted: bool, // Tombstone marker
}

/// Transaction snapshot for consistent reads
#[derive(Debug, Clone)]
pub struct Snapshot {
    pub transaction_id: TransactionId,
    pub start_time: u64,
    pub active_transactions: Vec<TransactionId>,
}

impl MVCCManager {
    /// Create a new MVCC manager
    pub fn new() -> Self {
        Self {
            version_chains: HashMap::new(),
            snapshots: HashMap::new(),
            transaction_counter: 0,
        }
    }

    /// Create a new snapshot for a transaction
    pub async fn create_snapshot(&self) -> Result<Snapshot, TransactionError> {
        let transaction_id = TransactionId(self.transaction_counter + 1);
        let start_time = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_millis() as u64;

        // Get list of active transactions (simplified)
        let active_transactions = self.snapshots.keys().cloned().collect();

        let snapshot = Snapshot {
            transaction_id,
            start_time,
            active_transactions,
        };

        Ok(snapshot)
    }

    /// Read the appropriate version for a transaction
    pub async fn read_version(&self, key: &[u8], snapshot: Option<&Snapshot>, reader_txn: TransactionId) -> Result<Option<Vec<u8>>, TransactionError> {
        let chain = match self.version_chains.get(key) {
            Some(chain) => chain,
            None => return Ok(None),
        };

        // Find the visible version for this transaction
        for (txn_id, version) in chain.versions.iter().rev() {
            // Skip versions from transactions that started after our snapshot
            if let Some(snapshot) = snapshot {
                if txn_id.0 > snapshot.transaction_id.0 {
                    continue;
                }
                // Skip uncommitted transactions that are not our own
                if version.committed_at.is_none() && *txn_id != reader_txn {
                    continue;
                }
            }

            // If this is a delete marker, return None
            if version.deleted {
                return Ok(None);
            }

            // Return the value
            return Ok(Some(version.value.clone()));
        }

        Ok(None)
    }

    /// Create a new version for a key
    pub async fn create_version(&mut self, key: Vec<u8>, value: Vec<u8>, txn_id: TransactionId) -> Result<(), TransactionError> {
        let version = Version {
            value,
            created_by: txn_id,
            committed_at: None, // Will be set on commit
            deleted: false,
        };

        let chain = self.version_chains.entry(key).or_insert_with(|| VersionChain {
            versions: BTreeMap::new(),
        });

        chain.versions.insert(txn_id, version);
        Ok(())
    }

    /// Delete a version (create tombstone)
    pub async fn delete_version(&mut self, key: Vec<u8>, txn_id: TransactionId) -> Result<(), TransactionError> {
        let version = Version {
            value: Vec::new(), // Empty value for deletes
            created_by: txn_id,
            committed_at: None,
            deleted: true,
        };

        let chain = self.version_chains.entry(key).or_insert_with(|| VersionChain {
            versions: BTreeMap::new(),
        });

        chain.versions.insert(txn_id, version);
        Ok(())
    }

    /// Commit a transaction - make its versions visible
    pub async fn commit_transaction(&mut self, txn_id: TransactionId, _snapshot: Option<&Snapshot>) -> Result<(), TransactionError> {
        let commit_time = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_millis() as u64;

        // Mark all versions from this transaction as committed
        for chain in self.version_chains.values_mut() {
            if let Some(version) = chain.versions.get_mut(&txn_id) {
                version.committed_at = Some(commit_time);
            }
        }

        // Clean up snapshot
        self.snapshots.remove(&txn_id);

        Ok(())
    }

    /// Rollback a transaction - remove its versions
    pub async fn rollback_transaction(&mut self, txn_id: TransactionId) -> Result<(), TransactionError> {
        // Remove all versions created by this transaction
        for chain in self.version_chains.values_mut() {
            chain.versions.remove(&txn_id);
        }

        // Clean up empty chains
        self.version_chains.retain(|_, chain| !chain.versions.is_empty());

        // Clean up snapshot
        self.snapshots.remove(&txn_id);

        Ok(())
    }

    /// Garbage collect old versions that are no longer visible
    pub async fn garbage_collect(&mut self, min_transaction_id: TransactionId) -> Result<usize, TransactionError> {
        let mut collected_count = 0;

        for chain in self.version_chains.values_mut() {
            // Remove versions from transactions older than min_transaction_id
            // that are committed and not the most recent version
            let versions_to_remove: Vec<TransactionId> = chain.versions
                .iter()
                .filter(|(txn_id, version)| {
                    txn_id.0 < min_transaction_id.0 &&
                    version.committed_at.is_some() &&
                    **txn_id != *chain.versions.keys().next_back().unwrap()
                })
                .map(|(txn_id, _)| *txn_id)
                .collect();

            for txn_id in versions_to_remove {
                chain.versions.remove(&txn_id);
                collected_count += 1;
            }
        }

        // Remove empty chains
        self.version_chains.retain(|_, chain| !chain.versions.is_empty());

        Ok(collected_count)
    }

    /// Get statistics about version chains
    pub fn get_statistics(&self) -> MVCCStatistics {
        let mut total_versions = 0;
        let mut max_chain_length = 0;
        let mut avg_chain_length = 0.0;

        for chain in self.version_chains.values() {
            let chain_len = chain.versions.len();
            total_versions += chain_len;
            max_chain_length = max_chain_length.max(chain_len);
        }

        if !self.version_chains.is_empty() {
            avg_chain_length = total_versions as f64 / self.version_chains.len() as f64;
        }

        MVCCStatistics {
            total_keys: self.version_chains.len(),
            total_versions,
            max_chain_length,
            avg_chain_length,
            active_snapshots: self.snapshots.len(),
        }
    }
}

/// MVCC statistics
#[derive(Debug, Clone)]
pub struct MVCCStatistics {
    pub total_keys: usize,
    pub total_versions: usize,
    pub max_chain_length: usize,
    pub avg_chain_length: f64,
    pub active_snapshots: usize,
}
