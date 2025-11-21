//! Abstract Syntax Tree (AST) for AuroraDB SQL
//!
//! Comprehensive AST representation supporting AuroraDB's UNIQUENESS SQL features:
//! - Standard SQL (SELECT, INSERT, UPDATE, DELETE)
//! - Advanced features (Views, CTEs, Window functions, Arrays)
//! - AuroraDB extensions (Vector search, Time series, Graph queries)

use std::collections::HashMap;
use crate::core::errors::{AuroraResult, AuroraError};

/// Complete SQL Statement AST
#[derive(Debug, Clone, PartialEq)]
pub enum Statement {
    Select(SelectStatement),
    Insert(InsertStatement),
    Update(UpdateStatement),
    Delete(DeleteStatement),
    Create(CreateStatement),
    Drop(DropStatement),
    Alter(AlterStatement),
    Begin(BeginStatement),
    Commit(CommitStatement),
    Rollback(RollbackStatement),
    Set(SetStatement),
    Show(ShowStatement),
    Explain(ExplainStatement),
    // AuroraDB UNIQUENESS extensions
    CreateView(CreateViewStatement),
    CreateIndex(CreateIndexStatement),
    CreateTrigger(CreateTriggerStatement),
    VectorSearch(VectorSearchStatement),
}

/// SELECT statement AST
#[derive(Debug, Clone, PartialEq)]
pub struct SelectStatement {
    pub with: Option<WithClause>,
    pub select: SelectClause,
    pub from: Option<FromClause>,
    pub where_clause: Option<WhereClause>,
    pub group_by: Option<GroupByClause>,
    pub having: Option<HavingClause>,
    pub order_by: Option<OrderByClause>,
    pub limit: Option<LimitClause>,
    pub offset: Option<OffsetClause>,
    pub union: Option<Box<SelectStatement>>,
    pub union_all: bool,
}

/// INSERT statement AST
#[derive(Debug, Clone, PartialEq)]
pub struct InsertStatement {
    pub table_name: String,
    pub columns: Vec<String>,
    pub values: Vec<Vec<Expression>>,
    pub select: Option<SelectStatement>,
    pub on_conflict: Option<OnConflictClause>,
    pub returning: Option<Vec<String>>,
}

/// UPDATE statement AST
#[derive(Debug, Clone, PartialEq)]
pub struct UpdateStatement {
    pub table_name: String,
    pub set: Vec<(String, Expression)>,
    pub from: Option<FromClause>,
    pub where_clause: Option<WhereClause>,
    pub returning: Option<Vec<String>>,
}

/// DELETE statement AST
#[derive(Debug, Clone, PartialEq)]
pub struct DeleteStatement {
    pub table_name: String,
    pub using: Option<FromClause>,
    pub where_clause: Option<WhereClause>,
    pub returning: Option<Vec<String>>,
}

/// CREATE statement AST
#[derive(Debug, Clone, PartialEq)]
pub enum CreateStatement {
    Table(CreateTableStatement),
    Database(CreateDatabaseStatement),
    Schema(CreateSchemaStatement),
}

/// CREATE TABLE statement
#[derive(Debug, Clone, PartialEq)]
pub struct CreateTableStatement {
    pub table_name: String,
    pub columns: Vec<ColumnDefinition>,
    pub constraints: Vec<TableConstraint>,
    pub if_not_exists: bool,
}

/// Column definition
#[derive(Debug, Clone, PartialEq)]
pub struct ColumnDefinition {
    pub name: String,
    pub data_type: DataType,
    pub nullable: bool,
    pub default: Option<Expression>,
    pub constraints: Vec<ColumnConstraint>,
}

/// Data types supported by AuroraDB
#[derive(Debug, Clone, PartialEq)]
pub enum DataType {
    // Basic types
    Boolean,
    Integer,
    BigInt,
    SmallInt,
    Float,
    Double,
    Decimal(u8, u8), // precision, scale
    String(u32),     // max length
    Text,
    Blob(u32),       // max size
    Date,
    Time,
    DateTime,
    Timestamp,
    Interval,

    // Arrays
    Array(Box<DataType>),

    // AuroraDB UNIQUENESS types
    Vector(u32),     // dimension for vector search
    Json,
    Uuid,
    IpAddress,
    MacAddress,
}

/// Table constraints
#[derive(Debug, Clone, PartialEq)]
pub enum TableConstraint {
    PrimaryKey(Vec<String>),
    Unique(Vec<String>),
    ForeignKey {
        columns: Vec<String>,
        ref_table: String,
        ref_columns: Vec<String>,
        on_delete: Option<ReferentialAction>,
        on_update: Option<ReferentialAction>,
    },
    Check(Expression),
}

