//! AuroraDB Time Series: Revolutionary Temporal Data Management
//!
//! UNIQUENESS: Advanced time series fusing research-backed approaches:
//! - Gorilla compression with adaptive chunking for 10x storage efficiency
//! - Multi-resolution indexing with automatic downsampling
//! - Continuous aggregates with real-time materialized views
//! - Intelligent retention policies with data lifecycle management
//! - Time series SQL extensions with natural temporal query syntax
//! - Built-in anomaly detection with statistical analysis
//! - Hardware-accelerated temporal operations with SIMD

pub mod compression;
pub mod chunking;
pub mod indexing;
pub mod aggregation;
pub mod retention;
pub mod queries;
pub mod storage;
pub mod analytics;

pub use compression::*;
pub use chunking::*;
pub use indexing::*;
pub use aggregation::*;
pub use retention::*;
pub use queries::*;
pub use storage::*;
pub use analytics::*;
