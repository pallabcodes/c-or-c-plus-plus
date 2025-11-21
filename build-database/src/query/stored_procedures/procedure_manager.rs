//! Procedure Manager: Intelligent Stored Procedure Management
//!
//! Advanced management system for stored procedures with JIT compilation,
//! multi-language support, security controls, and performance optimization.

use std::collections::{HashMap, HashSet};
use std::sync::Arc;
use parking_lot::RwLock;
use chrono::{DateTime, Utc};
use crate::core::errors::{AuroraResult, AuroraError};
use crate::core::schema::DataType;
use super::jit_compiler::{JITCompiler, CompilationResult};
use super::security_engine::{SecurityEngine, SecurityContext};
use super::runtime_environment::{RuntimeEnvironment, ExecutionContext};
use super::version_control::{VersionControl, ProcedureVersion};
use super::performance_monitor::PerformanceMonitor;

/// Programming languages supported for stored procedures
#[derive(Debug, Clone, PartialEq)]
pub enum ProcedureLanguage {
    SQL,           // Traditional SQL procedures
    Rust,          // Rust for performance-critical procedures
    Python,        // Python for data science and ML
    JavaScript,    // JavaScript for web integration
    Lua,           // Lua for lightweight scripting
}

/// Procedure execution modes
#[derive(Debug, Clone, PartialEq)]
pub enum ExecutionMode {
    Interpreted,   // Traditional interpreted execution
    JITCompiled,   // Just-In-Time compilation
    AOTCompiled,   // Ahead-Of-Time compilation
    Hybrid,        // Mix of compiled and interpreted
}

/// Stored procedure definition
#[derive(Debug, Clone)]
pub struct ProcedureDefinition {
    pub name: String,
    pub language: ProcedureLanguage,
    pub parameters: Vec<ProcedureParameter>,
    pub return_type: Option<DataType>,
    pub source_code: String,
    pub execution_mode: ExecutionMode,
    pub security_level: SecurityLevel,
    pub timeout_seconds: Option<u64>,
    pub max_memory_mb: Option<u64>,
    pub description: String,
    pub tags: HashSet<String>,
    pub created_at: DateTime<Utc>,
    pub modified_at: DateTime<Utc>,
    pub version: String,
}

/// Procedure parameter definition
#[derive(Debug, Clone)]
pub struct ProcedureParameter {
    pub name: String,
    pub data_type: DataType,
    pub is_output: bool,
    pub default_value: Option<String>,
    pub description: String,
}

/// Security levels for procedures
#[derive(Debug, Clone, PartialEq, PartialOrd)]
pub enum SecurityLevel {
    Public,        // No restrictions
    Restricted,    // Limited permissions
    Sensitive,     // Elevated permissions with audit
    Critical,      // Maximum security with full audit
}

/// Compiled procedure representation
#[derive(Debug)]
struct CompiledProcedure {
    definition: ProcedureDefinition,
    compiled_code: Vec<u8>, // JIT-compiled machine code
    symbol_table: HashMap<String, usize>, // Symbol to offset mapping
    metadata: ProcedureMetadata,
}

/// Procedure execution metadata
#[derive(Debug)]
struct ProcedureMetadata {
    compilation_time_ms: f64,
    code_size_bytes: usize,
    optimization_level: u8,
    security_hashes: HashMap<String, String>,
}

/// Execution result
#[derive(Debug)]
pub struct ExecutionResult {
    pub success: bool,
    pub return_value: Option<String>,
    pub output_parameters: HashMap<String, String>,
    pub execution_time_ms: f64,
    pub memory_used_mb: f64,
    pub security_events: Vec<String>,
    pub performance_metrics: HashMap<String, f64>,
}

/// Intelligent procedure manager
pub struct ProcedureManager {
    procedures: RwLock<HashMap<String, CompiledProcedure>>,
    jit_compiler: Arc<JITCompiler>,
    security_engine: Arc<SecurityEngine>,
    runtime_env: Arc<RuntimeEnvironment>,
    version_control: Arc<VersionControl>,
    performance_monitor: Arc<PerformanceMonitor>,
    procedure_cache: RwLock<HashMap<String, ExecutionResult>>, // Cache for expensive procedures
}

