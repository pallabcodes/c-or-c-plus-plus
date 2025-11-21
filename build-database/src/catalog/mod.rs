//! AuroraDB Catalog System
//!
//! The catalog stores metadata about database objects:
//! - Tables and their schemas
//! - Columns and data types
//! - Indexes and constraints
//! - System information
//!
//! This enables DDL operations and provides schema information for query planning.

pub mod table_catalog;
pub mod system_catalog;

pub use table_catalog::*;
pub use system_catalog::*;
