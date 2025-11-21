//! AuroraDB Custom Binary Protocol
//!
//! High-performance binary protocol optimized for AuroraDB features:
//! - Zero-copy serialization for hot paths
//! - Compact encoding for better bandwidth utilization
//! - Built-in compression support
//! - Efficient vector data encoding

use crate::network::protocol::*;
use std::collections::HashMap;

/// AuroraDB custom binary serializer (high performance)
pub struct AuroraBinarySerializer;

impl AuroraBinarySerializer {
    pub fn new() -> Self {
        Self
    }
}

impl MessageSerializer for AuroraBinarySerializer {
    fn serialize(&self, message: &AuroraMessage) -> Result<Vec<u8>, ProtocolError> {
        let mut buffer = Vec::new();

        // AuroraDB binary format: Magic (4 bytes) + Version (2 bytes) + Type (2 bytes) + Length (4 bytes) + Payload
        buffer.extend_from_slice(b"AUR\x01"); // Magic + version
        let msg_type_u16 = message_type_to_u16(&message.message_type);
        buffer.extend_from_slice(&msg_type_u16.to_le_bytes());
        let payload_len = message.payload.len() as u32;
        buffer.extend_from_slice(&payload_len.to_le_bytes());
        buffer.extend_from_slice(&message.payload);

        // Add metadata if present
        if !message.metadata.is_empty() {
            let metadata_json = serde_json::to_string(&message.metadata)
                .map_err(|e| ProtocolError::SerializationError(e.to_string()))?;
            let metadata_bytes = metadata_json.as_bytes();
            let metadata_len = metadata_bytes.len() as u32;
            buffer.extend_from_slice(&metadata_len.to_le_bytes());
            buffer.extend_from_slice(metadata_bytes);
        } else {
            buffer.extend_from_slice(&0u32.to_le_bytes());
        }

        Ok(buffer)
    }

    fn deserialize(&self, data: &[u8]) -> Result<AuroraMessage, ProtocolError> {
        if data.len() < 12 { // Minimum header size
            return Err(ProtocolError::DeserializationError("Message too short".to_string()));
        }

        // Check magic
        if &data[0..4] != b"AUR\x01" {
            return Err(ProtocolError::DeserializationError("Invalid magic bytes".to_string()));
        }

        let msg_type_u16 = u16::from_le_bytes([data[4], data[5]]);
        let message_type = u16_to_message_type(msg_type_u16)
            .ok_or_else(|| ProtocolError::InvalidMessageType(msg_type_u16 as u8))?;

        let payload_len = u32::from_le_bytes([data[6], data[7], data[8], data[9]]) as usize;

        if data.len() < 12 + payload_len {
            return Err(ProtocolError::DeserializationError("Incomplete payload".to_string()));
        }

        let payload = data[10..10 + payload_len].to_vec();

        // Parse metadata if present
        let metadata_offset = 10 + payload_len;
        let metadata = if data.len() > metadata_offset + 4 {
            let metadata_len = u32::from_le_bytes([
                data[metadata_offset],
                data[metadata_offset + 1],
                data[metadata_offset + 2],
                data[metadata_offset + 3],
            ]) as usize;

            if data.len() >= metadata_offset + 4 + metadata_len {
                let metadata_json = &data[metadata_offset + 4..metadata_offset + 4 + metadata_len];
                serde_json::from_slice(metadata_json)
                    .unwrap_or_default()
            } else {
                HashMap::new()
            }
        } else {
            HashMap::new()
        };

        Ok(AuroraMessage {
            message_type,
            payload,
            metadata,
        })
    }
}

/// High-performance vector encoding for AuroraDB protocol
pub struct VectorEncoder;

impl VectorEncoder {
    /// Encode floating point vector with quantization
    pub fn encode_vector_f32(vector: &[f32], quantization_bits: u8) -> Vec<u8> {
        match quantization_bits {
            8 => Self::quantize_to_u8(vector),
            4 => Self::quantize_to_u4(vector),
            _ => Self::encode_raw_f32(vector),
        }
    }

    /// Decode vector from AuroraDB binary format
    pub fn decode_vector(data: &[u8]) -> Result<Vec<f32>, ProtocolError> {
        if data.len() < 4 {
            return Err(ProtocolError::DeserializationError("Vector data too short".to_string()));
        }

        let quantization_type = data[0];
        let dimension = u32::from_le_bytes([data[1], data[2], data[3], data[4]]) as usize;

        match quantization_type {
            0 => Self::decode_raw_f32(&data[5..], dimension),
            8 => Self::decode_u8_quantized(&data[5..], dimension),
            4 => Self::decode_u4_quantized(&data[5..], dimension),
            _ => Err(ProtocolError::DeserializationError("Unknown quantization type".to_string())),
        }
    }

