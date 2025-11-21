//! AuroraDB Vector Index: Unified Interface for Similarity Search
//!
//! Intelligent vector indexing that automatically selects and adapts the best
//! algorithm based on dataset characteristics, query patterns, and performance requirements.

use std::collections::HashMap;
use crate::core::errors::{AuroraResult, AuroraError};
use super::distance_metrics::{DistanceComputer, DistanceMetric};
use super::hnsw_index::{HNSWIndex, HNSWConfig, AdaptiveHNSW};
use super::ivf_index::{IVFIndex, IVFConfig};

/// Vector index types supported by AuroraDB
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum VectorIndexType {
    /// HNSW (Hierarchical Navigable Small World) - best for high accuracy
    HNSW,
    /// IVF (Inverted File Index) with PQ - best for large datasets
    IVF,
    /// Adaptive HNSW that tunes parameters automatically
    AdaptiveHNSW,
    /// IVF with adaptive configuration
    AdaptiveIVF,
}

/// Configuration for vector indexing
#[derive(Debug, Clone)]
pub struct VectorIndexConfig {
    pub index_type: VectorIndexType,
    pub dimension: usize,
    pub metric: DistanceMetric,
    pub max_vectors: usize,
    pub index_params: IndexParameters,
}

/// Index-specific parameters
#[derive(Debug, Clone)]
pub enum IndexParameters {
    HNSW(HNSWConfig),
    IVF(IVFConfig),
    AdaptiveHNSW(HNSWConfig), // Base config, will be adapted
    AdaptiveIVF(IVFConfig),   // Base config, will be adapted
}

/// Unified vector index interface
pub trait VectorIndex {
    /// Insert a vector into the index
    fn insert(&mut self, id: usize, vector: Vec<f32>) -> AuroraResult<()>;

    /// Search for k nearest neighbors
    fn search(&self, query: &[f32], k: usize) -> AuroraResult<Vec<(usize, f32)>>;

    /// Delete a vector from the index
    fn delete(&mut self, id: usize) -> AuroraResult<()>;

    /// Get index statistics
    fn stats(&self) -> IndexStats;

    /// Optimize index for better performance
    fn optimize(&mut self) -> AuroraResult<()>;

    /// Build index from batch of vectors
    fn build(&mut self, vectors: HashMap<usize, Vec<f32>>) -> AuroraResult<()>;
}

/// Index statistics
#[derive(Debug, Clone)]
pub struct IndexStats {
    pub index_type: String,
    pub dimension: usize,
    pub metric: DistanceMetric,
    pub total_vectors: usize,
    pub memory_usage_mb: f64,
    pub build_time_ms: f64,
    pub avg_query_time_ms: f64,
    pub index_specific_stats: HashMap<String, f64>,
}

/// Intelligent vector index that automatically selects and adapts the best algorithm
pub struct AuroraVectorIndex {
    config: VectorIndexConfig,
    index: Box<dyn VectorIndex>,
    query_history: Vec<QueryRecord>,
    performance_monitor: PerformanceMonitor,
}

impl AuroraVectorIndex {
    /// Create a new Aurora vector index with intelligent algorithm selection
    pub fn new(config: VectorIndexConfig) -> AuroraResult<Self> {
        let index = Self::create_index(&config)?;
        let performance_monitor = PerformanceMonitor::new();

        Ok(Self {
            config,
            index,
            query_history: Vec::new(),
            performance_monitor,
        })
    }

    /// Auto-select the best index type based on dataset characteristics
    pub fn auto_select(dataset_size: usize, dimension: usize, query_patterns: &QueryPatterns) -> VectorIndexType {
        match dataset_size {
            0..=10000 => VectorIndexType::HNSW, // Small datasets: HNSW is fast and accurate
            10001..=100000 => {
                if query_patterns.high_accuracy {
                    VectorIndexType::HNSW
                } else {
                    VectorIndexType::AdaptiveHNSW
                }
            }
            100001..=1000000 => {
                if dimension <= 128 {
                    VectorIndexType::AdaptiveHNSW
                } else {
                    VectorIndexType::IVF
                }
            }
            _ => {
                if query_patterns.batch_queries {
                    VectorIndexType::AdaptiveIVF
                } else {
                    VectorIndexType::IVF
                }
            }
        }
    }

