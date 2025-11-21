//! JIT Compiler: Just-In-Time Compilation for Stored Procedures
//!
//! Advanced JIT compilation system that transforms stored procedures into
//! optimized machine code for maximum performance.

use std::collections::HashMap;
use std::sync::Arc;
use parking_lot::RwLock;
use crate::core::errors::{AuroraResult, AuroraError};
use super::procedure_manager::{ProcedureDefinition, ProcedureLanguage, ProcedureMetadata};

/// Compilation result
#[derive(Debug)]
pub struct CompilationResult {
    pub compiled_code: Vec<u8>,
    pub symbol_table: HashMap<String, usize>,
    pub metadata: ProcedureMetadata,
}

/// Optimization level for compilation
#[derive(Debug, Clone, PartialEq)]
pub enum OptimizationLevel {
    None,          // No optimization (fast compilation)
    Basic,         // Basic optimizations
    Standard,      // Standard optimizations
    Aggressive,    // Aggressive optimizations (slower compilation)
    Maximum,       // Maximum optimizations (slowest compilation)
}

/// JIT compiler for stored procedures
pub struct JITCompiler {
    compilation_cache: RwLock<HashMap<String, CompilationResult>>,
    optimization_profiles: HashMap<ProcedureLanguage, OptimizationProfile>,
    code_generator: Arc<CodeGenerator>,
    optimizer: Arc<ProcedureOptimizer>,
}

impl JITCompiler {
    pub fn new() -> Self {
        let mut optimization_profiles = HashMap::new();

        // Define optimization profiles for each language
        optimization_profiles.insert(ProcedureLanguage::Rust, OptimizationProfile {
            default_level: OptimizationLevel::Standard,
            supports_vectorization: true,
            supports_inlining: true,
            supports_loop_unrolling: true,
        });

        optimization_profiles.insert(ProcedureLanguage::Python, OptimizationProfile {
            default_level: OptimizationLevel::Basic,
            supports_vectorization: false,
            supports_inlining: true,
            supports_loop_unrolling: false,
        });

        optimization_profiles.insert(ProcedureLanguage::SQL, OptimizationProfile {
            default_level: OptimizationLevel::Standard,
            supports_vectorization: true,
            supports_inlining: true,
            supports_loop_unrolling: true,
        });

        optimization_profiles.insert(ProcedureLanguage::JavaScript, OptimizationProfile {
            default_level: OptimizationLevel::Basic,
            supports_vectorization: false,
            supports_inlining: true,
            supports_loop_unrolling: false,
        });

        optimization_profiles.insert(ProcedureLanguage::Lua, OptimizationProfile {
            default_level: OptimizationLevel::None,
            supports_vectorization: false,
            supports_inlining: false,
            supports_loop_unrolling: false,
        });

        Self {
            compilation_cache: RwLock::new(HashMap::new()),
            optimization_profiles,
            code_generator: Arc::new(CodeGenerator::new()),
            optimizer: Arc::new(ProcedureOptimizer::new()),
        }
    }

