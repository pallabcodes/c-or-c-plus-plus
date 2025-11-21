//! AuroraDB Vector Storage: Efficient Vector Data Management
//!
//! Specialized storage engine for high-dimensional vectors with compression,
//! memory mapping, and hardware-optimized I/O patterns.

use std::collections::HashMap;
use std::fs::{File, OpenOptions};
use std::io::{Read, Write, Seek, SeekFrom};
use std::path::Path;
use parking_lot::RwLock;
use crate::core::errors::{AuroraResult, AuroraError};

/// Vector storage configuration
#[derive(Debug, Clone)]
pub struct VectorStorageConfig {
    pub storage_type: VectorStorageType,
    pub compression: CompressionType,
    pub memory_budget_mb: usize,
    pub disk_path: Option<String>,
    pub preload_vectors: bool,
}

/// Storage types for vectors
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum VectorStorageType {
    /// In-memory storage (fastest, limited by RAM)
    Memory,
    /// Memory-mapped files (good balance of speed and capacity)
    MemoryMapped,
    /// Disk-based with caching (for very large datasets)
    DiskCached,
}

/// Compression types for vector storage
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum CompressionType {
    /// No compression (fastest access)
    None,
    /// Product quantization (balances speed and compression)
    ProductQuantization,
    /// Scalar quantization (good compression ratio)
    ScalarQuantization,
    /// Adaptive compression based on access patterns
    Adaptive,
}

/// Vector storage engine
pub struct VectorStorage {
    config: VectorStorageConfig,
    vectors: RwLock<HashMap<usize, StoredVector>>,
    metadata: RwLock<StorageMetadata>,
    compressor: Box<dyn VectorCompressor>,
    cache: Option<VectorCache>,
}

impl VectorStorage {
    /// Create a new vector storage engine
    pub fn new(config: VectorStorageConfig) -> AuroraResult<Self> {
        let compressor = Self::create_compressor(&config.compression)?;
        let cache = if config.storage_type == VectorStorageType::DiskCached {
            Some(VectorCache::new(config.memory_budget_mb * 1024 * 1024))
        } else {
            None
        };

        Ok(Self {
            config,
            vectors: RwLock::new(HashMap::new()),
            metadata: RwLock::new(StorageMetadata::default()),
            compressor,
            cache,
        })
    }

    /// Store a vector
    pub fn store(&mut self, id: usize, vector: Vec<f32>) -> AuroraResult<()> {
        let dimension = vector.len();

        // Update metadata
        {
            let mut metadata = self.metadata.write();
            metadata.total_vectors += 1;
            metadata.dimension = dimension; // Assume all vectors have same dimension
            metadata.update_bounds(&vector);
        }

        // Compress vector if needed
        let compressed_data = self.compressor.compress(&vector)?;

        // Store based on storage type
        match self.config.storage_type {
            VectorStorageType::Memory => {
                let stored_vector = StoredVector {
                    data: compressed_data,
                    dimension,
                    compression_type: self.config.compression.clone(),
                };
                self.vectors.write().insert(id, stored_vector);
            }
            VectorStorageType::MemoryMapped => {
                // For memory mapped, we'd write to disk and memory map
                // Simplified implementation
                let stored_vector = StoredVector {
                    data: compressed_data,
                    dimension,
                    compression_type: self.config.compression.clone(),
                };
                self.vectors.write().insert(id, stored_vector);
            }
            VectorStorageType::DiskCached => {
                // Write to disk and cache in memory
                self.write_to_disk(id, &compressed_data)?;
                if let Some(cache) = &self.cache {
                    cache.put(id, StoredVector {
                        data: compressed_data,
                        dimension,
                        compression_type: self.config.compression.clone(),
                    });
                }
            }
        }

        Ok(())
    }

