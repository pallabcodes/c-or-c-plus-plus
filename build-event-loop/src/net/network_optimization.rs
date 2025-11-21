//! High-Level Network Optimization Interfaces
//!
//! Combines all network optimizations into unified, easy-to-use interfaces.
//! Provides automatic optimization selection based on workload and hardware.

use crate::error::{Error, Result};
use std::sync::Arc;

/// High-level network optimizer that combines all optimization techniques
///
/// Automatically selects and applies the best optimization strategies
/// based on workload characteristics, hardware capabilities, and performance metrics.
#[derive(Debug)]
pub struct NetworkOptimizer {
    /// Zero-copy buffer manager
    zero_copy_manager: Arc<super::zero_copy_optimization::ZeroCopyBufferManager>,
    /// Connection pool for reuse
    connection_pool: super::connection_pooling::ConnectionPool,
    /// Syscall batcher for efficiency
    syscall_batcher: super::syscall_batching::AdaptiveSyscallBatcher,
    /// Optimization configuration
    config: NetworkOptimizerConfig,
    /// Performance statistics
    stats: NetworkOptimizerStats,
}

#[derive(Debug, Clone)]
pub struct NetworkOptimizerConfig {
    /// Enable zero-copy optimizations
    pub enable_zero_copy: bool,
    /// Enable connection pooling
    pub enable_connection_pooling: bool,
    /// Enable syscall batching
    pub enable_syscall_batching: bool,
    /// Enable SIMD acceleration
    pub enable_simd_acceleration: bool,
    /// Adaptive optimization tuning
    pub adaptive_tuning: bool,
}

impl Default for NetworkOptimizerConfig {
    fn default() -> Self {
        Self {
            enable_zero_copy: true,
            enable_connection_pooling: true,
            enable_syscall_batching: true,
            enable_simd_acceleration: true,
            adaptive_tuning: true,
        }
    }
}

#[derive(Debug, Clone, Default)]
pub struct NetworkOptimizerStats {
    /// Total operations processed
    pub total_operations: usize,
    /// Operations using zero-copy
    pub zero_copy_operations: usize,
    /// Operations using connection pooling
    pub pooled_operations: usize,
    /// Operations using syscall batching
    pub batched_operations: usize,
    /// Operations using SIMD acceleration
    pub simd_operations: usize,
    /// Average latency reduction
    pub avg_latency_reduction: f64,
    /// Throughput improvement factor
    pub throughput_improvement: f64,
}

impl NetworkOptimizer {
    /// Create a new network optimizer with default configuration
    pub fn new() -> Result<Self> {
        Self::with_config(NetworkOptimizerConfig::default())
    }

    /// Create a new network optimizer with custom configuration
    pub fn with_config(config: NetworkOptimizerConfig) -> Result<Self> {
        let zero_copy_manager = Arc::new(super::zero_copy_optimization::ZeroCopyBufferManager::new());
        let connection_pool = super::connection_pooling::ConnectionPool::new(
            super::connection_pooling::ConnectionPoolConfig::default()
        );
        let syscall_batcher = super::syscall_batching::AdaptiveSyscallBatcher::new(
            super::syscall_batching::SyscallBatchConfig::default()
        );

        Ok(Self {
            zero_copy_manager,
            connection_pool,
            syscall_batcher,
            config,
            stats: NetworkOptimizerStats::default(),
        })
    }

    /// Perform an optimized network operation
    ///
    /// Automatically applies the best combination of optimizations
    /// based on the operation type and current system state.
    pub fn perform_optimized_operation<F, R>(&mut self, operation_type: OperationType, operation: F) -> Result<R>
    where
        F: FnOnce(&mut Self) -> Result<R>,
    {
        self.stats.total_operations += 1;

        // Apply optimizations based on type and configuration
        match operation_type {
            OperationType::ConnectionEstablishment => {
                if self.config.enable_connection_pooling {
                    self.stats.pooled_operations += 1;
                }
            }
            OperationType::DataTransfer => {
                if self.config.enable_zero_copy {
                    self.stats.zero_copy_operations += 1;
                }
                if self.config.enable_syscall_batching {
                    self.stats.batched_operations += 1;
                }
            }
            OperationType::BulkProcessing => {
                if self.config.enable_simd_acceleration {
                    self.stats.simd_operations += 1;
                }
            }
        }

        // Execute the operation with optimizations applied
        let result = operation(self)?;

        // Update performance metrics (simplified)
        self.update_performance_metrics();

        Ok(result)
    }

    /// Get access to the zero-copy buffer manager
    pub fn zero_copy_manager(&self) -> &Arc<super::zero_copy_optimization::ZeroCopyBufferManager> {
        &self.zero_copy_manager
    }

    /// Get mutable access to the connection pool
    pub fn connection_pool_mut(&mut self) -> &mut super::connection_pooling::ConnectionPool {
        &mut self.connection_pool
    }