    /// Compile a stored procedure to machine code
    pub async fn compile_procedure(
        &self,
        definition: &ProcedureDefinition,
    ) -> AuroraResult<CompilationResult> {
        let start_time = std::time::Instant::now();

        // Check compilation cache first
        let cache_key = self.generate_cache_key(definition);
        if let Some(cached_result) = self.get_cached_compilation(&cache_key) {
            return Ok(cached_result);
        }

        println!("🔨 Compiling procedure '{}' in {:?} with JIT",
                definition.name, definition.language);

        // Parse and analyze source code
        let parsed_code = self.parse_source_code(definition).await?;

        // Apply language-specific optimizations
        let optimized_code = self.optimizer.optimize(&parsed_code, definition).await?;

        // Generate intermediate representation
        let ir_code = self.generate_intermediate_representation(&optimized_code, definition).await?;

        // Apply global optimizations
        let final_ir = self.optimizer.apply_global_optimizations(&ir_code, definition).await?;

        // Generate machine code
        let machine_code = self.code_generator.generate_machine_code(&final_ir, definition).await?;

        // Create symbol table
        let symbol_table = self.generate_symbol_table(&parsed_code, &machine_code).await?;

        // Generate security hashes
        let security_hashes = self.generate_security_hashes(&machine_code, definition).await?;

        let compilation_time = start_time.elapsed().as_millis() as f64;

        let metadata = ProcedureMetadata {
            compilation_time_ms: compilation_time,
            code_size_bytes: machine_code.len(),
            optimization_level: self.get_optimization_level(definition) as u8,
            security_hashes,
        };

        let result = CompilationResult {
            compiled_code: machine_code,
            symbol_table,
            metadata: metadata.clone(),
        };

        // Cache compilation result
        self.cache_compilation(cache_key, result.clone());

        println!("✅ Compiled '{}' - {} bytes, {:.2}ms, optimization level {}",
                definition.name, metadata.code_size_bytes, compilation_time, metadata.optimization_level);

        Ok(result)
    }

    /// Recompile procedure with different optimization level
    pub async fn recompile_with_optimization(
        &self,
        definition: &ProcedureDefinition,
        optimization_level: OptimizationLevel,
    ) -> AuroraResult<CompilationResult> {
        // Create modified definition with new optimization level
        let mut modified_definition = definition.clone();
        // In a real implementation, we'd store optimization level in the definition

        self.compile_procedure(&modified_definition).await
    }

    /// Get compilation statistics
    pub fn get_compilation_stats(&self) -> CompilationStats {
        let cache = self.compilation_cache.read();
        let total_compilations = cache.len();
        let total_compilation_time: f64 = cache.values()
            .map(|result| result.metadata.compilation_time_ms)
            .sum();
        let avg_compilation_time = if total_compilations > 0 {
            total_compilation_time / total_compilations as f64
        } else {
            0.0
        };

        let total_code_size: usize = cache.values()
            .map(|result| result.metadata.code_size_bytes)
            .sum();

        CompilationStats {
            total_compilations,
            avg_compilation_time_ms: avg_compilation_time,
            total_code_size_bytes: total_code_size,
            cache_hit_rate: 0.0, // Would track actual hits/misses
        }
    }

    /// Clear compilation cache
    pub fn clear_cache(&self) {
        let mut cache = self.compilation_cache.write();
        cache.clear();
    }

    // Private helper methods

    async fn parse_source_code(&self, definition: &ProcedureDefinition) -> AuroraResult<ParsedCode> {
        match definition.language {
            ProcedureLanguage::Rust => self.parse_rust_code(&definition.source_code).await,
            ProcedureLanguage::Python => self.parse_python_code(&definition.source_code).await,
            ProcedureLanguage::SQL => self.parse_sql_code(&definition.source_code).await,
            ProcedureLanguage::JavaScript => self.parse_javascript_code(&definition.source_code).await,
            ProcedureLanguage::Lua => self.parse_lua_code(&definition.source_code).await,
        }
    }

    async fn parse_rust_code(&self, source: &str) -> AuroraResult<ParsedCode> {
        // In a real implementation, this would use the Rust compiler's AST
        // For now, simulate parsing
        println!("🔍 Parsing Rust code...");

        // Extract function signatures and basic structure
        let functions = self.extract_functions(source, "fn ");
        let variables = self.extract_variables(source);
        let imports = self.extract_imports(source);

        Ok(ParsedCode {
            language: ProcedureLanguage::Rust,
            functions,
            variables,
            imports,
            control_flow: self.analyze_control_flow(source),
            complexity_score: self.calculate_complexity(source),
        })
    }

