//! Query Engine Error Types
//!
//! Comprehensive error handling for the AuroraDB query engine.

use thiserror::Error;

/// Result type for query operations
pub type Result<T> = std::result::Result<T, QueryError>;

/// Query engine error types
#[derive(Error, Debug)]
pub enum QueryError {
    #[error("Parse error: {message}")]
    ParseError { message: String },

    #[error("Semantic error: {message}")]
    SemanticError { message: String },

    #[error("Optimization error: {message}")]
    OptimizationError { message: String },

    #[error("Execution error: {message}")]
    ExecutionError { message: String },

    #[error("Type error: expected {expected}, got {actual}")]
    TypeError { expected: String, actual: String },

    #[error("Schema error: {message}")]
    SchemaError { message: String },

    #[error("Permission denied: {resource}")]
    PermissionDenied { resource: String },

    #[error("Timeout error: operation timed out after {duration}ms")]
    TimeoutError { duration: u64 },

    #[error("Resource exhausted: {resource}")]
    ResourceExhausted { resource: String },

    #[error("Connection error: {message}")]
    ConnectionError { message: String },

    #[error("Serialization error: {message}")]
    SerializationError { message: String },

    #[error("Internal error: {message}")]
    InternalError { message: String },
}

impl QueryError {
    /// Create a parse error
    pub fn parse(message: impl Into<String>) -> Self {
        QueryError::ParseError {
            message: message.into(),
        }
    }

    /// Create a semantic error
    pub fn semantic(message: impl Into<String>) -> Self {
        QueryError::SemanticError {
            message: message.into(),
        }
    }

    /// Create an optimization error
    pub fn optimization(message: impl Into<String>) -> Self {
        QueryError::OptimizationError {
            message: message.into(),
        }
    }

    /// Create an execution error
    pub fn execution(message: impl Into<String>) -> Self {
        QueryError::ExecutionError {
            message: message.into(),
        }
    }

    /// Create a type error
    pub fn type_error(expected: impl Into<String>, actual: impl Into<String>) -> Self {
        QueryError::TypeError {
            expected: expected.into(),
            actual: actual.into(),
        }
    }

    /// Create a schema error
    pub fn schema(message: impl Into<String>) -> Self {
        QueryError::SchemaError {
            message: message.into(),
        }
    }

    /// Create a permission error
    pub fn permission_denied(resource: impl Into<String>) -> Self {
        QueryError::PermissionDenied {
            resource: resource.into(),
        }
    }

    /// Create a timeout error
    pub fn timeout(duration: u64) -> Self {
        QueryError::TimeoutError { duration }
    }

    /// Create a resource exhausted error
    pub fn resource_exhausted(resource: impl Into<String>) -> Self {
        QueryError::ResourceExhausted {
            resource: resource.into(),
        }
    }

    /// Create a connection error
    pub fn connection(message: impl Into<String>) -> Self {
        QueryError::ConnectionError {
            message: message.into(),
        }
    }

    /// Create a serialization error
    pub fn serialization(message: impl Into<String>) -> Self {
        QueryError::SerializationError {
            message: message.into(),
        }
    }

    /// Create an internal error
    pub fn internal(message: impl Into<String>) -> Self {
        QueryError::InternalError {
            message: message.into(),
        }
    }

    /// Get error category for metrics
    pub fn category(&self) -> &'static str {
        match self {
            QueryError::ParseError { .. } => "parse",
            QueryError::SemanticError { .. } => "semantic",
            QueryError::OptimizationError { .. } => "optimization",
            QueryError::ExecutionError { .. } => "execution",
            QueryError::TypeError { .. } => "type",
            QueryError::SchemaError { .. } => "schema",
            QueryError::PermissionDenied { .. } => "permission",
            QueryError::TimeoutError { .. } => "timeout",
            QueryError::ResourceExhausted { .. } => "resource",
            QueryError::ConnectionError { .. } => "connection",
            QueryError::SerializationError { .. } => "serialization",
            QueryError::InternalError { .. } => "internal",
        }
    }

    /// Check if error is retryable
    pub fn is_retryable(&self) -> bool {
        matches!(
            self,
            QueryError::TimeoutError { .. }
                | QueryError::ConnectionError { .. }
                | QueryError::ResourceExhausted { .. }
        )
    }
}
