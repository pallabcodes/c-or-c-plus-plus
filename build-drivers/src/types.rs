//! AuroraDB Type System
//!
//! Comprehensive type definitions for AuroraDB's advanced features including
//! vector search, analytics, streaming, and AI/ML capabilities.

use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::time::Duration;

/// AuroraDB value types
#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
pub enum AuroraValue {
    /// Null value
    Null,

    /// Boolean
    Bool(bool),

    /// 8-bit signed integer
    TinyInt(i8),

    /// 16-bit signed integer
    SmallInt(i16),

    /// 32-bit signed integer
    Int(i32),

    /// 64-bit signed integer
    BigInt(i64),

    /// 32-bit floating point
    Float(f32),

    /// 64-bit floating point
    Double(f64),

    /// Decimal with arbitrary precision
    Decimal(String), // Using string to preserve precision

    /// UTF-8 string
    Text(String),

    /// Binary data
    Binary(Vec<u8>),

    /// Date (days since Unix epoch)
    Date(i32),

    /// Time (microseconds since midnight)
    Time(i64),

    /// Timestamp (microseconds since Unix epoch)
    Timestamp(i64),

    /// Timestamp with timezone
    TimestampTz(i64, String),

    /// JSON value
    Json(serde_json::Value),

    /// Vector embedding (for AI/ML)
    Vector(Vec<f32>),

    /// UUID
    Uuid(String),

    /// Array of values
    Array(Vec<AuroraValue>),

    /// Map/dictionary
    Map(HashMap<String, AuroraValue>),
}

/// AuroraDB column types
#[derive(Debug, Clone, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub enum AuroraType {
    /// Null
    Null,

    /// Boolean
    Bool,

    /// Integer types
    TinyInt,
    SmallInt,
    Int,
    BigInt,

    /// Floating point types
    Float,
    Double,

    /// Decimal
    Decimal(u8, u8), // precision, scale

    /// String types
    Char(u32),       // length
    Varchar(u32),    // max length
    Text,

    /// Binary types
    Binary(u32),     // length
    Varbinary(u32),  // max length
    Blob,

    /// Date/Time types
    Date,
    Time,
    Timestamp,
    TimestampTz,

    /// Advanced types
    Json,
    Vector(u32),     // dimensions
    Uuid,

    /// Collection types
    Array(Box<AuroraType>),
    Map(Box<AuroraType>, Box<AuroraType>),
}

/// Database column definition
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AuroraColumn {
    /// Column name
    pub name: String,

    /// Column type
    pub column_type: AuroraType,

    /// Nullable
    pub nullable: bool,

    /// Default value
    pub default_value: Option<AuroraValue>,

    /// Primary key
    pub primary_key: bool,

    /// Auto increment
    pub auto_increment: bool,

    /// Column comment
    pub comment: Option<String>,
}

/// Database row
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AuroraRow {
    /// Column values in order
    pub values: Vec<AuroraValue>,

    /// Column names (optional, for convenience)
    pub columns: Option<Vec<String>>,
}

/// Query result
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct QueryResult {
    /// Result rows
    pub rows: Vec<AuroraRow>,

    /// Column definitions
    pub columns: Vec<AuroraColumn>,

    /// Number of rows returned
    pub row_count: usize,

    /// Query execution time
    pub execution_time_ms: f64,

    /// Query ID for tracing
    pub query_id: String,
}

/// Execute result (for INSERT, UPDATE, DELETE)
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ExecuteResult {
    /// Number of rows affected
    pub rows_affected: u64,

    /// Last inserted ID (for auto-increment)
    pub last_insert_id: Option<u64>,

    /// Execution time
    pub execution_time_ms: f64,

    /// Statement ID for tracing
    pub statement_id: String,
}

