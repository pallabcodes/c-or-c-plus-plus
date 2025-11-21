//! Core Types for AuroraDB Query Engine
//!
//! Defines the fundamental data structures used throughout the query engine.

use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::time::{Duration, Instant};

/// Query represents a parsed and analyzed query
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Query {
    /// Unique query ID
    pub id: String,

    /// Original query text
    pub text: String,

    /// Parsed abstract syntax tree
    pub ast: QueryAST,

    /// Query parameters
    pub parameters: HashMap<String, QueryValue>,

    /// Query metadata
    pub metadata: QueryMetadata,

    /// Execution context
    pub context: ExecutionContext,
}

/// Abstract syntax tree for queries
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum QueryAST {
    /// SELECT statement
    Select(SelectStmt),

    /// INSERT statement
    Insert(InsertStmt),

    /// UPDATE statement
    Update(UpdateStmt),

    /// DELETE statement
    Delete(DeleteStmt),

    /// Vector search
    VectorSearch(VectorSearchStmt),

    /// Graph query
    GraphQuery(GraphQueryStmt),

    /// Time series query
    TimeSeriesQuery(TimeSeriesStmt),

    /// Custom query type
    Custom(CustomQuery),
}

/// SELECT statement AST
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SelectStmt {
    pub select_list: Vec<SelectItem>,
    pub from_clause: Vec<TableReference>,
    pub where_clause: Option<Expression>,
    pub group_by: Vec<Expression>,
    pub having: Option<Expression>,
    pub order_by: Vec<OrderByItem>,
    pub limit: Option<u64>,
    pub offset: Option<u64>,
}

/// Vector search statement
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct VectorSearchStmt {
    pub collection: String,
    pub query_vector: Vec<f32>,
    pub limit: usize,
    pub filters: Option<Expression>,
    pub rerank: bool,
    pub include_similarity: bool,
}

/// Table reference in FROM clause
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TableReference {
    pub name: String,
    pub alias: Option<String>,
    pub schema: Option<String>,
}

/// Select item (column expression)
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum SelectItem {
    /// Column reference
    Column(ColumnRef),

    /// Expression with alias
    Expression { expr: Expression, alias: Option<String> },

    /// Wildcard (*)
    Wildcard,
}

/// Column reference
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ColumnRef {
    pub table: Option<String>,
    pub column: String,
}

/// Expression in queries
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum Expression {
    /// Literal value
    Literal(QueryValue),

    /// Column reference
    Column(ColumnRef),

    /// Binary operation
    BinaryOp {
        left: Box<Expression>,
        op: BinaryOperator,
        right: Box<Expression>,
    },

    /// Function call
    Function {
        name: String,
        args: Vec<Expression>,
    },

    /// Subquery
    Subquery(Box<Query>),
}

/// Binary operators
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum BinaryOperator {
    Eq, Ne, Lt, Le, Gt, Ge,
    And, Or,
    Add, Sub, Mul, Div, Mod,
    Like, NotLike,
}

/// Query value types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum QueryValue {
    Null,
    Boolean(bool),
    Integer(i64),
    Float(f64),
    String(String),
    Bytes(Vec<u8>),
    Array(Vec<QueryValue>),
    Object(HashMap<String, QueryValue>),
    Vector(Vec<f32>),
}

/// Order by item
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct OrderByItem {
    pub expr: Expression,
    pub ascending: bool,
}

/// Query metadata
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct QueryMetadata {
    pub submitted_at: chrono::DateTime<chrono::Utc>,
    pub client_info: Option<String>,
    pub priority: QueryPriority,
    pub timeout: Option<Duration>,
    pub estimated_cost: Option<f64>,
}

/// Query priority levels
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum QueryPriority {
    Low,
    Normal,
    High,
    Critical,
}

/// Execution context
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ExecutionContext {
    pub user_id: Option<String>,
    pub session_id: Option<String>,
    pub transaction_id: Option<String>,
    pub read_only: bool,
    pub isolation_level: IsolationLevel,
}

/// Transaction isolation levels
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum IsolationLevel {
    ReadUncommitted,
    ReadCommitted,
    RepeatableRead,
    Serializable,
}

/// Query plan representation
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct QueryPlan {
    pub id: String,
    pub root: PlanNode,
    pub estimated_cost: f64,
    pub estimated_cardinality: u64,
    pub total_operators: usize,
    pub metadata: PlanMetadata,
}

