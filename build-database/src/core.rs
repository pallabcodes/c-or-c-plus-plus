//! AuroraDB Core Module
//!
//! This module provides the fundamental types, errors, and configurations
//! for the AuroraDB database system. It serves as the main entry point
//! and re-exports all core components.

// Re-export all modules for convenient access
pub mod errors;
pub mod types;
pub mod data;
pub mod schema;
pub mod config;

// Re-export commonly used types at the top level
pub use errors::{AuroraError, AuroraResult};
pub use types::*;
pub use data::*;
pub use schema::*;
pub use config::*;