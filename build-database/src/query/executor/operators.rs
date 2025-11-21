//! Physical Operators for Query Execution
//!
//! Re-exports from specialized operator implementations.
//! UNIQUENESS: Volcano iterator model with vectorized operations

pub mod traits;
pub mod scan;
pub mod join;
pub mod aggregate;
pub mod sort;

// Re-export the main operator trait and implementations
pub use traits::*;
pub use scan::*;
pub use join::*;
pub use aggregate::*;
pub use sort::*;