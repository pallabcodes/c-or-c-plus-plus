//! AuroraDB Production Error Handling System
//!
//! Comprehensive error handling with:
//! - Structured error codes and categories
//! - Error context and chaining
//! - Error metrics and monitoring
//! - User-friendly error messages
//! - Recovery suggestions

use std::collections::HashMap;
use std::fmt;
use std::sync::Arc;
use thiserror::Error;
use serde::{Serialize, Deserialize};
use tracing::{error, warn, info};
use crate::logging;

/// Global error metrics collector
static mut ERROR_METRICS: Option<Arc<ErrorMetrics>> = None;

/// Error severity levels
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Serialize, Deserialize)]
pub enum ErrorSeverity {
    Low = 1,
    Medium = 2,
    High = 3,
    Critical = 4,
}

/// Error categories for classification
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub enum ErrorCategory {
    // System-level errors
    System,
    Configuration,
    Resource,

    // Database operation errors
    Connection,
    Authentication,
    Authorization,
    Query,
    Transaction,
    Storage,

    // Network and communication
    Network,
    Protocol,

    // Security and compliance
    Security,
    Audit,
    Compliance,

    // Application logic
    Validation,
    BusinessLogic,
    DataIntegrity,

    // External dependencies
    ExternalService,
    ThirdParty,
}

/// Structured error codes
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
pub enum ErrorCode {
    // System errors (1000-1999)
    SystemOutOfMemory = 1001,
    SystemDiskFull = 1002,
    SystemNetworkUnreachable = 1003,
    SystemClockSkew = 1004,

    // Configuration errors (2000-2999)
    ConfigInvalidFormat = 2001,
    ConfigMissingRequired = 2002,
    ConfigInvalidValue = 2003,
    ConfigFileNotFound = 2004,

    // Connection errors (3000-3999)
    ConnectionTimeout = 3001,
    ConnectionRefused = 3002,
    ConnectionLost = 3003,
    ConnectionPoolExhausted = 3004,

    // Authentication errors (4000-4999)
    AuthInvalidCredentials = 4001,
    AuthTokenExpired = 4002,
    AuthInsufficientPermissions = 4003,
    AuthAccountLocked = 4004,

    // Query errors (5000-5999)
    QuerySyntaxError = 5001,
    QueryTimeout = 5002,
    QueryCancelled = 5003,
    QueryInvalidParameters = 5004,

    // Transaction errors (6000-6999)
    TransactionDeadlock = 6001,
    TransactionRollback = 6002,
    TransactionTimeout = 6003,
    TransactionConflict = 6004,

    // Storage errors (7000-7999)
    StorageDiskFull = 7001,
    StorageCorruption = 7002,
    StorageInconsistent = 7003,
    StorageUnavailable = 7004,

    // Security errors (8000-8999)
    SecurityEncryptionFailed = 8001,
    SecurityDecryptionFailed = 8002,
    SecurityCertificateInvalid = 8003,
    SecurityTamperingDetected = 8004,

    // Validation errors (9000-9999)
    ValidationRequiredField = 9001,
    ValidationInvalidFormat = 9002,
    ValidationConstraintViolation = 9003,
    ValidationTypeMismatch = 9004,
}

/// Core AuroraDB error with comprehensive context
#[derive(Debug, Clone, Error, Serialize, Deserialize)]
pub struct AuroraError {
    /// Unique error code
    pub code: ErrorCode,

    /// Error category
    pub category: ErrorCategory,

    /// Error severity
    pub severity: ErrorSeverity,

    /// Human-readable message
    pub message: String,

    /// Technical details for debugging
    pub details: Option<String>,

    /// Component that generated the error
    pub component: String,

    /// Operation being performed
    pub operation: Option<String>,

    /// Additional context data
    pub context: HashMap<String, String>,

    /// Source error (for chaining)
    pub source: Option<Box<AuroraError>>,

    /// Timestamp when error occurred
    pub timestamp: chrono::DateTime<chrono::Utc>,

    /// Request ID for correlation
    pub request_id: Option<String>,

    /// Recovery suggestions
    pub recovery_suggestions: Vec<String>,
}

impl AuroraError {
    /// Create a new AuroraDB error
    pub fn new(code: ErrorCode, message: impl Into<String>) -> Self {
        let (category, severity) = Self::get_category_and_severity(code);

        Self {
            code,
            category,
            severity,
            message: message.into(),
            details: None,
            component: "unknown".to_string(),
            operation: None,
            context: HashMap::new(),
            source: None,
            timestamp: chrono::Utc::now(),
            request_id: None,
            recovery_suggestions: Vec::new(),
        }
    }