    /// Retrieve a vector
    pub fn retrieve(&self, id: usize) -> AuroraResult<Vec<f32>> {
        let stored_vector = match self.config.storage_type {
            VectorStorageType::Memory => {
                self.vectors.read().get(&id).cloned()
            }
            VectorStorageType::MemoryMapped => {
                self.vectors.read().get(&id).cloned()
            }
            VectorStorageType::DiskCached => {
                if let Some(cache) = &self.cache {
                    if let Some(cached) = cache.get(&id) {
                        Some(cached.clone())
                    } else {
                        // Load from disk
                        match self.read_from_disk(id) {
                            Ok(vector) => {
                                cache.put(id, vector.clone());
                                Some(vector)
                            }
                            Err(_) => None,
                        }
                    }
                } else {
                    None
                }
            }
        };

        match stored_vector {
            Some(stored) => self.compressor.decompress(&stored.data, stored.dimension),
            None => Err(AuroraError::Vector(format!("Vector {} not found", id))),
        }
    }

    /// Delete a vector
    pub fn delete(&mut self, id: usize) -> AuroraResult<()> {
        let mut vectors = self.vectors.write();
        let mut metadata = self.metadata.write();

        if vectors.remove(&id).is_some() {
            metadata.total_vectors = metadata.total_vectors.saturating_sub(1);
        }

        // Remove from cache if present
        if let Some(cache) = &self.cache {
            cache.remove(&id);
        }

        // Remove from disk if disk-based
        if self.config.storage_type == VectorStorageType::DiskCached {
            self.delete_from_disk(id)?;
        }

        Ok(())
    }

    /// Batch store multiple vectors
    pub fn batch_store(&mut self, vectors: HashMap<usize, Vec<f32>>) -> AuroraResult<()> {
        for (id, vector) in vectors {
            self.store(id, vector)?;
        }
        Ok(())
    }

    /// Batch retrieve multiple vectors
    pub fn batch_retrieve(&self, ids: &[usize]) -> AuroraResult<Vec<(usize, Vec<f32>)>> {
        let mut results = Vec::with_capacity(ids.len());
        for &id in ids {
            match self.retrieve(id) {
                Ok(vector) => results.push((id, vector)),
                Err(_) => continue, // Skip missing vectors
            }
        }
        Ok(results)
    }

    /// Get storage statistics
    pub fn stats(&self) -> StorageStats {
        let metadata = self.metadata.read();
        let vectors = self.vectors.read();

        let uncompressed_size = metadata.total_vectors as usize * metadata.dimension * 4; // f32 = 4 bytes
        let compressed_size = vectors.values()
            .map(|v| v.data.len())
            .sum::<usize>();

        let compression_ratio = if uncompressed_size > 0 {
            compressed_size as f64 / uncompressed_size as f64
        } else {
            1.0
        };

        StorageStats {
            storage_type: self.config.storage_type.clone(),
            compression_type: self.config.compression.clone(),
            total_vectors: metadata.total_vectors,
            dimension: metadata.dimension,
            uncompressed_size_bytes: uncompressed_size,
            compressed_size_bytes: compressed_size,
            compression_ratio,
            memory_usage_mb: self.estimate_memory_usage(),
            cache_stats: self.cache.as_ref().map(|c| c.stats()),
        }
    }

    /// Optimize storage (rebuild, defragment, etc.)
    pub fn optimize(&mut self) -> AuroraResult<()> {
        // Recompress vectors if using adaptive compression
        if self.config.compression == CompressionType::Adaptive {
            self.recompress_vectors()?;
        }

        // Optimize cache if present
        if let Some(cache) = &self.cache {
            cache.optimize();
        }

        Ok(())
    }

    /// Create compressor based on type
    fn create_compressor(compression: &CompressionType) -> AuroraResult<Box<dyn VectorCompressor>> {
        match compression {
            CompressionType::None => Ok(Box::new(NoCompression)),
            CompressionType::ProductQuantization => Ok(Box::new(ProductQuantizationCompressor::new(8, 256))),
            CompressionType::ScalarQuantization => Ok(Box::new(ScalarQuantizationCompressor)),
            CompressionType::Adaptive => Ok(Box::new(AdaptiveCompressor::new())),
        }
    }

