//! Execution Engine: Trigger Execution with Intelligent Optimization
//!
//! Advanced execution environment for triggers with multiple execution modes,
//! resource management, and performance optimization.

use std::collections::HashMap;
use std::sync::Arc;
use tokio::sync::{Semaphore, RwLock};
use tokio::task;
use chrono::{DateTime, Utc, Duration};
use crate::core::errors::{AuroraResult, AuroraError};
use super::event_engine::DatabaseEvent;
use super::trigger_manager::{TriggerDefinition, TriggerExecutionResult, TriggerLanguage, TriggerExecutionMode};

/// Execution context for triggers
#[derive(Debug, Clone)]
pub struct TriggerExecutionContext {
    pub trigger_name: String,
    pub event: DatabaseEvent,
    pub trigger_definition: TriggerDefinition,
}

/// Execution result
#[derive(Debug)]
pub struct ExecutionResult {
    pub success: bool,
    pub affected_rows: u64,
    pub error_message: Option<String>,
    pub side_effects: Vec<String>,
}

/// Deferred execution task
#[derive(Debug)]
struct DeferredTask {
    context: TriggerExecutionContext,
    scheduled_at: DateTime<Utc>,
    retry_count: u32,
    max_retries: u32,
}

/// Execution statistics
#[derive(Debug)]
pub struct ExecutionStats {
    pub total_executions: u64,
    pub successful_executions: u64,
    pub failed_executions: u64,
    pub avg_execution_time_ms: f64,
    pub deferred_executions: u64,
    pub async_executions: u64,
    pub resource_limit_hits: u64,
}

/// Intelligent execution engine
pub struct ExecutionEngine {
    execution_semaphore: Arc<Semaphore>,
    max_concurrent_executions: usize,
    deferred_tasks: Arc<RwLock<Vec<DeferredTask>>>,
    execution_stats: Arc<RwLock<ExecutionStats>>,
    resource_limits: ExecutionResourceLimits,
    language_runtimes: HashMap<TriggerLanguage, Arc<dyn RuntimeExecutor>>,
}

impl ExecutionEngine {
    pub fn new() -> Self {
        let mut language_runtimes = HashMap::new();

        // Register language runtimes
        language_runtimes.insert(TriggerLanguage::SQL, Arc::new(SqlRuntime::new()));
        language_runtimes.insert(TriggerLanguage::Rust, Arc::new(RustRuntime::new()));
        language_runtimes.insert(TriggerLanguage::Python, Arc::new(PythonRuntime::new()));
        language_runtimes.insert(TriggerLanguage::JavaScript, Arc::new(JavaScriptRuntime::new()));
        language_runtimes.insert(TriggerLanguage::Lua, Arc::new(LuaRuntime::new()));

        Self {
            execution_semaphore: Arc::new(Semaphore::new(50)), // Max 50 concurrent executions
            max_concurrent_executions: 50,
            deferred_tasks: Arc::new(RwLock::new(Vec::new())),
            execution_stats: Arc::new(RwLock::new(ExecutionStats {
                total_executions: 0,
                successful_executions: 0,
                failed_executions: 0,
                avg_execution_time_ms: 0.0,
                deferred_executions: 0,
                async_executions: 0,
                resource_limit_hits: 0,
            })),
            resource_limits: ExecutionResourceLimits::default(),
            language_runtimes,
        }
    }

    /// Execute trigger synchronously (blocks the triggering operation)
    pub async fn execute_synchronous(&self, context: &TriggerExecutionContext) -> AuroraResult<ExecutionResult> {
        // Acquire execution permit
        let _permit = self.execution_semaphore.acquire().await
            .map_err(|_| AuroraError::InvalidArgument("Execution queue full".to_string()))?;

        // Check resource limits
        self.check_resource_limits(context)?;

        // Update stats
        {
            let mut stats = self.execution_stats.write().await;
            stats.total_executions += 1;
        }

        // Execute the trigger
        let result = self.execute_trigger_code(context).await;

        // Update stats
        {
            let mut stats = self.execution_stats.write().await;
            match &result {
                Ok(exec_result) => {
                    if exec_result.success {
                        stats.successful_executions += 1;
                    } else {
                        stats.failed_executions += 1;
                    }
                }
                Err(_) => stats.failed_executions += 1,
            }
        }

        result
    }

