//! Core Query Planner Implementation
//!
//! Main coordination logic for AI-powered query planning.

use crate::core::*;
use crate::query::parser::ast::*;
use crate::query::planner::*;
use super::super::{statistics::*, learning::*, optimizer::*};
use super::{PlanResult, PlanError, PlannerStats, PlanMetadata};
use super::planning::*;

/// AI-powered query planner
pub struct QueryPlanner {
    /// Statistics manager for cardinality estimation
    statistics: StatisticsManager,
    /// Machine learning component for optimization hints
    learner: QueryLearner,
    /// Query optimizer
    optimizer: QueryOptimizer,
    /// Physical planner
    physical_planner: PhysicalPlanner,
    /// Planning statistics
    stats: PlannerStats,
}

/// Final query plan with optimization metadata
#[derive(Debug, Clone)]
pub struct QueryPlan {
    /// Physical execution plan
    pub physical_plan: PhysicalPlan,
    /// Optimization metadata
    pub metadata: PlanMetadata,
}

/// Physical query plan with execution details
#[derive(Debug, Clone)]
pub struct PhysicalPlan {
    /// Logical plan
    pub logical_plan: LogicalPlan,
    /// Estimated cost
    pub cost: CostEstimate,
    /// Execution properties
    pub properties: PlanProperties,
}

impl QueryPlanner {
    /// Create a new AI-powered query planner
    pub fn new() -> Self {
        Self {
            statistics: StatisticsManager::new(),
            learner: QueryLearner::new(),
            optimizer: QueryOptimizer::new(),
            physical_planner: PhysicalPlanner::new(),
            stats: PlannerStats::default(),
        }
    }

    /// Plan a parsed query into an optimized execution plan
    pub async fn plan_query(&mut self, query: &Query) -> PlanResult<QueryPlan> {
        let start_time = std::time::Instant::now();

        // Generate initial logical plan
        let logical_plan = self.generate_logical_plan(query)?;

        // Get optimization hints from machine learning
        let hints = self.learner.get_hints(query).await;

        // Optimize the logical plan
        let optimized_plan = self.optimizer.optimize(logical_plan, &hints).await?;

        // Generate physical plan with cost estimation
        let physical_plan = self.physical_planner.generate_physical_plan(optimized_plan).await?;

        // Create final query plan
        let plan_time = start_time.elapsed().as_millis() as f64;
        let query_plan = QueryPlan {
            physical_plan,
            metadata: PlanMetadata {
                total_cost: physical_plan.cost.clone(),
                optimization_time_ms: plan_time,
                alternatives_considered: self.optimizer.alternatives_evaluated(),
                hints_applied: hints.iter().map(|h| h.description.clone()).collect(),
                statistics_used: self.statistics.get_used_statistics(),
            },
        };

        // Update statistics
        self.stats.queries_planned += 1;
        self.stats.optimization_time_ms =
            (self.stats.optimization_time_ms * (self.stats.queries_planned - 1) as f64 + plan_time)
                / self.stats.queries_planned as f64;
        self.stats.hints_applied += hints.len() as u64;

        // Learn from this planning decision
        self.learner.learn_from_plan(query, &query_plan).await;

        Ok(query_plan)
    }

    /// Generate initial logical plan from parsed query
    fn generate_logical_plan(&self, query: &Query) -> PlanResult<LogicalPlan> {
        match query {
            Query::Select(select) => SelectPlanner::plan(select),
            Query::Insert(_) => Err(PlanError::UnsupportedOperation {
                operation: "INSERT planning not yet implemented".to_string(),
            }),
            Query::Update(_) => Err(PlanError::UnsupportedOperation {
                operation: "UPDATE planning not yet implemented".to_string(),
            }),
            Query::Delete(_) => Err(PlanError::UnsupportedOperation {
                operation: "DELETE planning not yet implemented".to_string(),
            }),
            Query::VectorSearch(vector) => VectorPlanner::plan(vector),
            Query::CreateTable(_) | Query::DropTable(_) => Err(PlanError::UnsupportedOperation {
                operation: "DDL planning not supported".to_string(),
            }),
        }
    }

    /// Get planner statistics
    pub fn stats(&self) -> &PlannerStats {
        &self.stats
    }
}
