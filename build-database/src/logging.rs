//! AuroraDB Production Logging System
//!
//! Production-ready logging infrastructure supporting:
//! - Structured JSON logging with context
//! - Multiple log levels and filtering
//! - File rotation and compression
//! - Asynchronous logging for performance
//! - Log aggregation and correlation IDs
//! - Metrics integration for log monitoring

use std::collections::HashMap;
use std::fs::{self, OpenOptions};
use std::io::{self, Write};
use std::path::Path;
use std::sync::Arc;
use tokio::sync::{mpsc, RwLock};
use tracing::{Level, Event, Subscriber};
use tracing_subscriber::{Layer, layer::Context, registry::LookupSpan};
use serde::{Serialize, Deserialize};
use chrono::{DateTime, Utc};
use flate2::write::GzEncoder;
use flate2::Compression;

/// Global logging system instance
static mut LOGGING_SYSTEM: Option<Arc<LoggingSystem>> = None;

/// Log entry structure for structured logging
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct LogEntry {
    /// Timestamp in ISO 8601 format
    pub timestamp: DateTime<Utc>,

    /// Log level
    pub level: String,

    /// Logger name/target
    pub target: String,

    /// Log message
    pub message: String,

    /// Structured fields
    pub fields: HashMap<String, serde_json::Value>,

    /// Request ID for correlation
    pub request_id: Option<String>,

    /// User ID for security auditing
    pub user_id: Option<String>,

    /// Component name
    pub component: String,

    /// Hostname
    pub hostname: String,

    /// Process ID
    pub pid: u32,

    /// Thread ID
    pub thread_id: String,
}

/// Log writer for different output destinations
#[derive(Debug, Clone)]
pub enum LogWriter {
    /// Write to stdout
    Stdout,

    /// Write to stderr
    Stderr,

    /// Write to file with rotation
    File {
        path: String,
        max_size: u64,
        max_files: usize,
        compress: bool,
    },

    /// Write to multiple destinations
    Multi(Vec<LogWriter>),
}

/// Logging configuration
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct LoggingConfig {
    /// Minimum log level
    pub level: String,

    /// Log format (json, text, compact)
    pub format: String,

    /// Log writers
    pub writers: Vec<LogWriterConfig>,

    /// Enable request ID tracking
    pub enable_request_id: bool,

    /// Enable structured fields
    pub enable_structured_fields: bool,

    /// Buffer size for async logging
    pub buffer_size: usize,

    /// Flush interval in milliseconds
    pub flush_interval_ms: u64,

    /// Enable metrics for logging
    pub enable_metrics: bool,
}

/// Log writer configuration for serialization
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct LogWriterConfig {
    /// Writer type
    pub writer_type: String,

    /// File path (for file writer)
    pub file_path: Option<String>,

    /// Maximum file size in MB (for file writer)
    pub max_size_mb: Option<u64>,

    /// Maximum number of files (for file writer)
    pub max_files: Option<usize>,

    /// Enable compression (for file writer)
    pub compress: Option<bool>,
}

/// Main logging system
pub struct LoggingSystem {
    config: LoggingConfig,
    writers: Vec<Box<dyn LogOutput + Send + Sync>>,
    sender: mpsc::UnboundedSender<LogEntry>,
    metrics: Arc<LogMetrics>,
    hostname: String,
    pid: u32,
}

impl LoggingSystem {
    /// Initialize the global logging system
    pub async fn init(config: LoggingConfig) -> Result<(), LoggingError> {
        // Create writers from config
        let writers = Self::create_writers(&config.writers)?;

        // Create metrics
        let metrics = Arc::new(LogMetrics::new());

        // Create channel for async logging
        let (sender, receiver) = mpsc::unbounded_channel();

        // Get system info
        let hostname = hostname::get()
            .map_err(|_| LoggingError::SystemInfo("Failed to get hostname".to_string()))?
            .to_string_lossy()
            .to_string();
        let pid = std::process::id();

        let system = Arc::new(Self {
            config,
            writers,
            sender,
            metrics,
            hostname,
            pid,
        });

        // Set global instance
        unsafe {
            LOGGING_SYSTEM = Some(system.clone());
        }

        // Start async logging task
        tokio::spawn(Self::logging_worker(receiver, system.writers.clone(), system.metrics.clone()));

        // Start periodic flush task
        let flush_interval = system.config.flush_interval_ms;
        if flush_interval > 0 {
            tokio::spawn(Self::flush_worker(system.writers.clone(), flush_interval));
        }

        // Setup tracing subscriber
        Self::setup_tracing_subscriber(system.clone())?;

        Ok(())
    }

