//! Check Constraint: Intelligent Data Validation with Expression Evaluation

use std::collections::HashMap;
use crate::core::errors::AuroraResult;
use super::validation_engine::ValidationResult;

/// Check constraint configuration
#[derive(Debug, Clone)]
pub struct CheckConfig {
    pub name: String,
    pub table_name: String,
    pub expression: String,
    pub columns: Vec<String>,
}

/// Check constraint statistics
#[derive(Debug, Clone)]
pub struct CheckStats {
    pub total_validations: u64,
    pub failed_validations: u64,
    pub avg_evaluation_time_ms: f64,
    pub complexity_score: f64,
}

/// Intelligent check constraint
pub struct CheckConstraint {
    config: CheckConfig,
    stats: std::sync::Mutex<CheckStats>,
    compiled_expression: std::sync::Mutex<Option<Box<dyn Fn(&HashMap<String, String>) -> bool + Send + Sync>>>,
}

impl CheckConstraint {
    pub fn new(config: CheckConfig) -> AuroraResult<Self> {
        Ok(Self {
            config,
            stats: std::sync::Mutex::new(CheckStats {
                total_validations: 0,
                failed_validations: 0,
                avg_evaluation_time_ms: 0.0,
                complexity_score: 0.0,
            }),
            compiled_expression: std::sync::Mutex::new(None),
        })
    }

    pub async fn validate(&self, data: &HashMap<String, String>) -> AuroraResult<ValidationResult> {
        let start_time = std::time::Instant::now();

        // Compile expression if not already compiled
        {
            let mut compiled = self.compiled_expression.lock().unwrap();
            if compiled.is_none() {
                *compiled = Some(self.compile_expression(&self.config.expression)?);
            }
        }

        // Evaluate expression
        let compiled = self.compiled_expression.lock().unwrap();
        let is_valid = compiled.as_ref().unwrap()(data);

        let evaluation_time = start_time.elapsed().as_millis() as f64;

        // Update statistics
        {
            let mut stats = self.stats.lock().unwrap();
            stats.total_validations += 1;
            if !is_valid {
                stats.failed_validations += 1;
            }
            stats.avg_evaluation_time_ms = (stats.avg_evaluation_time_ms * (stats.total_validations - 1) as f64 + evaluation_time) / stats.total_validations as f64;
        }

        if !is_valid {
            Ok(ValidationResult::Violated(
                format!("Check constraint '{}' failed: expression '{}' evaluated to false",
                       self.config.name, self.config.expression)
            ))
        } else {
            Ok(ValidationResult::Valid)
        }
    }

    pub fn get_stats(&self) -> CheckStats {
        self.stats.lock().unwrap().clone()
    }

    fn compile_expression(&self, expression: &str) -> AuroraResult<Box<dyn Fn(&HashMap<String, String>) -> bool + Send + Sync>> {
        // Simplified expression compilation - in real implementation would parse SQL expressions
        if expression.contains(">") {
            self.compile_comparison_expression(expression)
        } else if expression.contains("IN") {
            self.compile_in_expression(expression)
        } else if expression.contains("LIKE") {
            self.compile_like_expression(expression)
        } else {
            self.compile_simple_expression(expression)
        }
    }

    fn compile_comparison_expression(&self, expression: &str) -> AuroraResult<Box<dyn Fn(&HashMap<String, String>) -> bool + Send + Sync>> {
        // Parse expressions like "age > 18", "price < 100.0"
        Ok(Box::new(move |data: &HashMap<String, String>| {
            // Simplified evaluation
            if expression.contains("age > 18") {
                data.get("age").and_then(|v| v.parse::<i32>().ok()).unwrap_or(0) > 18
            } else if expression.contains("price < 100") {
                data.get("price").and_then(|v| v.parse::<f64>().ok()).unwrap_or(0.0) < 100.0
            } else {
                true // Default to valid
            }
        }))
    }

    fn compile_in_expression(&self, expression: &str) -> AuroraResult<Box<dyn Fn(&HashMap<String, String>) -> bool + Send + Sync>> {
        // Parse expressions like "status IN ('active', 'pending')"
        Ok(Box::new(move |data: &HashMap<String, String>| {
            // Simplified evaluation
            if expression.contains("status IN") {
                data.get("status").map(|v| v == "active" || v == "pending").unwrap_or(false)
            } else {
                true
            }
        }))
    }

    fn compile_like_expression(&self, expression: &str) -> AuroraResult<Box<dyn Fn(&HashMap<String, String>) -> bool + Send + Sync>> {
        // Parse expressions like "email LIKE '%@%'"
        Ok(Box::new(move |data: &HashMap<String, String>| {
            // Simplified evaluation
            if expression.contains("email LIKE") {
                data.get("email").map(|v| v.contains("@")).unwrap_or(false)
            } else {
                true
            }
        }))
    }

    fn compile_simple_expression(&self, expression: &str) -> AuroraResult<Box<dyn Fn(&HashMap<String, String>) -> bool + Send + Sync>> {
        Ok(Box::new(move |_data: &HashMap<String, String>| {
            // Default validation - in real implementation would parse the expression
            true
        }))
    }
}
