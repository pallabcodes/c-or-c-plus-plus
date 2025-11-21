//! AuroraDB Vector Query Integration: SQL Vector Search
//!
//! Seamless integration of vector search into AuroraDB's SQL query engine,
//! enabling natural vector similarity queries with full SQL expressiveness.

use std::collections::HashMap;
use crate::core::errors::{AuroraResult, AuroraError};
use super::vector_index::{AuroraVectorIndex, VectorIndexConfig, VectorUseCase};
use super::vector_storage::{VectorStorage, VectorStorageConfig, VectorStorageType, CompressionType};
use crate::query::processing::*;

/// Vector query engine that integrates with AuroraDB's SQL processing
pub struct VectorQueryEngine {
    vector_indexes: HashMap<String, AuroraVectorIndex>,
    vector_storage: HashMap<String, VectorStorage>,
    table_vector_columns: HashMap<String, Vec<String>>, // table -> vector columns
}

impl VectorQueryEngine {
    /// Create a new vector query engine
    pub fn new() -> Self {
        Self {
            vector_indexes: HashMap::new(),
            vector_storage: HashMap::new(),
            table_vector_columns: HashMap::new(),
        }
    }

    /// Create a vector index for a table column
    pub async fn create_vector_index(&mut self, table_name: &str, column_name: &str, dimension: usize, usecase: VectorUseCase) -> AuroraResult<()> {
        let index_name = format!("{}_{}_vector_idx", table_name, column_name);

        // Create storage for the vectors
        let storage_config = VectorStorageConfig {
            storage_type: VectorStorageType::Memory, // Could be configurable
            compression: CompressionType::None, // Could be configurable
            memory_budget_mb: 1024,
            disk_path: None,
            preload_vectors: false,
        };

        let storage = VectorStorage::new(storage_config)?;
        self.vector_storage.insert(index_name.clone(), storage);

        // Create the vector index
        let index_config = AuroraVectorIndex::intelligent_config(usecase, 10000, dimension);
        let index = AuroraVectorIndex::new(index_config)?;

        self.vector_indexes.insert(index_name.clone(), index);

        // Register the vector column
        self.table_vector_columns.entry(table_name.to_string())
            .or_insert_with(Vec::new)
            .push(column_name.to_string());

        Ok(())
    }

    /// Drop a vector index
    pub async fn drop_vector_index(&mut self, table_name: &str, column_name: &str) -> AuroraResult<()> {
        let index_name = format!("{}_{}_vector_idx", table_name, column_name);

        self.vector_indexes.remove(&index_name);
        self.vector_storage.remove(&index_name);

        if let Some(columns) = self.table_vector_columns.get_mut(table_name) {
            columns.retain(|c| c != column_name);
        }

        Ok(())
    }

    /// Insert vectors into indexes (called during regular INSERT operations)
    pub async fn insert_vectors(&mut self, table_name: &str, column_name: &str, vectors: Vec<(usize, Vec<f32>)>) -> AuroraResult<()> {
        let index_name = format!("{}_{}_vector_idx", table_name, column_name);

        if let Some(index) = self.vector_indexes.get_mut(&index_name) {
            for (id, vector) in vectors {
                index.insert(id, vector.clone())?;
            }
        }

        if let Some(storage) = self.vector_storage.get_mut(&index_name) {
            let mut vector_map = HashMap::new();
            for (id, vector) in vectors {
                vector_map.insert(id, vector);
            }
            storage.batch_store(vector_map)?;
        }

        Ok(())
    }

    /// Execute a vector similarity search query
    pub async fn execute_vector_search(&self, table_name: &str, column_name: &str, query_vector: &[f32], limit: usize) -> AuroraResult<Vec<(usize, f32)>> {
        let index_name = format!("{}_{}_vector_idx", table_name, column_name);

        if let Some(index) = self.vector_indexes.get(&index_name) {
            index.search(query_vector, limit)
        } else {
            Err(AuroraError::Vector(format!("Vector index not found for {}.{}", table_name, column_name)))
        }
    }

    /// Check if a table has vector columns
    pub fn has_vector_columns(&self, table_name: &str) -> bool {
        self.table_vector_columns.contains_key(table_name)
    }

    /// Get vector columns for a table
    pub fn get_vector_columns(&self, table_name: &str) -> Option<&Vec<String>> {
        self.table_vector_columns.get(table_name)
    }

