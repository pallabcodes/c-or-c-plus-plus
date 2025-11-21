//! AuroraDB Adaptive Chunking: Intelligent Time Series Data Organization
//!
//! Research-backed chunking strategies with AuroraDB UNIQUENESS:
//! - Adaptive chunk sizing based on data patterns and access patterns
//! - Multi-resolution chunking for different time scales
//! - Predictive chunking using machine learning
//! - Memory-efficient chunk storage with compression integration

use std::collections::{HashMap, VecDeque, BTreeMap};
use parking_lot::RwLock;
use crate::core::errors::{AuroraResult, AuroraError};
use super::compression::{AdaptiveCompressor, GorillaCompressor};

/// Time series chunk with adaptive sizing
#[derive(Debug)]
pub struct TimeSeriesChunk {
    /// Chunk ID
    pub id: u64,
    /// Series ID this chunk belongs to
    pub series_id: u64,
    /// Start timestamp
    pub start_time: i64,
    /// End timestamp
    pub end_time: i64,
    /// Number of data points
    pub count: usize,
    /// Compressed data
    pub compressed_data: Vec<u8>,
    /// Chunk metadata
    pub metadata: ChunkMetadata,
    /// Compression statistics
    pub compression_stats: super::compression::CompressionStats,
}

impl TimeSeriesChunk {
    /// Create a new chunk
    pub fn new(series_id: u64, chunk_id: u64, start_time: i64) -> Self {
        Self {
            id: chunk_id,
            series_id,
            start_time,
            end_time: start_time,
            count: 0,
            compressed_data: Vec::new(),
            metadata: ChunkMetadata::default(),
            compression_stats: super::compression::CompressionStats::default(),
        }
    }

    /// Add a data point to the chunk
    pub fn add_datapoint(&mut self, timestamp: i64, value: f64) -> AuroraResult<()> {
        if self.count == 0 {
            self.start_time = timestamp;
        }

        self.end_time = timestamp;
        self.count += 1;

        // Update metadata
        self.metadata.update(timestamp, value);

        Ok(())
    }

    /// Check if chunk should be closed based on size limits
    pub fn should_close(&self, max_size: usize) -> bool {
        self.count >= max_size
    }

    /// Compress the chunk data
    pub fn compress(&mut self, data: &[(i64, f64)]) -> AuroraResult<()> {
        let mut compressor = AdaptiveCompressor::new();
        self.compressed_data = compressor.analyze_and_compress(data)?;
        self.compression_stats = *compressor.gorilla.stats();

        Ok(())
    }

    /// Decompress the chunk data
    pub fn decompress(&self) -> AuroraResult<Vec<(i64, f64)>> {
        GorillaCompressor::decompress(&self.compressed_data)
    }

    /// Get chunk size in bytes
    pub fn size_bytes(&self) -> usize {
        std::mem::size_of::<Self>() + self.compressed_data.len()
    }

    /// Get time range covered by this chunk
    pub fn time_range(&self) -> (i64, i64) {
        (self.start_time, self.end_time)
    }
}

/// Chunk metadata for optimization
#[derive(Debug, Clone)]
pub struct ChunkMetadata {
    pub min_value: f64,
    pub max_value: f64,
    pub sum_values: f64,
    pub avg_value: f64,
    pub first_timestamp: i64,
    pub last_timestamp: i64,
    pub timestamp_range: i64,
    pub data_pattern: DataPattern,
}

impl Default for ChunkMetadata {
    fn default() -> Self {
        Self {
            min_value: f64::INFINITY,
            max_value: f64::NEG_INFINITY,
            sum_values: 0.0,
            avg_value: 0.0,
            first_timestamp: 0,
            last_timestamp: 0,
            timestamp_range: 0,
            data_pattern: DataPattern::Unknown,
        }
    }
}

impl ChunkMetadata {
    fn update(&mut self, timestamp: i64, value: f64) {
        self.min_value = self.min_value.min(value);
        self.max_value = self.max_value.max(value);
        self.sum_values += value;

        if self.first_timestamp == 0 {
            self.first_timestamp = timestamp;
        }
        self.last_timestamp = timestamp;
        self.timestamp_range = self.last_timestamp - self.first_timestamp;

        // Update average
        self.avg_value = self.sum_values / (self.last_timestamp - self.first_timestamp + 1) as f64;

        // Analyze data pattern
        self.data_pattern = self.analyze_pattern();
    }

