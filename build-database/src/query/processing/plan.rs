//! Query Execution Plans for AuroraDB
//!
//! UNIQUENESS: Intelligent query plans with cost-based optimization,
//! adaptive execution, and research-backed algorithms for optimal performance.

use std::collections::{HashMap, HashSet};
use crate::core::errors::{AuroraResult, AuroraError};
use super::ast::*;

/// Complete query execution plan
#[derive(Debug, Clone)]
pub struct QueryPlan {
    pub root: PlanNode,
    pub estimated_cost: f64,
    pub estimated_rows: u64,
    pub execution_mode: ExecutionMode,
    pub optimization_hints: Vec<OptimizationHint>,
    pub statistics: PlanStatistics,
}

/// Execution modes for different query patterns
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum ExecutionMode {
    /// Sequential execution (single-threaded)
    Sequential,
    /// Parallel execution across cores
    Parallel,
    /// Vectorized execution with SIMD
    Vectorized,
    /// Adaptive execution with runtime optimization
    Adaptive,
    /// Streaming execution for large datasets
    Streaming,
}

/// Optimization hints for query execution
#[derive(Debug, Clone)]
pub enum OptimizationHint {
    /// Use specific index
    UseIndex(String),
    /// Prefer nested loop join
    PreferNestedLoop,
    /// Prefer hash join
    PreferHashJoin,
    /// Prefer merge join
    PreferMergeJoin,
    /// Force materialization
    ForceMaterialization,
    /// Use bitmap index
    UseBitmapIndex,
    /// Parallel execution hint
    ParallelExecution(u32), // number of threads
}

/// Plan execution statistics
#[derive(Debug, Clone)]
pub struct PlanStatistics {
    pub total_operators: u32,
    pub estimated_memory_mb: f64,
    pub estimated_cpu_cost: f64,
    pub estimated_io_cost: f64,
    pub selectivity_factors: HashMap<String, f64>,
}

/// Plan nodes representing different operations
#[derive(Debug, Clone)]
pub enum PlanNode {
    /// Sequential scan of a table
    SeqScan(SeqScanNode),
    /// Index scan using an index
    IndexScan(IndexScanNode),
    /// Bitmap index scan
    BitmapScan(BitmapScanNode),
    /// Filter rows based on condition
    Filter(FilterNode),
    /// Project columns
    Projection(ProjectionNode),
    /// Sort rows
    Sort(SortNode),
    /// Limit number of rows
    Limit(LimitNode),
    /// Aggregate operations (GROUP BY)
    Aggregate(AggregateNode),
    /// Join operations
    Join(JoinNode),
    /// Nested loop join
    NestedLoopJoin(NestedLoopJoinNode),
    /// Hash join
    HashJoin(HashJoinNode),
    /// Merge join
    MergeJoin(MergeJoinNode),
    /// Union operation
    Union(UnionNode),
    /// Insert operation
    Insert(InsertNode),
    /// Update operation
    Update(UpdateNode),
    /// Delete operation
    Delete(DeleteNode),
    /// Create table operation
    CreateTable(CreateTableNode),
    /// AuroraDB UNIQUENESS: Vector search
    VectorSearch(VectorSearchNode),
    /// AuroraDB UNIQUENESS: KNN search
    KnnSearch(KnnSearchNode),
    /// AuroraDB UNIQUENESS: Graph traversal
    GraphTraversal(GraphTraversalNode),
}

