//! Orchestration layer for Aurora Coordinator
//!
//! UNIQUENESS: Research-backed cluster orchestration combining consensus,
//! membership, networking, and AuroraDB coordination into a cohesive system.

pub mod coordinator;
pub mod aurora_integration;
pub mod cluster_manager;

// Re-export main types
pub use coordinator::Coordinator;
pub use aurora_integration::AuroraClusterManager;
pub use cluster_manager::ClusterManager;