    /// Get the global logging system instance
    pub fn global() -> Option<&'static Arc<LoggingSystem>> {
        unsafe { LOGGING_SYSTEM.as_ref() }
    }

    /// Log a message with structured fields
    pub fn log(&self, level: Level, target: &str, message: &str, fields: HashMap<String, serde_json::Value>) {
        let entry = LogEntry {
            timestamp: Utc::now(),
            level: level.to_string(),
            target: target.to_string(),
            message: message.to_string(),
            fields,
            request_id: self.get_current_request_id(),
            user_id: self.get_current_user_id(),
            component: self.get_component_name(target),
            hostname: self.hostname.clone(),
            pid: self.pid,
            thread_id: format!("{:?}", std::thread::current().id()),
        };

        // Update metrics
        self.metrics.record_log(&entry.level);

        // Send to async worker
        let _ = self.sender.send(entry);
    }

    /// Log with context (convenience method)
    pub fn log_context(&self, level: Level, target: &str, message: &str, context: &LogContext) {
        let mut fields = HashMap::new();

        if let Some(ref operation) = context.operation {
            fields.insert("operation".to_string(), serde_json::json!(operation));
        }
        if let Some(ref table) = context.table {
            fields.insert("table".to_string(), serde_json::json!(table));
        }
        if let Some(ref query_id) = context.query_id {
            fields.insert("query_id".to_string(), serde_json::json!(query_id));
        }
        if let Some(ref duration_ms) = context.duration_ms {
            fields.insert("duration_ms".to_string(), serde_json::json!(duration_ms));
        }
        if let Some(ref error_code) = context.error_code {
            fields.insert("error_code".to_string(), serde_json::json!(error_code));
        }
        if let Some(ref user_id) = context.user_id {
            fields.insert("user_id".to_string(), serde_json::json!(user_id));
        }
        if let Some(ref ip_address) = context.ip_address {
            fields.insert("ip_address".to_string(), serde_json::json!(ip_address));
        }

        self.log(level, target, message, fields);
    }

    /// Create writers from configuration
    fn create_writers(configs: &[LogWriterConfig]) -> Result<Vec<Box<dyn LogOutput + Send + Sync>>, LoggingError> {
        let mut writers = Vec::new();

        for config in configs {
            let writer: Box<dyn LogOutput + Send + Sync> = match config.writer_type.as_str() {
                "stdout" => Box::new(StdoutWriter),
                "stderr" => Box::new(StderrWriter),
                "file" => {
                    let path = config.file_path.as_ref()
                        .ok_or_else(|| LoggingError::Config("File path required for file writer".to_string()))?;
                    let max_size = config.max_size_mb.unwrap_or(100) * 1024 * 1024; // MB to bytes
                    let max_files = config.max_files.unwrap_or(10);
                    let compress = config.compress.unwrap_or(true);
                    Box::new(FileWriter::new(path, max_size, max_files, compress)?)
                }
                _ => return Err(LoggingError::Config(format!("Unknown writer type: {}", config.writer_type))),
            };
            writers.push(writer);
        }

        if writers.is_empty() {
            // Default to stdout if no writers configured
            writers.push(Box::new(StdoutWriter));
        }

        Ok(writers)
    }

    /// Setup tracing subscriber to integrate with our logging system
    fn setup_tracing_subscriber(system: Arc<LoggingSystem>) -> Result<(), LoggingError> {
        let level = match system.config.level.to_lowercase().as_str() {
            "error" => Level::ERROR,
            "warn" => Level::WARN,
            "info" => Level::INFO,
            "debug" => Level::DEBUG,
            "trace" => Level::TRACE,
            _ => Level::INFO,
        };

        let layer = AuroraTracingLayer {
            system,
            level,
        };

        tracing::subscriber::set_global_default(
            tracing_subscriber::registry().with(layer)
        ).map_err(|e| LoggingError::Setup(format!("Failed to set tracing subscriber: {}", e)))?;

        Ok(())
    }

    /// Async logging worker
    async fn logging_worker(
        mut receiver: mpsc::UnboundedReceiver<LogEntry>,
        writers: Vec<Box<dyn LogOutput + Send + Sync>>,
        metrics: Arc<LogMetrics>,
    ) {
        while let Some(entry) = receiver.recv().await {
            // Write to all writers
            for writer in &writers {
                if let Err(e) = writer.write(&entry).await {
                    eprintln!("Failed to write log entry: {}", e);
                    metrics.record_error();
                }
            }
        }
    }

    /// Periodic flush worker
    async fn flush_worker(writers: Vec<Box<dyn LogOutput + Send + Sync>>, interval_ms: u64) {
        let mut interval = tokio::time::interval(std::time::Duration::from_millis(interval_ms));

        loop {
            interval.tick().await;

            for writer in &writers {
                if let Err(e) = writer.flush().await {
                    eprintln!("Failed to flush log writer: {}", e);
                }
            }
        }
    }

    /// Get current request ID from context
    fn get_current_request_id(&self) -> Option<String> {
        // In a real implementation, this would get the request ID from async context
        // For now, return None
        None
    }

    /// Get current user ID from context
    fn get_current_user_id(&self) -> Option<String> {
        // In a real implementation, this would get the user ID from security context
        // For now, return None
        None
    }

    /// Extract component name from target
    fn get_component_name(&self, target: &str) -> String {
        // Extract component from target (e.g., "aurora_db::storage" -> "storage")
        if let Some(colon_pos) = target.find("::") {
            let after_colon = &target[colon_pos + 2..];
            if let Some(next_colon) = after_colon.find("::") {
                after_colon[..next_colon].to_string()
            } else {
                after_colon.to_string()
            }
        } else {
            "unknown".to_string()
        }
    }
}

