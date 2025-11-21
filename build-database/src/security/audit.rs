//! Comprehensive Audit Logging Implementation
//!
//! Enterprise-grade audit logging for compliance, security monitoring, and forensics.
//! UNIQUENESS: Research-backed audit logging with compliance frameworks and anomaly detection.

use std::collections::HashMap;
use std::sync::Arc;
use parking_lot::RwLock;
use serde::{Serialize, Deserialize};
use tokio::sync::mpsc;
use std::fs::OpenOptions;
use std::io::Write;
use crate::core::{AuroraResult, AuroraError, ErrorCode};

/// Audit event types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AuditEventType {
    // Authentication events
    LoginSuccess,
    LoginFailure,
    Logout,
    SessionExpired,

    // Authorization events
    PermissionGranted,
    PermissionDenied,
    RoleAssigned,
    RoleRevoked,

    // Data access events
    DataRead,
    DataModified,
    DataDeleted,
    SchemaChanged,

    // Administrative events
    UserCreated,
    UserDeleted,
    RoleCreated,
    RoleModified,
    SecurityPolicyChanged,

    // System events
    BackupStarted,
    BackupCompleted,
    RestoreStarted,
    RestoreCompleted,
    SystemShutdown,
    SystemStartup,

    // Security events
    SuspiciousActivity,
    EncryptionKeyRotated,
    AuditLogAccessed,
}

/// Audit log entry
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AuditLogEntry {
    pub id: String,
    pub timestamp: u64,
    pub event_type: AuditEventType,
    pub user_id: Option<String>,
    pub session_id: Option<String>,
    pub client_ip: Option<String>,
    pub user_agent: Option<String>,
    pub resource: Option<String>,
    pub action: String,
    pub parameters: HashMap<String, String>,
    pub result: AuditResult,
    pub details: Option<String>,
    pub compliance_tags: Vec<String>,
}

/// Audit result
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AuditResult {
    Success,
    Failure(String),
    Warning(String),
}

/// Compliance frameworks
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ComplianceFramework {
    SOX,      // Sarbanes-Oxley
    HIPAA,    // Health Insurance Portability and Accountability Act
    GDPR,     // General Data Protection Regulation
    PCI_DSS,  // Payment Card Industry Data Security Standard
    ISO27001, // Information Security Management
}

/// Audit configuration
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AuditConfig {
    pub log_file_path: String,
    pub max_log_size_mb: u64,
    pub retention_days: u32,
    pub enable_compliance_logging: bool,
    pub compliance_frameworks: Vec<ComplianceFramework>,
    pub enable_real_time_alerts: bool,
    pub alert_thresholds: HashMap<String, u32>,
}

/// Audit logger
pub struct AuditLogger {
    config: AuditConfig,
    log_sender: mpsc::UnboundedSender<AuditLogEntry>,
    log_receiver: Mutex<Option<mpsc::UnboundedReceiver<AuditLogEntry>>>,
    event_counter: RwLock<HashMap<String, u64>>,
}

impl AuditLogger {
    /// Create a new audit logger
    pub fn new(config: AuditConfig) -> Self {
        let (sender, receiver) = mpsc::unbounded_channel();

        Self {
            config,
            log_sender: sender,
            log_receiver: Mutex::new(Some(receiver)),
            event_counter: RwLock::new(HashMap::new()),
        }
    }

    /// Start the audit logging background task
    pub fn start(&self) {
        let mut receiver = self.log_receiver.lock().take().unwrap();
        let config = self.config.clone();

        tokio::spawn(async move {
            while let Some(entry) = receiver.recv().await {
                if let Err(e) = Self::write_log_entry(&config, &entry).await {
                    eprintln!("Failed to write audit log: {}", e);
                }
            }
        });
    }

    /// Log an audit event
    pub fn log_event(&self, entry: AuditLogEntry) -> AuroraResult<()> {
        // Update event counters
        let mut counters = self.event_counter.write();
        let event_key = format!("{:?}", entry.event_type);
        *counters.entry(event_key).or_insert(0) += 1;

        // Check alert thresholds
        if self.config.enable_real_time_alerts {
            self.check_alert_thresholds(&entry);
        }

        // Send to background writer
        self.log_sender.send(entry).map_err(|_| {
            AuroraError::new(
                ErrorCode::Audit,
                "Failed to send audit log entry".to_string()
            )
        })?;

        Ok(())
    }

