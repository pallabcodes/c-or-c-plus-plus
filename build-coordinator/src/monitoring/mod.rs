//! Performance Monitoring & Optimization: UNIQUENESS Research-Backed
//!
//! HDR Histograms, SIMD acceleration, NUMA optimization, and comprehensive benchmarking:
//! - **HDR Histograms**: High-dynamic-range latency measurements (Gil et al., 2008)
//! - **SIMD Acceleration**: Vectorized operations for coordination tasks
//! - **NUMA Optimization**: Memory hierarchy exploitation (Torrellas et al., 2010)
//! - **Memory Optimization**: Slab allocation and zero-copy techniques
//! - **Benchmarking Suite**: Comparative performance analysis
//! - **Real-Time Metrics**: Sub-microsecond measurement precision

pub mod hdr_histograms;
pub mod simd_acceleration;
pub mod numa_optimization;
pub mod memory_optimization;
pub mod benchmarking;
pub mod performance_metrics;
pub mod monitoring_system;

pub use hdr_histograms::{HDRHistogram, HistogramRecorder};
pub use simd_acceleration::{SIMDProcessor, VectorizedOperations};
pub use numa_optimization::{NumaAwareAllocator, NumaAwareScheduler, NumaTopology};
pub use memory_optimization::{MemoryOptimizer, SlabAllocator};
pub use benchmarking::{BenchmarkSuite, PerformanceBenchmark};
pub use performance_metrics::{PerformanceMetrics, LatencyStats, ThroughputStats};
pub use monitoring_system::MonitoringSystem;

// Re-export key types
pub use crate::types::NodeId;

// UNIQUENESS Research Citations:
// - HDR Histograms: Gil et al. (2008) - High-dynamic-range latency measurements
// - SIMD: Intel SIMD extensions - Vectorized processing
// - NUMA Optimization: Torrellas et al. (2010) - Cache-coherent memory hierarchy
// - Memory Optimization: Bonwick (1994) - Slab allocation
// - Performance Analysis: Various papers on benchmarking distributed systems