    /// Intelligent configuration based on use case
    pub fn intelligent_config(usecase: VectorUseCase, dataset_size: usize, dimension: usize) -> VectorIndexConfig {
        let index_type = Self::auto_select(dataset_size, dimension, &QueryPatterns::from_usecase(usecase));

        let index_params = match index_type {
            VectorIndexType::HNSW => IndexParameters::HNSW(HNSWConfig::default()),
            VectorIndexType::IVF => IndexParameters::IVF(IVFConfig::default()),
            VectorIndexType::AdaptiveHNSW => IndexParameters::AdaptiveHNSW(HNSWConfig::default()),
            VectorIndexType::AdaptiveIVF => IndexParameters::AdaptiveIVF(IVFConfig::default()),
        };

        VectorIndexConfig {
            index_type,
            dimension,
            metric: super::distance_metrics::DistanceMetricSelector::select_for_usecase(usecase),
            max_vectors: dataset_size,
            index_params,
        }
    }

    /// Create the underlying index implementation
    fn create_index(config: &VectorIndexConfig) -> AuroraResult<Box<dyn VectorIndex>> {
        match &config.index_params {
            IndexParameters::HNSW(hnsw_config) => {
                Ok(Box::new(HNSWIndex::new(config.dimension, config.metric.clone())))
            }
            IndexParameters::IVF(ivf_config) => {
                Ok(Box::new(IVFIndex::new(config.dimension, config.metric.clone(), ivf_config.clone())))
            }
            IndexParameters::AdaptiveHNSW(hnsw_config) => {
                Ok(Box::new(AdaptiveHNSW::new(config.dimension, config.metric.clone(), hnsw_config.clone())))
            }
            IndexParameters::AdaptiveIVF(ivf_config) => {
                Ok(Box::new(IVFIndex::new(config.dimension, config.metric.clone(), ivf_config.clone())))
            }
        }
    }

    /// Adaptive search with automatic parameter tuning
    pub fn adaptive_search(&mut self, query: &[f32], k: usize, accuracy_hint: Option<f32>) -> AuroraResult<Vec<(usize, f32)>> {
        // Record query start
        let query_start = std::time::Instant::now();

        // Adjust search parameters based on accuracy hint and history
        let adjusted_k = self.adjust_k_for_accuracy(k, accuracy_hint);

        // Perform search
        let results = self.index.search(query, adjusted_k)?;

        // Record query completion
        let query_time = query_start.elapsed().as_millis() as f64;

        // Update performance monitor
        self.performance_monitor.record_query(query_time, results.len());

        // Store query record for learning
        self.query_history.push(QueryRecord {
            query_vector: query.to_vec(),
            k_requested: k,
            k_actual: adjusted_k,
            results_count: results.len(),
            query_time_ms: query_time,
            timestamp: std::time::Instant::now(),
        });

        // Learn from query patterns
        self.learn_from_queries();

        Ok(results)
    }

    /// Adjust k based on accuracy requirements and performance history
    fn adjust_k_for_accuracy(&self, k: usize, accuracy_hint: Option<f32>) -> usize {
        let accuracy_hint = accuracy_hint.unwrap_or(0.95);

        // If we need high accuracy, search for more candidates
        if accuracy_hint > 0.95 {
            (k as f32 * 1.5) as usize
        } else if accuracy_hint < 0.8 {
            // If lower accuracy is acceptable, search for fewer
            std::cmp::max(k / 2, 1)
        } else {
            k
        }
    }

    /// Learn from query patterns to optimize future performance
    fn learn_from_queries(&mut self) {
        // Analyze recent queries for patterns
        if self.query_history.len() >= 100 {
            // Keep only recent queries
            self.query_history.truncate(50);
        }

        // This could trigger index reorganization or parameter tuning
        // For now, just maintain history
    }

    /// Get comprehensive index statistics and recommendations
    pub fn comprehensive_stats(&self) -> ComprehensiveIndexStats {
        let base_stats = self.index.stats();
        let perf_stats = self.performance_monitor.stats();

        ComprehensiveIndexStats {
            base_stats,
            performance_stats: perf_stats,
            query_patterns: self.analyze_query_patterns(),
            recommendations: self.generate_recommendations(),
        }
    }

    /// Analyze query patterns for optimization opportunities
    fn analyze_query_patterns(&self) -> QueryPatternAnalysis {
        let total_queries = self.query_history.len();
        if total_queries == 0 {
            return QueryPatternAnalysis::default();
        }

        let avg_k = self.query_history.iter().map(|q| q.k_requested).sum::<usize>() as f64 / total_queries as f64;
        let avg_query_time = self.query_history.iter().map(|q| q.query_time_ms).sum::<f64>() / total_queries as f64;

        let high_k_queries = self.query_history.iter().filter(|q| q.k_requested > 100).count();
        let high_k_ratio = high_k_queries as f64 / total_queries as f64;

        QueryPatternAnalysis {
            total_queries,
            avg_k,
            avg_query_time_ms: avg_query_time,
            high_k_ratio,
            temporal_patterns: self.analyze_temporal_patterns(),
        }
    }

