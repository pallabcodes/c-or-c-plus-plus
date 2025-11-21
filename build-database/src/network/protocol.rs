//! Wire Protocol Implementation
//!
//! AuroraDB wire protocol supporting multiple formats:
//! - PostgreSQL-compatible for seamless migration
//! - Custom binary protocol for performance
//! - HTTP/JSON for web applications
//! - gRPC for distributed communication

use crate::core::*;
use std::collections::HashMap;
use super::protocols::*;

/// Wire protocol abstraction supporting multiple formats
pub struct WireProtocol {
    /// Protocol format
    format: ProtocolFormat,
    /// Protocol version
    version: ProtocolVersion,
    /// Message serializers for different formats
    serializers: HashMap<ProtocolFormat, Box<dyn MessageSerializer>>,
}

/// Supported protocol formats
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum ProtocolFormat {
    /// PostgreSQL wire protocol compatible
    PostgreSQL,
    /// MySQL client protocol compatible
    MySQL,
    /// Custom binary protocol for performance
    AuroraBinary,
    /// HTTP/JSON for web applications
    HTTP,
    /// gRPC for distributed communication
    GRPC,
}

/// Protocol version for compatibility
#[derive(Debug, Clone)]
pub struct ProtocolVersion {
    pub major: u16,
    pub minor: u16,
    pub patch: u16,
}

impl Default for ProtocolVersion {
    fn default() -> Self {
        Self {
            major: 1,
            minor: 0,
            patch: 0,
        }
    }
}

/// Message types in the wire protocol
#[derive(Debug, Clone)]
pub enum MessageType {
    // Connection messages
    Startup,
    Authentication,
    SSLRequest,
    CancelRequest,

    // Query messages
    Query,
    Parse,
    Bind,
    Execute,
    Describe,
    Close,

    // Response messages
    RowDescription,
    DataRow,
    CommandComplete,
    ReadyForQuery,
    ErrorResponse,
    NoticeResponse,

    // Transaction messages
    Begin,
    Commit,
    Rollback,

    // Extended protocol
    FunctionCall,
    CopyData,
    CopyDone,
    CopyFail,

    // Custom AuroraDB messages
    VectorQuery,
    AnalyticsQuery,
    BulkLoad,
    StreamResponse,
}

/// AuroraDB protocol message
#[derive(Debug, Clone)]
pub struct AuroraMessage {
    pub message_type: MessageType,
    pub payload: Vec<u8>,
    pub metadata: HashMap<String, String>,
}

/// Message serialization trait
pub trait MessageSerializer: Send + Sync {
    fn serialize(&self, message: &AuroraMessage) -> Result<Vec<u8>, ProtocolError>;
    fn deserialize(&self, data: &[u8]) -> Result<AuroraMessage, ProtocolError>;
}

/// Protocol operation errors
#[derive(Debug, thiserror::Error)]
pub enum ProtocolError {
    #[error("Serialization error: {0}")]
    SerializationError(String),

    #[error("Deserialization error: {0}")]
    DeserializationError(String),

    #[error("Protocol version mismatch: expected {expected}, got {actual}")]
    VersionMismatch { expected: String, actual: String },

    #[error("Invalid message type: {0}")]
    InvalidMessageType(u8),

    #[error("Message too large: {size} bytes (max {max})")]
    MessageTooLarge { size: usize, max: usize },

    #[error("Authentication failed: {reason}")]
    AuthenticationFailed { reason: String },

    #[error("Connection error: {0}")]
    ConnectionError(String),
}

impl WireProtocol {
    /// Create a new wire protocol handler
    pub fn new(format: ProtocolFormat) -> Self {
        let mut serializers = HashMap::new();

        // Initialize serializers for different formats
        serializers.insert(ProtocolFormat::PostgreSQL, Box::new(PostgreSQLSerializer::new()) as Box<dyn MessageSerializer>);
        serializers.insert(ProtocolFormat::AuroraBinary, Box::new(AuroraBinarySerializer::new()) as Box<dyn MessageSerializer>);
        serializers.insert(ProtocolFormat::HTTP, Box::new(HTTPSerializer::new()) as Box<dyn MessageSerializer>);

        Self {
            format,
            version: ProtocolVersion::default(),
            serializers,
        }
    }

    /// Serialize a message for transmission
    pub fn serialize(&self, message: &AuroraMessage) -> Result<Vec<u8>, ProtocolError> {
        if let Some(serializer) = self.serializers.get(&self.format) {
            serializer.serialize(message)
        } else {
            Err(ProtocolError::SerializationError(
                format!("No serializer for protocol format {:?}", self.format)
            ))
        }
    }

    /// Deserialize a received message
    pub fn deserialize(&self, data: &[u8]) -> Result<AuroraMessage, ProtocolError> {
        if let Some(serializer) = self.serializers.get(&self.format) {
            serializer.deserialize(data)
        } else {
            Err(ProtocolError::DeserializationError(
                format!("No deserializer for protocol format {:?}", self.format)
            ))
        }
    }

    /// Create a query message
    pub fn create_query_message(&self, sql: &str) -> AuroraMessage {
        AuroraMessage {
            message_type: MessageType::Query,
            payload: sql.as_bytes().to_vec(),
            metadata: HashMap::new(),
        }
    }

    /// Create a response message
    pub fn create_response_message(&self, data: Vec<u8>) -> AuroraMessage {
        AuroraMessage {
            message_type: MessageType::DataRow,
            payload: data,
            metadata: HashMap::new(),
        }
    }

    /// Create an error message
    pub fn create_error_message(&self, error: &str) -> AuroraMessage {
        let mut metadata = HashMap::new();
        metadata.insert("error".to_string(), error.to_string());

        AuroraMessage {
            message_type: MessageType::ErrorResponse,
            payload: Vec::new(),
            metadata,
        }
    }

    /// Negotiate protocol version with client
    pub fn negotiate_version(&mut self, client_version: ProtocolVersion) -> Result<(), ProtocolError> {
        // For now, accept any version (in production, check compatibility)
        if client_version.major > self.version.major {
            return Err(ProtocolError::VersionMismatch {
                expected: format!("{}.{}.{}", self.version.major, self.version.minor, self.version.patch),
                actual: format!("{}.{}.{}", client_version.major, client_version.minor, client_version.patch),
            });
        }
        Ok(())
    }
}