//! MVCC (Multi-Version Concurrency Control) Implementation
//!
//! Research-backed MVCC from PostgreSQL architecture enabling:
//! - Snapshot isolation without blocking
//! - Multiple concurrent transactions
//! - Non-blocking reads during writes
//! - Serializable isolation levels

pub mod transaction;
pub mod version;
pub mod snapshot;
pub mod visibility;
pub mod lock_manager;

pub use transaction::*;
pub use version::*;
pub use snapshot::*;
pub use visibility::*;
pub use lock_manager::*;
