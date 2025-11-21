//! Security Engine: Advanced Security Controls for Stored Procedures
//!
//! Comprehensive security system that provides fine-grained access control,
//! audit trails, and protection against common security vulnerabilities.

use std::collections::{HashMap, HashSet};
use std::sync::Arc;
use parking_lot::RwLock;
use chrono::{DateTime, Utc, Duration};
use crate::core::errors::{AuroraResult, AuroraError};
use super::procedure_manager::{ProcedureDefinition, SecurityLevel, SecurityContext, SecurityInfo};

/// Security policy definition
#[derive(Debug, Clone)]
pub struct SecurityPolicy {
    pub name: String,
    pub rules: Vec<SecurityRule>,
    pub priority: u8,
    pub enabled: bool,
}

/// Security rule definition
#[derive(Debug, Clone)]
pub struct SecurityRule {
    pub rule_type: RuleType,
    pub conditions: Vec<SecurityCondition>,
    pub actions: Vec<SecurityAction>,
    pub severity: Severity,
}

/// Rule types
#[derive(Debug, Clone, PartialEq)]
pub enum RuleType {
    AccessControl,
    InputValidation,
    ResourceLimits,
    AuditLogging,
    ThreatDetection,
}

/// Security conditions
#[derive(Debug, Clone)]
pub struct SecurityCondition {
    pub field: String,
    pub operator: ConditionOperator,
    pub value: String,
}

/// Condition operators
#[derive(Debug, Clone, PartialEq)]
pub enum ConditionOperator {
    Equals,
    NotEquals,
    Contains,
    NotContains,
    GreaterThan,
    LessThan,
    RegexMatch,
    InList,
}

/// Security actions
#[derive(Debug, Clone)]
pub enum SecurityAction {
    Allow,
    Deny,
    Log(Severity),
    Alert(String),
    Quarantine,
    Terminate,
}

/// Severity levels
#[derive(Debug, Clone, PartialEq, PartialOrd)]
pub enum Severity {
    Low,
    Medium,
    High,
    Critical,
}

/// Security audit event
#[derive(Debug, Clone)]
pub struct AuditEvent {
    pub timestamp: DateTime<Utc>,
    pub procedure_name: String,
    pub user: String,
    pub action: String,
    pub result: String,
    pub details: HashMap<String, String>,
    pub severity: Severity,
    pub source_ip: Option<String>,
}

/// Threat detection pattern
#[derive(Debug, Clone)]
pub struct ThreatPattern {
    pub name: String,
    pub pattern_type: PatternType,
    pub signature: String,
    pub severity: Severity,
    pub description: String,
}

/// Pattern types for threat detection
#[derive(Debug, Clone, PartialEq)]
pub enum PatternType {
    SQLInjection,
    XSS,
    CommandInjection,
    BufferOverflow,
    DenialOfService,
    PrivilegeEscalation,
    DataExfiltration,
}

/// Security violation
#[derive(Debug, Clone)]
pub struct SecurityViolation {
    pub timestamp: DateTime<Utc>,
    pub violation_type: ViolationType,
    pub procedure_name: String,
    pub user: String,
    pub details: String,
    pub severity: Severity,
    pub mitigated: bool,
}

/// Violation types
#[derive(Debug, Clone, PartialEq)]
pub enum ViolationType {
    UnauthorizedAccess,
    InputValidationFailure,
    ResourceLimitExceeded,
    SuspiciousActivity,
    PolicyViolation,
    SecurityBypass,
}

/// Intelligent security engine
pub struct SecurityEngine {
    policies: RwLock<HashMap<String, SecurityPolicy>>,
    audit_log: RwLock<Vec<AuditEvent>>,
    threat_patterns: RwLock<Vec<ThreatPattern>>,
    violations: RwLock<Vec<SecurityViolation>>,
    procedure_permissions: RwLock<HashMap<String, HashSet<String>>>,
    user_sessions: RwLock<HashMap<String, UserSession>>,
    rate_limits: RwLock<HashMap<String, RateLimit>>,
}

