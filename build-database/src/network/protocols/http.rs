//! HTTP/JSON Protocol for Web Applications
//!
//! RESTful API interface for web applications and HTTP clients.
//! Supports JSON payloads, standard HTTP status codes, and web-friendly features.

use crate::network::protocol::*;
use std::collections::HashMap;

/// HTTP/JSON serializer for web applications
pub struct HTTPSerializer;

impl HTTPSerializer {
    pub fn new() -> Self {
        Self
    }
}

impl MessageSerializer for HTTPSerializer {
    fn serialize(&self, message: &AuroraMessage) -> Result<Vec<u8>, ProtocolError> {
        // Create JSON representation
        let json_message = serde_json::json!({
            "type": format!("{:?}", message.message_type),
            "payload": base64::encode(&message.payload),
            "metadata": message.metadata
        });

        let json_str = json_message.to_string();
        Ok(json_str.into_bytes())
    }

    fn deserialize(&self, data: &[u8]) -> Result<AuroraMessage, ProtocolError> {
        let json: serde_json::Value = serde_json::from_slice(data)
            .map_err(|e| ProtocolError::DeserializationError(e.to_string()))?;

        let message_type_str = json.get("type")
            .and_then(|v| v.as_str())
            .ok_or_else(|| ProtocolError::DeserializationError("Missing message type".to_string()))?;

        // Parse message type from string (simplified)
        let message_type = match message_type_str {
            "Query" => MessageType::Query,
            "DataRow" => MessageType::DataRow,
            "ErrorResponse" => MessageType::ErrorResponse,
            "CommandComplete" => MessageType::CommandComplete,
            "ReadyForQuery" => MessageType::ReadyForQuery,
            "VectorQuery" => MessageType::VectorQuery,
            "AnalyticsQuery" => MessageType::AnalyticsQuery,
            _ => MessageType::Query, // Default fallback
        };

        let payload_b64 = json.get("payload")
            .and_then(|v| v.as_str())
            .unwrap_or("");
        let payload = base64::decode(payload_b64)
            .map_err(|e| ProtocolError::DeserializationError(e.to_string()))?;

        let metadata: HashMap<String, String> = json.get("metadata")
            .and_then(|v| v.as_object())
            .map(|obj| {
                obj.iter()
                    .filter_map(|(k, v)| v.as_str().map(|s| (k.clone(), s.to_string())))
                    .collect()
            })
            .unwrap_or_default();

        Ok(AuroraMessage {
            message_type,
            payload,
            metadata,
        })
    }
}

/// HTTP request/response builders for REST API
pub struct HTTPProtocol;

impl HTTPProtocol {
    /// Create HTTP response for successful query
    pub fn create_query_response(results: Vec<serde_json::Value>, execution_time: f64) -> (u16, String, serde_json::Value) {
        let response = serde_json::json!({
            "status": "success",
            "data": results,
            "execution_time_ms": execution_time,
            "row_count": results.len()
        });

        (200, "application/json".to_string(), response)
    }

    /// Create HTTP error response
    pub fn create_error_response(error: &str, error_code: &str) -> (u16, String, serde_json::Value) {
        let status_code = match error_code {
            "SYNTAX_ERROR" => 400,
            "AUTHENTICATION_FAILED" => 401,
            "PERMISSION_DENIED" => 403,
            "NOT_FOUND" => 404,
            "TIMEOUT" => 408,
            "CONFLICT" => 409,
            "PAYLOAD_TOO_LARGE" => 413,
            "UNSUPPORTED_MEDIA_TYPE" => 415,
            "TOO_MANY_REQUESTS" => 429,
            _ => 500, // Internal server error
        };

        let response = serde_json::json!({
            "status": "error",
            "error": {
                "code": error_code,
                "message": error
            }
        });

        (status_code, "application/json".to_string(), response)
    }

    /// Parse HTTP request body for SQL queries
    pub fn parse_query_request(body: &str) -> Result<String, ProtocolError> {
        let json: serde_json::Value = serde_json::from_str(body)
            .map_err(|e| ProtocolError::DeserializationError(e.to_string()))?;

        let query = json.get("query")
            .and_then(|v| v.as_str())
            .ok_or_else(|| ProtocolError::DeserializationError("Missing 'query' field in request".to_string()))?;

        Ok(query.to_string())
    }

    /// Create streaming response for large result sets
    pub fn create_streaming_response() -> (u16, String, String) {
        let headers = "Transfer-Encoding: chunked\r\nContent-Type: application/json\r\n".to_string();
        (200, headers, String::new())
    }

    /// Create chunk for streaming response
    pub fn create_chunk(data: serde_json::Value) -> String {
        let chunk_data = data.to_string();
        format!("{:X}\r\n{}\r\n", chunk_data.len(), chunk_data)
    }

    /// Create end chunk for streaming response
    pub fn create_end_chunk() -> String {
        "0\r\n\r\n".to_string()
    }
}

/// WebSocket protocol support for real-time queries
pub struct WebSocketProtocol;

impl WebSocketProtocol {
    /// Handle WebSocket message for real-time query execution
    pub fn handle_query_message(message: &str) -> Result<String, ProtocolError> {
        let json: serde_json::Value = serde_json::from_str(message)
            .map_err(|e| ProtocolError::DeserializationError(e.to_string()))?;

        let query_type = json.get("type").and_then(|v| v.as_str()).unwrap_or("unknown");

        match query_type {
            "subscribe" => {
                // Handle subscription for real-time data
                let subscription_id = json.get("subscription_id")
                    .and_then(|v| v.as_str())
                    .unwrap_or("unknown");

                let response = serde_json::json!({
                    "type": "subscribed",
                    "subscription_id": subscription_id,
                    "status": "active"
                });

                Ok(response.to_string())
            }
            "query" => {
                // Handle real-time query
                let sql = json.get("sql")
                    .and_then(|v| v.as_str())
                    .ok_or_else(|| ProtocolError::DeserializationError("Missing SQL in query message".to_string()))?;

                let response = serde_json::json!({
                    "type": "query_result",
                    "query": sql,
                    "status": "executing"
                });

                Ok(response.to_string())
            }
            _ => {
                Err(ProtocolError::DeserializationError(format!("Unknown message type: {}", query_type)))
            }
        }
    }

    /// Create heartbeat message
    pub fn create_heartbeat() -> String {
        serde_json::json!({
            "type": "heartbeat",
            "timestamp": std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_secs()
        }).to_string()
    }
}
