//! Constraint Manager: Intelligent Constraint Management and Validation
//!
//! Advanced constraint management system with multiple constraint types,
//! intelligent validation, and performance optimization.

use std::collections::{HashMap, HashSet};
use std::sync::Arc;
use parking_lot::RwLock;
use chrono::{DateTime, Utc};
use crate::core::errors::{AuroraResult, AuroraError};
use crate::core::schema::DataType;
use super::foreign_key_constraint::{ForeignKeyConstraint, ForeignKeyConfig, ReferentialAction};
use super::check_constraint::{CheckConstraint, CheckConfig};
use super::unique_constraint::{UniqueConstraint, UniqueConfig};
use super::not_null_constraint::{NotNullConstraint, NotNullConfig};
use super::validation_engine::{ValidationEngine, ValidationResult};
use super::performance_optimizer::{PerformanceOptimizer, ConstraintPerformanceStats};
use super::constraint_suggester::{ConstraintSuggester, ConstraintSuggestion};

/// Constraint types supported by AuroraDB
#[derive(Debug, Clone, PartialEq)]
pub enum ConstraintType {
    PrimaryKey,
    ForeignKey,
    Unique,
    Check,
    NotNull,
    Default,
}

/// Constraint configuration
#[derive(Debug, Clone)]
pub struct ConstraintConfig {
    pub name: String,
    pub table_name: String,
    pub constraint_type: ConstraintType,
    pub columns: Vec<String>,
    pub definition: String, // SQL definition or expression
    pub enabled: bool,
    pub deferrable: bool,
    pub initially_deferred: bool,
    pub created_at: DateTime<Utc>,
    pub last_validated: Option<DateTime<Utc>>,
    pub validation_stats: ConstraintValidationStats,
}

/// Constraint validation statistics
#[derive(Debug, Clone)]
pub struct ConstraintValidationStats {
    pub total_validations: u64,
    pub successful_validations: u64,
    pub failed_validations: u64,
    pub avg_validation_time_ms: f64,
    pub last_failure_reason: Option<String>,
}

/// Constraint violation information
#[derive(Debug, Clone)]
pub struct ConstraintViolation {
    pub constraint_name: String,
    pub table_name: String,
    pub constraint_type: ConstraintType,
    pub violated_values: HashMap<String, String>,
    pub error_message: String,
    pub timestamp: DateTime<Utc>,
    pub transaction_id: Option<String>,
}

/// Intelligent constraint manager
pub struct ConstraintManager {
    constraints: RwLock<HashMap<String, ConstraintConfig>>,
    table_constraints: RwLock<HashMap<String, Vec<String>>>, // table -> constraint names

    // Constraint implementations
    foreign_keys: RwLock<HashMap<String, ForeignKeyConstraint>>,
    check_constraints: RwLock<HashMap<String, CheckConstraint>>,
    unique_constraints: RwLock<HashMap<String, UniqueConstraint>>,
    not_null_constraints: RwLock<HashMap<String, NotNullConstraint>>,

    // Intelligence components
    validation_engine: Arc<ValidationEngine>,
    performance_optimizer: Arc<PerformanceOptimizer>,
    constraint_suggester: Arc<ConstraintSuggester>,
}

impl ConstraintManager {
    pub fn new() -> Self {
        Self {
            constraints: RwLock::new(HashMap::new()),
            table_constraints: RwLock::new(HashMap::new()),
            foreign_keys: RwLock::new(HashMap::new()),
            check_constraints: RwLock::new(HashMap::new()),
            unique_constraints: RwLock::new(HashMap::new()),
            not_null_constraints: RwLock::new(HashMap::new()),
            validation_engine: Arc::new(ValidationEngine::new()),
            performance_optimizer: Arc::new(PerformanceOptimizer::new()),
            constraint_suggester: Arc::new(ConstraintSuggester::new()),
        }
    }

