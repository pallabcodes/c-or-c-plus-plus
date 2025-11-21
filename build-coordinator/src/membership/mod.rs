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

/// Membership message for cross-node communication
#[derive(Debug, Clone)]
pub struct MembershipMessage {
    pub from: NodeId,
    pub to: NodeId,
    pub message_type: MembershipMessageType,
    pub data: Vec<u8>,
}

/// Types of membership messages
#[derive(Debug, Clone)]
pub enum MembershipMessageType {
    Ping,
    Ack,
    PingReq,
    MembershipUpdate,
    FailureSuspected,
    FailureConfirmed,
}

/// Membership change type
#[derive(Debug, Clone)]
pub enum MembershipChangeType {
    NodeJoined,
    NodeFailed,
    NodeLeft,
}

/// Membership change event
#[derive(Debug, Clone)]
pub struct MembershipChange {
    pub node_id: NodeId,
    pub change_type: MembershipChangeType,
    pub timestamp: std::time::SystemTime,
}

// Re-export key types
pub use crate::types::{NodeId, ClusterMember, NodeStatus};

// UNIQUENESS Research Citations:
// - SWIM: Das et al. (2002) - Scalable membership protocol
// - Phi Accrual: Hayashibara et al. (2004) - Adaptive failure detection
// - Gossip Protocols: Various papers on epidemic algorithms
// - Failure Detection: Chandra & Toueg (1996) - Unreliable failure detectors