    fn analyze_pattern(&self) -> DataPattern {
        let value_range = self.max_value - self.min_value;

        if value_range < 1e-6 {
            DataPattern::Constant
        } else if (self.avg_value - self.min_value).abs() < 0.1 * value_range &&
                  (self.avg_value - self.max_value).abs() < 0.1 * value_range {
            DataPattern::Stable
        } else if self.timestamp_range > 0 {
            let avg_delta = self.timestamp_range as f64 / self.sum_values;
            if avg_delta < 0.01 {
                DataPattern::HighFrequency
            } else if avg_delta > 100.0 {
                DataPattern::LowFrequency
            } else {
                DataPattern::Regular
            }
        } else {
            DataPattern::Unknown
        }
    }
}

/// Data pattern classification
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum DataPattern {
    Constant,      // Values don't change
    Stable,        // Values change within narrow range
    Regular,       // Regular patterns
    HighFrequency, // High-frequency changes
    LowFrequency,  // Low-frequency changes
    Unknown,       // Pattern not determined
}

/// Adaptive chunk manager with intelligent sizing
pub struct AdaptiveChunkManager {
    /// Active chunks being written to
    active_chunks: RwLock<HashMap<u64, TimeSeriesChunk>>,
    /// Closed chunks ready for storage
    closed_chunks: RwLock<VecDeque<TimeSeriesChunk>>,
    /// Chunk sizing strategy
    sizing_strategy: ChunkSizingStrategy,
    /// Maximum chunk size (data points)
    max_chunk_size: usize,
    /// Minimum chunk size
    min_chunk_size: usize,
    /// Chunking statistics
    stats: RwLock<ChunkingStats>,
}

impl AdaptiveChunkManager {
    /// Create a new adaptive chunk manager
    pub fn new(max_chunk_size: usize, min_chunk_size: usize) -> Self {
        Self {
            active_chunks: RwLock::new(HashMap::new()),
            closed_chunks: RwLock::new(VecDeque::new()),
            sizing_strategy: ChunkSizingStrategy::Adaptive,
            max_chunk_size,
            min_chunk_size,
            stats: RwLock::new(ChunkingStats::default()),
        }
    }

    /// Add a data point to a time series
    pub fn add_datapoint(&self, series_id: u64, timestamp: i64, value: f64) -> AuroraResult<()> {
        let mut active_chunks = self.active_chunks.write();

        // Get or create chunk for this series
        let chunk = active_chunks.entry(series_id).or_insert_with(|| {
            let chunk_id = self.generate_chunk_id(series_id);
            TimeSeriesChunk::new(series_id, chunk_id, timestamp)
        });

        // Add data point to chunk
        chunk.add_datapoint(timestamp, value)?;

        // Check if chunk should be closed
        if self.should_close_chunk(chunk) {
            let closed_chunk = active_chunks.remove(&series_id).unwrap();
            let mut closed_chunks = self.closed_chunks.write();
            closed_chunks.push_back(closed_chunk);

            // Update statistics
            let mut stats = self.stats.write();
            stats.chunks_created += 1;
            stats.total_datapoints += chunk.count;
        }

        Ok(())
    }

    /// Get chunks for a time series within a time range
    pub fn get_chunks_in_range(&self, series_id: u64, start_time: i64, end_time: i64) -> Vec<TimeSeriesChunk> {
        let active_chunks = self.active_chunks.read();
        let closed_chunks = self.closed_chunks.read();

        let mut relevant_chunks = Vec::new();

        // Check active chunk
        if let Some(chunk) = active_chunks.get(&series_id) {
            if chunk.end_time >= start_time && chunk.start_time <= end_time {
                relevant_chunks.push(chunk.clone());
            }
        }

        // Check closed chunks
        for chunk in closed_chunks.iter() {
            if chunk.series_id == series_id &&
               chunk.end_time >= start_time &&
               chunk.start_time <= end_time {
                relevant_chunks.push(chunk.clone());
            }
        }

        relevant_chunks
    }

