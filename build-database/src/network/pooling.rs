//! Connection Pooling
//!
//! Efficient connection reuse and management.
//! Prevents connection overhead and resource exhaustion.

use crate::core::*;
use super::connection::*;
use std::collections::{HashMap, VecDeque};
use std::sync::Arc;
use parking_lot::RwLock;
use tokio::sync::Semaphore;
use std::time::{Duration, Instant};

/// Connection pool for efficient connection reuse
pub struct ConnectionPool {
    /// Available connections
    available: RwLock<VecDeque<Arc<RwLock<Connection>>>>,
    /// In-use connections
    in_use: RwLock<HashMap<u64, Arc<RwLock<Connection>>>>,
    /// Connection factory
    factory: ConnectionFactory,
    /// Pool configuration
    config: PoolConfig,
    /// Connection semaphore for limiting concurrent connections
    semaphore: Arc<Semaphore>,
    /// Pool statistics
    stats: PoolStats,
}

/// Pool configuration
#[derive(Debug, Clone)]
pub struct PoolConfig {
    pub max_connections: usize,
    pub min_connections: usize,
    pub max_idle_time_ms: u64,
    pub connection_timeout_ms: u64,
    pub health_check_interval_ms: u64,
    pub connection_config: ConnectionConfig,
}

/// Pool statistics
#[derive(Debug, Clone, Default)]
pub struct PoolStats {
    pub total_connections_created: u64,
    pub total_connections_destroyed: u64,
    pub connections_acquired: u64,
    pub connections_released: u64,
    pub connections_timed_out: u64,
    pub pool_exhaustions: u64,
    pub average_wait_time_ms: f64,
}

/// Connection factory trait
pub trait ConnectionFactory: Send + Sync {
    fn create_connection(&self) -> impl std::future::Future<Output = Result<Connection, ConnectionError>> + Send;
}

/// Default TCP connection factory
pub struct TcpConnectionFactory {
    config: ConnectionConfig,
}

impl TcpConnectionFactory {
    pub fn new(config: ConnectionConfig) -> Self {
        Self { config }
    }
}

impl ConnectionFactory for TcpConnectionFactory {
    async fn create_connection(&self) -> Result<Connection, ConnectionError> {
        use tokio::net::TcpStream;
        use tokio::time::{timeout, Duration};

        let address = format!("{}:{}", self.config.host, self.config.port);
        let connect_future = TcpStream::connect(&address);
        let stream = timeout(Duration::from_millis(self.config.connection_timeout_ms), connect_future)
            .await
            .map_err(|_| ConnectionError::Timeout)??;

        Connection::new(stream, self.config.clone()).await
    }
}

impl ConnectionPool {
    /// Create a new connection pool
    pub fn new(config: PoolConfig, factory: Box<dyn ConnectionFactory>) -> Self {
        Self {
            available: RwLock::new(VecDeque::new()),
            in_use: RwLock::new(HashMap::new()),
            factory: factory,
            config,
            semaphore: Arc::new(Semaphore::new(config.max_connections)),
            stats: PoolStats::default(),
        }
    }

    /// Get a connection from the pool
    pub async fn get_connection(&mut self) -> Result<PooledConnection, PoolError> {
        let start_time = Instant::now();

        // Acquire semaphore permit
        let permit = self.semaphore.clone().acquire_owned().await
            .map_err(|_| PoolError::PoolExhausted)?;

        // Try to get an available connection
        if let Some(connection) = self.available.write().pop_front() {
            let mut conn = connection.write();
            if self.is_connection_valid(&conn) {
                let id = conn.id;
                self.in_use.write().insert(id, connection.clone());
                self.stats.connections_acquired += 1;

                let wait_time = start_time.elapsed().as_millis() as f64;
                self.stats.average_wait_time_ms =
                    (self.stats.average_wait_time_ms * (self.stats.connections_acquired - 1) as f64 + wait_time)
                        / self.stats.connections_acquired as f64;

                return Ok(PooledConnection::new(connection, permit, self));
            } else {
                // Connection is invalid, clean it up
                self.stats.total_connections_destroyed += 1;
            }
        }

        // Create a new connection if we haven't reached the maximum
        let current_total = self.available.read().len() + self.in_use.read().len();
        if current_total < self.config.max_connections {
            match self.factory.create_connection().await {
                Ok(mut conn) => {
                    conn.handshake().await?;
                    let connection = Arc::new(RwLock::new(conn));
                    let id = connection.read().id;
                    self.in_use.write().insert(id, connection.clone());
                    self.stats.total_connections_created += 1;
                    self.stats.connections_acquired += 1;

                    let wait_time = start_time.elapsed().as_millis() as f64;
                    self.stats.average_wait_time_ms =
                        (self.stats.average_wait_time_ms * (self.stats.connections_acquired - 1) as f64 + wait_time)
                            / self.stats.connections_acquired as f64;

                    Ok(PooledConnection::new(connection, permit, self))
                }
                Err(e) => {
                    self.stats.pool_exhaustions += 1;
                    Err(PoolError::ConnectionError(e))
                }
            }
        } else {
            // Pool exhausted
            self.stats.pool_exhaustions += 1;
            Err(PoolError::PoolExhausted)
        }
    }

