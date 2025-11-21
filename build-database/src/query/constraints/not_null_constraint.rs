//! Not Null Constraint: Intelligent NULL Value Validation

use std::collections::HashMap;
use crate::core::errors::AuroraResult;
use super::validation_engine::ValidationResult;

/// Not null constraint configuration
#[derive(Debug, Clone)]
pub struct NotNullConfig {
    pub name: String,
    pub table_name: String,
    pub columns: Vec<String>,
}

/// Not null constraint statistics
#[derive(Debug, Clone)]
pub struct NotNullStats {
    pub total_validations: u64,
    pub null_violations: u64,
    pub avg_validation_time_ms: f64,
}

/// Intelligent not null constraint
pub struct NotNullConstraint {
    config: NotNullConfig,
    stats: std::sync::Mutex<NotNullStats>,
}

impl NotNullConstraint {
    pub fn new(config: NotNullConfig) -> AuroraResult<Self> {
        Ok(Self {
            config,
            stats: std::sync::Mutex::new(NotNullStats {
                total_validations: 0,
                null_violations: 0,
                avg_validation_time_ms: 0.0,
            }),
        })
    }

    pub async fn validate(&self, data: &HashMap<String, String>) -> AuroraResult<ValidationResult> {
        let start_time = std::time::Instant::now();

        for column in &self.config.columns {
            if let Some(value) = data.get(column) {
                if value.trim().is_empty() || value == "NULL" {
                    let validation_time = start_time.elapsed().as_millis() as f64;

                    // Update statistics
                    {
                        let mut stats = self.stats.lock().unwrap();
                        stats.total_validations += 1;
                        stats.null_violations += 1;
                        stats.avg_validation_time_ms = (stats.avg_validation_time_ms * (stats.total_validations - 1) as f64 + validation_time) / stats.total_validations as f64;
                    }

                    return Ok(ValidationResult::Violated(
                        format!("NOT NULL constraint '{}' violated: column '{}' cannot be NULL",
                               self.config.name, column)
                    ));
                }
            } else {
                let validation_time = start_time.elapsed().as_millis() as f64;

                // Update statistics
                {
                    let mut stats = self.stats.lock().unwrap();
                    stats.total_validations += 1;
                    stats.null_violations += 1;
                    stats.avg_validation_time_ms = (stats.avg_validation_time_ms * (stats.total_validations - 1) as f64 + validation_time) / stats.total_validations as f64;
                }

                return Ok(ValidationResult::Violated(
                    format!("NOT NULL constraint '{}' violated: column '{}' is missing",
                           self.config.name, column)
                ));
            }
        }

        let validation_time = start_time.elapsed().as_millis() as f64;

        // Update statistics
        {
            let mut stats = self.stats.lock().unwrap();
            stats.total_validations += 1;
            stats.avg_validation_time_ms = (stats.avg_validation_time_ms * (stats.total_validations - 1) as f64 + validation_time) / stats.total_validations as f64;
        }

        Ok(ValidationResult::Valid)
    }

    pub fn get_stats(&self) -> NotNullStats {
        self.stats.lock().unwrap().clone()
    }
}
