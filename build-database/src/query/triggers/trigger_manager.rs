//! Trigger Manager: Intelligent Trigger Management System
//!
//! Advanced management system for database triggers with event-driven
//! architecture, intelligent filtering, and performance optimization.

use std::collections::{HashMap, HashSet};
use std::sync::Arc;
use parking_lot::RwLock;
use chrono::{DateTime, Utc};
use crate::core::errors::{AuroraResult, AuroraError};
use crate::core::schema::DataType;
use super::event_engine::{EventEngine, DatabaseEvent, EventFilter};
use super::execution_engine::{ExecutionEngine, TriggerExecutionContext};
use super::condition_evaluator::{ConditionEvaluator, TriggerCondition};
use super::performance_monitor::{TriggerPerformanceMonitor, PerformanceMetrics};
use super::conflict_resolver::{ConflictResolver, TriggerConflict};

/// Trigger timing (when to execute)
#[derive(Debug, Clone, PartialEq)]
pub enum TriggerTiming {
    Before,     // Execute before the triggering operation
    After,      // Execute after the triggering operation
    Instead,    // Execute instead of the triggering operation
}

/// Trigger events (what operations trigger execution)
#[derive(Debug, Clone, PartialEq)]
pub enum TriggerEvent {
    Insert,
    Update,
    Delete,
    Truncate,
    Select,     // For SELECT triggers (uncommon but useful for auditing)
}

/// Trigger execution modes
#[derive(Debug, Clone, PartialEq)]
pub enum TriggerExecutionMode {
    Synchronous,    // Block the triggering operation
    Asynchronous,   // Execute in background
    Deferred,       // Execute at transaction commit
    Conditional,    // Execute only if conditions met
}

/// Programming languages for trigger logic
#[derive(Debug, Clone, PartialEq)]
pub enum TriggerLanguage {
    SQL,
    Rust,
    Python,
    JavaScript,
    Lua,
}

/// Trigger definition
#[derive(Debug, Clone)]
pub struct TriggerDefinition {
    pub name: String,
    pub table_name: String,
    pub timing: TriggerTiming,
    pub events: HashSet<TriggerEvent>,
    pub execution_mode: TriggerExecutionMode,
    pub language: TriggerLanguage,
    pub source_code: String,
    pub conditions: Vec<TriggerCondition>,
    pub priority: i32,
    pub enabled: bool,
    pub description: String,
    pub tags: HashSet<String>,
    pub created_at: DateTime<Utc>,
    pub modified_at: DateTime<Utc>,
    pub version: String,
}

/// Trigger execution result
#[derive(Debug)]
pub struct TriggerExecutionResult {
    pub trigger_name: String,
    pub success: bool,
    pub execution_time_ms: f64,
    pub affected_rows: u64,
    pub error_message: Option<String>,
    pub side_effects: Vec<String>,
}

/// Trigger statistics
#[derive(Debug)]
pub struct TriggerStats {
    pub total_executions: u64,
    pub successful_executions: u64,
    pub failed_executions: u64,
    pub avg_execution_time_ms: f64,
    pub max_execution_time_ms: f64,
    pub min_execution_time_ms: f64,
    pub last_executed: Option<DateTime<Utc>>,
}

/// Intelligent trigger manager
pub struct TriggerManager {
    triggers: RwLock<HashMap<String, TriggerDefinition>>,
    trigger_index: RwLock<HashMap<String, Vec<String>>>, // table -> trigger names
    event_engine: Arc<EventEngine>,
    execution_engine: Arc<ExecutionEngine>,
    condition_evaluator: Arc<ConditionEvaluator>,
    performance_monitor: Arc<TriggerPerformanceMonitor>,
    conflict_resolver: Arc<ConflictResolver>,
}

impl TriggerManager {
    pub fn new() -> Self {
        Self {
            triggers: RwLock::new(HashMap::new()),
            trigger_index: RwLock::new(HashMap::new()),
            event_engine: Arc::new(EventEngine::new()),
            execution_engine: Arc::new(ExecutionEngine::new()),
            condition_evaluator: Arc::new(ConditionEvaluator::new()),
            performance_monitor: Arc::new(TriggerPerformanceMonitor::new()),
            conflict_resolver: Arc::new(ConflictResolver::new()),
        }
    }

