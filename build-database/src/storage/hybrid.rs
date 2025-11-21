//! Hybrid Storage Engine Implementation

use super::engine::*;
use super::{BTreeStorageEngine, LSMStorageEngine};
use crate::core::*;
use std::collections::HashMap;
use std::sync::Arc;
use parking_lot::RwLock;

/// Workload pattern detection
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum WorkloadPattern {
    ReadHeavy,
    WriteHeavy,
    Mixed,
}

/// Hybrid storage engine combining B+ trees and LSM trees
pub struct HybridStorageEngine {
    /// B+ tree engine for read-optimized data
    btree_engine: BTreeStorageEngine,
    /// LSM tree engine for write-optimized data
    lsm_engine: LSMStorageEngine,
    /// Data placement mapping
    placement_map: Arc<RwLock<HashMap<Vec<u8>, WorkloadPattern>>>,
    /// Current workload pattern
    current_pattern: Arc<RwLock<WorkloadPattern>>,
    /// Configuration
    config: StorageEngineConfig,
    /// Statistics
    stats: Arc<RwLock<StorageStats>>,
}

impl HybridStorageEngine {
    /// Create a new hybrid storage engine
    pub fn new(config: StorageEngineConfig) -> Self {
        Self {
            btree_engine: BTreeStorageEngine::new(config.clone()),
            lsm_engine: LSMStorageEngine::new(config.clone()),
            placement_map: Arc::new(RwLock::new(HashMap::new())),
            current_pattern: Arc<RwLock::new(WorkloadPattern::Mixed)),
            config,
            stats: Arc::new(RwLock::new(StorageStats::default())),
        }
    }

    /// Route operation to appropriate engine
    async fn route_to_engine(&self, key: &[u8]) -> &dyn StorageEngine {
        let pattern = self.current_pattern.read();
        match *pattern {
            WorkloadPattern::ReadHeavy => &self.btree_engine,
            WorkloadPattern::WriteHeavy => &self.lsm_engine,
            WorkloadPattern::Mixed => &self.lsm_engine, // Default to LSM for mixed
        }
    }

    async fn route_to_engine_mut(&mut self, key: &[u8]) -> &mut dyn StorageEngine {
        let pattern = self.current_pattern.read();
        match *pattern {
            WorkloadPattern::ReadHeavy => &mut self.btree_engine,
            WorkloadPattern::WriteHeavy => &mut self.lsm_engine,
            WorkloadPattern::Mixed => &mut self.lsm_engine,
        }
    }
}

#[async_trait::async_trait]
impl StorageEngine for HybridStorageEngine {
    async fn init(&mut self, config: &DatabaseConfig) -> StorageResult<()> {
        self.btree_engine.init(config).await?;
        self.lsm_engine.init(config).await?;
        Ok(())
    }

    async fn shutdown(&mut self) -> StorageResult<()> {
        self.btree_engine.shutdown().await?;
        self.lsm_engine.shutdown().await?;
        Ok(())
    }

    async fn put(&mut self, key: &[u8], value: &[u8]) -> StorageResult<()> {
        let engine = self.route_to_engine_mut(key).await;
        let result = engine.put(key, value).await?;

        let mut stats = self.stats.write();
        stats.write_operations += 1;

        Ok(result)
    }

    async fn get(&self, key: &[u8]) -> StorageResult<Option<Vec<u8>>> {
        let engine = self.route_to_engine(key).await;
        let result = engine.get(key).await?;

        let mut stats = self.stats.write();
        stats.read_operations += 1;

        Ok(result)
    }

    async fn delete(&mut self, key: &[u8]) -> StorageResult<bool> {
        let engine = self.route_to_engine_mut(key).await;
        let result = engine.delete(key).await?;

        if result {
            let mut stats = self.stats.write();
            stats.write_operations += 1;
        }

        Ok(result)
    }

    async fn exists(&self, key: &[u8]) -> StorageResult<bool> {
        let engine = self.route_to_engine(key).await;
        engine.exists(key).await
    }

    async fn range(&self, start: &[u8], end: &[u8]) -> StorageResult<Box<dyn Iterator<Item = (Vec<u8>, Vec<u8>)> + Send>> {
        // Prefer B+ tree for range queries
        self.btree_engine.range(start, end).await
    }

    async fn stats(&self) -> StorageResult<StorageStats> {
        let btree_stats = self.btree_engine.stats().await?;
        let lsm_stats = self.lsm_engine.stats().await?;
        let hybrid_stats = self.stats.read();

        Ok(StorageStats {
            total_keys: btree_stats.total_keys + lsm_stats.total_keys,
            total_size_bytes: btree_stats.total_size_bytes + lsm_stats.total_size_bytes,
            read_operations: btree_stats.read_operations + lsm_stats.read_operations + hybrid_stats.read_operations,
            write_operations: btree_stats.write_operations + lsm_stats.write_operations + hybrid_stats.write_operations,
            cache_hit_ratio: (btree_stats.cache_hit_ratio + lsm_stats.cache_hit_ratio) / 2.0,
            compaction_operations: btree_stats.compaction_operations + lsm_stats.compaction_operations,
            average_response_time_ms: (btree_stats.average_response_time_ms + lsm_stats.average_response_time_ms) / 2.0,
        })
    }

    async fn flush(&mut self) -> StorageResult<()> {
        self.btree_engine.flush().await?;
        self.lsm_engine.flush().await?;
        Ok(())
    }

    async fn maintenance(&mut self) -> StorageResult<()> {
        self.btree_engine.maintenance().await?;
        self.lsm_engine.maintenance().await?;
        Ok(())
    }
}