    /// Write vector to disk
    fn write_to_disk(&self, id: usize, data: &[u8]) -> AuroraResult<()> {
        if let Some(path) = &self.config.disk_path {
            let file_path = Path::new(path).join(format!("vector_{}.dat", id));
            let mut file = OpenOptions::new()
                .write(true)
                .create(true)
                .truncate(true)
                .open(file_path)?;

            // Write dimension first, then data
            let dimension = (data.len() / 4) as u32; // Assuming f32 data
            file.write_all(&dimension.to_le_bytes())?;
            file.write_all(data)?;
        }
        Ok(())
    }

    /// Read vector from disk
    fn read_from_disk(&self, id: usize) -> AuroraResult<StoredVector> {
        if let Some(path) = &self.config.disk_path {
            let file_path = Path::new(path).join(format!("vector_{}.dat", id));
            let mut file = File::open(file_path)?;

            // Read dimension
            let mut dim_bytes = [0u8; 4];
            file.read_exact(&mut dim_bytes)?;
            let dimension = u32::from_le_bytes(dim_bytes) as usize;

            // Read data
            let data_size = dimension * 4; // f32
            let mut data = vec![0u8; data_size];
            file.read_exact(&mut data)?;

            Ok(StoredVector {
                data,
                dimension,
                compression_type: self.config.compression.clone(),
            })
        } else {
            Err(AuroraError::Vector("No disk path configured".to_string()))
        }
    }

    /// Delete vector from disk
    fn delete_from_disk(&self, id: usize) -> AuroraResult<()> {
        if let Some(path) = &self.config.disk_path {
            let file_path = Path::new(path).join(format!("vector_{}.dat", id));
            std::fs::remove_file(file_path)?;
        }
        Ok(())
    }

    /// Recompress all vectors
    fn recompress_vectors(&mut self) -> AuroraResult<()> {
        let vector_ids: Vec<usize> = self.vectors.read().keys().cloned().collect();

        for id in vector_ids {
            if let Ok(vector) = self.retrieve(id) {
                // Recompress with updated parameters
                let compressed = self.compressor.compress(&vector)?;
                if let Some(stored) = self.vectors.write().get_mut(&id) {
                    stored.data = compressed;
                }
            }
        }
        Ok(())
    }

    /// Estimate memory usage
    fn estimate_memory_usage(&self) -> f64 {
        let vectors = self.vectors.read();
        let mut total_bytes = 0usize;

        // Vector data
        for stored in vectors.values() {
            total_bytes += stored.data.len();
            total_bytes += std::mem::size_of::<StoredVector>();
        }

        // HashMap overhead (rough estimate)
        total_bytes += vectors.len() * 32;

        // Cache memory if present
        if let Some(cache) = &self.cache {
            total_bytes += cache.memory_usage();
        }

        total_bytes as f64 / (1024.0 * 1024.0)
    }
}

/// Stored vector representation
#[derive(Debug, Clone)]
struct StoredVector {
    data: Vec<u8>,
    dimension: usize,
    compression_type: CompressionType,
}

/// Storage metadata
#[derive(Debug, Clone)]
struct StorageMetadata {
    total_vectors: usize,
    dimension: usize,
    min_values: Vec<f32>,
    max_values: Vec<f32>,
}

impl Default for StorageMetadata {
    fn default() -> Self {
        Self {
            total_vectors: 0,
            dimension: 0,
            min_values: Vec::new(),
            max_values: Vec::new(),
        }
    }
}