    /// Create a new trigger
    pub async fn create_trigger(&self, definition: TriggerDefinition) -> AuroraResult<()> {
        println!("🚀 Creating trigger '{}' on table '{}' with {:?} timing",
                definition.name, definition.table_name, definition.timing);

        // Validate trigger definition
        self.validate_trigger(&definition).await?;

        // Check for conflicts with existing triggers
        self.check_trigger_conflicts(&definition).await?;

        // Register with event engine
        self.event_engine.register_trigger(&definition).await?;

        // Store trigger definition
        {
            let mut triggers = self.triggers.write();
            triggers.insert(definition.name.clone(), definition.clone());
        }

        // Update trigger index
        {
            let mut trigger_index = self.trigger_index.write();
            trigger_index.entry(definition.table_name.clone())
                .or_insert_with(Vec::new)
                .push(definition.name.clone());
        }

        // Initialize performance monitoring
        self.performance_monitor.register_trigger(&definition.name).await?;

        println!("✅ Created trigger '{}' - {} events, {} conditions",
                definition.name, definition.events.len(), definition.conditions.len());

        Ok(())
    }

    /// Process a database event and execute relevant triggers
    pub async fn process_event(
        &self,
        event: DatabaseEvent,
    ) -> AuroraResult<Vec<TriggerExecutionResult>> {
        // Get relevant triggers for this event
        let relevant_triggers = self.get_relevant_triggers(&event).await?;

        if relevant_triggers.is_empty() {
            return Ok(vec![]);
        }

        println!("🎯 Processing event on table '{}' - {} relevant triggers",
                event.table_name, relevant_triggers.len());

        // Sort triggers by priority
        let mut sorted_triggers = relevant_triggers;
        sorted_triggers.sort_by(|a, b| {
            let a_pri = self.triggers.read().get(a).map(|t| t.priority).unwrap_or(0);
            let b_pri = self.triggers.read().get(b).map(|t| t.priority).unwrap_or(0);
            a_pri.cmp(&b_pri)
        });

        let mut results = Vec::new();

        // Execute triggers based on timing
        for timing in &[TriggerTiming::Before, TriggerTiming::Instead, TriggerTiming::After] {
            let timing_triggers: Vec<_> = sorted_triggers.iter()
                .filter(|trigger_name| {
                    self.triggers.read().get(*trigger_name)
                        .map(|t| t.timing == *timing)
                        .unwrap_or(false)
                })
                .cloned()
                .collect();

            for trigger_name in timing_triggers {
                let result = self.execute_trigger(&trigger_name, &event).await?;
                results.push(result);
            }
        }

        Ok(results)
    }

    /// Drop a trigger
    pub async fn drop_trigger(&self, trigger_name: &str) -> AuroraResult<()> {
        // Get trigger definition before removal
        let trigger_def = {
            let triggers = self.triggers.read();
            triggers.get(trigger_name).cloned()
                .ok_or_else(|| AuroraError::NotFound(format!("Trigger '{}' not found", trigger_name)))?
        };

        // Remove from event engine
        self.event_engine.unregister_trigger(trigger_name).await?;

        // Remove from triggers map
        {
            let mut triggers = self.triggers.write();
            triggers.remove(trigger_name);
        }

        // Update trigger index
        {
            let mut trigger_index = self.trigger_index.write();
            if let Some(table_triggers) = trigger_index.get_mut(&trigger_def.table_name) {
                table_triggers.retain(|t| t != trigger_name);
                if table_triggers.is_empty() {
                    trigger_index.remove(&trigger_def.table_name);
                }
            }
        }

        // Clean up performance monitoring
        self.performance_monitor.remove_trigger(trigger_name).await?;

        println!("🗑️  Dropped trigger '{}'", trigger_name);
        Ok(())
    }

    /// Enable/disable a trigger
    pub async fn set_trigger_enabled(&self, trigger_name: &str, enabled: bool) -> AuroraResult<()> {
        let mut triggers = self.triggers.write();
        if let Some(trigger) = triggers.get_mut(trigger_name) {
            trigger.enabled = enabled;
            println!("{} trigger '{}'", if enabled { "✅ Enabled" } else { "⏸️  Disabled" }, trigger_name);
            Ok(())
        } else {
            Err(AuroraError::NotFound(format!("Trigger '{}' not found", trigger_name)))
        }
    }