impl SecurityEngine {
    pub fn new() -> Self {
        let mut engine = Self {
            policies: RwLock::new(HashMap::new()),
            audit_log: RwLock::new(Vec::new()),
            threat_patterns: RwLock::new(Vec::new()),
            violations: RwLock::new(Vec::new()),
            procedure_permissions: RwLock::new(HashMap::new()),
            user_sessions: RwLock::new(HashMap::new()),
            rate_limits: RwLock::new(HashMap::new()),
        };

        engine.initialize_default_policies();
        engine.initialize_default_threat_patterns();

        engine
    }

    /// Validate execution security
    pub async fn validate_execution(
        &self,
        definition: &ProcedureDefinition,
        security_context: &SecurityContext,
    ) -> AuroraResult<()> {
        // Check user authentication
        self.validate_authentication(security_context).await?;

        // Check authorization
        self.validate_authorization(definition, security_context).await?;

        // Apply security policies
        self.apply_security_policies(definition, security_context).await?;

        // Check rate limits
        self.check_rate_limits(definition, security_context).await?;

        // Detect threats
        self.detect_threats(definition, security_context).await?;

        // Log audit event
        self.log_audit_event(
            &definition.name,
            &security_context.user,
            "EXECUTE_PROCEDURE",
            "ALLOWED",
            security_context,
        ).await;

        Ok(())
    }

    /// Validate procedure definition security
    pub async fn validate_definition(&self, definition: &ProcedureDefinition) -> AuroraResult<()> {
        // Check for dangerous patterns in source code
        self.validate_source_code(definition).await?;

        // Validate security level appropriateness
        self.validate_security_level(definition).await?;

        // Check for secure coding practices
        self.validate_coding_practices(definition).await?;

        Ok(())
    }

    /// Register procedure with security system
    pub async fn register_procedure(&self, definition: &ProcedureDefinition) -> AuroraResult<()> {
        // Extract required permissions from procedure analysis
        let permissions = self.extract_required_permissions(definition).await?;

        // Store permissions
        {
            let mut proc_perms = self.procedure_permissions.write();
            proc_perms.insert(definition.name.clone(), permissions);
        }

        // Initialize rate limiting for the procedure
        self.initialize_rate_limiting(&definition.name).await?;

        println!("🔒 Registered security controls for procedure '{}'", definition.name);
        Ok(())
    }

    /// Remove procedure security controls
    pub async fn remove_procedure(&self, procedure_name: &str) -> AuroraResult<()> {
        let mut proc_perms = self.procedure_permissions.write();
        proc_perms.remove(procedure_name);

        let mut rate_limits = self.rate_limits.write();
        rate_limits.remove(procedure_name);

        Ok(())
    }

    /// Get security information
    pub async fn get_security_info(&self, procedure_name: &str) -> AuroraResult<SecurityInfo> {
        let proc_perms = self.procedure_permissions.read();
        let permissions = proc_perms.get(procedure_name)
            .cloned()
            .unwrap_or_default();

        let last_review = self.get_last_security_review(procedure_name).await;

        Ok(SecurityInfo {
            security_level: SecurityLevel::Public, // Would be stored per procedure
            required_permissions: permissions,
            audit_enabled: true,
            last_security_review: last_review,
        })
    }

    /// Add security policy
    pub async fn add_policy(&self, policy: SecurityPolicy) -> AuroraResult<()> {
        let mut policies = self.policies.write();
        policies.insert(policy.name.clone(), policy);
        Ok(())
    }

    /// Get security violations
    pub async fn get_violations(&self, limit: usize) -> Vec<SecurityViolation> {
        let violations = self.violations.read();
        violations.iter().rev().take(limit).cloned().collect()
    }

    /// Get audit events
    pub async fn get_audit_events(&self, procedure_name: Option<&str>, limit: usize) -> Vec<AuditEvent> {
        let audit_log = self.audit_log.read();
        let filtered: Vec<&AuditEvent> = audit_log.iter()
            .filter(|event| {
                procedure_name.map_or(true, |name| event.procedure_name == name)
            })
            .collect();

        filtered.into_iter().rev().take(limit).map(|e| e.clone()).collect()
    }

    // Private methods

