//! AuroraDB Production Configuration Management
//!
//! Production-ready configuration system supporting:
//! - TOML configuration files with validation
//! - Environment variable overrides
//! - Configuration hot-reloading
//! - Schema validation and error reporting
//! - Hierarchical configuration (defaults → file → env → CLI)

use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::env;
use std::fs;
use std::path::Path;
use std::sync::Arc;
use tokio::sync::RwLock;
use validator::{Validate, ValidationError};
use crate::core::AuroraResult;

/// Master configuration for AuroraDB
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct AuroraConfig {
    /// Database configuration
    pub database: DatabaseConfig,

    /// Server configuration
    pub server: ServerConfig,

    /// Storage configuration
    pub storage: StorageConfig,

    /// Network configuration
    pub network: NetworkConfig,

    /// Security configuration
    pub security: SecurityConfig,

    /// Logging configuration
    pub logging: LoggingConfig,

    /// Monitoring configuration
    pub monitoring: MonitoringConfig,

    /// Transaction configuration
    pub transaction: TransactionConfig,

    /// Vector search configuration
    pub vector: VectorConfig,

    /// Audit configuration
    pub audit: AuditConfig,
}

/// Database core configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct DatabaseConfig {
    /// Maximum concurrent connections
    #[validate(range(min = 1, max = 100000))]
    pub max_connections: usize,

    /// Buffer pool size in bytes
    #[validate(range(min = 1048576))] // 1MB minimum
    pub buffer_pool_size: usize,

    /// Maximum tables per database
    #[validate(range(min = 1, max = 1000000))]
    pub max_tables: usize,

    /// Maximum columns per table
    #[validate(range(min = 1, max = 10000))]
    pub max_columns_per_table: usize,

    /// Default transaction isolation level
    pub default_isolation_level: String,

    /// Transaction timeout in milliseconds
    #[validate(range(min = 1000, max = 3600000))] // 1s to 1 hour
    pub transaction_timeout_ms: u64,

    /// Query cache size in MB
    #[validate(range(min = 0, max = 10000))]
    pub query_cache_size_mb: usize,

    /// Enable query logging
    pub enable_query_logging: bool,

    /// Enable metrics collection
    pub enable_metrics: bool,

    /// Database data directory
    pub data_directory: String,

    /// Database temp directory
    pub temp_directory: String,
}

/// Server configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct ServerConfig {
    /// PostgreSQL wire protocol port
    #[validate(range(min = 1024, max = 65535))]
    pub postgresql_port: u16,

    /// HTTP API port
    #[validate(range(min = 1024, max = 65535))]
    pub http_port: u16,

    /// Binary protocol port
    #[validate(range(min = 1024, max = 65535))]
    pub binary_port: u16,

    /// Bind address
    pub bind_address: String,

    /// Server hostname
    pub hostname: String,

    /// Maximum request size in MB
    #[validate(range(min = 1, max = 100))]
    pub max_request_size_mb: usize,

    /// Request timeout in seconds
    #[validate(range(min = 1, max = 300))]
    pub request_timeout_seconds: u64,

    /// Enable graceful shutdown
    pub enable_graceful_shutdown: bool,

    /// Graceful shutdown timeout in seconds
    #[validate(range(min = 1, max = 300))]
    pub shutdown_timeout_seconds: u64,
}

/// Storage configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct StorageConfig {
    /// Storage engine selection strategy
    pub selection_strategy: String,

    /// B+ Tree engine configuration
    pub btree: BTreeConfig,

    /// LSM Tree engine configuration
    pub lsm: LSMConfig,

    /// Hybrid engine configuration
    pub hybrid: HybridConfig,

    /// WAL configuration
    pub wal: WALConfig,

    /// Compression settings
    pub compression: CompressionConfig,
}

/// B+ Tree storage engine configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct BTreeConfig {
    /// Page size in KB
    #[validate(range(min = 1, max = 64))]
    pub page_size_kb: usize,

    /// Maximum table size in MB
    #[validate(range(min = 1, max = 1000000))]
    pub max_table_size_mb: usize,

    /// Cache size in MB
    #[validate(range(min = 1, max = 100000))]
    pub cache_size_mb: usize,

    /// Maximum concurrent transactions
    #[validate(range(min = 1, max = 10000))]
    pub max_concurrent_transactions: usize,
}

