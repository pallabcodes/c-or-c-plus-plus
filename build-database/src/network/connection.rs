//! Connection Management
//!
//! Connection pooling, multiplexing, and lifecycle management.
//! Handles client connections with efficient resource utilization.

use crate::core::*;
use super::protocol::*;
use std::collections::HashMap;
use std::sync::Arc;
use parking_lot::RwLock;
use tokio::net::{TcpListener, TcpStream};
use tokio::io::{AsyncReadExt, AsyncWriteExt};

/// Connection configuration
#[derive(Debug, Clone)]
pub struct ConnectionConfig {
    pub host: String,
    pub port: u16,
    pub max_connections: usize,
    pub connection_timeout_ms: u64,
    pub idle_timeout_ms: u64,
    pub buffer_size: usize,
    pub protocol_format: ProtocolFormat,
}

/// Client connection representation
pub struct Connection {
    /// Unique connection ID
    id: u64,
    /// Underlying TCP stream
    stream: TcpStream,
    /// Connection state
    state: ConnectionState,
    /// Protocol handler
    protocol: WireProtocol,
    /// Connection statistics
    stats: ConnectionStats,
    /// Configuration
    config: ConnectionConfig,
}

/// Connection states
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum ConnectionState {
    Connecting,
    Handshaking,
    Authenticated,
    Ready,
    Busy,
    Closing,
    Closed,
}

/// Connection statistics
#[derive(Debug, Clone, Default)]
pub struct ConnectionStats {
    pub bytes_sent: u64,
    pub bytes_received: u64,
    pub messages_sent: u64,
    pub messages_received: u64,
    pub connected_at: u64,
    pub last_activity: u64,
    pub total_queries: u64,
}

impl Connection {
    /// Create a new connection from a TCP stream
    pub async fn new(stream: TcpStream, config: ConnectionConfig) -> Result<Self, ConnectionError> {
        let id = generate_connection_id();
        let protocol = WireProtocol::new(config.protocol_format.clone());

        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_millis() as u64;

        Ok(Self {
            id,
            stream,
            state: ConnectionState::Connecting,
            protocol,
            stats: ConnectionStats {
                connected_at: now,
                last_activity: now,
                ..Default::default()
            },
            config,
        })
    }

    /// Perform connection handshake
    pub async fn handshake(&mut self) -> Result<(), ConnectionError> {
        self.state = ConnectionState::Handshaking;

        // For PostgreSQL protocol, wait for startup message
        if matches!(self.config.protocol_format, ProtocolFormat::PostgreSQL) {
            self.handle_postgresql_startup().await?;
        }

        // For AuroraDB binary protocol, perform custom handshake
        if matches!(self.config.protocol_format, ProtocolFormat::AuroraBinary) {
            self.handle_aurora_handshake().await?;
        }

        self.state = ConnectionState::Authenticated;
        Ok(())
    }

    /// Handle PostgreSQL startup message
    async fn handle_postgresql_startup(&mut self) -> Result<(), ConnectionError> {
        let mut buffer = [0u8; 1024];
        let n = self.stream.read(&mut buffer).await?;
        let startup_data = &buffer[..n];

        // Parse startup message (simplified)
        if startup_data.len() >= 8 {
            let length = u32::from_be_bytes([startup_data[0], startup_data[1], startup_data[2], startup_data[3]]);
            let protocol_version = u32::from_be_bytes([startup_data[4], startup_data[5], startup_data[6], startup_data[7]]);

            if protocol_version == 196608 { // 3.0
                // Send authentication OK and ready for query
                let response = super::protocols::postgresql::PostgreSQLStartupParser::create_startup_response();
                self.stream.write_all(&response).await?;
                self.state = ConnectionState::Ready;
            }
        }

        Ok(())
    }

    /// Handle AuroraDB custom handshake
    async fn handle_aurora_handshake(&mut self) -> Result<(), ConnectionError> {
        // Send server hello
        let hello = b"AURORA\x01\x00\x00"; // Magic + version
        self.stream.write_all(hello).await?;

        // Wait for client response
        let mut response = [0u8; 4];
        self.stream.read_exact(&mut response).await?;

        if &response == b"OK\x00\x00" {
            self.state = ConnectionState::Ready;
        } else {
            return Err(ConnectionError::HandshakeFailed("Invalid client response".to_string()));
        }

        Ok(())
    }

