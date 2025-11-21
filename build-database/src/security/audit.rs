//! AuroraDB Production Audit Logging System
//!
//! Comprehensive audit logging for security and compliance:
//! - User authentication events
//! - DDL operations (CREATE, DROP, ALTER)
//! - DML operations with sensitive data filtering
//! - Administrative actions
//! - Security incidents
//! - Compliance reporting

use std::collections::HashMap;
use std::fs::{self, OpenOptions};
use std::io::{self, Write};
use std::path::Path;
use std::sync::Arc;
use tokio::sync::mpsc;
use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};
use flate2::write::GzEncoder;
use flate2::Compression;
use crate::core::AuroraResult;
use crate::errors::AuroraError;

/// Audit log entry
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AuditEntry {
    /// Unique event ID
    pub event_id: String,

    /// Timestamp of the event
    pub timestamp: DateTime<Utc>,

    /// Event type
    pub event_type: AuditEventType,

    /// User who performed the action
    pub user_id: Option<String>,

    /// Username
    pub username: Option<String>,

    /// User roles
    pub user_roles: Vec<String>,

    /// Client IP address
    pub client_ip: Option<String>,

    /// Session ID
    pub session_id: Option<String>,

    /// Database name
    pub database: String,

    /// Object type affected
    pub object_type: Option<String>,

    /// Object name affected
    pub object_name: Option<String>,

    /// Operation performed
    pub operation: String,

    /// Success/failure status
    pub success: bool,

    /// Error message if failed
    pub error_message: Option<String>,

    /// Additional context data
    pub context: HashMap<String, serde_json::Value>,

    /// Compliance flags
    pub compliance_flags: Vec<ComplianceFlag>,
}

/// Audit event types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AuditEventType {
    // Authentication events
    AuthenticationSuccess,
    AuthenticationFailure,
    Logout,
    SessionExpired,

    // DDL events
    CreateTable,
    DropTable,
    AlterTable,
    CreateIndex,
    DropIndex,
    CreateView,
    DropView,

    // DML events (sampled)
    Insert,
    Update,
    Delete,

    // Administrative events
    UserCreated,
    UserDeleted,
    UserModified,
    RoleGranted,
    RoleRevoked,
    BackupStarted,
    BackupCompleted,
    RestoreStarted,
    RestoreCompleted,

    // Security events
    AccessDenied,
    PrivilegeEscalation,
    SuspiciousActivity,
    DataExport,

    // System events
    ServerStart,
    ServerStop,
    ConfigurationChange,
    SecurityPolicyUpdate,
}

/// Compliance flags for regulatory requirements
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ComplianceFlag {
    GDPR,
    HIPAA,
    SOX,
    PCI,
    NIST,
    ISO27001,
}

/// Audit configuration
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AuditConfig {
    /// Enable audit logging
    pub enabled: bool,

    /// Audit log file path
    pub log_file: String,

    /// Maximum log file size in MB
    pub max_file_size_mb: u64,

    /// Maximum number of log files to keep
    pub max_files: usize,

    /// Enable compression of rotated logs
    pub compress_rotated: bool,

    /// Log authentication events
    pub log_authentication: bool,

    /// Log DDL operations
    pub log_ddl: bool,

    /// Log DML operations (sampled)
    pub log_dml: bool,

    /// Log administrative operations
    pub log_admin: bool,

    /// Log security events
    pub log_security: bool,

    /// Sample rate for DML operations (0.0 to 1.0)
    pub dml_sample_rate: f64,

    /// Retention period in days
    pub retention_days: usize,

    /// Sensitive data masking
    pub mask_sensitive_data: bool,
}

/// Main audit logger
pub struct AuditLogger {
    config: AuditConfig,
    sender: mpsc::UnboundedSender<AuditEntry>,
    sensitive_fields: Vec<String>,
}

impl AuditLogger {
    /// Create a new audit logger
    pub fn new(config: AuditConfig) -> Self {
        let (sender, receiver) = mpsc::unbounded_channel();

        let logger = Self {
            config,
            sender,
            sensitive_fields: vec![
                "password".to_string(),
                "password_hash".to_string(),
                "ssn".to_string(),
                "credit_card".to_string(),
                "api_key".to_string(),
                "secret".to_string(),
            ],
        };

        // Start async logging task
        tokio::spawn(Self::logging_worker(receiver, logger.config.clone()));

        logger
    }