/// Vector search request
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct VectorSearchRequest {
    /// Collection/table name
    pub collection: String,

    /// Query vector
    pub query_vector: Vec<f32>,

    /// Maximum number of results
    pub limit: usize,

    /// Search filters
    pub filters: Option<HashMap<String, FilterCondition>>,

    /// Use ML reranking
    pub rerank: bool,

    /// Return similarity explanations
    pub explain: bool,

    /// Search timeout
    pub timeout: Option<Duration>,
}

/// Filter conditions for vector search
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum FilterCondition {
    /// Equal
    Eq(AuroraValue),

    /// Not equal
    Ne(AuroraValue),

    /// Less than
    Lt(AuroraValue),

    /// Less than or equal
    Le(AuroraValue),

    /// Greater than
    Gt(AuroraValue),

    /// Greater than or equal
    Ge(AuroraValue),

    /// In array
    In(Vec<AuroraValue>),

    /// Range (min, max)
    Range(AuroraValue, AuroraValue),

    /// Text search
    Text(String),

    /// Custom filter expression
    Custom(String),
}

/// Vector search result
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct VectorSearchResult {
    /// Search results with similarity scores
    pub results: Vec<VectorSearchMatch>,

    /// Search execution time
    pub search_time_ms: f64,

    /// Total candidates evaluated
    pub total_candidates: usize,

    /// Search metadata
    pub metadata: VectorSearchMetadata,
}

/// Individual vector search match
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct VectorSearchMatch {
    /// Row data
    pub row: AuroraRow,

    /// Similarity score (0.0 to 1.0)
    pub score: f32,

    /// Similarity explanation (if requested)
    pub explanation: Option<VectorExplanation>,

    /// Distance metric used
    pub distance_metric: DistanceMetric,
}

/// Distance metrics for vector similarity
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum DistanceMetric {
    /// Cosine similarity
    Cosine,

    /// Euclidean distance
    Euclidean,

    /// Dot product
    DotProduct,

    /// Manhattan distance
    Manhattan,
}

/// Vector similarity explanation
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct VectorExplanation {
    /// Similarity score breakdown
    pub score_breakdown: HashMap<String, f32>,

    /// Contributing factors
    pub factors: Vec<String>,

    /// Query vector used
    pub query_vector: Vec<f32>,
}

/// Vector search metadata
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct VectorSearchMetadata {
    /// Index used for search
    pub index_used: String,

    /// Number of vectors scanned
    pub vectors_scanned: usize,

    /// Search algorithm used
    pub algorithm: String,

    /// Performance metrics
    pub performance: VectorPerformance,
}

/// Vector search performance metrics
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct VectorPerformance {
    /// Index lookup time
    pub index_lookup_ms: f64,

    /// Vector computation time
    pub vector_computation_ms: f64,

    /// Filtering time
    pub filtering_ms: f64,

    /// Total search time
    pub total_ms: f64,
}

/// Analytics request
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AnalyticsRequest {
    /// Analytics SQL query
    pub sql: String,

    /// Query parameters
    pub params: Vec<AuroraValue>,

    /// Query timeout
    pub timeout: Option<Duration>,
}

/// Analytics result
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AnalyticsResult {
    /// Analytics data
    pub data: Vec<HashMap<String, AuroraValue>>,

    /// Result metadata
    pub metadata: AnalyticsMetadata,

    /// Execution time
    pub execution_time_ms: f64,

    /// Query ID
    pub query_id: String,
}

/// Analytics metadata
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AnalyticsMetadata {
    /// Number of rows processed
    pub rows_processed: usize,

    /// Data sources queried
    pub data_sources: Vec<String>,

    /// Query execution plan
    pub execution_plan: Option<String>,

    /// Cache hit ratio
    pub cache_hit_ratio: Option<f64>,
}

/// Stream request for real-time data
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StreamRequest {
    /// Stream SQL query
    pub sql: String,

    /// Stream parameters
    pub params: Vec<AuroraValue>,

    /// Stream window specification
    pub window: Option<WindowSpec>,

    /// Stream timeout
    pub timeout: Option<Duration>,
}