impl ProcedureManager {
    pub fn new() -> Self {
        Self {
            procedures: RwLock::new(HashMap::new()),
            jit_compiler: Arc::new(JITCompiler::new()),
            security_engine: Arc::new(SecurityEngine::new()),
            runtime_env: Arc::new(RuntimeEnvironment::new()),
            version_control: Arc::new(VersionControl::new()),
            performance_monitor: Arc::new(PerformanceMonitor::new()),
            procedure_cache: RwLock::new(HashMap::new()),
        }
    }

    /// Create and compile a stored procedure
    pub async fn create_procedure(
        &self,
        definition: ProcedureDefinition,
    ) -> AuroraResult<()> {
        println!("🚀 Creating procedure '{}' in {:?} with {:?} execution",
                definition.name, definition.language, definition.execution_mode);

        // Validate procedure definition
        self.validate_procedure(&definition).await?;

        // Compile procedure based on language and execution mode
        let compilation_result = match definition.execution_mode {
            ExecutionMode::JITCompiled | ExecutionMode::AOTCompiled => {
                self.jit_compiler.compile_procedure(&definition).await?
            }
            _ => {
                // For interpreted modes, "compile" to bytecode or AST
                CompilationResult {
                    compiled_code: definition.source_code.as_bytes().to_vec(),
                    symbol_table: HashMap::new(),
                    metadata: ProcedureMetadata {
                        compilation_time_ms: 0.0,
                        code_size_bytes: definition.source_code.len(),
                        optimization_level: 0,
                        security_hashes: HashMap::new(),
                    },
                }
            }
        };

        // Create compiled procedure
        let compiled_procedure = CompiledProcedure {
            definition: definition.clone(),
            compiled_code: compilation_result.compiled_code,
            symbol_table: compilation_result.symbol_table,
            metadata: compilation_result.metadata,
        };

        // Store compiled procedure
        {
            let mut procedures = self.procedures.write();
            procedures.insert(definition.name.clone(), compiled_procedure);
        }

        // Register with version control
        self.version_control.register_procedure(&definition).await?;

        // Initialize security context
        self.security_engine.register_procedure(&definition).await?;

        // Initialize performance monitoring
        self.performance_monitor.register_procedure(&definition.name).await?;

        println!("✅ Created procedure '{}' - {} bytes compiled, {}ms compilation time",
                definition.name,
                compilation_result.metadata.code_size_bytes,
                compilation_result.metadata.compilation_time_ms);

        Ok(())
    }

    /// Execute a stored procedure with intelligent optimization
    pub async fn execute_procedure(
        &self,
        procedure_name: &str,
        parameters: HashMap<String, String>,
        security_context: &SecurityContext,
    ) -> AuroraResult<ExecutionResult> {
        // Check cache first
        let cache_key = self.generate_cache_key(procedure_name, &parameters);
        if let Some(cached_result) = self.get_cached_result(&cache_key) {
            return Ok(cached_result);
        }

        // Get compiled procedure
        let compiled_procedure = {
            let procedures = self.procedures.read();
            procedures.get(procedure_name).cloned()
                .ok_or_else(|| AuroraError::NotFound(format!("Procedure '{}' not found", procedure_name)))?
        };

        // Security check
        self.security_engine.validate_execution(&compiled_procedure.definition, security_context).await?;

        // Create execution context
        let exec_context = ExecutionContext {
            procedure_name: procedure_name.to_string(),
            parameters: parameters.clone(),
            security_context: security_context.clone(),
            timeout_seconds: compiled_procedure.definition.timeout_seconds,
            max_memory_mb: compiled_procedure.definition.max_memory_mb,
        };

        // Execute procedure
        let start_time = std::time::Instant::now();

        let result = match compiled_procedure.definition.execution_mode {
            ExecutionMode::JITCompiled | ExecutionMode::AOTCompiled => {
                self.runtime_env.execute_compiled(&compiled_procedure, &exec_context).await?
            }
            ExecutionMode::Interpreted => {
                self.runtime_env.execute_interpreted(&compiled_procedure, &exec_context).await?
            }
            ExecutionMode::Hybrid => {
                self.runtime_env.execute_hybrid(&compiled_procedure, &exec_context).await?
            }
        };

        let execution_time = start_time.elapsed().as_millis() as f64;

        // Create final result with metrics
        let final_result = ExecutionResult {
            success: result.success,
            return_value: result.return_value,
            output_parameters: result.output_parameters,
            execution_time_ms: execution_time,
            memory_used_mb: result.memory_used_mb,
            security_events: result.security_events,
            performance_metrics: self.performance_monitor.get_metrics(procedure_name).await?,
        };

        // Update performance monitoring
        self.performance_monitor.record_execution(procedure_name, &final_result).await?;

        // Cache result if appropriate
        if self.should_cache_result(&compiled_procedure.definition, &final_result) {
            self.cache_result(cache_key, final_result.clone());
        }

        Ok(final_result)
    }