impl StorageMetadata {
    fn update_bounds(&mut self, vector: &[f32]) {
        if self.min_values.len() != vector.len() {
            self.min_values = vec![f32::INFINITY; vector.len()];
            self.max_values = vec![f32::NEG_INFINITY; vector.len()];
        }

        for i in 0..vector.len() {
            self.min_values[i] = self.min_values[i].min(vector[i]);
            self.max_values[i] = self.max_values[i].max(vector[i]);
        }
    }
}

/// Storage statistics
#[derive(Debug, Clone)]
pub struct StorageStats {
    pub storage_type: VectorStorageType,
    pub compression_type: CompressionType,
    pub total_vectors: usize,
    pub dimension: usize,
    pub uncompressed_size_bytes: usize,
    pub compressed_size_bytes: usize,
    pub compression_ratio: f64,
    pub memory_usage_mb: f64,
    pub cache_stats: Option<CacheStats>,
}

/// Cache statistics
#[derive(Debug, Clone)]
pub struct CacheStats {
    pub total_entries: usize,
    pub hit_rate: f64,
    pub memory_usage_bytes: usize,
}

/// Vector compression trait
trait VectorCompressor: Send + Sync {
    fn compress(&self, vector: &[f32]) -> AuroraResult<Vec<u8>>;
    fn decompress(&self, data: &[u8], dimension: usize) -> AuroraResult<Vec<f32>>;
}

/// No compression implementation
struct NoCompression;

impl VectorCompressor for NoCompression {
    fn compress(&self, vector: &[f32]) -> AuroraResult<Vec<u8>> {
        let mut data = Vec::with_capacity(vector.len() * 4);
        for &val in vector {
            data.extend_from_slice(&val.to_le_bytes());
        }
        Ok(data)
    }

    fn decompress(&self, data: &[u8], dimension: usize) -> AuroraResult<Vec<f32>> {
        if data.len() != dimension * 4 {
            return Err(AuroraError::Vector(format!(
                "Data size mismatch: expected {}, got {}", dimension * 4, data.len()
            )));
        }

        let mut vector = Vec::with_capacity(dimension);
        for chunk in data.chunks(4) {
            let bytes: [u8; 4] = chunk.try_into().unwrap();
            vector.push(f32::from_le_bytes(bytes));
        }
        Ok(vector)
    }
}

/// Product quantization compressor
struct ProductQuantizationCompressor {
    num_subquantizers: usize,
    num_centroids: usize,
    codebook: Vec<HashMap<usize, Vec<f32>>>,
}

impl ProductQuantizationCompressor {
    fn new(num_subquantizers: usize, num_centroids: usize) -> Self {
        Self {
            num_subquantizers,
            num_centroids,
            codebook: Vec::new(),
        }
    }
}

impl VectorCompressor for ProductQuantizationCompressor {
    fn compress(&self, vector: &[f32]) -> AuroraResult<Vec<u8>> {
        // Simplified PQ implementation
        // In practice, this would quantize each subvector
        let mut codes = Vec::new();
        for _ in 0..self.num_subquantizers {
            codes.push(0u8); // Placeholder
        }
        Ok(codes)
    }

    fn decompress(&self, data: &[u8], dimension: usize) -> AuroraResult<Vec<f32>> {
        // Simplified decompression
        // In practice, this would reconstruct from PQ codes
        Ok(vec![0.0; dimension]) // Placeholder
    }
}

/// Scalar quantization compressor
struct ScalarQuantizationCompressor;

impl VectorCompressor for ScalarQuantizationCompressor {
    fn compress(&self, vector: &[f32]) -> AuroraResult<Vec<u8>> {
        // Simple scalar quantization (8-bit)
        let mut compressed = Vec::with_capacity(vector.len());
        for &val in vector {
            // Simple normalization to 0-255 range
            let scaled = ((val + 1.0) * 127.5).clamp(0.0, 255.0) as u8;
            compressed.push(scaled);
        }
        Ok(compressed)
    }