/// Column constraints
#[derive(Debug, Clone, PartialEq)]
pub enum ColumnConstraint {
    NotNull,
    Unique,
    PrimaryKey,
    ForeignKey {
        ref_table: String,
        ref_column: String,
        on_delete: Option<ReferentialAction>,
        on_update: Option<ReferentialAction>,
    },
    Check(Expression),
    Default(Expression),
}

/// Referential actions
#[derive(Debug, Clone, PartialEq)]
pub enum ReferentialAction {
    NoAction,
    Restrict,
    Cascade,
    SetNull,
    SetDefault,
}

/// WITH clause for CTEs
#[derive(Debug, Clone, PartialEq)]
pub struct WithClause {
    pub recursive: bool,
    pub ctes: Vec<CommonTableExpression>,
}

/// Common Table Expression
#[derive(Debug, Clone, PartialEq)]
pub struct CommonTableExpression {
    pub name: String,
    pub columns: Option<Vec<String>>,
    pub query: SelectStatement,
}

/// SELECT clause
#[derive(Debug, Clone, PartialEq)]
pub struct SelectClause {
    pub distinct: bool,
    pub select_list: Vec<SelectItem>,
}

/// Select list items
#[derive(Debug, Clone, PartialEq)]
pub enum SelectItem {
    Expression(Expression, Option<String>), // expression, alias
    Wildcard,                               // *
    QualifiedWildcard(String),              // table.*
}

/// FROM clause
#[derive(Debug, Clone, PartialEq)]
pub struct FromClause {
    pub items: Vec<FromItem>,
}

/// FROM clause items
#[derive(Debug, Clone, PartialEq)]
pub enum FromItem {
    Table {
        name: String,
        alias: Option<String>,
    },
    Subquery {
        query: Box<SelectStatement>,
        alias: String,
    },
    Join {
        left: Box<FromItem>,
        right: Box<FromItem>,
        join_type: JoinType,
        condition: Option<Expression>,
    },
}

/// Join types
#[derive(Debug, Clone, PartialEq)]
pub enum JoinType {
    Inner,
    Left,
    Right,
    Full,
    Cross,
}

/// WHERE clause
#[derive(Debug, Clone, PartialEq)]
pub struct WhereClause {
    pub condition: Expression,
}

/// GROUP BY clause
#[derive(Debug, Clone, PartialEq)]
pub struct GroupByClause {
    pub expressions: Vec<Expression>,
}

/// HAVING clause
#[derive(Debug, Clone, PartialEq)]
pub struct HavingClause {
    pub condition: Expression,
}

/// ORDER BY clause
#[derive(Debug, Clone, PartialEq)]
pub struct OrderByClause {
    pub items: Vec<OrderByItem>,
}

/// ORDER BY item
#[derive(Debug, Clone, PartialEq)]
pub struct OrderByItem {
    pub expression: Expression,
    pub direction: OrderDirection,
    pub nulls: NullsOrder,
}

/// Order direction
#[derive(Debug, Clone, PartialEq)]
pub enum OrderDirection {
    Ascending,
    Descending,
}

/// NULLS ordering
#[derive(Debug, Clone, PartialEq)]
pub enum NullsOrder {
    First,
    Last,
}

/// LIMIT clause
#[derive(Debug, Clone, PartialEq)]
pub struct LimitClause {
    pub count: Expression,
}

/// OFFSET clause
#[derive(Debug, Clone, PartialEq)]
pub struct OffsetClause {
    pub offset: Expression,
}

/// Expressions in AuroraDB
#[derive(Debug, Clone, PartialEq)]
pub enum Expression {
    // Literals
    Literal(LiteralValue),

    // Column references
    Column(String),                    // column
    QualifiedColumn(String, String),   // table.column

    // Binary operations
    BinaryOp {
        left: Box<Expression>,
        op: BinaryOperator,
        right: Box<Expression>,
    },

    // Unary operations
    UnaryOp {
        op: UnaryOperator,
        expr: Box<Expression>,
    },

    // Function calls
    Function {
        name: String,
        args: Vec<Expression>,
        distinct: bool,
        filter: Option<Box<Expression>>, // FILTER clause
        over: Option<WindowSpec>,        // Window functions
    },

    // Aggregates
    Aggregate {
        func: AggregateFunction,
        args: Vec<Expression>,
        distinct: bool,
        filter: Option<Box<Expression>>,
    },

    // Subqueries
    Subquery(Box<SelectStatement>),

    // EXISTS
    Exists(Box<SelectStatement>),

    // IN expressions
    In {
        expr: Box<Expression>,
        list: Vec<Expression>,
        not: bool,
    },

    // BETWEEN
    Between {
        expr: Box<Expression>,
        low: Box<Expression>,
        high: Box<Expression>,
        not: bool,
    },

    // CASE expressions
    Case {
        operand: Option<Box<Expression>>,
        when_clauses: Vec<(Expression, Expression)>,
        else_clause: Option<Box<Expression>>,
    },