    fn quantize_to_u8(vector: &[f32]) -> Vec<u8> {
        let mut result = Vec::with_capacity(vector.len() + 5);

        // Header: quantization type (1) + dimension (4)
        result.push(8); // 8-bit quantization
        result.extend_from_slice(&(vector.len() as u32).to_le_bytes());

        // Quantize values to 0-255 range
        for &value in vector {
            let quantized = ((value + 1.0) * 127.5).clamp(0.0, 255.0) as u8;
            result.push(quantized);
        }

        result
    }

    fn quantize_to_u4(_vector: &[f32]) -> Vec<u8> {
        // TODO: Implement 4-bit quantization (pack two 4-bit values per byte)
        Vec::new()
    }

    fn encode_raw_f32(vector: &[f32]) -> Vec<u8> {
        let mut result = Vec::with_capacity(vector.len() * 4 + 5);

        // Header: quantization type (1) + dimension (4)
        result.push(0); // Raw encoding
        result.extend_from_slice(&(vector.len() as u32).to_le_bytes());

        // Raw float values
        for &value in vector {
            result.extend_from_slice(&value.to_le_bytes());
        }

        result
    }

    fn decode_raw_f32(data: &[u8], dimension: usize) -> Result<Vec<f32>, ProtocolError> {
        if data.len() != dimension * 4 {
            return Err(ProtocolError::DeserializationError("Incorrect data length for raw f32 vector".to_string()));
        }

        let mut result = Vec::with_capacity(dimension);
        for i in 0..dimension {
            let offset = i * 4;
            let bytes = [data[offset], data[offset + 1], data[offset + 2], data[offset + 3]];
            result.push(f32::from_le_bytes(bytes));
        }

        Ok(result)
    }

    fn decode_u8_quantized(data: &[u8], dimension: usize) -> Result<Vec<f32>, ProtocolError> {
        if data.len() != dimension {
            return Err(ProtocolError::DeserializationError("Incorrect data length for u8 quantized vector".to_string()));
        }

        let mut result = Vec::with_capacity(dimension);
        for &byte in data {
            let value = (byte as f32 / 127.5) - 1.0;
            result.push(value);
        }

        Ok(result)
    }

    fn decode_u4_quantized(_data: &[u8], _dimension: usize) -> Result<Vec<f32>, ProtocolError> {
        // TODO: Implement 4-bit decoding
        Err(ProtocolError::DeserializationError("4-bit quantization not yet implemented".to_string()))
    }
}

/// Compression support for AuroraDB protocol
pub struct Compression;

impl Compression {
    /// Compress payload if beneficial
    pub fn maybe_compress(data: &[u8], compression_threshold: usize) -> (Vec<u8>, bool) {
        if data.len() < compression_threshold {
            return (data.to_vec(), false);
        }

        // TODO: Implement LZ4 or Zstd compression
        // For now, return uncompressed
        (data.to_vec(), false)
    }

    /// Decompress payload if compressed
    pub fn maybe_decompress(data: &[u8]) -> Result<Vec<u8>, ProtocolError> {
        // Check compression header
        if data.len() < 4 {
            return Ok(data.to_vec());
        }

        let compression_magic = &data[0..4];
        match compression_magic {
            b"LZ4\x01" => {
                // TODO: LZ4 decompression
                Ok(data[4..].to_vec())
            }
            b"ZSTD" => {
                // TODO: Zstd decompression
                Ok(data[4..].to_vec())
            }
            _ => Ok(data.to_vec()), // Not compressed
        }
    }
}

/// Helper functions for message type conversion
fn message_type_to_u16(msg_type: &MessageType) -> u16 {
    match msg_type {
        MessageType::Startup => 1,
        MessageType::Authentication => 2,
        MessageType::Query => 3,
        MessageType::DataRow => 4,
        MessageType::ErrorResponse => 5,
        MessageType::CommandComplete => 6,
        MessageType::ReadyForQuery => 7,
        MessageType::Begin => 8,
        MessageType::Commit => 9,
        MessageType::Rollback => 10,
        MessageType::VectorQuery => 11,
        MessageType::AnalyticsQuery => 12,
        MessageType::BulkLoad => 13,
        MessageType::StreamResponse => 14,
        _ => 0, // Unknown
    }
}

fn u16_to_message_type(value: u16) -> Option<MessageType> {
    match value {
        1 => Some(MessageType::Startup),
        2 => Some(MessageType::Authentication),
        3 => Some(MessageType::Query),
        4 => Some(MessageType::DataRow),
        5 => Some(MessageType::ErrorResponse),
        6 => Some(MessageType::CommandComplete),
        7 => Some(MessageType::ReadyForQuery),
        8 => Some(MessageType::Begin),
        9 => Some(MessageType::Commit),
        10 => Some(MessageType::Rollback),
        11 => Some(MessageType::VectorQuery),
        12 => Some(MessageType::AnalyticsQuery),
        13 => Some(MessageType::BulkLoad),
        14 => Some(MessageType::StreamResponse),
        _ => None,
    }
}
