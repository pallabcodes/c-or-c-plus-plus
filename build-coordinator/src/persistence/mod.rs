//! Persistence Layer: Production-Grade State Management
//!
//! UNIQUENESS: Durable state persistence with atomic writes,
//! crash recovery, and multi-version concurrency control.

pub mod state_store;
pub mod log_store;
pub mod snapshot_store;
pub mod recovery_manager;

pub use state_store::StateStore;
pub use log_store::LogStore;
pub use snapshot_store::SnapshotStore;
pub use recovery_manager::RecoveryManager;

// UNIQUENESS Research Citations:
// - **Write-Ahead Logging**: PostgreSQL WAL implementation
// - **LSM Trees**: LevelDB, RocksDB architecture
// - **MVCC**: PostgreSQL multi-version concurrency control
// - **Atomic Writes**: Filesystem atomic operations research
