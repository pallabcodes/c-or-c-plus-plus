//! Storage Engine Traits and Interfaces
//!
//! Defines the core interfaces that all storage engines must implement,
//! providing a unified API for database operations.

use crate::core::*;
use std::collections::HashMap;

/// Storage operation result type
pub type StorageResult<T> = Result<T, StorageError>;

/// Storage engine specific errors
#[derive(Debug, thiserror::Error)]
pub enum StorageError {
    #[error("I/O error: {0}")]
    Io(#[from] std::io::Error),

    #[error("Page not found: {0}")]
    PageNotFound(PageId),

    #[error("Key not found: {0:?}")]
    KeyNotFound(Vec<u8>),

    #[error("Duplicate key: {0:?}")]
    DuplicateKey(Vec<u8>),

    #[error("Storage engine error: {0}")]
    Engine(String),

    #[error("Corruption detected: {0}")]
    Corruption(String),

    #[error("Transaction conflict: {0}")]
    TransactionConflict(String),
}

/// Core storage engine trait that all implementations must satisfy
#[async_trait::async_trait]
pub trait StorageEngine: Send + Sync {
    /// Initialize the storage engine with configuration
    async fn init(&mut self, config: &DatabaseConfig) -> StorageResult<()>;

    /// Shutdown the storage engine gracefully
    async fn shutdown(&mut self) -> StorageResult<()>;

    /// Insert or update a key-value pair
    async fn put(&mut self, key: &[u8], value: &[u8]) -> StorageResult<()>;

    /// Retrieve a value by key
    async fn get(&self, key: &[u8]) -> StorageResult<Option<Vec<u8>>>;

    /// Delete a key-value pair
    async fn delete(&mut self, key: &[u8]) -> StorageResult<bool>;

    /// Check if a key exists
    async fn exists(&self, key: &[u8]) -> StorageResult<bool>;

    /// Get an iterator over a key range
    async fn range(&self, start: &[u8], end: &[u8]) -> StorageResult<Box<dyn Iterator<Item = (Vec<u8>, Vec<u8>)> + Send>>;

    /// Get storage statistics
    async fn stats(&self) -> StorageResult<StorageStats>;

    /// Flush pending writes to durable storage
    async fn flush(&mut self) -> StorageResult<()>;

    /// Perform maintenance operations (compaction, etc.)
    async fn maintenance(&mut self) -> StorageResult<()>;
}

/// Storage engine statistics
#[derive(Debug, Clone, Default)]
pub struct StorageStats {
    pub total_keys: u64,
    pub total_size_bytes: u64,
    pub read_operations: u64,
    pub write_operations: u64,
    pub cache_hit_ratio: f64,
    pub compaction_operations: u64,
    pub average_response_time_ms: f64,
}

/// Storage engine types available in AuroraDB
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum StorageEngineType {
    BTree,
    LSM,
    Hybrid,
}

/// Configuration for storage engine selection and tuning
#[derive(Debug, Clone)]
pub struct StorageEngineConfig {
    pub engine_type: StorageEngineType,
    pub page_size: usize,
    pub cache_size: usize,
    pub max_file_size: u64,
    pub compaction_threshold: f64,
    pub enable_compression: bool,
    pub enable_encryption: bool,
    pub write_ahead_log: bool,
}