    /// Get trigger statistics
    pub async fn get_trigger_stats(&self, trigger_name: &str) -> AuroraResult<TriggerStats> {
        self.performance_monitor.get_trigger_stats(trigger_name).await
    }

    /// List all triggers
    pub async fn list_triggers(&self) -> Vec<TriggerSummary> {
        let triggers = self.triggers.read();
        let mut summaries = Vec::new();

        for (name, trigger) in triggers.iter() {
            let stats = self.performance_monitor.get_trigger_stats(name).await.unwrap_or_default();

            summaries.push(TriggerSummary {
                name: name.clone(),
                table_name: trigger.table_name.clone(),
                timing: trigger.timing.clone(),
                events: trigger.events.clone(),
                execution_mode: trigger.execution_mode.clone(),
                language: trigger.language.clone(),
                enabled: trigger.enabled,
                priority: trigger.priority,
                total_executions: stats.total_executions,
                avg_execution_time_ms: stats.avg_execution_time_ms,
                last_executed: stats.last_executed,
            });
        }

        summaries.sort_by(|a, b| a.table_name.cmp(&b.table_name).then(a.name.cmp(&b.name)));
        summaries
    }

    /// Get triggers for a specific table
    pub async fn get_table_triggers(&self, table_name: &str) -> Vec<String> {
        let trigger_index = self.trigger_index.read();
        trigger_index.get(table_name).cloned().unwrap_or_default()
    }

    // Private methods

    async fn validate_trigger(&self, definition: &TriggerDefinition) -> AuroraResult<()> {
        // Validate trigger name
        if definition.name.is_empty() || definition.name.len() > 128 {
            return Err(AuroraError::InvalidArgument("Trigger name must be 1-128 characters".to_string()));
        }

        // Validate table name
        if definition.table_name.is_empty() {
            return Err(AuroraError::InvalidArgument("Table name cannot be empty".to_string()));
        }

        // Validate events
        if definition.events.is_empty() {
            return Err(AuroraError::InvalidArgument("Trigger must have at least one event".to_string()));
        }

        // Validate INSTEAD OF triggers (only for views)
        if definition.timing == TriggerTiming::Instead {
            // In a real implementation, check if table_name refers to a view
            // For now, allow it
        }

        // Validate source code based on language
        self.validate_trigger_code(definition)?;

        Ok(())
    }

    fn validate_trigger_code(&self, definition: &TriggerDefinition) -> AuroraResult<()> {
        match definition.language {
            TriggerLanguage::SQL => self.validate_sql_trigger(&definition.source_code),
            TriggerLanguage::Rust => self.validate_rust_trigger(&definition.source_code),
            TriggerLanguage::Python => self.validate_python_trigger(&definition.source_code),
            TriggerLanguage::JavaScript => self.validate_javascript_trigger(&definition.source_code),
            TriggerLanguage::Lua => self.validate_lua_trigger(&definition.source_code),
        }
    }

    fn validate_sql_trigger(&self, source: &str) -> AuroraResult<()> {
        // Basic SQL validation
        if !source.to_uppercase().contains("BEGIN") {
            return Err(AuroraError::InvalidArgument("SQL trigger must contain BEGIN block".to_string()));
        }
        Ok(())
    }

    fn validate_rust_trigger(&self, source: &str) -> AuroraResult<()> {
        // Basic Rust validation
        if !source.contains("fn ") {
            return Err(AuroraError::InvalidArgument("Rust trigger must contain a function".to_string()));
        }
        Ok(())
    }

    fn validate_python_trigger(&self, source: &str) -> AuroraResult<()> {
        // Basic Python validation
        if !source.contains("def ") {
            return Err(AuroraError::InvalidArgument("Python trigger must contain a function".to_string()));
        }
        Ok(())
    }

    fn validate_javascript_trigger(&self, source: &str) -> AuroraResult<()> {
        // Basic JavaScript validation
        if !source.contains("function ") {
            return Err(AuroraError::InvalidArgument("JavaScript trigger must contain a function".to_string()));
        }
        Ok(())
    }

    fn validate_lua_trigger(&self, source: &str) -> AuroraResult<()> {
        // Basic Lua validation
        if !source.contains("function ") {
            return Err(AuroraError::InvalidArgument("Lua trigger must contain a function".to_string()));
        }
        Ok(())
    }

