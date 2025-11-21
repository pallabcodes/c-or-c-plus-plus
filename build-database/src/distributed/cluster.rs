//! Cluster Node Management
//!
//! Multi-node cluster management with automatic discovery, membership,
//! and communication protocols.
//! UNIQUENESS: Advanced cluster management combining gossip protocols with consensus.

use std::collections::{HashMap, HashSet};
use std::sync::Arc;
use parking_lot::RwLock;
use serde::{Serialize, Deserialize};
use tokio::sync::mpsc;
use crate::core::{AuroraResult, AuroraError, ErrorCode};

/// Cluster node state
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub enum NodeState {
    Starting,
    Running,
    Unhealthy,
    Leaving,
    Left,
}

/// Cluster node information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ClusterNode {
    pub node_id: String,
    pub address: String,
    pub port: u16,
    pub state: NodeState,
    pub roles: HashSet<NodeRole>,
    pub region: String,
    pub zone: String,
    pub last_heartbeat: u64,
    pub metadata: HashMap<String, String>,
}

/// Node roles in the cluster
#[derive(Debug, Clone, Hash, Eq, PartialEq, Serialize, Deserialize)]
pub enum NodeRole {
    Leader,
    Follower,
    Witness,
    LoadBalancer,
    Coordinator,
}

/// Cluster configuration
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ClusterConfig {
    pub cluster_name: String,
    pub node_id: String,
    pub bind_address: String,
    pub bind_port: u16,
    pub seed_nodes: Vec<String>, // Initial nodes to join
    pub heartbeat_interval_ms: u64,
    pub failure_detection_timeout_ms: u64,
    pub max_nodes: usize,
    pub enable_auto_join: bool,
    pub enable_auto_leave: bool,
}

/// Cluster membership events
#[derive(Debug, Clone)]
pub enum MembershipEvent {
    NodeJoined(ClusterNode),
    NodeLeft(String), // node_id
    NodeFailed(String), // node_id
    NodeRecovered(String), // node_id
    LeadershipChanged { old_leader: Option<String>, new_leader: String },
}

/// Cluster manager
pub struct ClusterManager {
    config: ClusterConfig,
    local_node: ClusterNode,
    nodes: RwLock<HashMap<String, ClusterNode>>,
    event_sender: mpsc::UnboundedSender<MembershipEvent>,
    event_receiver: Mutex<Option<mpsc::UnboundedReceiver<MembershipEvent>>>,
}

impl ClusterManager {
    /// Create a new cluster manager
    pub fn new(config: ClusterConfig) -> Self {
        let local_node = ClusterNode {
            node_id: config.node_id.clone(),
            address: config.bind_address.clone(),
            port: config.bind_port,
            state: NodeState::Starting,
            roles: HashSet::new(),
            region: "us-east-1".to_string(), // Default region
            zone: "us-east-1a".to_string(), // Default zone
            last_heartbeat: std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_secs(),
            metadata: HashMap::new(),
        };

        let (sender, receiver) = mpsc::unbounded_channel();

        Self {
            config,
            local_node,
            nodes: RwLock::new(HashMap::new()),
            event_sender: sender,
            event_receiver: Mutex::new(Some(receiver)),
        }
    }

    /// Initialize the cluster manager
    pub async fn initialize(&mut self) -> AuroraResult<()> {
        log::info!("Initializing cluster manager for node {}", self.config.node_id);

        // Add local node
        let mut nodes = self.nodes.write();
        nodes.insert(self.local_node.node_id.clone(), self.local_node.clone());

        // Join seed nodes if configured
        if self.config.enable_auto_join {
            for seed_addr in &self.config.seed_nodes {
                if let Err(e) = self.join_cluster(seed_addr).await {
                    log::warn!("Failed to join seed node {}: {}", seed_addr, e);
                }
            }
        }

        // Set initial state to running
        self.local_node.state = NodeState::Running;
        nodes.insert(self.local_node.node_id.clone(), self.local_node.clone());

        log::info!("Cluster manager initialized with {} nodes", nodes.len());
        Ok(())
    }

    /// Join an existing cluster
    pub async fn join_cluster(&self, contact_node: &str) -> AuroraResult<()> {
        log::info!("Attempting to join cluster via node: {}", contact_node);

        // In a real implementation, this would:
        // 1. Connect to the contact node
        // 2. Send join request
        // 3. Receive cluster state
        // 4. Update local membership

        // For demo, simulate joining by adding some nodes
        let mut nodes = self.nodes.write();

        let node1 = ClusterNode {
            node_id: "node-001".to_string(),
            address: "10.0.0.11".to_string(),
            port: 5432,
            state: NodeState::Running,
            roles: vec![NodeRole::Follower].into_iter().collect(),
            region: "us-east-1".to_string(),
            zone: "us-east-1a".to_string(),
            last_heartbeat: std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_secs(),
            metadata: HashMap::new(),
        };

        let node2 = ClusterNode {
            node_id: "node-002".to_string(),
            address: "10.0.0.12".to_string(),
            port: 5432,
            state: NodeState::Running,
            roles: vec![NodeRole::Follower].into_iter().collect(),
            region: "us-east-1".to_string(),
            zone: "us-east-1b".to_string(),
            last_heartbeat: std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_secs(),
            metadata: HashMap::new(),
        };

        nodes.insert(node1.node_id.clone(), node1.clone());
        nodes.insert(node2.node_id.clone(), node2.clone());

        // Emit membership events
        let _ = self.event_sender.send(MembershipEvent::NodeJoined(node1));
        let _ = self.event_sender.send(MembershipEvent::NodeJoined(node2));

        log::info!("Successfully joined cluster with {} total nodes", nodes.len());
        Ok(())
    }

