//! AuroraDB Query Engine - UNIQUENESS AI-Powered Query Processing
//!
//! High-performance, ML-optimized query processing engine that leverages
//! AuroraDB's advanced storage capabilities and Cyclone's networking.
//!
//! Key Features:
//! - ML-powered query optimization with cost prediction
//! - SIMD-accelerated query execution
//! - Real-time adaptive optimization
//! - Multi-model query support (SQL + Vector + Graph + Time Series)
//! - Federated query execution across clusters

pub mod parser;
pub mod optimizer;
pub mod executor;
pub mod planner;
pub mod statistics;
pub mod cache;
pub mod types;
pub mod error;

#[cfg(feature = "ml_optimization")]
pub mod ml_optimizer;

#[cfg(feature = "simd_acceleration")]
pub mod simd_executor;

pub use parser::QueryParser;
pub use optimizer::QueryOptimizer;
pub use executor::QueryExecutor;
pub use planner::QueryPlanner;
pub use statistics::StatisticsManager;
pub use cache::QueryCache;
pub use types::*;
pub use error::{QueryError, Result};

// Re-export commonly used types
pub use types::{
    Query, QueryPlan, ExecutionResult, QueryMetrics,
    TableSchema, ColumnSchema, DataType,
};

// UNIQUENESS Research Citations:
// - **Query Optimization**: Selinger (1979) - System R optimizer
// - **Volcano Model**: Graefe (1990) - Iterator-based execution
// - **Cost-Based Optimization**: Ioannidis & Christodoulakis (1993)
// - **Adaptive Query Processing**: Deshpande et al. (2007)
// - **ML Query Optimization**: Krishnan et al. (2018) - Neo research