    /// Return a connection to the pool
    pub fn return_connection(&mut self, connection: Arc<RwLock<Connection>>) {
        let mut conn = connection.write();
        let id = conn.id;

        // Remove from in-use map
        self.in_use.write().remove(&id);

        // Check if connection is still valid
        if self.is_connection_valid(&conn) && !self.is_pool_full() {
            // Return to available pool
            self.available.write().push_back(connection);
        } else {
            // Connection is invalid or pool is full, destroy it
            self.stats.total_connections_destroyed += 1;
        }

        self.stats.connections_released += 1;
    }

    /// Check if connection is valid
    fn is_connection_valid(&self, connection: &Connection) -> bool {
        // Check if connection is not closed and not idle too long
        matches!(connection.state(), ConnectionState::Ready | ConnectionState::Authenticated)
            && !connection.is_idle()
    }

    /// Check if available pool is at capacity
    fn is_pool_full(&self) -> bool {
        self.available.read().len() >= self.config.min_connections
    }

    /// Get pool statistics
    pub fn stats(&self) -> &PoolStats {
        &self.stats
    }

    /// Get current pool size
    pub fn size(&self) -> PoolSize {
        let available = self.available.read().len();
        let in_use = self.in_use.read().len();
        let total = available + in_use;

        PoolSize {
            available,
            in_use,
            total,
            max: self.config.max_connections,
        }
    }

    /// Perform maintenance (cleanup idle connections, health checks)
    pub async fn maintain(&mut self) {
        let mut to_remove = Vec::new();

        // Check available connections
        {
            let mut available = self.available.write();
            let mut i = 0;
            while i < available.len() {
                let conn = available[i].read();
                if !self.is_connection_valid(&conn) {
                    to_remove.push(available.remove(i).unwrap());
                } else {
                    i += 1;
                }
            }
        }

        // Clean up invalid connections
        self.stats.total_connections_destroyed += to_remove.len();

        // Ensure minimum connections are available
        let current_available = self.available.read().len();
        let current_total = current_available + self.in_use.read().len();

        for _ in current_total..self.config.min_connections {
            if let Ok(conn) = self.factory.create_connection().await {
                if let Ok(mut conn) = conn {
                    if conn.handshake().await.is_ok() {
                        self.available.write().push_back(Arc::new(RwLock::new(conn)));
                        self.stats.total_connections_created += 1;
                    }
                }
            }
        }
    }
}

/// Current pool size information
#[derive(Debug, Clone)]
pub struct PoolSize {
    pub available: usize,
    pub in_use: usize,
    pub total: usize,
    pub max: usize,
}

/// Pooled connection wrapper
pub struct PooledConnection<'a> {
    connection: Arc<RwLock<Connection>>,
    _permit: tokio::sync::OwnedSemaphorePermit,
    pool: &'a mut ConnectionPool,
}

impl<'a> PooledConnection<'a> {
    fn new(
        connection: Arc<RwLock<Connection>>,
        permit: tokio::sync::OwnedSemaphorePermit,
        pool: &'a mut ConnectionPool,
    ) -> Self {
        Self {
            connection,
            _permit: permit,
            pool,
        }
    }

    /// Get reference to the underlying connection
    pub fn connection(&self) -> &Arc<RwLock<Connection>> {
        &self.connection
    }

    /// Send a message using the connection
    pub async fn send_message(&mut self, message: &AuroraMessage) -> Result<(), ConnectionError> {
        self.connection.write().send_message(message).await
    }

    /// Receive a message using the connection
    pub async fn receive_message(&mut self) -> Result<AuroraMessage, ConnectionError> {
        self.connection.write().receive_message().await
    }
}

impl<'a> Drop for PooledConnection<'a> {
    fn drop(&mut self) {
        // Return connection to pool when dropped
        self.pool.return_connection(self.connection.clone());
    }
}

/// Pool operation errors
#[derive(Debug, thiserror::Error)]
pub enum PoolError {
    #[error("Connection pool exhausted")]
    PoolExhausted,

    #[error("Connection creation failed: {0}")]
    ConnectionError(#[from] ConnectionError),

    #[error("Pool timeout")]
    Timeout,
}
