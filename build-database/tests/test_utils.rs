//! Test Utilities and Helpers
//!
//! Common testing utilities, fixtures, and helper functions.
//! Provides reusable components for comprehensive test coverage.

use aurora_db::*;
use std::collections::HashMap;
use std::sync::Arc;
use tokio::runtime::Runtime;

/// Test database fixture
pub struct TestDatabase {
    pub db: AuroraDB,
    pub runtime: Runtime,
}

impl TestDatabase {
    /// Create a new test database instance
    pub fn new() -> Self {
        let runtime = Runtime::new().unwrap();

        let config = DatabaseConfig {
            max_connections: 5,
            buffer_pool_size: 16 * 1024 * 1024, // 16MB for tests
            max_tables: 5,
            max_columns_per_table: 10,
            default_isolation_level: IsolationLevel::ReadCommitted,
            transaction_timeout_ms: 5000,
            enable_query_logging: false,
            enable_metrics: false,
        };

        let db = runtime.block_on(AuroraDB::new(config))
            .expect("Failed to create test database");

        Self { db, runtime }
    }

    /// Execute a query and return the result
    pub fn execute(&self, sql: &str) -> Result<QueryResult, Box<dyn std::error::Error>> {
        self.runtime.block_on(self.db.execute_query(sql))
            .map_err(|e| e.into())
    }