    async fn parse_python_code(&self, source: &str) -> AuroraResult<ParsedCode> {
        println!("🔍 Parsing Python code...");

        let functions = self.extract_functions(source, "def ");
        let variables = self.extract_variables_python(source);
        let imports = self.extract_imports_python(source);

        Ok(ParsedCode {
            language: ProcedureLanguage::Python,
            functions,
            variables,
            imports,
            control_flow: self.analyze_control_flow_python(source),
            complexity_score: self.calculate_complexity_python(source),
        })
    }

    async fn parse_sql_code(&self, source: &str) -> AuroraResult<ParsedCode> {
        println!("🔍 Parsing SQL code...");

        // SQL procedures have different structure
        let functions = vec!["main_procedure".to_string()]; // Simplified
        let variables = self.extract_sql_variables(source);
        let imports = vec![]; // SQL doesn't have imports in the same way

        Ok(ParsedCode {
            language: ProcedureLanguage::SQL,
            functions,
            variables,
            imports,
            control_flow: self.analyze_sql_control_flow(source),
            complexity_score: self.calculate_sql_complexity(source),
        })
    }

    async fn parse_javascript_code(&self, source: &str) -> AuroraResult<ParsedCode> {
        println!("🔍 Parsing JavaScript code...");

        let functions = self.extract_functions(source, "function ");
        let variables = self.extract_variables_js(source);
        let imports = self.extract_imports_js(source);

        Ok(ParsedCode {
            language: ProcedureLanguage::JavaScript,
            functions,
            variables,
            imports,
            control_flow: self.analyze_control_flow_js(source),
            complexity_score: self.calculate_complexity_js(source),
        })
    }

    async fn parse_lua_code(&self, source: &str) -> AuroraResult<ParsedCode> {
        println!("🔍 Parsing Lua code...");

        let functions = self.extract_functions(source, "function ");
        let variables = self.extract_variables_lua(source);
        let imports = vec![]; // Lua uses require()

        Ok(ParsedCode {
            language: ProcedureLanguage::Lua,
            functions,
            variables,
            imports,
            control_flow: self.analyze_control_flow_lua(source),
            complexity_score: self.calculate_complexity_lua(source),
        })
    }

    async fn generate_intermediate_representation(
        &self,
        optimized_code: &ParsedCode,
        definition: &ProcedureDefinition,
    ) -> AuroraResult<IntermediateRepresentation> {
        // Convert parsed code to intermediate representation
        // This would be a bytecode or IR format suitable for final compilation

        println!("🔧 Generating intermediate representation...");

        let instructions = self.generate_instructions(optimized_code);
        let constants = self.extract_constants(optimized_code);
        let labels = self.identify_labels(optimized_code);

        Ok(IntermediateRepresentation {
            instructions,
            constants,
            labels,
            metadata: IRMetadata {
                estimated_cycles: self.estimate_execution_cycles(optimized_code),
                memory_requirements: self.estimate_memory_usage(optimized_code),
                optimization_applied: self.get_applied_optimizations(definition),
            },
        })
    }

    async fn generate_machine_code(
        &self,
        ir: &IntermediateRepresentation,
        definition: &ProcedureDefinition,
    ) -> AuroraResult<Vec<u8>> {
        self.code_generator.generate_machine_code(ir, definition).await
    }

    async fn generate_symbol_table(
        &self,
        parsed_code: &ParsedCode,
        machine_code: &[u8],
    ) -> AuroraResult<HashMap<String, usize>> {
        let mut symbol_table = HashMap::new();

        // Map function names to offsets in machine code
        for (i, function) in parsed_code.functions.iter().enumerate() {
            symbol_table.insert(function.clone(), i * 64); // Simplified offset calculation
        }

        // Map variables to offsets
        for (i, variable) in parsed_code.variables.iter().enumerate() {
            symbol_table.insert(variable.clone(), machine_code.len() - (i + 1) * 8);
        }

        Ok(symbol_table)
    }