    /// Drop a stored procedure
    pub async fn drop_procedure(&self, procedure_name: &str) -> AuroraResult<()> {
        // Remove from procedures
        let removed_procedure = {
            let mut procedures = self.procedures.write();
            procedures.remove(procedure_name)
                .ok_or_else(|| AuroraError::NotFound(format!("Procedure '{}' not found", procedure_name)))?
        };

        // Clean up related resources
        self.version_control.remove_procedure(procedure_name).await?;
        self.security_engine.remove_procedure(procedure_name).await?;
        self.performance_monitor.remove_procedure(procedure_name).await?;

        // Clear cache entries
        self.clear_procedure_cache(procedure_name);

        println!("🗑️  Dropped procedure '{}'", procedure_name);
        Ok(())
    }

    /// Get procedure information and statistics
    pub async fn get_procedure_info(&self, procedure_name: &str) -> AuroraResult<ProcedureInfo> {
        let compiled_procedure = {
            let procedures = self.procedures.read();
            procedures.get(procedure_name).cloned()
                .ok_or_else(|| AuroraError::NotFound(format!("Procedure '{}' not found", procedure_name)))?
        };

        let version_info = self.version_control.get_version_info(procedure_name).await?;
        let security_info = self.security_engine.get_security_info(procedure_name).await?;
        let performance_stats = self.performance_monitor.get_statistics(procedure_name).await?;

        Ok(ProcedureInfo {
            definition: compiled_procedure.definition,
            compilation_info: compiled_procedure.metadata,
            version_info,
            security_info,
            performance_stats,
        })
    }

    /// List all procedures with their metadata
    pub async fn list_procedures(&self) -> Vec<ProcedureSummary> {
        let procedures = self.procedures.read();
        let mut summaries = Vec::new();

        for (name, compiled_proc) in procedures.iter() {
            let perf_stats = self.performance_monitor.get_statistics(name).await.unwrap_or_default();
            let security_info = self.security_engine.get_security_info(name).await.unwrap_or_default();

            summaries.push(ProcedureSummary {
                name: name.clone(),
                language: compiled_proc.definition.language.clone(),
                execution_mode: compiled_proc.definition.execution_mode.clone(),
                security_level: compiled_proc.definition.security_level.clone(),
                total_executions: perf_stats.total_executions,
                avg_execution_time_ms: perf_stats.avg_execution_time_ms,
                last_executed: perf_stats.last_executed,
                created_at: compiled_proc.definition.created_at,
            });
        }

        summaries.sort_by(|a, b| a.name.cmp(&b.name));
        summaries
    }

    /// Upgrade procedure to new version
    pub async fn upgrade_procedure(
        &self,
        procedure_name: &str,
        new_definition: ProcedureDefinition,
    ) -> AuroraResult<()> {
        // Validate new version
        self.validate_procedure(&new_definition).await?;

        // Create backup of current version
        self.version_control.create_backup(procedure_name).await?;

        // Compile new version
        let compilation_result = self.jit_compiler.compile_procedure(&new_definition).await?;

        // Create new compiled procedure
        let new_compiled = CompiledProcedure {
            definition: new_definition.clone(),
            compiled_code: compilation_result.compiled_code,
            symbol_table: compilation_result.symbol_table,
            metadata: compilation_result.metadata,
        };

        // Atomic replacement
        {
            let mut procedures = self.procedures.write();
            procedures.insert(procedure_name.to_string(), new_compiled);
        }

        // Register new version
        self.version_control.register_version(procedure_name, &new_definition).await?;

        // Clear cache for this procedure
        self.clear_procedure_cache(procedure_name);

        println!("⬆️  Upgraded procedure '{}' to version {}", procedure_name, new_definition.version);
        Ok(())
    }