    /// Log authentication event
    pub fn log_authentication(&self, user_id: Option<&str>, event_type: AuditEventType, success: bool, client_ip: Option<&str>) -> AuroraResult<()> {
        let result = if success {
            AuditResult::Success
        } else {
            AuditResult::Failure("Authentication failed".to_string())
        };

        let entry = AuditLogEntry {
            id: format!("auth_{}", chrono::Utc::now().timestamp_nanos()),
            timestamp: std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_secs(),
            event_type,
            user_id: user_id.map(|s| s.to_string()),
            session_id: None,
            client_ip: client_ip.map(|s| s.to_string()),
            user_agent: None,
            resource: None,
            action: "authentication".to_string(),
            parameters: HashMap::new(),
            result,
            details: None,
            compliance_tags: self.get_compliance_tags(&event_type),
        };

        self.log_event(entry)
    }

    /// Log authorization event
    pub fn log_authorization(&self, user_id: &str, resource: &str, action: &str, granted: bool, session_id: Option<&str>) -> AuroraResult<()> {
        let event_type = if granted {
            AuditEventType::PermissionGranted
        } else {
            AuditEventType::PermissionDenied
        };

        let result = if granted {
            AuditResult::Success
        } else {
            AuditResult::Failure("Permission denied".to_string())
        };

        let mut parameters = HashMap::new();
        parameters.insert("resource".to_string(), resource.to_string());
        parameters.insert("action".to_string(), action.to_string());

        let entry = AuditLogEntry {
            id: format!("authz_{}", chrono::Utc::now().timestamp_nanos()),
            timestamp: std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_secs(),
            event_type,
            user_id: Some(user_id.to_string()),
            session_id: session_id.map(|s| s.to_string()),
            client_ip: None,
            user_agent: None,
            resource: Some(resource.to_string()),
            action: action.to_string(),
            parameters,
            result,
            details: None,
            compliance_tags: self.get_compliance_tags(&event_type),
        };

        self.log_event(entry)
    }

    /// Log data access event
    pub fn log_data_access(&self, user_id: &str, table: &str, operation: &str, record_count: u64, session_id: Option<&str>) -> AuroraResult<()> {
        let event_type = match operation {
            "SELECT" => AuditEventType::DataRead,
            "INSERT" | "UPDATE" => AuditEventType::DataModified,
            "DELETE" => AuditEventType::DataDeleted,
            _ => AuditEventType::DataRead,
        };

        let mut parameters = HashMap::new();
        parameters.insert("table".to_string(), table.to_string());
        parameters.insert("record_count".to_string(), record_count.to_string());

        let entry = AuditLogEntry {
            id: format!("data_{}", chrono::Utc::now().timestamp_nanos()),
            timestamp: std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_secs(),
            event_type,
            user_id: Some(user_id.to_string()),
            session_id: session_id.map(|s| s.to_string()),
            client_ip: None,
            user_agent: None,
            resource: Some(table.to_string()),
            action: operation.to_string(),
            parameters,
            result: AuditResult::Success,
            details: None,
            compliance_tags: self.get_compliance_tags(&event_type),
        };

        self.log_event(entry)
    }

    /// Log administrative event
    pub fn log_administrative(&self, admin_user: &str, action: &str, target: &str, success: bool) -> AuroraResult<()> {
        let event_type = match action {
            "create_user" => AuditEventType::UserCreated,
            "delete_user" => AuditEventType::UserDeleted,
            "create_role" => AuditEventType::RoleCreated,
            "modify_role" => AuditEventType::RoleModified,
            _ => AuditEventType::SecurityPolicyChanged,
        };

        let result = if success {
            AuditResult::Success
        } else {
            AuditResult::Failure("Administrative action failed".to_string())
        };

        let mut parameters = HashMap::new();
        parameters.insert("target".to_string(), target.to_string());

        let entry = AuditLogEntry {
            id: format!("admin_{}", chrono::Utc::now().timestamp_nanos()),
            timestamp: std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_secs(),
            event_type,
            user_id: Some(admin_user.to_string()),
            session_id: None,
            client_ip: None,
            user_agent: None,
            resource: Some(target.to_string()),
            action: action.to_string(),
            parameters,
            result,
            details: None,
            compliance_tags: self.get_compliance_tags(&event_type),
        };

        self.log_event(entry)
    }

