//! PostgreSQL Wire Protocol Implementation
//!
//! Implements the PostgreSQL frontend/backend protocol for client communication.
//! Supports authentication, query execution, and result formatting.

use std::io::{Read, Write};
use std::net::TcpStream;
use bytes::{Buf, BufMut, BytesMut};
use tokio::net::TcpListener;
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use std::sync::Arc;

use crate::engine::AuroraDB;
use crate::security::UserContext;

/// PostgreSQL protocol version
const PROTOCOL_VERSION: i32 = 196608; // 3.0

/// Message types
#[derive(Debug)]
pub enum MessageType {
    AuthenticationOk = b'R' as isize,
    AuthenticationCleartextPassword = b'R' as isize,
    CommandComplete = b'C' as isize,
    DataRow = b'D' as isize,
    ErrorResponse = b'E' as isize,
    ReadyForQuery = b'Z' as isize,
    RowDescription = b'T' as isize,
    Query = b'Q' as isize,
    Terminate = b'X' as isize,
}

/// PostgreSQL protocol handler
pub struct PostgresProtocol {
    db: Arc<AuroraDB>,
}

impl PostgresProtocol {
    pub fn new(db: Arc<AuroraDB>) -> Self {
        Self { db }
    }

    /// Handle a client connection
    pub async fn handle_connection(&self, mut socket: tokio::net::TcpStream) -> Result<(), Box<dyn std::error::Error>> {
        log::info!("New PostgreSQL client connection");

        // Read startup message
        let startup_message = self.read_startup_message(&mut socket).await?;
        log::debug!("Startup message: {:?}", startup_message);

        // Send authentication request (cleartext for simplicity)
        self.send_authentication_cleartext(&mut socket).await?;

        // Read password response
        let password = self.read_password_response(&mut socket).await?;
        log::debug!("Password received: {}", if password.is_empty() { "(empty)" } else { "(provided)" });

        // Send authentication success
        self.send_authentication_ok(&mut socket).await?;

        // Send ready for query
        self.send_ready_for_query(&mut socket).await?;

        // Main query loop
        loop {
            match self.read_message(&mut socket).await {
                Ok(Some((message_type, message_data))) => {
                    match message_type {
                        b'Q' => { // Query message
                            let query = String::from_utf8_lossy(&message_data[4..]); // Skip length
                            log::info!("Executing query: {}", query.trim());

                            match self.execute_query(&query.trim()).await {
                                Ok(response_messages) => {
                                    for message in response_messages {
                                        socket.write_all(&message).await?;
                                    }
                                }
                                Err(e) => {
                                    log::error!("Query execution failed: {}", e);
                                    let error_msg = self.create_error_response(&format!("Query execution failed: {}", e));
                                    socket.write_all(&error_msg).await?;
                                }
                            }

                            // Send ready for query after each command
                            self.send_ready_for_query(&mut socket).await?;
                        }
                        b'X' => { // Terminate
                            log::info!("Client disconnected");
                            break;
                        }
                        _ => {
                            log::warn!("Unhandled message type: {}", message_type);
                        }
                    }
                }
                Ok(None) => {
                    log::info!("Connection closed by client");
                    break;
                }
                Err(e) => {
                    log::error!("Error reading message: {}", e);
                    break;
                }
            }
        }

        Ok(())
    }

    /// Read startup message
    async fn read_startup_message(&self, socket: &mut tokio::net::TcpStream) -> Result<Vec<u8>, Box<dyn std::error::Error>> {
        let mut length_buf = [0u8; 4];
        socket.read_exact(&mut length_buf).await?;
        let length = u32::from_be_bytes(length_buf) as usize;

        let mut message = vec![0u8; length - 4];
        socket.read_exact(&mut message).await?;

        Ok(message)
    }