    /// Create error with context
    pub fn with_context(mut self, key: impl Into<String>, value: impl Into<String>) -> Self {
        self.context.insert(key.into(), value.into());
        self
    }

    /// Add operation context
    pub fn with_operation(mut self, operation: impl Into<String>) -> Self {
        self.operation = Some(operation.into());
        self
    }

    /// Add component context
    pub fn with_component(mut self, component: impl Into<String>) -> Self {
        self.component = component.into();
        self
    }

    /// Add request ID for correlation
    pub fn with_request_id(mut self, request_id: impl Into<String>) -> Self {
        self.request_id = Some(request_id.into());
        self
    }

    /// Add recovery suggestions
    pub fn with_recovery_suggestion(mut self, suggestion: impl Into<String>) -> Self {
        self.recovery_suggestions.push(suggestion.into());
        self
    }

    /// Chain errors
    pub fn caused_by(mut self, source: AuroraError) -> Self {
        self.source = Some(Box::new(source));
        self
    }

    /// Log the error with appropriate level
    pub fn log(&self) {
        let log_context = crate::logging::LogContext {
            operation: self.operation.clone(),
            error_code: Some(format!("{:?}", self.code)),
            user_id: None,
            ip_address: None,
            ..Default::default()
        };

        match self.severity {
            ErrorSeverity::Critical => {
                error!(target: &self.component, "Critical error: {} ({:?})", self.message, self.code);
                crate::logging::log_error!(&self.component, "Critical error: {} ({:?})", self.message, self.code);
            }
            ErrorSeverity::High => {
                error!(target: &self.component, "High severity error: {} ({:?})", self.message, self.code);
                crate::logging::log_error!(&self.component, "High severity error: {} ({:?})", self.message, self.code);
            }
            ErrorSeverity::Medium => {
                warn!(target: &self.component, "Medium severity error: {} ({:?})", self.message, self.code);
                crate::logging::log_warn!(&self.component, "Medium severity error: {} ({:?})", self.message, self.code);
            }
            ErrorSeverity::Low => {
                info!(target: &self.component, "Low severity error: {} ({:?})", self.message, self.code);
                crate::logging::log_info!(&self.component, "Low severity error: {} ({:?})", self.message, self.code);
            }
        }

        // Record error metrics
        if let Some(metrics) = Self::get_metrics() {
            metrics.record_error(self.category, self.severity);
        }
    }

    /// Get user-friendly error message
    pub fn user_message(&self) -> String {
        match self.code {
            ErrorCode::ConnectionTimeout => "Connection timed out. Please check your network connection and try again.".to_string(),
            ErrorCode::AuthInvalidCredentials => "Invalid username or password. Please check your credentials and try again.".to_string(),
            ErrorCode::QuerySyntaxError => "Invalid query syntax. Please check your SQL syntax and try again.".to_string(),
            ErrorCode::TransactionDeadlock => "Transaction deadlock detected. The transaction has been rolled back. Please retry your operation.".to_string(),
            ErrorCode::StorageDiskFull => "Database storage is full. Please free up disk space or contact your administrator.".to_string(),
            _ => format!("An error occurred: {}. {}", self.message, self.recovery_suggestions.join(" "))
        }
    }

    /// Check if error is retryable
    pub fn is_retryable(&self) -> bool {
        matches!(self.code,
            ErrorCode::ConnectionTimeout |
            ErrorCode::ConnectionLost |
            ErrorCode::TransactionDeadlock |
            ErrorCode::SystemNetworkUnreachable
        )
    }

    /// Check if error is client error (4xx)
    pub fn is_client_error(&self) -> bool {
        matches!(self.category,
            ErrorCategory::Authentication |
            ErrorCategory::Authorization |
            ErrorCategory::Validation
        )
    }

    /// Check if error is server error (5xx)
    pub fn is_server_error(&self) -> bool {
        matches!(self.category,
            ErrorCategory::System |
            ErrorCategory::Storage |
            ErrorCategory::Transaction
        )
    }