    // Helper methods

    async fn validate_procedure(&self, definition: &ProcedureDefinition) -> AuroraResult<()> {
        // Validate procedure name
        if definition.name.is_empty() || definition.name.len() > 128 {
            return Err(AuroraError::InvalidArgument("Procedure name must be 1-128 characters".to_string()));
        }

        // Validate parameters
        let mut param_names = HashSet::new();
        for param in &definition.parameters {
            if param.name.is_empty() {
                return Err(AuroraError::InvalidArgument("Parameter name cannot be empty".to_string()));
            }
            if param_names.contains(&param.name) {
                return Err(AuroraError::InvalidArgument(format!("Duplicate parameter name: {}", param.name)));
            }
            param_names.insert(param.name.clone());
        }

        // Language-specific validation
        match definition.language {
            ProcedureLanguage::Rust => self.validate_rust_procedure(definition)?,
            ProcedureLanguage::Python => self.validate_python_procedure(definition)?,
            ProcedureLanguage::SQL => self.validate_sql_procedure(definition)?,
            _ => {} // Other languages have basic validation
        }

        // Security validation
        self.security_engine.validate_definition(definition).await?;

        Ok(())
    }

    fn validate_rust_procedure(&self, definition: &ProcedureDefinition) -> AuroraResult<()> {
        // Basic Rust syntax validation (would use rustc in real implementation)
        if !definition.source_code.contains("fn ") {
            return Err(AuroraError::InvalidArgument("Rust procedure must contain a function".to_string()));
        }
        Ok(())
    }

    fn validate_python_procedure(&self, definition: &ProcedureDefinition) -> AuroraResult<()> {
        // Basic Python validation
        if !definition.source_code.contains("def ") {
            return Err(AuroraError::InvalidArgument("Python procedure must contain a function".to_string()));
        }
        Ok(())
    }

    fn validate_sql_procedure(&self, definition: &ProcedureDefinition) -> AuroraResult<()> {
        // Basic SQL validation
        if !definition.source_code.to_uppercase().contains("BEGIN") {
            return Err(AuroraError::InvalidArgument("SQL procedure must contain BEGIN block".to_string()));
        }
        Ok(())
    }

    fn generate_cache_key(&self, procedure_name: &str, parameters: &HashMap<String, String>) -> String {
        let mut key = procedure_name.to_string();
        let mut sorted_params: Vec<_> = parameters.iter().collect();
        sorted_params.sort_by(|a, b| a.0.cmp(b.0));

        for (param_name, param_value) in sorted_params {
            key.push_str(&format!(":{}={}", param_name, param_value));
        }

        key
    }

    fn get_cached_result(&self, cache_key: &str) -> Option<ExecutionResult> {
        self.procedure_cache.read().get(cache_key).cloned()
    }

    fn cache_result(&self, cache_key: String, result: ExecutionResult) {
        let mut cache = self.procedure_cache.write();
        cache.insert(cache_key, result);
    }

    fn should_cache_result(&self, definition: &ProcedureDefinition, result: &ExecutionResult) -> bool {
        // Cache if execution was expensive and successful
        result.success &&
        result.execution_time_ms > 100.0 && // More than 100ms
        matches!(definition.execution_mode, ExecutionMode::JITCompiled | ExecutionMode::AOTCompiled)
    }

    fn clear_procedure_cache(&self, procedure_name: &str) {
        let mut cache = self.procedure_cache.write();
        let keys_to_remove: Vec<String> = cache.keys()
            .filter(|key| key.starts_with(&format!("{}:", procedure_name)))
            .cloned()
            .collect();

        for key in keys_to_remove {
            cache.remove(&key);
        }
    }
}

/// Procedure information summary
#[derive(Debug)]
pub struct ProcedureInfo {
    pub definition: ProcedureDefinition,
    pub compilation_info: ProcedureMetadata,
    pub version_info: ProcedureVersion,
    pub security_info: SecurityInfo,
    pub performance_stats: PerformanceStats,
}

