//! AuroraDB Server Implementation
//!
//! High-performance database server with connection pooling and PostgreSQL protocol support.
//! Handles multiple concurrent client connections efficiently.

use std::sync::Arc;
use tokio::net::TcpListener;
use tokio::time::{self, Duration};

use crate::engine::AuroraDB;
use crate::network::{PostgresProtocol, ConnectionPool, ConnectionPoolManager, ConnectionPoolConfig};

/// AuroraDB server configuration
#[derive(Debug, Clone)]
pub struct ServerConfig {
    pub address: String,
    pub port: u16,
    pub max_connections: usize,
    pub connection_pool_config: ConnectionPoolConfig,
    pub health_check_interval: Duration,
}

impl Default for ServerConfig {
    fn default() -> Self {
        Self {
            address: "127.0.0.1".to_string(),
            port: 5432, // Default PostgreSQL port
            max_connections: 1000,
            connection_pool_config: ConnectionPoolConfig::default(),
            health_check_interval: Duration::from_secs(30),
        }
    }
}

/// AuroraDB database server
pub struct AuroraServer {
    config: ServerConfig,
    db: Arc<AuroraDB>,
    connection_pool_manager: Arc<ConnectionPoolManager>,
}

impl AuroraServer {
    /// Create a new AuroraDB server
    pub fn new(db: Arc<AuroraDB>, config: ServerConfig) -> Self {
        let connection_pool_manager = Arc::new(ConnectionPoolManager::new(config.connection_pool_config.clone()));

        Self {
            config,
            db,
            connection_pool_manager,
        }
    }

    /// Start the server
    pub async fn start(&self) -> Result<(), Box<dyn std::error::Error>> {
        let address = format!("{}:{}", self.config.address, self.config.port);
        let listener = TcpListener::bind(&address).await?;

        log::info!("🚀 AuroraDB server starting on {}", address);
        log::info!("📊 Max connections: {}", self.config.max_connections);
        log::info!("🔄 Connection pool: {} min, {} max",
                  self.config.connection_pool_config.min_connections,
                  self.config.connection_pool_config.max_connections);

        // Start health check task
        let pool_manager = Arc::clone(&self.connection_pool_manager);
        let health_interval = self.config.health_check_interval;
        tokio::spawn(async move {
            let mut interval = time::interval(health_interval);
            loop {
                interval.tick().await;
                pool_manager.health_check_all();
                log::debug!("Performed connection pool health check");
            }
        });

        // Main connection acceptance loop
        let mut connection_count = 0;

        loop {
            match listener.accept().await {
                Ok((socket, addr)) => {
                    connection_count += 1;
                    log::info!("📥 Connection #{} from {}", connection_count, addr);

                    // Check connection limit
                    if connection_count > self.config.max_connections {
                        log::warn!("⚠️  Connection limit exceeded, rejecting connection from {}", addr);
                        continue;
                    }

                    // Get connection pool for this database
                    let pool = self.connection_pool_manager.get_pool("default", Arc::clone(&self.db));

                    // Create protocol handler
                    let protocol = PostgresProtocol::new(Arc::clone(&self.db));

                    // Handle connection in separate task
                    tokio::spawn(async move {
                        if let Err(e) = protocol.handle_connection(socket).await {
                            log::error!("❌ Connection error: {}", e);
                        }
                    });
                }
                Err(e) => {
                    log::error!("❌ Accept error: {}", e);
                    // Continue accepting connections despite errors
                }
            }

            // Periodic stats logging
            if connection_count % 10 == 0 {
                self.log_server_stats();
            }
        }
    }

    /// Get server statistics
    pub fn get_stats(&self) -> ServerStats {
        let pool_stats = self.connection_pool_manager.get_all_stats();
        let default_pool_stats = pool_stats.get("default").cloned().unwrap_or_default();

        ServerStats {
            address: format!("{}:{}", self.config.address, self.config.port),
            max_connections: self.config.max_connections,
            connection_pool_stats: default_pool_stats,
            uptime_seconds: 0, // Would need to track actual uptime
        }
    }

    /// Log server statistics
    fn log_server_stats(&self) {
        let stats = self.get_stats();
        log::info!("📊 Server Stats: {} connections ({} available, {} max)",
                  stats.connection_pool_stats.total_connections,
                  stats.connection_pool_stats.available_connections,
                  stats.max_connections);
    }
}

/// Server statistics
#[derive(Debug, Clone)]
pub struct ServerStats {
    pub address: String,
    pub max_connections: usize,
    pub connection_pool_stats: crate::network::ConnectionPoolStats,
    pub uptime_seconds: u64,
}

/// Server builder for fluent configuration
pub struct ServerBuilder {
    config: ServerConfig,
}

impl ServerBuilder {
    pub fn new() -> Self {
        Self {
            config: ServerConfig::default(),
        }
    }

    pub fn address(mut self, address: impl Into<String>) -> Self {
        self.config.address = address.into();
        self
    }

    pub fn port(mut self, port: u16) -> Self {
        self.config.port = port;
        self
    }

    pub fn max_connections(mut self, max: usize) -> Self {
        self.config.max_connections = max;
        self
    }

    pub fn connection_pool_config(mut self, pool_config: ConnectionPoolConfig) -> Self {
        self.config.connection_pool_config = pool_config;
        self
    }

    pub fn build(self, db: Arc<AuroraDB>) -> AuroraServer {
        AuroraServer::new(db, self.config)
    }
}

impl Default for ServerBuilder {
    fn default() -> Self {
        Self::new()
    }
}