/// Sequential scan node
#[derive(Debug, Clone)]
pub struct SeqScanNode {
    pub table_name: String,
    pub output_columns: Vec<String>,
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Index scan node
#[derive(Debug, Clone)]
pub struct IndexScanNode {
    pub table_name: String,
    pub index_name: String,
    pub index_condition: Expression,
    pub output_columns: Vec<String>,
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Bitmap scan node
#[derive(Debug, Clone)]
pub struct BitmapScanNode {
    pub table_name: String,
    pub bitmap_indexes: Vec<String>,
    pub conditions: Vec<Expression>,
    pub output_columns: Vec<String>,
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Filter node
#[derive(Debug, Clone)]
pub struct FilterNode {
    pub input: Box<PlanNode>,
    pub condition: Expression,
    pub estimated_rows: u64,
    pub selectivity: f64,
    pub cost: f64,
}

/// Projection node
#[derive(Debug, Clone)]
pub struct ProjectionNode {
    pub input: Box<PlanNode>,
    pub expressions: Vec<(Expression, Option<String>)>, // expression, alias
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Sort node
#[derive(Debug, Clone)]
pub struct SortNode {
    pub input: Box<PlanNode>,
    pub sort_keys: Vec<OrderByItem>,
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Limit node
#[derive(Debug, Clone)]
pub struct LimitNode {
    pub input: Box<PlanNode>,
    pub limit: u64,
    pub offset: u64,
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Aggregate node
#[derive(Debug, Clone)]
pub struct AggregateNode {
    pub input: Box<PlanNode>,
    pub group_by: Vec<Expression>,
    pub aggregates: Vec<(AggregateFunction, Vec<Expression>, Option<String>)>, // func, args, alias
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Join node
#[derive(Debug, Clone)]
pub struct JoinNode {
    pub left: Box<PlanNode>,
    pub right: Box<PlanNode>,
    pub join_type: JoinType,
    pub condition: Option<Expression>,
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Nested loop join node
#[derive(Debug, Clone)]
pub struct NestedLoopJoinNode {
    pub left: Box<PlanNode>,
    pub right: Box<PlanNode>,
    pub join_type: JoinType,
    pub condition: Option<Expression>,
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Hash join node
#[derive(Debug, Clone)]
pub struct HashJoinNode {
    pub left: Box<PlanNode>,
    pub right: Box<PlanNode>,
    pub join_type: JoinType,
    pub condition: Option<Expression>,
    pub hash_keys_left: Vec<Expression>,
    pub hash_keys_right: Vec<Expression>,
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Merge join node
#[derive(Debug, Clone)]
pub struct MergeJoinNode {
    pub left: Box<PlanNode>,
    pub right: Box<PlanNode>,
    pub join_type: JoinType,
    pub condition: Option<Expression>,
    pub sort_keys_left: Vec<OrderByItem>,
    pub sort_keys_right: Vec<OrderByItem>,
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Union node
#[derive(Debug, Clone)]
pub struct UnionNode {
    pub left: Box<PlanNode>,
    pub right: Box<PlanNode>,
    pub all: bool, // UNION ALL vs UNION
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Insert node
#[derive(Debug, Clone)]
pub struct InsertNode {
    pub table_name: String,
    pub columns: Vec<String>,
    pub values: Vec<Vec<Expression>>,
    pub select: Option<Box<PlanNode>>,
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Update node
#[derive(Debug, Clone)]
pub struct UpdateNode {
    pub table_name: String,
    pub assignments: Vec<(String, Expression)>,
    pub condition: Option<Expression>,
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Delete node
#[derive(Debug, Clone)]
pub struct DeleteNode {
    pub table_name: String,
    pub condition: Option<Expression>,
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Create table node
#[derive(Debug, Clone)]
pub struct CreateTableNode {
    pub table_name: String,
    pub columns: Vec<ColumnDefinition>,
    pub constraints: Vec<TableConstraint>,
    pub cost: f64,
}

/// Vector search node for AuroraDB UNIQUENESS
#[derive(Debug, Clone)]
pub struct VectorSearchNode {
    pub table_name: String,
    pub vector_column: String,
    pub query_vector: Vec<f32>,
    pub metric: VectorMetric,
    pub limit: u32,
    pub filter_condition: Option<Expression>,
    pub index_name: Option<String>, // HNSW, IVF, etc.
    pub estimated_rows: u64,
    pub cost: f64,
}

/// KNN search node
#[derive(Debug, Clone)]
pub struct KnnSearchNode {
    pub table_name: String,
    pub vector_column: String,
    pub query_vector: Vec<f32>,
    pub k: u32, // number of nearest neighbors
    pub metric: VectorMetric,
    pub filter_condition: Option<Expression>,
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Graph traversal node for AuroraDB UNIQUENESS
#[derive(Debug, Clone)]
pub struct GraphTraversalNode {
    pub graph_name: String,
    pub start_node: Expression,
    pub traversal_type: TraversalType,
    pub max_depth: Option<u32>,
    pub filter_condition: Option<Expression>,
    pub estimated_rows: u64,
    pub cost: f64,
}

/// Traversal types for graph queries
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum TraversalType {
    BreadthFirst,
    DepthFirst,
    ShortestPath,
    AllPaths,
    PageRank,
}

/// Cost estimation for query plans
#[derive(Debug, Clone)]
pub struct CostEstimate {
    pub cpu_cost: f64,
    pub io_cost: f64,
    pub memory_cost: f64,
    pub network_cost: f64,
    pub total_cost: f64,
    pub estimated_rows: u64,
    pub estimated_width: u32, // average row width in bytes
}

/// Cardinality estimation
#[derive(Debug, Clone)]
pub struct CardinalityEstimate {
    pub table_name: String,
    pub total_rows: u64,
    pub filtered_rows: u64,
    pub selectivity: f64,
    pub confidence: f64,
}

/// Plan visitor pattern for analysis and optimization
pub trait PlanVisitor<T> {
    fn visit_seq_scan(&mut self, node: &SeqScanNode) -> AuroraResult<T>;
    fn visit_index_scan(&mut self, node: &IndexScanNode) -> AuroraResult<T>;
    fn visit_filter(&mut self, node: &FilterNode) -> AuroraResult<T>;
    fn visit_projection(&mut self, node: &ProjectionNode) -> AuroraResult<T>;
    fn visit_join(&mut self, node: &JoinNode) -> AuroraResult<T>;
    fn visit_aggregate(&mut self, node: &AggregateNode) -> AuroraResult<T>;
    fn visit_sort(&mut self, node: &SortNode) -> AuroraResult<T>;
    fn visit_limit(&mut self, node: &LimitNode) -> AuroraResult<T>;
}

/// Plan analysis utilities
impl QueryPlan {
    /// Calculate total cost of the plan
    pub fn total_cost(&self) -> f64 {
        self.estimated_cost
    }