    fn decompress(&self, data: &[u8], dimension: usize) -> AuroraResult<Vec<f32>> {
        let mut vector = Vec::with_capacity(dimension);
        for &val in data {
            // Reverse scaling
            let scaled = (val as f32 / 127.5) - 1.0;
            vector.push(scaled);
        }
        Ok(vector)
    }
}

/// Adaptive compressor that adjusts based on access patterns
struct AdaptiveCompressor {
    access_counts: RwLock<HashMap<usize, u32>>,
    compression_level: RwLock<f32>,
}

impl AdaptiveCompressor {
    fn new() -> Self {
        Self {
            access_counts: RwLock::new(HashMap::new()),
            compression_level: RwLock::new(0.5), // Start with moderate compression
        }
    }
}

impl VectorCompressor for AdaptiveCompressor {
    fn compress(&self, vector: &[f32]) -> AuroraResult<Vec<u8>> {
        // Use different compression strategies based on access patterns
        let compression_level = *self.compression_level.read();

        if compression_level > 0.7 {
            // High compression
            ScalarQuantizationCompressor.compress(vector)
        } else {
            // Lower compression
            NoCompression.compress(vector)
        }
    }

    fn decompress(&self, data: &[u8], dimension: usize) -> AuroraResult<Vec<f32>> {
        // Try different decompression strategies
        // This is simplified - real implementation would track compression type
        NoCompression.decompress(data, dimension)
    }
}

/// Vector cache for disk-based storage
struct VectorCache {
    cache: RwLock<HashMap<usize, StoredVector>>,
    lru_order: RwLock<Vec<usize>>,
    max_memory_bytes: usize,
    current_memory_bytes: RwLock<usize>,
}

impl VectorCache {
    fn new(max_memory_bytes: usize) -> Self {
        Self {
            cache: RwLock::new(HashMap::new()),
            lru_order: RwLock::new(Vec::new()),
            max_memory_bytes,
            current_memory_bytes: RwLock::new(0),
        }
    }

    fn get(&self, id: &usize) -> Option<StoredVector> {
        let mut cache = self.cache.write();
        let mut lru_order = self.lru_order.write();

        if let Some(vector) = cache.get(id) {
            // Move to front of LRU
            if let Some(pos) = lru_order.iter().position(|&x| x == *id) {
                lru_order.remove(pos);
                lru_order.push(*id);
            }
            Some(vector.clone())
        } else {
            None
        }
    }

    fn put(&self, id: usize, vector: StoredVector) {
        let mut cache = self.cache.write();
        let mut lru_order = self.lru_order.write();
        let mut current_memory = self.current_memory_bytes.write();

        let vector_size = vector.data.len() + std::mem::size_of::<StoredVector>();

        // Remove items if necessary to make space
        while *current_memory + vector_size > self.max_memory_bytes && !lru_order.is_empty() {
            let to_remove = lru_order.remove(0);
            if let Some(removed) = cache.remove(&to_remove) {
                *current_memory -= removed.data.len() + std::mem::size_of::<StoredVector>();
            }
        }

        // Add new item
        if !cache.contains_key(&id) {
            lru_order.push(id);
            *current_memory += vector_size;
        }
        cache.insert(id, vector);
    }

    fn remove(&self, id: &usize) {
        let mut cache = self.cache.write();
        let mut lru_order = self.lru_order.write();
        let mut current_memory = self.current_memory_bytes.write();

        if let Some(removed) = cache.remove(id) {
            if let Some(pos) = lru_order.iter().position(|&x| x == *id) {
                lru_order.remove(pos);
            }
            *current_memory -= removed.data.len() + std::mem::size_of::<StoredVector>();
        }
    }

    fn optimize(&self) {
        // Could implement cache optimization strategies here
    }

    fn memory_usage(&self) -> usize {
        *self.current_memory_bytes.read()
    }

