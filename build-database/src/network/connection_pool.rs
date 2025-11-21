//! Connection Pool Implementation
//!
//! Manages a pool of database connections for efficient client handling.
//! Supports connection reuse, health checking, and load balancing.

use std::collections::VecDeque;
use std::sync::{Arc, Mutex};
use tokio::sync::Semaphore;
use std::time::{Duration, Instant};

use crate::engine::AuroraDB;
use crate::security::UserContext;

/// Pooled database connection
pub struct PooledConnection {
    pub db: Arc<AuroraDB>,
    pub user_context: UserContext,
    pub created_at: Instant,
    pub last_used: Instant,
    pub id: u64,
}

/// Connection pool configuration
#[derive(Debug, Clone)]
pub struct ConnectionPoolConfig {
    pub max_connections: usize,
    pub min_connections: usize,
    pub max_idle_time: Duration,
    pub max_lifetime: Duration,
    pub health_check_interval: Duration,
}

impl Default for ConnectionPoolConfig {
    fn default() -> Self {
        Self {
            max_connections: 100,
            min_connections: 10,
            max_idle_time: Duration::from_secs(300), // 5 minutes
            max_lifetime: Duration::from_secs(3600), // 1 hour
            health_check_interval: Duration::from_secs(30),
        }
    }
}

/// Database connection pool
pub struct ConnectionPool {
    config: ConnectionPoolConfig,
    db: Arc<AuroraDB>,
    available: Mutex<VecDeque<PooledConnection>>,
    semaphore: Arc<Semaphore>,
    next_id: std::sync::atomic::AtomicU64,
}

impl ConnectionPool {
    /// Create a new connection pool
    pub fn new(db: Arc<AuroraDB>, config: ConnectionPoolConfig) -> Self {
        let pool = Self {
            config,
            db,
            available: Mutex::new(VecDeque::new()),
            semaphore: Arc::new(Semaphore::new(config.max_connections)),
            next_id: std::sync::atomic::AtomicU64::new(1),
        };

        // Pre-populate with minimum connections
        pool.initialize_connections();

        pool
    }

    /// Get a connection from the pool
    pub async fn get_connection(&self, user_context: UserContext) -> Result<PooledConnection, Box<dyn std::error::Error>> {
        // Acquire semaphore permit
        let _permit = self.semaphore.acquire().await?;

        // Try to get an existing connection
        if let Some(mut conn) = self.available.lock().unwrap().pop_front() {
            // Check if connection is still valid
            if self.is_connection_valid(&conn) {
                conn.last_used = Instant::now();
                return Ok(conn);
            }
            // Connection is stale, create a new one
        }

        // Create a new connection
        Ok(self.create_connection(user_context))
    }

    /// Return a connection to the pool
    pub fn return_connection(&self, mut conn: PooledConnection) {
        conn.last_used = Instant::now();

        let mut available = self.available.lock().unwrap();
        if available.len() < self.config.max_connections {
            available.push_back(conn);
        }
        // If pool is full, connection will be dropped
    }

    /// Get pool statistics
    pub fn stats(&self) -> ConnectionPoolStats {
        let available = self.available.lock().unwrap();
        ConnectionPoolStats {
            total_connections: available.len(),
            available_connections: available.len(),
            max_connections: self.config.max_connections,
            min_connections: self.config.min_connections,
        }
    }

    /// Health check and cleanup stale connections
    pub fn health_check(&self) {
        let mut available = self.available.lock().unwrap();
        let now = Instant::now();

        // Remove stale connections
        available.retain(|conn| {
            let age = now.duration_since(conn.created_at);
            let idle_time = now.duration_since(conn.last_used);

            age < self.config.max_lifetime && idle_time < self.config.max_idle_time
        });

        // Ensure minimum connections
        let current_count = available.len();
        if current_count < self.config.min_connections {
            drop(available); // Release lock before creating new connections
            self.initialize_connections();
        }
    }

