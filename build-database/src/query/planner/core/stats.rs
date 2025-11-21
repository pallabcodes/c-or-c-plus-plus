//! Planner Statistics
//!
//! Performance metrics and monitoring for query planning.

/// Planner performance statistics
#[derive(Debug, Clone, Default)]
pub struct PlannerStats {
    pub queries_planned: u64,
    pub optimization_time_ms: f64,
    pub plans_evaluated: u64,
    pub hints_applied: u64,
}

/// Plan execution properties
#[derive(Debug, Clone, Default)]
pub struct PlanProperties {
    pub preserves_ordering: bool,
    pub output_cardinality: Option<u64>,
    pub memory_required: usize,
    pub cpu_cost: f64,
    pub io_cost: f64,
}

/// Plan optimization metadata
#[derive(Debug, Clone)]
pub struct PlanMetadata {
    pub total_cost: crate::query::planner::cost_model::CostEstimate,
    pub optimization_time_ms: f64,
    pub alternatives_considered: usize,
    pub hints_applied: Vec<String>,
    pub statistics_used: Vec<String>,
}