/// Log output trait
#[async_trait::async_trait]
pub trait LogOutput: Send + Sync {
    async fn write(&self, entry: &LogEntry) -> Result<(), LoggingError>;
    async fn flush(&self) -> Result<(), LoggingError>;
}

/// Standard output writer
pub struct StdoutWriter;

#[async_trait::async_trait]
impl LogOutput for StdoutWriter {
    async fn write(&self, entry: &LogEntry) -> Result<(), LoggingError> {
        let output = format_log_entry(entry)?;
        print!("{}", output);
        Ok(())
    }

    async fn flush(&self) -> Result<(), LoggingError> {
        io::stdout().flush().map_err(LoggingError::Io)?;
        Ok(())
    }
}

/// Standard error writer
pub struct StderrWriter;

#[async_trait::async_trait]
impl LogOutput for StderrWriter {
    async fn write(&self, entry: &LogEntry) -> Result<(), LoggingError> {
        let output = format_log_entry(entry)?;
        eprint!("{}", output);
        Ok(())
    }

    async fn flush(&self) -> Result<(), LoggingError> {
        io::stderr().flush().map_err(LoggingError::Io)?;
        Ok(())
    }
}

/// File writer with rotation and compression
pub struct FileWriter {
    base_path: String,
    current_file: RwLock<String>,
    max_size: u64,
    max_files: usize,
    compress: bool,
    current_size: RwLock<u64>,
}

impl FileWriter {
    fn new(base_path: &str, max_size: u64, max_files: usize, compress: bool) -> Result<Self, LoggingError> {
        let current_file = format!("{}.0", base_path);

        // Create directory if it doesn't exist
        if let Some(parent) = Path::new(&current_file).parent() {
            fs::create_dir_all(parent).map_err(LoggingError::Io)?;
        }

        Ok(Self {
            base_path: base_path.to_string(),
            current_file: RwLock::new(current_file),
            max_size,
            max_files,
            compress,
            current_size: RwLock::new(0),
        })
    }

    async fn rotate_if_needed(&self) -> Result<(), LoggingError> {
        let current_size = *self.current_size.read().await;

        if current_size >= self.max_size {
            self.rotate_files().await?;
        }

        Ok(())
    }

    async fn rotate_files(&self) -> Result<(), LoggingError> {
        // Move existing files
        for i in (0..self.max_files).rev() {
            let src = if i == 0 {
                format!("{}.0", self.base_path)
            } else {
                format!("{}.{}", self.base_path, i)
            };

            let dst = format!("{}.{}", self.base_path, i + 1);

            if Path::new(&src).exists() {
                if self.compress && i > 0 {
                    // Compress older files
                    self.compress_file(&src, &dst).await?;
                    fs::remove_file(&src).map_err(LoggingError::Io)?;
                } else {
                    fs::rename(&src, &dst).map_err(LoggingError::Io)?;
                }
            }
        }

        // Reset current file and size
        let new_file = format!("{}.0", self.base_path);
        *self.current_file.write().await = new_file;
        *self.current_size.write().await = 0;

        Ok(())
    }

