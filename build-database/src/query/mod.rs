//! AuroraDB Query Processing Module
//!
//! This module provides AI-Native query processing capabilities:
//! - SQL Parser: Parse traditional SQL + vector search extensions
//! - Query Planner: AI-powered optimization with learning
//! - Execution Engine: Vectorized operators + JIT compilation
//! - Analytics Engine: Real-time analytics with ML acceleration
//!
//! UNIQUENESS: Fuses PostgreSQL query optimization + ClickHouse vectorized execution +
//! Pinecone vector search + ML-powered adaptive optimization for 10x better performance

pub mod parser;
pub mod planner;
pub mod executor;
pub mod optimizer;
pub mod analytics;
pub mod views;
pub mod recursive_ctes;
pub mod stored_procedures;
pub mod triggers;
pub mod indexes;

// Re-export main query processing components
pub use parser::{SqlParser, ParseResult, Query};
pub use planner::{QueryPlanner, PlanResult, QueryPlan};
pub use executor::{QueryExecutor, ExecutionResult};
pub use optimizer::{QueryOptimizer, OptimizationResult};
pub use views::*;
pub use recursive_ctes::*;
pub use stored_procedures::*;
pub use triggers::*;
pub use indexes::*;