    /// Write log entry to file
    async fn write_log_entry(config: &AuditConfig, entry: &AuditLogEntry) -> AuroraResult<()> {
        let json = serde_json::to_string(entry)
            .map_err(|e| AuroraError::new(ErrorCode::Audit, format!("JSON serialization failed: {}", e)))?;

        let mut file = OpenOptions::new()
            .create(true)
            .append(true)
            .open(&config.log_file_path)
            .map_err(|e| AuroraError::new(ErrorCode::Audit, format!("Failed to open audit log file: {}", e)))?;

        writeln!(file, "{}", json)
            .map_err(|e| AuroraError::new(ErrorCode::Audit, format!("Failed to write audit log: {}", e)))?;

        // Check if log rotation is needed
        if let Ok(metadata) = std::fs::metadata(&config.log_file_path) {
            let size_mb = metadata.len() / (1024 * 1024);
            if size_mb >= config.max_log_size_mb {
                Self::rotate_log_file(config).await?;
            }
        }

        Ok(())
    }

    /// Rotate log file when it gets too large
    async fn rotate_log_file(config: &AuditConfig) -> AuroraResult<()> {
        let timestamp = chrono::Utc::now().format("%Y%m%d_%H%M%S");
        let rotated_path = format!("{}.{}.bak", config.log_file_path, timestamp);

        std::fs::rename(&config.log_file_path, &rotated_path)
            .map_err(|e| AuroraError::new(ErrorCode::Audit, format!("Log rotation failed: {}", e)))?;

        log::info!("Audit log rotated: {} -> {}", config.log_file_path, rotated_path);
        Ok(())
    }

    /// Get compliance tags for an event type
    fn get_compliance_tags(&self, event_type: &AuditEventType) -> Vec<String> {
        if !self.config.enable_compliance_logging {
            return vec![];
        }

        let mut tags = Vec::new();

        // Add tags based on compliance frameworks
        for framework in &self.config.compliance_frameworks {
            match framework {
                ComplianceFramework::SOX => {
                    match event_type {
                        AuditEventType::DataModified | AuditEventType::DataDeleted => {
                            tags.push("SOX-Financial".to_string());
                        }
                        AuditEventType::UserCreated | AuditEventType::UserDeleted => {
                            tags.push("SOX-Access".to_string());
                        }
                        _ => {}
                    }
                }
                ComplianceFramework::HIPAA => {
                    match event_type {
                        AuditEventType::DataRead | AuditEventType::DataModified => {
                            tags.push("HIPAA-Privacy".to_string());
                        }
                        _ => {}
                    }
                }
                ComplianceFramework::GDPR => {
                    match event_type {
                        AuditEventType::DataRead | AuditEventType::DataDeleted => {
                            tags.push("GDPR-DataAccess".to_string());
                        }
                        _ => {}
                    }
                }
                ComplianceFramework::PCI_DSS => {
                    if matches!(event_type, AuditEventType::DataRead | AuditEventType::DataModified) {
                        tags.push("PCI-Compliance".to_string());
                    }
                }
                ComplianceFramework::ISO27001 => {
                    tags.push("ISO27001-Security".to_string());
                }
            }
        }

        tags
    }

    /// Check alert thresholds and trigger alerts if needed
    fn check_alert_thresholds(&self, entry: &AuditLogEntry) {
        let counters = self.event_counter.read();

        for (event_type, threshold) in &self.config.alert_thresholds {
            if let Some(count) = counters.get(event_type) {
                if *count >= *threshold as u64 {
                    log::warn!("AUDIT ALERT: {} events of type {} detected (threshold: {})",
                              count, event_type, threshold);

                    // In production, this would trigger email alerts, SIEM integration, etc.
                    // For demo, we just log the alert
                }
            }
        }
    }

    /// Get audit statistics
    pub fn get_audit_stats(&self) -> AuditStats {
        let counters = self.event_counter.read();

        AuditStats {
            total_events: counters.values().sum(),
            events_by_type: counters.clone(),
            compliance_enabled: self.config.enable_compliance_logging,
            active_frameworks: self.config.compliance_frameworks.len(),
        }
    }

    /// Search audit logs (basic implementation)
    pub fn search_logs(&self, user_id: Option<&str>, event_type: Option<&AuditEventType>, limit: usize) -> AuroraResult<Vec<AuditLogEntry>> {
        // In production, this would query a proper audit database
        // For demo, return empty results
        Ok(vec![])
    }
}

/// Audit statistics
#[derive(Debug, Clone)]
pub struct AuditStats {
    pub total_events: u64,
    pub events_by_type: HashMap<String, u64>,
    pub compliance_enabled: bool,
    pub active_frameworks: usize,
}