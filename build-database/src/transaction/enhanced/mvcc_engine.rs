//! MVCC Engine: Multi-Version Concurrency Control
//!
//! UNIQUENESS: Research-backed MVCC implementation combining:
//! - PostgreSQL-style MVCC for high concurrency
//! - Version chain management for efficient storage
//! - Garbage collection for old versions
//! - Snapshot isolation with minimal overhead

use std::collections::{HashMap, VecDeque};
use std::sync::Arc;
use parking_lot::RwLock;
use crate::core::errors::{AuroraResult, AuroraError};
use super::unified_transaction_manager::{TransactionId, IsolationLevel};

/// MVCC version record
#[derive(Debug, Clone)]
pub struct VersionRecord {
    pub transaction_id: TransactionId,
    pub value: String,
    pub created_at: std::time::Instant,
    pub xmin: TransactionId, // Transaction that created this version
    pub xmax: Option<TransactionId>, // Transaction that deleted this version (if any)
}

/// Version chain for a data item
#[derive(Debug)]
pub struct VersionChain {
    pub key: String,
    pub versions: VecDeque<VersionRecord>,
    pub latest_version: usize, // Index of latest visible version
}

/// Transaction snapshot for MVCC
#[derive(Debug, Clone)]
pub struct TransactionSnapshot {
    pub transaction_id: TransactionId,
    pub active_transactions: Vec<TransactionId>, // Transactions active when snapshot was taken
    pub committed_transactions: Vec<TransactionId>, // Committed transactions visible to this snapshot
}

/// MVCC engine for high-concurrency transaction isolation
///
/// Implements PostgreSQL-style MVCC with efficient version management
/// and garbage collection for optimal performance.
pub struct MVCCManager {
    /// Version chains for all data items
    version_chains: RwLock<HashMap<String, VersionChain>>,

    /// Active transaction snapshots
    snapshots: RwLock<HashMap<TransactionId, TransactionSnapshot>>,

    /// Committed transaction IDs (for visibility checks)
    committed_transactions: RwLock<Vec<TransactionId>>,

    /// Statistics
    stats: Arc<Mutex<MVCCStats>>,
}

/// MVCC statistics
#[derive(Debug, Clone)]
pub struct MVCCStats {
    pub total_versions: u64,
    pub active_versions: u64,
    pub garbage_collected_versions: u64,
    pub average_chain_length: f64,
    pub snapshot_count: u64,
    pub visibility_check_count: u64,
    pub version_creation_count: u64,
}

impl Default for MVCCStats {
    fn default() -> Self {
        Self {
            total_versions: 0,
            active_versions: 0,
            garbage_collected_versions: 0,
            average_chain_length: 0.0,
            snapshot_count: 0,
            visibility_check_count: 0,
            version_creation_count: 0,
        }
    }
}

impl MVCCManager {
    /// Create a new MVCC manager
    pub fn new() -> Self {
        Self {
            version_chains: RwLock::new(HashMap::new()),
            snapshots: RwLock::new(HashMap::new()),
            committed_transactions: RwLock::new(Vec::new()),
            stats: Arc::new(Mutex::new(MVCCStats::default())),
        }
    }

    /// Begin a transaction in MVCC
    pub async fn begin_transaction(&self, txn_id: TransactionId) -> AuroraResult<()> {
        // Create snapshot for this transaction
        let snapshot = self.create_snapshot(txn_id).await;

        {
            let mut snapshots = self.snapshots.write().unwrap();
            snapshots.insert(txn_id, snapshot);
        }

        let mut stats = self.stats.lock().unwrap();
        stats.snapshot_count += 1;

        Ok(())
    }

    /// Read data using MVCC visibility rules
    pub async fn read_data(&self, txn_id: TransactionId, key: &str) -> AuroraResult<Option<String>> {
        let snapshot = {
            let snapshots = self.snapshots.read().unwrap();
            snapshots.get(&txn_id).cloned()
                .ok_or_else(|| AuroraError::NotFound(format!("Snapshot not found for transaction {}", txn_id.0)))?
        };

        let version_chains = self.version_chains.read().unwrap();

        if let Some(chain) = version_chains.get(key) {
            // Find the visible version for this transaction's snapshot
            for version in chain.versions.iter().rev() {
                let mut stats = self.stats.lock().unwrap();
                stats.visibility_check_count += 1;

                if self.is_version_visible(&snapshot, version).await? {
                    return Ok(Some(version.value.clone()));
                }
            }
        }

        Ok(None)
    }