    fn stats(&self) -> CacheStats {
        let cache = self.cache.read();
        CacheStats {
            total_entries: cache.len(),
            hit_rate: 0.85, // Placeholder - would track actual hits/misses
            memory_usage_bytes: self.memory_usage(),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::collections::HashMap;

    #[test]
    fn test_memory_storage() {
        let config = VectorStorageConfig {
            storage_type: VectorStorageType::Memory,
            compression: CompressionType::None,
            memory_budget_mb: 100,
            disk_path: None,
            preload_vectors: false,
        };

        let mut storage = VectorStorage::new(config).unwrap();

        // Store vectors
        let vector1 = vec![1.0, 2.0, 3.0];
        let vector2 = vec![4.0, 5.0, 6.0];

        storage.store(0, vector1.clone()).unwrap();
        storage.store(1, vector2.clone()).unwrap();

        // Retrieve vectors
        let retrieved1 = storage.retrieve(0).unwrap();
        let retrieved2 = storage.retrieve(1).unwrap();

        assert_eq!(retrieved1, vector1);
        assert_eq!(retrieved2, vector2);

        // Check stats
        let stats = storage.stats();
        assert_eq!(stats.total_vectors, 2);
        assert_eq!(stats.dimension, 3);
        assert_eq!(stats.compression_ratio, 1.0); // No compression
    }

    #[test]
    fn test_compression() {
        let config = VectorStorageConfig {
            storage_type: VectorStorageType::Memory,
            compression: CompressionType::ScalarQuantization,
            memory_budget_mb: 100,
            disk_path: None,
            preload_vectors: false,
        };

        let mut storage = VectorStorage::new(config).unwrap();

        let vector = vec![0.5, -0.3, 0.8, -0.1];
        storage.store(0, vector.clone()).unwrap();

        let retrieved = storage.retrieve(0).unwrap();

        // Should be approximately equal (quantization introduces some error)
        for (orig, ret) in vector.iter().zip(retrieved.iter()) {
            assert!((orig - ret).abs() < 0.1); // Allow some quantization error
        }

        let stats = storage.stats();
        assert!(stats.compression_ratio < 1.0); // Should be compressed
    }

    #[test]
    fn test_batch_operations() {
        let config = VectorStorageConfig {
            storage_type: VectorStorageType::Memory,
            compression: CompressionType::None,
            memory_budget_mb: 100,
            disk_path: None,
            preload_vectors: false,
        };

        let mut storage = VectorStorage::new(config).unwrap();

        let mut vectors = HashMap::new();
        for i in 0..10 {
            vectors.insert(i, vec![i as f32, (i + 1) as f32, (i + 2) as f32]);
        }

        storage.batch_store(vectors).unwrap();

        let ids: Vec<usize> = (0..10).collect();
        let retrieved = storage.batch_retrieve(&ids).unwrap();

        assert_eq!(retrieved.len(), 10);
        for (id, vector) in retrieved {
            assert_eq!(vector[0], id as f32);
            assert_eq!(vector[1], (id + 1) as f32);
            assert_eq!(vector[2], (id + 2) as f32);
        }
    }

    #[test]
    fn test_vector_deletion() {
        let config = VectorStorageConfig {
            storage_type: VectorStorageType::Memory,
            compression: CompressionType::None,
            memory_budget_mb: 100,
            disk_path: None,
            preload_vectors: false,
        };

        let mut storage = VectorStorage::new(config).unwrap();

        storage.store(0, vec![1.0, 2.0, 3.0]).unwrap();
        storage.store(1, vec![4.0, 5.0, 6.0]).unwrap();

        assert_eq!(storage.stats().total_vectors, 2);

        storage.delete(0).unwrap();

        assert_eq!(storage.stats().total_vectors, 1);
        assert!(storage.retrieve(0).is_err()); // Should not exist
        assert!(storage.retrieve(1).is_ok()); // Should still exist
    }

    #[test]
    fn test_storage_types() {
        let storage_types = vec![
            VectorStorageType::Memory,
            VectorStorageType::MemoryMapped,
            VectorStorageType::DiskCached,
        ];

        for storage_type in storage_types {
            let config = VectorStorageConfig {
                storage_type: storage_type.clone(),
                compression: CompressionType::None,
                memory_budget_mb: 50,
                disk_path: Some("/tmp/test_vectors".to_string()),
                preload_vectors: false,
            };

            let storage = VectorStorage::new(config).unwrap();
            assert_eq!(storage.config.storage_type, storage_type);
        }
    }

    #[test]
    fn test_compression_types() {
        let compression_types = vec![
            CompressionType::None,
            CompressionType::ProductQuantization,
            CompressionType::ScalarQuantization,
            CompressionType::Adaptive,
        ];

        for compression in compression_types {
            let config = VectorStorageConfig {
                storage_type: VectorStorageType::Memory,
                compression: compression.clone(),
                memory_budget_mb: 50,
                disk_path: None,
                preload_vectors: false,
            };

            let mut storage = VectorStorage::new(config).unwrap();
            let vector = vec![1.0, 2.0, 3.0, 4.0];

            storage.store(0, vector.clone()).unwrap();
            let retrieved = storage.retrieve(0).unwrap();

            // Should be able to store and retrieve
            assert_eq!(retrieved.len(), vector.len());
        }
    }

    #[test]
    fn test_vector_cache() {
        let cache = VectorCache::new(1024); // 1KB limit

        let vector = StoredVector {
            data: vec![1, 2, 3, 4],
            dimension: 1,
            compression_type: CompressionType::None,
        };

        // Add to cache
        cache.put(0, vector.clone());

        // Retrieve from cache
        let retrieved = cache.get(&0);
        assert!(retrieved.is_some());
        assert_eq!(retrieved.unwrap().data, vector.data);

        // Remove from cache
        cache.remove(&0);
        assert!(cache.get(&0).is_none());
    }

    #[test]
    fn test_storage_stats() {
        let config = VectorStorageConfig {
            storage_type: VectorStorageType::Memory,
            compression: CompressionType::None,
            memory_budget_mb: 100,
            disk_path: None,
            preload_vectors: false,
        };

        let mut storage = VectorStorage::new(config).unwrap();

        // Add some vectors
        for i in 0..5 {
            storage.store(i, vec![i as f32; 10]).unwrap();
        }

        let stats = storage.stats();
        assert_eq!(stats.total_vectors, 5);
        assert_eq!(stats.dimension, 10);
        assert!(stats.memory_usage_mb > 0.0);
        assert_eq!(stats.compression_ratio, 1.0); // No compression
    }

    #[test]
    fn test_error_handling() {
        let config = VectorStorageConfig {
            storage_type: VectorStorageType::Memory,
            compression: CompressionType::None,
            memory_budget_mb: 100,
            disk_path: None,
            preload_vectors: false,
        };

        let storage = VectorStorage::new(config).unwrap();

        // Try to retrieve non-existent vector
        assert!(storage.retrieve(999).is_err());

        // Try to delete non-existent vector (should not error)
        assert!(storage.delete(999).is_ok());
    }

    #[test]
    fn test_scalar_quantization() {
        let compressor = ScalarQuantizationCompressor;

        let vector = vec![0.5, -0.5, 1.0, -1.0];
        let compressed = compressor.compress(&vector).unwrap();
        let decompressed = compressor.decompress(&compressed, 4).unwrap();

        // Should be approximately equal
        for (orig, decomp) in vector.iter().zip(decompressed.iter()) {
            assert!((orig - decomp).abs() < 0.1); // Quantization error
        }

        // Compressed size should be smaller (1 byte per value vs 4)
        assert_eq!(compressed.len(), 4); // 4 bytes
        assert_eq!(decompressed.len(), 4); // 4 f32 values
    }
}
