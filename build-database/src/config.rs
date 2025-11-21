use crate::types::IsolationLevel;

/// Database page constants
pub const PAGE_SIZE: usize = 8192; // 8KB pages (common size)
pub const HEADER_SIZE: usize = 64; // Page header size
pub const MAX_PAYLOAD_SIZE: usize = PAGE_SIZE - HEADER_SIZE;

/// Database configuration constants
pub const MAX_TABLES: usize = 65536; // Maximum tables per database
pub const MAX_COLUMNS_PER_TABLE: usize = 4096; // Maximum columns per table
pub const MAX_CONNECTIONS: usize = 10000; // Maximum concurrent connections
pub const DEFAULT_BUFFER_POOL_SIZE: usize = 128 * 1024 * 1024; // 128MB default buffer pool
pub const MAX_TRANSACTION_TIMEOUT: u64 = 300000; // 5 minutes in milliseconds

/// Database configuration for AuroraDB instance
#[derive(Debug, Clone)]
pub struct DatabaseConfig {
    pub max_connections: usize,
    pub buffer_pool_size: usize,
    pub max_tables: usize,
    pub max_columns_per_table: usize,
    pub default_isolation_level: IsolationLevel,
    pub transaction_timeout_ms: u64,
    pub enable_query_logging: bool,
    pub enable_metrics: bool,
}

impl Default for DatabaseConfig {
    fn default() -> Self {
        Self {
            max_connections: MAX_CONNECTIONS,
            buffer_pool_size: DEFAULT_BUFFER_POOL_SIZE,
            max_tables: MAX_TABLES,
            max_columns_per_table: MAX_COLUMNS_PER_TABLE,
            default_isolation_level: IsolationLevel::ReadCommitted,
            transaction_timeout_ms: MAX_TRANSACTION_TIMEOUT,
            enable_query_logging: true,
            enable_metrics: true,
        }
    }
}

/// Connection configuration for client connections
#[derive(Debug, Clone)]
pub struct ConnectionConfig {
    pub host: String,
    pub port: u16,
    pub database_name: String,
    pub username: String,
    pub password: Option<String>,
    pub ssl_mode: SslMode,
    pub connection_timeout_ms: u64,
    pub max_pool_size: usize,
}

/// SSL/TLS connection modes
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum SslMode {
    Disabled,
    Preferred,
    Required,
    VerifyCa,
    VerifyFull,
}