    /// Get execution complexity
    pub fn complexity(&self) -> PlanComplexity {
        match self.estimated_cost {
            c if c < 100.0 => PlanComplexity::Simple,
            c if c < 1000.0 => PlanComplexity::Medium,
            c if c < 10000.0 => PlanComplexity::Complex,
            _ => PlanComplexity::VeryComplex,
        }
    }

    /// Check if plan is cacheable
    pub fn is_cacheable(&self) -> bool {
        // Plans with parameters or volatile functions are not cacheable
        !self.contains_parameters() && !self.contains_volatile_functions()
    }

    /// Check if plan contains parameters
    pub fn contains_parameters(&self) -> bool {
        self.has_parameters(&self.root)
    }

    /// Check if plan contains volatile functions
    pub fn contains_volatile_functions(&self) -> bool {
        self.has_volatile_functions(&self.root)
    }

    fn has_parameters(&self, node: &PlanNode) -> bool {
        match node {
            PlanNode::Filter(filter) => self.expr_has_parameters(&filter.condition),
            PlanNode::Projection(proj) => proj.expressions.iter().any(|(expr, _)| self.expr_has_parameters(expr)),
            PlanNode::Join(join) => {
                self.has_parameters(&join.left) || self.has_parameters(&join.right) ||
                join.condition.as_ref().map_or(false, |c| self.expr_has_parameters(c))
            }
            PlanNode::NestedLoopJoin(join) => {
                self.has_parameters(&join.left) || self.has_parameters(&join.right) ||
                join.condition.as_ref().map_or(false, |c| self.expr_has_parameters(c))
            }
            PlanNode::HashJoin(join) => {
                self.has_parameters(&join.left) || self.has_parameters(&join.right) ||
                join.condition.as_ref().map_or(false, |c| self.expr_has_parameters(c))
            }
            PlanNode::MergeJoin(join) => {
                self.has_parameters(&join.left) || self.has_parameters(&join.right) ||
                join.condition.as_ref().map_or(false, |c| self.expr_has_parameters(c))
            }
            _ => false, // Other nodes don't directly contain expressions
        }
    }

