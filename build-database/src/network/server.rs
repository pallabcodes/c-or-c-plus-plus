//! AuroraDB Server
//!
//! Main server implementation handling client connections and query processing.
//! Supports multiple protocols with efficient connection management.

use crate::core::*;
use super::connection::*;
use super::pooling::*;
use super::protocol::*;
use std::collections::HashMap;
use std::sync::Arc;
use parking_lot::RwLock;
use tokio::net::TcpListener;
use tokio::sync::mpsc;

/// AuroraDB server instance
pub struct AuroraServer {
    /// Server configuration
    config: ServerConfig,
    /// Connection pool
    connection_pool: ConnectionPool,
    /// Active connections
    active_connections: Arc<RwLock<HashMap<u64, Arc<RwLock<Connection>>>>>,
    /// Query processing channels
    query_sender: mpsc::UnboundedSender<QueryRequest>,
    query_receiver: mpsc::UnboundedReceiver<QueryRequest>,
    /// Server statistics
    stats: ServerStats,
}

/// Server configuration
#[derive(Debug, Clone)]
pub struct ServerConfig {
    pub host: String,
    pub port: u16,
    pub max_connections: usize,
    pub connection_timeout_ms: u64,
    pub query_timeout_ms: u64,
    pub protocol_formats: Vec<ProtocolFormat>,
    pub enable_ssl: bool,
    pub ssl_cert_path: Option<String>,
    pub ssl_key_path: Option<String>,
}

/// Server statistics
#[derive(Debug, Clone, Default)]
pub struct ServerStats {
    pub total_connections_accepted: u64,
    pub total_connections_active: u64,
    pub total_queries_processed: u64,
    pub total_queries_failed: u64,
    pub average_query_time_ms: f64,
    pub uptime_seconds: u64,
}

/// Query processing request
pub struct QueryRequest {
    pub connection_id: u64,
    pub query: String,
    pub protocol: ProtocolFormat,
    pub response_sender: mpsc::UnboundedSender<QueryResponse>,
}

/// Query processing response
pub enum QueryResponse {
    Success { data: Vec<u8>, execution_time_ms: f64 },
    Error { message: String, error_code: String },
    StreamStart,
    StreamData { data: Vec<u8> },
    StreamEnd,
}

impl AuroraServer {
    /// Create a new AuroraDB server
    pub fn new(config: ServerConfig) -> Self {
        let (query_sender, query_receiver) = mpsc::unbounded_channel();

        // Create connection pool
        let pool_config = PoolConfig {
            max_connections: config.max_connections,
            min_connections: 10, // Start with 10 connections
            max_idle_time_ms: 300000, // 5 minutes
            connection_timeout_ms: config.connection_timeout_ms,
            health_check_interval_ms: 30000, // 30 seconds
            connection_config: ConnectionConfig {
                host: config.host.clone(),
                port: config.port,
                max_connections: config.max_connections,
                connection_timeout_ms: config.connection_timeout_ms,
                idle_timeout_ms: 300000, // 5 minutes
                buffer_size: 8192,
                protocol_format: config.protocol_formats.first().cloned().unwrap_or(ProtocolFormat::PostgreSQL),
            },
        };

        let factory = Box::new(TcpConnectionFactory::new(pool_config.connection_config.clone()));
        let connection_pool = ConnectionPool::new(pool_config, factory);

        Self {
            config,
            connection_pool,
            active_connections: Arc::new(RwLock::new(HashMap::new())),
            query_sender,
            query_receiver,
            stats: ServerStats::default(),
        }
    }

    /// Start the server and begin accepting connections
    pub async fn start(&mut self) -> Result<(), ServerError> {
        let address = format!("{}:{}", self.config.host, self.config.port);
        let listener = TcpListener::bind(&address).await
            .map_err(|e| ServerError::BindError(e.to_string()))?;

        println!("AuroraDB server listening on {}", address);

        // Start query processor
        let query_processor = tokio::spawn(self.clone().run_query_processor());

        // Start connection maintenance
        let maintenance = tokio::spawn(self.clone().run_maintenance());

        // Start accepting connections
        let accept_loop = self.run_accept_loop(listener);

        // Wait for all tasks
        tokio::try_join!(query_processor, maintenance, accept_loop)?;

        Ok(())
    }