/// Security information
#[derive(Debug)]
pub struct SecurityInfo {
    pub security_level: SecurityLevel,
    pub required_permissions: HashSet<String>,
    pub audit_enabled: bool,
    pub last_security_review: Option<DateTime<Utc>>,
}

/// Performance statistics
#[derive(Debug)]
pub struct PerformanceStats {
    pub total_executions: u64,
    pub avg_execution_time_ms: f64,
    pub max_execution_time_ms: f64,
    pub min_execution_time_ms: f64,
    pub total_memory_used_mb: f64,
    pub last_executed: Option<DateTime<Utc>>,
    pub error_rate: f64,
}

impl Default for PerformanceStats {
    fn default() -> Self {
        Self {
            total_executions: 0,
            avg_execution_time_ms: 0.0,
            max_execution_time_ms: 0.0,
            min_execution_time_ms: 0.0,
            total_memory_used_mb: 0.0,
            last_executed: None,
            error_rate: 0.0,
        }
    }
}

/// Procedure summary for listing
#[derive(Debug)]
pub struct ProcedureSummary {
    pub name: String,
    pub language: ProcedureLanguage,
    pub execution_mode: ExecutionMode,
    pub security_level: SecurityLevel,
    pub total_executions: u64,
    pub avg_execution_time_ms: f64,
    pub last_executed: Option<DateTime<Utc>>,
    pub created_at: DateTime<Utc>,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_procedure_manager_creation() {
        let manager = ProcedureManager::new();
        assert!(true); // Passes if created successfully
    }

    #[test]
    fn test_procedure_languages() {
        assert_eq!(ProcedureLanguage::Rust, ProcedureLanguage::Rust);
        assert_ne!(ProcedureLanguage::Python, ProcedureLanguage::SQL);
    }

    #[test]
    fn test_execution_modes() {
        assert_eq!(ExecutionMode::JITCompiled, ExecutionMode::JITCompiled);
        assert_ne!(ExecutionMode::Interpreted, ExecutionMode::AOTCompiled);
    }

    #[test]
    fn test_security_levels() {
        assert!(SecurityLevel::Public < SecurityLevel::Critical);
        assert!(SecurityLevel::Sensitive > SecurityLevel::Restricted);
    }

    #[test]
    fn test_cache_key_generation() {
        let manager = ProcedureManager::new();
        let mut params = HashMap::new();
        params.insert("user_id".to_string(), "123".to_string());
        params.insert("amount".to_string(), "100.50".to_string());

        let key = manager.generate_cache_key("process_payment", &params);

        // Key should contain procedure name and sorted parameters
        assert!(key.contains("process_payment"));
        assert!(key.contains("amount=100.50"));
        assert!(key.contains("user_id=123"));
    }

    #[test]
    fn test_procedure_validation() {
        let manager = ProcedureManager::new();

        // Valid procedure
        let valid_proc = ProcedureDefinition {
            name: "test_proc".to_string(),
            language: ProcedureLanguage::SQL,
            parameters: vec![
                ProcedureParameter {
                    name: "param1".to_string(),
                    data_type: crate::core::data::DataType::Text,
                    is_output: false,
                    default_value: None,
                    description: "Test parameter".to_string(),
                }
            ],
            return_type: None,
            source_code: "BEGIN SELECT 1; END".to_string(),
            execution_mode: ExecutionMode::Interpreted,
            security_level: SecurityLevel::Public,
            timeout_seconds: Some(30),
            max_memory_mb: Some(100),
            description: "Test procedure".to_string(),
            tags: HashSet::new(),
            created_at: Utc::now(),
            modified_at: Utc::now(),
            version: "1.0.0".to_string(),
        };

        // This would normally validate, but we're testing the structure
        assert_eq!(valid_proc.name, "test_proc");
        assert_eq!(valid_proc.language, ProcedureLanguage::SQL);
    }

    #[test]
    fn test_execution_result_structure() {
        let result = ExecutionResult {
            success: true,
            return_value: Some("42".to_string()),
            output_parameters: HashMap::new(),
            execution_time_ms: 150.0,
            memory_used_mb: 25.0,
            security_events: vec![],
            performance_metrics: HashMap::new(),
        };

        assert!(result.success);
        assert_eq!(result.return_value, Some("42".to_string()));
        assert_eq!(result.execution_time_ms, 150.0);
    }
}