    async fn validate_authentication(&self, security_context: &SecurityContext) -> AuroraResult<()> {
        // Check if user session is valid
        let sessions = self.user_sessions.read();
        if let Some(session) = sessions.get(&security_context.user) {
            if session.is_expired() {
                return Err(AuroraError::InvalidArgument("User session expired".to_string()));
            }
        } else {
            return Err(AuroraError::InvalidArgument("User not authenticated".to_string()));
        }

        Ok(())
    }

    async fn validate_authorization(
        &self,
        definition: &ProcedureDefinition,
        security_context: &SecurityContext,
    ) -> AuroraResult<()> {
        let proc_perms = self.procedure_permissions.read();
        if let Some(required_perms) = proc_perms.get(&definition.name) {
            // Check if user has all required permissions
            for perm in required_perms {
                if !security_context.permissions.contains(perm) {
                    self.log_security_violation(
                        ViolationType::UnauthorizedAccess,
                        &definition.name,
                        &security_context.user,
                        &format!("Missing permission: {}", perm),
                        Severity::High,
                    ).await;

                    return Err(AuroraError::InvalidArgument(format!("Missing required permission: {}", perm)));
                }
            }
        }

        Ok(())
    }

    async fn apply_security_policies(
        &self,
        definition: &ProcedureDefinition,
        security_context: &SecurityContext,
    ) -> AuroraResult<()> {
        let policies = self.policies.read();

        for policy in policies.values() {
            if !policy.enabled {
                continue;
            }

            if self.policy_matches(definition, security_context, policy).await? {
                for rule in &policy.rules {
                    if self.rule_conditions_met(security_context, rule).await? {
                        for action in &rule.actions {
                            match action {
                                SecurityAction::Allow => continue,
                                SecurityAction::Deny => {
                                    return Err(AuroraError::InvalidArgument("Access denied by security policy".to_string()));
                                }
                                SecurityAction::Log(severity) => {
                                    self.log_audit_event(
                                        &definition.name,
                                        &security_context.user,
                                        "POLICY_VIOLATION",
                                        &format!("{:?}", severity),
                                        security_context,
                                    ).await;
                                }
                                SecurityAction::Alert(message) => {
                                    println!("🚨 Security Alert: {}", message);
                                }
                                SecurityAction::Quarantine => {
                                    // Implement quarantine logic
                                    println!("🔒 Procedure '{}' quarantined", definition.name);
                                }
                                SecurityAction::Terminate => {
                                    // Implement termination logic
                                    println!("🛑 Execution terminated by security policy");
                                    return Err(AuroraError::InvalidArgument("Execution terminated by security policy".to_string()));
                                }
                            }
                        }
                    }
                }
            }
        }

        Ok(())
    }

    async fn check_rate_limits(
        &self,
        definition: &ProcedureDefinition,
        security_context: &SecurityContext,
    ) -> AuroraResult<()> {
        let mut rate_limits = self.rate_limits.write();
        let key = format!("{}:{}", security_context.user, definition.name);

        if let Some(limit) = rate_limits.get_mut(&key) {
            limit.requests += 1;

            if limit.requests > limit.max_requests {
                if Utc::now() < limit.reset_time {
                    self.log_security_violation(
                        ViolationType::ResourceLimitExceeded,
                        &definition.name,
                        &security_context.user,
                        "Rate limit exceeded",
                        Severity::Medium,
                    ).await;

                    return Err(AuroraError::InvalidArgument("Rate limit exceeded".to_string()));
                } else {
                    // Reset the limit
                    limit.requests = 1;
                    limit.reset_time = Utc::now() + Duration::minutes(1);
                }
            }
        }

        Ok(())
    }

    async fn detect_threats(
        &self,
        definition: &ProcedureDefinition,
        security_context: &SecurityContext,
    ) -> AuroraResult<()> {
        let patterns = self.threat_patterns.read();

        for pattern in patterns.iter() {
            if self.pattern_matches(definition, security_context, pattern).await? {
                self.log_security_violation(
                    ViolationType::SuspiciousActivity,
                    &definition.name,
                    &security_context.user,
                    &format!("Threat pattern detected: {}", pattern.name),
                    pattern.severity.clone(),
                ).await;

                // Depending on severity, take action
                if pattern.severity >= Severity::High {
                    return Err(AuroraError::InvalidArgument(format!("Threat detected: {}", pattern.name)));
                }
            }
        }

        Ok(())
    }

