//! Network Protocol Layer
//!
//! Client/server communication and distributed protocols:
//! - Wire protocol compatible with PostgreSQL/MySQL clients
//! - Connection pooling and multiplexing
//! - Distributed consensus and replication protocols
//! - Network optimization and compression
//!
//! UNIQUENESS: Fuses PostgreSQL wire protocol + gRPC streaming + RDMA optimization
//! Research: Zero-copy networking + protocol buffers + consensus algorithms

pub mod protocol;
pub mod server;
pub mod client;
pub mod connection;
pub mod pooling;
pub mod distributed;

// Re-export main network components
pub use protocol::{WireProtocol, MessageType, AuroraMessage};
pub use server::AuroraServer;
pub use client::AuroraClient;
pub use connection::{Connection, ConnectionConfig};
pub use pooling::ConnectionPool;
pub use distributed::{ConsensusProtocol, ReplicationProtocol};
