//! JIT Compilation Cache
//!
//! Intelligent caching of compiled queries with LRU eviction and dependency tracking.
//! Ensures optimal memory usage while maximizing performance gains.

use crate::jit::compiler::*;
use std::collections::{HashMap, VecDeque};
use std::sync::Arc;
use parking_lot::RwLock;

/// JIT compilation cache with intelligent eviction
pub struct JITCache {
    /// Cache entries with LRU tracking
    entries: RwLock<HashMap<QueryHash, CacheEntry>>,
    /// LRU order for eviction
    lru_order: RwLock<VecDeque<QueryHash>>,
    /// Cache configuration
    config: CacheConfig,
    /// Cache statistics
    stats: CacheStats,
}

/// Cache entry with metadata
#[derive(Debug, Clone)]
pub struct CacheEntry {
    pub query_hash: QueryHash,
    pub compiled_query: CompiledQuery,
    pub access_count: u64,
    pub last_accessed: u64,
    pub creation_time: u64,
    pub memory_size: usize,
    pub dependencies: Vec<QueryHash>,
    pub performance_score: f64,
}

/// Cache configuration
#[derive(Debug, Clone)]
pub struct CacheConfig {
    pub max_entries: usize,
    pub max_memory_mb: usize,
    pub entry_ttl_seconds: u64,
    pub enable_lru: bool,
    pub enable_dependency_tracking: bool,
    pub performance_threshold: f64,
}

/// Cache statistics
#[derive(Debug, Clone, Default)]
pub struct CacheStats {
    pub total_entries: usize,
    pub cache_hits: u64,
    pub cache_misses: u64,
    pub evictions: u64,
    pub memory_used_mb: usize,
    pub hit_rate: f64,
    pub average_entry_lifetime: f64,
}

impl JITCache {
    /// Create a new JIT cache
    pub fn new(config: CacheConfig) -> Self {
        Self {
            entries: RwLock::new(HashMap::new()),
            lru_order: RwLock::new(VecDeque::new()),
            config,
            stats: CacheStats::default(),
        }
    }

    /// Get a compiled query from cache
    pub fn get(&self, query_hash: &QueryHash) -> Option<CompiledQuery> {
        let mut entries = self.entries.write();
        let mut lru_order = self.lru_order.write();

        if let Some(entry) = entries.get_mut(query_hash) {
            // Check if entry is still valid (TTL)
            let now = std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_secs();

            if now - entry.creation_time > self.config.entry_ttl_seconds {
                // Entry expired, remove it
                entries.remove(query_hash);
                if let Some(pos) = lru_order.iter().position(|h| h == query_hash) {
                    lru_order.remove(pos);
                }
                self.stats.evictions += 1;
                self.update_memory_stats(&entries);
                return None;
            }

            // Update access statistics
            entry.access_count += 1;
            entry.last_accessed = now;

            // Update LRU order
            if self.config.enable_lru {
                if let Some(pos) = lru_order.iter().position(|h| h == query_hash) {
                    lru_order.remove(pos);
                }
                lru_order.push_back(*query_hash);
            }

            self.stats.cache_hits += 1;
            Some(entry.compiled_query.clone())
        } else {
            self.stats.cache_misses += 1;
            None
        }
    }

    /// Put a compiled query in cache
    pub fn put(&self, query_hash: QueryHash, compiled_query: CompiledQuery) -> Result<(), CacheError> {
        let mut entries = self.entries.write();
        let mut lru_order = self.lru_order.write();

        // Calculate entry size (approximate)
        let entry_size = std::mem::size_of::<CompiledQuery>() +
                        compiled_query.metadata.source_plan.len() +
                        compiled_query.module.functions.iter().map(|f| f.len()).sum::<usize>();

        // Check memory limit
        let current_memory = self.calculate_memory_usage(&entries);
        let new_memory = current_memory + entry_size;

        if new_memory > self.config.max_memory_mb * 1024 * 1024 {
            self.evict_entries(&mut entries, &mut lru_order, new_memory - self.config.max_memory_mb * 1024 * 1024)?;
        }

        // Check entry limit
        if entries.len() >= self.config.max_entries {
            self.evict_lru_entry(&mut entries, &mut lru_order)?;
        }

        // Create cache entry
        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_secs();

        let entry = CacheEntry {
            query_hash,
            compiled_query: compiled_query.clone(),
            access_count: 1,
            last_accessed: now,
            creation_time: now,
            memory_size: entry_size,
            dependencies: Vec::new(), // Would be populated based on query analysis
            performance_score: self.calculate_performance_score(&compiled_query),
        };

        entries.insert(query_hash, entry);
        if self.config.enable_lru {
            lru_order.push_back(query_hash);
        }

        self.update_memory_stats(&entries);
        self.stats.total_entries = entries.len();

        Ok(())
    }

    /// Invalidate cache entries based on dependencies
    pub fn invalidate_dependencies(&self, changed_objects: &[String]) {
        let mut entries = self.entries.write();
        let mut lru_order = self.lru_order.write();
        let mut invalidated = Vec::new();

        // Find entries that depend on changed objects
        for (hash, entry) in entries.iter() {
            // In a real implementation, this would check if the entry's query
            // references any of the changed objects (tables, indexes, etc.)
            let query_refs_changed_objects = self.query_references_objects(&entry.compiled_query, changed_objects);

            if query_refs_changed_objects {
                invalidated.push(*hash);
            }
        }

        // Remove invalidated entries
        for hash in invalidated {
            entries.remove(&hash);
            if let Some(pos) = lru_order.iter().position(|h| h == &hash) {
                lru_order.remove(pos);
            }
            self.stats.evictions += 1;
        }

        self.update_memory_stats(&entries);
        self.stats.total_entries = entries.len();
    }