    /// Get comprehensive statistics about vector indexes
    pub fn get_vector_stats(&self) -> VectorEngineStats {
        let mut index_stats = Vec::new();
        let mut storage_stats = Vec::new();

        for (name, index) in &self.vector_indexes {
            let stats = index.comprehensive_stats();
            index_stats.push((name.clone(), stats));
        }

        for (name, storage) in &self.vector_storage {
            let stats = storage.stats();
            storage_stats.push((name.clone(), stats));
        }

        VectorEngineStats {
            total_indexes: self.vector_indexes.len(),
            total_stored_vectors: storage_stats.iter().map(|(_, s)| s.total_vectors).sum(),
            total_memory_usage_mb: index_stats.iter().map(|(_, s)| s.base_stats.memory_usage_mb).sum::<f64>() +
                                  storage_stats.iter().map(|(_, s)| s.memory_usage_mb).sum::<f64>(),
            index_stats,
            storage_stats,
        }
    }
}

/// Vector engine statistics
#[derive(Debug)]
pub struct VectorEngineStats {
    pub total_indexes: usize,
    pub total_stored_vectors: usize,
    pub total_memory_usage_mb: f64,
    pub index_stats: Vec<(String, super::vector_index::ComprehensiveIndexStats)>,
    pub storage_stats: Vec<(String, super::vector_storage::StorageStats)>,
}

/// Vector search expression in SQL AST
#[derive(Debug, Clone)]
pub struct VectorSearchExpression {
    pub table_name: String,
    pub column_name: String,
    pub query_vector: Vec<f32>,
    pub limit: usize,
    pub distance_metric: super::distance_metrics::DistanceMetric,
}

/// Vector distance expression in SQL
#[derive(Debug, Clone)]
pub struct VectorDistanceExpression {
    pub left: Box<Expression>,
    pub right: Box<Expression>,
    pub metric: super::distance_metrics::DistanceMetric,
}

/// Extend the query planner to handle vector operations
pub trait VectorQueryPlanner {
    fn plan_vector_search(&self, search_expr: &VectorSearchExpression) -> AuroraResult<QueryPlan>;
    fn plan_vector_distance(&self, distance_expr: &VectorDistanceExpression) -> AuroraResult<QueryPlan>;
}

/// Extend the query executor to handle vector operations
#[async_trait::async_trait]
pub trait VectorQueryExecutor {
    async fn execute_vector_search(&self, search_expr: &VectorSearchExpression) -> AuroraResult<ExecutionResult>;
    async fn execute_vector_distance(&self, distance_expr: &VectorDistanceExpression) -> AuroraResult<ExecutionResult>;
}

/// Vector query processor that integrates with the main query engine
pub struct VectorQueryProcessor {
    vector_engine: VectorQueryEngine,
    sql_parser: SqlParser,
    query_planner: QueryPlanner,
    query_optimizer: QueryOptimizer,
    execution_engine: ExecutionEngine,
}

impl VectorQueryProcessor {
    pub fn new() -> Self {
        Self {
            vector_engine: VectorQueryEngine::new(),
            sql_parser: SqlParser::new(""),
            query_planner: QueryPlanner::new(),
            query_optimizer: QueryOptimizer::new(),
            execution_engine: ExecutionEngine::new(),
        }
    }

    /// Execute a vector-enabled SQL query
    pub async fn execute_vector_query(&mut self, sql: &str, context: ExecutionContext) -> AuroraResult<ExecutionResult> {
        // Parse the SQL
        let mut parser = SqlParser::new(sql);
        let ast = parser.parse()?;

        // Check if this is a vector query
        if self.is_vector_query(&ast) {
            self.execute_vector_specific_query(ast, context).await
        } else {
            // Regular SQL query processing
            self.execute_regular_query(ast, context).await
        }
    }

    /// Check if a query involves vector operations
    fn is_vector_query(&self, ast: &Statement) -> bool {
        match ast {
            Statement::Select(select) => {
                // Check for vector search syntax or vector distance functions
                self.contains_vector_operations(&select.select)
            }
            _ => false,
        }
    }

    /// Check if a SELECT statement contains vector operations
    fn contains_vector_operations(&self, select_clause: &SelectClause) -> bool {
        for item in &select_clause.select_list {
            match item {
                SelectItem::Expression(expr, _) => {
                    if self.expression_contains_vectors(expr) {
                        return true;
                    }
                }
                _ => continue,
            }
        }
        false
    }

