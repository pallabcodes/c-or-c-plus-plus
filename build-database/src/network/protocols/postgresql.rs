//! PostgreSQL-Compatible Wire Protocol
//!
//! Implements PostgreSQL frontend/backend protocol for seamless client compatibility.
//! Supports standard SQL clients like psql, JDBC, and other PostgreSQL drivers.

use crate::network::protocol::*;
use std::collections::HashMap;

/// PostgreSQL-compatible serializer
pub struct PostgreSQLSerializer;

impl PostgreSQLSerializer {
    pub fn new() -> Self {
        Self
    }
}

impl MessageSerializer for PostgreSQLSerializer {
    fn serialize(&self, message: &AuroraMessage) -> Result<Vec<u8>, ProtocolError> {
        let mut buffer = Vec::new();

        // PostgreSQL message format: Type (1 byte) + Length (4 bytes) + Payload
        match message.message_type {
            MessageType::Query => {
                buffer.push(b'Q'); // Query message type
                let payload_len = (message.payload.len() + 4) as u32;
                buffer.extend_from_slice(&payload_len.to_be_bytes());
                buffer.extend_from_slice(&message.payload);
                buffer.push(0); // Null terminator
            }
            MessageType::DataRow => {
                buffer.push(b'D'); // DataRow message type
                let payload_len = (message.payload.len() + 4) as u32;
                buffer.extend_from_slice(&payload_len.to_be_bytes());
                buffer.extend_from_slice(&message.payload);
            }
            MessageType::ErrorResponse => {
                buffer.push(b'E'); // Error message type
                let error_msg = message.metadata.get("error").unwrap_or(&"Unknown error".to_string());
                let payload = format!("ERROR: {}", error_msg);
                let payload_len = (payload.len() + 4 + 1) as u32; // +1 for null terminator
                buffer.extend_from_slice(&payload_len.to_be_bytes());
                buffer.extend_from_slice(payload.as_bytes());
                buffer.push(0); // Null terminator
            }
            MessageType::CommandComplete => {
                buffer.push(b'C'); // CommandComplete message type
                let tag = b"SELECT"; // Placeholder
                let payload_len = (tag.len() + 4 + 1) as u32;
                buffer.extend_from_slice(&payload_len.to_be_bytes());
                buffer.extend_from_slice(tag);
                buffer.push(0);
            }
            MessageType::ReadyForQuery => {
                buffer.push(b'Z'); // ReadyForQuery message type
                buffer.extend_from_slice(&5u32.to_be_bytes()); // Length
                buffer.push(b'I'); // Idle status
            }
            MessageType::RowDescription => {
                buffer.push(b'T'); // RowDescription message type
                // Placeholder: field count = 0
                buffer.extend_from_slice(&6u32.to_be_bytes()); // Length
                buffer.extend_from_slice(&0u16.to_be_bytes()); // Field count
            }
            _ => {
                return Err(ProtocolError::SerializationError(
                    format!("Unsupported message type for PostgreSQL protocol: {:?}", message.message_type)
                ));
            }
        }

        Ok(buffer)
    }

    fn deserialize(&self, data: &[u8]) -> Result<AuroraMessage, ProtocolError> {
        if data.is_empty() {
            return Err(ProtocolError::DeserializationError("Empty message".to_string()));
        }

        let message_type = match data[0] {
            b'Q' => MessageType::Query,
            b'P' => MessageType::Parse,
            b'B' => MessageType::Bind,
            b'E' => MessageType::Execute,
            b'D' => MessageType::Describe,
            b'C' => MessageType::Close,
            b'S' => MessageType::FunctionCall, // Sync
            b'H' => MessageType::CopyData, // Copy
            _ => {
                return Err(ProtocolError::InvalidMessageType(data[0]));
            }
        };

        // For PostgreSQL protocol, extract payload (skip type byte and length)
        let payload = if data.len() > 5 {
            data[5..].to_vec()
        } else {
            Vec::new()
        };

        Ok(AuroraMessage {
            message_type,
            payload,
            metadata: HashMap::new(),
        })
    }
}

/// PostgreSQL startup message parser
pub struct PostgreSQLStartupParser;

impl PostgreSQLStartupParser {
    pub fn parse_startup(data: &[u8]) -> Result<HashMap<String, String>, ProtocolError> {
        if data.len() < 8 {
            return Err(ProtocolError::DeserializationError("Startup message too short".to_string()));
        }

        let length = u32::from_be_bytes([data[0], data[1], data[2], data[3]]) as usize;
        let protocol_version = u32::from_be_bytes([data[4], data[5], data[6], data[7]]);

        if protocol_version == 196608 { // 3.0
            // Parse parameters (null-terminated strings)
            let mut params = HashMap::new();
            let mut pos = 8;

            while pos + 1 < length {
                // Find null terminator for key
                let key_end = data[pos..].iter().position(|&b| b == 0)
                    .ok_or_else(|| ProtocolError::DeserializationError("Invalid startup parameters".to_string()))?;
                let key = String::from_utf8_lossy(&data[pos..pos + key_end]);

                pos += key_end + 1;

                // Find null terminator for value
                let value_end = data[pos..].iter().position(|&b| b == 0)
                    .ok_or_else(|| ProtocolError::DeserializationError("Invalid startup parameters".to_string()))?;
                let value = String::from_utf8_lossy(&data[pos..pos + value_end]);

                params.insert(key.to_string(), value.to_string());
                pos += value_end + 1;

                if pos >= length {
                    break;
                }
            }

            Ok(params)
        } else {
            Err(ProtocolError::VersionMismatch {
                expected: "3.0".to_string(),
                actual: format!("{}.{}", protocol_version >> 16, protocol_version & 0xFFFF),
            })
        }
    }

    pub fn create_startup_response() -> Vec<u8> {
        let mut buffer = Vec::new();
        buffer.push(b'R'); // AuthenticationOk
        buffer.extend_from_slice(&8u32.to_be_bytes()); // Length
        buffer.extend_from_slice(&0u32.to_be_bytes()); // Success

        buffer.push(b'S'); // ParameterStatus
        let param = b"application_name\0AuroraDB\0";
        let param_len = (param.len() + 4) as u32;
        buffer.extend_from_slice(&param_len.to_be_bytes());
        buffer.extend_from_slice(param);

        buffer.push(b'S'); // ParameterStatus
        let param = b"client_encoding\0UTF8\0";
        let param_len = (param.len() + 4) as u32;
        buffer.extend_from_slice(&param_len.to_be_bytes());
        buffer.extend_from_slice(param);

        buffer.push(b'S'); // ParameterStatus
        let param = b"server_version\0AuroraDB 1.0\0";
        let param_len = (param.len() + 4) as u32;
        buffer.extend_from_slice(&param_len.to_be_bytes());
        buffer.extend_from_slice(param);

        buffer.push(b'K'); // BackendKeyData
        buffer.extend_from_slice(&12u32.to_be_bytes()); // Length
        buffer.extend_from_slice(&1234u32.to_be_bytes()); // PID
        buffer.extend_from_slice(&5678u32.to_be_bytes()); // Key

        buffer.push(b'Z'); // ReadyForQuery
        buffer.extend_from_slice(&5u32.to_be_bytes()); // Length
        buffer.push(b'I'); // Idle

        buffer
    }
}