    fn has_volatile_functions(&self, node: &PlanNode) -> bool {
        match node {
            PlanNode::Filter(filter) => self.expr_has_volatile_functions(&filter.condition),
            PlanNode::Projection(proj) => proj.expressions.iter().any(|(expr, _)| self.expr_has_volatile_functions(expr)),
            PlanNode::Join(join) => {
                self.has_volatile_functions(&join.left) || self.has_volatile_functions(&join.right) ||
                join.condition.as_ref().map_or(false, |c| self.expr_has_volatile_functions(c))
            }
            _ => false,
        }
    }

    fn expr_has_parameters(&self, expr: &Expression) -> bool {
        match expr {
            Expression::Literal(_) => false,
            Expression::Column(_) => false,
            Expression::BinaryOp { left, right, .. } => {
                self.expr_has_parameters(left) || self.expr_has_parameters(right)
            }
            Expression::Function { args, .. } => {
                args.iter().any(|arg| self.expr_has_parameters(arg))
            }
            // Parameters would be represented as special expressions
            _ => false,
        }
    }

    fn expr_has_volatile_functions(&self, expr: &Expression) -> bool {
        match expr {
            Expression::Function { name, .. } => {
                matches!(name.to_uppercase().as_str(), "NOW" | "RANDOM" | "UUID_GENERATE_V4")
            }
            Expression::BinaryOp { left, right, .. } => {
                self.expr_has_volatile_functions(left) || self.expr_has_volatile_functions(right)
            }
            Expression::Function { args, .. } => {
                args.iter().any(|arg| self.expr_has_volatile_functions(arg))
            }
            _ => false,
        }
    }
}

/// Plan complexity levels
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub enum PlanComplexity {
    Simple,
    Medium,
    Complex,
    VeryComplex,
}

/// Cost model for AuroraDB
#[derive(Debug)]
pub struct CostModel {
    /// CPU cost per operation
    pub cpu_operation_cost: f64,
    /// IO cost per page
    pub io_page_cost: f64,
    /// Memory cost per byte
    pub memory_byte_cost: f64,
    /// Network cost per byte
    pub network_byte_cost: f64,
    /// Index lookup cost
    pub index_lookup_cost: f64,
    /// Sequential scan cost per row
    pub seq_scan_row_cost: f64,
    /// Join cost factors
    pub join_cost_factors: HashMap<String, f64>,
}

impl CostModel {
    /// Create default cost model
    pub fn new() -> Self {
        Self {
            cpu_operation_cost: 0.01,
            io_page_cost: 1.0,
            memory_byte_cost: 0.0001,
            network_byte_cost: 0.001,
            index_lookup_cost: 0.1,
            seq_scan_row_cost: 0.001,
            join_cost_factors: HashMap::from([
                ("nested_loop".to_string(), 1.0),
                ("hash_join".to_string(), 0.8),
                ("merge_join".to_string(), 0.6),
            ]),
        }
    }

    /// Estimate cost for sequential scan
    pub fn estimate_seq_scan_cost(&self, table_rows: u64, row_width: u32, pages: u64) -> CostEstimate {
        let io_cost = pages as f64 * self.io_page_cost;
        let cpu_cost = table_rows as f64 * self.seq_scan_row_cost;
        let memory_cost = table_rows as f64 * row_width as f64 * self.memory_byte_cost;

        CostEstimate {
            cpu_cost,
            io_cost,
            memory_cost,
            network_cost: 0.0,
            total_cost: io_cost + cpu_cost + memory_cost,
            estimated_rows: table_rows,
            estimated_width: row_width,
        }
    }

    /// Estimate cost for index scan
    pub fn estimate_index_scan_cost(&self, index_pages: u64, table_pages: u64, matching_rows: u64) -> CostEstimate {
        let index_io_cost = index_pages as f64 * self.io_page_cost;
        let table_io_cost = table_pages as f64 * self.io_page_cost;
        let cpu_cost = matching_rows as f64 * self.cpu_operation_cost;

        CostEstimate {
            cpu_cost,
            io_cost: index_io_cost + table_io_cost,
            memory_cost: 0.0,
            network_cost: 0.0,
            total_cost: index_io_cost + table_io_cost + cpu_cost,
            estimated_rows: matching_rows,
            estimated_width: 256, // estimated
        }
    }