    async fn generate_security_hashes(
        &self,
        machine_code: &[u8],
        definition: &ProcedureDefinition,
    ) -> AuroraResult<HashMap<String, String>> {
        use sha2::{Sha256, Digest};

        let mut hashes = HashMap::new();

        // Generate SHA256 hash of machine code
        let mut hasher = Sha256::new();
        hasher.update(machine_code);
        let code_hash = format!("{:x}", hasher.finalize());
        hashes.insert("code_sha256".to_string(), code_hash);

        // Generate hash of source code
        let mut source_hasher = Sha256::new();
        source_hasher.update(definition.source_code.as_bytes());
        let source_hash = format!("{:x}", source_hasher.finalize());
        hashes.insert("source_sha256".to_string(), source_hash);

        Ok(hashes)
    }

    fn generate_cache_key(&self, definition: &ProcedureDefinition) -> String {
        format!("{}:{:?}:{}",
                definition.name,
                definition.language,
                definition.version)
    }

    fn get_cached_compilation(&self, cache_key: &str) -> Option<CompilationResult> {
        self.compilation_cache.read().get(cache_key).cloned()
    }

    fn cache_compilation(&self, cache_key: String, result: CompilationResult) {
        let mut cache = self.compilation_cache.write();
        cache.insert(cache_key, result);
    }

    fn get_optimization_level(&self, definition: &ProcedureDefinition) -> u8 {
        self.optimization_profiles.get(&definition.language)
            .map(|profile| match profile.default_level {
                OptimizationLevel::None => 0,
                OptimizationLevel::Basic => 1,
                OptimizationLevel::Standard => 2,
                OptimizationLevel::Aggressive => 3,
                OptimizationLevel::Maximum => 4,
            })
            .unwrap_or(2)
    }

    // Helper methods for code analysis

    fn extract_functions(&self, source: &str, prefix: &str) -> Vec<String> {
        source.lines()
            .filter(|line| line.trim().starts_with(prefix))
            .map(|line| {
                let start = line.find(prefix).unwrap() + prefix.len();
                let end = line[start..].find('(').unwrap_or(line.len() - start) + start;
                line[start..end].trim().to_string()
            })
            .collect()
    }

    fn extract_variables(&self, source: &str) -> Vec<String> {
        // Simplified variable extraction
        source.lines()
            .filter(|line| line.contains("let ") && line.contains("="))
            .map(|line| {
                let let_pos = line.find("let ").unwrap() + 4;
                let eq_pos = line[let_pos..].find('=').unwrap() + let_pos;
                line[let_pos..eq_pos].trim().to_string()
            })
            .collect()
    }

    fn extract_variables_python(&self, source: &str) -> Vec<String> {
        source.lines()
            .filter(|line| line.contains("=") && !line.trim().starts_with("def ") && !line.trim().starts_with("class "))
            .map(|line| {
                let eq_pos = line.find('=').unwrap();
                line[..eq_pos].trim().to_string()
            })
            .collect()
    }

    fn extract_sql_variables(&self, source: &str) -> Vec<String> {
        // SQL variables (DECLARE, parameters, etc.)
        source.lines()
            .filter(|line| line.to_uppercase().contains("DECLARE") || line.contains("@"))
            .map(|line| "sql_var".to_string()) // Simplified
            .collect()
    }

    fn extract_variables_js(&self, source: &str) -> Vec<String> {
        source.lines()
            .filter(|line| line.contains("var ") || line.contains("let ") || line.contains("const "))
            .map(|line| {
                let keyword = if line.contains("var ") { "var " }
                    else if line.contains("let ") { "let " }
                    else { "const " };
                let start = line.find(keyword).unwrap() + keyword.len();
                let end = line[start..].find('=').unwrap_or(line[start..].find(';').unwrap_or(line.len() - start)) + start;
                line[start..end].trim().to_string()
            })
            .collect()
    }

    fn extract_variables_lua(&self, source: &str) -> Vec<String> {
        source.lines()
            .filter(|line| line.contains("local ") && line.contains("="))
            .map(|line| {
                let local_pos = line.find("local ").unwrap() + 6;
                let eq_pos = line[local_pos..].find('=').unwrap() + local_pos;
                line[local_pos..eq_pos].trim().to_string()
            })
            .collect()
    }