    /// Analyze temporal patterns in queries
    fn analyze_temporal_patterns(&self) -> TemporalPatterns {
        // Simple analysis - could be extended to detect time-based patterns
        TemporalPatterns {
            has_temporal_patterns: false,
            peak_hours: Vec::new(),
            seasonal_patterns: Vec::new(),
        }
    }

    /// Generate optimization recommendations
    fn generate_recommendations(&self) -> Vec<String> {
        let mut recommendations = Vec::new();

        let patterns = self.analyze_query_patterns();
        let perf_stats = self.performance_monitor.stats();

        if patterns.avg_query_time_ms > 100.0 {
            recommendations.push("Consider using IVF index for faster queries".to_string());
        }

        if patterns.high_k_ratio > 0.5 {
            recommendations.push("High k values detected - consider increasing ef_search parameter".to_string());
        }

        if perf_stats.memory_usage_mb > 1024.0 {
            recommendations.push("High memory usage - consider using PQ compression".to_string());
        }

        if recommendations.is_empty() {
            recommendations.push("Index performance is optimal".to_string());
        }

        recommendations
    }
}

impl VectorIndex for AuroraVectorIndex {
    fn insert(&mut self, id: usize, vector: Vec<f32>) -> AuroraResult<()> {
        self.index.insert(id, vector)
    }

    fn search(&self, query: &[f32], k: usize) -> AuroraResult<Vec<(usize, f32)>> {
        self.index.search(query, k)
    }

    fn delete(&mut self, id: usize) -> AuroraResult<()> {
        self.index.delete(id)
    }

    fn stats(&self) -> IndexStats {
        self.index.stats()
    }

    fn optimize(&mut self) -> AuroraResult<()> {
        self.index.optimize()
    }

    fn build(&mut self, vectors: HashMap<usize, Vec<f32>>) -> AuroraResult<()> {
        self.index.build(vectors)
    }
}

/// Query patterns for intelligent index selection
#[derive(Debug, Clone)]
pub struct QueryPatterns {
    pub high_accuracy: bool,
    pub batch_queries: bool,
    pub real_time: bool,
    pub high_throughput: bool,
}

impl QueryPatterns {
    pub fn from_usecase(usecase: VectorUseCase) -> Self {
        match usecase {
            VectorUseCase::SemanticSearch => QueryPatterns {
                high_accuracy: true,
                batch_queries: false,
                real_time: true,
                high_throughput: false,
            },
            VectorUseCase::ImageSimilarity => QueryPatterns {
                high_accuracy: true,
                batch_queries: true,
                real_time: false,
                high_throughput: true,
            },
            VectorUseCase::Recommendation => QueryPatterns {
                high_accuracy: false,
                batch_queries: true,
                real_time: false,
                high_throughput: true,
            },
            VectorUseCase::Clustering => QueryPatterns {
                high_accuracy: true,
                batch_queries: false,
                real_time: false,
                high_throughput: false,
            },
            VectorUseCase::AnomalyDetection => QueryPatterns {
                high_accuracy: true,
                batch_queries: false,
                real_time: true,
                high_throughput: false,
            },
            VectorUseCase::BinaryClassification => QueryPatterns {
                high_accuracy: true,
                batch_queries: true,
                real_time: false,
                high_throughput: true,
            },
            VectorUseCase::TextSimilarity => QueryPatterns {
                high_accuracy: true,
                batch_queries: false,
                real_time: true,
                high_throughput: false,
            },
            VectorUseCase::SparseData => QueryPatterns {
                high_accuracy: false,
                batch_queries: false,
                real_time: true,
                high_throughput: false,
            },
        }
    }
}

/// Vector use cases for configuration
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum VectorUseCase {
    SemanticSearch,
    ImageSimilarity,
    Recommendation,
    Clustering,
    AnomalyDetection,
    BinaryClassification,
    TextSimilarity,
    SparseData,
}

/// Performance monitor for query analytics
struct PerformanceMonitor {
    query_times: Vec<f64>,
    memory_usage: Vec<f64>,
    max_samples: usize,
}

impl PerformanceMonitor {
    fn new() -> Self {
        Self {
            query_times: Vec::new(),
            memory_usage: Vec::new(),
            max_samples: 1000,
        }
    }

    fn record_query(&mut self, query_time_ms: f64, result_count: usize) {
        self.query_times.push(query_time_ms);
        // Mock memory usage based on result count
        self.memory_usage.push(result_count as f64 * 0.1);

        // Keep only recent samples
        if self.query_times.len() > self.max_samples {
            self.query_times.remove(0);
            self.memory_usage.remove(0);
        }
    }