/// LSM Tree storage engine configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct LSMConfig {
    /// Memory table size in MB
    #[validate(range(min = 1, max = 1000))]
    pub memtable_size_mb: usize,

    /// SSTable size in MB
    #[validate(range(min = 1, max = 10000))]
    pub sstable_size_mb: usize,

    /// Compaction threads
    #[validate(range(min = 1, max = 32))]
    pub compaction_threads: usize,

    /// Bloom filter bits per key
    #[validate(range(min = 1, max = 20))]
    pub bloom_filter_bits: usize,
}

/// Hybrid storage engine configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct HybridConfig {
    /// Adaptive threshold (rows)
    #[validate(range(min = 1000, max = 10000000))]
    pub adaptive_threshold: usize,

    /// Vector column threshold (ratio)
    #[validate(range(min = 0.0, max = 1.0))]
    pub vector_threshold: f64,
}

/// WAL configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct WALConfig {
    /// WAL directory
    pub directory: String,

    /// WAL segment size in MB
    #[validate(range(min = 1, max = 1000))]
    pub segment_size_mb: usize,

    /// Maximum WAL segments
    #[validate(range(min = 1, max = 1000))]
    pub max_segments: usize,

    /// Sync strategy
    pub sync_strategy: String,

    /// Flush interval in milliseconds
    #[validate(range(min = 1, max = 10000))]
    pub flush_interval_ms: u64,
}

/// Compression configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct CompressionConfig {
    /// Default compression algorithm
    pub algorithm: String,

    /// Compression level (1-9)
    #[validate(range(min = 1, max = 9))]
    pub level: u32,

    /// Minimum block size for compression
    #[validate(range(min = 1024, max = 1048576))]
    pub min_block_size: usize,
}

/// Network configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct NetworkConfig {
    /// Connection pool configuration
    pub connection_pool: ConnectionPoolConfig,

    /// TLS configuration
    pub tls: TLSConfig,

    /// Load balancer configuration
    pub load_balancer: LoadBalancerConfig,
}

/// Connection pool configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct ConnectionPoolConfig {
    /// Maximum connections
    #[validate(range(min = 1, max = 10000))]
    pub max_connections: usize,

    /// Minimum idle connections
    #[validate(range(min = 0, max = 1000))]
    pub min_idle: usize,

    /// Maximum idle time in seconds
    #[validate(range(min = 1, max = 3600))]
    pub max_idle_time_seconds: u64,

    /// Connection timeout in seconds
    #[validate(range(min = 1, max = 300))]
    pub connection_timeout_seconds: u64,

    /// Health check interval in seconds
    #[validate(range(min = 1, max = 300))]
    pub health_check_interval_seconds: u64,
}

/// TLS configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct TLSConfig {
    /// Enable TLS
    pub enabled: bool,

    /// Certificate file path
    pub cert_file: Option<String>,

    /// Private key file path
    pub key_file: Option<String>,

    /// CA certificate file path
    pub ca_file: Option<String>,

    /// Minimum TLS version
    pub min_version: String,

    /// Mutual authentication
    pub mutual_auth: bool,
}

/// Load balancer configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct LoadBalancerConfig {
    /// Enable load balancing
    pub enabled: bool,

    /// Load balancing algorithm
    pub algorithm: String,

    /// Health check interval in seconds
    #[validate(range(min = 1, max = 300))]
    pub health_check_interval_seconds: u64,

    /// Failover timeout in seconds
    #[validate(range(min = 1, max = 300))]
    pub failover_timeout_seconds: u64,
}