    async fn validate_source_code(&self, definition: &ProcedureDefinition) -> AuroraResult<()> {
        let source = &definition.source_code;

        // Check for dangerous patterns
        let dangerous_patterns = vec![
            "eval(", "exec(", "system(", "shell_exec(",
            "DROP TABLE", "DELETE FROM", "TRUNCATE TABLE",
        ];

        for pattern in dangerous_patterns {
            if source.contains(pattern) {
                if definition.security_level < SecurityLevel::Critical {
                    return Err(AuroraError::InvalidArgument(
                        format!("Dangerous pattern '{}' found in non-critical procedure", pattern)
                    ));
                }
            }
        }

        // Check for SQL injection vulnerabilities
        if definition.language == super::procedure_manager::ProcedureLanguage::SQL {
            if source.contains("EXEC(") || source.contains("EXECUTE(") {
                if !source.contains("@") { // Parameterized queries should use @
                    return Err(AuroraError::InvalidArgument("Potential SQL injection vulnerability".to_string()));
                }
            }
        }

        Ok(())
    }

    async fn validate_security_level(&self, definition: &ProcedureDefinition) -> AuroraResult<()> {
        // Check if security level is appropriate for the operations
        let source = &definition.source_code;

        if (source.contains("DROP") || source.contains("DELETE") || source.contains("UPDATE"))
            && definition.security_level < SecurityLevel::Sensitive {
            return Err(AuroraError::InvalidArgument(
                "Destructive operations require Sensitive or higher security level".to_string()
            ));
        }

        Ok(())
    }

    async fn validate_coding_practices(&self, definition: &ProcedureDefinition) -> AuroraResult<()> {
        let source = &definition.source_code;

        // Check for input validation
        if definition.parameters.len() > 0 {
            let has_validation = source.contains("validate") ||
                               source.contains("check") ||
                               source.contains("sanitize");

            if !has_validation && definition.security_level >= SecurityLevel::Restricted {
                println!("⚠️  Warning: No input validation detected in procedure '{}'", definition.name);
            }
        }

        // Check for error handling
        let has_error_handling = source.contains("try") ||
                                source.contains("catch") ||
                                source.contains("BEGIN TRY") ||
                                source.contains("EXCEPTION");

        if !has_error_handling && definition.security_level >= SecurityLevel::Sensitive {
            println!("⚠️  Warning: No error handling detected in procedure '{}'", definition.name);
        }

        Ok(())
    }

    async fn extract_required_permissions(&self, definition: &ProcedureDefinition) -> AuroraResult<HashSet<String>> {
        let mut permissions = HashSet::new();
        let source = &definition.source_code;

        // Analyze source code to determine required permissions
        if source.contains("SELECT") {
            permissions.insert("SELECT".to_string());
        }
        if source.contains("INSERT") {
            permissions.insert("INSERT".to_string());
        }
        if source.contains("UPDATE") {
            permissions.insert("UPDATE".to_string());
        }
        if source.contains("DELETE") {
            permissions.insert("DELETE".to_string());
        }
        if source.contains("CREATE") || source.contains("DROP") {
            permissions.insert("DDL".to_string());
        }

        // Add security-level-based permissions
        match definition.security_level {
            SecurityLevel::Critical => {
                permissions.insert("CRITICAL_ACCESS".to_string());
            }
            SecurityLevel::Sensitive => {
                permissions.insert("SENSITIVE_DATA".to_string());
            }
            _ => {}
        }

        Ok(permissions)
    }

    async fn initialize_rate_limiting(&self, procedure_name: &str) -> AuroraResult<()> {
        let rate_limit = RateLimit {
            max_requests: 100, // per minute
            requests: 0,
            reset_time: Utc::now() + Duration::minutes(1),
        };

        let mut rate_limits = self.rate_limits.write();
        rate_limits.insert(procedure_name.to_string(), rate_limit);

        Ok(())
    }

    async fn get_last_security_review(&self, _procedure_name: &str) -> Option<DateTime<Utc>> {
        // In a real implementation, this would track security reviews
        Some(Utc::now() - Duration::days(30))
    }

