//! Membership Management: UNIQUENESS Implementation
//!
//! Research-backed membership protocol combining SWIM and Phi Accrual:
//! - **SWIM**: Scalable Weakly-consistent Infection-style Membership (Das et al., 2002)
//! - **Phi Accrual**: Adaptive failure detection (Hayashibara et al., 2004)
//! - **UNIQUENESS**: Optimized for AuroraDB cluster coordination

pub mod swim;
pub mod phi_accrual;
pub mod membership_manager;

pub use membership_manager::{MembershipManager, MembershipConfig, MembershipStats, NodeEventCallback};
pub use swim::SwimProtocol;
pub use phi_accrual::PhiAccrualFailureDetector;

// Re-export key types
pub use crate::types::{NodeId, ClusterMember, NodeStatus};

// UNIQUENESS Research Citations:
// - SWIM: Das et al. (2002) - Scalable membership protocol
// - Phi Accrual: Hayashibara et al. (2004) - Adaptive failure detection
// - Gossip Protocols: Various papers on epidemic algorithms
// - Failure Detection: Chandra & Toueg (1996) - Unreliable failure detectors