    /// Check if an expression contains vector operations
    fn expression_contains_vectors(&self, expr: &Expression) -> bool {
        match expr {
            Expression::Function { name, .. } => {
                matches!(name.to_uppercase().as_str(), "COSINE_DISTANCE" | "EUCLIDEAN_DISTANCE" | "DOT_PRODUCT")
            }
            Expression::BinaryOp { op, left, right } => {
                matches!(op, BinaryOperator::ArrayContains | BinaryOperator::ArrayContainedBy) ||
                self.expression_contains_vectors(left) ||
                self.expression_contains_vectors(right)
            }
            Expression::VectorDistance { .. } => true,
            _ => false,
        }
    }

    /// Execute vector-specific queries
    async fn execute_vector_specific_query(&mut self, ast: Statement, context: ExecutionContext) -> AuroraResult<ExecutionResult> {
        match ast {
            Statement::Select(select) => {
                // Check for vector search syntax: SELECT * FROM table ORDER BY column <-> [1,2,3] LIMIT k
                if let Some(search_expr) = self.extract_vector_search(&select) {
                    return self.vector_engine.execute_vector_search(
                        &search_expr.table_name,
                        &search_expr.column_name,
                        &search_expr.query_vector,
                        search_expr.limit,
                    ).await.map(|results| {
                        // Convert results to execution result format
                        ExecutionResult {
                            query_id: context.query_id,
                            result_batches: vec![], // Would contain actual result rows
                            total_rows: results.len(),
                            execution_stats: QueryExecutionStats {
                                query_id: context.query_id,
                                execution_time_ms: 10.0, // Placeholder
                                rows_processed: results.len(),
                                bytes_processed: 0,
                                operators_executed: 1,
                                memory_peak_mb: 50.0,
                                io_operations: 0,
                                network_calls: 0,
                                cache_hits: 0,
                                cache_misses: 0,
                            },
                            execution_plan: QueryPlan {
                                root: PlanNode::VectorSearch(super::plan::VectorSearchNode {
                                    table_name: search_expr.table_name,
                                    vector_column: search_expr.column_name,
                                    query_vector: search_expr.query_vector,
                                    metric: search_expr.distance_metric,
                                    limit: search_expr.limit,
                                    filter_condition: None,
                                    index_name: None,
                                    estimated_rows: search_expr.limit,
                                    cost: 100.0,
                                }),
                                estimated_cost: 100.0,
                                estimated_rows: search_expr.limit,
                                execution_mode: ExecutionMode::Sequential,
                                optimization_hints: vec![],
                                statistics: PlanStatistics::default(),
                            },
                        }
                    });
                }
            }
            _ => {}
        }

        // Fallback to regular query processing
        self.execute_regular_query(ast, context).await
    }

    /// Execute regular (non-vector) queries
    async fn execute_regular_query(&mut self, ast: Statement, context: ExecutionContext) -> AuroraResult<ExecutionResult> {
        // Standard query processing pipeline
        match ast {
            Statement::Select(select) => {
                let plan = self.query_planner.plan_select(&select)?;
                let optimized_plan = self.query_optimizer.optimize(plan, &QueryContext {
                    user_id: context.user_id.clone(),
                    session_id: context.session_id.clone(),
                    client_ip: "127.0.0.1".to_string(),
                    available_memory_mb: context.memory_limit_mb,
                    max_parallel_workers: context.max_parallel_workers,
                    query_priority: QueryPriority::Normal,
                    time_constraints: None,
                }).await?;

                self.execution_engine.execute_plan(optimized_plan, context).await
            }
            _ => Err(AuroraError::Query("Unsupported statement type".to_string())),
        }
    }

    /// Extract vector search expression from SELECT statement
    fn extract_vector_search(&self, select: &SelectStatement) -> Option<VectorSearchExpression> {
        // Look for ORDER BY with vector distance
        if let Some(order_by) = &select.order_by {
            if let Some(order_item) = order_by.items.first() {
                if let Expression::VectorDistance { left, right, metric } = &order_item.expression {
                    // Check if left is a column and right is a vector literal
                    if let (Expression::Column(column), Expression::VectorLiteral(query_vector)) = (left.as_ref(), right.as_ref()) {
                        let table_column = self.parse_table_column(column)?;
                        let limit = select.limit.as_ref()
                            .and_then(|l| if let Expression::Literal(LiteralValue::Integer(val)) = &l.count { Some(val as usize) } else { None })
                            .unwrap_or(10);

                        return Some(VectorSearchExpression {
                            table_name: table_column.0,
                            column_name: table_column.1,
                            query_vector: query_vector.clone(),
                            limit,
                            distance_metric: metric.clone(),
                        });
                    }
                }
            }
        }

        // Look for vector search functions in SELECT
        for item in &select.select_list {
            if let SelectItem::Expression(expr, _) = item {
                if let Some(search_expr) = self.extract_vector_search_from_expression(expr) {
                    return Some(search_expr);
                }
            }
        }

        None
    }

