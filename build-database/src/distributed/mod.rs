//! AuroraDB High Availability Clustering
//!
//! Enterprise-grade multi-node clustering with automatic failover:
//! - Leader election and consensus
//! - Data replication and consistency
//! - Load balancing and query routing
//! - Health monitoring and recovery
//! - Cross-region replication
//! UNIQUENESS: Research-backed clustering combining Raft consensus with advanced replication.

pub mod cluster;
pub mod consensus;
pub mod replication;
pub mod failover;
pub mod load_balancer;
pub mod health_monitor;

pub use cluster::*;
pub use consensus::*;
pub use replication::*;
pub use failover::*;
pub use load_balancer::*;
pub use health_monitor::*;
