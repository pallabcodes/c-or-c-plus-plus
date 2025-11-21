//! Optimization Rules
//!
//! Rule-based query optimizations applied before cost-based planning.

use crate::query::planner::logical::plans::*;
use super::super::{OptimizationResult, OptimizationError};

/// Optimization rule
#[derive(Debug, Clone)]
pub struct OptimizationRule {
    pub name: String,
    pub condition: RuleCondition,
    pub transformation: RuleTransformation,
}

/// Rule condition for applicability
#[derive(Debug, Clone)]
pub enum RuleCondition {
    HasSeqScan,
    HasNestedLoopJoin,
    HasSortWithoutLimit,
    HasFilter,
    HighCardinalityJoin,
}

/// Rule transformation function
#[derive(Debug, Clone)]
pub enum RuleTransformation {
    ConvertToIndexScan,
    ConvertToHashJoin,
    PushDownLimit,
    EliminateUnnecessarySort,
    AddIndexHint,
}

/// Rule-based optimizer
pub struct RuleOptimizer;

impl RuleOptimizer {
    /// Apply rule-based optimizations
    pub fn apply_rules(plan: LogicalPlan) -> OptimizationResult<LogicalPlan> {
        let rules = Self::initialize_rules();
        let mut result = plan;

        // Apply each rule if applicable
        for rule in rules {
            if Self::rule_applies(&result, &rule.condition) {
                result = Self::apply_transformation(result, &rule.transformation)?;
            }
        }

        Ok(result)
    }

    /// Check if optimization rule applies
    fn rule_applies(plan: &LogicalPlan, condition: &RuleCondition) -> bool {
        match condition {
            RuleCondition::HasSeqScan => matches!(plan, LogicalPlan::SeqScan { .. }),
            RuleCondition::HasNestedLoopJoin => matches!(plan, LogicalPlan::NestedLoopJoin { .. }),
            RuleCondition::HasSortWithoutLimit => {
                matches!(plan, LogicalPlan::Sort { .. }) &&
                !matches!(plan, LogicalPlan::Limit { .. })
            }
            RuleCondition::HasFilter => Self::plan_has_filters(plan),
            RuleCondition::HighCardinalityJoin => false, // TODO: Check join cardinality
        }
    }

    /// Apply transformation to plan
    fn apply_transformation(plan: LogicalPlan, transformation: &RuleTransformation) -> OptimizationResult<LogicalPlan> {
        match transformation {
            RuleTransformation::ConvertToIndexScan => {
                if let LogicalPlan::SeqScan { table, filter } = plan {
                    Ok(LogicalPlan::IndexScan {
                        table,
                        index: "auto_selected".to_string(),
                        filter,
                    })
                } else {
                    Ok(plan)
                }
            }
            RuleTransformation::ConvertToHashJoin => {
                if let LogicalPlan::NestedLoopJoin { left, right, condition } = plan {
                    Ok(LogicalPlan::HashJoin { left, right, condition })
                } else {
                    Ok(plan)
                }
            }
            RuleTransformation::PushDownLimit => Ok(plan), // TODO: Implement limit pushdown
            RuleTransformation::EliminateUnnecessarySort => Ok(plan), // TODO: Implement sort elimination
            RuleTransformation::AddIndexHint => Ok(plan), // Hints don't change structure
        }
    }

    /// Check if plan has any filters
    fn plan_has_filters(plan: &LogicalPlan) -> bool {
        match plan {
            LogicalPlan::SeqScan { filter, .. } => filter.is_some(),
            LogicalPlan::IndexScan { filter, .. } => filter.is_some(),
            LogicalPlan::VectorSearch { filter, .. } => filter.is_some(),
            LogicalPlan::NestedLoopJoin { left, right, .. } |
            LogicalPlan::HashJoin { left, right, .. } => {
                Self::plan_has_filters(left) || Self::plan_has_filters(right)
            }
            LogicalPlan::Sort { input, .. } |
            LogicalPlan::GroupBy { input, .. } |
            LogicalPlan::Limit { input, .. } => Self::plan_has_filters(input),
        }
    }

    /// Initialize optimization rules
    fn initialize_rules() -> Vec<OptimizationRule> {
        vec![
            OptimizationRule {
                name: "seq_scan_to_index".to_string(),
                condition: RuleCondition::HasSeqScan,
                transformation: RuleTransformation::ConvertToIndexScan,
            },
            OptimizationRule {
                name: "nested_loop_to_hash".to_string(),
                condition: RuleCondition::HasNestedLoopJoin,
                transformation: RuleTransformation::ConvertToHashJoin,
            },
            OptimizationRule {
                name: "push_limit".to_string(),
                condition: RuleCondition::HasSortWithoutLimit,
                transformation: RuleTransformation::PushDownLimit,
            },
        ]
    }
}
