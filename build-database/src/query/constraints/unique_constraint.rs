//! Unique Constraint: Intelligent Uniqueness Validation with NULL Handling

use std::collections::{HashMap, HashSet};
use crate::core::errors::AuroraResult;
use super::validation_engine::ValidationResult;

/// Unique constraint configuration
#[derive(Debug, Clone)]
pub struct UniqueConfig {
    pub name: String,
    pub table_name: String,
    pub columns: Vec<String>,
}

/// Unique constraint statistics
#[derive(Debug, Clone)]
pub struct UniqueStats {
    pub total_validations: u64,
    pub uniqueness_violations: u64,
    pub avg_lookup_time_ms: f64,
    pub index_effectiveness: f64,
}

/// Intelligent unique constraint
pub struct UniqueConstraint {
    config: UniqueConfig,
    stats: std::sync::Mutex<UniqueStats>,
    value_index: std::sync::Mutex<HashSet<String>>, // Composite key -> existence
}

impl UniqueConstraint {
    pub fn new(config: UniqueConfig) -> AuroraResult<Self> {
        Ok(Self {
            config,
            stats: std::sync::Mutex::new(UniqueStats {
                total_validations: 0,
                uniqueness_violations: 0,
                avg_lookup_time_ms: 0.0,
                index_effectiveness: 0.0,
            }),
            value_index: std::sync::Mutex::new(HashSet::new()),
        })
    }

    pub async fn validate(&self, data: &HashMap<String, String>) -> AuroraResult<ValidationResult> {
        let start_time = std::time::Instant::now();

        // Create composite key from unique columns
        let composite_key = self.create_composite_key(data);

        // Check for NULL values (NULLs are allowed in unique constraints)
        if self.has_null_values(&composite_key) {
            return Ok(ValidationResult::Valid);
        }

        // Check uniqueness
        let exists = {
            let index = self.value_index.lock().unwrap();
            index.contains(&composite_key)
        };

        let lookup_time = start_time.elapsed().as_millis() as f64;

        // Update statistics
        {
            let mut stats = self.stats.lock().unwrap();
            stats.total_validations += 1;
            stats.avg_lookup_time_ms = (stats.avg_lookup_time_ms * (stats.total_validations - 1) as f64 + lookup_time) / stats.total_validations as f64;

            if exists {
                stats.uniqueness_violations += 1;
            }
        }

        if exists {
            Ok(ValidationResult::Violated(
                format!("Unique constraint '{}' violated: duplicate values for columns {:?}",
                       self.config.name, self.config.columns)
            ))
        } else {
            // Add to index
            {
                let mut index = self.value_index.lock().unwrap();
                index.insert(composite_key);
            }
            Ok(ValidationResult::Valid)
        }
    }

    pub fn get_stats(&self) -> UniqueStats {
        self.stats.lock().unwrap().clone()
    }

    pub async fn remove_value(&self, data: &HashMap<String, String>) -> AuroraResult<()> {
        let composite_key = self.create_composite_key(data);
        let mut index = self.value_index.lock().unwrap();
        index.remove(&composite_key);
        Ok(())
    }

    fn create_composite_key(&self, data: &HashMap<String, String>) -> String {
        self.config.columns.iter()
            .map(|col| data.get(col).cloned().unwrap_or_else(|| "NULL".to_string()))
            .collect::<Vec<String>>()
            .join("|")
    }

    fn has_null_values(&self, composite_key: &str) -> bool {
        composite_key.split('|').any(|part| part == "NULL")
    }
}
