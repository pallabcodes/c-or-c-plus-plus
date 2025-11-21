//! AuroraDB Constraints: Intelligent Data Integrity with Smart Validation
//!
//! Revolutionary constraint system that eliminates traditional database
//! constraint pain points through intelligent validation, performance
//! optimization, and automated management.

pub mod constraint_manager;
pub mod foreign_key_constraint;
pub mod check_constraint;
pub mod unique_constraint;
pub mod not_null_constraint;
pub mod validation_engine;
pub mod performance_optimizer;
pub mod constraint_suggester;

pub use constraint_manager::*;
pub use foreign_key_constraint::*;
pub use check_constraint::*;
pub use unique_constraint::*;
pub use not_null_constraint::*;
pub use validation_engine::*;
pub use performance_optimizer::*;
pub use constraint_suggester::*;