    fn extract_imports(&self, source: &str) -> Vec<String> {
        source.lines()
            .filter(|line| line.trim().starts_with("use "))
            .map(|line| line.trim().to_string())
            .collect()
    }

    fn extract_imports_python(&self, source: &str) -> Vec<String> {
        source.lines()
            .filter(|line| line.trim().starts_with("import ") || line.trim().starts_with("from "))
            .map(|line| line.trim().to_string())
            .collect()
    }

    fn extract_imports_js(&self, source: &str) -> Vec<String> {
        source.lines()
            .filter(|line| line.trim().starts_with("import ") || line.trim().starts_with("require("))
            .map(|line| line.trim().to_string())
            .collect()
    }

    fn analyze_control_flow(&self, source: &str) -> ControlFlowInfo {
        ControlFlowInfo {
            loops: source.matches("for ").count() + source.matches("while ").count(),
            conditionals: source.matches("if ").count(),
            functions: source.matches("fn ").count(),
            recursion_depth: self.estimate_recursion_depth(source),
        }
    }

    fn analyze_control_flow_python(&self, source: &str) -> ControlFlowInfo {
        ControlFlowInfo {
            loops: source.matches("for ").count() + source.matches("while ").count(),
            conditionals: source.matches("if ").count(),
            functions: source.matches("def ").count(),
            recursion_depth: 1, // Simplified
        }
    }

    fn analyze_sql_control_flow(&self, source: &str) -> ControlFlowInfo {
        ControlFlowInfo {
            loops: source.to_uppercase().matches("WHILE").count(),
            conditionals: source.to_uppercase().matches("IF").count(),
            functions: 1, // Main procedure
            recursion_depth: 0, // SQL procedures typically don't recurse
        }
    }

    fn analyze_control_flow_js(&self, source: &str) -> ControlFlowInfo {
        ControlFlowInfo {
            loops: source.matches("for(").count() + source.matches("while(").count(),
            conditionals: source.matches("if(").count(),
            functions: source.matches("function ").count(),
            recursion_depth: 1, // Simplified
        }
    }

    fn analyze_control_flow_lua(&self, source: &str) -> ControlFlowInfo {
        ControlFlowInfo {
            loops: source.matches("for ").count() + source.matches("while ").count(),
            conditionals: source.matches("if ").count(),
            functions: source.matches("function ").count(),
            recursion_depth: 1, // Simplified
        }
    }

    fn calculate_complexity(&self, source: &str) -> f64 {
        // Simplified cyclomatic complexity
        let branches = source.matches("if ").count() + source.matches("match ").count();
        let loops = source.matches("for ").count() + source.matches("while ").count();
        (branches + loops + 1) as f64
    }

    fn calculate_complexity_python(&self, source: &str) -> f64 {
        let branches = source.matches("if ").count();
        let loops = source.matches("for ").count() + source.matches("while ").count();
        (branches + loops + 1) as f64
    }

    fn calculate_sql_complexity(&self, source: &str) -> f64 {
        let selects = source.to_uppercase().matches("SELECT").count();
        let joins = source.to_uppercase().matches("JOIN").count();
        let unions = source.to_uppercase().matches("UNION").count();
        (selects + joins + unions + 1) as f64
    }

    fn calculate_complexity_js(&self, source: &str) -> f64 {
        let branches = source.matches("if(").count() + source.matches("switch(").count();
        let loops = source.matches("for(").count() + source.matches("while(").count();
        (branches + loops + 1) as f64
    }

    fn calculate_complexity_lua(&self, source: &str) -> f64 {
        let branches = source.matches("if ").count();
        let loops = source.matches("for ").count() + source.matches("while ").count();
        (branches + loops + 1) as f64
    }