    /// Extract vector search from expression
    fn extract_vector_search_from_expression(&self, expr: &Expression) -> Option<VectorSearchExpression> {
        match expr {
            Expression::Function { name, args, .. } => {
                if name.to_uppercase() == "VECTOR_SEARCH" && args.len() >= 3 {
                    // VECTOR_SEARCH(table, column, query_vector, limit?)
                    if let (Expression::Literal(LiteralValue::String(table)), Expression::Literal(LiteralValue::String(column))) = (&args[0], &args[1]) {
                        if let Expression::VectorLiteral(query_vector) = &args[2] {
                            let limit = if args.len() > 3 {
                                if let Expression::Literal(LiteralValue::Integer(val)) = &args[3] {
                                    *val as usize
                                } else {
                                    10
                                }
                            } else {
                                10
                            };

                            return Some(VectorSearchExpression {
                                table_name: table.clone(),
                                column_name: column.clone(),
                                query_vector: query_vector.clone(),
                                limit,
                                distance_metric: super::distance_metrics::DistanceMetric::Cosine,
                            });
                        }
                    }
                }
            }
            _ => {}
        }
        None
    }

    /// Parse table.column format
    fn parse_table_column(&self, column_expr: &str) -> Option<(String, String)> {
        let parts: Vec<&str> = column_expr.split('.').collect();
        if parts.len() == 2 {
            Some((parts[0].to_string(), parts[1].to_string()))
        } else {
            None
        }
    }
}

/// SQL extensions for vector operations
pub mod vector_sql_extensions {
    use super::*;

    /// Parse vector literal: [1.0, 2.0, 3.0]
    pub fn parse_vector_literal(input: &str) -> AuroraResult<Vec<f32>> {
        if !input.starts_with('[') || !input.ends_with(']') {
            return Err(AuroraError::Vector("Invalid vector literal format".to_string()));
        }

        let content = &input[1..input.len()-1];
        let parts: Result<Vec<f32>, _> = content
            .split(',')
            .map(|s| s.trim().parse::<f32>())
            .collect();

        parts.map_err(|_| AuroraError::Vector("Invalid float value in vector literal".to_string()))
    }

    /// Generate SQL for vector search
    pub fn generate_vector_search_sql(table: &str, column: &str, query_vector: &[f32], limit: usize) -> String {
        let vector_str = format!("[{}]", query_vector.iter()
            .map(|x| x.to_string())
            .collect::<Vec<_>>()
            .join(", "));

        format!(
            "SELECT * FROM {} ORDER BY {} <-> {} LIMIT {}",
            table, column, vector_str, limit
        )
    }