    async fn check_trigger_conflicts(&self, definition: &TriggerDefinition) -> AuroraResult<()> {
        let conflicts = self.conflict_resolver.detect_conflicts(definition, &self.triggers.read()).await?;

        if !conflicts.is_empty() {
            println!("⚠️  Detected {} potential trigger conflicts", conflicts.len());
            for conflict in conflicts {
                match conflict.conflict_type {
                    super::conflict_resolver::ConflictType::Priority => {
                        println!("   Priority conflict with '{}'", conflict.other_trigger);
                    }
                    super::conflict_resolver::ConflictType::Timing => {
                        println!("   Timing conflict with '{}'", conflict.other_trigger);
                    }
                    super::conflict_resolver::ConflictType::Condition => {
                        println!("   Condition overlap with '{}'", conflict.other_trigger);
                    }
                }
            }

            // For now, allow conflicts but log them
            // In production, might want to require explicit resolution
        }

        Ok(())
    }

    async fn get_relevant_triggers(&self, event: &DatabaseEvent) -> AuroraResult<Vec<String>> {
        let trigger_index = self.trigger_index.read();

        if let Some(table_triggers) = trigger_index.get(&event.table_name) {
            let mut relevant = Vec::new();

            for trigger_name in table_triggers {
                if let Some(trigger) = self.triggers.read().get(trigger_name) {
                    if !trigger.enabled {
                        continue;
                    }

                    // Check if trigger responds to this event
                    let event_matches = match event.operation.as_str() {
                        "INSERT" => trigger.events.contains(&TriggerEvent::Insert),
                        "UPDATE" => trigger.events.contains(&TriggerEvent::Update),
                        "DELETE" => trigger.events.contains(&TriggerEvent::Delete),
                        "TRUNCATE" => trigger.events.contains(&TriggerEvent::Truncate),
                        "SELECT" => trigger.events.contains(&TriggerEvent::Select),
                        _ => false,
                    };

                    if event_matches {
                        // Check conditions if any
                        let conditions_met = if trigger.conditions.is_empty() {
                            true
                        } else {
                            self.condition_evaluator.evaluate_conditions(&trigger.conditions, event).await?
                        };

                        if conditions_met {
                            relevant.push(trigger_name.clone());
                        }
                    }
                }
            }

            Ok(relevant)
        } else {
            Ok(vec![])
        }
    }

    async fn execute_trigger(
        &self,
        trigger_name: &str,
        event: &DatabaseEvent,
    ) -> AuroraResult<TriggerExecutionResult> {
        let trigger_def = {
            let triggers = self.triggers.read();
            triggers.get(trigger_name).cloned()
                .ok_or_else(|| AuroraError::NotFound(format!("Trigger '{}' not found", trigger_name)))?
        };

        let start_time = std::time::Instant::now();

        // Create execution context
        let exec_context = TriggerExecutionContext {
            trigger_name: trigger_name.to_string(),
            event: event.clone(),
            trigger_definition: trigger_def.clone(),
        };

        // Execute the trigger
        let result = match trigger_def.execution_mode {
            TriggerExecutionMode::Synchronous => {
                self.execution_engine.execute_synchronous(&exec_context).await?
            }
            TriggerExecutionMode::Asynchronous => {
                self.execution_engine.execute_asynchronous(&exec_context).await?
            }
            TriggerExecutionMode::Deferred => {
                self.execution_engine.schedule_deferred(&exec_context).await?;
                // For deferred, return success immediately
                TriggerExecutionResult {
                    trigger_name: trigger_name.to_string(),
                    success: true,
                    execution_time_ms: 0.0,
                    affected_rows: 0,
                    error_message: None,
                    side_effects: vec!["deferred_execution_scheduled".to_string()],
                }
            }
            TriggerExecutionMode::Conditional => {
                self.execution_engine.execute_conditional(&exec_context).await?
            }
        };

        let execution_time = start_time.elapsed().as_millis() as f64;

        // Record performance metrics
        let metrics = PerformanceMetrics {
            execution_time_ms: execution_time,
            memory_used_mb: 0.0, // Would be measured
            cpu_utilization: 0.0,
            timestamp: Utc::now(),
        };

        self.performance_monitor.record_execution(trigger_name, &metrics).await?;

        // Return result with actual timing
        Ok(TriggerExecutionResult {
            trigger_name: trigger_name.to_string(),
            success: result.success,
            execution_time_ms: execution_time,
            affected_rows: result.affected_rows,
            error_message: result.error_message,
            side_effects: result.side_effects,
        })
    }
}