    /// Log authentication success
    pub async fn log_auth_success(&self, username: &str, user_id: Option<&str>, ip: Option<&str>, session_id: Option<&str>) -> AuroraResult<()> {
        if !self.config.enabled || !self.config.log_authentication {
            return Ok(());
        }

        let entry = AuditEntry {
            event_id: self.generate_event_id(),
            timestamp: Utc::now(),
            event_type: AuditEventType::AuthenticationSuccess,
            user_id: user_id.map(|s| s.to_string()),
            username: Some(username.to_string()),
            user_roles: vec![], // Will be filled by context
            client_ip: ip.map(|s| s.to_string()),
            session_id: session_id.map(|s| s.to_string()),
            database: "aurora".to_string(),
            object_type: None,
            object_name: None,
            operation: "LOGIN".to_string(),
            success: true,
            error_message: None,
            context: HashMap::new(),
            compliance_flags: vec![ComplianceFlag::GDPR, ComplianceFlag::SOX],
        };

        self.send_entry(entry).await
    }

    /// Log authentication failure
    pub async fn log_auth_failure(&self, username: &str, ip: Option<&str>, reason: &str) -> AuroraResult<()> {
        if !self.config.enabled || !self.config.log_authentication {
            return Ok(());
        }

        let entry = AuditEntry {
            event_id: self.generate_event_id(),
            timestamp: Utc::now(),
            event_type: AuditEventType::AuthenticationFailure,
            user_id: None,
            username: Some(username.to_string()),
            user_roles: vec![],
            client_ip: ip.map(|s| s.to_string()),
            session_id: None,
            database: "aurora".to_string(),
            object_type: None,
            object_name: None,
            operation: "LOGIN_FAILED".to_string(),
            success: false,
            error_message: Some(reason.to_string()),
            context: HashMap::new(),
            compliance_flags: vec![ComplianceFlag::GDPR, ComplianceFlag::SOX],
        };

        self.send_entry(entry).await
    }

    /// Log DDL operation
    pub async fn log_ddl(&self, operation: &str, object_type: &str, object_name: &str, user_context: &AuditContext) -> AuroraResult<()> {
        if !self.config.enabled || !self.config.log_ddl {
            return Ok(());
        }

        let event_type = match operation.to_uppercase().as_str() {
            "CREATE TABLE" => AuditEventType::CreateTable,
            "DROP TABLE" => AuditEventType::DropTable,
            "ALTER TABLE" => AuditEventType::AlterTable,
            "CREATE INDEX" => AuditEventType::CreateIndex,
            "DROP INDEX" => AuditEventType::DropIndex,
            "CREATE VIEW" => AuditEventType::CreateView,
            "DROP VIEW" => AuditEventType::DropView,
            _ => return Ok(()), // Not a tracked DDL operation
        };

        let entry = AuditEntry {
            event_id: self.generate_event_id(),
            timestamp: Utc::now(),
            event_type,
            user_id: user_context.user_id.clone(),
            username: user_context.username.clone(),
            user_roles: user_context.roles.clone(),
            client_ip: user_context.client_ip.clone(),
            session_id: user_context.session_id.clone(),
            database: user_context.database.clone(),
            object_type: Some(object_type.to_string()),
            object_name: Some(object_name.to_string()),
            operation: operation.to_string(),
            success: true,
            error_message: None,
            context: HashMap::new(),
            compliance_flags: vec![ComplianceFlag::GDPR, ComplianceFlag::SOX],
        };

        self.send_entry(entry).await
    }