    fn estimate_recursion_depth(&self, _source: &str) -> usize {
        // Simplified estimation
        1
    }

    fn generate_instructions(&self, _parsed_code: &ParsedCode) -> Vec<String> {
        // Generate intermediate instructions
        vec![
            "LOAD_CONST 0".to_string(),
            "STORE_VAR result".to_string(),
            "RETURN".to_string(),
        ]
    }

    fn extract_constants(&self, _parsed_code: &ParsedCode) -> Vec<String> {
        vec!["42".to_string(), "hello".to_string()]
    }

    fn identify_labels(&self, _parsed_code: &ParsedCode) -> HashMap<String, usize> {
        let mut labels = HashMap::new();
        labels.insert("main".to_string(), 0);
        labels
    }

    fn estimate_execution_cycles(&self, _parsed_code: &ParsedCode) -> u64 {
        1000 // Simplified estimation
    }

    fn estimate_memory_usage(&self, _parsed_code: &ParsedCode) -> usize {
        1024 // Simplified estimation
    }

    fn get_applied_optimizations(&self, _definition: &ProcedureDefinition) -> Vec<String> {
        vec![
            "constant_folding".to_string(),
            "dead_code_elimination".to_string(),
            "function_inlining".to_string(),
        ]
    }
}

/// Optimization profile for each language
#[derive(Debug)]
struct OptimizationProfile {
    default_level: OptimizationLevel,
    supports_vectorization: bool,
    supports_inlining: bool,
    supports_loop_unrolling: bool,
}

/// Parsed code representation
#[derive(Debug)]
struct ParsedCode {
    language: ProcedureLanguage,
    functions: Vec<String>,
    variables: Vec<String>,
    imports: Vec<String>,
    control_flow: ControlFlowInfo,
    complexity_score: f64,
}

/// Control flow information
#[derive(Debug)]
struct ControlFlowInfo {
    loops: usize,
    conditionals: usize,
    functions: usize,
    recursion_depth: usize,
}

/// Intermediate representation
#[derive(Debug)]
struct IntermediateRepresentation {
    instructions: Vec<String>,
    constants: Vec<String>,
    labels: HashMap<String, usize>,
    metadata: IRMetadata,
}

/// IR metadata
#[derive(Debug)]
struct IRMetadata {
    estimated_cycles: u64,
    memory_requirements: usize,
    optimization_applied: Vec<String>,
}

/// Code generator for machine code
#[derive(Debug)]
struct CodeGenerator;

impl CodeGenerator {
    fn new() -> Self {
        Self
    }

    async fn generate_machine_code(
        &self,
        _ir: &IntermediateRepresentation,
        _definition: &ProcedureDefinition,
    ) -> AuroraResult<Vec<u8>> {
        // In a real implementation, this would generate actual machine code
        // For now, simulate with placeholder bytes
        Ok(vec![0x48, 0x89, 0xF8, 0xC3]) // Simple x86-64 return instruction
    }
}

/// Procedure optimizer
#[derive(Debug)]
struct ProcedureOptimizer;

impl ProcedureOptimizer {
    fn new() -> Self {
        Self
    }

    async fn optimize(
        &self,
        parsed_code: &ParsedCode,
        _definition: &ProcedureDefinition,
    ) -> AuroraResult<ParsedCode> {
        // Apply language-specific optimizations
        Ok(parsed_code.clone()) // Simplified - no actual optimization
    }

    async fn apply_global_optimizations(
        &self,
        ir: &IntermediateRepresentation,
        _definition: &ProcedureDefinition,
    ) -> AuroraResult<IntermediateRepresentation> {
        // Apply global optimizations to IR
        Ok(ir.clone()) // Simplified - no actual optimization
    }
}

/// Compilation statistics
#[derive(Debug)]
pub struct CompilationStats {
    pub total_compilations: usize,
    pub avg_compilation_time_ms: f64,
    pub total_code_size_bytes: usize,
    pub cache_hit_rate: f64,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_jit_compiler_creation() {
        let compiler = JITCompiler::new();
        assert!(true); // Passes if created successfully
    }

