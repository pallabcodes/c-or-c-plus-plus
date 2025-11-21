//! Physical Operator Traits
//!
//! Core traits for the Volcano iterator model execution.

use crate::core::*;
use super::super::ExecutionResult;

/// Physical operator trait - Volcano iterator model
#[async_trait::async_trait]
pub trait PhysicalOperator: Send + Sync {
    /// Open the operator for execution
    async fn open(&mut self) -> ExecutionResult<()>;

    /// Get next tuple from operator
    async fn next(&mut self) -> ExecutionResult<Option<Row>>;

    /// Close the operator and cleanup resources
    async fn close(&mut self) -> ExecutionResult<()>;

    /// Get operator statistics
    fn stats(&self) -> OperatorStats;
}

/// Operator execution statistics
#[derive(Debug, Clone, Default)]
pub struct OperatorStats {
    pub rows_processed: u64,
    pub execution_time_ms: f64,
    pub memory_used_bytes: usize,
    pub io_operations: u64,
    pub cache_hits: u64,
}