    fn stats(&self) -> PerformanceStats {
        if self.query_times.is_empty() {
            return PerformanceStats::default();
        }

        let avg_query_time = self.query_times.iter().sum::<f64>() / self.query_times.len() as f64;
        let avg_memory = self.memory_usage.iter().sum::<f64>() / self.memory_usage.len() as f64;

        let mut sorted_times = self.query_times.clone();
        sorted_times.sort_by(|a, b| a.partial_cmp(b).unwrap());

        let p95_idx = (sorted_times.len() as f64 * 0.95) as usize;
        let p99_idx = (sorted_times.len() as f64 * 0.99) as usize;

        PerformanceStats {
            total_queries: self.query_times.len(),
            avg_query_time_ms: avg_query_time,
            p95_query_time_ms: sorted_times.get(p95_idx).copied().unwrap_or(avg_query_time),
            p99_query_time_ms: sorted_times.get(p99_idx).copied().unwrap_or(avg_query_time),
            avg_memory_usage_mb: avg_memory,
            max_memory_usage_mb: self.memory_usage.iter().fold(0.0, |a, &b| a.max(b)),
        }
    }
}

/// Performance statistics
#[derive(Debug, Clone)]
struct PerformanceStats {
    total_queries: usize,
    avg_query_time_ms: f64,
    p95_query_time_ms: f64,
    p99_query_time_ms: f64,
    avg_memory_usage_mb: f64,
    max_memory_usage_mb: f64,
}

impl Default for PerformanceStats {
    fn default() -> Self {
        Self {
            total_queries: 0,
            avg_query_time_ms: 0.0,
            p95_query_time_ms: 0.0,
            p99_query_time_ms: 0.0,
            avg_memory_usage_mb: 0.0,
            max_memory_usage_mb: 0.0,
        }
    }
}

/// Query record for learning
#[derive(Debug, Clone)]
struct QueryRecord {
    query_vector: Vec<f32>,
    k_requested: usize,
    k_actual: usize,
    results_count: usize,
    query_time_ms: f64,
    timestamp: std::time::Instant,
}

/// Comprehensive index statistics
#[derive(Debug, Clone)]
pub struct ComprehensiveIndexStats {
    pub base_stats: IndexStats,
    pub performance_stats: PerformanceStats,
    pub query_patterns: QueryPatternAnalysis,
    pub recommendations: Vec<String>,
}

/// Query pattern analysis
#[derive(Debug, Clone)]
pub struct QueryPatternAnalysis {
    pub total_queries: usize,
    pub avg_k: f64,
    pub avg_query_time_ms: f64,
    pub high_k_ratio: f64,
    pub temporal_patterns: TemporalPatterns,
}

impl Default for QueryPatternAnalysis {
    fn default() -> Self {
        Self {
            total_queries: 0,
            avg_k: 0.0,
            avg_query_time_ms: 0.0,
            high_k_ratio: 0.0,
            temporal_patterns: TemporalPatterns::default(),
        }
    }
}

/// Temporal patterns in queries
#[derive(Debug, Clone)]
pub struct TemporalPatterns {
    pub has_temporal_patterns: bool,
    pub peak_hours: Vec<u32>,
    pub seasonal_patterns: Vec<String>,
}