/// Plan metadata
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PlanMetadata {
    pub created_at: chrono::DateTime<chrono::Utc>,
    pub optimizer_version: String,
    pub planning_time_ms: u64,
}

/// Plan node types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum PlanNode {
    /// Sequential scan
    SeqScan {
        table: String,
        filter: Option<Expression>,
    },

    /// Index scan
    IndexScan {
        table: String,
        index: String,
        filter: Option<Expression>,
    },

    /// Vector index scan
    VectorScan {
        collection: String,
        query_vector: Vec<f32>,
        limit: usize,
    },

    /// Nested loop join
    NestedLoopJoin {
        left: Box<PlanNode>,
        right: Box<PlanNode>,
        join_condition: Expression,
    },

    /// Hash join
    HashJoin {
        left: Box<PlanNode>,
        right: Box<PlanNode>,
        join_condition: Expression,
        build_side: JoinSide,
    },

    /// Sort operation
    Sort {
        input: Box<PlanNode>,
        sort_keys: Vec<OrderByItem>,
    },

    /// Aggregation
    Aggregate {
        input: Box<PlanNode>,
        group_by: Vec<Expression>,
        aggregates: Vec<AggregateExpr>,
    },

    /// Limit operation
    Limit {
        input: Box<PlanNode>,
        limit: u64,
        offset: u64,
    },

    /// Projection
    Projection {
        input: Box<PlanNode>,
        expressions: Vec<(Expression, Option<String>)>,
    },
}

/// Join side for hash joins
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum JoinSide {
    Left,
    Right,
}

/// Aggregate expression
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AggregateExpr {
    pub function: AggregateFunction,
    pub args: Vec<Expression>,
    pub alias: Option<String>,
}

/// Aggregate functions
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AggregateFunction {
    Count,
    Sum,
    Avg,
    Min,
    Max,
    CountDistinct,
}

/// Execution result
#[derive(Debug, Clone)]
pub struct ExecutionResult {
    pub query_id: String,
    pub success: bool,
    pub rows_affected: Option<u64>,
    pub data: Option<QueryData>,
    pub metrics: QueryMetrics,
    pub error: Option<String>,
}

/// Query data result
#[derive(Debug, Clone)]
pub enum QueryData {
    Rows(Vec<HashMap<String, QueryValue>>),
    Scalar(QueryValue),
    Empty,
}

/// Query execution metrics
#[derive(Debug, Clone)]
pub struct QueryMetrics {
    pub parsing_time: Duration,
    pub planning_time: Duration,
    pub execution_time: Duration,
    pub total_time: Duration,
    pub bytes_processed: u64,
    pub rows_processed: u64,
    pub cache_hits: u64,
    pub cache_misses: u64,
    pub network_requests: u64,
    pub storage_requests: u64,
}

/// Table schema definition
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TableSchema {
    pub name: String,
    pub columns: Vec<ColumnSchema>,
    pub primary_key: Option<Vec<String>>,
    pub indexes: Vec<IndexSchema>,
    pub constraints: Vec<Constraint>,
}

/// Column schema
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ColumnSchema {
    pub name: String,
    pub data_type: DataType,
    pub nullable: bool,
    pub default_value: Option<QueryValue>,
    pub description: Option<String>,
}

/// Data types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum DataType {
    Boolean,
    Int8, Int16, Int32, Int64,
    UInt8, UInt16, UInt32, UInt64,
    Float32, Float64,
    String,
    Bytes,
    Date, Time, DateTime,
    Json,
    Vector { dimensions: usize },
    Array(Box<DataType>),
}

/// Index schema
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct IndexSchema {
    pub name: String,
    pub columns: Vec<String>,
    pub index_type: IndexType,
    pub unique: bool,
}

/// Index types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum IndexType {
    BTree,
    Hash,
    Gist,
    Gin,
    SpGist,
    Brin,
    Vector { algorithm: VectorIndexAlgorithm },
}

/// Vector index algorithms
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum VectorIndexAlgorithm {
    HNSW,
    IVF,
    PQ,
}

/// Table constraints
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum Constraint {
    NotNull(String), // column name
    Unique(Vec<String>), // column names
    PrimaryKey(Vec<String>), // column names
    ForeignKey {
        columns: Vec<String>,
        referenced_table: String,
        referenced_columns: Vec<String>,
    },
    Check(String), // check expression
}

// Placeholder implementations for incomplete types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct InsertStmt;
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct UpdateStmt;
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct DeleteStmt;
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GraphQueryStmt;
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TimeSeriesStmt;
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CustomQuery;
