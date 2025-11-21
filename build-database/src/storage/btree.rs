//! B+ Tree Storage Engine Implementation

use crate::storage::engine::*;
use crate::core::*;
use std::collections::BTreeMap;
use std::sync::Arc;
use parking_lot::RwLock;

/// B+ Tree storage engine implementation
pub struct BTreeStorageEngine {
    /// In-memory tree structure (for simplicity - production would use page-based storage)
    tree: Arc<RwLock<BTreeMap<Vec<u8>, Vec<u8>>>>,
    /// Configuration
    config: StorageEngineConfig,
    /// Statistics
    stats: Arc<RwLock<StorageStats>>,
    /// Next node ID for allocation
    next_node_id: Arc<RwLock<u64>>,
}

impl BTreeStorageEngine {
    /// Create a new B+ tree storage engine
    pub fn new(config: StorageEngineConfig) -> Self {
        Self {
            tree: Arc::new(RwLock::new(BTreeMap::new())),
            config,
            stats: Arc::new(RwLock::new(StorageStats::default())),
            next_node_id: Arc::new(RwLock::new(1)),
        }
    }
}

#[async_trait::async_trait]
impl StorageEngine for BTreeStorageEngine {
    async fn init(&mut self, _config: &DatabaseConfig) -> StorageResult<()> {
        Ok(())
    }

    async fn shutdown(&mut self) -> StorageResult<()> {
        Ok(())
    }

    async fn put(&mut self, key: &[u8], value: &[u8]) -> StorageResult<()> {
        let mut tree = self.tree.write();
        let mut stats = self.stats.write();

        let key_vec = key.to_vec();
        let value_vec = value.to_vec();
        let is_update = tree.contains_key(&key_vec);

        tree.insert(key_vec, value_vec);

        stats.write_operations += 1;
        stats.total_keys = tree.len() as u64;
        stats.total_size_bytes += value.len() as u64;

        if is_update {
            stats.total_size_bytes += key.len() as u64;
        }

        Ok(())
    }

    async fn get(&self, key: &[u8]) -> StorageResult<Option<Vec<u8>>> {
        let tree = self.tree.read();
        let mut stats = self.stats.write();

        stats.read_operations += 1;

        match tree.get(key) {
            Some(value) => Ok(Some(value.clone())),
            None => Ok(None),
        }
    }

    async fn delete(&mut self, key: &[u8]) -> StorageResult<bool> {
        let mut tree = self.tree.write();
        let mut stats = self.stats.write();

        if let Some(removed_value) = tree.remove(key) {
            stats.write_operations += 1;
            stats.total_keys = tree.len() as u64;
            stats.total_size_bytes -= removed_value.len() as u64;
            Ok(true)
        } else {
            Ok(false)
        }
    }

    async fn exists(&self, key: &[u8]) -> StorageResult<bool> {
        let tree = self.tree.read();
        let mut stats = self.stats.write();

        stats.read_operations += 1;
        Ok(tree.contains_key(key))
    }

    async fn range(&self, start: &[u8], end: &[u8]) -> StorageResult<Box<dyn Iterator<Item = (Vec<u8>, Vec<u8>)> + Send>> {
        let tree = self.tree.read();
        let mut stats = self.stats.write();

        stats.read_operations += 1;

        let iter = tree.range(start.to_vec()..end.to_vec())
            .map(|(k, v)| (k.clone(), v.clone()))
            .collect::<Vec<_>>()
            .into_iter();

        Ok(Box::new(iter))
    }

    async fn stats(&self) -> StorageResult<StorageStats> {
        let stats = self.stats.read();
        Ok(stats.clone())
    }

    async fn flush(&mut self) -> StorageResult<()> {
        Ok(())
    }

    async fn maintenance(&mut self) -> StorageResult<()> {
        let mut stats = self.stats.write();
        stats.compaction_operations += 1;
        Ok(())
    }
}