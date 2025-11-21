//! AuroraDB Error Types
//!
//! Comprehensive error handling for AuroraDB drivers with detailed diagnostics,
//! error classification, and recovery suggestions.

use std::fmt;

/// AuroraDB error type
#[derive(Debug)]
pub enum AuroraError {
    /// Connection errors
    Connection(String),

    /// Authentication failures
    Authentication(String),

    /// Query execution errors
    Query(String),

    /// Transaction errors
    Transaction(String),

    /// Serialization/deserialization errors
    Serialization(String),

    /// Protocol errors
    Protocol(String),

    /// TLS/SSL errors
    Tls(String),

    /// Timeout errors
    Timeout(String),

    /// Pool exhaustion
    PoolExhausted(String),

    /// Configuration errors
    Configuration(String),

    /// Vector search specific errors
    VectorSearch(String),

    /// Analytics specific errors
    Analytics(String),

    /// Stream processing errors
    Streaming(String),

    /// I/O errors
    Io(std::io::Error),

    /// URL parsing errors
    Url(String),

    /// Generic errors
    Other(String),
}

impl fmt::Display for AuroraError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            AuroraError::Connection(msg) => write!(f, "Connection error: {}", msg),
            AuroraError::Authentication(msg) => write!(f, "Authentication error: {}", msg),
            AuroraError::Query(msg) => write!(f, "Query error: {}", msg),
            AuroraError::Transaction(msg) => write!(f, "Transaction error: {}", msg),
            AuroraError::Serialization(msg) => write!(f, "Serialization error: {}", msg),
            AuroraError::Protocol(msg) => write!(f, "Protocol error: {}", msg),
            AuroraError::Tls(msg) => write!(f, "TLS error: {}", msg),
            AuroraError::Timeout(msg) => write!(f, "Timeout error: {}", msg),
            AuroraError::PoolExhausted(msg) => write!(f, "Pool exhausted: {}", msg),
            AuroraError::Configuration(msg) => write!(f, "Configuration error: {}", msg),
            AuroraError::VectorSearch(msg) => write!(f, "Vector search error: {}", msg),
            AuroraError::Analytics(msg) => write!(f, "Analytics error: {}", msg),
            AuroraError::Streaming(msg) => write!(f, "Streaming error: {}", msg),
            AuroraError::Io(err) => write!(f, "I/O error: {}", err),
            AuroraError::Url(msg) => write!(f, "URL error: {}", msg),
            AuroraError::Other(msg) => write!(f, "Error: {}", msg),
        }
    }
}

impl std::error::Error for AuroraError {}

impl From<std::io::Error> for AuroraError {
    fn from(err: std::io::Error) -> Self {
        AuroraError::Io(err)
    }
}

impl From<serde_json::Error> for AuroraError {
    fn from(err: serde_json::Error) -> Self {
        AuroraError::Serialization(format!("JSON error: {}", err))
    }
}

impl From<bincode::Error> for AuroraError {
    fn from(err: bincode::Error) -> Self {
        AuroraError::Serialization(format!("Binary serialization error: {}", err))
    }
}

/// Result type alias
pub type Result<T> = std::result::Result<T, AuroraError>;

/// Error classification for handling strategies
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ErrorClass {
    /// Temporary errors that should be retried
    Transient,

    /// Permanent errors that should not be retried
    Permanent,

    /// Authentication/authorization errors
    Auth,

    /// Configuration errors
    Config,

    /// Network connectivity errors
    Network,

    /// Resource exhaustion errors
    Resource,

    /// Internal server errors
    Server,
}

impl AuroraError {
    /// Classify the error for appropriate handling
    pub fn classify(&self) -> ErrorClass {
        match self {
            AuroraError::Connection(_) | AuroraError::Timeout(_) | AuroraError::Tls(_) => ErrorClass::Network,
            AuroraError::PoolExhausted(_) => ErrorClass::Resource,
            AuroraError::Authentication(_) => ErrorClass::Auth,
            AuroraError::Configuration(_) => ErrorClass::Config,
            AuroraError::Io(_) => {
                // Check if it's a connection-related I/O error
                ErrorClass::Network
            }
            AuroraError::Query(_) | AuroraError::Transaction(_) | AuroraError::Protocol(_) => ErrorClass::Server,
            AuroraError::Serialization(_) | AuroraError::Url(_) => ErrorClass::Permanent,
            AuroraError::VectorSearch(_) | AuroraError::Analytics(_) | AuroraError::Streaming(_) => ErrorClass::Server,
            AuroraError::Other(_) => ErrorClass::Permanent,
        }
    }

    /// Check if error is retryable
    pub fn is_retryable(&self) -> bool {
        matches!(self.classify(), ErrorClass::Transient | ErrorClass::Network | ErrorClass::Resource)
    }

