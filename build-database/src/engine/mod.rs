//! AuroraDB Production Database Engine Module
//!
//! This module provides the core integration layer that transforms AuroraDB
//! from individual research components into a unified, production-ready database system.
//!
//! Key responsibilities:
//! - Main AuroraDB engine orchestration
//! - Query execution pipeline integration
//! - Production database server
//! - Component lifecycle management
//! - Production-grade error handling and logging

pub mod aurora_db;
pub mod query_pipeline;
pub mod server;

// Re-export the main database engine
pub use aurora_db::*;

// Re-export query pipeline
pub use query_pipeline::*;

// Re-export server components
pub use server::*;

// Re-export common types for convenience
pub use aurora_db::{
    AuroraDB, UserContext, QueryResult, VectorSearchRequest, VectorSearchResult,
    AnalyticsQuery, AnalyticsResult, IsolationLevel, TableSchema, ColumnDefinition,
    DataType, IndexDefinition, IndexType, HealthStatus, HealthState, DatabaseMetrics,
};