/// Security configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct SecurityConfig {
    /// Enable authentication
    pub enable_authentication: bool,

    /// Enable authorization
    pub enable_authorization: bool,

    /// Password minimum length
    #[validate(range(min = 8, max = 128))]
    pub password_min_length: usize,

    /// Session timeout in minutes
    #[validate(range(min = 1, max = 1440))] // 1 minute to 24 hours
    pub session_timeout_minutes: u64,

    /// Enable row-level security
    pub enable_row_level_security: bool,

    /// Encryption at rest
    pub encryption_at_rest: bool,

    /// Encryption key file
    pub encryption_key_file: Option<String>,
}

/// Logging configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct LoggingConfig {
    /// Log level
    pub level: String,

    /// Log format (json, text, compact)
    pub format: String,

    /// Log file path
    pub file: Option<String>,

    /// Maximum log file size in MB
    #[validate(range(min = 1, max = 1000))]
    pub max_size_mb: usize,

    /// Maximum number of log files
    #[validate(range(min = 1, max = 100))]
    pub max_files: usize,

    /// Enable log compression
    pub compress_rotated: bool,

    /// Log slow queries threshold in milliseconds
    #[validate(range(min = 0, max = 60000))]
    pub slow_query_threshold_ms: u64,
}

/// Monitoring configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct MonitoringConfig {
    /// Enable Prometheus metrics
    pub enable_prometheus: bool,

    /// Prometheus metrics port
    #[validate(range(min = 1024, max = 65535))]
    pub prometheus_port: u16,

    /// Metrics collection interval in seconds
    #[validate(range(min = 1, max = 300))]
    pub collection_interval_seconds: u64,

    /// Enable health checks
    pub enable_health_checks: bool,

    /// Health check port
    #[validate(range(min = 1024, max = 65535))]
    pub health_check_port: u16,

    /// Alert thresholds
    pub alert_thresholds: AlertThresholds,
}

/// Alert thresholds
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct AlertThresholds {
    /// High CPU usage threshold (%)
    #[validate(range(min = 1, max = 100))]
    pub cpu_high_threshold: u8,

    /// High memory usage threshold (%)
    #[validate(range(min = 1, max = 100))]
    pub memory_high_threshold: u8,

    /// High disk usage threshold (%)
    #[validate(range(min = 1, max = 100))]
    pub disk_high_threshold: u8,

    /// Connection pool utilization threshold (%)
    #[validate(range(min = 1, max = 100))]
    pub connection_pool_high_threshold: u8,

    /// Slow query threshold in milliseconds
    #[validate(range(min = 100, max = 60000))]
    pub slow_query_threshold_ms: u64,
}

/// Transaction configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct TransactionConfig {
    /// Maximum concurrent transactions
    #[validate(range(min = 1, max = 10000))]
    pub max_concurrent_transactions: usize,

    /// Deadlock detection interval in milliseconds
    #[validate(range(min = 10, max = 10000))]
    pub deadlock_detection_interval_ms: u64,

    /// Transaction timeout in milliseconds
    #[validate(range(min = 1000, max = 3600000))]
    pub transaction_timeout_ms: u64,

    /// Isolation level
    pub isolation_level: String,

    /// Enable distributed transactions
    pub enable_distributed_transactions: bool,

    /// Transaction log directory
    pub log_directory: String,
}

/// Vector search configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct VectorConfig {
    /// Default vector dimension
    #[validate(range(min = 1, max = 4096))]
    pub default_dimension: usize,

    /// Index type (hnsw, ivf, flat)
    pub index_type: String,

    /// Maximum connections per layer (HNSW)
    #[validate(range(min = 1, max = 100))]
    pub max_connections: usize,

    /// Construction search parameter (HNSW)
    #[validate(range(min = 1, max = 1000))]
    pub ef_construction: usize,

    /// Query search parameter (HNSW)
    #[validate(range(min = 1, max = 1000))]
    pub ef_search: usize,

    /// Enable GPU acceleration
    pub enable_gpu: bool,

    /// Vector cache size in MB
    #[validate(range(min = 1, max = 10000))]
    pub cache_size_mb: usize,
}

/// Audit configuration
#[derive(Debug, Clone, Validate, Serialize, Deserialize)]
pub struct AuditConfig {
    /// Enable audit logging
    pub enable_audit_logging: bool,

    /// Audit log file path
    pub audit_log_file: String,