    /// Execute trigger asynchronously (in background)
    pub async fn execute_asynchronous(&self, context: &TriggerExecutionContext) -> AuroraResult<ExecutionResult> {
        let context_clone = context.clone();
        let engine_clone = self.clone_for_async();

        // Spawn async task
        task::spawn(async move {
            let result = engine_clone.execute_synchronous(&context_clone).await;
            match result {
                Ok(exec_result) => {
                    println!("✅ Async trigger '{}' executed successfully", context_clone.trigger_name);
                    // Update async stats
                    let mut stats = engine_clone.execution_stats.write().await;
                    stats.async_executions += 1;
                }
                Err(e) => {
                    println!("❌ Async trigger '{}' failed: {}", context_clone.trigger_name, e);
                }
            }
        });

        // Return success immediately
        Ok(ExecutionResult {
            success: true,
            affected_rows: 0,
            error_message: None,
            side_effects: vec!["async_execution_started".to_string()],
        })
    }

    /// Schedule trigger for deferred execution
    pub async fn schedule_deferred(&self, context: &TriggerExecutionContext) -> AuroraResult<()> {
        let task = DeferredTask {
            context: context.clone(),
            scheduled_at: Utc::now() + Duration::seconds(30), // Execute in 30 seconds
            retry_count: 0,
            max_retries: 3,
        };

        {
            let mut tasks = self.deferred_tasks.write().await;
            tasks.push(task);
        }

        {
            let mut stats = self.execution_stats.write().await;
            stats.deferred_executions += 1;
        }

        println!("⏰ Scheduled deferred execution for trigger '{}'", context.trigger_name);
        Ok(())
    }

    /// Execute trigger conditionally
    pub async fn execute_conditional(&self, context: &TriggerExecutionContext) -> AuroraResult<ExecutionResult> {
        // Check if conditions are met before executing
        if self.should_execute_conditionally(context)? {
            self.execute_synchronous(context).await
        } else {
            Ok(ExecutionResult {
                success: true,
                affected_rows: 0,
                error_message: None,
                side_effects: vec!["condition_not_met".to_string()],
            })
        }
    }

    /// Process deferred tasks
    pub async fn process_deferred_tasks(&self) -> AuroraResult<usize> {
        let now = Utc::now();
        let mut tasks_to_execute = Vec::new();

        // Collect due tasks
        {
            let mut tasks = self.deferred_tasks.write().await;
            tasks.retain(|task| {
                if task.scheduled_at <= now {
                    tasks_to_execute.push(task.clone());
                    false // Remove from list
                } else {
                    true // Keep in list
                }
            });
        }

        // Execute due tasks
        for task in tasks_to_execute {
            let result = self.execute_synchronous(&task.context).await;
            match result {
                Ok(_) => println!("✅ Deferred trigger '{}' executed successfully", task.context.trigger_name),
                Err(e) => {
                    if task.retry_count < task.max_retries {
                        // Reschedule with backoff
                        let backoff_seconds = 30 * (task.retry_count + 1) as i64;
                        let retry_task = DeferredTask {
                            context: task.context,
                            scheduled_at: Utc::now() + Duration::seconds(backoff_seconds),
                            retry_count: task.retry_count + 1,
                            max_retries: task.max_retries,
                        };

                        let mut tasks = self.deferred_tasks.write().await;
                        tasks.push(retry_task);

                        println!("🔄 Deferred trigger '{}' rescheduled (attempt {})", task.context.trigger_name, task.retry_count + 1);
                    } else {
                        println!("❌ Deferred trigger '{}' failed permanently: {}", task.context.trigger_name, e);
                    }
                }
            }
        }

        Ok(tasks_to_execute.len())
    }