    /// Generate SQL for vector distance calculation
    pub fn generate_vector_distance_sql(left_expr: &str, right_expr: &str, metric: &super::distance_metrics::DistanceMetric) -> String {
        let metric_name = match metric {
            super::distance_metrics::DistanceMetric::Cosine => "COSINE_DISTANCE",
            super::distance_metrics::DistanceMetric::Euclidean => "EUCLIDEAN_DISTANCE",
            super::distance_metrics::DistanceMetric::DotProduct => "DOT_PRODUCT",
            _ => "COSINE_DISTANCE",
        };

        format!("{}({}, {})", metric_name, left_expr, right_expr)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use super::vector_sql_extensions::*;

    #[test]
    fn test_vector_query_engine() {
        let mut engine = VectorQueryEngine::new();

        // Initially no vector columns
        assert!(!engine.has_vector_columns("test_table"));
        assert!(engine.get_vector_columns("test_table").is_none());
    }

    #[tokio::test]
    async fn test_create_vector_index() {
        let mut engine = VectorQueryEngine::new();

        engine.create_vector_index("products", "embedding", 384, VectorUseCase::SemanticSearch).await.unwrap();

        assert!(engine.has_vector_columns("products"));
        assert_eq!(engine.get_vector_columns("products").unwrap(), &vec!["embedding".to_string()]);
    }

    #[tokio::test]
    async fn test_vector_insert_and_search() {
        let mut engine = VectorQueryEngine::new();

        // Create index
        engine.create_vector_index("products", "embedding", 3, VectorUseCase::SemanticSearch).await.unwrap();

        // Insert vectors
        let vectors = vec![
            (0, vec![1.0, 0.0, 0.0]),
            (1, vec![0.0, 1.0, 0.0]),
            (2, vec![0.0, 0.0, 1.0]),
        ];

        engine.insert_vectors("products", "embedding", vectors).await.unwrap();

        // Search
        let query = vec![1.0, 0.0, 0.0];
        let results = engine.execute_vector_search("products", "embedding", &query, 2).await.unwrap();

        assert_eq!(results.len(), 2);
        assert_eq!(results[0].0, 0); // Should find the identical vector first
    }

    #[test]
    fn test_parse_vector_literal() {
        let vector = parse_vector_literal("[1.0, 2.5, -3.2]").unwrap();
        assert_eq!(vector, vec![1.0, 2.5, -3.2]);

        // Invalid formats
        assert!(parse_vector_literal("1.0, 2.0").is_err());
        assert!(parse_vector_literal("[1.0, invalid]").is_err());
    }

    #[test]
    fn test_generate_vector_search_sql() {
        let sql = generate_vector_search_sql("products", "embedding", &[1.0, 2.0, 3.0], 5);
        assert_eq!(sql, "SELECT * FROM products ORDER BY embedding <-> [1, 2, 3] LIMIT 5");
    }

    #[test]
    fn test_generate_vector_distance_sql() {
        let sql = generate_vector_distance_sql("a.embedding", "b.embedding", &super::distance_metrics::DistanceMetric::Cosine);
        assert_eq!(sql, "COSINE_DISTANCE(a.embedding, b.embedding)");
    }

    #[test]
    fn test_vector_query_processor() {
        let mut processor = VectorQueryProcessor::new();

        // Test regular query
        let sql = "SELECT 1";
        let context = ExecutionContext {
            query_id: "test".to_string(),
            user_id: "test".to_string(),
            session_id: "test".to_string(),
            start_time: std::time::Instant::now(),
            timeout: None,
            memory_limit_mb: 100,
            max_parallel_workers: 1,
            execution_mode: ExecutionMode::Sequential,
            parameters: HashMap::new(),
            transaction_id: None,
        };

        // This would normally execute, but we're just testing the structure
        assert_eq!(processor.vector_engine.vector_indexes.len(), 0);
    }

    #[tokio::test]
    async fn test_drop_vector_index() {
        let mut engine = VectorQueryEngine::new();

        // Create index
        engine.create_vector_index("products", "embedding", 384, VectorUseCase::SemanticSearch).await.unwrap();
        assert!(engine.has_vector_columns("products"));

        // Drop index
        engine.drop_vector_index("products", "embedding").await.unwrap();
        assert!(!engine.has_vector_columns("products"));
    }

    #[test]
    fn test_vector_engine_stats() {
        let engine = VectorQueryEngine::new();
        let stats = engine.get_vector_stats();

        assert_eq!(stats.total_indexes, 0);
        assert_eq!(stats.total_stored_vectors, 0);
        assert_eq!(stats.index_stats.len(), 0);
        assert_eq!(stats.storage_stats.len(), 0);
    }

    #[test]
    fn test_vector_search_expression() {
        let search_expr = VectorSearchExpression {
            table_name: "products".to_string(),
            column_name: "embedding".to_string(),
            query_vector: vec![1.0, 2.0, 3.0],
            limit: 10,
            distance_metric: super::distance_metrics::DistanceMetric::Cosine,
        };

        assert_eq!(search_expr.table_name, "products");
        assert_eq!(search_expr.limit, 10);
        assert_eq!(search_expr.query_vector, vec![1.0, 2.0, 3.0]);
    }

    #[test]
    fn test_vector_distance_expression() {
        let distance_expr = VectorDistanceExpression {
            left: Box::new(Expression::Column("a.embedding".to_string())),
            right: Box::new(Expression::VectorLiteral(vec![1.0, 2.0, 3.0])),
            metric: super::distance_metrics::DistanceMetric::Euclidean,
        };

        match distance_expr.left.as_ref() {
            Expression::Column(col) => assert_eq!(col, "a.embedding"),
            _ => panic!("Expected column expression"),
        }

        assert_eq!(distance_expr.metric, super::distance_metrics::DistanceMetric::Euclidean);
    }
}