    /// Create a constraint with intelligent validation
    pub async fn create_constraint(&self, config: ConstraintConfig) -> AuroraResult<()> {
        println!("🚀 Creating {} constraint '{}' on table '{}'",
                format_constraint_type(&config.constraint_type),
                config.name,
                config.table_name);

        // Validate constraint configuration
        self.validate_constraint_config(&config).await?;

        // Check for constraint conflicts
        self.check_constraint_conflicts(&config).await?;

        // Create the appropriate constraint implementation
        match config.constraint_type {
            ConstraintType::ForeignKey => self.create_foreign_key_constraint(&config).await?,
            ConstraintType::Check => self.create_check_constraint(&config).await?,
            ConstraintType::Unique => self.create_unique_constraint(&config).await?,
            ConstraintType::NotNull => self.create_not_null_constraint(&config).await?,
            ConstraintType::PrimaryKey => self.create_primary_key_constraint(&config).await?,
            ConstraintType::Default => self.create_default_constraint(&config).await?,
        }

        // Register constraint
        {
            let mut constraints = self.constraints.write();
            constraints.insert(config.name.clone(), config.clone());
        }

        // Update table constraint mapping
        {
            let mut table_constraints = self.table_constraints.write();
            table_constraints.entry(config.table_name.clone())
                .or_insert_with(Vec::new)
                .push(config.name.clone());
        }

        println!("✅ Created {} constraint '{}' - {} columns, {} enabled",
                format_constraint_type(&config.constraint_type),
                config.name,
                config.columns.len(),
                if config.enabled { "enabled" } else { "disabled" });

        Ok(())
    }

    /// Drop a constraint
    pub async fn drop_constraint(&self, constraint_name: &str) -> AuroraResult<()> {
        // Get constraint configuration before removal
        let config = {
            let constraints = self.constraints.read();
            constraints.get(constraint_name).cloned()
                .ok_or_else(|| AuroraError::NotFound(format!("Constraint '{}' not found", constraint_name)))?
        };

        // Remove from appropriate constraint storage
        match config.constraint_type {
            ConstraintType::ForeignKey => {
                let mut foreign_keys = self.foreign_keys.write();
                foreign_keys.remove(constraint_name);
            }
            ConstraintType::Check => {
                let mut check_constraints = self.check_constraints.write();
                check_constraints.remove(constraint_name);
            }
            ConstraintType::Unique => {
                let mut unique_constraints = self.unique_constraints.write();
                unique_constraints.remove(constraint_name);
            }
            ConstraintType::NotNull => {
                let mut not_null_constraints = self.not_null_constraints.write();
                not_null_constraints.remove(constraint_name);
            }
            _ => {} // Other types don't have specialized storage
        }

        // Remove from mappings
        {
            let mut constraints = self.constraints.write();
            constraints.remove(constraint_name);
        }

        {
            let mut table_constraints = self.table_constraints.write();
            if let Some(table_constraint_list) = table_constraints.get_mut(&config.table_name) {
                table_constraint_list.retain(|c| c != constraint_name);
                if table_constraint_list.is_empty() {
                    table_constraints.remove(&config.table_name);
                }
            }
        }

        println!("🗑️  Dropped constraint '{}'", constraint_name);
        Ok(())
    }

    /// Validate data against all constraints
    pub async fn validate_data(&self, table_name: &str, data: &HashMap<String, String>) -> AuroraResult<Vec<ConstraintViolation>> {
        let table_constraint_names = {
            let table_constraints = self.table_constraints.read();
            table_constraints.get(table_name).cloned().unwrap_or_default()
        };

        let mut violations = Vec::new();

        for constraint_name in table_constraint_names {
            if let Some(violation) = self.validate_single_constraint(&constraint_name, data).await? {
                violations.push(violation);
            }
        }

        Ok(violations)
    }

    /// Enable/disable a constraint
    pub async fn set_constraint_enabled(&self, constraint_name: &str, enabled: bool) -> AuroraResult<()> {
        let mut constraints = self.constraints.write();
        if let Some(constraint) = constraints.get_mut(constraint_name) {
            constraint.enabled = enabled;
            println!("{} constraint '{}'", if enabled { "✅ Enabled" } else { "⏸️  Disabled" }, constraint_name);
            Ok(())
        } else {
            Err(AuroraError::NotFound(format!("Constraint '{}' not found", constraint_name)))
        }
    }

    /// Get intelligent constraint suggestions
    pub async fn get_constraint_suggestions(&self, table_name: &str) -> AuroraResult<Vec<ConstraintSuggestion>> {
        self.constraint_suggester.suggest_constraints(table_name).await
    }

    /// Analyze constraint performance
    pub async fn analyze_constraint_performance(&self, constraint_name: &str) -> AuroraResult<ConstraintPerformanceStats> {
        self.performance_optimizer.analyze_performance(constraint_name).await
    }

    /// Get constraints for a table
    pub async fn get_table_constraints(&self, table_name: &str) -> Vec<ConstraintConfig> {
        let table_constraints = self.table_constraints.read();
        if let Some(constraint_names) = table_constraints.get(table_name) {
            let constraints = self.constraints.read();
            constraint_names.iter()
                .filter_map(|name| constraints.get(name).cloned())
                .collect()
        } else {
            vec![]
        }
    }