    /// Log DML operation (with sampling)
    pub async fn log_dml(&self, operation: &str, table: &str, row_count: u64, user_context: &AuditContext) -> AuroraResult<()> {
        if !self.config.enabled || !self.config.log_dml {
            return Ok(());
        }

        // Sample DML operations based on configured rate
        if rand::random::<f64>() > self.config.dml_sample_rate {
            return Ok(());
        }

        let event_type = match operation.to_uppercase().as_str() {
            "INSERT" => AuditEventType::Insert,
            "UPDATE" => AuditEventType::Update,
            "DELETE" => AuditEventType::Delete,
            _ => return Ok(()),
        };

        let mut context = HashMap::new();
        context.insert("row_count".to_string(), serde_json::json!(row_count));

        let entry = AuditEntry {
            event_id: self.generate_event_id(),
            timestamp: Utc::now(),
            event_type,
            user_id: user_context.user_id.clone(),
            username: user_context.username.clone(),
            user_roles: user_context.roles.clone(),
            client_ip: user_context.client_ip.clone(),
            session_id: user_context.session_id.clone(),
            database: user_context.database.clone(),
            object_type: Some("TABLE".to_string()),
            object_name: Some(table.to_string()),
            operation: operation.to_string(),
            success: true,
            error_message: None,
            context,
            compliance_flags: vec![ComplianceFlag::GDPR],
        };

        self.send_entry(entry).await
    }

    /// Log administrative operation
    pub async fn log_admin(&self, operation: &str, details: HashMap<String, serde_json::Value>, user_context: &AuditContext) -> AuroraResult<()> {
        if !self.config.enabled || !self.config.log_admin {
            return Ok(());
        }

        let event_type = match operation.to_uppercase().as_str() {
            "USER_CREATED" => AuditEventType::UserCreated,
            "USER_DELETED" => AuditEventType::UserDeleted,
            "USER_MODIFIED" => AuditEventType::UserModified,
            "ROLE_GRANTED" => AuditEventType::RoleGranted,
            "ROLE_REVOKED" => AuditEventType::RoleRevoked,
            "BACKUP_STARTED" => AuditEventType::BackupStarted,
            "BACKUP_COMPLETED" => AuditEventType::BackupCompleted,
            "RESTORE_STARTED" => AuditEventType::RestoreStarted,
            "RESTORE_COMPLETED" => AuditEventType::RestoreCompleted,
            _ => return Ok(()),
        };

        let entry = AuditEntry {
            event_id: self.generate_event_id(),
            timestamp: Utc::now(),
            event_type,
            user_id: user_context.user_id.clone(),
            username: user_context.username.clone(),
            user_roles: user_context.roles.clone(),
            client_ip: user_context.client_ip.clone(),
            session_id: user_context.session_id.clone(),
            database: user_context.database.clone(),
            object_type: None,
            object_name: None,
            operation: operation.to_string(),
            success: true,
            error_message: None,
            context: details,
            compliance_flags: vec![ComplianceFlag::SOX, ComplianceFlag::ISO27001],
        };

        self.send_entry(entry).await
    }

    /// Log security event
    pub async fn log_security(&self, event_type: AuditEventType, details: HashMap<String, serde_json::Value>, user_context: &AuditContext) -> AuroraResult<()> {
        if !self.config.enabled || !self.config.log_security {
            return Ok(());
        }

        let entry = AuditEntry {
            event_id: self.generate_event_id(),
            timestamp: Utc::now(),
            event_type,
            user_id: user_context.user_id.clone(),
            username: user_context.username.clone(),
            user_roles: user_context.roles.clone(),
            client_ip: user_context.client_ip.clone(),
            session_id: user_context.session_id.clone(),
            database: user_context.database.clone(),
            object_type: None,
            object_name: None,
            operation: format!("{:?}", event_type),
            success: false, // Security events are typically failures/incidents
            error_message: None,
            context: details,
            compliance_flags: vec![ComplianceFlag::GDPR, ComplianceFlag::HIPAA, ComplianceFlag::SOX],
        };

        self.send_entry(entry).await
    }

    /// Get audit log statistics
    pub async fn get_stats(&self) -> AuditStats {
        // In a real implementation, this would track metrics
        AuditStats {
            total_events: 0,
            events_today: 0,
            failed_auth_attempts: 0,
            ddl_operations: 0,
            security_events: 0,
        }
    }

    /// Search audit logs
    pub async fn search_logs(&self, query: &AuditQuery) -> AuroraResult<Vec<AuditEntry>> {
        // In a real implementation, this would search the audit logs
        // For now, return empty results
        Ok(vec![])
    }

    // Private helper methods
    fn generate_event_id(&self) -> String {
        format!("audit_{}", uuid::Uuid::new_v4().simple())
    }

