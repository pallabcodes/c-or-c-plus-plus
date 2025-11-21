//! Enhanced AuroraDB Transaction Management with UNIQUENESS
//!
//! Fuses multiple research-backed approaches for ACID compliance and high concurrency:
//! - ARIES recovery algorithm for durability
//! - MVCC for isolation and high concurrency
//! - 2PL + SSI for consistency guarantees
//! - Adaptive concurrency control based on workload patterns

pub mod unified_transaction_manager;
pub mod mvcc_engine;
pub mod deadlock_detector;
pub mod adaptive_concurrency_control;
pub mod distributed_transaction_coordinator;

pub use unified_transaction_manager::*;
pub use mvcc_engine::*;
pub use deadlock_detector::*;
pub use adaptive_concurrency_control::*;
pub use distributed_transaction_coordinator::*;
