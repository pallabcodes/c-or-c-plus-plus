//! Query Optimizer
//!
//! Main optimizer interface coordinating rule-based and cost-based optimization.

use super::logical::plans::*;
use super::learning::*;
use super::OptimizationResult;

/// Query optimizer with rule-based and cost-based optimization
pub struct QueryOptimizer {
    rule_optimizer: super::optimization::rules::RuleOptimizer,
    alternative_explorer: super::optimization::alternatives::AlternativeExplorer,
}

impl QueryOptimizer {
    /// Create a new query optimizer
    pub fn new() -> Self {
        Self {
            rule_optimizer: super::optimization::rules::RuleOptimizer,
            alternative_explorer: super::optimization::alternatives::AlternativeExplorer::new(),
        }
    }

    /// Optimize a logical plan
    pub async fn optimize(&mut self, plan: LogicalPlan, hints: &[OptimizationHint]) -> OptimizationResult<LogicalPlan> {
        let mut optimized_plan = plan;

        // Apply rule-based optimizations
        optimized_plan = self.rule_optimizer.apply_rules(optimized_plan)?;

        // Apply AI hints
        optimized_plan = self.apply_hints(optimized_plan, hints)?;

        // Cost-based optimization - try alternative plans
        optimized_plan = self.alternative_explorer.explore_alternatives(optimized_plan).await?;

        Ok(optimized_plan)
    }

    /// Apply AI-powered optimization hints
    fn apply_hints(&self, plan: LogicalPlan, hints: &[OptimizationHint]) -> OptimizationResult<LogicalPlan> {
        let mut result = plan;

        for hint in hints {
            if hint.confidence > 0.7 {
                result = self.apply_hint_transformation(result, &hint.hint_type)?;
            }
        }

        Ok(result)
    }

    /// Apply hint transformation
    fn apply_hint_transformation(&self, plan: LogicalPlan, hint_type: &HintType) -> OptimizationResult<LogicalPlan> {
        match hint_type {
            HintType::UseIndex => {
                // Convert seq scan to index scan
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
            HintType::UseHashJoin => {
                // Convert nested loop to hash join
                if let LogicalPlan::NestedLoopJoin { left, right, condition } = plan {
                    Ok(LogicalPlan::HashJoin { left, right, condition })
                } else {
                    Ok(plan)
                }
            }
            HintType::UseVectorIndex => {
                // Vector indexes are handled at the access method level
                Ok(plan)
            }
            _ => Ok(plan), // TODO: Implement other hint types
        }
    }

    /// Get number of alternatives evaluated
    pub fn alternatives_evaluated(&self) -> usize {
        self.alternative_explorer.alternatives_evaluated()
    }
}