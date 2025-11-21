//! Cyclone Networking Integration: UNIQUENESS High-Performance Communication
//!
//! Research-backed networking stack for distributed coordination:
//! - **RDMA**: Remote Direct Memory Access for microsecond latency
//! - **DPDK**: User-space networking acceleration
//! - **Zero-Copy**: Scatter-gather I/O with buffer management
//! - **XDP/eBPF**: Kernel-bypass packet processing

pub mod network_layer;
pub mod rdma_transport;
pub mod dpdk_acceleration;
pub mod zero_copy_messaging;
pub mod message_router;

pub use network_layer::{NetworkLayer, NetworkConfig, ConnectionType};
pub use rdma_transport::RDMATransport;
pub use dpdk_acceleration::DPDKAccelerator;
pub use zero_copy_messaging::{ZeroCopyMessenger, MessageBuffer};
pub use message_router::MessageRouter;

// Re-export key types for AuroraDB coordination
pub use crate::types::{NodeId, ClusterMember};

// UNIQUENESS Research Citations:
// - RDMA: "High Performance RDMA-Based MPI Implementation" (Liu et al., 2003)
// - DPDK: Intel DPDK - User-space networking acceleration
// - Zero-Copy: "Zero-Copy TCP" (Druschel & Banga, 1996)
// - XDP: Linux kernel XDP/eBPF for programmable networking
