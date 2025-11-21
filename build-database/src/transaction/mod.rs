//! Transaction Management Module
//!
//! ACID-compliant transaction management with MVCC and concurrency control:
//! - Multi-Version Concurrency Control (MVCC) for snapshot isolation
//! - ACID guarantees (Atomicity, Consistency, Isolation, Durability)
//! - Lock-based and optimistic concurrency control
//! - Write-ahead logging and recovery
//!
//! UNIQUENESS: Fuses ARIES recovery + MVCC research + distributed consensus
//! Research: ARIES algorithm + Serializable Snapshot Isolation + Optimistic Concurrency

pub mod manager;
pub mod mvcc;
pub mod locking;
pub mod logging;
pub mod recovery;

// Re-export main transaction components
pub use manager::{TransactionManager, Transaction, TransactionId, TransactionStatus};
pub use mvcc::{MVCCManager, VersionChain, Snapshot};
pub use locking::{LockManager, LockMode, LockRequest};
pub use logging::{WALManager, LogRecord};
pub use recovery::{RecoveryManager, CheckpointManager};