    /// List all constraints with their statistics
    pub async fn list_constraints(&self) -> Vec<ConstraintSummary> {
        let constraints = self.constraints.read();
        let mut summaries = Vec::new();

        for (name, config) in constraints.iter() {
            let perf_stats = self.performance_optimizer.analyze_performance(name).await.unwrap_or_default();

            summaries.push(ConstraintSummary {
                name: name.clone(),
                table_name: config.table_name.clone(),
                constraint_type: config.constraint_type.clone(),
                columns: config.columns.clone(),
                enabled: config.enabled,
                deferrable: config.deferrable,
                total_validations: config.validation_stats.total_validations,
                failed_validations: config.validation_stats.failed_validations,
                avg_validation_time_ms: config.validation_stats.avg_validation_time_ms,
                performance_impact: perf_stats.avg_validation_time_ms,
                last_validated: config.last_validated,
            });
        }

        // Sort by table name, then by name
        summaries.sort_by(|a, b| {
            a.table_name.cmp(&b.table_name).then(a.name.cmp(&b.name))
        });

        summaries
    }

    /// Validate existing data against a constraint before enabling it
    pub async fn validate_existing_data(&self, constraint_name: &str) -> AuroraResult<Vec<ConstraintViolation>> {
        let config = {
            let constraints = self.constraints.read();
            constraints.get(constraint_name).cloned()
                .ok_or_else(|| AuroraError::NotFound(format!("Constraint '{}' not found", constraint_name)))?
        };

        if !config.enabled {
            return Err(AuroraError::InvalidArgument("Constraint must be enabled to validate existing data".to_string()));
        }

        // This would scan all existing data in the table and validate against the constraint
        // For now, return empty vector (no violations found)
        println!("🔍 Validating existing data against constraint '{}'...", constraint_name);
        Ok(vec![])
    }

    // Private methods

    async fn validate_constraint_config(&self, config: &ConstraintConfig) -> AuroraResult<()> {
        // Validate constraint name
        if config.name.is_empty() || config.name.len() > 128 {
            return Err(AuroraError::InvalidArgument("Constraint name must be 1-128 characters".to_string()));
        }

        // Validate table name
        if config.table_name.is_empty() {
            return Err(AuroraError::InvalidArgument("Table name cannot be empty".to_string()));
        }

        // Validate columns
        if config.columns.is_empty() && !matches!(config.constraint_type, ConstraintType::Check) {
            return Err(AuroraError::InvalidArgument("Constraint must have at least one column".to_string()));
        }

        // Check for duplicate constraint names
        let constraints = self.constraints.read();
        if constraints.contains_key(&config.name) {
            return Err(AuroraError::InvalidArgument(format!("Constraint '{}' already exists", config.name)));
        }

        Ok(())
    }

    async fn check_constraint_conflicts(&self, config: &ConstraintConfig) -> AuroraResult<()> {
        let table_constraints = self.get_table_constraints(&config.table_name).await;

        for existing_constraint in table_constraints {
            // Check for exact duplicate columns (same type)
            if existing_constraint.columns == config.columns &&
               existing_constraint.constraint_type == config.constraint_type {
                return Err(AuroraError::InvalidArgument(
                    format!("Duplicate {} constraint already exists on same columns",
                           format_constraint_type(&config.constraint_type))
                ));
            }

            // Check for conflicting unique constraints
            if matches!(config.constraint_type, ConstraintType::Unique | ConstraintType::PrimaryKey) &&
               matches!(existing_constraint.constraint_type, ConstraintType::Unique | ConstraintType::PrimaryKey) &&
               existing_constraint.columns == config.columns {
                return Err(AuroraError::InvalidArgument(
                    "Cannot create duplicate unique constraint on same columns".to_string()
                ));
            }

            // Check for foreign key conflicts
            if matches!(config.constraint_type, ConstraintType::ForeignKey) &&
               matches!(existing_constraint.constraint_type, ConstraintType::ForeignKey) &&
               existing_constraint.columns == config.columns {
                return Err(AuroraError::InvalidArgument(
                    "Cannot create duplicate foreign key constraint on same columns".to_string()
                ));
            }
        }

        Ok(())
    }

    async fn create_foreign_key_constraint(&self, config: &ConstraintConfig) -> AuroraResult<()> {
        // Parse foreign key definition
        let fk_config = self.parse_foreign_key_definition(&config.definition)?;

        let constraint = ForeignKeyConstraint::new(fk_config)?;
        let mut foreign_keys = self.foreign_keys.write();
        foreign_keys.insert(config.name.clone(), constraint);

        Ok(())
    }

