//! Logical Plan Structures
//!
//! Abstract representation of query execution plans.

use crate::query::parser::ast::*;

/// Logical query plan representation
#[derive(Debug, Clone)]
pub enum LogicalPlan {
    /// Sequential scan of a table
    SeqScan {
        table: String,
        filter: Option<Expression>,
    },
    /// Index scan using an index
    IndexScan {
        table: String,
        index: String,
        filter: Option<Expression>,
    },
    /// Vector similarity search
    VectorSearch {
        vector_expr: Expression,
        distance_metric: DistanceMetric,
        k: usize,
        filter: Option<Box<LogicalPlan>>,
    },
    /// Nested loop join
    NestedLoopJoin {
        left: Box<LogicalPlan>,
        right: Box<LogicalPlan>,
        condition: Expression,
    },
    /// Hash join
    HashJoin {
        left: Box<LogicalPlan>,
        right: Box<LogicalPlan>,
        condition: Expression,
    },
    /// Sort operation
    Sort {
        input: Box<LogicalPlan>,
        order_by: Vec<OrderByItem>,
    },
    /// Group by aggregation
    GroupBy {
        input: Box<LogicalPlan>,
        group_by: Vec<Expression>,
        aggregates: Vec<AggregateExpr>,
    },
    /// Limit operation
    Limit {
        input: Box<LogicalPlan>,
        limit: usize,
        offset: usize,
    },
}

/// Aggregate expression
#[derive(Debug, Clone)]
pub struct AggregateExpr {
    pub function: AggregateFunction,
    pub argument: Expression,
    pub alias: Option<String>,
}

/// Aggregate functions
#[derive(Debug, Clone)]
pub enum AggregateFunction {
    Count,
    Sum,
    Avg,
    Min,
    Max,
    CountDistinct,
}
