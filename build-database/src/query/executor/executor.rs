//! Query Executor Implementation
//!
//! Main execution engine implementing the Volcano iterator model
//! with adaptive optimization and vectorized operations.

use crate::core::*;
use crate::query::planner::core::*;
use crate::storage::engine::*;
use super::operators::*;
use super::adaptive::*;
use std::collections::HashMap;

/// Query execution result
pub type ExecutionResult<T> = Result<T, ExecutionError>;

/// Execution engine specific errors
#[derive(Debug, thiserror::Error)]
pub enum ExecutionError {
    #[error("Execution failed: {message}")]
    Failed { message: String },

    #[error("Operator error: {operator} - {message}")]
    OperatorError { operator: String, message: String },

    #[error("Resource exhausted: {resource}")]
    ResourceExhausted { resource: String },

    #[error("Timeout exceeded: {duration_ms}ms")]
    TimeoutExceeded { duration_ms: u64 },

    #[error("Data type mismatch: expected {expected}, got {actual}")]
    TypeMismatch { expected: String, actual: String },
}

/// Main query executor with Volcano iterator model
pub struct QueryExecutor {
    /// Storage engine for data access
    storage: Box<dyn StorageEngine>,
    /// Execution operators registry
    operators: HashMap<String, Box<dyn PhysicalOperator>>,
    /// Adaptive execution manager
    adaptive_manager: AdaptiveExecutionManager,
    /// Execution statistics
    stats: ExecutionStats,
    /// Vectorized execution enabled
    vectorized_enabled: bool,
}

/// Execution performance statistics
#[derive(Debug, Clone, Default)]
pub struct ExecutionStats {
    pub queries_executed: u64,
    pub total_execution_time_ms: f64,
    pub rows_processed: u64,
    pub operators_executed: u64,
    pub cache_hits: u64,
    pub vectorized_operations: u64,
    pub adaptive_switches: u64,
}

impl QueryExecutor {
    /// Create a new query executor
    pub fn new(storage: Box<dyn StorageEngine>) -> Self {
        Self {
            storage,
            operators: HashMap::new(),
            adaptive_manager: AdaptiveExecutionManager::new(),
            stats: ExecutionStats::default(),
            vectorized_enabled: true,
        }
    }

    /// Execute a query plan and return results
    pub async fn execute(&mut self, plan: &QueryPlan) -> ExecutionResult<QueryResult> {
        let start_time = std::time::Instant::now();

        // Create execution tree from physical plan
        let root_operator = self.build_execution_tree(&plan.physical_plan.logical_plan).await?;

        // Execute with adaptive optimization
        let result = self.adaptive_manager.execute_adaptive(root_operator, plan).await?;

        // Update statistics
        let execution_time = start_time.elapsed().as_millis() as f64;
        self.stats.queries_executed += 1;
        self.stats.total_execution_time_ms += execution_time;
        self.stats.rows_processed += result.row_count;
        self.stats.operators_executed += result.operators_used;

        Ok(result)
    }

    /// Build execution tree from logical plan
    async fn build_execution_tree(&mut self, plan: &LogicalPlan) -> ExecutionResult<Box<dyn PhysicalOperator>> {
        match plan {
            LogicalPlan::SeqScan { table, filter } => {
                Ok(Box::new(SeqScanOperator::new(
                    table.clone(),
                    filter.clone(),
                    self.storage.as_ref(),
                )))
            }
            LogicalPlan::IndexScan { table, index, filter } => {
                Ok(Box::new(IndexScanOperator::new(
                    table.clone(),
                    index.clone(),
                    filter.clone(),
                    self.storage.as_ref(),
                )))
            }
            LogicalPlan::VectorSearch { vector_expr, distance_metric, k, filter } => {
                Ok(Box::new(VectorSearchOperator::new(
                    vector_expr.clone(),
                    *distance_metric,
                    *k,
                    filter.clone(),
                    self.storage.as_ref(),
                )))
            }
            LogicalPlan::NestedLoopJoin { left, right, condition } => {
                let left_op = self.build_execution_tree(left).await?;
                let right_op = self.build_execution_tree(right).await?;
                Ok(Box::new(NestedLoopJoinOperator::new(
                    left_op,
                    right_op,
                    condition.clone(),
                )))
            }
            LogicalPlan::HashJoin { left, right, condition } => {
                let left_op = self.build_execution_tree(left).await?;
                let right_op = self.build_execution_tree(right).await?;
                Ok(Box::new(HashJoinOperator::new(
                    left_op,
                    right_op,
                    condition.clone(),
                    self.vectorized_enabled,
                )))
            }
            LogicalPlan::Sort { input, order_by } => {
                let input_op = self.build_execution_tree(input).await?;
                Ok(Box::new(SortOperator::new(
                    input_op,
                    order_by.clone(),
                    self.vectorized_enabled,
                )))
            }
            LogicalPlan::GroupBy { input, group_by, aggregates } => {
                let input_op = self.build_execution_tree(input).await?;
                Ok(Box::new(GroupByOperator::new(
                    input_op,
                    group_by.clone(),
                    aggregates.clone(),
                    self.vectorized_enabled,
                )))
            }
            LogicalPlan::Limit { input, limit, offset } => {
                let input_op = self.build_execution_tree(input).await?;
                Ok(Box::new(LimitOperator::new(input_op, *limit, *offset)))
            }
        }
    }

    /// Enable or disable vectorized execution
    pub fn set_vectorized(&mut self, enabled: bool) {
        self.vectorized_enabled = enabled;
    }

    /// Get execution statistics
    pub fn stats(&self) -> &ExecutionStats {
        &self.stats
    }

    /// Get adaptive execution statistics
    pub fn adaptive_stats(&self) -> &AdaptiveStats {
        self.adaptive_manager.stats()
    }
}

/// Query execution result
#[derive(Debug, Clone)]
pub struct QueryResult {
    pub rows: Vec<Row>,
    pub row_count: u64,
    pub execution_time_ms: f64,
    pub operators_used: u64,
    pub memory_used_bytes: usize,
    pub cache_hit_ratio: f64,
}