    /// Read password response
    async fn read_password_response(&self, socket: &mut tokio::net::TcpStream) -> Result<String, Box<dyn std::error::Error>> {
        let (msg_type, data) = self.read_message(socket).await?
            .ok_or("Expected password response")?;

        if msg_type != b'p' {
            return Err(format!("Expected password message, got {}", msg_type).into());
        }

        // Skip length (4 bytes) and get password
        Ok(String::from_utf8_lossy(&data[4..]).to_string())
    }

    /// Read a protocol message
    async fn read_message(&self, socket: &mut tokio::net::TcpStream) -> Result<Option<(u8, Vec<u8>)>, Box<dyn std::error::Error>> {
        let mut type_buf = [0u8; 1];
        match socket.read_exact(&mut type_buf).await {
            Ok(_) => {}
            Err(_) => return Ok(None), // Connection closed
        }

        let mut length_buf = [0u8; 4];
        socket.read_exact(&mut length_buf).await?;
        let length = u32::from_be_bytes(length_buf) as usize;

        let mut data = vec![0u8; length - 4];
        socket.read_exact(&mut data).await?;

        Ok(Some((type_buf[0], [length_buf.to_vec(), data].concat())))
    }

    /// Send authentication cleartext password request
    async fn send_authentication_cleartext(&self, socket: &mut tokio::net::TcpStream) -> Result<(), Box<dyn std::error::Error>> {
        let mut buf = BytesMut::new();
        buf.put_u8(b'R'); // Authentication message
        buf.put_u32(8); // Length
        buf.put_u32(3); // Cleartext password

        socket.write_all(&buf).await?;
        Ok(())
    }

    /// Send authentication OK
    async fn send_authentication_ok(&self, socket: &mut tokio::net::TcpStream) -> Result<(), Box<dyn std::error::Error>> {
        let mut buf = BytesMut::new();
        buf.put_u8(b'R'); // Authentication message
        buf.put_u32(8); // Length
        buf.put_u32(0); // AuthenticationOk

        socket.write_all(&buf).await?;
        Ok(())
    }

    /// Send ready for query
    async fn send_ready_for_query(&self, socket: &mut tokio::net::TcpStream) -> Result<(), Box<dyn std::error::Error>> {
        let mut buf = BytesMut::new();
        buf.put_u8(b'Z'); // ReadyForQuery
        buf.put_u32(5); // Length
        buf.put_u8(b'I'); // Idle

        socket.write_all(&buf).await?;
        Ok(())
    }

    /// Execute a query and return response messages
    async fn execute_query(&self, query: &str) -> Result<Vec<Vec<u8>>, Box<dyn std::error::Error>> {
        let user_context = UserContext::system_user();

        match self.db.execute_query(query, &user_context).await {
            Ok(result) => {
                let mut messages = Vec::new();

                // Send row description if we have columns
                if let Some(ref rows) = result.rows {
                    if let Some(first_row) = rows.first() {
                        let row_desc = self.create_row_description(first_row.keys())?;
                        messages.push(row_desc);

                        // Send data rows
                        for row in rows {
                            let data_row = self.create_data_row(&row)?;
                            messages.push(data_row);
                        }
                    }
                }

                // Send command complete
                let cmd_complete = self.create_command_complete(&result)?;
                messages.push(cmd_complete);

                Ok(messages)
            }
            Err(e) => {
                Err(e.into())
            }
        }
    }

    /// Create row description message
    fn create_row_description(&self, column_names: impl Iterator<Item = &String>) -> Result<Vec<u8>, Box<dyn std::error::Error>> {
        let mut buf = BytesMut::new();
        buf.put_u8(b'T'); // RowDescription

        // Calculate message length (placeholder for now)
        let length_pos = buf.len();
        buf.put_u32(0); // Length placeholder

        // Number of fields
        let fields: Vec<String> = column_names.cloned().collect();
        buf.put_u16(fields.len() as u16);

        // Field descriptions
        for (i, field_name) in fields.iter().enumerate() {
            // Field name
            buf.put_slice(field_name.as_bytes());
            buf.put_u8(0); // Null terminator

            // Table OID (0 for now)
            buf.put_u32(0);

            // Column index
            buf.put_u16((i + 1) as u16);

            // Type OID (assume text for simplicity)
            buf.put_u32(25); // TEXT type

            // Type size
            buf.put_u16(0xFFFF); // Variable size

            // Type modifier
            buf.put_u32(0xFFFFFFFF);

            // Format code (text)
            buf.put_u16(0);
        }

        // Update message length
        let length = (buf.len() - length_pos - 4) as u32;
        let length_bytes = length.to_be_bytes();
        buf[length_pos..length_pos + 4].copy_from_slice(&length_bytes);

        Ok(buf.to_vec())
    }

