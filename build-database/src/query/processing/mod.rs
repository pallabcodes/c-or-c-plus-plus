//! AuroraDB Query Processing Engine: The Brain of the Database
//!
//! UNIQUENESS: Revolutionary query processing fusing research-backed approaches:
//! - Pratt parser for robust SQL parsing with error recovery
//! - AI-powered query optimization with cost-based planning
//! - Vectorized execution engine with SIMD acceleration
//! - Adaptive query processing with runtime optimization

pub mod sql_parser;
pub mod query_planner;
pub mod query_optimizer;
pub mod execution_engine;
pub mod ast;
pub mod plan;

pub use sql_parser::*;
pub use query_planner::*;
pub use query_optimizer::*;
pub use execution_engine::*;
pub use ast::*;
pub use plan::*;