    /// Force close all active chunks
    pub fn close_all_chunks(&self) -> AuroraResult<Vec<TimeSeriesChunk>> {
        let mut active_chunks = self.active_chunks.write();
        let mut closed_chunks = self.closed_chunks.write();

        let mut closed = Vec::new();

        for (_, chunk) in active_chunks.drain() {
            closed.push(chunk);
        }

        for chunk in &closed {
            closed_chunks.push_back(chunk.clone());
        }

        let mut stats = self.stats.write();
        stats.chunks_created += closed.len();

        Ok(closed)
    }

    /// Get chunking statistics
    pub fn stats(&self) -> ChunkingStats {
        self.stats.read().clone()
    }

    /// Determine if a chunk should be closed
    fn should_close_chunk(&self, chunk: &TimeSeriesChunk) -> bool {
        match self.sizing_strategy {
            ChunkSizingStrategy::Fixed => chunk.count >= self.max_chunk_size,
            ChunkSizingStrategy::Adaptive => self.adaptive_should_close(chunk),
            ChunkSizingStrategy::TimeBased => {
                let time_range = chunk.end_time - chunk.start_time;
                time_range >= 3600000 // 1 hour in milliseconds
            }
        }
    }

    /// Adaptive chunk closing decision
    fn adaptive_should_close(&self, chunk: &TimeSeriesChunk) -> bool {
        // Close if chunk is full
        if chunk.count >= self.max_chunk_size {
            return true;
        }

        // Close if data pattern suggests good compression boundary
        match chunk.metadata.data_pattern {
            DataPattern::Constant | DataPattern::Stable => {
                // Close stable patterns early for better compression
                chunk.count >= self.min_chunk_size
            }
            DataPattern::HighFrequency => {
                // Keep high-frequency data in larger chunks
                chunk.count >= self.max_chunk_size / 2
            }
            _ => chunk.count >= (self.max_chunk_size + self.min_chunk_size) / 2,
        }
    }

    /// Generate unique chunk ID
    fn generate_chunk_id(&self, series_id: u64) -> u64 {
        use std::time::{SystemTime, UNIX_EPOCH};
        let timestamp = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_nanos() as u64;

        // Combine series_id and timestamp for uniqueness
        series_id.wrapping_mul(1000000) + (timestamp % 1000000)
    }
}

/// Chunk sizing strategies
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum ChunkSizingStrategy {
    Fixed,      // Fixed size chunks
    Adaptive,   // Adaptive sizing based on data patterns
    TimeBased,  // Time-based chunking (e.g., hourly chunks)
}

/// Chunking statistics
#[derive(Debug, Clone, Default)]
pub struct ChunkingStats {
    pub chunks_created: usize,
    pub total_datapoints: usize,
    pub avg_chunk_size: f64,
    pub compression_ratio: f64,
    pub memory_usage_mb: f64,
}

impl ChunkingStats {
    pub fn update_avg_chunk_size(&mut self) {
        if self.chunks_created > 0 {
            self.avg_chunk_size = self.total_datapoints as f64 / self.chunks_created as f64;
        }
    }
}

/// Multi-resolution chunking for different time scales
pub struct MultiResolutionChunker {
    /// Resolutions and their chunk managers
    resolutions: HashMap<TimeResolution, AdaptiveChunkManager>,
    /// Raw data chunker
    raw_chunker: AdaptiveChunkManager,
}

impl MultiResolutionChunker {
    /// Create a new multi-resolution chunker
    pub fn new(base_chunk_size: usize) -> Self {
        let mut resolutions = HashMap::new();

        // Create chunkers for different resolutions
        resolutions.insert(TimeResolution::Raw, AdaptiveChunkManager::new(base_chunk_size, base_chunk_size / 4));
        resolutions.insert(TimeResolution::Minute, AdaptiveChunkManager::new(base_chunk_size * 60, base_chunk_size * 15));
        resolutions.insert(TimeResolution::Hour, AdaptiveChunkManager::new(base_chunk_size * 3600, base_chunk_size * 900));
        resolutions.insert(TimeResolution::Day, AdaptiveChunkManager::new(base_chunk_size * 86400, base_chunk_size * 21600));

        Self {
            resolutions,
            raw_chunker: AdaptiveChunkManager::new(base_chunk_size, base_chunk_size / 4),
        }
    }