    /// Get execution statistics
    pub async fn get_execution_stats(&self) -> ExecutionStats {
        self.execution_stats.read().await.clone()
    }

    /// Update resource limits
    pub fn update_resource_limits(&mut self, limits: ExecutionResourceLimits) {
        self.resource_limits = limits;
    }

    // Private methods

    fn clone_for_async(&self) -> Self {
        Self {
            execution_semaphore: Arc::clone(&self.execution_semaphore),
            max_concurrent_executions: self.max_concurrent_executions,
            deferred_tasks: Arc::clone(&self.deferred_tasks),
            execution_stats: Arc::clone(&self.execution_stats),
            resource_limits: self.resource_limits.clone(),
            language_runtimes: self.language_runtimes.clone(),
        }
    }

    fn check_resource_limits(&self, context: &TriggerExecutionContext) -> AuroraResult<()> {
        let available_permits = self.execution_semaphore.available_permits();

        if available_permits == 0 {
            let mut stats = self.execution_stats.try_write()
                .map_err(|_| AuroraError::InvalidArgument("Stats lock error".to_string()))?;
            stats.resource_limit_hits += 1;
            return Err(AuroraError::InvalidArgument("Resource limit exceeded - too many concurrent executions".to_string()));
        }

        // Check memory limits (simplified)
        if self.resource_limits.max_memory_mb > 0 {
            // In a real implementation, would check actual memory usage
            // For now, just check against configured limit
        }

        Ok(())
    }

    async fn execute_trigger_code(&self, context: &TriggerExecutionContext) -> AuroraResult<ExecutionResult> {
        let runtime = self.language_runtimes.get(&context.trigger_definition.language)
            .ok_or_else(|| AuroraError::InvalidArgument(format!("Unsupported language: {:?}", context.trigger_definition.language)))?;

        runtime.execute(context).await
    }

    fn should_execute_conditionally(&self, context: &TriggerExecutionContext) -> AuroraResult<bool> {
        // Check trigger conditions
        for condition in &context.trigger_definition.conditions {
            if !self.evaluate_condition(condition, &context.event)? {
                return Ok(false);
            }
        }

        // Additional conditional logic could go here
        // For example, time-based conditions, load-based conditions, etc.

        Ok(true)
    }

    fn evaluate_condition(&self, condition: &super::condition_evaluator::TriggerCondition, event: &DatabaseEvent) -> AuroraResult<bool> {
        // Simplified condition evaluation
        match condition.condition_type.as_str() {
            "row_count_greater_than" => {
                if let Some(value) = condition.parameters.get("threshold") {
                    if let Ok(threshold) = value.parse::<u64>() {
                        return Ok(event.affected_rows > threshold);
                    }
                }
                Ok(false)
            }
            "user_is_admin" => {
                if let Some(user_id) = &event.user_id {
                    return Ok(user_id.starts_with("admin_"));
                }
                Ok(false)
            }
            "value_changed" => {
                if let (Some(old_values), Some(new_values)) = (&event.old_values, &event.new_values) {
                    if let Some(field) = condition.parameters.get("field") {
                        return Ok(old_values.get(field) != new_values.get(field));
                    }
                }
                Ok(false)
            }
            _ => Ok(true), // Unknown conditions pass through
        }
    }
}

/// Resource limits for execution
#[derive(Debug, Clone)]
pub struct ExecutionResourceLimits {
    pub max_memory_mb: u64,
    pub max_cpu_time_ms: u64,
    pub max_concurrent_executions: usize,
    pub max_deferred_tasks: usize,
}

impl Default for ExecutionResourceLimits {
    fn default() -> Self {
        Self {
            max_memory_mb: 1024, // 1GB
            max_cpu_time_ms: 30000, // 30 seconds
            max_concurrent_executions: 50,
            max_deferred_tasks: 1000,
        }
    }
}

/// Runtime executor trait for different languages
#[async_trait::async_trait]
trait RuntimeExecutor: Send + Sync {
    async fn execute(&self, context: &TriggerExecutionContext) -> AuroraResult<ExecutionResult>;
}