    async fn compress_file(&self, src: &str, dst: &str) -> Result<(), LoggingError> {
        let input = fs::read(src).map_err(LoggingError::Io)?;
        let output_file = OpenOptions::new()
            .write(true)
            .create(true)
            .truncate(true)
            .open(dst)
            .map_err(LoggingError::Io)?;

        let mut encoder = GzEncoder::new(output_file, Compression::default());
        encoder.write_all(&input).map_err(LoggingError::Io)?;
        encoder.finish().map_err(LoggingError::Io)?;

        Ok(())
    }
}

#[async_trait::async_trait]
impl LogOutput for FileWriter {
    async fn write(&self, entry: &LogEntry) -> Result<(), LoggingError> {
        self.rotate_if_needed().await?;

        let output = format_log_entry(entry)?;
        let bytes = output.as_bytes();

        let file_path = self.current_file.read().await.clone();
        let mut file = OpenOptions::new()
            .create(true)
            .append(true)
            .open(&file_path)
            .map_err(LoggingError::Io)?;

        file.write_all(bytes).map_err(LoggingError::Io)?;
        file.flush().map_err(LoggingError::Io)?;

        // Update current size
        let mut current_size = self.current_size.write().await;
        *current_size += bytes.len() as u64;

        Ok(())
    }

    async fn flush(&self) -> Result<(), LoggingError> {
        let file_path = self.current_file.read().await.clone();
        if Path::new(&file_path).exists() {
            let file = OpenOptions::new()
                .write(true)
                .open(&file_path)
                .map_err(LoggingError::Io)?;
            file.sync_all().map_err(LoggingError::Io)?;
        }
        Ok(())
    }
}

/// Tracing layer to integrate with our logging system
struct AuroraTracingLayer {
    system: Arc<LoggingSystem>,
    level: Level,
}

impl<S> Layer<S> for AuroraTracingLayer
where
    S: Subscriber + for<'lookup> LookupSpan<'lookup>,
{
    fn on_event(&self, event: &Event<'_>, _ctx: Context<'_, S>) {
        if event.metadata().level() > &self.level {
            return;
        }

        let mut fields = HashMap::new();

        // Extract fields from the event
        let mut visitor = FieldVisitor(&mut fields);
        event.record(&mut visitor);

        let message = fields.remove("message")
            .and_then(|v| v.as_str().map(|s| s.to_string()))
            .unwrap_or_else(|| "No message".to_string());

        self.system.log(
            *event.metadata().level(),
            event.metadata().target(),
            &message,
            fields,
        );
    }
}

/// Field visitor for extracting tracing fields
struct FieldVisitor<'a>(&'a mut HashMap<String, serde_json::Value>);

impl<'a> tracing::field::Visit for FieldVisitor<'a> {
    fn record_debug(&mut self, field: &tracing::field::Field, value: &dyn std::fmt::Debug) {
        self.0.insert(field.name().to_string(), serde_json::json!(format!("{:?}", value)));
    }

    fn record_str(&mut self, field: &tracing::field::Field, value: &str) {
        self.0.insert(field.name().to_string(), serde_json::json!(value));
    }

    fn record_i64(&mut self, field: &tracing::field::Field, value: i64) {
        self.0.insert(field.name().to_string(), serde_json::json!(value));
    }

    fn record_u64(&mut self, field: &tracing::field::Field, value: u64) {
        self.0.insert(field.name().to_string(), serde_json::json!(value));
    }

    fn record_bool(&mut self, field: &tracing::field::Field, value: bool) {
        self.0.insert(field.name().to_string(), serde_json::json!(value));
    }