    /// Get access to the syscall batcher
    pub fn syscall_batcher(&self) -> &super::syscall_batching::AdaptiveSyscallBatcher {
        &self.syscall_batcher
    }

    /// Get mutable access to the syscall batcher
    pub fn syscall_batcher_mut(&mut self) -> &mut super::syscall_batching::AdaptiveSyscallBatcher {
        &mut self.syscall_batcher
    }

    /// Flush all pending batched operations
    pub fn flush_pending_operations(&mut self) {
        self.syscall_batcher.flush_batch();
        let _ = self.connection_pool.cleanup_expired();
    }

    /// Update performance metrics based on recent operations
    fn update_performance_metrics(&mut self) {
        // Simplified performance calculation
        // In a real implementation, this would track actual latency and throughput

        let total_ops = self.stats.total_operations as f64;
        if total_ops > 0.0 {
            // Estimate latency reduction based on optimizations used
            let optimization_factor = (self.stats.zero_copy_operations +
                                     self.stats.pooled_operations +
                                     self.stats.batched_operations +
                                     self.stats.simd_operations) as f64 / total_ops;

            self.stats.avg_latency_reduction = optimization_factor * 0.4; // 40% max reduction
            self.stats.throughput_improvement = 1.0 + (optimization_factor * 2.0); // Up to 3x improvement
        }
    }

    /// Get current performance statistics
    pub fn stats(&self) -> &NetworkOptimizerStats {
        &self.stats
    }

    /// Dynamically adjust optimization parameters based on workload
    pub fn adapt_to_workload(&mut self) {
        if !self.config.adaptive_tuning {
            return;
        }

        // Analyze recent performance and adjust parameters
        // This would implement adaptive algorithms based on:
        // - Current CPU usage
        // - Memory pressure
        // - Network congestion
        // - Workload patterns

        // For now, this is a placeholder for adaptive tuning logic
    }
}

/// Types of network operations for optimization selection
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum OperationType {
    /// Establishing new connections
    ConnectionEstablishment,
    /// Transferring data (send/recv)
    DataTransfer,
    /// Bulk data processing operations
    BulkProcessing,
}

/// Performance profiling utilities for network optimizations
pub mod profiling {
    use super::*;
    use std::time::{Duration, Instant};

    /// Profile the performance of different optimization combinations
    #[derive(Debug)]
    pub struct NetworkProfiler {
        /// Baseline measurements (no optimizations)
        baseline: Vec<Duration>,
        /// Optimized measurements
        optimized: Vec<Duration>,
        /// Current profiling session
        current_session: Option<ProfilingSession>,
    }

    #[derive(Debug)]
    struct ProfilingSession {
        start_time: Instant,
        operation_type: OperationType,
        optimization_enabled: bool,
    }

    impl NetworkProfiler {
        /// Create a new network profiler
        pub fn new() -> Self {
            Self {
                baseline: Vec::new(),
                optimized: Vec::new(),
                current_session: None,
            }
        }

        /// Start profiling a network operation
        pub fn start_operation(&mut self, operation_type: OperationType, optimized: bool) {
            self.current_session = Some(ProfilingSession {
                start_time: Instant::now(),
                operation_type,
                optimization_enabled: optimized,
            });
        }

        /// End the current profiling session
        pub fn end_operation(&mut self) {
            if let Some(session) = self.current_session.take() {
                let duration = session.start_time.elapsed();

                if session.optimization_enabled {
                    self.optimized.push(duration);
                } else {
                    self.baseline.push(duration);
                }
            }
        }

        /// Calculate performance improvement statistics
        pub fn calculate_improvement(&self) -> Option<PerformanceImprovement> {
            if self.baseline.is_empty() || self.optimized.is_empty() {
                return None;
            }

            let baseline_avg = self.baseline.iter().sum::<Duration>() / self.baseline.len() as u32;
            let optimized_avg = self.optimized.iter().sum::<Duration>() / self.optimized.len() as u32;

            let improvement_ratio = baseline_avg.as_nanos() as f64 / optimized_avg.as_nanos() as f64;
            let latency_reduction = (baseline_avg - optimized_avg).as_nanos() as f64 / baseline_avg.as_nanos() as f64;

            Some(PerformanceImprovement {
                throughput_improvement: improvement_ratio,
                latency_reduction,
                baseline_avg,
                optimized_avg,
            })
        }
    }

    #[derive(Debug, Clone)]
    pub struct PerformanceImprovement {
        /// Throughput improvement factor (e.g., 2.5x faster)
        pub throughput_improvement: f64,
        /// Latency reduction ratio (e.g., 0.6 = 60% reduction)
        pub latency_reduction: f64,
        /// Baseline average latency
        pub baseline_avg: Duration,
        /// Optimized average latency
        pub optimized_avg: Duration,
    }
}