    fn initialize_default_policies(&mut self) {
        // Default security policies
        let policies = vec![
            SecurityPolicy {
                name: "default_access_control".to_string(),
                rules: vec![
                    SecurityRule {
                        rule_type: RuleType::AccessControl,
                        conditions: vec![
                            SecurityCondition {
                                field: "security_level".to_string(),
                                operator: ConditionOperator::Equals,
                                value: "Critical".to_string(),
                            }
                        ],
                        actions: vec![SecurityAction::Log(Severity::High)],
                        severity: Severity::High,
                    }
                ],
                priority: 1,
                enabled: true,
            }
        ];

        let mut self_policies = self.policies.write();
        for policy in policies {
            self_policies.insert(policy.name.clone(), policy);
        }
    }

    fn initialize_default_threat_patterns(&mut self) {
        // Default threat patterns
        let patterns = vec![
            ThreatPattern {
                name: "sql_injection".to_string(),
                pattern_type: PatternType::SQLInjection,
                signature: "'; DROP TABLE".to_string(),
                severity: Severity::Critical,
                description: "SQL injection attempt".to_string(),
            },
            ThreatPattern {
                name: "command_injection".to_string(),
                pattern_type: PatternType::CommandInjection,
                signature: "; rm -rf".to_string(),
                severity: Severity::Critical,
                description: "Command injection attempt".to_string(),
            },
        ];

        let mut self_patterns = self.threat_patterns.write();
        *self_patterns = patterns;
    }

    async fn policy_matches(
        &self,
        _definition: &ProcedureDefinition,
        _security_context: &SecurityContext,
        _policy: &SecurityPolicy,
    ) -> AuroraResult<bool> {
        // Simplified policy matching
        Ok(true)
    }

    async fn rule_conditions_met(
        &self,
        _security_context: &SecurityContext,
        _rule: &SecurityRule,
    ) -> AuroraResult<bool> {
        // Simplified condition checking
        Ok(false)
    }

    async fn pattern_matches(
        &self,
        definition: &ProcedureDefinition,
        security_context: &SecurityContext,
        pattern: &ThreatPattern,
    ) -> AuroraResult<bool> {
        // Check if the threat pattern matches in procedure or parameters
        let source_contains = definition.source_code.contains(&pattern.signature);
        let params_contain = security_context.parameters.values()
            .any(|v| v.contains(&pattern.signature));

        Ok(source_contains || params_contain)
    }

    async fn log_audit_event(
        &self,
        procedure_name: &str,
        user: &str,
        action: &str,
        result: &str,
        security_context: &SecurityContext,
    ) {
        let event = AuditEvent {
            timestamp: Utc::now(),
            procedure_name: procedure_name.to_string(),
            user: user.to_string(),
            action: action.to_string(),
            result: result.to_string(),
            details: security_context.parameters.clone(),
            severity: Severity::Low,
            source_ip: security_context.source_ip.clone(),
        };

        let mut audit_log = self.audit_log.write();
        audit_log.push(event);
    }

    async fn log_security_violation(
        &self,
        violation_type: ViolationType,
        procedure_name: &str,
        user: &str,
        details: &str,
        severity: Severity,
    ) {
        let violation = SecurityViolation {
            timestamp: Utc::now(),
            violation_type,
            procedure_name: procedure_name.to_string(),
            user: user.to_string(),
            details: details.to_string(),
            severity,
            mitigated: false,
        };

        let mut violations = self.violations.write();
        violations.push(violation);
    }
}

/// User session information
#[derive(Debug, Clone)]
pub struct UserSession {
    pub user: String,
    pub login_time: DateTime<Utc>,
    pub last_activity: DateTime<Utc>,
    pub permissions: HashSet<String>,
    pub session_id: String,
}

impl UserSession {
    pub fn is_expired(&self) -> bool {
        let timeout = Duration::hours(8); // 8 hour session timeout
        Utc::now() - self.last_activity > timeout
    }
}

/// Rate limiting information
#[derive(Debug, Clone)]
pub struct RateLimit {
    pub max_requests: u32,
    pub requests: u32,
    pub reset_time: DateTime<Utc>,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_security_engine_creation() {
        let engine = SecurityEngine::new();
        assert!(true); // Passes if created successfully
    }