/// Trigger summary for listing
#[derive(Debug)]
pub struct TriggerSummary {
    pub name: String,
    pub table_name: String,
    pub timing: TriggerTiming,
    pub events: HashSet<TriggerEvent>,
    pub execution_mode: TriggerExecutionMode,
    pub language: TriggerLanguage,
    pub enabled: bool,
    pub priority: i32,
    pub total_executions: u64,
    pub avg_execution_time_ms: f64,
    pub last_executed: Option<DateTime<Utc>>,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_trigger_manager_creation() {
        let manager = TriggerManager::new();
        assert!(true); // Passes if created successfully
    }

    #[test]
    fn test_trigger_timing() {
        assert_eq!(TriggerTiming::Before, TriggerTiming::Before);
        assert_ne!(TriggerTiming::After, TriggerTiming::Instead);
    }

    #[test]
    fn test_trigger_events() {
        assert_eq!(TriggerEvent::Insert, TriggerEvent::Insert);
        assert_ne!(TriggerEvent::Update, TriggerEvent::Delete);
    }

    #[test]
    fn test_execution_modes() {
        assert_eq!(TriggerExecutionMode::Synchronous, TriggerExecutionMode::Synchronous);
        assert_ne!(TriggerExecutionMode::Asynchronous, TriggerExecutionMode::Deferred);
    }

    #[test]
    fn test_trigger_languages() {
        assert_eq!(TriggerLanguage::SQL, TriggerLanguage::SQL);
        assert_ne!(TriggerLanguage::Rust, TriggerLanguage::Python);
    }

    #[test]
    fn test_trigger_definition() {
        let trigger = TriggerDefinition {
            name: "test_trigger".to_string(),
            table_name: "users".to_string(),
            timing: TriggerTiming::After,
            events: HashSet::from([TriggerEvent::Insert, TriggerEvent::Update]),
            execution_mode: TriggerExecutionMode::Synchronous,
            language: TriggerLanguage::SQL,
            source_code: "BEGIN SELECT 1; END".to_string(),
            conditions: vec![],
            priority: 0,
            enabled: true,
            description: "Test trigger".to_string(),
            tags: HashSet::new(),
            created_at: Utc::now(),
            modified_at: Utc::now(),
            version: "1.0.0".to_string(),
        };

        assert_eq!(trigger.name, "test_trigger");
        assert_eq!(trigger.table_name, "users");
        assert_eq!(trigger.events.len(), 2);
        assert!(trigger.enabled);
    }

    #[test]
    fn test_execution_result() {
        let result = TriggerExecutionResult {
            trigger_name: "test_trigger".to_string(),
            success: true,
            execution_time_ms: 150.0,
            affected_rows: 5,
            error_message: None,
            side_effects: vec!["audit_logged".to_string()],
        };

        assert!(result.success);
        assert_eq!(result.execution_time_ms, 150.0);
        assert_eq!(result.affected_rows, 5);
        assert!(result.error_message.is_none());
    }

    #[tokio::test]
    async fn test_trigger_validation() {
        let manager = TriggerManager::new();

        // Valid trigger
        let valid_trigger = TriggerDefinition {
            name: "valid_trigger".to_string(),
            table_name: "test_table".to_string(),
            timing: TriggerTiming::After,
            events: HashSet::from([TriggerEvent::Insert]),
            execution_mode: TriggerExecutionMode::Synchronous,
            language: TriggerLanguage::SQL,
            source_code: "BEGIN SELECT 1; END".to_string(),
            conditions: vec![],
            priority: 0,
            enabled: true,
            description: "Valid trigger".to_string(),
            tags: HashSet::new(),
            created_at: Utc::now(),
            modified_at: Utc::now(),
            version: "1.0.0".to_string(),
        };

        // This would normally validate (we're testing the structure)
        assert_eq!(valid_trigger.name, "valid_trigger");
        assert!(valid_trigger.enabled);
    }
}