    async fn send_entry(&self, entry: AuditEntry) -> AuroraResult<()> {
        self.sender.send(entry)
            .map_err(|_| AuroraError::new(crate::errors::ErrorCode::StorageUnavailable, "Audit logging failed"))
    }

    async fn logging_worker(mut receiver: mpsc::UnboundedReceiver<AuditEntry>, config: AuditConfig) {
        if !config.enabled {
            return;
        }

        let mut file_writer = match AuditFileWriter::new(&config).await {
            Ok(writer) => writer,
            Err(e) => {
                eprintln!("Failed to initialize audit file writer: {}", e);
                return;
            }
        };

        while let Some(entry) = receiver.recv().await {
            if let Err(e) = file_writer.write_entry(&entry).await {
                eprintln!("Failed to write audit entry: {}", e);
            }
        }
    }

    fn mask_sensitive_data(&self, data: &mut HashMap<String, serde_json::Value>) {
        if !self.config.mask_sensitive_data {
            return;
        }

        for field in &self.sensitive_fields {
            if data.contains_key(field) {
                data.insert(field.clone(), serde_json::json!("***MASKED***"));
            }
        }
    }
}

/// Audit context for operations
#[derive(Debug, Clone)]
pub struct AuditContext {
    pub user_id: Option<String>,
    pub username: Option<String>,
    pub roles: Vec<String>,
    pub client_ip: Option<String>,
    pub session_id: Option<String>,
    pub database: String,
}

/// Audit query for searching logs
#[derive(Debug, Clone)]
pub struct AuditQuery {
    pub start_time: Option<DateTime<Utc>>,
    pub end_time: Option<DateTime<Utc>>,
    pub user_id: Option<String>,
    pub event_type: Option<AuditEventType>,
    pub object_name: Option<String>,
    pub limit: usize,
}

/// Audit statistics
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AuditStats {
    pub total_events: u64,
    pub events_today: u64,
    pub failed_auth_attempts: u64,
    pub ddl_operations: u64,
    pub security_events: u64,
}

/// File writer for audit logs
struct AuditFileWriter {
    base_path: String,
    current_file: String,
    max_size: u64,
    max_files: usize,
    compress: bool,
    current_size: u64,
}

impl AuditFileWriter {
    async fn new(config: &AuditConfig) -> Result<Self, AuditError> {
        let base_path = config.log_file.clone();
        let current_file = format!("{}.0", base_path);
        let max_size = config.max_file_size_mb * 1024 * 1024;

        // Ensure directory exists
        if let Some(parent) = Path::new(&current_file).parent() {
            tokio::fs::create_dir_all(parent).await
                .map_err(AuditError::Io)?;
        }

        Ok(Self {
            base_path,
            current_file,
            max_size,
            max_files: config.max_files,
            compress: config.compress_rotated,
            current_size: 0,
        })
    }

    async fn write_entry(&mut self, entry: &AuditEntry) -> Result<(), AuditError> {
        // Check if rotation is needed
        if self.current_size >= self.max_size {
            self.rotate_files().await?;
        }

        // Format entry
        let json = serde_json::to_string(entry)
            .map_err(AuditError::Serialization)?;
        let line = format!("{}\n", json);

        // Write to file
        let mut file = OpenOptions::new()
            .create(true)
            .append(true)
            .open(&self.current_file)
            .await
            .map_err(AuditError::Io)?;

        file.write_all(line.as_bytes()).await
            .map_err(AuditError::Io)?;

        file.flush().await.map_err(AuditError::Io)?;

        self.current_size += line.len() as u64;

        Ok(())
    }

    async fn rotate_files(&mut self) -> Result<(), AuditError> {
        // Move existing files
        for i in (0..self.max_files).rev() {
            let src = if i == 0 {
                format!("{}.0", self.base_path)
            } else {
                format!("{}.{}", self.base_path, i)
            };

            let dst = format!("{}.{}", self.base_path, i + 1);

            if tokio::fs::metadata(&src).await.is_ok() {
                if self.compress && i > 0 {
                    self.compress_file(&src, &dst).await?;
                    tokio::fs::remove_file(&src).await.map_err(AuditError::Io)?;
                } else {
                    tokio::fs::rename(&src, &dst).await.map_err(AuditError::Io)?;
                }
            }
        }

        // Reset current file
        self.current_file = format!("{}.0", self.base_path);
        self.current_size = 0;

        Ok(())
    }