    /// Write data using MVCC (creates new version)
    pub async fn write_data(&self, txn_id: TransactionId, key: &str, value: &str) -> AuroraResult<()> {
        let version = VersionRecord {
            transaction_id: txn_id,
            value: value.to_string(),
            created_at: std::time::Instant::now(),
            xmin: txn_id,
            xmax: None,
        };

        {
            let mut version_chains = self.version_chains.write().unwrap();

            let chain = version_chains.entry(key.to_string())
                .or_insert_with(|| VersionChain {
                    key: key.to_string(),
                    versions: VecDeque::new(),
                    latest_version: 0,
                });

            chain.versions.push_back(version);
            chain.latest_version = chain.versions.len() - 1;
        }

        let mut stats = self.stats.lock().unwrap();
        stats.total_versions += 1;
        stats.active_versions += 1;
        stats.version_creation_count += 1;

        // Update average chain length
        let version_chains = self.version_chains.read().unwrap();
        let total_chains = version_chains.len() as f64;
        let total_versions = stats.total_versions as f64;
        stats.average_chain_length = total_versions / total_chains.max(1.0);

        Ok(())
    }

    /// Commit a transaction in MVCC
    pub async fn commit_transaction(&self, txn_id: TransactionId) -> AuroraResult<()> {
        // Add transaction to committed list
        {
            let mut committed = self.committed_transactions.write().unwrap();
            committed.push(txn_id);
        }

        // Versions created by this transaction are now visible to future transactions
        Ok(())
    }

    /// Abort a transaction in MVCC
    pub async fn abort_transaction(&self, txn_id: TransactionId) -> AuroraResult<()> {
        // Mark all versions created by this transaction as aborted
        // (In a real implementation, we'd either delete them or mark them invalid)

        {
            let mut version_chains = self.version_chains.write().unwrap();

            for chain in version_chains.values_mut() {
                chain.versions.retain(|version| version.transaction_id != txn_id);
                chain.latest_version = chain.versions.len().saturating_sub(1);
            }
        }

        // Remove snapshot
        {
            let mut snapshots = self.snapshots.write().unwrap();
            snapshots.remove(&txn_id);
        }

        let mut stats = self.stats.lock().unwrap();
        stats.active_versions = stats.active_versions.saturating_sub(1);

        Ok(())
    }

    /// Check if a version is visible to a transaction snapshot
    async fn is_version_visible(&self, snapshot: &TransactionSnapshot, version: &VersionRecord) -> AuroraResult<bool> {
        let committed = self.committed_transactions.read().unwrap();

        // Version was created by a transaction that committed before our snapshot
        if committed.contains(&version.xmin) && version.xmin.0 < snapshot.transaction_id.0 {
            // And was not deleted, or was deleted by a transaction that started after our snapshot
            if let Some(xmax) = version.xmax {
                // Version was deleted - check if the deleting transaction is visible
                if committed.contains(&xmax) && xmax.0 < snapshot.transaction_id.0 {
                    return Ok(false); // Version was deleted and deletion is visible
                }
            }
            return Ok(true); // Version exists and is visible
        }

        // Version was created by our own transaction
        if version.xmin == snapshot.transaction_id {
            return Ok(true);
        }

        Ok(false)
    }

    /// Create a snapshot for a transaction
    async fn create_snapshot(&self, txn_id: TransactionId) -> TransactionSnapshot {
        let active_transactions = {
            let snapshots = self.snapshots.read().unwrap();
            snapshots.keys().cloned().collect()
        };

        let committed_transactions = {
            let committed = self.committed_transactions.read().unwrap();
            committed.clone()
        };

        TransactionSnapshot {
            transaction_id: txn_id,
            active_transactions,
            committed_transactions,
        }
    }

    /// Perform garbage collection of old versions
    pub async fn perform_garbage_collection(&self) -> AuroraResult<()> {
        let mut collected = 0;

        {
            let mut version_chains = self.version_chains.write().unwrap();
            let snapshots = self.snapshots.read().unwrap();
            let committed = self.committed_transactions.read().unwrap();

            // Find the oldest active transaction
            let oldest_active_txn = snapshots.keys()
                .min_by_key(|id| id.0)
                .cloned();

            if let Some(oldest_txn) = oldest_active_txn {
                for chain in version_chains.values_mut() {
                    // Remove versions that are no longer visible to any active transaction
                    let original_len = chain.versions.len();

                    chain.versions.retain(|version| {
                        // Keep if created by transaction that hasn't committed yet
                        if !committed.contains(&version.xmin) {
                            return true;
                        }

                        // Keep if xmin is newer than oldest active transaction
                        if version.xmin.0 >= oldest_txn.0 {
                            return true;
                        }

                        // Keep if xmax exists and xmax is newer than oldest active transaction
                        if let Some(xmax) = version.xmax {
                            if xmax.0 >= oldest_txn.0 {
                                return true;
                            }
                        }

                        false // Can be garbage collected
                    });

                    collected += original_len - chain.versions.len();
                    chain.latest_version = chain.versions.len().saturating_sub(1);
                }
            }
        }

        let mut stats = self.stats.lock().unwrap();
        stats.garbage_collected_versions += collected as u64;
        stats.active_versions = stats.active_versions.saturating_sub(collected as u64);

        Ok(())
    }

    /// Get MVCC statistics
    pub fn stats(&self) -> MVCCStats {
        self.stats.lock().unwrap().clone()
    }