    /// Create a new connection
    fn create_connection(&self, user_context: UserContext) -> PooledConnection {
        let id = self.next_id.fetch_add(1, std::sync::atomic::Ordering::SeqCst);
        PooledConnection {
            db: Arc::clone(&self.db),
            user_context,
            created_at: Instant::now(),
            last_used: Instant::now(),
            id,
        }
    }

    /// Check if a connection is still valid
    fn is_connection_valid(&self, conn: &PooledConnection) -> bool {
        let now = Instant::now();
        let age = now.duration_since(conn.created_at);
        let idle_time = now.duration_since(conn.last_used);

        age < self.config.max_lifetime && idle_time < self.config.max_idle_time
    }

    /// Initialize minimum number of connections
    fn initialize_connections(&self) {
        let mut available = self.available.lock().unwrap();
        let current_count = available.len();

        for _ in current_count..self.config.min_connections {
            let user_context = UserContext::system_user(); // Default user
            let conn = self.create_connection(user_context);
            available.push_back(conn);
        }
    }
}

/// Connection pool statistics
#[derive(Debug, Clone)]
pub struct ConnectionPoolStats {
    pub total_connections: usize,
    pub available_connections: usize,
    pub max_connections: usize,
    pub min_connections: usize,
}

/// Connection pool manager
pub struct ConnectionPoolManager {
    pools: Mutex<std::collections::HashMap<String, Arc<ConnectionPool>>>,
    default_config: ConnectionPoolConfig,
}

impl ConnectionPoolManager {
    pub fn new(default_config: ConnectionPoolConfig) -> Self {
        Self {
            pools: Mutex::new(std::collections::HashMap::new()),
            default_config,
        }
    }

    /// Get or create a connection pool for a database
    pub fn get_pool(&self, db_name: &str, db: Arc<AuroraDB>) -> Arc<ConnectionPool> {
        let mut pools = self.pools.lock().unwrap();

        if let Some(pool) = pools.get(db_name) {
            Arc::clone(pool)
        } else {
            let pool = Arc::new(ConnectionPool::new(db, self.default_config.clone()));
            pools.insert(db_name.to_string(), Arc::clone(&pool));
            pool
        }
    }

    /// Get statistics for all pools
    pub fn get_all_stats(&self) -> std::collections::HashMap<String, ConnectionPoolStats> {
        let pools = self.pools.lock().unwrap();
        let mut stats = std::collections::HashMap::new();

        for (name, pool) in pools.iter() {
            stats.insert(name.clone(), pool.stats());
        }

        stats
    }

    /// Health check all pools
    pub fn health_check_all(&self) {
        let pools = self.pools.lock().unwrap();
        for (_name, pool) in pools.iter() {
            pool.health_check();
        }
    }
}

/// RAII wrapper for pooled connections
pub struct PooledConnectionGuard<'a> {
    connection: Option<PooledConnection>,
    pool: &'a ConnectionPool,
}

impl<'a> PooledConnectionGuard<'a> {
    pub fn new(connection: PooledConnection, pool: &'a ConnectionPool) -> Self {
        Self {
            connection: Some(connection),
            pool,
        }
    }

    pub fn connection(&self) -> &PooledConnection {
        self.connection.as_ref().unwrap()
    }

    pub fn connection_mut(&mut self) -> &mut PooledConnection {
        self.connection.as_mut().unwrap()
    }
}

impl<'a> Drop for PooledConnectionGuard<'a> {
    fn drop(&mut self) {
        if let Some(conn) = self.connection.take() {
            self.pool.return_connection(conn);
        }
    }
}

impl<'a> std::ops::Deref for PooledConnectionGuard<'a> {
    type Target = PooledConnection;

    fn deref(&self) -> &Self::Target {
        self.connection.as_ref().unwrap()
    }
}

impl<'a> std::ops::DerefMut for PooledConnectionGuard<'a> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        self.connection.as_mut().unwrap()
    }
}