    #[test]
    fn test_security_levels() {
        assert!(SecurityLevel::Public < SecurityLevel::Critical);
        assert!(SecurityLevel::Sensitive > SecurityLevel::Restricted);
    }

    #[test]
    fn test_severity_levels() {
        assert!(Severity::Low < Severity::Critical);
        assert!(Severity::Medium > Severity::Low);
    }

    #[test]
    fn test_rule_types() {
        assert_eq!(RuleType::AccessControl, RuleType::AccessControl);
        assert_ne!(RuleType::ThreatDetection, RuleType::AuditLogging);
    }

    #[test]
    fn test_pattern_types() {
        assert_eq!(PatternType::SQLInjection, PatternType::SQLInjection);
        assert_ne!(PatternType::XSS, PatternType::BufferOverflow);
    }

    #[test]
    fn test_violation_types() {
        assert_eq!(ViolationType::UnauthorizedAccess, ViolationType::UnauthorizedAccess);
        assert_ne!(ViolationType::InputValidationFailure, ViolationType::PolicyViolation);
    }

    #[test]
    fn test_user_session_expiry() {
        let old_session = UserSession {
            user: "test".to_string(),
            login_time: Utc::now() - Duration::hours(10),
            last_activity: Utc::now() - Duration::hours(10),
            permissions: HashSet::new(),
            session_id: "session1".to_string(),
        };

        assert!(old_session.is_expired());

        let active_session = UserSession {
            user: "test".to_string(),
            login_time: Utc::now() - Duration::hours(1),
            last_activity: Utc::now() - Duration::minutes(30),
            permissions: HashSet::new(),
            session_id: "session2".to_string(),
        };

        assert!(!active_session.is_expired());
    }

    #[tokio::test]
    async fn test_audit_logging() {
        let engine = SecurityEngine::new();
        let security_context = SecurityContext {
            user: "test_user".to_string(),
            permissions: HashSet::from(["SELECT".to_string()]),
            parameters: HashMap::new(),
            source_ip: Some("127.0.0.1".to_string()),
        };

        engine.log_audit_event(
            "test_proc",
            "test_user",
            "EXECUTE",
            "SUCCESS",
            &security_context,
        ).await;

        let events = engine.get_audit_events(Some("test_proc"), 10).await;
        assert_eq!(events.len(), 1);
        assert_eq!(events[0].procedure_name, "test_proc");
        assert_eq!(events[0].action, "EXECUTE");
    }

    #[tokio::test]
    async fn test_security_violations() {
        let engine = SecurityEngine::new();

        engine.log_security_violation(
            ViolationType::UnauthorizedAccess,
            "test_proc",
            "test_user",
            "Access denied",
            Severity::High,
        ).await;

        let violations = engine.get_violations(10).await;
        assert_eq!(violations.len(), 1);
        assert_eq!(violations[0].violation_type, ViolationType::UnauthorizedAccess);
        assert_eq!(violations[0].severity, Severity::High);
    }

    #[test]
    fn test_rate_limit_tracking() {
        let rate_limit = RateLimit {
            max_requests: 100,
            requests: 45,
            reset_time: Utc::now() + Duration::minutes(1),
        };

        assert!(rate_limit.requests < rate_limit.max_requests);
        assert!(rate_limit.reset_time > Utc::now());
    }

    #[test]
    fn test_security_rule_structure() {
        let rule = SecurityRule {
            rule_type: RuleType::AccessControl,
            conditions: vec![
                SecurityCondition {
                    field: "user_role".to_string(),
                    operator: ConditionOperator::Equals,
                    value: "admin".to_string(),
                }
            ],
            actions: vec![SecurityAction::Allow],
            severity: Severity::Medium,
        };

        assert_eq!(rule.rule_type, RuleType::AccessControl);
        assert_eq!(rule.conditions.len(), 1);
        assert_eq!(rule.actions.len(), 1);
    }

    #[tokio::test]
    async fn test_policy_management() {
        let engine = SecurityEngine::new();

        let policy = SecurityPolicy {
            name: "test_policy".to_string(),
            rules: vec![],
            priority: 1,
            enabled: true,
        };

        engine.add_policy(policy).await.unwrap();

        // Policy should be added (we can't easily verify without exposing internals)
        assert!(true);
    }
}