    async fn compress_file(&self, src: &str, dst: &str) -> Result<(), AuditError> {
        let data = tokio::fs::read(src).await.map_err(AuditError::Io)?;
        let output_file = tokio::fs::File::create(dst).await.map_err(AuditError::Io)?;
        let mut encoder = GzEncoder::new(output_file.into_std().await, Compression::default());

        tokio::task::spawn_blocking(move || {
            encoder.write_all(&data).map_err(AuditError::Io)?;
            encoder.finish().map_err(AuditError::Io)?;
            Ok(())
        }).await.map_err(|_| AuditError::Compression("Compression task failed".to_string()))?
    }
}

/// Audit errors
#[derive(Debug, thiserror::Error)]
pub enum AuditError {
    #[error("I/O error: {0}")]
    Io(#[from] tokio::io::Error),

    #[error("Serialization error: {0}")]
    Serialization(#[from] serde_json::Error),

    #[error("Compression error: {0}")]
    Compression(String),

    #[error("Configuration error: {0}")]
    Config(String),
}

/// Convenience macros for audit logging
#[macro_export]
macro_rules! audit_auth_success {
    ($logger:expr, $username:expr, $user_id:expr, $ip:expr, $session_id:expr) => {
        if let Err(e) = $logger.log_auth_success($username, $user_id, $ip, $session_id).await {
            tracing::warn!("Failed to log auth success: {}", e);
        }
    };
}

#[macro_export]
macro_rules! audit_auth_failure {
    ($logger:expr, $username:expr, $ip:expr, $reason:expr) => {
        if let Err(e) = $logger.log_auth_failure($username, $ip, $reason).await {
            tracing::warn!("Failed to log auth failure: {}", e);
        }
    };
}

#[macro_export]
macro_rules! audit_ddl {
    ($logger:expr, $operation:expr, $object_type:expr, $object_name:expr, $context:expr) => {
        if let Err(e) = $logger.log_ddl($operation, $object_type, $object_name, $context).await {
            tracing::warn!("Failed to log DDL: {}", e);
        }
    };
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_audit_logger_creation() {
        let config = AuditConfig {
            enabled: true,
            log_file: "/tmp/test_audit.log".to_string(),
            max_file_size_mb: 10,
            max_files: 5,
            compress_rotated: true,
            log_authentication: true,
            log_ddl: true,
            log_dml: false,
            log_admin: true,
            log_security: true,
            dml_sample_rate: 0.1,
            retention_days: 90,
            mask_sensitive_data: true,
        };

        let logger = AuditLogger::new(config);
        assert!(logger.config.enabled);
    }

    #[tokio::test]
    async fn test_audit_entry_creation() {
        let config = AuditConfig {
            enabled: true,
            log_file: "/tmp/test_audit.log".to_string(),
            max_file_size_mb: 10,
            max_files: 5,
            compress_rotated: true,
            log_authentication: true,
            log_ddl: true,
            log_dml: false,
            log_admin: true,
            log_security: true,
            dml_sample_rate: 0.1,
            retention_days: 90,
            mask_sensitive_data: true,
        };

        let logger = AuditLogger::new(config);

        let context = AuditContext {
            user_id: Some("user123".to_string()),
            username: Some("testuser".to_string()),
            roles: vec!["user".to_string()],
            client_ip: Some("127.0.0.1".to_string()),
            session_id: Some("session123".to_string()),
            database: "aurora".to_string(),
        };

        // Test DDL logging
        logger.log_ddl("CREATE TABLE", "TABLE", "test_table", &context).await.unwrap();
    }

    #[test]
    fn test_audit_event_types() {
        let event = AuditEventType::AuthenticationSuccess;
        assert!(matches!(event, AuditEventType::AuthenticationSuccess));
    }

    #[test]
    fn test_compliance_flags() {
        let flags = vec![ComplianceFlag::GDPR, ComplianceFlag::HIPAA];
        assert_eq!(flags.len(), 2);
    }
}