    /// Send a message to the client
    pub async fn send_message(&mut self, message: &AuroraMessage) -> Result<(), ConnectionError> {
        let serialized = self.protocol.serialize(message)?;
        self.stream.write_all(&serialized).await?;

        self.stats.bytes_sent += serialized.len() as u64;
        self.stats.messages_sent += 1;
        self.update_activity();

        Ok(())
    }

    /// Receive a message from the client
    pub async fn receive_message(&mut self) -> Result<AuroraMessage, ConnectionError> {
        let mut buffer = vec![0u8; self.config.buffer_size];

        // Read message header to determine size
        let header_size = match self.config.protocol_format {
            ProtocolFormat::PostgreSQL => 5, // Type + length
            ProtocolFormat::AuroraBinary => 12, // Magic + version + type + length
            ProtocolFormat::HTTP => 1024, // Read until complete HTTP request
            _ => 1024,
        };

        let n = self.stream.read(&mut buffer[..header_size]).await?;
        if n == 0 {
            return Err(ConnectionError::ConnectionClosed);
        }

        let message_size = self.calculate_message_size(&buffer[..n])?;
        let remaining = message_size.saturating_sub(header_size);

        if remaining > 0 {
            if remaining > self.config.buffer_size - header_size {
                return Err(ConnectionError::MessageTooLarge(remaining));
            }
            self.stream.read_exact(&mut buffer[header_size..header_size + remaining]).await?;
        }

        let data = &buffer[..header_size + remaining];
        let message = self.protocol.deserialize(data)?;

        self.stats.bytes_received += data.len() as u64;
        self.stats.messages_received += 1;
        self.update_activity();

        Ok(message)
    }

    /// Calculate expected message size from header
    fn calculate_message_size(&self, header: &[u8]) -> Result<usize, ConnectionError> {
        match self.config.protocol_format {
            ProtocolFormat::PostgreSQL => {
                if header.len() >= 5 {
                    let length = u32::from_be_bytes([header[1], header[2], header[3], header[4]]) as usize;
                    Ok(length + 1) // +1 for type byte
                } else {
                    Err(ConnectionError::InvalidHeader)
                }
            }
            ProtocolFormat::AuroraBinary => {
                if header.len() >= 12 {
                    let payload_len = u32::from_le_bytes([header[6], header[7], header[8], header[9]]) as usize;
                    let metadata_len = u32::from_le_bytes([header[10], header[11], header[12], header[13]]) as usize;
                    Ok(14 + payload_len + metadata_len) // Header + payload + metadata
                } else {
                    Err(ConnectionError::InvalidHeader)
                }
            }
            _ => Ok(1024), // Default buffer size for HTTP
        }
    }

    /// Update last activity timestamp
    fn update_activity(&mut self) {
        self.stats.last_activity = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_millis() as u64;
    }

    /// Check if connection is idle
    pub fn is_idle(&self) -> bool {
        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_millis() as u64;

        now - self.stats.last_activity > self.config.idle_timeout_ms
    }

    /// Get connection statistics
    pub fn stats(&self) -> &ConnectionStats {
        &self.stats
    }

    /// Get connection state
    pub fn state(&self) -> &ConnectionState {
        &self.state
    }

    /// Close the connection
    pub async fn close(&mut self) -> Result<(), ConnectionError> {
        self.state = ConnectionState::Closing;
        self.stream.shutdown().await?;
        self.state = ConnectionState::Closed;
        Ok(())
    }
}

/// Connection operation errors
#[derive(Debug, thiserror::Error)]
pub enum ConnectionError {
    #[error("Connection closed by peer")]
    ConnectionClosed,

    #[error("Handshake failed: {0}")]
    HandshakeFailed(String),

    #[error("Authentication failed: {0}")]
    AuthenticationFailed(String),

    #[error("Message too large: {0} bytes")]
    MessageTooLarge(usize),

    #[error("Invalid message header")]
    InvalidHeader,

    #[error("Protocol error: {0}")]
    ProtocolError(#[from] ProtocolError),

    #[error("I/O error: {0}")]
    IoError(#[from] std::io::Error),

    #[error("Timeout")]
    Timeout,
}

/// Generate unique connection ID
fn generate_connection_id() -> u64 {
    use std::sync::atomic::{AtomicU64, Ordering};
    static COUNTER: AtomicU64 = AtomicU64::new(1);
    COUNTER.fetch_add(1, Ordering::Relaxed)
}