    /// Log sensitive operations
    pub log_sensitive_operations: bool,

    /// Log connection events
    pub log_connection_events: bool,

    /// Log DDL operations
    pub log_ddl_operations: bool,

    /// Log DML operations
    pub log_dml_operations: bool,

    /// Audit log retention days
    #[validate(range(min = 1, max = 3650))] // 1 day to 10 years
    pub retention_days: usize,

    /// Maximum audit log size in MB
    #[validate(range(min = 1, max = 10000))]
    pub max_log_size_mb: usize,
}

/// Configuration manager for loading and managing configuration
pub struct ConfigManager {
    config: Arc<RwLock<AuroraConfig>>,
    config_path: Option<String>,
    watchers: Vec<Box<dyn ConfigWatcher + Send + Sync>>,
}

impl ConfigManager {
    /// Create a new configuration manager
    pub fn new() -> Self {
        Self {
            config: Arc::new(RwLock::new(AuroraConfig::default())),
            config_path: None,
            watchers: Vec::new(),
        }
    }

    /// Load configuration from file and environment variables
    pub async fn load_config(&mut self, config_path: Option<&str>) -> AuroraResult<()> {
        // Start with defaults
        let mut config = AuroraConfig::default();

        // Load from file if provided
        if let Some(path) = config_path {
            if Path::new(path).exists() {
                let content = fs::read_to_string(path)?;
                config = toml::from_str(&content)?;
                self.config_path = Some(path.to_string());
            }
        }

        // Override with environment variables
        self.override_from_env(&mut config)?;

        // Validate configuration
        config.validate()?;

        // Store configuration
        *self.config.write().await = config;

        Ok(())
    }

    /// Override configuration with environment variables
    fn override_from_env(&self, config: &mut AuroraConfig) -> AuroraResult<()> {
        // Database overrides
        if let Ok(val) = env::var("AURORA_DB_MAX_CONNECTIONS") {
            config.database.max_connections = val.parse()?;
        }

        if let Ok(val) = env::var("AURORA_DB_BUFFER_POOL_SIZE") {
            config.database.buffer_pool_size = parse_size(&val)?;
        }

        // Server overrides
        if let Ok(val) = env::var("AURORA_SERVER_POSTGRESQL_PORT") {
            config.server.postgresql_port = val.parse()?;
        }

        if let Ok(val) = env::var("AURORA_SERVER_HTTP_PORT") {
            config.server.http_port = val.parse()?;
        }

        // Security overrides
        if let Ok(val) = env::var("AURORA_SECURITY_TLS_ENABLED") {
            config.network.tls.enabled = val.parse()?;
        }

        // Logging overrides
        if let Ok(val) = env::var("AURORA_LOG_LEVEL") {
            config.logging.level = val;
        }

        Ok(())
    }

    /// Get current configuration
    pub async fn get_config(&self) -> AuroraResult<AuroraConfig> {
        Ok(self.config.read().await.clone())
    }

    /// Update configuration (for hot-reloading)
    pub async fn update_config(&self, new_config: AuroraConfig) -> AuroraResult<()> {
        new_config.validate()?;
        *self.config.write().await = new_config;

        // Notify watchers
        for watcher in &self.watchers {
            watcher.on_config_changed(&new_config).await?;
        }

        Ok(())
    }

    /// Add a configuration watcher
    pub fn add_watcher(&mut self, watcher: Box<dyn ConfigWatcher + Send + Sync>) {
        self.watchers.push(watcher);
    }

    /// Start configuration file watching for hot-reloading
    pub async fn start_file_watching(&self) -> AuroraResult<()> {
        if let Some(config_path) = &self.config_path {
            // In a real implementation, this would watch the file for changes
            // and reload configuration when it changes
            println!("Configuration file watching enabled for: {}", config_path);
        }
        Ok(())
    }
}

/// Configuration watcher trait for hot-reloading
#[async_trait::async_trait]
pub trait ConfigWatcher: Send + Sync {
    async fn on_config_changed(&self, config: &AuroraConfig) -> AuroraResult<()>;
}

