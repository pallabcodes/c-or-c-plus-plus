//! SELECT Query Planning
//!
//! Plans SELECT queries with support for joins, aggregations, and vector extensions.

use crate::query::parser::ast::*;
use crate::query::planner::logical::plans::*;

/// SELECT query planner
pub struct SelectPlanner;

impl SelectPlanner {
    /// Plan a SELECT query
    pub fn plan(select: &SelectQuery) -> PlanResult<LogicalPlan> {
        // Start with base table scan
        let mut plan = LogicalPlan::SeqScan {
            table: select.from_clause.table.clone(),
            filter: select.where_clause.clone(),
        };

        // Add joins if present
        for join in &select.from_clause.joins {
            plan = LogicalPlan::NestedLoopJoin {
                left: Box::new(plan),
                right: Box::new(LogicalPlan::SeqScan {
                    table: join.table.clone(),
                    filter: None,
                }),
                condition: join.condition.clone(),
            };
        }

        // Add vector extensions if present
        if let Some(vector_ext) = &select.vector_extensions {
            if let Some(nearest) = &vector_ext.nearest_neighbors {
                plan = LogicalPlan::VectorSearch {
                    vector_expr: nearest.vector_expression.clone(),
                    distance_metric: nearest.distance_metric.clone(),
                    k: nearest.k,
                    filter: Some(LogicalPlan::SeqScan {
                        table: select.from_clause.table.clone(),
                        filter: select.where_clause.clone(),
                    }.into()),
                };
            }
        }

        // Add GROUP BY if present
        if let Some(group_by) = &select.group_by {
            plan = LogicalPlan::GroupBy {
                input: Box::new(plan),
                group_by: group_by.expressions.clone(),
                aggregates: vec![], // TODO: Extract from select list
            };
        }

        // Add ORDER BY if present
        if let Some(order_by) = &select.order_by {
            plan = LogicalPlan::Sort {
                input: Box::new(plan),
                order_by: order_by.items.clone(),
            };
        }

        // Add LIMIT if present
        if let Some(limit) = &select.limit {
            plan = LogicalPlan::Limit {
                input: Box::new(plan),
                limit: limit.limit,
                offset: limit.offset.unwrap_or(0),
            };
        }

        Ok(plan)
    }
}
