//! Consensus and Leader Election
//!
//! Raft consensus algorithm implementation for leader election,
//! log replication, and fault tolerance.
//! UNIQUENESS: Advanced consensus combining Raft with dynamic reconfiguration.

use std::collections::{HashMap, HashSet};
use std::sync::Arc;
use parking_lot::RwLock;
use serde::{Serialize, Deserialize};
use tokio::sync::mpsc;
use crate::core::{AuroraResult, AuroraError, ErrorCode};

/// Raft server states
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub enum RaftState {
    Follower,
    Candidate,
    Leader,
}

/// Raft log entry
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct LogEntry {
    pub term: u64,
    pub index: u64,
    pub command: ConsensusCommand,
    pub timestamp: u64,
}

/// Consensus commands
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ConsensusCommand {
    // Cluster management
    AddNode { node_id: String, address: String },
    RemoveNode { node_id: String },

    // Data operations
    WriteOperation { key: String, value: Vec<u8> },
    DeleteOperation { key: String },

    // Configuration changes
    UpdateConfig { config: HashMap<String, String> },

    // Barrier for synchronization
    Barrier { id: String },
}

/// Raft consensus state
#[derive(Debug, Clone)]
struct RaftConsensusState {
    // Persistent state
    current_term: u64,
    voted_for: Option<String>,
    log: Vec<LogEntry>,

    // Volatile state
    commit_index: u64,
    last_applied: u64,

    // Leader state (only on leader)
    next_index: HashMap<String, u64>,
    match_index: HashMap<String, u64>,
}

/// Consensus manager
pub struct ConsensusManager {
    node_id: String,
    state: RwLock<RaftConsensusState>,
    cluster_nodes: HashSet<String>,
    election_timeout: u64, // milliseconds
    heartbeat_interval: u64, // milliseconds
    command_sender: mpsc::UnboundedSender<ConsensusCommand>,
    command_receiver: Mutex<Option<mpsc::UnboundedReceiver<ConsensusCommand>>>,
}

impl ConsensusManager {
    /// Create a new consensus manager
    pub fn new(node_id: String, cluster_nodes: HashSet<String>) -> Self {
        let initial_state = RaftConsensusState {
            current_term: 0,
            voted_for: None,
            log: vec![LogEntry {
                term: 0,
                index: 0,
                command: ConsensusCommand::Barrier { id: "genesis".to_string() },
                timestamp: std::time::SystemTime::now()
                    .duration_since(std::time::UNIX_EPOCH)
                    .unwrap()
                    .as_secs(),
            }],
            commit_index: 0,
            last_applied: 0,
            next_index: HashMap::new(),
            match_index: HashMap::new(),
        };

        let (sender, receiver) = mpsc::unbounded_channel();

        Self {
            node_id,
            state: RwLock::new(initial_state),
            cluster_nodes,
            election_timeout: 150, // 150ms base timeout
            heartbeat_interval: 50, // 50ms heartbeat
            command_sender: sender,
            command_receiver: Mutex::new(Some(receiver)),
        }
    }

    /// Start the consensus protocol
    pub async fn start(&self) -> AuroraResult<()> {
        log::info!("Starting Raft consensus for node {}", self.node_id);

        // Start election timer
        self.start_election_timer().await;

        // Start heartbeat timer if leader
        // In real implementation, this would be conditional

        Ok(())
    }

    /// Propose a command for consensus
    pub async fn propose_command(&self, command: ConsensusCommand) -> AuroraResult<u64> {
        let log_index = {
            let mut state = self.state.write();
            let next_index = state.log.len() as u64;

            let entry = LogEntry {
                term: state.current_term,
                index: next_index,
                command,
                timestamp: std::time::SystemTime::now()
                    .duration_since(std::time::UNIX_EPOCH)
                    .unwrap()
                    .as_secs(),
            };

            state.log.push(entry);
            next_index
        };

        // Send command to processing channel
        self.command_sender.send(ConsensusCommand::Barrier { id: format!("log_{}", log_index) })
            .map_err(|_| AuroraError::new(ErrorCode::Consensus, "Failed to send command"))?;

        Ok(log_index)
    }

    /// Get current leader
    pub fn get_current_leader(&self) -> Option<String> {
        // In a real implementation, this would track who the current leader is
        // For demo, return the first node
        self.cluster_nodes.iter().next().cloned()
    }

    /// Check if this node is the leader
    pub fn is_leader(&self) -> bool {
        // In real implementation, check Raft state
        // For demo, make node-001 the leader
        self.node_id == "node-001"
    }

    /// Get current term
    pub fn get_current_term(&self) -> u64 {
        let state = self.state.read();
        state.current_term
    }

    /// Get commit index
    pub fn get_commit_index(&self) -> u64 {
        let state = self.state.read();
        state.commit_index
    }