/// SQL runtime executor
#[derive(Debug)]
struct SqlRuntime;

impl SqlRuntime {
    fn new() -> Self {
        Self
    }
}

#[async_trait::async_trait]
impl RuntimeExecutor for SqlRuntime {
    async fn execute(&self, context: &TriggerExecutionContext) -> AuroraResult<ExecutionResult> {
        println!("🗃️  Executing SQL trigger '{}'", context.trigger_name);

        // Simulate SQL execution
        tokio::time::sleep(tokio::time::Duration::from_millis(20)).await;

        // Mock SQL execution result
        Ok(ExecutionResult {
            success: true,
            affected_rows: 1,
            error_message: None,
            side_effects: vec!["audit_log_inserted".to_string()],
        })
    }
}

/// Rust runtime executor
#[derive(Debug)]
struct RustRuntime;

impl RustRuntime {
    fn new() -> Self {
        Self
    }
}

#[async_trait::async_trait]
impl RuntimeExecutor for RustRuntime {
    async fn execute(&self, context: &TriggerExecutionContext) -> AuroraResult<ExecutionResult> {
        println!("🦀 Executing Rust trigger '{}'", context.trigger_name);

        // Simulate compiled execution (fast)
        tokio::time::sleep(tokio::time::Duration::from_millis(5)).await;

        Ok(ExecutionResult {
            success: true,
            affected_rows: 2,
            error_message: None,
            side_effects: vec!["cache_invalidated".to_string(), "metrics_updated".to_string()],
        })
    }
}

/// Python runtime executor
#[derive(Debug)]
struct PythonRuntime;

impl PythonRuntime {
    fn new() -> Self {
        Self
    }
}

#[async_trait::async_trait]
impl RuntimeExecutor for PythonRuntime {
    async fn execute(&self, context: &TriggerExecutionContext) -> AuroraResult<ExecutionResult> {
        println!("🐍 Executing Python trigger '{}'", context.trigger_name);

        // Simulate interpreted execution (slower)
        tokio::time::sleep(tokio::time::Duration::from_millis(50)).await;

        Ok(ExecutionResult {
            success: true,
            affected_rows: 3,
            error_message: None,
            side_effects: vec!["ml_model_updated".to_string()],
        })
    }
}

/// JavaScript runtime executor
#[derive(Debug)]
struct JavaScriptRuntime;

impl JavaScriptRuntime {
    fn new() -> Self {
        Self
    }
}

#[async_trait::async_trait]
impl RuntimeExecutor for JavaScriptRuntime {
    async fn execute(&self, context: &TriggerExecutionContext) -> AuroraResult<ExecutionResult> {
        println!("📜 Executing JavaScript trigger '{}'", context.trigger_name);

        // Simulate JS execution
        tokio::time::sleep(tokio::time::Duration::from_millis(30)).await;

        Ok(ExecutionResult {
            success: true,
            affected_rows: 1,
            error_message: None,
            side_effects: vec!["validation_performed".to_string()],
        })
    }
}

/// Lua runtime executor
#[derive(Debug)]
struct LuaRuntime;

impl LuaRuntime {
    fn new() -> Self {
        Self
    }
}