    fn record_error(&mut self, field: &tracing::field::Field, value: &(dyn std::error::Error + 'static)) {
        self.0.insert(field.name().to_string(), serde_json::json!(value.to_string()));
    }
}

/// Context for logging operations
#[derive(Debug, Clone)]
pub struct LogContext {
    pub operation: Option<String>,
    pub table: Option<String>,
    pub query_id: Option<String>,
    pub duration_ms: Option<u64>,
    pub error_code: Option<String>,
    pub user_id: Option<String>,
    pub ip_address: Option<String>,
}

impl Default for LogContext {
    fn default() -> Self {
        Self {
            operation: None,
            table: None,
            query_id: None,
            duration_ms: None,
            error_code: None,
            user_id: None,
            ip_address: None,
        }
    }
}

/// Logging metrics
pub struct LogMetrics {
    pub total_logs: std::sync::atomic::AtomicU64,
    pub error_logs: std::sync::atomic::AtomicU64,
    pub warn_logs: std::sync::atomic::AtomicU64,
    pub info_logs: std::sync::atomic::AtomicU64,
    pub debug_logs: std::sync::atomic::AtomicU64,
    pub trace_logs: std::sync::atomic::AtomicU64,
    pub write_errors: std::sync::atomic::AtomicU64,
    pub flush_errors: std::sync::atomic::AtomicU64,
}

impl LogMetrics {
    fn new() -> Self {
        Self {
            total_logs: std::sync::atomic::AtomicU64::new(0),
            error_logs: std::sync::atomic::AtomicU64::new(0),
            warn_logs: std::sync::atomic::AtomicU64::new(0),
            info_logs: std::sync::atomic::AtomicU64::new(0),
            debug_logs: std::sync::atomic::AtomicU64::new(0),
            trace_logs: std::sync::atomic::AtomicU64::new(0),
            write_errors: std::sync::atomic::AtomicU64::new(0),
            flush_errors: std::sync::atomic::AtomicU64::new(0),
        }
    }

    fn record_log(&self, level: &str) {
        self.total_logs.fetch_add(1, std::sync::atomic::Ordering::Relaxed);

        match level.to_lowercase().as_str() {
            "error" => { self.error_logs.fetch_add(1, std::sync::atomic::Ordering::Relaxed); }
            "warn" => { self.warn_logs.fetch_add(1, std::sync::atomic::Ordering::Relaxed); }
            "info" => { self.info_logs.fetch_add(1, std::sync::atomic::Ordering::Relaxed); }
            "debug" => { self.debug_logs.fetch_add(1, std::sync::atomic::Ordering::Relaxed); }
            "trace" => { self.trace_logs.fetch_add(1, std::sync::atomic::Ordering::Relaxed); }
            _ => {}
        }
    }

    fn record_error(&self) {
        self.write_errors.fetch_add(1, std::sync::atomic::Ordering::Relaxed);
    }

    fn record_flush_error(&self) {
        self.flush_errors.fetch_add(1, std::sync::atomic::Ordering::Relaxed);
    }