    /// Estimate cost for join operations
    pub fn estimate_join_cost(&self, join_type: &str, left_rows: u64, right_rows: u64, result_rows: u64) -> CostEstimate {
        let factor = self.join_cost_factors.get(join_type).unwrap_or(&1.0);
        let base_cost = (left_rows + right_rows) as f64 * self.cpu_operation_cost * factor;

        CostEstimate {
            cpu_cost: base_cost,
            io_cost: 0.0, // Assume in-memory for now
            memory_cost: (left_rows + right_rows) as f64 * 100.0 * self.memory_byte_cost, // Rough estimate
            network_cost: 0.0,
            total_cost: base_cost,
            estimated_rows: result_rows,
            estimated_width: 256, // estimated
        }
    }
}

impl Default for PlanStatistics {
    fn default() -> Self {
        Self {
            total_operators: 0,
            estimated_memory_mb: 0.0,
            estimated_cpu_cost: 0.0,
            estimated_io_cost: 0.0,
            selectivity_factors: HashMap::new(),
        }
    }
}

impl std::fmt::Display for QueryPlan {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "Query Plan (cost: {:.2}, rows: {}, mode: {:?})",
               self.estimated_cost, self.estimated_rows, self.execution_mode)
    }
}

impl std::fmt::Display for PlanNode {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            PlanNode::SeqScan(node) => write!(f, "SeqScan({})", node.table_name),
            PlanNode::IndexScan(node) => write!(f, "IndexScan({}, {})", node.table_name, node.index_name),
            PlanNode::Filter(_) => write!(f, "Filter"),
            PlanNode::Projection(_) => write!(f, "Projection"),
            PlanNode::Join(_) => write!(f, "Join"),
            PlanNode::Aggregate(_) => write!(f, "Aggregate"),
            PlanNode::Sort(_) => write!(f, "Sort"),
            PlanNode::Limit(_) => write!(f, "Limit"),
            PlanNode::VectorSearch(_) => write!(f, "VectorSearch"),
            _ => write!(f, "PlanNode"),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_query_plan_creation() {
        let plan = QueryPlan {
            root: PlanNode::SeqScan(SeqScanNode {
                table_name: "users".to_string(),
                output_columns: vec!["id".to_string(), "name".to_string()],
                estimated_rows: 1000,
                cost: 10.0,
            }),
            estimated_cost: 10.0,
            estimated_rows: 1000,
            execution_mode: ExecutionMode::Sequential,
            optimization_hints: vec![],
            statistics: PlanStatistics::default(),
        };

        assert_eq!(plan.estimated_cost, 10.0);
        assert_eq!(plan.estimated_rows, 1000);
        assert_eq!(plan.execution_mode, ExecutionMode::Sequential);
    }

    #[test]
    fn test_plan_complexity() {
        let simple_plan = QueryPlan {
            root: PlanNode::SeqScan(SeqScanNode {
                table_name: "test".to_string(),
                output_columns: vec![],
                estimated_rows: 100,
                cost: 50.0,
            }),
            estimated_cost: 50.0,
            estimated_rows: 100,
            execution_mode: ExecutionMode::Sequential,
            optimization_hints: vec![],
            statistics: PlanStatistics::default(),
        };

        let complex_plan = QueryPlan {
            root: PlanNode::SeqScan(SeqScanNode {
                table_name: "test".to_string(),
                output_columns: vec![],
                estimated_rows: 100,
                cost: 5000.0,
            }),
            estimated_cost: 5000.0,
            estimated_rows: 100,
            execution_mode: ExecutionMode::Sequential,
            optimization_hints: vec![],
            statistics: PlanStatistics::default(),
        };

        assert_eq!(simple_plan.complexity(), PlanComplexity::Simple);
        assert_eq!(complex_plan.complexity(), PlanComplexity::Complex);
    }

    #[test]
    fn test_cost_model() {
        let model = CostModel::new();
        let estimate = model.estimate_seq_scan_cost(1000, 256, 10);

        assert!(estimate.total_cost > 0.0);
        assert_eq!(estimate.estimated_rows, 1000);
        assert_eq!(estimate.estimated_width, 256);
    }