    /// Create a test table with sample data
    pub fn setup_test_table(&self, table_name: &str, rows: usize) -> Result<(), Box<dyn std::error::Error>> {
        // Create table
        let create_sql = format!(r#"
            CREATE TABLE {} (
                id INTEGER PRIMARY KEY,
                name VARCHAR(100),
                value INTEGER
            )
        "#, table_name);

        self.execute(&create_sql)?;

        // Insert test data
        for i in 0..rows {
            let insert_sql = format!(r#"
                INSERT INTO {} (id, name, value) VALUES ({}, 'Test{}', {})
            "#, table_name, i, i, i * 10);

            self.execute(&insert_sql)?;
        }

        Ok(())
    }

    /// Clean up test data
    pub fn cleanup(&self) -> Result<(), Box<dyn std::error::Error>> {
        // Drop all test tables
        let tables = vec!["test_table", "users", "products", "orders"];

        for table in tables {
            let _ = self.execute(&format!("DROP TABLE IF EXISTS {}", table));
        }

        Ok(())
    }
}

impl Drop for TestDatabase {
    fn drop(&mut self) {
        let _ = self.cleanup();
    }
}

/// Performance measurement utilities
pub struct PerformanceTimer {
    start_time: std::time::Instant,
    measurements: Vec<(String, std::time::Duration)>,
}

impl PerformanceTimer {
    pub fn new() -> Self {
        Self {
            start_time: std::time::Instant::now(),
            measurements: vec![],
        }
    }

    pub fn checkpoint(&mut self, name: &str) {
        let elapsed = self.start_time.elapsed();
        self.measurements.push((name.to_string(), elapsed));
    }

    pub fn report(&self) -> PerformanceReport {
        let mut report = PerformanceReport {
            total_time: self.start_time.elapsed(),
            checkpoints: self.measurements.clone(),
            averages: HashMap::new(),
        };

        // Calculate time between checkpoints
        for i in 1..self.measurements.len() {
            let prev_time = self.measurements[i-1].1;
            let curr_time = self.measurements[i].1;
            let duration = curr_time - prev_time;
            let name = format!("{}_to_{}", self.measurements[i-1].0, self.measurements[i].0);
            report.averages.insert(name, duration);
        }

        report
    }
}

#[derive(Debug, Clone)]
pub struct PerformanceReport {
    pub total_time: std::time::Duration,
    pub checkpoints: Vec<(String, std::time::Duration)>,
    pub averages: HashMap<String, std::time::Duration>,
}

impl PerformanceReport {
    pub fn print(&self) {
        println!("Performance Report:");
        println!("  Total time: {:.2}ms", self.total_time.as_millis());

        for (checkpoint, duration) in &self.checkpoints {
            println!("  {}: {:.2}ms", checkpoint, duration.as_millis());
        }

        for (interval, duration) in &self.averages {
            println!("  {}: {:.2}ms", interval, duration.as_millis());
        }
    }
}

/// Memory usage measurement
pub struct MemoryMonitor {
    initial_memory: usize,
    measurements: Vec<(String, usize)>,
}

impl MemoryMonitor {
    pub fn new() -> Self {
        Self {
            initial_memory: Self::get_current_memory(),
            measurements: vec![],
        }
    }

    pub fn checkpoint(&mut self, name: &str) {
        let current = Self::get_current_memory();
        self.measurements.push((name.to_string(), current));
    }

    pub fn report(&self) -> MemoryReport {
        let final_memory = Self::get_current_memory();
        let peak_memory = self.measurements.iter()
            .map(|(_, mem)| *mem)
            .max()
            .unwrap_or(self.initial_memory);

        MemoryReport {
            initial_memory: self.initial_memory,
            final_memory,
            peak_memory,
            measurements: self.measurements.clone(),
        }
    }

    fn get_current_memory() -> usize {
        // In a real implementation, this would use system APIs
        // For now, return a mock value
        1024 * 1024 // 1MB mock
    }
}

#[derive(Debug, Clone)]
pub struct MemoryReport {
    pub initial_memory: usize,
    pub final_memory: usize,
    pub peak_memory: usize,
    pub measurements: Vec<(String, usize)>,
}

impl MemoryReport {
    pub fn print(&self) {
        println!("Memory Report:");
        println!("  Initial: {} bytes", self.initial_memory);
        println!("  Final: {} bytes", self.final_memory);
        println!("  Peak: {} bytes", self.peak_memory);
        println!("  Delta: {} bytes", self.final_memory as isize - self.initial_memory as isize);

        for (checkpoint, memory) in &self.measurements {
            println!("  {}: {} bytes", checkpoint, memory);
        }
    }
}

/// Concurrent test runner
pub struct ConcurrentTestRunner {
    runtime: Runtime,
}

impl ConcurrentTestRunner {
    pub fn new() -> Self {
        Self {
            runtime: Runtime::new().unwrap(),
        }
    }

    /// Run multiple operations concurrently
    pub async fn run_concurrent<F, Fut, T>(&self, operations: Vec<F>) -> Vec<Result<T, Box<dyn std::error::Error + Send + Sync>>>
    where
        F: FnOnce() -> Fut + Send + 'static,
        Fut: std::future::Future<Output = Result<T, Box<dyn std::error::Error + Send + Sync>>> + Send,
        T: Send + 'static,
    {
        let mut handles = vec![];

        for operation in operations {
            let handle = tokio::spawn(operation());
            handles.push(handle);
        }

        let mut results = vec![];
        for handle in handles {
            let result = handle.await
                .map_err(|e| Box::new(e) as Box<dyn std::error::Error + Send + Sync>)
                .and_then(|r| r);
            results.push(result);
        }

        results
    }

    /// Run operations with controlled concurrency
    pub async fn run_with_semaphore<F, Fut, T>(
        &self,
        operations: Vec<F>,
        max_concurrent: usize
    ) -> Vec<Result<T, Box<dyn std::error::Error + Send + Sync>>>
    where
        F: FnOnce() -> Fut + Send + 'static,
        Fut: std::future::Future<Output = Result<T, Box<dyn std::error::Error + Send + Sync>>> + Send,
        T: Send + 'static,
    {
        use tokio::sync::Semaphore;
        let semaphore = Arc::new(Semaphore::new(max_concurrent));

        let operations: Vec<_> = operations.into_iter().enumerate().map(|(i, op)| {
            let sem = semaphore.clone();
            async move {
                let _permit = sem.acquire().await.map_err(|e| {
                    Box::new(e) as Box<dyn std::error::Error + Send + Sync>
                })?;
                op().await
            }
        }).collect();

        self.run_concurrent(operations).await
    }
}

/// SQL query validator
pub struct SqlValidator;

impl SqlValidator {
    /// Validate SQL syntax (basic checks)
    pub fn validate_syntax(sql: &str) -> Result<(), String> {
        let sql = sql.trim();

        if sql.is_empty() {
            return Err("Empty SQL statement".to_string());
        }

        // Check for basic SQL keywords
        let upper_sql = sql.to_uppercase();
        let has_select = upper_sql.contains("SELECT");
        let has_insert = upper_sql.contains("INSERT");
        let has_update = upper_sql.contains("UPDATE");
        let has_delete = upper_sql.contains("DELETE");
        let has_create = upper_sql.contains("CREATE");

        let keyword_count = [has_select, has_insert, has_update, has_delete, has_create]
            .iter()
            .filter(|&&x| x)
            .count();

        if keyword_count == 0 {
            return Err("No valid SQL keyword found".to_string());
        }

        if keyword_count > 1 {
            return Err("Multiple SQL keywords found - only one operation per statement allowed".to_string());
        }

        // Check for balanced parentheses
        let paren_count = sql.chars().fold(0, |count, c| {
            match c {
                '(' => count + 1,
                ')' => count - 1,
                _ => count,
            }
        });

        if paren_count != 0 {
            return Err("Unbalanced parentheses".to_string());
        }

        // Check for balanced quotes
        let quote_count = sql.chars().fold(0, |count, c| {
            match c {
                '\'' => count + 1,
                _ => count,
            }
        });

        if quote_count % 2 != 0 {
            return Err("Unbalanced quotes".to_string());
        }

        Ok(())
    }

    /// Validate query result format
    pub fn validate_result(result: &QueryResult) -> Result<(), String> {
        if result.columns.is_empty() && !result.data.is_empty() {
            return Err("Result has data but no column names".to_string());
        }

        if !result.columns.is_empty() && result.data.is_empty() && result.row_count > 0 {
            return Err("Result has columns but no data despite row_count > 0".to_string());
        }

        // Check that execution time is reasonable (not negative, not too large)
        if result.execution_time_ms < 0.0 {
            return Err("Negative execution time".to_string());
        }

        if result.execution_time_ms > 300000.0 { // 5 minutes
            return Err("Execution time suspiciously large".to_string());
        }

        Ok(())
    }
}

/// Test assertion helpers
pub struct TestAssertions;

impl TestAssertions {
    /// Assert that a query returns expected row count
    pub fn assert_row_count(result: &QueryResult, expected: usize, context: &str) {
        assert_eq!(result.row_count, expected,
                  "{}: Expected {} rows, got {}", context, expected, result.row_count);
    }

    /// Assert that a query returns specific data
    pub fn assert_contains_data(result: &QueryResult, expected_data: &str, context: &str) {
        let data_str = result.data.join(" ");
        assert!(data_str.contains(expected_data),
               "{}: Expected data '{}' not found in result: {:?}", context, expected_data, result.data);
    }

    /// Assert that execution time is within bounds
    pub fn assert_execution_time(result: &QueryResult, max_ms: f64, context: &str) {
        assert!(result.execution_time_ms <= max_ms,
               "{}: Execution time {}ms exceeds maximum {}ms", context, result.execution_time_ms, max_ms);
    }

    /// Assert that transaction completed successfully
    pub fn assert_transaction_success<T, E>(result: &Result<T, E>, context: &str)
    where
        E: std::fmt::Debug,
    {
        assert!(result.is_ok(), "{}: Transaction failed: {:?}", context, result.as_ref().err());
    }

    /// Assert that performance meets baseline
    pub fn assert_performance_baseline(actual: f64, baseline: f64, tolerance_percent: f64, context: &str) {
        let tolerance = baseline * (tolerance_percent / 100.0);
        let min_acceptable = baseline - tolerance;

        assert!(actual >= min_acceptable,
               "{}: Performance {} below baseline {} (tolerance: {}%)",
               context, actual, baseline, tolerance_percent);
    }
}

/// Benchmark result analyzer
pub struct BenchmarkAnalyzer;

impl BenchmarkAnalyzer {
    /// Analyze benchmark results for regressions
    pub fn analyze_results(results: &[(String, std::time::Duration)], baseline: &[(String, std::time::Duration)]) -> AnalysisResult {
        let mut regressions = vec![];
        let mut improvements = vec![];

        for (name, duration) in results {
            if let Some((_, baseline_duration)) = baseline.iter().find(|(b_name, _)| b_name == name) {
                let ratio = duration.as_nanos() as f64 / baseline_duration.as_nanos() as f64;

                if ratio > 1.1 { // 10% regression
                    regressions.push((name.clone(), ratio));
                } else if ratio < 0.9 { // 10% improvement
                    improvements.push((name.clone(), ratio));
                }
            }
        }

        AnalysisResult {
            regressions,
            improvements,
            total_tests: results.len(),
        }
    }

    /// Generate performance summary
    pub fn generate_summary(results: &[(String, std::time::Duration)]) -> String {
        let mut summary = String::from("Performance Summary:\n");

        for (name, duration) in results {
            summary.push_str(&format!("  {}: {:.2}ms\n", name, duration.as_millis()));
        }

        let avg_time = results.iter()
            .map(|(_, d)| d.as_millis())
            .sum::<u128>() as f64 / results.len() as f64;

        summary.push_str(&format!("  Average: {:.2}ms\n", avg_time));

        summary
    }
}

#[derive(Debug)]
pub struct AnalysisResult {
    pub regressions: Vec<(String, f64)>,
    pub improvements: Vec<(String, f64)>,
    pub total_tests: usize,
}

impl AnalysisResult {
    pub fn has_regressions(&self) -> bool {
        !self.regressions.is_empty()
    }

    pub fn print(&self) {
        println!("Benchmark Analysis:");

        if !self.regressions.is_empty() {
            println!("  Regressions:");
            for (name, ratio) in &self.regressions {
                println!("    {}: {:.1}% slower", name, (ratio - 1.0) * 100.0);
            }
        }

        if !self.improvements.is_empty() {
            println!("  Improvements:");
            for (name, ratio) in &self.improvements {
                println!("    {}: {:.1}% faster", name, (1.0 - ratio) * 100.0);
            }
        }

        println!("  Total tests: {}", self.total_tests);
    }
}