    // CAST expressions
    Cast {
        expr: Box<Expression>,
        data_type: DataType,
    },

    // Array expressions
    Array(Vec<Expression>),
    ArrayAccess {
        array: Box<Expression>,
        index: Box<Expression>,
    },

    // AuroraDB UNIQUENESS expressions
    VectorLiteral(Vec<f32>),           // [1.0, 2.0, 3.0]
    VectorDistance {
        left: Box<Expression>,
        right: Box<Expression>,
        metric: VectorMetric,
    },
    JsonExtract {
        json: Box<Expression>,
        path: String,
    },
}

/// Literal values
#[derive(Debug, Clone, PartialEq)]
pub enum LiteralValue {
    Null,
    Boolean(bool),
    Integer(i64),
    Float(f64),
    String(String),
    Date(String),
    Time(String),
    DateTime(String),
    Interval(String),
    Array(Vec<LiteralValue>),
}

/// Binary operators
#[derive(Debug, Clone, PartialEq)]
pub enum BinaryOperator {
    // Arithmetic
    Plus,
    Minus,
    Multiply,
    Divide,
    Modulo,

    // Comparison
    Equal,
    NotEqual,
    LessThan,
    LessThanOrEqual,
    GreaterThan,
    GreaterThanOrEqual,
    Like,
    NotLike,
    ILike,
    NotILike,
    SimilarTo,
    NotSimilarTo,
    RegexMatch,
    NotRegexMatch,
    RegexIMatch,
    NotRegexIMatch,

    // Logical
    And,
    Or,

    // String concatenation
    Concatenate,

    // Array operations
    ArrayContains,
    ArrayContainedBy,
    ArrayOverlap,

    // JSON operations
    JsonContains,
    JsonContainedBy,
}

/// Unary operators
#[derive(Debug, Clone, PartialEq)]
pub enum UnaryOperator {
    Not,
    Plus,
    Minus,
    IsNull,
    IsNotNull,
}

/// Aggregate functions
#[derive(Debug, Clone, PartialEq)]
pub enum AggregateFunction {
    Count,
    Sum,
    Avg,
    Min,
    Max,
    StdDev,
    Variance,
    StringAgg,
    ArrayAgg,
    // AuroraDB UNIQUENESS aggregates
    VectorAvg,     // Average of vectors
    Percentile,    // Percentile calculation
}

/// Window specification for window functions
#[derive(Debug, Clone, PartialEq)]
pub struct WindowSpec {
    pub partition_by: Vec<Expression>,
    pub order_by: Vec<OrderByItem>,
    pub frame: Option<WindowFrame>,
}

/// Window frame specification
#[derive(Debug, Clone, PartialEq)]
pub struct WindowFrame {
    pub frame_type: FrameType,
    pub start_bound: FrameBound,
    pub end_bound: FrameBound,
}

/// Frame types
#[derive(Debug, Clone, PartialEq)]
pub enum FrameType {
    Rows,
    Range,
    Groups,
}

/// Frame bounds
#[derive(Debug, Clone, PartialEq)]
pub enum FrameBound {
    UnboundedPreceding,
    Preceding(u64),
    CurrentRow,
    Following(u64),
    UnboundedFollowing,
}

/// Vector distance metrics for AuroraDB UNIQUENESS
#[derive(Debug, Clone, PartialEq)]
pub enum VectorMetric {
    Cosine,
    Euclidean,
    DotProduct,
    Manhattan,
    Hamming,
}

// Additional statement types

/// CREATE VIEW statement
#[derive(Debug, Clone, PartialEq)]
pub struct CreateViewStatement {
    pub view_name: String,
    pub columns: Option<Vec<String>>,
    pub query: SelectStatement,
    pub materialized: bool,
    pub if_not_exists: bool,
}

/// CREATE INDEX statement
#[derive(Debug, Clone, PartialEq)]
pub struct CreateIndexStatement {
    pub index_name: String,
    pub table_name: String,
    pub columns: Vec<String>,
    pub index_type: IndexType,
    pub unique: bool,
    pub if_not_exists: bool,
}

/// Index types for AuroraDB UNIQUENESS
#[derive(Debug, Clone, PartialEq)]
pub enum IndexType {
    BTree,
    Hash,
    Gist,
    Gin,
    SpGist,
    Brin,
    // AuroraDB UNIQUENESS index types
    VectorHNSW,    // Hierarchical Navigable Small World
    VectorIVF,     // Inverted File Index
    FullText,      // Full-text search index
}

/// CREATE TRIGGER statement
#[derive(Debug, Clone, PartialEq)]
pub struct CreateTriggerStatement {
    pub trigger_name: String,
    pub table_name: String,
    pub events: Vec<TriggerEvent>,
    pub timing: TriggerTiming,
    pub function_name: String,
    pub arguments: Vec<String>,
    pub condition: Option<Expression>,
}