    #[test]
    fn test_join_nodes() {
        let left = Box::new(PlanNode::SeqScan(SeqScanNode {
            table_name: "users".to_string(),
            output_columns: vec!["id".to_string()],
            estimated_rows: 100,
            cost: 5.0,
        }));

        let right = Box::new(PlanNode::SeqScan(SeqScanNode {
            table_name: "orders".to_string(),
            output_columns: vec!["user_id".to_string()],
            estimated_rows: 500,
            cost: 10.0,
        }));

        let hash_join = PlanNode::HashJoin(HashJoinNode {
            left: left.clone(),
            right: right.clone(),
            join_type: JoinType::Inner,
            condition: Some(Expression::BinaryOp {
                left: Box::new(Expression::Column("users.id".to_string())),
                op: BinaryOperator::Equal,
                right: Box::new(Expression::Column("orders.user_id".to_string())),
            }),
            hash_keys_left: vec![Expression::Column("id".to_string())],
            hash_keys_right: vec![Expression::Column("user_id".to_string())],
            estimated_rows: 450,
            cost: 25.0,
        });

        if let PlanNode::HashJoin(join) = hash_join {
            assert_eq!(join.join_type, JoinType::Inner);
            assert_eq!(join.estimated_rows, 450);
            assert_eq!(join.cost, 25.0);
        } else {
            panic!("Expected HashJoin node");
        }
    }

    #[test]
    fn test_vector_search_node() {
        let vector_search = PlanNode::VectorSearch(VectorSearchNode {
            table_name: "products".to_string(),
            vector_column: "embedding".to_string(),
            query_vector: vec![0.1, 0.2, 0.3],
            metric: VectorMetric::Cosine,
            limit: 10,
            filter_condition: Some(Expression::BinaryOp {
                left: Box::new(Expression::Column("category".to_string())),
                op: BinaryOperator::Equal,
                right: Box::new(Expression::Literal(LiteralValue::String("electronics".to_string()))),
            }),
            index_name: Some("hnsw_index".to_string()),
            estimated_rows: 10,
            cost: 15.0,
        });

        if let PlanNode::VectorSearch(node) = vector_search {
            assert_eq!(node.table_name, "products");
            assert_eq!(node.metric, VectorMetric::Cosine);
            assert_eq!(node.limit, 10);
            assert!(node.index_name.is_some());
        } else {
            panic!("Expected VectorSearch node");
        }
    }

    #[test]
    fn test_optimization_hints() {
        let hints = vec![
            OptimizationHint::UseIndex("users_pkey".to_string()),
            OptimizationHint::PreferHashJoin,
            OptimizationHint::ParallelExecution(4),
        ];

        assert_eq!(hints.len(), 3);

        match &hints[0] {
            OptimizationHint::UseIndex(index_name) => assert_eq!(index_name, "users_pkey"),
            _ => panic!("Expected UseIndex hint"),
        }

        match &hints[1] {
            OptimizationHint::PreferHashJoin => {},
            _ => panic!("Expected PreferHashJoin hint"),
        }

        match &hints[2] {
            OptimizationHint::ParallelExecution(threads) => assert_eq!(*threads, 4),
            _ => panic!("Expected ParallelExecution hint"),
        }
    }

    #[test]
    fn test_plan_cacheability() {
        let cacheable_plan = QueryPlan {
            root: PlanNode::SeqScan(SeqScanNode {
                table_name: "users".to_string(),
                output_columns: vec!["id".to_string()],
                estimated_rows: 100,
                cost: 10.0,
            }),
            estimated_cost: 10.0,
            estimated_rows: 100,
            execution_mode: ExecutionMode::Sequential,
            optimization_hints: vec![],
            statistics: PlanStatistics::default(),
        };

        // Plans without parameters or volatile functions should be cacheable
        assert!(cacheable_plan.is_cacheable());
    }

    #[test]
    fn test_execution_modes() {
        assert_eq!(ExecutionMode::Sequential, ExecutionMode::Sequential);
        assert_ne!(ExecutionMode::Parallel, ExecutionMode::Vectorized);
        assert_eq!(ExecutionMode::Adaptive, ExecutionMode::Adaptive);
    }

    #[test]
    fn test_traversal_types() {
        assert_eq!(TraversalType::BreadthFirst, TraversalType::BreadthFirst);
        assert_ne!(TraversalType::DepthFirst, TraversalType::ShortestPath);
        assert_eq!(TraversalType::PageRank, TraversalType::PageRank);
    }
}