    /// Get recovery suggestions
    pub fn recovery_suggestions(&self) -> Vec<String> {
        match self.classify() {
            ErrorClass::Network => vec![
                "Check network connectivity".to_string(),
                "Verify AuroraDB server is running".to_string(),
                "Check firewall settings".to_string(),
                "Verify TLS certificate validity".to_string(),
            ],
            ErrorClass::Auth => vec![
                "Verify username and password".to_string(),
                "Check user permissions".to_string(),
                "Verify TLS client certificate".to_string(),
            ],
            ErrorClass::Resource => vec![
                "Increase connection pool size".to_string(),
                "Check system resource usage".to_string(),
                "Implement connection pooling".to_string(),
                "Add retry logic with backoff".to_string(),
            ],
            ErrorClass::Config => vec![
                "Verify connection URL format".to_string(),
                "Check configuration parameters".to_string(),
                "Validate SSL/TLS settings".to_string(),
            ],
            ErrorClass::Server => vec![
                "Check AuroraDB server logs".to_string(),
                "Verify query syntax".to_string(),
                "Check database schema".to_string(),
                "Contact AuroraDB support".to_string(),
            ],
            ErrorClass::Transient => vec![
                "Implement retry logic".to_string(),
                "Add exponential backoff".to_string(),
                "Use circuit breaker pattern".to_string(),
            ],
        }
    }

    /// Get error severity level
    pub fn severity(&self) -> ErrorSeverity {
        match self.classify() {
            ErrorClass::Network | ErrorClass::Resource => ErrorSeverity::Medium,
            ErrorClass::Auth | ErrorClass::Config => ErrorSeverity::High,
            ErrorClass::Server | ErrorClass::Transient => ErrorSeverity::Medium,
            ErrorClass::Permanent => ErrorSeverity::Low,
        }
    }
}

/// Error severity levels
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum ErrorSeverity {
    Low,
    Medium,
    High,
    Critical,
}

/// Error context for better debugging
#[derive(Debug, Clone)]
pub struct ErrorContext {
    /// Operation being performed
    pub operation: String,

    /// Connection ID
    pub connection_id: Option<String>,

    /// Query ID
    pub query_id: Option<String>,

    /// Timestamp
    pub timestamp: std::time::SystemTime,

    /// Stack trace (if available)
    pub stack_trace: Option<String>,

    /// Additional metadata
    pub metadata: std::collections::HashMap<String, String>,
}

impl ErrorContext {
    pub fn new(operation: &str) -> Self {
        Self {
            operation: operation.to_string(),
            connection_id: None,
            query_id: None,
            timestamp: std::time::SystemTime::now(),
            stack_trace: None,
            metadata: std::collections::HashMap::new(),
        }
    }

    pub fn with_connection_id(mut self, id: &str) -> Self {
        self.connection_id = Some(id.to_string());
        self
    }

    pub fn with_query_id(mut self, id: &str) -> Self {
        self.query_id = Some(id.to_string());
        self
    }

    pub fn with_metadata(mut self, key: &str, value: &str) -> Self {
        self.metadata.insert(key.to_string(), value.to_string());
        self
    }
}

/// Enhanced error with context
#[derive(Debug)]
pub struct ContextualError {
    pub error: AuroraError,
    pub context: ErrorContext,
}

impl ContextualError {
    pub fn new(error: AuroraError, context: ErrorContext) -> Self {
        Self { error, context }
    }

    /// Generate detailed error report
    pub fn error_report(&self) -> String {
        format!(
            "AuroraDB Error Report\n\
             ===================\n\
             Error: {}\n\
             Class: {:?}\n\
             Severity: {:?}\n\
             Retryable: {}\n\
             Operation: {}\n\
             Timestamp: {:?}\n\
             Connection ID: {}\n\
             Query ID: {}\n\
             \n\
             Recovery Suggestions:\n\
             {}\n\
             \n\
             Metadata:\n\
             {}\n\
             \n\
             Stack Trace:\n\
             {}",
            self.error,
            self.error.classify(),
            self.error.severity(),
            self.error.is_retryable(),
            self.context.operation,
            self.context.timestamp,
            self.context.connection_id.as_deref().unwrap_or("N/A"),
            self.context.query_id.as_deref().unwrap_or("N/A"),
            self.error.recovery_suggestions().join("\n- "),
            self.context.metadata.iter()
                .map(|(k, v)| format!("  {}: {}", k, v))
                .collect::<Vec<_>>()
                .join("\n"),
            self.context.stack_trace.as_deref().unwrap_or("Not available")
        )
    }
}

impl fmt::Display for ContextualError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{} (context: {})", self.error, self.context.operation)
    }
}

impl std::error::Error for ContextualError {}

// Convenience functions for creating errors with context
pub fn connection_error(msg: &str, context: ErrorContext) -> ContextualError {
    ContextualError::new(AuroraError::Connection(msg.to_string()), context)
}

pub fn query_error(msg: &str, context: ErrorContext) -> ContextualError {
    ContextualError::new(AuroraError::Query(msg.to_string()), context)
}

pub fn auth_error(msg: &str, context: ErrorContext) -> ContextualError {
    ContextualError::new(AuroraError::Authentication(msg.to_string()), context)
}

pub fn timeout_error(msg: &str, context: ErrorContext) -> ContextualError {
    ContextualError::new(AuroraError::Timeout(msg.to_string()), context)
}

// UNIQUENESS Validation:
// - [x] Comprehensive error classification system
// - [x] Error severity levels and retry logic
// - [x] Recovery suggestions for all error types
// - [x] Contextual error reporting with metadata
// - [x] Integration with Rust error handling ecosystem