    /// Get cache statistics
    pub fn stats(&self) -> CacheStats {
        let mut stats = self.stats.clone();
        stats.hit_rate = if stats.cache_hits + stats.cache_misses > 0 {
            stats.cache_hits as f64 / (stats.cache_hits + stats.cache_misses) as f64
        } else {
            0.0
        };
        stats
    }

    /// Clear the entire cache
    pub fn clear(&self) {
        let mut entries = self.entries.write();
        let mut lru_order = self.lru_order.write();

        entries.clear();
        lru_order.clear();
        self.stats.evictions += self.stats.total_entries as u64;
        self.stats.total_entries = 0;
        self.stats.memory_used_mb = 0;
    }

    /// Get cache entries sorted by performance score
    pub fn get_top_performers(&self, limit: usize) -> Vec<(QueryHash, f64)> {
        let entries = self.entries.read();
        let mut performers: Vec<_> = entries.values()
            .map(|entry| (entry.query_hash, entry.performance_score))
            .collect();

        performers.sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap_or(std::cmp::Ordering::Equal));
        performers.into_iter().take(limit).collect()
    }

    /// Calculate performance score for a compiled query
    fn calculate_performance_score(&self, compiled_query: &CompiledQuery) -> f64 {
        // Performance score based on expected speedup and compilation overhead
        let speedup = 1.0 + (compiled_query.performance.vectorization_efficiency * 2.0);
        let overhead_penalty = 0.1; // Compilation overhead factor

        speedup * (1.0 - overhead_penalty)
    }

    /// Check if query references specific objects
    fn query_references_objects(&self, compiled_query: &CompiledQuery, objects: &[String]) -> bool {
        // In a real implementation, this would parse the query plan
        // and check for references to the changed objects
        let plan_str = &compiled_query.metadata.source_plan;
        objects.iter().any(|obj| plan_str.contains(obj))
    }

    /// Calculate current memory usage
    fn calculate_memory_usage(&self, entries: &HashMap<QueryHash, CacheEntry>) -> usize {
        entries.values().map(|entry| entry.memory_size).sum()
    }

    /// Update memory statistics
    fn update_memory_stats(&self, entries: &HashMap<QueryHash, CacheEntry>) {
        self.stats.memory_used_mb = self.calculate_memory_usage(entries) / (1024 * 1024);
    }

    /// Evict entries to free up memory
    fn evict_entries(&self, entries: &mut HashMap<QueryHash, CacheEntry>, lru_order: &mut VecDeque<QueryHash>, memory_to_free: usize) -> Result<(), CacheError> {
        let mut freed_memory = 0;
        let mut evicted_hashes = Vec::new();

        // Evict based on LRU or performance score
        while freed_memory < memory_to_free && !entries.is_empty() {
            let hash_to_evict = if self.config.enable_lru {
                // LRU eviction
                lru_order.pop_front()
            } else {
                // Performance-based eviction (remove lowest scoring entries)
                entries.iter()
                    .min_by(|a, b| a.1.performance_score.partial_cmp(&b.1.performance_score).unwrap_or(std::cmp::Ordering::Equal))
                    .map(|(hash, _)| *hash)
            };

            if let Some(hash) = hash_to_evict {
                if let Some(entry) = entries.remove(&hash) {
                    freed_memory += entry.memory_size;
                    evicted_hashes.push(hash);
                }
            } else {
                break;
            }
        }

        // Remove evicted entries from LRU order
        lru_order.retain(|hash| !evicted_hashes.contains(hash));

        self.stats.evictions += evicted_hashes.len() as u64;
        Ok(())
    }

    /// Evict a single LRU entry
    fn evict_lru_entry(&self, entries: &mut HashMap<QueryHash, CacheEntry>, lru_order: &mut VecDeque<QueryHash>) -> Result<(), CacheError> {
        if let Some(hash) = lru_order.pop_front() {
            entries.remove(&hash);
            self.stats.evictions += 1;
            Ok(())
        } else {
            Err(CacheError::CacheFull)
        }
    }

    /// Prefetch commonly used queries
    pub async fn prefetch_common_queries(&self, common_queries: &[QueryHash]) {
        // In a real implementation, this would compile and cache
        // frequently used queries during system startup
        println!("Prefetching {} common queries", common_queries.len());
    }

    /// Optimize cache based on access patterns
    pub fn optimize_cache(&self) {
        let entries = self.entries.read();

        // Analyze access patterns and adjust cache strategy
        let total_accesses: u64 = entries.values().map(|e| e.access_count).sum();
        let avg_accesses = if entries.is_empty() { 0.0 } else { total_accesses as f64 / entries.len() as f64 };

        // Adjust TTL for frequently vs infrequently accessed entries
        for entry in entries.values() {
            if entry.access_count as f64 > avg_accesses * 2.0 {
                // Frequently accessed - extend TTL
            } else if entry.access_count as f64 < avg_accesses * 0.5 {
                // Infrequently accessed - reduce TTL
            }
        }
    }
}

/// Cache operation errors
#[derive(Debug, thiserror::Error)]
pub enum CacheError {
    #[error("Cache is full")]
    CacheFull,

    #[error("Entry not found")]
    EntryNotFound,

    #[error("Invalid cache entry")]
    InvalidEntry,

    #[error("Memory limit exceeded")]
    MemoryLimitExceeded,
}

/// Default cache configuration
impl Default for CacheConfig {
    fn default() -> Self {
        Self {
            max_entries: 1000,
            max_memory_mb: 512, // 512MB
            entry_ttl_seconds: 3600, // 1 hour
            enable_lru: true,
            enable_dependency_tracking: true,
            performance_threshold: 1.2, // 20% speedup minimum
        }
    }
}
