//! AuroraDB Drivers - UNIQUENESS Database Connectivity
//!
//! High-performance, async-native database drivers for AuroraDB that leverage
//! the database's advanced features including vector search, analytics, and streaming.
//!
//! This crate provides the core protocol implementation and foundation for
//! language-specific driver bindings.

pub mod protocol;
pub mod connection;
pub mod pool;
pub mod types;
pub mod error;
pub mod config;
pub mod metrics;

pub use protocol::AuroraProtocol;
pub use connection::AuroraConnection;
pub use pool::AuroraConnectionPool;
pub use types::*;
pub use error::{AuroraError, Result};
pub use config::AuroraConfig;
pub use metrics::DriverMetrics;

// Re-export commonly used types
pub use types::{
    AuroraValue, AuroraRow, AuroraColumn, AuroraType,
    VectorSearchRequest, VectorSearchResult,
    AnalyticsRequest, AnalyticsResult,
    QueryRequest, QueryResult,
    ExecuteRequest, ExecuteResult,
    StreamRequest,
};

// UNIQUENESS Research Citations:
// - **Database Drivers**: PostgreSQL libpq, MySQL Connector/C research
// - **Async I/O**: tokio, async-std research on async database drivers
// - **Protocol Design**: AuroraDB binary protocol, PostgreSQL wire protocol
// - **Connection Pooling**: HikariCP, c3p0 research on connection management
// - **Type Systems**: SQL type systems, ORM type mapping research
