//! AuroraDB Client
//!
//! Client implementation for connecting to AuroraDB servers.
//! Supports connection pooling and multiple protocol formats.

use crate::core::*;
use super::connection::*;
use super::pooling::*;
use super::protocol::*;
use std::time::Duration;

/// AuroraDB client for database connections
pub struct AuroraClient {
    /// Connection pool
    pool: ConnectionPool,
    /// Client configuration
    config: ClientConfig,
    /// Client statistics
    stats: ClientStats,
}

/// Client configuration
#[derive(Debug, Clone)]
pub struct ClientConfig {
    pub host: String,
    pub port: u16,
    pub username: String,
    pub password: Option<String>,
    pub database: String,
    pub protocol_format: ProtocolFormat,
    pub connection_timeout_ms: u64,
    pub query_timeout_ms: u64,
    pub max_connections: usize,
    pub enable_ssl: bool,
}

/// Client statistics
#[derive(Debug, Clone, Default)]
pub struct ClientStats {
    pub total_queries_executed: u64,
    pub total_connections_created: u64,
    pub total_connections_failed: u64,
    pub average_query_time_ms: f64,
    pub total_bytes_sent: u64,
    pub total_bytes_received: u64,
}

impl AuroraClient {
    /// Create a new AuroraDB client
    pub fn new(config: ClientConfig) -> Self {
        let pool_config = PoolConfig {
            max_connections: config.max_connections,
            min_connections: 1,
            max_idle_time_ms: 300000, // 5 minutes
            connection_timeout_ms: config.connection_timeout_ms,
            health_check_interval_ms: 30000, // 30 seconds
            connection_config: ConnectionConfig {
                host: config.host.clone(),
                port: config.port,
                max_connections: config.max_connections,
                connection_timeout_ms: config.connection_timeout_ms,
                idle_timeout_ms: 300000,
                buffer_size: 8192,
                protocol_format: config.protocol_format.clone(),
            },
        };

        let factory = Box::new(TcpConnectionFactory::new(pool_config.connection_config.clone()));
        let pool = ConnectionPool::new(pool_config, factory);

        Self {
            pool,
            config,
            stats: ClientStats::default(),
        }
    }

    /// Execute a SQL query
    pub async fn execute_query(&mut self, sql: &str) -> Result<QueryResult, ClientError> {
        let start_time = std::time::Instant::now();

        // Get connection from pool
        let mut pooled_conn = self.pool.get_connection().await?;

        // Create query message
        let query_message = pooled_conn.connection().read().protocol.create_query_message(sql);

        // Send query
        pooled_conn.send_message(&query_message).await?;
        self.stats.total_bytes_sent += query_message.payload.len() as u64;

        // Receive response
        let response = pooled_conn.receive_message().await?;
        self.stats.total_bytes_received += response.payload.len() as u64;

        // Process response based on type
        match response.message_type {
            MessageType::DataRow => {
                let execution_time = start_time.elapsed().as_millis() as f64;

                // Parse result data (simplified)
                let result_data = String::from_utf8_lossy(&response.payload).to_string();

                self.stats.total_queries_executed += 1;
                self.stats.average_query_time_ms =
                    (self.stats.average_query_time_ms * (self.stats.total_queries_executed - 1) as f64 + execution_time)
                        / self.stats.total_queries_executed as f64;

                Ok(QueryResult {
                    data: vec![result_data],
                    row_count: 1,
                    execution_time_ms: execution_time,
                    columns: vec!["result".to_string()],
                })
            }
            MessageType::ErrorResponse => {
                let error_msg = response.metadata.get("error")
                    .cloned()
                    .unwrap_or_else(|| "Unknown error".to_string());
                Err(ClientError::QueryError(error_msg))
            }
            _ => {
                Err(ClientError::ProtocolError(format!("Unexpected response type: {:?}", response.message_type)))
            }
        }
    }

