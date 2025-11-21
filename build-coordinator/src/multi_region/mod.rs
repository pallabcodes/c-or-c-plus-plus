//! Multi-Region Support: UNIQUENESS Global Coordination
//!
//! Research-backed multi-region coordination for global deployments:
//! - **WAN-Optimized Consensus**: Consensus algorithms optimized for high latency
//! - **Regional Leader Election**: Geographic leader distribution
//! - **Cross-Region Replication**: Efficient data replication across regions
//! - **Regional Failure Domains**: Independent failure domains per region
//! - **Traffic Steering**: Intelligent request routing based on region health
//! - **Compliance Boundaries**: Data residency and sovereignty compliance

pub mod wan_consensus;
pub mod regional_leaders;
pub mod cross_region_replication;
pub mod failure_domains;
pub mod traffic_steering;
pub mod compliance_boundaries;

pub use wan_consensus::WANConsensus;
pub use regional_leaders::RegionalLeaderManager;
pub use cross_region_replication::CrossRegionReplicator;
pub use failure_domains::FailureDomainManager;
pub use traffic_steering::TrafficSteerer;
pub use compliance_boundaries::ComplianceManager;

// UNIQUENESS Research Citations:
// - **WAN Consensus**: Research on consensus over wide-area networks
// - **Geo-Replication**: Google Spanner, CockroachDB research
// - **Regional Architectures**: AWS, Azure multi-region research
// - **Compliance**: GDPR, CCPA data residency research