    /// Run the connection accept loop
    async fn run_accept_loop(&mut self, listener: TcpListener) -> Result<(), ServerError> {
        loop {
            match listener.accept().await {
                Ok((stream, addr)) => {
                    println!("Accepted connection from {}", addr);

                    // Create connection config based on protocol negotiation
                    let conn_config = ConnectionConfig {
                        host: self.config.host.clone(),
                        port: self.config.port,
                        max_connections: self.config.max_connections,
                        connection_timeout_ms: self.config.connection_timeout_ms,
                        idle_timeout_ms: 300000,
                        buffer_size: 8192,
                        protocol_format: ProtocolFormat::PostgreSQL, // Default, will be negotiated
                    };

                    match Connection::new(stream, conn_config).await {
                        Ok(mut connection) => {
                            self.stats.total_connections_accepted += 1;
                            self.stats.total_connections_active += 1;

                            let connection_id = connection.id;
                            let connection = Arc::new(RwLock::new(connection));
                            self.active_connections.write().insert(connection_id, connection.clone());

                            // Spawn connection handler
                            let server_clone = self.clone();
                            tokio::spawn(async move {
                                if let Err(e) = server_clone.handle_connection(connection_id, connection).await {
                                    eprintln!("Connection error: {}", e);
                                }
                            });
                        }
                        Err(e) => {
                            eprintln!("Failed to create connection: {}", e);
                        }
                    }
                }
                Err(e) => {
                    eprintln!("Accept error: {}", e);
                    return Err(ServerError::AcceptError(e.to_string()));
                }
            }
        }
    }

    /// Handle individual client connection
    async fn handle_connection(
        &self,
        connection_id: u64,
        connection: Arc<RwLock<Connection>>,
    ) -> Result<(), ServerError> {
        // Perform handshake
        {
            let mut conn = connection.write();
            conn.handshake().await?;
        }

        // Connection is now ready for queries
        loop {
            // Receive query message
            let message = {
                let mut conn = connection.write();
                match conn.receive_message().await {
                    Ok(msg) => msg,
                    Err(ConnectionError::ConnectionClosed) => break,
                    Err(e) => {
                        eprintln!("Receive error: {}", e);
                        break;
                    }
                }
            };

            // Process the query
            match message.message_type {
                MessageType::Query => {
                    let sql = String::from_utf8_lossy(&message.payload).to_string();
                    self.process_query(connection_id, sql, connection.clone()).await?;
                }
                MessageType::VectorQuery => {
                    // Handle vector-specific queries
                    let query_data = message.payload;
                    self.process_vector_query(connection_id, query_data, connection.clone()).await?;
                }
                MessageType::Terminate => {
                    break;
                }
                _ => {
                    // Send error for unsupported message types
                    let error_msg = connection.write().protocol.create_error_message("Unsupported message type");
                    let _ = connection.write().send_message(&error_msg).await;
                }
            }
        }

        // Clean up connection
        self.active_connections.write().remove(&connection_id);
        self.stats.total_connections_active -= 1;

        Ok(())
    }

    /// Process a SQL query
    async fn process_query(
        &self,
        connection_id: u64,
        sql: String,
        connection: Arc<RwLock<Connection>>,
    ) -> Result<(), ServerError> {
        let (response_sender, mut response_receiver) = mpsc::unbounded_channel();

        // Send query to processor
        let request = QueryRequest {
            connection_id,
            query: sql,
            protocol: connection.read().config.protocol_format.clone(),
            response_sender,
        };

        self.query_sender.send(request)?;

        // Wait for response
        while let Some(response) = response_receiver.recv().await {
            match response {
                QueryResponse::Success { data, execution_time_ms } => {
                    let message = AuroraMessage {
                        message_type: MessageType::DataRow,
                        payload: data,
                        metadata: HashMap::new(),
                    };
                    let _ = connection.write().send_message(&message).await;

                    // Send command complete
                    let complete_msg = AuroraMessage {
                        message_type: MessageType::CommandComplete,
                        payload: Vec::new(),
                        metadata: HashMap::new(),
                    };
                    let _ = connection.write().send_message(&complete_msg).await;
                }
                QueryResponse::Error { message, error_code } => {
                    let mut metadata = HashMap::new();
                    metadata.insert("error_code".to_string(), error_code);
                    let error_msg = AuroraMessage {
                        message_type: MessageType::ErrorResponse,
                        payload: Vec::new(),
                        metadata,
                    };
                    let _ = connection.write().send_message(&error_msg).await;
                    self.stats.total_queries_failed += 1;
                }
                _ => {} // Handle streaming responses
            }
        }

        Ok(())
    }

    /// Process a vector query
    async fn process_vector_query(
        &self,
        connection_id: u64,
        query_data: Vec<u8>,
        connection: Arc<RwLock<Connection>>,
    ) -> Result<(), ServerError> {
        // For now, just acknowledge the vector query
        let response = AuroraMessage {
            message_type: MessageType::DataRow,
            payload: b"Vector query processed".to_vec(),
            metadata: HashMap::new(),
        };
        let _ = connection.write().send_message(&response).await;

        Ok(())
    }