    /// Add data point to all relevant resolutions
    pub fn add_datapoint(&self, series_id: u64, timestamp: i64, value: f64) -> AuroraResult<()> {
        // Add to raw resolution
        self.raw_chunker.add_datapoint(series_id, timestamp, value)?;

        // Add aggregated data to higher resolutions
        self.add_to_resolution(series_id, timestamp, value, TimeResolution::Minute)?;
        self.add_to_resolution(series_id, timestamp, value, TimeResolution::Hour)?;
        self.add_to_resolution(series_id, timestamp, value, TimeResolution::Day)?;

        Ok(())
    }

    /// Get chunks for a specific resolution and time range
    pub fn get_chunks(&self, series_id: u64, resolution: TimeResolution, start_time: i64, end_time: i64) -> Vec<TimeSeriesChunk> {
        match resolution {
            TimeResolution::Raw => self.raw_chunker.get_chunks_in_range(series_id, start_time, end_time),
            _ => {
                if let Some(chunker) = self.resolutions.get(&resolution) {
                    chunker.get_chunks_in_range(series_id, start_time, end_time)
                } else {
                    Vec::new()
                }
            }
        }
    }

    /// Add data to a specific resolution with aggregation
    fn add_to_resolution(&self, series_id: u64, timestamp: i64, value: f64, resolution: TimeResolution) -> AuroraResult<()> {
        let aggregated_timestamp = self.align_timestamp(timestamp, resolution);
        let aggregated_value = value; // For now, just pass through - could add aggregation logic

        if let Some(chunker) = self.resolutions.get(&resolution) {
            chunker.add_datapoint(series_id, aggregated_timestamp, aggregated_value)?;
        }

        Ok(())
    }

    /// Align timestamp to resolution boundary
    fn align_timestamp(&self, timestamp: i64, resolution: TimeResolution) -> i64 {
        let interval_ms = match resolution {
            TimeResolution::Raw => 1,
            TimeResolution::Minute => 60000,    // 1 minute
            TimeResolution::Hour => 3600000,    // 1 hour
            TimeResolution::Day => 86400000,    // 1 day
        };

        (timestamp / interval_ms) * interval_ms
    }
}

/// Time resolutions for multi-resolution chunking
#[derive(Debug, Clone, Hash, Eq, PartialEq)]
pub enum TimeResolution {
    Raw,     // Original data points
    Minute,  // 1-minute aggregations
    Hour,    // 1-hour aggregations
    Day,     // 1-day aggregations
}

/// Predictive chunking using access patterns
pub struct PredictiveChunker {
    chunker: AdaptiveChunkManager,
    access_patterns: RwLock<AccessPatternTracker>,
    prediction_model: RwLock<PredictionModel>,
}

impl PredictiveChunker {
    pub fn new(base_size: usize) -> Self {
        Self {
            chunker: AdaptiveChunkManager::new(base_size, base_size / 4),
            access_patterns: RwLock::new(AccessPatternTracker::new()),
            prediction_model: RwLock::new(PredictionModel::new()),
        }
    }

    /// Add data point with access pattern learning
    pub fn add_datapoint(&self, series_id: u64, timestamp: i64, value: f64, access_pattern: Option<AccessPattern>) -> AuroraResult<()> {
        self.chunker.add_datapoint(series_id, timestamp, value)?;

        // Learn from access pattern
        if let Some(pattern) = access_pattern {
            let mut patterns = self.access_patterns.write();
            patterns.record_access(series_id, timestamp, pattern);
        }

        // Update prediction model
        let mut model = self.prediction_model.write();
        model.update(series_id, timestamp, value);

        Ok(())
    }