    /// Create data row message
    fn create_data_row(&self, row: &std::collections::HashMap<String, crate::types::DataValue>) -> Result<Vec<u8>, Box<dyn std::error::Error>> {
        let mut buf = BytesMut::new();
        buf.put_u8(b'D'); // DataRow

        let length_pos = buf.len();
        buf.put_u32(0); // Length placeholder

        // Number of columns
        buf.put_u16(row.len() as u16);

        // Column values
        for (_col_name, value) in row {
            let value_str = format!("{}", value);
            let value_bytes = value_str.as_bytes();

            buf.put_u32(value_bytes.len() as u32); // Column length
            buf.put_slice(value_bytes); // Column data
        }

        // Update message length
        let length = (buf.len() - length_pos - 4) as u32;
        let length_bytes = length.to_be_bytes();
        buf[length_pos..length_pos + 4].copy_from_slice(&length_bytes);

        Ok(buf.to_vec())
    }

    /// Create command complete message
    fn create_command_complete(&self, result: &crate::engine::QueryResult) -> Result<Vec<u8>, Box<dyn std::error::Error>> {
        let mut buf = BytesMut::new();
        buf.put_u8(b'C'); // CommandComplete

        let tag = if result.rows_affected.is_some() {
            format!("INSERT 0 {}", result.rows_affected.unwrap_or(0))
        } else if result.rows.is_some() {
            format!("SELECT {}", result.rows.as_ref().unwrap().len())
        } else {
            "SELECT 0".to_string()
        };

        let tag_bytes = tag.as_bytes();
        buf.put_u32((4 + tag_bytes.len() + 1) as u32); // Length
        buf.put_slice(tag_bytes);
        buf.put_u8(0); // Null terminator

        Ok(buf.to_vec())
    }

    /// Create error response message
    fn create_error_response(&self, error_msg: &str) -> Vec<u8> {
        let mut buf = BytesMut::new();
        buf.put_u8(b'E'); // ErrorResponse

        let error_bytes = error_msg.as_bytes();
        buf.put_u32((4 + 1 + error_bytes.len() + 1) as u32); // Length
        buf.put_u8(b'M'); // Error message field
        buf.put_slice(error_bytes);
        buf.put_u8(0); // Null terminator
        buf.put_u8(0); // Message terminator

        buf.to_vec()
    }
}

/// PostgreSQL server implementation
pub struct PostgresServer {
    protocol: PostgresProtocol,
    address: String,
}

impl PostgresServer {
    pub fn new(db: Arc<AuroraDB>, address: String) -> Self {
        Self {
            protocol: PostgresProtocol::new(db),
            address,
        }
    }

    /// Start the PostgreSQL server
    pub async fn start(&self) -> Result<(), Box<dyn std::error::Error>> {
        let listener = TcpListener::bind(&self.address).await?;
        log::info!("PostgreSQL server listening on {}", self.address);

        loop {
            let (socket, addr) = listener.accept().await?;
            log::info!("Accepted connection from {}", addr);

            let protocol = self.protocol.clone();
            tokio::spawn(async move {
                if let Err(e) = protocol.handle_connection(socket).await {
                    log::error!("Connection error: {}", e);
                }
            });
        }
    }
}

impl Clone for PostgresProtocol {
    fn clone(&self) -> Self {
        Self {
            db: Arc::clone(&self.db),
        }
    }
}