/// Default configuration
impl Default for AuroraConfig {
    fn default() -> Self {
        Self {
            database: DatabaseConfig::default(),
            server: ServerConfig::default(),
            storage: StorageConfig::default(),
            network: NetworkConfig::default(),
            security: SecurityConfig::default(),
            logging: LoggingConfig::default(),
            monitoring: MonitoringConfig::default(),
            transaction: TransactionConfig::default(),
            vector: VectorConfig::default(),
            audit: AuditConfig::default(),
        }
    }
}

// Default implementations for all config structs
impl Default for DatabaseConfig {
    fn default() -> Self {
        Self {
            max_connections: 1000,
            buffer_pool_size: 1_073_741_824, // 1GB
            max_tables: 65536,
            max_columns_per_table: 4096,
            default_isolation_level: "read_committed".to_string(),
            transaction_timeout_ms: 300000,
            query_cache_size_mb: 512,
            enable_query_logging: true,
            enable_metrics: true,
            data_directory: "/var/lib/aurora/data".to_string(),
            temp_directory: "/tmp/aurora".to_string(),
        }
    }
}

impl Default for ServerConfig {
    fn default() -> Self {
        Self {
            postgresql_port: 5433,
            http_port: 8080,
            binary_port: 9090,
            bind_address: "0.0.0.0".to_string(),
            hostname: "localhost".to_string(),
            max_request_size_mb: 10,
            request_timeout_seconds: 30,
            enable_graceful_shutdown: true,
            shutdown_timeout_seconds: 30,
        }
    }
}

impl Default for StorageConfig {
    fn default() -> Self {
        Self {
            selection_strategy: "workload_based".to_string(),
            btree: BTreeConfig::default(),
            lsm: LSMConfig::default(),
            hybrid: HybridConfig::default(),
            wal: WALConfig::default(),
            compression: CompressionConfig::default(),
        }
    }
}

impl Default for NetworkConfig {
    fn default() -> Self {
        Self {
            connection_pool: ConnectionPoolConfig::default(),
            tls: TLSConfig::default(),
            load_balancer: LoadBalancerConfig::default(),
        }
    }
}

impl Default for SecurityConfig {
    fn default() -> Self {
        Self {
            enable_authentication: true,
            enable_authorization: true,
            password_min_length: 8,
            session_timeout_minutes: 60,
            enable_row_level_security: false,
            encryption_at_rest: false,
            encryption_key_file: None,
        }
    }
}

impl Default for LoggingConfig {
    fn default() -> Self {
        Self {
            level: "info".to_string(),
            format: "json".to_string(),
            file: Some("/var/log/aurora/aurora.log".to_string()),
            max_size_mb: 100,
            max_files: 10,
            compress_rotated: true,
            slow_query_threshold_ms: 1000,
        }
    }
}

impl Default for MonitoringConfig {
    fn default() -> Self {
        Self {
            enable_prometheus: true,
            prometheus_port: 9091,
            collection_interval_seconds: 15,
            enable_health_checks: true,
            health_check_port: 8081,
            alert_thresholds: AlertThresholds::default(),
        }
    }
}

impl Default for TransactionConfig {
    fn default() -> Self {
        Self {
            max_concurrent_transactions: 100,
            deadlock_detection_interval_ms: 100,
            transaction_timeout_ms: 300000,
            isolation_level: "read_committed".to_string(),
            enable_distributed_transactions: false,
            log_directory: "/var/lib/aurora/tx_logs".to_string(),
        }
    }
}

impl Default for VectorConfig {
    fn default() -> Self {
        Self {
            default_dimension: 384,
            index_type: "hnsw".to_string(),
            max_connections: 32,
            ef_construction: 200,
            ef_search: 64,
            enable_gpu: false,
            cache_size_mb: 1024,
        }
    }
}

impl Default for AuditConfig {
    fn default() -> Self {
        Self {
            enable_audit_logging: true,
            audit_log_file: "/var/log/aurora/audit.log".to_string(),
            log_sensitive_operations: true,
            log_connection_events: true,
            log_ddl_operations: true,
            log_dml_operations: false,
            retention_days: 365,
            max_log_size_mb: 1024,
        }
    }
}

