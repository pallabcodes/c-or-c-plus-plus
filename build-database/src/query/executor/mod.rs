//! Query Execution Engine
//!
//! Volcano-based iterator model execution with vectorized operations:
//! - Pull-based execution with proper data flow control
//! - SIMD vectorized operations for analytical workloads
//! - Adaptive execution with runtime optimization
//! - JIT compilation for hot execution paths
//!
//! UNIQUENESS: Fuses Volcano iterator model + vectorized execution + adaptive optimization
//! Research: Iterator-based execution + SIMD processing + runtime code generation

pub mod executor;
pub mod operators;
pub mod vectorized;
pub mod adaptive;

// Re-export main execution components
pub use executor::{QueryExecutor, ExecutionResult, ExecutionStats};
pub use operators::*;
pub use vectorized::*;
pub use adaptive::*;