    /// Get version chain information for debugging
    pub fn get_version_chain(&self, key: &str) -> Option<VersionChain> {
        let version_chains = self.version_chains.read().unwrap();
        version_chains.get(key).cloned()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_mvcc_manager_creation() {
        let manager = MVCCManager::new();
        let stats = manager.stats();
        assert_eq!(stats.total_versions, 0);
    }

    #[test]
    fn test_version_record() {
        let record = VersionRecord {
            transaction_id: TransactionId(123),
            value: "test_value".to_string(),
            created_at: std::time::Instant::now(),
            xmin: TransactionId(123),
            xmax: None,
        };

        assert_eq!(record.transaction_id, TransactionId(123));
        assert_eq!(record.value, "test_value");
        assert!(record.xmax.is_none());
    }

    #[test]
    fn test_version_chain() {
        let mut chain = VersionChain {
            key: "test_key".to_string(),
            versions: VecDeque::new(),
            latest_version: 0,
        };

        assert_eq!(chain.key, "test_key");
        assert_eq!(chain.versions.len(), 0);
    }

    #[test]
    fn test_transaction_snapshot() {
        let snapshot = TransactionSnapshot {
            transaction_id: TransactionId(456),
            active_transactions: vec![TransactionId(123), TransactionId(789)],
            committed_transactions: vec![TransactionId(111), TransactionId(222)],
        };

        assert_eq!(snapshot.transaction_id, TransactionId(456));
        assert_eq!(snapshot.active_transactions.len(), 2);
        assert_eq!(snapshot.committed_transactions.len(), 2);
    }

    #[tokio::test]
    async fn test_mvcc_transaction_workflow() {
        let manager = MVCCManager::new();
        let txn_id = TransactionId(1);

        // Begin transaction
        manager.begin_transaction(txn_id).await.unwrap();

        // Write data
        manager.write_data(txn_id, "key1", "value1").await.unwrap();

        // Read data (should see our own write)
        let value = manager.read_data(txn_id, "key1").await.unwrap();
        assert_eq!(value, Some("value1".to_string()));

        // Commit transaction
        manager.commit_transaction(txn_id).await.unwrap();

        // Verify statistics
        let stats = manager.stats();
        assert_eq!(stats.total_versions, 1);
        assert_eq!(stats.version_creation_count, 1);

        // Verify version chain
        let chain = manager.get_version_chain("key1").unwrap();
        assert_eq!(chain.versions.len(), 1);
        assert_eq!(chain.versions[0].value, "value1");
    }

    #[tokio::test]
    async fn test_mvcc_isolation() {
        let manager = MVCCManager::new();

        let txn1 = TransactionId(1);
        let txn2 = TransactionId(2);

        // Begin both transactions
        manager.begin_transaction(txn1).await.unwrap();
        manager.begin_transaction(txn2).await.unwrap();

        // Txn1 writes data
        manager.write_data(txn1, "shared_key", "txn1_value").await.unwrap();

        // Txn2 should not see txn1's uncommitted write
        let value = manager.read_data(txn2, "shared_key").await.unwrap();
        assert_eq!(value, None);

        // Txn1 should see its own write
        let value = manager.read_data(txn1, "shared_key").await.unwrap();
        assert_eq!(value, Some("txn1_value".to_string()));

        // Commit txn1
        manager.commit_transaction(txn1).await.unwrap();

        // Now txn2 should see the committed value
        let value = manager.read_data(txn2, "shared_key").await.unwrap();
        assert_eq!(value, Some("txn1_value".to_string()));
    }

    #[tokio::test]
    async fn test_mvcc_abort() {
        let manager = MVCCManager::new();
        let txn_id = TransactionId(1);

        // Begin transaction
        manager.begin_transaction(txn_id).await.unwrap();

        // Write data
        manager.write_data(txn_id, "test_key", "test_value").await.unwrap();

        // Verify data exists
        let value = manager.read_data(txn_id, "test_key").await.unwrap();
        assert_eq!(value, Some("test_value".to_string()));

        // Abort transaction
        manager.abort_transaction(txn_id).await.unwrap();

        // Data should be gone
        let chain = manager.get_version_chain("test_key");
        assert!(chain.is_none() || chain.unwrap().versions.is_empty());
    }

    #[tokio::test]
    async fn test_garbage_collection() {
        let manager = MVCCManager::new();

        // Create multiple transactions and versions
        for i in 1..=5 {
            let txn_id = TransactionId(i);
            manager.begin_transaction(txn_id).await.unwrap();
            manager.write_data(txn_id, &format!("key{}", i), &format!("value{}", i)).await.unwrap();
            manager.commit_transaction(txn_id).await.unwrap();
        }

        let initial_stats = manager.stats();
        let initial_versions = initial_stats.total_versions;

        // Perform garbage collection with all transactions committed
        manager.perform_garbage_collection().await.unwrap();

        let final_stats = manager.stats();

        // Should not have collected anything since all transactions are "old"
        assert_eq!(final_stats.total_versions, initial_versions);
        assert_eq!(final_stats.garbage_collected_versions, 0);
    }
}
