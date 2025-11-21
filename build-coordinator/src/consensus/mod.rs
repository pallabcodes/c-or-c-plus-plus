//! Consensus Module: UNIQUENESS Implementation
//!
//! This module implements a hybrid consensus algorithm combining the best of Raft and Paxos:
//! - **Raft**: Leader election, log replication, safety properties
//! - **Paxos**: Multi-Paxos for efficient steady-state operation
//! - **UNIQUENESS**: Research-backed optimizations for high performance

pub mod raft;
pub mod paxos;
pub mod hybrid;
pub mod state_machine;
pub mod log_manager;

pub use hybrid::HybridConsensus;
pub use raft::{RaftConsensus, RaftNode};
pub use paxos::{PaxosConsensus, PaxosInstance};
pub use state_machine::StateMachine;
pub use log_manager::LogManager;

// Re-export key types
pub use crate::types::{LogEntry, NodeId};

// Type aliases for consensus operations
pub type LogIndex = u64;
pub type Term = u64;

// UNIQUENESS Research Citations:
// - Raft: Ongaro & Ousterhout (2014) - Understandable consensus algorithm
// - Paxos: Lamport (1998, 2001) - Fault-tolerant consensus foundation
// - Multi-Paxos: Lamport (2001) - Efficient steady-state operation
// - Hybrid Approaches: Various papers on combining Paxos/Raft strengths