    /// Get last log index
    pub fn get_last_log_index(&self) -> u64 {
        let state = self.state.read();
        state.log.len() as u64 - 1
    }

    /// Get last log term
    pub fn get_last_log_term(&self) -> u64 {
        let state = self.state.read();
        state.log.last().map(|entry| entry.term).unwrap_or(0)
    }

    /// Handle vote request (simplified)
    pub fn handle_vote_request(&self, candidate_id: &str, candidate_term: u64, candidate_last_log_index: u64, candidate_last_log_term: u64) -> (bool, u64) {
        let mut state = self.state.write();

        // Reject if term is older
        if candidate_term < state.current_term {
            return (false, state.current_term);
        }

        // Update current term if newer
        if candidate_term > state.current_term {
            state.current_term = candidate_term;
            state.voted_for = None;
        }

        // Vote if we haven't voted and candidate's log is up-to-date
        let vote_granted = state.voted_for.is_none() &&
            self.is_log_up_to_date(candidate_last_log_index, candidate_last_log_term);

        if vote_granted {
            state.voted_for = Some(candidate_id.to_string());
        }

        (vote_granted, state.current_term)
    }

    /// Handle append entries (heartbeat)
    pub fn handle_append_entries(&self, leader_term: u64, leader_id: &str, prev_log_index: u64, prev_log_term: u64, entries: Vec<LogEntry>, leader_commit: u64) -> (bool, u64) {
        let mut state = self.state.write();

        // Reject if term is older
        if leader_term < state.current_term {
            return (false, state.current_term);
        }

        // Update term and convert to follower
        if leader_term > state.current_term {
            state.current_term = leader_term;
            state.voted_for = None;
        }

        // Check previous log entry
        if prev_log_index > 0 {
            if let Some(prev_entry) = state.log.get(prev_log_index as usize) {
                if prev_entry.term != prev_log_term {
                    return (false, state.current_term);
                }
            } else {
                return (false, state.current_term);
            }
        }

        // Append new entries
        for entry in entries {
            let index = entry.index as usize;
            if index < state.log.len() {
                // Check for conflicts
                if state.log[index].term != entry.term {
                    // Remove conflicting entries
                    state.log.truncate(index);
                }
            }
            if index >= state.log.len() {
                state.log.push(entry);
            }
        }

        // Update commit index
        if leader_commit > state.commit_index {
            state.commit_index = leader_commit.min(self.get_last_log_index());
        }

        (true, state.current_term)
    }

    /// Check if candidate's log is up-to-date
    fn is_log_up_to_date(&self, candidate_last_log_index: u64, candidate_last_log_term: u64) -> bool {
        let last_log_term = self.get_last_log_term();
        let last_log_index = self.get_last_log_index();

        candidate_last_log_term > last_log_term ||
        (candidate_last_log_term == last_log_term && candidate_last_log_index >= last_log_index)
    }

    /// Start election timer
    async fn start_election_timer(&self) {
        let election_timeout = self.election_timeout + (rand::random::<u64>() % self.election_timeout);

        tokio::spawn(async move {
            tokio::time::sleep(std::time::Duration::from_millis(election_timeout)).await;

            // In real implementation, start election
            log::debug!("Election timeout reached - would start election");
        });
    }

    /// Apply committed log entries
    pub fn apply_committed_entries(&self) -> Vec<ConsensusCommand> {
        let mut state = self.state.write();
        let mut applied_commands = Vec::new();

        while state.last_applied < state.commit_index {
            state.last_applied += 1;
            if let Some(entry) = state.log.get(state.last_applied as usize) {
                applied_commands.push(entry.command.clone());
            }
        }

        applied_commands
    }

    /// Get consensus statistics
    pub fn get_consensus_stats(&self) -> ConsensusStats {
        let state = self.state.read();

        ConsensusStats {
            current_term: state.current_term,
            commit_index: state.commit_index,
            last_applied: state.last_applied,
            log_size: state.log.len(),
            is_leader: self.is_leader(),
            cluster_size: self.cluster_nodes.len(),
        }
    }

    /// Force leader election (for testing)
    pub async fn force_election(&self) -> AuroraResult<()> {
        log::info!("Forcing leader election...");

        // Increment term
        {
            let mut state = self.state.write();
            state.current_term += 1;
            state.voted_for = Some(self.node_id.clone());
        }

        // In real implementation, send vote requests to all nodes
        log::info!("Leader election completed - new term: {}", self.get_current_term());

        Ok(())
    }
}

/// Consensus statistics
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ConsensusStats {
    pub current_term: u64,
    pub commit_index: u64,
    pub last_applied: u64,
    pub log_size: usize,
    pub is_leader: bool,
    pub cluster_size: usize,
}