    /// Get category and severity for error code
    fn get_category_and_severity(code: ErrorCode) -> (ErrorCategory, ErrorSeverity) {
        match code {
            // System errors
            ErrorCode::SystemOutOfMemory | ErrorCode::SystemDiskFull |
            ErrorCode::SystemNetworkUnreachable | ErrorCode::SystemClockSkew => {
                (ErrorCategory::System, ErrorSeverity::Critical)
            }

            // Configuration errors
            ErrorCode::ConfigInvalidFormat | ErrorCode::ConfigMissingRequired |
            ErrorCode::ConfigInvalidValue | ErrorCode::ConfigFileNotFound => {
                (ErrorCategory::Configuration, ErrorSeverity::High)
            }

            // Connection errors
            ErrorCode::ConnectionTimeout | ErrorCode::ConnectionRefused |
            ErrorCode::ConnectionLost | ErrorCode::ConnectionPoolExhausted => {
                (ErrorCategory::Connection, ErrorSeverity::Medium)
            }

            // Authentication errors
            ErrorCode::AuthInvalidCredentials | ErrorCode::AuthTokenExpired |
            ErrorCode::AuthInsufficientPermissions | ErrorCode::AuthAccountLocked => {
                (ErrorCategory::Authentication, ErrorSeverity::Medium)
            }

            // Query errors
            ErrorCode::QuerySyntaxError | ErrorCode::QueryTimeout |
            ErrorCode::QueryCancelled | ErrorCode::QueryInvalidParameters => {
                (ErrorCategory::Query, ErrorSeverity::Medium)
            }

            // Transaction errors
            ErrorCode::TransactionDeadlock | ErrorCode::TransactionRollback |
            ErrorCode::TransactionTimeout | ErrorCode::TransactionConflict => {
                (ErrorCategory::Transaction, ErrorSeverity::High)
            }

            // Storage errors
            ErrorCode::StorageDiskFull | ErrorCode::StorageCorruption |
            ErrorCode::StorageInconsistent | ErrorCode::StorageUnavailable => {
                (ErrorCategory::Storage, ErrorSeverity::Critical)
            }

            // Security errors
            ErrorCode::SecurityEncryptionFailed | ErrorCode::SecurityDecryptionFailed |
            ErrorCode::SecurityCertificateInvalid | ErrorCode::SecurityTamperingDetected => {
                (ErrorCategory::Security, ErrorSeverity::Critical)
            }

            // Validation errors
            ErrorCode::ValidationRequiredField | ErrorCode::ValidationInvalidFormat |
            ErrorCode::ValidationConstraintViolation | ErrorCode::ValidationTypeMismatch => {
                (ErrorCategory::Validation, ErrorSeverity::Low)
            }
        }
    }

    /// Get global error metrics
    fn get_metrics() -> Option<&'static Arc<ErrorMetrics>> {
        unsafe { ERROR_METRICS.as_ref() }
    }
}

impl fmt::Display for AuroraError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "[{:?}] {}: {}", self.code, self.category, self.message)?;

        if let Some(ref operation) = self.operation {
            write!(f, " (during {})", operation)?;
        }

        if !self.context.is_empty() {
            write!(f, " [context: {}]", self.context.len())?;
        }

        Ok(())
    }
}

impl From<std::io::Error> for AuroraError {
    fn from(err: std::io::Error) -> Self {
        let code = match err.kind() {
            std::io::ErrorKind::NotFound => ErrorCode::ConfigFileNotFound,
            std::io::ErrorKind::PermissionDenied => ErrorCode::AuthInsufficientPermissions,
            std::io::ErrorKind::ConnectionRefused => ErrorCode::ConnectionRefused,
            std::io::ErrorKind::ConnectionAborted => ErrorCode::ConnectionLost,
            std::io::ErrorKind::TimedOut => ErrorCode::ConnectionTimeout,
            _ => ErrorCode::SystemDiskFull, // Generic I/O error
        };

        AuroraError::new(code, err.to_string())
            .with_component("io")
            .with_context("error_kind", format!("{:?}", err.kind()))
    }
}

impl From<serde_json::Error> for AuroraError {
    fn from(err: serde_json::Error) -> Self {
        AuroraError::new(ErrorCode::ValidationInvalidFormat, err.to_string())
            .with_component("serialization")
            .with_context("error_type", "json")
    }
}

impl From<tokio::time::error::Elapsed> for AuroraError {
    fn from(_err: tokio::time::error::Elapsed) -> Self {
        AuroraError::new(ErrorCode::ConnectionTimeout, "Operation timed out")
            .with_component("timeout")
    }
}

/// Result type alias for AuroraDB operations
pub type AuroraResult<T> = Result<T, AuroraError>;

/// Error metrics for monitoring
pub struct ErrorMetrics {
    pub total_errors: std::sync::atomic::AtomicU64,
    pub system_errors: std::sync::atomic::AtomicU64,
    pub connection_errors: std::sync::atomic::AtomicU64,
    pub query_errors: std::sync::atomic::AtomicU64,
    pub auth_errors: std::sync::atomic::AtomicU64,
    pub storage_errors: std::sync::atomic::AtomicU64,
    pub security_errors: std::sync::atomic::AtomicU64,

