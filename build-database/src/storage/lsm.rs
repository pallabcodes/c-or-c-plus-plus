//! LSM Tree Storage Engine Implementation

use crate::storage::engine::*;
use crate::core::*;
use std::collections::BTreeMap;
use std::sync::Arc;
use parking_lot::RwLock;

/// LSM Tree storage engine implementation
pub struct LSMStorageEngine {
    /// Active memtable for writes
    memtable: Arc<RwLock<BTreeMap<Vec<u8>, Vec<u8>>>>,
    /// Immutable memtables being flushed
    immutable_memtables: Arc<RwLock<Vec<BTreeMap<Vec<u8>, Vec<u8>>>>>,
    /// Configuration
    config: StorageEngineConfig,
    /// Statistics
    stats: Arc<RwLock<StorageStats>>,
}

impl LSMStorageEngine {
    pub fn new(config: StorageEngineConfig) -> Self {
        Self {
            memtable: Arc::new(RwLock::new(BTreeMap::new())),
            immutable_memtables: Arc::new(RwLock::new(Vec::new())),
            config,
            stats: Arc::new(RwLock::new(StorageStats::default())),
        }
    }

    fn should_flush_memtable(&self) -> bool {
        let memtable = self.memtable.read();
        memtable.len() > 1000
    }

    async fn flush_memtable(&mut self) -> StorageResult<()> {
        let mut memtable = self.memtable.write();
        let mut immutable = self.immutable_memtables.write();

        if memtable.is_empty() {
            return Ok(());
        }

        let flushed_table = std::mem::take(&mut *memtable);
        immutable.push(flushed_table);

        Ok(())
    }
}

#[async_trait::async_trait]
impl StorageEngine for LSMStorageEngine {
    async fn init(&mut self, _config: &DatabaseConfig) -> StorageResult<()> {
        Ok(())
    }

    async fn shutdown(&mut self) -> StorageResult<()> {
        self.flush_memtable().await?;
        Ok(())
    }

    async fn put(&mut self, key: &[u8], value: &[u8]) -> StorageResult<()> {
        let mut memtable = self.memtable.write();
        let mut stats = self.stats.write();

        if self.should_flush_memtable() {
            drop(memtable);
            drop(stats);
            self.flush_memtable().await?;
            memtable = self.memtable.write();
            stats = self.stats.write();
        }

        let key_vec = key.to_vec();
        let value_vec = value.to_vec();
        let is_update = memtable.contains_key(&key_vec);

        memtable.insert(key_vec, value_vec);

        stats.write_operations += 1;
        stats.total_keys += if is_update { 0 } else { 1 };
        stats.total_size_bytes += (key.len() + value.len()) as u64;

        Ok(())
    }

    async fn get(&self, key: &[u8]) -> StorageResult<Option<Vec<u8>>> {
        let mut stats = self.stats.write();
        stats.read_operations += 1;

        // Check memtable first
        {
            let memtable = self.memtable.read();
            if let Some(value) = memtable.get(key) {
                return Ok(Some(value.clone()));
            }
        }

        // Check immutable memtables
        {
            let immutable = self.immutable_memtables.read();
            for table in immutable.iter().rev() {
                if let Some(value) = table.get(key) {
                    return Ok(Some(value.clone()));
                }
            }
        }

        Ok(None)
    }

    async fn delete(&mut self, key: &[u8]) -> StorageResult<bool> {
        let delete_marker = vec![0u8];
        self.put(key, &delete_marker).await?;
        Ok(true)
    }

    async fn exists(&self, key: &[u8]) -> StorageResult<bool> {
        match self.get(key).await? {
            Some(value) => Ok(value != vec![0u8]),
            None => Ok(false),
        }
    }

    async fn range(&self, start: &[u8], end: &[u8]) -> StorageResult<Box<dyn Iterator<Item = (Vec<u8>, Vec<u8>)> + Send>> {
        let mut results = Vec::new();

        // Collect from memtable
        {
            let memtable = self.memtable.read();
            for (k, v) in memtable.range(start.to_vec()..end.to_vec()) {
                results.push((k.clone(), v.clone()));
            }
        }

        // Collect from immutable memtables
        {
            let immutable = self.immutable_memtables.read();
            for table in &*immutable {
                for (k, v) in table.range(start.to_vec()..end.to_vec()) {
                    results.push((k.clone(), v.clone()));
                }
            }
        }

        results.sort_by(|a, b| a.0.cmp(&b.0));
        results.dedup_by(|a, b| a.0 == b.0);

        Ok(Box::new(results.into_iter()))
    }

    async fn stats(&self) -> StorageResult<StorageStats> {
        let stats = self.stats.read();
        let mut result = stats.clone();

        let memtable = self.memtable.read();
        let immutable = self.immutable_memtables.read();
        result.total_keys += memtable.len() as u64;
        for table in &*immutable {
            result.total_keys += table.len() as u64;
        }

        Ok(result)
    }

    async fn flush(&mut self) -> StorageResult<()> {
        self.flush_memtable().await
    }

    async fn maintenance(&mut self) -> StorageResult<()> {
        let mut stats = self.stats.write();
        stats.compaction_operations += 1;
        Ok(())
    }
}