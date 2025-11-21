//! Logical Plan Operators
//!
//! Operations for manipulating and transforming logical plans.

use super::plans::*;
use crate::query::parser::ast::*;

/// Logical plan visitor trait for traversing plans
pub trait LogicalPlanVisitor<T> {
    fn visit_seq_scan(&mut self, table: &str, filter: &Option<Expression>) -> T;
    fn visit_index_scan(&mut self, table: &str, index: &str, filter: &Option<Expression>) -> T;
    fn visit_vector_search(&mut self, vector_expr: &Expression, distance_metric: &DistanceMetric, k: usize, filter: &Option<Box<LogicalPlan>>) -> T;
    fn visit_nested_loop_join(&mut self, left: &LogicalPlan, right: &LogicalPlan, condition: &Expression) -> T;
    fn visit_hash_join(&mut self, left: &LogicalPlan, right: &LogicalPlan, condition: &Expression) -> T;
    fn visit_sort(&mut self, input: &LogicalPlan, order_by: &[OrderByItem]) -> T;
    fn visit_group_by(&mut self, input: &LogicalPlan, group_by: &[Expression], aggregates: &[AggregateExpr]) -> T;
    fn visit_limit(&mut self, input: &LogicalPlan, limit: usize, offset: usize) -> T;
}

/// Plan transformation utilities
pub struct PlanTransformer;

impl PlanTransformer {
    /// Push down filters as far as possible in the plan
    pub fn push_down_filters(plan: LogicalPlan) -> LogicalPlan {
        match plan {
            LogicalPlan::SeqScan { table, filter } => {
                // For now, keep filters at scan level
                LogicalPlan::SeqScan { table, filter }
            }
            LogicalPlan::NestedLoopJoin { left, right, condition } => {
                // TODO: Implement filter pushdown through joins
                LogicalPlan::NestedLoopJoin {
                    left: Box::new(Self::push_down_filters(*left)),
                    right: Box::new(Self::push_down_filters(*right)),
                    condition,
                }
            }
            // TODO: Add more transformation rules
            other => other,
        }
    }

    /// Eliminate unnecessary operations
    pub fn eliminate_unnecessary_ops(plan: LogicalPlan) -> LogicalPlan {
        match plan {
            LogicalPlan::Limit { input, limit, offset } => {
                // If limit is 0, return empty result
                if limit == 0 {
                    return LogicalPlan::Limit {
                        input,
                        limit: 0,
                        offset,
                    };
                }
                LogicalPlan::Limit {
                    input: Box::new(Self::eliminate_unnecessary_ops(*input)),
                    limit,
                    offset,
                }
            }
            LogicalPlan::NestedLoopJoin { left, right, condition } => {
                LogicalPlan::NestedLoopJoin {
                    left: Box::new(Self::eliminate_unnecessary_ops(*left)),
                    right: Box::new(Self::eliminate_unnecessary_ops(*right)),
                    condition,
                }
            }
            // TODO: Add more elimination rules
            other => other,
        }
    }
}

/// Plan cost estimation visitor
pub struct CostEstimator;

impl LogicalPlanVisitor<f64> for CostEstimator {
    fn visit_seq_scan(&mut self, _table: &str, filter: &Option<Expression>) -> f64 {
        let base_cost = 100.0; // Base I/O cost
        let filter_cost = if filter.is_some() { 50.0 } else { 0.0 };
        base_cost + filter_cost
    }

    fn visit_index_scan(&mut self, _table: &str, _index: &str, _filter: &Option<Expression>) -> f64 {
        25.0 // Index scans are typically cheaper
    }

    fn visit_vector_search(&mut self, _vector_expr: &Expression, _distance_metric: &DistanceMetric, _k: usize, _filter: &Option<Box<LogicalPlan>>) -> f64 {
        75.0 // Vector search computational cost
    }

    fn visit_nested_loop_join(&mut self, left: &LogicalPlan, right: &LogicalPlan, _condition: &Expression) -> f64 {
        // Cost is roughly left_cost + (left_cardinality * right_cost)
        let left_cost = self.estimate_plan_cost(left);
        let right_cost = self.estimate_plan_cost(right);
        left_cost + right_cost + 200.0 // Join overhead
    }

    fn visit_hash_join(&mut self, left: &LogicalPlan, right: &LogicalPlan, _condition: &Expression) -> f64 {
        let left_cost = self.estimate_plan_cost(left);
        let right_cost = self.estimate_plan_cost(right);
        left_cost + right_cost + 150.0 // Hash join overhead (usually cheaper than nested loop)
    }

    fn visit_sort(&mut self, input: &LogicalPlan, _order_by: &[OrderByItem]) -> f64 {
        let input_cost = self.estimate_plan_cost(input);
        input_cost + 300.0 // Sorting is expensive
    }

    fn visit_group_by(&mut self, input: &LogicalPlan, _group_by: &[Expression], _aggregates: &[AggregateExpr]) -> f64 {
        let input_cost = self.estimate_plan_cost(input);
        input_cost + 100.0 // Aggregation cost
    }

    fn visit_limit(&mut self, input: &LogicalPlan, _limit: usize, _offset: usize) -> f64 {
        let input_cost = self.estimate_plan_cost(input);
        input_cost * 0.1 // Limits can significantly reduce cost
    }
}

impl CostEstimator {
    /// Estimate cost for a complete logical plan
    pub fn estimate_plan_cost(&mut self, plan: &LogicalPlan) -> f64 {
        match plan {
            LogicalPlan::SeqScan { table, filter } => self.visit_seq_scan(table, filter),
            LogicalPlan::IndexScan { table, index, filter } => self.visit_index_scan(table, index, filter),
            LogicalPlan::VectorSearch { vector_expr, distance_metric, k, filter } => self.visit_vector_search(vector_expr, distance_metric, *k, filter),
            LogicalPlan::NestedLoopJoin { left, right, condition } => self.visit_nested_loop_join(left, right, condition),
            LogicalPlan::HashJoin { left, right, condition } => self.visit_hash_join(left, right, condition),
            LogicalPlan::Sort { input, order_by } => self.visit_sort(input, order_by),
            LogicalPlan::GroupBy { input, group_by, aggregates } => self.visit_group_by(input, group_by, aggregates),
            LogicalPlan::Limit { input, limit, offset } => self.visit_limit(input, *limit, *offset),
        }
    }
}