    pub critical_errors: std::sync::atomic::AtomicU64,
    pub high_errors: std::sync::atomic::AtomicU64,
    pub medium_errors: std::sync::atomic::AtomicU64,
    pub low_errors: std::sync::atomic::AtomicU64,
}

impl ErrorMetrics {
    pub fn new() -> Self {
        Self {
            total_errors: std::sync::atomic::AtomicU64::new(0),
            system_errors: std::sync::atomic::AtomicU64::new(0),
            connection_errors: std::sync::atomic::AtomicU64::new(0),
            query_errors: std::sync::atomic::AtomicU64::new(0),
            auth_errors: std::sync::atomic::AtomicU64::new(0),
            storage_errors: std::sync::atomic::AtomicU64::new(0),
            security_errors: std::sync::atomic::AtomicU64::new(0),
            critical_errors: std::sync::atomic::AtomicU64::new(0),
            high_errors: std::sync::atomic::AtomicU64::new(0),
            medium_errors: std::sync::atomic::AtomicU64::new(0),
            low_errors: std::sync::atomic::AtomicU64::new(0),
        }
    }

    pub fn record_error(&self, category: ErrorCategory, severity: ErrorSeverity) {
        self.total_errors.fetch_add(1, std::sync::atomic::Ordering::Relaxed);

        match category {
            ErrorCategory::System => { self.system_errors.fetch_add(1, std::sync::atomic::Ordering::Relaxed); }
            ErrorCategory::Connection => { self.connection_errors.fetch_add(1, std::sync::atomic::Ordering::Relaxed); }
            ErrorCategory::Query => { self.query_errors.fetch_add(1, std::sync::atomic::Ordering::Relaxed); }
            ErrorCategory::Authentication => { self.auth_errors.fetch_add(1, std::sync::atomic::Ordering::Relaxed); }
            ErrorCategory::Storage => { self.storage_errors.fetch_add(1, std::sync::atomic::Ordering::Relaxed); }
            ErrorCategory::Security => { self.security_errors.fetch_add(1, std::sync::atomic::Ordering::Relaxed); }
            _ => {}
        }

        match severity {
            ErrorSeverity::Critical => { self.critical_errors.fetch_add(1, std::sync::atomic::Ordering::Relaxed); }
            ErrorSeverity::High => { self.high_errors.fetch_add(1, std::sync::atomic::Ordering::Relaxed); }
            ErrorSeverity::Medium => { self.medium_errors.fetch_add(1, std::sync::atomic::Ordering::Relaxed); }
            ErrorSeverity::Low => { self.low_errors.fetch_add(1, std::sync::atomic::Ordering::Relaxed); }
        }
    }