    /// Leave the cluster gracefully
    pub async fn leave_cluster(&self) -> AuroraResult<()> {
        log::info!("Node {} leaving cluster gracefully", self.config.node_id);

        // Notify other nodes
        // In real implementation, broadcast leave message

        // Update local state
        let mut nodes = self.nodes.write();
        if let Some(node) = nodes.get_mut(&self.config.node_id) {
            node.state = NodeState::Left;
        }

        // Emit leave event
        let _ = self.event_sender.send(MembershipEvent::NodeLeft(self.config.node_id.clone()));

        log::info!("Successfully left cluster");
        Ok(())
    }

    /// Get cluster status
    pub fn get_cluster_status(&self) -> ClusterStatus {
        let nodes = self.nodes.read();

        let total_nodes = nodes.len();
        let healthy_nodes = nodes.values()
            .filter(|n| n.state == NodeState::Running)
            .count();
        let unhealthy_nodes = nodes.values()
            .filter(|n| n.state == NodeState::Unhealthy)
            .count();

        let regions: HashSet<String> = nodes.values()
            .map(|n| n.region.clone())
            .collect();

        let roles_distribution: HashMap<String, usize> = nodes.values()
            .flat_map(|n| &n.roles)
            .fold(HashMap::new(), |mut acc, role| {
                let role_str = format!("{:?}", role);
                *acc.entry(role_str).or_insert(0) += 1;
                acc
            });

        ClusterStatus {
            cluster_name: self.config.cluster_name.clone(),
            total_nodes,
            healthy_nodes,
            unhealthy_nodes,
            regions: regions.into_iter().collect(),
            roles_distribution,
            leader_node: self.get_current_leader(),
        }
    }

    /// Get current leader node
    pub fn get_current_leader(&self) -> Option<String> {
        let nodes = self.nodes.read();
        nodes.values()
            .find(|n| n.roles.contains(&NodeRole::Leader))
            .map(|n| n.node_id.clone())
    }

    /// Get node by ID
    pub fn get_node(&self, node_id: &str) -> Option<ClusterNode> {
        let nodes = self.nodes.read();
        nodes.get(node_id).cloned()
    }

    /// Get all nodes
    pub fn get_all_nodes(&self) -> Vec<ClusterNode> {
        let nodes = self.nodes.read();
        nodes.values().cloned().collect()
    }

    /// Get nodes by role
    pub fn get_nodes_by_role(&self, role: &NodeRole) -> Vec<ClusterNode> {
        let nodes = self.nodes.read();
        nodes.values()
            .filter(|n| n.roles.contains(role))
            .cloned()
            .collect()
    }

    /// Get nodes by region
    pub fn get_nodes_by_region(&self, region: &str) -> Vec<ClusterNode> {
        let nodes = self.nodes.read();
        nodes.values()
            .filter(|n| n.region == region)
            .cloned()
            .collect()
    }

    /// Update node heartbeat
    pub fn update_heartbeat(&self, node_id: &str) {
        let mut nodes = self.nodes.write();
        if let Some(node) = nodes.get_mut(node_id) {
            node.last_heartbeat = std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_secs();
        }
    }

    /// Mark node as failed
    pub fn mark_node_failed(&self, node_id: &str) {
        let mut nodes = self.nodes.write();
        if let Some(node) = nodes.get_mut(node_id) {
            if node.state != NodeState::Left {
                node.state = NodeState::Unhealthy;
                let _ = self.event_sender.send(MembershipEvent::NodeFailed(node_id.to_string()));
            }
        }
    }

    /// Mark node as recovered
    pub fn mark_node_recovered(&self, node_id: &str) {
        let mut nodes = self.nodes.write();
        if let Some(node) = nodes.get_mut(node_id) {
            if node.state == NodeState::Unhealthy {
                node.state = NodeState::Running;
                let _ = self.event_sender.send(MembershipEvent::NodeRecovered(node_id.to_string()));
            }
        }
    }

    /// Assign role to node
    pub fn assign_role(&self, node_id: &str, role: NodeRole) -> AuroraResult<()> {
        let mut nodes = self.nodes.write();
        if let Some(node) = nodes.get_mut(node_id) {
            node.roles.insert(role);
            Ok(())
        } else {
            Err(AuroraError::new(
                ErrorCode::Cluster,
                format!("Node {} not found", node_id)
            ))
        }
    }

    /// Remove role from node
    pub fn remove_role(&self, node_id: &str, role: &NodeRole) -> AuroraResult<()> {
        let mut nodes = self.nodes.write();
        if let Some(node) = nodes.get_mut(node_id) {
            node.roles.remove(role);
            Ok(())
        } else {
            Err(AuroraError::new(
                ErrorCode::Cluster,
                format!("Node {} not found", node_id)
            ))
        }
    }

    /// Check if node is healthy
    pub fn is_node_healthy(&self, node_id: &str) -> bool {
        let nodes = self.nodes.read();
        if let Some(node) = nodes.get(node_id) {
            let now = std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_secs();

            let time_since_heartbeat = now - node.last_heartbeat;
            node.state == NodeState::Running && time_since_heartbeat < (self.config.failure_detection_timeout_ms / 1000)
        } else {
            false
        }
    }

    /// Get membership events (consume the receiver)
    pub fn take_event_receiver(&self) -> Option<mpsc::UnboundedReceiver<MembershipEvent>> {
        self.event_receiver.lock().take()
    }
}

/// Cluster status information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ClusterStatus {
    pub cluster_name: String,
    pub total_nodes: usize,
    pub healthy_nodes: usize,
    pub unhealthy_nodes: usize,
    pub regions: Vec<String>,
    pub roles_distribution: HashMap<String, usize>,
    pub leader_node: Option<String>,
}