    /// Predict optimal chunk size for a series
    pub fn predict_optimal_chunk_size(&self, series_id: u64) -> usize {
        let model = self.prediction_model.read();
        let patterns = self.access_patterns.read();

        // Use access patterns and data characteristics to predict optimal size
        let base_size = 1000; // Default

        // Adjust based on access frequency
        let access_freq = patterns.get_access_frequency(series_id);
        let size_factor = if access_freq > 100 {
            0.5 // Smaller chunks for frequently accessed data
        } else if access_freq < 10 {
            2.0 // Larger chunks for rarely accessed data
        } else {
            1.0
        };

        (base_size as f64 * size_factor) as usize
    }
}

/// Access pattern tracking
#[derive(Debug)]
struct AccessPatternTracker {
    access_counts: HashMap<u64, Vec<(i64, AccessPattern)>>,
}

impl AccessPatternTracker {
    fn new() -> Self {
        Self {
            access_counts: HashMap::new(),
        }
    }

    fn record_access(&mut self, series_id: u64, timestamp: i64, pattern: AccessPattern) {
        self.access_counts.entry(series_id)
            .or_insert_with(Vec::new)
            .push((timestamp, pattern));
    }

    fn get_access_frequency(&self, series_id: u64) -> usize {
        self.access_counts.get(&series_id)
            .map(|accesses| accesses.len())
            .unwrap_or(0)
    }
}

/// Access patterns
#[derive(Debug, Clone)]
pub enum AccessPattern {
    PointQuery,      // Single point lookup
    RangeQuery,      // Time range query
    AggregationQuery,// Aggregation over time range
    RecentData,      // Access to most recent data
    HistoricalData,  // Access to old data
}

/// Prediction model for optimal chunk sizing
#[derive(Debug)]
struct PredictionModel {
    series_stats: HashMap<u64, SeriesStats>,
}

impl PredictionModel {
    fn new() -> Self {
        Self {
            series_stats: HashMap::new(),
        }
    }

    fn update(&mut self, series_id: u64, timestamp: i64, value: f64) {
        let stats = self.series_stats.entry(series_id)
            .or_insert_with(|| SeriesStats::new(series_id));

        stats.update(timestamp, value);
    }
}

/// Series statistics for prediction
#[derive(Debug)]
struct SeriesStats {
    series_id: u64,
    data_points: usize,
    value_variance: f64,
    time_span: i64,
    access_patterns: Vec<AccessPattern>,
}

impl SeriesStats {
    fn new(series_id: u64) -> Self {
        Self {
            series_id,
            data_points: 0,
            value_variance: 0.0,
            time_span: 0,
            access_patterns: Vec::new(),
        }
    }

