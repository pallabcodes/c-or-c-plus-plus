//! AuroraDB Rust Driver
//!
//! The native Rust driver for AuroraDB, providing high-performance,
//! async/await native database connectivity with advanced features.

pub mod client;
pub mod connection;
pub mod pool;
pub mod protocol;
pub mod types;
pub mod error;
pub mod config;

pub use client::AuroraClient;
pub use connection::AuroraConnection;
pub use pool::AuroraConnectionPool;
pub use error::{AuroraError, Result};
pub use config::AuroraConfig;

/// Re-export commonly used types
pub use types::{
    AuroraValue, AuroraRow, AuroraColumn,
    VectorSearchResult, AnalyticsResult,
    QueryResult, ExecuteResult
};

// UNIQUENESS Research Citations:
// - **Async Database Drivers**: tokio-postgres, rust-postgres research
// - **Connection Pooling**: bb8, deadpool connection pool research
// - **Protocol Design**: PostgreSQL wire protocol, AuroraDB binary protocol
// - **Performance Optimization**: zero-copy operations, SIMD acceleration