    /// Execute a vector similarity search
    pub async fn execute_vector_query(&mut self, vector: &[f32], k: usize, table: &str) -> Result<VectorResult, ClientError> {
        let start_time = std::time::Instant::now();

        // Get connection from pool
        let mut pooled_conn = self.pool.get_connection().await?;

        // Create vector query message
        let query_data = format!("{}|{}|{:?}", table, k, vector);
        let query_message = AuroraMessage {
            message_type: MessageType::VectorQuery,
            payload: query_data.into_bytes(),
            metadata: HashMap::new(),
        };

        // Send query
        pooled_conn.send_message(&query_message).await?;
        self.stats.total_bytes_sent += query_message.payload.len() as u64;

        // Receive response
        let response = pooled_conn.receive_message().await?;
        self.stats.total_bytes_received += response.payload.len() as u64;

        let execution_time = start_time.elapsed().as_millis() as f64;

        match response.message_type {
            MessageType::DataRow => {
                // Parse vector results (simplified)
                let result_text = String::from_utf8_lossy(&response.payload).to_string();

                Ok(VectorResult {
                    vectors: vec![], // Placeholder
                    distances: vec![], // Placeholder
                    execution_time_ms: execution_time,
                    result_text,
                })
            }
            MessageType::ErrorResponse => {
                let error_msg = response.metadata.get("error")
                    .cloned()
                    .unwrap_or_else(|| "Vector query failed".to_string());
                Err(ClientError::QueryError(error_msg))
            }
            _ => Err(ClientError::ProtocolError("Invalid vector response".to_string())),
        }
    }

    /// Execute a batch of queries
    pub async fn execute_batch(&mut self, queries: Vec<String>) -> Result<Vec<QueryResult>, ClientError> {
        let mut results = Vec::with_capacity(queries.len());

        for query in queries {
            let result = self.execute_query(&query).await?;
            results.push(result);
        }

        Ok(results)
    }

    /// Begin a transaction
    pub async fn begin_transaction(&mut self) -> Result<TransactionHandle, ClientError> {
        let result = self.execute_query("BEGIN").await?;
        Ok(TransactionHandle {
            client: self,
            active: true,
        })
    }

    /// Get client statistics
    pub fn stats(&self) -> &ClientStats {
        &self.stats
    }

    /// Get connection pool statistics
    pub fn pool_stats(&self) -> &PoolStats {
        self.pool.stats()
    }

    /// Close all connections and cleanup
    pub async fn close(&mut self) -> Result<(), ClientError> {
        // Connection pool will be cleaned up automatically
        Ok(())
    }
}

/// Query execution result
#[derive(Debug, Clone)]
pub struct QueryResult {
    pub data: Vec<String>,
    pub row_count: usize,
    pub execution_time_ms: f64,
    pub columns: Vec<String>,
}

/// Vector query result
#[derive(Debug, Clone)]
pub struct VectorResult {
    pub vectors: Vec<Vec<f32>>,
    pub distances: Vec<f32>,
    pub execution_time_ms: f64,
    pub result_text: String,
}

/// Transaction handle for managing transactions
pub struct TransactionHandle<'a> {
    client: &'a mut AuroraClient,
    active: bool,
}

impl<'a> TransactionHandle<'a> {
    /// Execute a query within the transaction
    pub async fn execute(&mut self, sql: &str) -> Result<QueryResult, ClientError> {
        if !self.active {
            return Err(ClientError::TransactionError("Transaction not active".to_string()));
        }
        self.client.execute_query(sql).await
    }

    /// Commit the transaction
    pub async fn commit(mut self) -> Result<(), ClientError> {
        if !self.active {
            return Err(ClientError::TransactionError("Transaction not active".to_string()));
        }

        self.client.execute_query("COMMIT").await?;
        self.active = false;
        Ok(())
    }

    /// Rollback the transaction
    pub async fn rollback(mut self) -> Result<(), ClientError> {
        if !self.active {
            return Err(ClientError::TransactionError("Transaction not active".to_string()));
        }

        self.client.execute_query("ROLLBACK").await?;
        self.active = false;
        Ok(())
    }
}

impl<'a> Drop for TransactionHandle<'a> {
    fn drop(&mut self) {
        if self.active {
            // Auto-rollback on drop (simplified - in practice, this should be async)
            eprintln!("Warning: Transaction was not explicitly committed or rolled back");
        }
    }
}

/// Client operation errors
#[derive(Debug, thiserror::Error)]
pub enum ClientError {
    #[error("Connection error: {0}")]
    ConnectionError(#[from] ConnectionError),

    #[error("Pool error: {0}")]
    PoolError(#[from] PoolError),

    #[error("Query execution failed: {0}")]
    QueryError(String),

    #[error("Transaction error: {0}")]
    TransactionError(String),

    #[error("Protocol error: {0}")]
    ProtocolError(String),

    #[error("Timeout")]
    Timeout,
}