/// Trigger events
#[derive(Debug, Clone, PartialEq)]
pub enum TriggerEvent {
    Insert,
    Update,
    Delete,
}

/// Trigger timing
#[derive(Debug, Clone, PartialEq)]
pub enum TriggerTiming {
    Before,
    After,
    InsteadOf,
}

/// Vector search statement for AuroraDB UNIQUENESS
#[derive(Debug, Clone, PartialEq)]
pub struct VectorSearchStatement {
    pub table_name: String,
    pub vector_column: String,
    pub query_vector: Vec<f32>,
    pub metric: VectorMetric,
    pub limit: Option<u32>,
    pub where_clause: Option<WhereClause>,
}

/// Other statement types (simplified)
#[derive(Debug, Clone, PartialEq)]
pub struct DropStatement {
    pub object_type: String,
    pub object_name: String,
    pub if_exists: bool,
}

#[derive(Debug, Clone, PartialEq)]
pub struct AlterStatement {
    pub object_type: String,
    pub object_name: String,
    pub action: String,
}

#[derive(Debug, Clone, PartialEq)]
pub struct BeginStatement {
    pub isolation_level: Option<String>,
    pub read_only: bool,
}

#[derive(Debug, Clone, PartialEq)]
pub struct CommitStatement;

#[derive(Debug, Clone, PartialEq)]
pub struct RollbackStatement;

#[derive(Debug, Clone, PartialEq)]
pub struct SetStatement {
    pub variable: String,
    pub value: Expression,
}

#[derive(Debug, Clone, PartialEq)]
pub struct ShowStatement {
    pub what: String,
}

#[derive(Debug, Clone, PartialEq)]
pub struct ExplainStatement {
    pub statement: Box<Statement>,
    pub analyze: bool,
    pub verbose: bool,
}

#[derive(Debug, Clone, PartialEq)]
pub struct CreateDatabaseStatement {
    pub database_name: String,
    pub if_not_exists: bool,
}

#[derive(Debug, Clone, PartialEq)]
pub struct CreateSchemaStatement {
    pub schema_name: String,
    pub if_not_exists: bool,
}

#[derive(Debug, Clone, PartialEq)]
pub struct OnConflictClause {
    pub target: Option<Vec<String>>,
    pub action: ConflictAction,
}

#[derive(Debug, Clone, PartialEq)]
pub enum ConflictAction {
    DoNothing,
    DoUpdate(Vec<(String, Expression)>),
}

// AST visitor pattern for query processing
pub trait AstVisitor<T> {
    fn visit_statement(&mut self, stmt: &Statement) -> AuroraResult<T>;
    fn visit_select(&mut self, select: &SelectStatement) -> AuroraResult<T>;
    fn visit_expression(&mut self, expr: &Expression) -> AuroraResult<T>;
}

impl std::fmt::Display for Statement {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Statement::Select(_) => write!(f, "SELECT statement"),
            Statement::Insert(_) => write!(f, "INSERT statement"),
            Statement::Update(_) => write!(f, "UPDATE statement"),
            Statement::Delete(_) => write!(f, "DELETE statement"),
            Statement::Create(_) => write!(f, "CREATE statement"),
            Statement::Drop(_) => write!(f, "DROP statement"),
            Statement::Alter(_) => write!(f, "ALTER statement"),
            Statement::Begin(_) => write!(f, "BEGIN statement"),
            Statement::Commit(_) => write!(f, "COMMIT statement"),
            Statement::Rollback(_) => write!(f, "ROLLBACK statement"),
            Statement::Set(_) => write!(f, "SET statement"),
            Statement::Show(_) => write!(f, "SHOW statement"),
            Statement::Explain(_) => write!(f, "EXPLAIN statement"),
            Statement::CreateView(_) => write!(f, "CREATE VIEW statement"),
            Statement::CreateIndex(_) => write!(f, "CREATE INDEX statement"),
            Statement::CreateTrigger(_) => write!(f, "CREATE TRIGGER statement"),
            Statement::VectorSearch(_) => write!(f, "VECTOR SEARCH statement"),
        }
    }
}

impl std::fmt::Display for Expression {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Expression::Literal(_) => write!(f, "literal"),
            Expression::Column(name) => write!(f, "column {}", name),
            Expression::QualifiedColumn(table, column) => write!(f, "{}.{}", table, column),
            Expression::BinaryOp { op, .. } => write!(f, "binary operation {:?}", op),
            Expression::Function { name, .. } => write!(f, "function {}", name),
            Expression::Aggregate { func, .. } => write!(f, "aggregate {:?}", func),
            Expression::Subquery(_) => write!(f, "subquery"),
            _ => write!(f, "expression"),
        }
    }
}