/// Window specification for streaming
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct WindowSpec {
    /// Window type
    pub window_type: WindowType,

    /// Window size
    pub size: WindowSize,

    /// Slide interval (for sliding windows)
    pub slide: Option<WindowSize>,
}

/// Window types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum WindowType {
    /// Tumbling window
    Tumbling,

    /// Sliding window
    Sliding,

    /// Session window
    Session,
}

/// Window size specification
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum WindowSize {
    /// Time-based window
    Time(Duration),

    /// Row-based window
    Rows(usize),
}

/// Database schema information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SchemaInfo {
    /// Database name
    pub database_name: String,

    /// Tables in the database
    pub tables: Vec<TableInfo>,

    /// Indexes
    pub indexes: Vec<IndexInfo>,

    /// Constraints
    pub constraints: Vec<ConstraintInfo>,
}

/// Table information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TableInfo {
    /// Table name
    pub name: String,

    /// Table type
    pub table_type: TableType,

    /// Columns
    pub columns: Vec<AuroraColumn>,

    /// Primary key columns
    pub primary_key: Vec<String>,

    /// Row count estimate
    pub row_count: Option<u64>,

    /// Table size estimate
    pub size_bytes: Option<u64>,
}

/// Table types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum TableType {
    /// Regular table
    Table,

    /// View
    View,

    /// System table
    System,

    /// Temporary table
    Temporary,
}

/// Index information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct IndexInfo {
    /// Index name
    pub name: String,

    /// Table name
    pub table_name: String,

    /// Index type
    pub index_type: IndexType,

    /// Indexed columns
    pub columns: Vec<String>,

    /// Unique index
    pub unique: bool,

    /// Index size
    pub size_bytes: Option<u64>,
}

/// Index types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum IndexType {
    /// B-tree index
    BTree,

    /// Hash index
    Hash,

    /// Vector index
    Vector,

    /// Full-text search index
    FullText,

    /// Spatial index
    Spatial,
}

/// Constraint information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ConstraintInfo {
    /// Constraint name
    pub name: String,

    /// Table name
    pub table_name: String,

    /// Constraint type
    pub constraint_type: ConstraintType,

    /// Affected columns
    pub columns: Vec<String>,

    /// Referenced table (for foreign keys)
    pub referenced_table: Option<String>,

    /// Referenced columns (for foreign keys)
    pub referenced_columns: Option<Vec<String>>,
}

/// Constraint types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ConstraintType {
    /// Primary key
    PrimaryKey,

    /// Unique constraint
    Unique,

    /// Foreign key
    ForeignKey,

    /// Check constraint
    Check,

    /// Not null constraint
    NotNull,
}

/// Health status
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct HealthStatus {
    /// Overall health state
    pub state: HealthState,

    /// Health message
    pub message: String,

    /// Detailed health checks
    pub details: HashMap<String, HealthCheck>,
}

/// Health states
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum HealthState {
    /// System is healthy
    Healthy,

    /// System is degraded but functional
    Degraded,

    /// System is unhealthy
    Unhealthy,
}

/// Individual health check
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct HealthCheck {
    /// Check name
    pub name: String,

    /// Check status
    pub status: HealthState,

    /// Check message
    pub message: String,

    /// Check duration
    pub duration_ms: f64,

    /// Additional metadata
    pub metadata: HashMap<String, String>,
}

// Query/Execute request types (used by protocol)
#[derive(Debug, Serialize, Deserialize)]
pub struct QueryRequest {
    pub sql: String,
    pub params: Vec<AuroraValue>,
    pub timeout: Option<Duration>,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct ExecuteRequest {
    pub sql: String,
    pub params: Vec<AuroraValue>,
    pub timeout: Option<Duration>,
}

// UNIQUENESS Validation:
// - [x] Comprehensive type system covering all AuroraDB features
// - [x] Vector search types with advanced filtering
// - [x] Analytics types with metadata and performance info
// - [x] Streaming types with window specifications
// - [x] Schema introspection types
// - [x] Health monitoring types
// - [x] Serialization support for all types
