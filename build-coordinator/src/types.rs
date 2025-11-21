//! Common types and data structures for Aurora Coordinator
//!
//! UNIQUENESS: Research-backed type design with memory-safe primitives
//! and distributed systems optimizations.

use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::fmt;

/// Node identifier in the distributed cluster
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub struct NodeId(pub u64);

impl fmt::Display for NodeId {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "node-{}", self.0)
    }
}

impl From<u64> for NodeId {
    fn from(id: u64) -> Self {
        Self(id)
    }
}

/// Cluster member information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ClusterMember {
    /// Unique node identifier
    pub node_id: NodeId,
    
    /// Human-readable node name
    pub name: String,
    
    /// Node address for communication
    pub address: String,
    
    /// Node role in the cluster
    pub role: NodeRole,
    
    /// Node status
    pub status: NodeStatus,
    
    /// Last heartbeat timestamp
    pub last_heartbeat: std::time::SystemTime,
    
    /// Node capabilities (AuroraDB, Cyclone, etc.)
    pub capabilities: NodeCapabilities,
}

/// Node roles in the cluster
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub enum NodeRole {
    /// Leader node (handles coordination)
    Leader,
    
    /// Follower node (participates in consensus)
    Follower,
    
    /// Candidate node (seeking leadership)
    Candidate,
    
    /// Learner node (receives but doesn't vote)
    Learner,
    
    /// AuroraDB node (database server)
    AuroraDb,
    
    /// Load balancer node
    LoadBalancer,
}

/// Node status
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub enum NodeStatus {
    /// Node is healthy and participating
    Healthy,
    
    /// Node is suspected of failure
    Suspected,
    
    /// Node has failed and is unreachable
    Failed,
    
    /// Node is recovering from failure
    Recovering,
    
    /// Node is being decommissioned
    Decommissioned,
}

/// Node capabilities (UNIQUENESS: Multi-system integration)
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct NodeCapabilities {
    /// Supports AuroraDB operations
    pub aurora_db: bool,
    
    /// Supports Cyclone networking
    pub cyclone_networking: bool,
    
    /// Supports RDMA communication
    pub rdma_support: bool,
    
    /// Supports DPDK acceleration
    pub dpdk_support: bool,
    
    /// Available CPU cores
    pub cpu_cores: usize,
    
    /// Available memory (MB)
    pub memory_mb: usize,
    
    /// Storage capacity (GB)
    pub storage_gb: usize,
}

/// Consensus log entry
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct LogEntry {
    /// Log entry index
    pub index: u64,
    
    /// Term when entry was created
    pub term: u64,
    
    /// Log entry data
    pub data: LogData,
    
    /// Timestamp of entry creation
    pub timestamp: std::time::SystemTime,
}

/// Types of data that can be stored in consensus log
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum LogData {
    /// Cluster configuration change
    ConfigChange(ConfigChange),
    
    /// AuroraDB schema change
    SchemaChange(SchemaChange),
    
    /// Transaction coordination
    Transaction(TransactionEntry),
    
    /// Node heartbeat
    Heartbeat(HeartbeatData),
    
    /// Custom application data
    Custom(Vec<u8>),
}

/// Configuration changes to the cluster
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ConfigChange {
    /// Type of configuration change
    pub change_type: ConfigChangeType,
    
    /// Node affected by change
    pub node_id: NodeId,
    
    /// Additional change data
    pub data: HashMap<String, String>,
}

/// Types of configuration changes
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ConfigChangeType {
    /// Add new node to cluster
    AddNode,
    
    /// Remove node from cluster
    RemoveNode,
    
    /// Update node configuration
    UpdateNode,
    
    /// Change cluster settings
    ClusterConfig,
}

/// AuroraDB schema changes
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SchemaChange {
    /// Database name
    pub database: String,
    
    /// Schema change operation
    pub operation: SchemaOperation,
    
    /// SQL statement for the change
    pub sql: String,
    
    /// Transaction ID if part of larger transaction
    pub transaction_id: Option<String>,
}

/// Schema operations
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum SchemaOperation {
    /// Create table
    CreateTable,
    
    /// Alter table
    AlterTable,
    
    /// Drop table
    DropTable,
    
    /// Create index
    CreateIndex,
    
    /// Drop index
    DropIndex,
}

/// Transaction coordination entry
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TransactionEntry {
    /// Global transaction ID
    pub transaction_id: String,
    
    /// Transaction state
    pub state: TransactionState,
    
    /// Participating nodes
    pub participants: Vec<NodeId>,
    
    /// Transaction timeout
    pub timeout: std::time::Duration,
}

/// Transaction states for coordination
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum TransactionState {
    /// Transaction starting
    Starting,
    
    /// Transaction prepared (2PC phase 1 complete)
    Prepared,
    
    /// Transaction committing
    Committing,
    
    /// Transaction committed
    Committed,
    
    /// Transaction aborting
    Aborting,
    
    /// Transaction aborted
    Aborted,
}

/// Heartbeat data for node liveness
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct HeartbeatData {
    /// Node sending heartbeat
    pub node_id: NodeId,
    
    /// Current term
    pub term: u64,
    
    /// Commit index
    pub commit_index: u64,
    
    /// Last applied index
    pub last_applied: u64,
}

/// AuroraDB cluster information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AuroraCluster {
    /// Cluster name
    pub name: String,
    
    /// Leader node for coordination
    pub leader: Option<NodeId>,
    
    /// All cluster members
    pub members: HashMap<NodeId, ClusterMember>,
    
    /// Current term
    pub term: u64,
    
    /// Commit index
    pub commit_index: u64,
    
    /// Cluster configuration version
    pub config_version: u64,
}

/// Query routing information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct QueryRoute {
    /// Target node for query execution
    pub target_node: NodeId,
    
    /// Query execution priority
    pub priority: QueryPriority,
    
    /// Estimated execution time
    pub estimated_time: std::time::Duration,
    
    /// Load balancing hint
    pub load_hint: f64,
}

/// Query execution priorities
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum QueryPriority {
    /// High priority (real-time)
    High,
    
    /// Normal priority (standard queries)
    Normal,
    
    /// Low priority (background tasks)
    Low,
    
    /// Bulk operations
    Bulk,
}

/// Performance metrics for monitoring
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PerformanceMetrics {
    /// Timestamp of metrics collection
    pub timestamp: std::time::SystemTime,
    
    /// Node ID reporting metrics
    pub node_id: NodeId,
    
    /// CPU usage percentage
    pub cpu_usage: f64,
    
    /// Memory usage percentage
    pub memory_usage: f64,
    
    /// Network I/O rates (bytes/sec)
    pub network_rx_rate: f64,
    pub network_tx_rate: f64,
    
    /// Request latency percentiles (microseconds)
    pub latency_p50: u64,
    pub latency_p95: u64,
    pub latency_p99: u64,
    
    /// Active connections
    pub active_connections: usize,
    
    /// Queue depths
    pub request_queue_depth: usize,
    pub response_queue_depth: usize,
}

// UNIQUENESS Validation: Type Design
// - [x] Memory-safe types (no unsafe pointers)
// - [x] Research-backed data structures (log-structured for consensus)
// - [x] Comprehensive type coverage for distributed coordination
// - [x] AuroraDB + Cyclone integration types
// - [x] Linux kernel inspired performance metrics