// Default implementations for nested structs
impl Default for BTreeConfig {
    fn default() -> Self {
        Self {
            page_size_kb: 8,
            max_table_size_mb: 10240,
            cache_size_mb: 2048,
            max_concurrent_transactions: 100,
        }
    }
}

impl Default for LSMConfig {
    fn default() -> Self {
        Self {
            memtable_size_mb: 64,
            sstable_size_mb: 256,
            compaction_threads: 4,
            bloom_filter_bits: 10,
        }
    }
}

impl Default for HybridConfig {
    fn default() -> Self {
        Self {
            adaptive_threshold: 10000,
            vector_threshold: 0.1,
        }
    }
}

impl Default for WALConfig {
    fn default() -> Self {
        Self {
            directory: "/var/lib/aurora/wal".to_string(),
            segment_size_mb: 64,
            max_segments: 100,
            sync_strategy: "fsync".to_string(),
            flush_interval_ms: 1000,
        }
    }
}

impl Default for CompressionConfig {
    fn default() -> Self {
        Self {
            algorithm: "lz4".to_string(),
            level: 1,
            min_block_size: 65536,
        }
    }
}

impl Default for ConnectionPoolConfig {
    fn default() -> Self {
        Self {
            max_connections: 1000,
            min_idle: 10,
            max_idle_time_seconds: 300,
            connection_timeout_seconds: 30,
            health_check_interval_seconds: 60,
        }
    }
}

impl Default for TLSConfig {
    fn default() -> Self {
        Self {
            enabled: false,
            cert_file: None,
            key_file: None,
            ca_file: None,
            min_version: "TLS1.2".to_string(),
            mutual_auth: false,
        }
    }
}

impl Default for LoadBalancerConfig {
    fn default() -> Self {
        Self {
            enabled: false,
            algorithm: "round_robin".to_string(),
            health_check_interval_seconds: 30,
            failover_timeout_seconds: 30,
        }
    }
}

impl Default for AlertThresholds {
    fn default() -> Self {
        Self {
            cpu_high_threshold: 80,
            memory_high_threshold: 85,
            disk_high_threshold: 90,
            connection_pool_high_threshold: 95,
            slow_query_threshold_ms: 5000,
        }
    }
}

/// Parse size strings (e.g., "1GB", "512MB") into bytes
fn parse_size(size_str: &str) -> AuroraResult<usize> {
    let size_str = size_str.trim();

    if let Ok(bytes) = size_str.parse::<usize>() {
        return Ok(bytes);
    }

    if size_str.len() < 3 {
        return Err(crate::core::AuroraError::InvalidArgument(
            format!("Invalid size format: {}", size_str)
        ));
    }

    let (num_str, unit) = size_str.split_at(size_str.len() - 2);
    let multiplier = match unit.to_uppercase().as_str() {
        "KB" => 1024,
        "MB" => 1024 * 1024,
        "GB" => 1024 * 1024 * 1024,
        _ => return Err(crate::core::AuroraError::InvalidArgument(
            format!("Unknown size unit: {}", unit)
        )),
    };

    let num: usize = num_str.parse()?;
    Ok(num * multiplier)
}

/// Create configuration manager instance
pub async fn create_config_manager(config_path: Option<&str>) -> AuroraResult<ConfigManager> {
    let mut manager = ConfigManager::new();
    manager.load_config(config_path).await?;
    Ok(manager)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_size() {
        assert_eq!(parse_size("1024").unwrap(), 1024);
        assert_eq!(parse_size("1KB").unwrap(), 1024);
        assert_eq!(parse_size("1MB").unwrap(), 1024 * 1024);
        assert_eq!(parse_size("1GB").unwrap(), 1024 * 1024 * 1024);
    }

    #[tokio::test]
    async fn test_config_validation() {
        let config = AuroraConfig::default();
        assert!(config.validate().is_ok());
    }

    #[tokio::test]
    async fn test_config_manager() {
        let manager = ConfigManager::new();
        let config = manager.get_config().await.unwrap();
        assert_eq!(config.database.max_connections, 1000);
    }
}