    pub fn get_metrics(&self) -> HashMap<String, u64> {
        let mut metrics = HashMap::new();
        metrics.insert("total_errors".to_string(), self.total_errors.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("system_errors".to_string(), self.system_errors.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("connection_errors".to_string(), self.connection_errors.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("query_errors".to_string(), self.query_errors.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("auth_errors".to_string(), self.auth_errors.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("storage_errors".to_string(), self.storage_errors.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("security_errors".to_string(), self.security_errors.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("critical_errors".to_string(), self.critical_errors.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("high_errors".to_string(), self.high_errors.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("medium_errors".to_string(), self.medium_errors.load(std::sync::atomic::Ordering::Relaxed));
        metrics.insert("low_errors".to_string(), self.low_errors.load(std::sync::atomic::Ordering::Relaxed));
        metrics
    }
}

/// Initialize global error metrics
pub fn init_error_metrics() -> Arc<ErrorMetrics> {
    let metrics = Arc::new(ErrorMetrics::new());
    unsafe {
        ERROR_METRICS = Some(metrics.clone());
    }
    metrics
}

/// Convenience functions for creating common errors
pub mod errors {
    use super::*;

    pub fn connection_timeout(operation: &str) -> AuroraError {
        AuroraError::new(ErrorCode::ConnectionTimeout, "Connection timed out")
            .with_operation(operation)
            .with_component("connection")
            .with_recovery_suggestion("Check network connectivity and retry")
            .with_recovery_suggestion("Increase connection timeout settings")
    }

    pub fn authentication_failed(username: &str) -> AuroraError {
        AuroraError::new(ErrorCode::AuthInvalidCredentials, "Authentication failed")
            .with_operation("authentication")
            .with_component("security")
            .with_context("username", username)
            .with_recovery_suggestion("Verify username and password")
            .with_recovery_suggestion("Check account status")
    }

    pub fn query_syntax_error(query: &str, position: usize) -> AuroraError {
        AuroraError::new(ErrorCode::QuerySyntaxError, "Invalid SQL syntax")
            .with_operation("query_parsing")
            .with_component("query")
            .with_context("query", query)
            .with_context("position", position.to_string())
            .with_recovery_suggestion("Check SQL syntax near position " + &position.to_string())
    }

    pub fn storage_full(path: &str) -> AuroraError {
        AuroraError::new(ErrorCode::StorageDiskFull, "Storage device is full")
            .with_operation("storage_operation")
            .with_component("storage")
            .with_context("path", path)
            .with_recovery_suggestion("Free up disk space")
            .with_recovery_suggestion("Add additional storage capacity")
    }

    pub fn transaction_deadlock(tx_id: &str) -> AuroraError {
        AuroraError::new(ErrorCode::TransactionDeadlock, "Transaction deadlock detected")
            .with_operation("transaction_execution")
            .with_component("transaction")
            .with_context("transaction_id", tx_id)
            .with_recovery_suggestion("Transaction has been rolled back")
            .with_recovery_suggestion("Retry the operation")
            .with_recovery_suggestion("Consider breaking the transaction into smaller operations")
    }

    pub fn config_missing_field(field: &str, section: &str) -> AuroraError {
        AuroraError::new(ErrorCode::ConfigMissingRequired, format!("Missing required configuration field: {}", field))
            .with_operation("configuration_loading")
            .with_component("config")
            .with_context("field", field)
            .with_context("section", section)
            .with_recovery_suggestion(format!("Add '{}' field to '{}' section in configuration", field, section))
    }

    pub fn system_resource_exhausted(resource: &str) -> AuroraError {
        AuroraError::new(ErrorCode::SystemOutOfMemory, format!("System {} exhausted", resource))
            .with_operation("resource_allocation")
            .with_component("system")
            .with_context("resource", resource)
            .with_recovery_suggestion("Increase system resources")
            .with_recovery_suggestion("Reduce concurrent load")
            .with_recovery_suggestion("Optimize application memory usage")
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_error_creation() {
        let error = AuroraError::new(ErrorCode::QuerySyntaxError, "Invalid syntax");
        assert_eq!(error.code, ErrorCode::QuerySyntaxError);
        assert_eq!(error.category, ErrorCategory::Query);
        assert_eq!(error.severity, ErrorSeverity::Medium);
    }

    #[test]
    fn test_error_with_context() {
        let error = AuroraError::new(ErrorCode::ConnectionTimeout, "Connection failed")
            .with_context("host", "localhost")
            .with_operation("connect")
            .with_component("network");

        assert_eq!(error.context.get("host"), Some(&"localhost".to_string()));
        assert_eq!(error.operation, Some("connect".to_string()));
        assert_eq!(error.component, "network");
    }

    #[test]
    fn test_error_chaining() {
        let root_error = AuroraError::new(ErrorCode::StorageCorruption, "Data corruption detected");
        let chained_error = AuroraError::new(ErrorCode::QueryTimeout, "Query failed due to storage issue")
            .caused_by(root_error);

        assert!(chained_error.source.is_some());
        assert_eq!(chained_error.source.as_ref().unwrap().code, ErrorCode::StorageCorruption);
    }

    #[test]
    fn test_error_user_message() {
        let error = AuroraError::new(ErrorCode::ConnectionTimeout, "Connection timed out");
        let user_msg = error.user_message();
        assert!(user_msg.contains("Connection timed out"));
        assert!(user_msg.contains("network connection"));
    }

    #[test]
    fn test_error_is_retryable() {
        let timeout_error = AuroraError::new(ErrorCode::ConnectionTimeout, "Timeout");
        let syntax_error = AuroraError::new(ErrorCode::QuerySyntaxError, "Syntax error");

        assert!(timeout_error.is_retryable());
        assert!(!syntax_error.is_retryable());
    }

    #[test]
    fn test_convenience_error_functions() {
        let error = errors::connection_timeout("database_connect");
        assert_eq!(error.code, ErrorCode::ConnectionTimeout);
        assert_eq!(error.operation, Some("database_connect".to_string()));
        assert!(!error.recovery_suggestions.is_empty());
    }

    #[test]
    fn test_error_metrics() {
        let metrics = ErrorMetrics::new();
        metrics.record_error(ErrorCategory::Query, ErrorSeverity::High);

        let metric_data = metrics.get_metrics();
        assert_eq!(metric_data["total_errors"], 1);
        assert_eq!(metric_data["query_errors"], 1);
        assert_eq!(metric_data["high_errors"], 1);
    }
}