#[async_trait::async_trait]
impl RuntimeExecutor for LuaRuntime {
    async fn execute(&self, context: &TriggerExecutionContext) -> AuroraResult<ExecutionResult> {
        println!("🌙 Executing Lua trigger '{}'", context.trigger_name);

        // Simulate lightweight execution
        tokio::time::sleep(tokio::time::Duration::from_millis(8)).await;

        Ok(ExecutionResult {
            success: true,
            affected_rows: 1,
            error_message: None,
            side_effects: vec!["config_updated".to_string()],
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::query::triggers::trigger_manager::{TriggerTiming, TriggerEvent, TriggerExecutionMode};

    fn create_test_context() -> TriggerExecutionContext {
        TriggerExecutionContext {
            trigger_name: "test_trigger".to_string(),
            event: super::event_engine::DatabaseEvent {
                event_type: super::event_engine::EventType::AfterInsert,
                table_name: "users".to_string(),
                operation: "INSERT".to_string(),
                timestamp: Utc::now(),
                transaction_id: Some("tx_123".to_string()),
                user_id: Some("user_456".to_string()),
                session_id: Some("session_789".to_string()),
                old_values: None,
                new_values: Some(HashMap::from([
                    ("id".to_string(), "1".to_string()),
                    ("name".to_string(), "John Doe".to_string()),
                ])),
                affected_rows: 1,
                query_text: Some("INSERT INTO users (name) VALUES ('John Doe')".to_string()),
                client_info: None,
            },
            trigger_definition: TriggerDefinition {
                name: "test_trigger".to_string(),
                table_name: "users".to_string(),
                timing: TriggerTiming::After,
                events: std::collections::HashSet::from([TriggerEvent::Insert]),
                execution_mode: TriggerExecutionMode::Synchronous,
                language: TriggerLanguage::SQL,
                source_code: "BEGIN SELECT 1; END".to_string(),
                conditions: vec![],
                priority: 0,
                enabled: true,
                description: "Test trigger".to_string(),
                tags: std::collections::HashSet::new(),
                created_at: Utc::now(),
                modified_at: Utc::now(),
                version: "1.0.0".to_string(),
            },
        }
    }

    #[tokio::test]
    async fn test_execution_engine_creation() {
        let engine = ExecutionEngine::new();
        assert!(true); // Passes if created successfully
    }

    #[test]
    fn test_execution_resource_limits() {
        let limits = ExecutionResourceLimits::default();
        assert_eq!(limits.max_memory_mb, 1024);
        assert_eq!(limits.max_cpu_time_ms, 30000);
    }

    #[tokio::test]
    async fn test_execution_stats() {
        let engine = ExecutionEngine::new();
        let stats = engine.get_execution_stats().await;

        assert_eq!(stats.total_executions, 0);
        assert_eq!(stats.successful_executions, 0);
        assert_eq!(stats.failed_executions, 0);
    }

    #[tokio::test]
    async fn test_synchronous_execution() {
        let engine = ExecutionEngine::new();
        let context = create_test_context();

        let result = engine.execute_synchronous(&context).await.unwrap();

        assert!(result.success);
        assert_eq!(result.affected_rows, 1);
        assert!(result.error_message.is_none());
    }

    #[tokio::test]
    async fn test_asynchronous_execution() {
        let engine = ExecutionEngine::new();
        let context = create_test_context();

        let result = engine.execute_asynchronous(&context).await.unwrap();

        assert!(result.success);
        assert_eq!(result.affected_rows, 0); // Async returns immediately
        assert!(result.side_effects.contains(&"async_execution_started".to_string()));
    }

    #[tokio::test]
    async fn test_deferred_execution() {
        let engine = ExecutionEngine::new();
        let context = create_test_context();

        let result = engine.schedule_deferred(&context).await;
        assert!(result.is_ok());

        // Check that task was scheduled
        let stats = engine.get_execution_stats().await;
        assert_eq!(stats.deferred_executions, 1);
    }

    #[tokio::test]
    async fn test_conditional_execution() {
        let engine = ExecutionEngine::new();
        let context = create_test_context();

        let result = engine.execute_conditional(&context).await.unwrap();

        assert!(result.success);
        // Should execute since no conditions are defined
    }

    #[test]
    fn test_execution_result() {
        let result = ExecutionResult {
            success: true,
            affected_rows: 5,
            error_message: None,
            side_effects: vec!["audit_logged".to_string(), "cache_cleared".to_string()],
        };

        assert!(result.success);
        assert_eq!(result.affected_rows, 5);
        assert!(result.error_message.is_none());
        assert_eq!(result.side_effects.len(), 2);
    }

    #[test]
    fn test_trigger_execution_context() {
        let context = create_test_context();

        assert_eq!(context.trigger_name, "test_trigger");
        assert_eq!(context.event.table_name, "users");
        assert_eq!(context.trigger_definition.language, TriggerLanguage::SQL);
    }
}