    pub fn get_metrics(&self) -> HashMap<String, u64> {
        let mut metrics = HashMap::new();
        metrics.insert("total_logs".to_string(), self.total_logs.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("error_logs".to_string(), self.error_logs.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("warn_logs".to_string(), self.warn_logs.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("info_logs".to_string(), self.info_logs.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("debug_logs".to_string(), self.debug_logs.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("trace_logs".to_string(), self.trace_logs.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("write_errors".to_string(), self.write_errors.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("flush_errors".to_string(), self.flush_errors.load(std::sync::atomic::Ordering::Relaxed));
        metrics
    }
}

/// Format log entry based on configuration
fn format_log_entry(entry: &LogEntry) -> Result<String, LoggingError> {
    if let Some(system) = LoggingSystem::global() {
        match system.config.format.as_str() {
            "json" => {
                serde_json::to_string(entry)
                    .map(|s| s + "\n")
                    .map_err(LoggingError::Serialization)
            }
            "compact" => {
                Ok(format!(
                    "{} [{}] {}: {}\n",
                    entry.timestamp.format("%Y-%m-%d %H:%M:%S%.3f"),
                    entry.level.chars().next().unwrap_or('?'),
                    entry.target,
                    entry.message
                ))
            }
            _ => {
                // Text format
                let mut output = format!(
                    "{} [{}] {} - {}\n",
                    entry.timestamp.format("%Y-%m-%d %H:%M:%S%.3f"),
                    entry.level.to_uppercase(),
                    entry.target,
                    entry.message
                );

                // Add structured fields
                for (key, value) in &entry.fields {
                    output.push_str(&format!("  {}: {}\n", key, value));
                }

                Ok(output)
            }
        }
    } else {
        // Fallback format
        Ok(format!(
            "{} [{}] {} - {}\n",
            entry.timestamp.format("%Y-%m-%d %H:%M:%S%.3f"),
            entry.level.to_uppercase(),
            entry.target,
            entry.message
        ))
    }
}

/// Logging errors
#[derive(Debug, thiserror::Error)]
pub enum LoggingError {
    #[error("I/O error: {0}")]
    Io(#[from] std::io::Error),

    #[error("Serialization error: {0}")]
    Serialization(#[from] serde_json::Error),

    #[error("Configuration error: {0}")]
    Config(String),

    #[error("Setup error: {0}")]
    Setup(String),

    #[error("System info error: {0}")]
    SystemInfo(String),
}

/// Convenience macros for logging
#[macro_export]
macro_rules! log_error {
    ($target:expr, $($arg:tt)*) => {
        if let Some(system) = $crate::logging::LoggingSystem::global() {
            system.log(tracing::Level::ERROR, $target, &format!($($arg)*), std::collections::HashMap::new());
        }
    };
}

#[macro_export]
macro_rules! log_warn {
    ($target:expr, $($arg:tt)*) => {
        if let Some(system) = $crate::logging::LoggingSystem::global() {
            system.log(tracing::Level::WARN, $target, &format!($($arg)*), std::collections::HashMap::new());
        }
    };
}

#[macro_export]
macro_rules! log_info {
    ($target:expr, $($arg:tt)*) => {
        if let Some(system) = $crate::logging::LoggingSystem::global() {
            system.log(tracing::Level::INFO, $target, &format!($($arg)*), std::collections::HashMap::new());
        }
    };
}

#[macro_export]
macro_rules! log_debug {
    ($target:expr, $($arg:tt)*) => {
        if let Some(system) = $crate::logging::LoggingSystem::global() {
            system.log(tracing::Level::DEBUG, $target, &format!($($arg)*), std::collections::HashMap::new());
        }
    };
}

#[macro_export]
macro_rules! log_trace {
    ($target:expr, $($arg:tt)*) => {
        if let Some(system) = $crate::logging::LoggingSystem::global() {
            system.log(tracing::Level::TRACE, $target, &format!($($arg)*), std::collections::HashMap::new());
        }
    };
}

/// Initialize logging with default configuration
pub async fn init_default_logging() -> Result<(), LoggingError> {
    let config = LoggingConfig {
        level: "info".to_string(),
        format: "json".to_string(),
        writers: vec![
            LogWriterConfig {
                writer_type: "stdout".to_string(),
                file_path: None,
                max_size_mb: None,
                max_files: None,
                compress: None,
            }
        ],
        enable_request_id: true,
        enable_structured_fields: true,
        buffer_size: 1000,
        flush_interval_ms: 1000,
        enable_metrics: true,
    };

    LoggingSystem::init(config).await
}

/// Initialize logging from configuration
pub async fn init_logging_from_config(config: &crate::config::LoggingConfig) -> Result<(), LoggingError> {
    let logging_config = LoggingConfig {
        level: config.level.clone(),
        format: config.format.clone(),
        writers: vec![
            LogWriterConfig {
                writer_type: if config.file.is_some() { "file".to_string() } else { "stdout".to_string() },
                file_path: config.file.clone(),
                max_size_mb: Some(config.max_size_mb as u64),
                max_files: Some(config.max_files as usize),
                compress: Some(config.compress_rotated),
            }
        ],
        enable_request_id: true,
        enable_structured_fields: true,
        buffer_size: 10000,
        flush_interval_ms: 5000,
        enable_metrics: true,
    };

    LoggingSystem::init(logging_config).await
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_default_logging_init() {
        init_default_logging().await.expect("Failed to initialize default logging");
        assert!(LoggingSystem::global().is_some());
    }

    #[tokio::test]
    async fn test_log_entry_creation() {
        let entry = LogEntry {
            timestamp: Utc::now(),
            level: "info".to_string(),
            target: "test".to_string(),
            message: "Test message".to_string(),
            fields: HashMap::new(),
            request_id: None,
            user_id: None,
            component: "test".to_string(),
            hostname: "localhost".to_string(),
            pid: 12345,
            thread_id: "thread-1".to_string(),
        };

        assert_eq!(entry.level, "info");
        assert_eq!(entry.message, "Test message");
    }

    #[test]
    fn test_format_log_entry() {
        let entry = LogEntry {
            timestamp: Utc::now(),
            level: "info".to_string(),
            target: "test".to_string(),
            message: "Test message".to_string(),
            fields: HashMap::new(),
            request_id: None,
            user_id: None,
            component: "test".to_string(),
            hostname: "localhost".to_string(),
            pid: 12345,
            thread_id: "thread-1".to_string(),
        };

        let formatted = format_log_entry(&entry).expect("Failed to format log entry");
        assert!(formatted.contains("Test message"));
    }
}
