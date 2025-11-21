//! Abstract Syntax Tree Definitions
//!
//! Defines the AST structures for parsed SQL queries.
//! Supports traditional SQL + vector search + analytics extensions.

/// Parser result type
pub type ParseResult<T> = Result<T, ParseError>;

/// Parser specific errors with detailed context
#[derive(Debug, thiserror::Error)]
pub enum ParseError {
    #[error("Syntax error at position {position}: {message}")]
    SyntaxError { position: usize, message: String },

    #[error("Unexpected token '{found}' at position {position}, expected {expected}")]
    UnexpectedToken { found: String, expected: String, position: usize },

    #[error("Invalid identifier '{name}' at position {position}")]
    InvalidIdentifier { name: String, position: usize },

    #[error("Unsupported feature '{feature}' at position {position}")]
    UnsupportedFeature { feature: String, position: usize },

    #[error("Incomplete query at position {position}")]
    IncompleteQuery { position: usize },
}

/// Parsed query representation
#[derive(Debug, Clone)]
pub enum Query {
    Select(SelectQuery),
    Insert(InsertQuery),
    Update(UpdateQuery),
    Delete(DeleteQuery),
    VectorSearch(VectorQuery),
    CreateTable(CreateTableQuery),
    DropTable(DropTableQuery),
}

/// SELECT query with AI extensions
#[derive(Debug, Clone)]
pub struct SelectQuery {
    pub select_list: Vec<SelectItem>,
    pub from_clause: FromClause,
    pub where_clause: Option<Expression>,
    pub group_by: Option<GroupByClause>,
    pub having: Option<Expression>,
    pub order_by: Option<OrderByClause>,
    pub limit: Option<LimitClause>,
    pub vector_extensions: Option<VectorExtensions>,
}

/// Vector search specific extensions
#[derive(Debug, Clone)]
pub struct VectorExtensions {
    pub nearest_neighbors: Option<NearestNeighbors>,
    pub similarity_threshold: Option<f64>,
    pub vector_column: Option<String>,
    pub embedding_model: Option<String>,
}

/// Nearest neighbors search specification
#[derive(Debug, Clone)]
pub struct NearestNeighbors {
    pub k: usize,
    pub distance_metric: DistanceMetric,
    pub vector_expression: Expression,
}

/// Distance metrics for vector similarity
#[derive(Debug, Clone, PartialEq)]
pub enum DistanceMetric {
    Cosine,
    Euclidean,
    DotProduct,
    Manhattan,
}

/// Basic expression types
#[derive(Debug, Clone)]
pub enum Expression {
    Literal(Literal),
    Column(String),
    BinaryOp(BinaryOp),
    Function(FunctionCall),
    WindowFunction(WindowFunction),
    VectorLiteral(Vec<f64>),
    Asterisk,
}

/// Binary operations
#[derive(Debug, Clone)]
pub struct BinaryOp {
    pub left: Box<Expression>,
    pub operator: BinaryOperator,
    pub right: Box<Expression>,
}

/// Binary operators
#[derive(Debug, Clone)]
pub enum BinaryOperator {
    Equal,
    NotEqual,
    LessThan,
    GreaterThan,
    LessEqual,
    GreaterEqual,
    And,
    Or,
    Plus,
    Minus,
    Multiply,
    Divide,
}

/// Function calls
#[derive(Debug, Clone)]
pub struct FunctionCall {
    pub name: String,
    pub arguments: Vec<Expression>,
}

/// Window function specification
#[derive(Debug, Clone)]
pub struct WindowFunction {
    pub function: FunctionCall,
    pub partition_by: Vec<Expression>,
    pub order_by: Vec<OrderByItem>,
    pub frame_clause: Option<FrameClause>,
}

/// Window frame clause
#[derive(Debug, Clone)]
pub struct FrameClause {
    pub frame_type: FrameType,
    pub start_bound: FrameBound,
    pub end_bound: Option<FrameBound>,
}

/// Window frame types
#[derive(Debug, Clone)]
pub enum FrameType {
    Rows,
    Range,
}

/// Window frame bounds
#[derive(Debug, Clone)]
pub enum FrameBound {
    UnboundedPreceding,
    Preceding(u64),
    CurrentRow,
    Following(u64),
    UnboundedFollowing,
}

/// Literal values
#[derive(Debug, Clone)]
pub enum Literal {
    Integer(i64),
    Float(f64),
    String(String),
    Boolean(bool),
    Null,
}

/// Select list items
#[derive(Debug, Clone)]
pub enum SelectItem {
    Expression(Expression),
    Wildcard,
    Aliased { expression: Expression, alias: String },
}

/// FROM clause
#[derive(Debug, Clone)]
pub struct FromClause {
    pub table: String,
    pub alias: Option<String>,
    pub joins: Vec<JoinClause>,
}

/// Join clauses
#[derive(Debug, Clone)]
pub struct JoinClause {
    pub join_type: JoinType,
    pub table: String,
    pub alias: Option<String>,
    pub condition: Expression,
}

/// Join types
#[derive(Debug, Clone)]
pub enum JoinType {
    Inner,
    Left,
    Right,
    Full,
}

/// GROUP BY clause
#[derive(Debug, Clone)]
pub struct GroupByClause {
    pub expressions: Vec<Expression>,
}

/// ORDER BY clause
#[derive(Debug, Clone)]
pub struct OrderByClause {
    pub items: Vec<OrderByItem>,
}

/// ORDER BY item
#[derive(Debug, Clone)]
pub struct OrderByItem {
    pub expression: Expression,
    pub direction: SortDirection,
}

/// Sort direction
#[derive(Debug, Clone)]
pub enum SortDirection {
    Ascending,
    Descending,
}

/// LIMIT clause
#[derive(Debug, Clone)]
pub struct LimitClause {
    pub limit: usize,
    pub offset: Option<usize>,
}

/// INSERT query
#[derive(Debug, Clone)]
pub struct InsertQuery {
    pub table: String,
    pub columns: Vec<String>,
    pub values: Vec<Vec<Expression>>,
}

/// UPDATE query
#[derive(Debug, Clone)]
pub struct UpdateQuery {
    pub table: String,
    pub assignments: Vec<Assignment>,
    pub where_clause: Option<Expression>,
}

/// UPDATE assignment
#[derive(Debug, Clone)]
pub struct Assignment {
    pub column: String,
    pub value: Expression,
}

/// DELETE query
#[derive(Debug, Clone)]
pub struct DeleteQuery {
    pub table: String,
    pub where_clause: Option<Expression>,
}

/// Vector search query
#[derive(Debug, Clone)]
pub struct VectorQuery {
    pub nearest: NearestNeighbors,
    pub filter: Option<Expression>,
    pub limit: Option<LimitClause>,
}

/// CREATE TABLE query
#[derive(Debug, Clone)]
pub struct CreateTableQuery {
    pub name: String,
    pub columns: Vec<ColumnDefinition>,
    pub constraints: Vec<TableConstraint>,
}

/// Column definition
#[derive(Debug, Clone)]
pub struct ColumnDefinition {
    pub name: String,
    pub data_type: crate::data::DataType,
    pub nullable: bool,
    pub default: Option<Expression>,
}

/// Table constraints
#[derive(Debug, Clone)]
pub enum TableConstraint {
    PrimaryKey(Vec<String>),
    Unique(Vec<String>),
    ForeignKey {
        columns: Vec<String>,
        ref_table: String,
        ref_columns: Vec<String>,
    },
}

/// DROP TABLE query
#[derive(Debug, Clone)]
pub struct DropTableQuery {
    pub name: String,
    pub if_exists: bool,
}
