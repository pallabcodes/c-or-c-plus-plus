//! Hash Index: High-Performance Equality Lookups
//!
//! Optimized hash-based index for constant-time equality lookups
//! and primary key operations.

use std::collections::HashMap;
use crate::core::errors::AuroraResult;

#[derive(Debug, Clone)]
pub struct HashIndexConfig {
    pub name: String,
    pub columns: Vec<String>,
    pub bucket_count: usize,
}

#[derive(Debug)]
pub struct HashIndex {
    config: HashIndexConfig,
    buckets: Vec<HashMap<String, Vec<u64>>>, // composite_key -> row_ids
    total_entries: u64,
}

impl HashIndex {
    pub fn new(config: HashIndexConfig) -> AuroraResult<Self> {
        let buckets = (0..config.bucket_count)
            .map(|_| HashMap::new())
            .collect();

        Ok(Self {
            config,
            buckets,
            total_entries: 0,
        })
    }

    pub fn insert(&mut self, key: String, row_id: u64) -> AuroraResult<()> {
        let bucket_idx = self.get_bucket(&key);
        let bucket = &mut self.buckets[bucket_idx];
        bucket.entry(key).or_insert_with(Vec::new).push(row_id);
        self.total_entries += 1;
        Ok(())
    }

    pub fn search(&self, key: &str) -> AuroraResult<Vec<u64>> {
        let bucket_idx = self.get_bucket(key);
        let bucket = &self.buckets[bucket_idx];
        Ok(bucket.get(key).cloned().unwrap_or_default())
    }

    pub fn delete(&mut self, key: &str, row_id: u64) -> AuroraResult<bool> {
        let bucket_idx = self.get_bucket(key);
        let bucket = &mut self.buckets[bucket_idx];

        if let Some(row_ids) = bucket.get_mut(key) {
            if let Some(pos) = row_ids.iter().position(|&id| id == row_id) {
                row_ids.remove(pos);
                self.total_entries -= 1;

                // Remove key if no more values
                if row_ids.is_empty() {
                    bucket.remove(key);
                }

                return Ok(true);
            }
        }

        Ok(false)
    }

    fn get_bucket(&self, key: &str) -> usize {
        use std::collections::hash_map::DefaultHasher;
        use std::hash::{Hash, Hasher};

        let mut hasher = DefaultHasher::new();
        key.hash(&mut hasher);
        (hasher.finish() as usize) % self.config.bucket_count
    }
}