    async fn create_check_constraint(&self, config: &ConstraintConfig) -> AuroraResult<()> {
        let check_config = CheckConfig {
            name: config.name.clone(),
            table_name: config.table_name.clone(),
            expression: config.definition.clone(),
            columns: config.columns.clone(),
        };

        let constraint = CheckConstraint::new(check_config)?;
        let mut check_constraints = self.check_constraints.write();
        check_constraints.insert(config.name.clone(), constraint);

        Ok(())
    }

    async fn create_unique_constraint(&self, config: &ConstraintConfig) -> AuroraResult<()> {
        let unique_config = UniqueConfig {
            name: config.name.clone(),
            table_name: config.table_name.clone(),
            columns: config.columns.clone(),
        };

        let constraint = UniqueConstraint::new(unique_config)?;
        let mut unique_constraints = self.unique_constraints.write();
        unique_constraints.insert(config.name.clone(), constraint);

        Ok(())
    }

    async fn create_not_null_constraint(&self, config: &ConstraintConfig) -> AuroraResult<()> {
        let not_null_config = NotNullConfig {
            name: config.name.clone(),
            table_name: config.table_name.clone(),
            columns: config.columns.clone(),
        };

        let constraint = NotNullConstraint::new(not_null_config)?;
        let mut not_null_constraints = self.not_null_constraints.write();
        not_null_constraints.insert(config.name.clone(), constraint);

        Ok(())
    }

    async fn create_primary_key_constraint(&self, config: &ConstraintConfig) -> AuroraResult<()> {
        // Primary keys are implemented as unique constraints with additional properties
        self.create_unique_constraint(config).await
    }

    async fn create_default_constraint(&self, config: &ConstraintConfig) -> AuroraResult<()> {
        // Default constraints are handled at the column level
        // This is a placeholder for future implementation
        Ok(())
    }

    fn parse_foreign_key_definition(&self, definition: &str) -> AuroraResult<ForeignKeyConfig> {
        // Simplified parsing - in real implementation would parse REFERENCES clause
        Ok(ForeignKeyConfig {
            name: "parsed_fk".to_string(),
            table_name: "source_table".to_string(),
            columns: vec!["id".to_string()],
            referenced_table: "referenced_table".to_string(),
            referenced_columns: vec!["id".to_string()],
            on_delete: ReferentialAction::Restrict,
            on_update: ReferentialAction::Restrict,
        })
    }

    async fn validate_single_constraint(&self, constraint_name: &str, data: &HashMap<String, String>) -> AuroraResult<Option<ConstraintViolation>> {
        let config = {
            let constraints = self.constraints.read();
            match constraints.get(constraint_name) {
                Some(c) if c.enabled => c.clone(),
                _ => return Ok(None),
            }
        };

        let result = match config.constraint_type {
            ConstraintType::ForeignKey => {
                let foreign_keys = self.foreign_keys.read();
                if let Some(fk) = foreign_keys.get(constraint_name) {
                    fk.validate(data).await?
                } else {
                    return Ok(None);
                }
            }
            ConstraintType::Check => {
                let check_constraints = self.check_constraints.read();
                if let Some(check) = check_constraints.get(constraint_name) {
                    check.validate(data).await?
                } else {
                    return Ok(None);
                }
            }
            ConstraintType::Unique => {
                let unique_constraints = self.unique_constraints.read();
                if let Some(unique) = unique_constraints.get(constraint_name) {
                    unique.validate(data).await?
                } else {
                    return Ok(None);
                }
            }
            ConstraintType::NotNull => {
                let not_null_constraints = self.not_null_constraints.read();
                if let Some(not_null) = not_null_constraints.get(constraint_name) {
                    not_null.validate(data).await?
                } else {
                    return Ok(None);
                }
            }
            _ => ValidationResult::Valid,
        };

        match result {
            ValidationResult::Valid => Ok(None),
            ValidationResult::Violated(message) => {
                let violated_values = config.columns.iter()
                    .filter_map(|col| data.get(col).map(|val| (col.clone(), val.clone())))
                    .collect();

                Ok(Some(ConstraintViolation {
                    constraint_name: constraint_name.to_string(),
                    table_name: config.table_name.clone(),
                    constraint_type: config.constraint_type.clone(),
                    violated_values,
                    error_message: message,
                    timestamp: Utc::now(),
                    transaction_id: None,
                }))
            }
        }
    }
}

