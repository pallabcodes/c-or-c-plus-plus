//! Consensus Engine for Aurora Coordinator
//!
//! UNIQUENESS: Hybrid Raft/Paxos consensus implementation combining
//! the best of both algorithms for optimal performance.

pub mod hybrid;
pub mod raft;
pub mod paxos;
pub mod state_machine;
pub mod log_manager;

pub use hybrid::HybridConsensus;
pub use raft::{RaftConsensus, RaftNode};
pub use paxos::PaxosConsensus;

use crate::config::ConsensusConfig;
use crate::error::{Error, Result};
use crate::types::NodeId;

/// Main consensus engine - now uses the hybrid implementation
pub struct ConsensusEngine {
    /// The hybrid consensus implementation
    hybrid: hybrid::HybridConsensus,
}

impl ConsensusEngine {
    /// Create new consensus engine with hybrid implementation
    pub async fn new(node_id: NodeId, config: &ConsensusConfig) -> Result<Self> {
        let state_machine = std::sync::Arc::new(state_machine::StateMachine::new());
        let hybrid = hybrid::HybridConsensus::new(node_id, config.clone(), state_machine).await?;

        Ok(Self { hybrid })
    }

    /// Start the consensus engine
    pub async fn start(&self) -> Result<()> {
        self.hybrid.start().await
    }

    /// Stop the consensus engine
    pub async fn stop(&self) -> Result<()> {
        self.hybrid.stop().await
    }

    /// Propose a new log entry
    pub async fn propose(&self, entry: crate::types::LogEntry) -> Result<u64> {
        self.hybrid.propose(entry).await
    }

    /// Get current leader
    pub async fn current_leader(&self) -> Option<NodeId> {
        self.hybrid.current_leader().await
    }

    /// Get current consensus mode
    pub async fn current_mode(&self) -> hybrid::ConsensusMode {
        self.hybrid.current_mode().await
    }

    /// Get current term (from Raft component)
    pub async fn current_term(&self) -> u64 {
        // Access through hybrid consensus metrics
        // In a real implementation, this would be exposed properly
        0 // Placeholder
    }

    /// Get commit index
    pub async fn commit_index(&self) -> u64 {
        // Access through hybrid consensus metrics
        0 // Placeholder
    }

    /// Get consensus metrics
    pub async fn metrics(&self) -> hybrid::HybridMetrics {
        self.hybrid.metrics().await
    }

    /// Force recovery mode
    pub async fn enter_recovery_mode(&self) -> Result<()> {
        self.hybrid.enter_recovery_mode().await
    }
}