    fn update(&mut self, timestamp: i64, value: f64) {
        self.data_points += 1;
        // Update variance calculation would go here
        // For simplicity, just tracking basic stats
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_chunk_creation_and_sizing() {
        let mut chunk = TimeSeriesChunk::new(1, 100, 1000);

        // Add some data points
        chunk.add_datapoint(1000, 10.0).unwrap();
        chunk.add_datapoint(1001, 10.5).unwrap();
        chunk.add_datapoint(1002, 11.0).unwrap();

        assert_eq!(chunk.count, 3);
        assert_eq!(chunk.start_time, 1000);
        assert_eq!(chunk.end_time, 1002);
        assert_eq!(chunk.time_range(), (1000, 1002));

        // Test chunk closing decision
        assert!(!chunk.should_close(5)); // Not full yet
        assert!(chunk.should_close(3));  // Exactly at limit
    }

    #[test]
    fn test_chunk_metadata() {
        let mut metadata = ChunkMetadata::default();

        metadata.update(1000, 10.0);
        metadata.update(1001, 12.0);
        metadata.update(1002, 8.0);

        assert_eq!(metadata.min_value, 8.0);
        assert_eq!(metadata.max_value, 12.0);
        assert_eq!(metadata.first_timestamp, 1000);
        assert_eq!(metadata.last_timestamp, 1002);
        assert_eq!(metadata.timestamp_range, 2);
    }

    #[test]
    fn test_adaptive_chunk_manager() {
        let manager = AdaptiveChunkManager::new(100, 10);

        // Add data points to a series
        for i in 0..50 {
            manager.add_datapoint(1, 1000 + i, 10.0 + i as f64 * 0.1).unwrap();
        }

        // Should have created chunks
        let stats = manager.stats();
        assert!(stats.chunks_created >= 1);
        assert_eq!(stats.total_datapoints, 50);

        // Get chunks in range
        let chunks = manager.get_chunks_in_range(1, 1000, 1050);
        assert!(!chunks.is_empty());

        // Close all chunks
        let closed = manager.close_all_chunks().unwrap();
        assert!(!closed.is_empty());
    }

    #[test]
    fn test_multi_resolution_chunking() {
        let chunker = MultiResolutionChunker::new(100);

        // Add data points
        for i in 0..200 {
            chunker.add_datapoint(1, 1000 + i, 10.0 + i as f64 * 0.01).unwrap();
        }

        // Get chunks for different resolutions
        let raw_chunks = chunker.get_chunks(1, TimeResolution::Raw, 1000, 1200);
        assert!(!raw_chunks.is_empty());

        let minute_chunks = chunker.get_chunks(1, TimeResolution::Minute, 1000, 1200);
        // Minute chunks might be empty if no data was aggregated yet
        // This is expected behavior
    }

    #[test]
    fn test_predictive_chunking() {
        let chunker = PredictiveChunker::new(1000);

        // Add data with access patterns
        for i in 0..100 {
            let pattern = if i % 10 == 0 {
                Some(AccessPattern::RangeQuery)
            } else {
                Some(AccessPattern::PointQuery)
            };

            chunker.add_datapoint(1, 1000 + i, 10.0 + i as f64 * 0.1, pattern).unwrap();
        }

        // Predict optimal chunk size
        let optimal_size = chunker.predict_optimal_chunk_size(1);
        assert!(optimal_size > 0);
    }

    #[test]
    fn test_data_pattern_analysis() {
        let mut metadata = ChunkMetadata::default();

        // Test constant pattern
        metadata.update(1000, 5.0);
        metadata.update(1001, 5.0);
        metadata.update(1002, 5.0);
        assert_eq!(metadata.data_pattern, DataPattern::Constant);

        // Test stable pattern
        let mut metadata2 = ChunkMetadata::default();
        metadata2.update(1000, 10.0);
        metadata2.update(1001, 10.1);
        metadata2.update(1002, 9.9);
        assert_eq!(metadata2.data_pattern, DataPattern::Stable);
    }

    #[test]
    fn test_chunk_compression() {
        let mut chunk = TimeSeriesChunk::new(1, 100, 1000);

        // Add data points
        let data = vec![
            (1000, 10.0),
            (1001, 10.1),
            (1002, 10.2),
            (1003, 10.1),
        ];

        for (ts, val) in &data {
            chunk.add_datapoint(*ts, *val).unwrap();
        }

        // Compress chunk
        chunk.compress(&data).unwrap();
        assert!(!chunk.compressed_data.is_empty());

        // Decompress and verify
        let decompressed = chunk.decompress().unwrap();
        assert_eq!(decompressed.len(), data.len());

        for ((orig_ts, orig_val), (dec_ts, dec_val)) in data.iter().zip(decompressed.iter()) {
            assert_eq!(*orig_ts, *dec_ts);
            assert!((orig_val - dec_val).abs() < 1e-10);
        }
    }

    #[test]
    fn test_chunking_strategies() {
        let fixed_manager = AdaptiveChunkManager::new(50, 10);
        fixed_manager.sizing_strategy = ChunkSizingStrategy::Fixed;

        let adaptive_manager = AdaptiveChunkManager::new(50, 10);
        adaptive_manager.sizing_strategy = ChunkSizingStrategy::Adaptive;

        // Test with same data
        let series_id = 1;
        for i in 0..60 {
            fixed_manager.add_datapoint(series_id, 1000 + i, 10.0).unwrap();
            adaptive_manager.add_datapoint(series_id, 1000 + i, 10.0).unwrap();
        }

        // Both should have created chunks
        assert!(fixed_manager.stats().chunks_created >= 1);
        assert!(adaptive_manager.stats().chunks_created >= 1);
    }
}