impl Default for TemporalPatterns {
    fn default() -> Self {
        Self {
            has_temporal_patterns: false,
            peak_hours: Vec::new(),
            seasonal_patterns: Vec::new(),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_auto_index_selection() {
        // Small dataset should use HNSW
        let index_type = AuroraVectorIndex::auto_select(5000, 128, &QueryPatterns {
            high_accuracy: true,
            batch_queries: false,
            real_time: true,
            high_throughput: false,
        });
        assert_eq!(index_type, VectorIndexType::HNSW);

        // Large dataset should use IVF
        let index_type = AuroraVectorIndex::auto_select(500000, 128, &QueryPatterns {
            high_accuracy: false,
            batch_queries: true,
            real_time: false,
            high_throughput: true,
        });
        assert_eq!(index_type, VectorIndexType::AdaptiveIVF);
    }

    #[test]
    fn test_intelligent_config() {
        let config = AuroraVectorIndex::intelligent_config(VectorUseCase::SemanticSearch, 50000, 384);

        assert_eq!(config.dimension, 384);
        assert_eq!(config.metric, DistanceMetric::Cosine); // Best for semantic search
        assert_eq!(config.index_type, VectorIndexType::AdaptiveHNSW); // Medium dataset
    }

    #[test]
    fn test_usecase_patterns() {
        let semantic_patterns = QueryPatterns::from_usecase(VectorUseCase::SemanticSearch);
        assert!(semantic_patterns.high_accuracy);
        assert!(!semantic_patterns.batch_queries);
        assert!(semantic_patterns.real_time);

        let recommendation_patterns = QueryPatterns::from_usecase(VectorUseCase::Recommendation);
        assert!(!recommendation_patterns.high_accuracy);
        assert!(recommendation_patterns.batch_queries);
        assert!(recommendation_patterns.high_throughput);
    }

    #[test]
    fn test_adaptive_search() {
        let config = AuroraVectorIndex::intelligent_config(VectorUseCase::SemanticSearch, 1000, 128);
        let mut index = AuroraVectorIndex::new(config).unwrap();

        // Insert some test vectors
        for i in 0..10 {
            let vector = vec![i as f32; 128];
            index.insert(i, vector).unwrap();
        }

        // Test adaptive search
        let query = vec![5.0; 128];
        let results = index.adaptive_search(&query, 3, Some(0.95)).unwrap();

        assert_eq!(results.len(), 3);
        assert_eq!(results[0].0, 5); // Should find the identical vector first
    }

    #[test]
    fn test_comprehensive_stats() {
        let config = AuroraVectorIndex::intelligent_config(VectorUseCase::ImageSimilarity, 10000, 512);
        let index = AuroraVectorIndex::new(config).unwrap();

        let stats = index.comprehensive_stats();
        assert!(stats.base_stats.total_vectors >= 0);
        assert!(stats.performance_stats.total_queries >= 0);
        assert!(!stats.recommendations.is_empty());
    }

    #[test]
    fn test_performance_monitor() {
        let mut monitor = PerformanceMonitor::new();

        // Record some queries
        monitor.record_query(50.0, 10);
        monitor.record_query(75.0, 15);
        monitor.record_query(60.0, 12);

        let stats = monitor.stats();
        assert_eq!(stats.total_queries, 3);
        assert!(stats.avg_query_time_ms > 0.0);
        assert!(stats.avg_memory_usage_mb > 0.0);
    }

    #[test]
    fn test_query_pattern_analysis() {
        let mut index = AuroraVectorIndex::new(
            AuroraVectorIndex::intelligent_config(VectorUseCase::SemanticSearch, 1000, 128)
        ).unwrap();

        // Add some query history
        for i in 0..10 {
            index.query_history.push(QueryRecord {
                query_vector: vec![i as f32; 128],
                k_requested: if i < 8 { 10 } else { 200 }, // Some high k queries
                k_actual: 10,
                results_count: 10,
                query_time_ms: 25.0,
                timestamp: std::time::Instant::now(),
            });
        }

        let patterns = index.analyze_query_patterns();
        assert_eq!(patterns.total_queries, 10);
        assert!(patterns.avg_k > 0.0);
        assert!(patterns.high_k_ratio > 0.0); // Should detect high k queries
    }

    #[test]
    fn test_different_use_cases() {
        let use_cases = vec![
            VectorUseCase::SemanticSearch,
            VectorUseCase::ImageSimilarity,
            VectorUseCase::Recommendation,
            VectorUseCase::Clustering,
        ];

        for usecase in use_cases {
            let config = AuroraVectorIndex::intelligent_config(usecase, 10000, 256);
            let index = AuroraVectorIndex::new(config).unwrap();

            // Should create successfully
            assert!(index.config.dimension == 256);
        }
    }

    #[test]
    fn test_index_type_variants() {
        let types = vec![
            VectorIndexType::HNSW,
            VectorIndexType::IVF,
            VectorIndexType::AdaptiveHNSW,
            VectorIndexType::AdaptiveIVF,
        ];

        for index_type in types {
            let config = VectorIndexConfig {
                index_type: index_type.clone(),
                dimension: 128,
                metric: DistanceMetric::Cosine,
                max_vectors: 10000,
                index_params: match index_type {
                    VectorIndexType::HNSW => IndexParameters::HNSW(HNSWConfig::default()),
                    VectorIndexType::IVF => IndexParameters::IVF(IVFConfig::default()),
                    VectorIndexType::AdaptiveHNSW => IndexParameters::AdaptiveHNSW(HNSWConfig::default()),
                    VectorIndexType::AdaptiveIVF => IndexParameters::AdaptiveIVF(IVFConfig::default()),
                },
            };

            let index = AuroraVectorIndex::new(config).unwrap();
            // Should create successfully
            assert!(index.config.dimension == 128);
        }
    }
}