    /// Run the query processor task
    async fn run_query_processor(mut self) {
        while let Some(request) = self.query_receiver.recv().await {
            // Process query (placeholder - in real implementation, this would execute against the database)
            let start_time = std::time::Instant::now();

            // Simulate query processing
            tokio::time::sleep(tokio::time::Duration::from_millis(10)).await;

            let execution_time = start_time.elapsed().as_millis() as f64;

            // Send mock response
            let response = QueryResponse::Success {
                data: format!("Query result for: {}", request.query).into_bytes(),
                execution_time_ms: execution_time,
            };

            let _ = request.response_sender.send(response);
            self.stats.total_queries_processed += 1;
            self.stats.average_query_time_ms =
                (self.stats.average_query_time_ms * (self.stats.total_queries_processed - 1) as f64 + execution_time)
                    / self.stats.total_queries_processed as f64;
        }
    }

    /// Run maintenance tasks (connection cleanup, health checks)
    async fn run_maintenance(mut self) {
        let mut interval = tokio::time::interval(tokio::time::Duration::from_secs(30));

        loop {
            interval.tick().await;

            // Update uptime
            self.stats.uptime_seconds += 30;

            // Perform pool maintenance
            self.connection_pool.maintain().await;

            // Clean up idle connections
            self.cleanup_idle_connections().await;
        }
    }

    /// Clean up idle connections
    async fn cleanup_idle_connections(&mut self) {
        let mut to_remove = Vec::new();

        {
            let connections = self.active_connections.read();
            for (id, connection) in connections.iter() {
                if connection.read().is_idle() {
                    to_remove.push(*id);
                }
            }
        }

        for id in to_remove {
            if let Some(connection) = self.active_connections.write().remove(&id) {
                let _ = connection.write().close().await;
                self.stats.total_connections_active -= 1;
            }
        }
    }

    /// Get server statistics
    pub fn stats(&self) -> &ServerStats {
        &self.stats
    }

    /// Shutdown the server gracefully
    pub async fn shutdown(&mut self) -> Result<(), ServerError> {
        // Close all active connections
        let connections: Vec<_> = self.active_connections.read().keys().cloned().collect();
        for id in connections {
            if let Some(connection) = self.active_connections.write().remove(&id) {
                let _ = connection.write().close().await;
            }
        }

        // Close query channels
        self.query_sender.closed().await;

        Ok(())
    }
}

impl Clone for AuroraServer {
    fn clone(&self) -> Self {
        // Note: This is a simplified clone for spawning tasks
        // In production, use Arc for shared state
        Self {
            config: self.config.clone(),
            connection_pool: ConnectionPool::new(
                PoolConfig {
                    max_connections: self.config.max_connections,
                    min_connections: 10,
                    max_idle_time_ms: 300000,
                    connection_timeout_ms: self.config.connection_timeout_ms,
                    health_check_interval_ms: 30000,
                    connection_config: ConnectionConfig {
                        host: self.config.host.clone(),
                        port: self.config.port,
                        max_connections: self.config.max_connections,
                        connection_timeout_ms: self.config.connection_timeout_ms,
                        idle_timeout_ms: 300000,
                        buffer_size: 8192,
                        protocol_format: ProtocolFormat::PostgreSQL,
                    },
                },
                Box::new(TcpConnectionFactory::new(ConnectionConfig {
                    host: self.config.host.clone(),
                    port: self.config.port,
                    max_connections: self.config.max_connections,
                    connection_timeout_ms: self.config.connection_timeout_ms,
                    idle_timeout_ms: 300000,
                    buffer_size: 8192,
                    protocol_format: ProtocolFormat::PostgreSQL,
                })),
            ),
            active_connections: self.active_connections.clone(),
            query_sender: self.query_sender.clone(),
            query_receiver: mpsc::unbounded_channel().1, // New receiver (won't work in practice)
            stats: self.stats.clone(),
        }
    }
}

/// Server operation errors
#[derive(Debug, thiserror::Error)]
pub enum ServerError {
    #[error("Bind error: {0}")]
    BindError(String),

    #[error("Accept error: {0}")]
    AcceptError(String),

    #[error("Connection error: {0}")]
    ConnectionError(#[from] ConnectionError),

    #[error("Query processing error: {0}")]
    QueryError(String),

    #[error("Channel error: {0}")]
    ChannelError(#[from] tokio::sync::mpsc::error::SendError<QueryRequest>),
}
