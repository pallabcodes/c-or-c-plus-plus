//! Query Planning Errors
//!
//! Specific error types for query planning operations.

/// Query planning result
pub type PlanResult<T> = Result<T, PlanError>;

/// Query planning specific errors
#[derive(Debug, thiserror::Error)]
pub enum PlanError {
    #[error("Invalid query structure: {message}")]
    InvalidQuery { message: String },

    #[error("Unsupported operation: {operation}")]
    UnsupportedOperation { operation: String },

    #[error("Statistics unavailable for table: {table}")]
    MissingStatistics { table: String },

    #[error("Optimization failed: {message}")]
    OptimizationFailed { message: String },

    #[error("Cost estimation error: {message}")]
    CostEstimationError { message: String },
}