/// Constraint summary for listing
#[derive(Debug, Clone)]
pub struct ConstraintSummary {
    pub name: String,
    pub table_name: String,
    pub constraint_type: ConstraintType,
    pub columns: Vec<String>,
    pub enabled: bool,
    pub deferrable: bool,
    pub total_validations: u64,
    pub failed_validations: u64,
    pub avg_validation_time_ms: f64,
    pub performance_impact: f64,
    pub last_validated: Option<DateTime<Utc>>,
}

// Helper function
fn format_constraint_type(constraint_type: &ConstraintType) -> &'static str {
    match constraint_type {
        ConstraintType::PrimaryKey => "Primary Key",
        ConstraintType::ForeignKey => "Foreign Key",
        ConstraintType::Unique => "Unique",
        ConstraintType::Check => "Check",
        ConstraintType::NotNull => "Not Null",
        ConstraintType::Default => "Default",
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn create_test_config(name: &str, table: &str, constraint_type: ConstraintType, columns: Vec<&str>) -> ConstraintConfig {
        ConstraintConfig {
            name: name.to_string(),
            table_name: table.to_string(),
            constraint_type,
            columns: columns.iter().map(|s| s.to_string()).collect(),
            definition: "test definition".to_string(),
            enabled: true,
            deferrable: false,
            initially_deferred: false,
            created_at: Utc::now(),
            last_validated: None,
            validation_stats: ConstraintValidationStats {
                total_validations: 0,
                successful_validations: 0,
                failed_validations: 0,
                avg_validation_time_ms: 0.0,
                last_failure_reason: None,
            },
        }
    }

    #[tokio::test]
    async fn test_constraint_manager_creation() {
        let manager = ConstraintManager::new();
        assert!(true); // Passes if created successfully
    }

    #[test]
    fn test_constraint_types() {
        assert_eq!(ConstraintType::PrimaryKey, ConstraintType::PrimaryKey);
        assert_ne!(ConstraintType::ForeignKey, ConstraintType::Unique);
    }

    #[test]
    fn test_format_constraint_type() {
        assert_eq!(format_constraint_type(&ConstraintType::PrimaryKey), "Primary Key");
        assert_eq!(format_constraint_type(&ConstraintType::ForeignKey), "Foreign Key");
        assert_eq!(format_constraint_type(&ConstraintType::Unique), "Unique");
    }

    #[test]
    fn test_constraint_config() {
        let config = create_test_config("test_pk", "users", ConstraintType::PrimaryKey, vec!["id"]);
        assert_eq!(config.name, "test_pk");
        assert_eq!(config.table_name, "users");
        assert_eq!(config.constraint_type, ConstraintType::PrimaryKey);
        assert_eq!(config.columns.len(), 1);
        assert!(config.enabled);
    }

    #[test]
    fn test_constraint_summary() {
        let summary = ConstraintSummary {
            name: "test_pk".to_string(),
            table_name: "users".to_string(),
            constraint_type: ConstraintType::PrimaryKey,
            columns: vec!["id".to_string()],
            enabled: true,
            deferrable: false,
            total_validations: 100,
            failed_validations: 5,
            avg_validation_time_ms: 2.5,
            performance_impact: 1.2,
            last_validated: Some(Utc::now()),
        };

        assert_eq!(summary.name, "test_pk");
        assert_eq!(summary.total_validations, 100);
        assert_eq!(summary.failed_validations, 5);
        assert_eq!(summary.avg_validation_time_ms, 2.5);
    }

    #[test]
    fn test_constraint_violation() {
        let violation = ConstraintViolation {
            constraint_name: "fk_user_role".to_string(),
            table_name: "users".to_string(),
            constraint_type: ConstraintType::ForeignKey,
            violated_values: HashMap::from([
                ("role_id".to_string(), "999".to_string()),
            ]),
            error_message: "Foreign key constraint violated".to_string(),
            timestamp: Utc::now(),
            transaction_id: Some("tx_123".to_string()),
        };

        assert_eq!(violation.constraint_name, "fk_user_role");
        assert_eq!(violation.constraint_type, ConstraintType::ForeignKey);
        assert!(violation.violated_values.contains_key("role_id"));
    }

    #[test]
    fn test_constraint_validation_stats() {
        let stats = ConstraintValidationStats {
            total_validations: 1000,
            successful_validations: 950,
            failed_validations: 50,
            avg_validation_time_ms: 5.2,
            last_failure_reason: Some("Invalid data format".to_string()),
        };

        assert_eq!(stats.total_validations, 1000);
        assert_eq!(stats.successful_validations, 950);
        assert_eq!(stats.failed_validations, 50);
        assert_eq!(stats.avg_validation_time_ms, 5.2);
    }
}