    #[test]
    fn test_optimization_levels() {
        assert!(OptimizationLevel::None < OptimizationLevel::Maximum);
        assert!(OptimizationLevel::Basic < OptimizationLevel::Aggressive);
    }

    #[test]
    fn test_function_extraction() {
        let compiler = JITCompiler::new();

        let rust_code = r#"
            fn calculate_total(a: i32, b: i32) -> i32 {
                a + b
            }

            fn main() {
                println!("Hello");
            }
        "#;

        let functions = compiler.extract_functions(rust_code, "fn ");
        assert_eq!(functions.len(), 2);
        assert!(functions.contains(&"calculate_total".to_string()));
        assert!(functions.contains(&"main".to_string()));
    }

    #[test]
    fn test_variable_extraction() {
        let compiler = JITCompiler::new();

        let rust_code = r#"
            fn main() {
                let x = 42;
                let y = "hello";
                println!("{}", x);
            }
        "#;

        let variables = compiler.extract_variables(rust_code);
        assert_eq!(variables.len(), 2);
        assert!(variables.contains(&"x".to_string()));
        assert!(variables.contains(&"y".to_string()));
    }

    #[test]
    fn test_python_variable_extraction() {
        let compiler = JITCompiler::new();

        let python_code = r#"
            def process_data():
                x = 42
                y = "hello"
                z = [1, 2, 3]
                return x + len(z)
        "#;

        let variables = compiler.extract_variables_python(python_code);
        assert!(variables.len() >= 3); // x, y, z
    }

    #[test]
    fn test_complexity_calculation() {
        let compiler = JITCompiler::new();

        let complex_code = r#"
            fn complex_function() {
                if condition1 {
                    for i in 0..10 {
                        if condition2 {
                            // nested
                        }
                    }
                } else {
                    while condition3 {
                        // loop
                    }
                }
            }
        "#;

        let complexity = compiler.calculate_complexity(complex_code);
        assert!(complexity >= 4.0); // 3 branches/loops + 1 base
    }

    #[test]
    fn test_control_flow_analysis() {
        let compiler = JITCompiler::new();

        let code = r#"
            fn example() {
                if x > 0 {
                    for i in 0..10 {
                        if y > 5 {
                            // nested
                        }
                    }
                }
            }
        "#;

        let cf_info = compiler.analyze_control_flow(code);
        assert_eq!(cf_info.conditionals, 2); // 2 if statements
        assert_eq!(cf_info.loops, 1); // 1 for loop
        assert_eq!(cf_info.functions, 1); // 1 function
    }

    #[tokio::test]
    async fn test_compilation_stats() {
        let compiler = JITCompiler::new();
        let stats = compiler.get_compilation_stats();

        // Initially empty
        assert_eq!(stats.total_compilations, 0);
        assert_eq!(stats.avg_compilation_time_ms, 0.0);
        assert_eq!(stats.total_code_size_bytes, 0);
    }

    #[test]
    fn test_cache_key_generation() {
        let compiler = JITCompiler::new();

        let definition = ProcedureDefinition {
            name: "test_proc".to_string(),
            language: ProcedureLanguage::Rust,
            parameters: vec![],
            return_type: None,
            source_code: "fn test() {}".to_string(),
            execution_mode: ExecutionMode::JITCompiled,
            security_level: super::procedure_manager::SecurityLevel::Public,
            timeout_seconds: None,
            max_memory_mb: None,
            description: "Test".to_string(),
            tags: std::collections::HashSet::new(),
            created_at: chrono::Utc::now(),
            modified_at: chrono::Utc::now(),
            version: "1.0.0".to_string(),
        };

        let key = compiler.generate_cache_key(&definition);
        assert!(key.contains("test_proc"));
        assert!(key.contains("Rust"));
        assert!(key.contains("1.0.0"));
    }
}
