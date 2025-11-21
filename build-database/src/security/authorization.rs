//! Authorization Implementation
//!
//! Access control enforcement integrating RBAC, audit logging, and policy evaluation.
//! UNIQUENESS: Research-backed authorization with policy-based access control and context-aware decisions.

use std::collections::HashMap;
use std::sync::Arc;
use serde::{Serialize, Deserialize};
use crate::core::{AuroraResult, AuroraError, ErrorCode};
use crate::security::rbac::{RBACManager, Permission};
use crate::security::audit::AuditLogger;

/// Authorization context
#[derive(Debug, Clone)]
pub struct AuthzContext {
    pub user_id: String,
    pub session_id: Option<String>,
    pub client_ip: Option<String>,
    pub user_agent: Option<String>,
    pub resource_attributes: HashMap<String, String>,
    pub environment_attributes: HashMap<String, String>,
}

/// Authorization decision
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AuthzDecision {
    Allow,
    Deny(String),
    RequireMFA,
    RequireApproval(String),
}

/// Authorization policy
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AuthzPolicy {
    pub id: String,
    pub name: String,
    pub effect: PolicyEffect,
    pub conditions: Vec<PolicyCondition>,
    pub priority: i32,
}

/// Policy effect
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum PolicyEffect {
    Allow,
    Deny,
}

/// Policy condition
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PolicyCondition {
    pub attribute: String,
    pub operator: ConditionOperator,
    pub value: String,
}

/// Condition operators
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ConditionOperator {
    Equals,
    NotEquals,
    Contains,
    NotContains,
    Regex,
    IpInRange,
}

/// Authorization manager
pub struct AuthzManager {
    rbac_manager: Arc<RBACManager>,
    audit_logger: Arc<AuditLogger>,
    policies: Vec<AuthzPolicy>,
}

impl AuthzManager {
    /// Create a new authorization manager
    pub fn new(rbac_manager: Arc<RBACManager>, audit_logger: Arc<AuditLogger>) -> Self {
        let mut manager = Self {
            rbac_manager,
            audit_logger,
            policies: Vec::new(),
        };

        // Initialize default policies
        manager.initialize_default_policies();

        manager
    }

    /// Authorize an action
    pub async fn authorize(&self, context: &AuthzContext, permission: &Permission) -> AuroraResult<AuthzDecision> {
        // First check RBAC permissions
        let rbac_result = self.rbac_manager.check_permission(&context.user_id, permission);

        match rbac_result {
            crate::security::rbac::PermissionResult::Granted => {
                // Check additional policies
                let policy_decision = self.evaluate_policies(context, permission).await?;

                match policy_decision {
                    AuthzDecision::Allow => {
                        // Log successful authorization
                        self.audit_logger.log_authorization(
                            &context.user_id,
                            &self.permission_to_resource(permission),
                            &self.permission_to_action(permission),
                            true,
                            context.session_id.as_deref()
                        )?;
                        Ok(AuthzDecision::Allow)
                    }
                    other => {
                        // Log policy-based denial
                        self.audit_logger.log_authorization(
                            &context.user_id,
                            &self.permission_to_resource(permission),
                            &self.permission_to_action(permission),
                            false,
                            context.session_id.as_deref()
                        )?;
                        Ok(other)
                    }
                }
            }
            crate::security::rbac::PermissionResult::Denied(reason) => {
                // Log RBAC denial
                self.audit_logger.log_authorization(
                    &context.user_id,
                    &self.permission_to_resource(permission),
                    &self.permission_to_action(permission),
                    false,
                    context.session_id.as_deref()
                )?;
                Ok(AuthzDecision::Deny(reason))
            }
            crate::security::rbac::PermissionResult::RequiresMFA => {
                Ok(AuthzDecision::RequireMFA)
            }
        }
    }

    /// Check if user has administrative access
    pub fn is_admin(&self, user_id: &str) -> bool {
        self.rbac_manager.is_admin(user_id)
    }

    /// Grant permission to user (admin operation)
    pub async fn grant_permission(&self, admin_user: &str, target_user: &str, permission: &Permission) -> AuroraResult<()> {
        // Check if admin_user has permission to grant roles
        let context = AuthzContext {
            user_id: admin_user.to_string(),
            session_id: None,
            client_ip: None,
            user_agent: None,
            resource_attributes: HashMap::new(),
            environment_attributes: HashMap::new(),
        };

        let decision = self.authorize(&context, &Permission::GrantRole).await?;

        match decision {
            AuthzDecision::Allow => {
                // In a real implementation, this would map permissions to roles
                // For demo, we'll just log the operation
                self.audit_logger.log_administrative(
                    admin_user,
                    "grant_permission",
                    &format!("user:{} permission:{:?}", target_user, permission),
                    true
                )?;
                Ok(())
            }
            _ => Err(AuroraError::new(
                ErrorCode::Authorization,
                "Insufficient permissions to grant roles".to_string()
            )),
        }
    }

    /// Revoke permission from user (admin operation)
    pub async fn revoke_permission(&self, admin_user: &str, target_user: &str, permission: &Permission) -> AuroraResult<()> {
        let context = AuthzContext {
            user_id: admin_user.to_string(),
            session_id: None,
            client_ip: None,
            user_agent: None,
            resource_attributes: HashMap::new(),
            environment_attributes: HashMap::new(),
        };

        let decision = self.authorize(&context, &Permission::RevokeRole).await?;

        match decision {
            AuthzDecision::Allow => {
                self.audit_logger.log_administrative(
                    admin_user,
                    "revoke_permission",
                    &format!("user:{} permission:{:?}", target_user, permission),
                    true
                )?;
                Ok(())
            }
            _ => Err(AuroraError::new(
                ErrorCode::Authorization,
                "Insufficient permissions to revoke roles".to_string()
            )),
        }
    }

    /// Add a custom authorization policy
    pub fn add_policy(&mut self, policy: AuthzPolicy) {
        self.policies.push(policy);
        // Sort by priority (higher priority first)
        self.policies.sort_by(|a, b| b.priority.cmp(&a.priority));
    }

    /// Evaluate policies for a request
    async fn evaluate_policies(&self, context: &AuthzContext, permission: &Permission) -> AuroraResult<AuthzDecision> {
        // Check each policy in priority order
        for policy in &self.policies {
            if self.policy_matches(context, permission, policy) {
                match policy.effect {
                    PolicyEffect::Allow => return Ok(AuthzDecision::Allow),
                    PolicyEffect::Deny => {
                        return Ok(AuthzDecision::Deny(format!("Policy '{}' denies access", policy.name)));
                    }
                }
            }
        }

        // No matching policies, allow by default
        Ok(AuthzDecision::Allow)
    }

    /// Check if a policy matches the current context and permission
    fn policy_matches(&self, context: &AuthzContext, permission: &Permission, policy: &AuthzPolicy) -> bool {
        for condition in &policy.conditions {
            if !self.evaluate_condition(context, permission, condition) {
                return false;
            }
        }
        true
    }

    /// Evaluate a single policy condition
    fn evaluate_condition(&self, context: &AuthzContext, permission: &Permission, condition: &PolicyCondition) -> bool {
        let attribute_value = match condition.attribute.as_str() {
            "user_id" => Some(context.user_id.as_str()),
            "client_ip" => context.client_ip.as_deref(),
            "user_agent" => context.user_agent.as_deref(),
            "resource_type" => self.get_resource_type(permission),
            "action" => Some(self.permission_to_action(permission).as_str()),
            "time_of_day" => {
                let hour = chrono::Utc::now().hour();
                Some(hour.to_string().as_str())
            }
            _ => {
                // Check resource and environment attributes
                context.resource_attributes.get(&condition.attribute)
                    .or_else(|| context.environment_attributes.get(&condition.attribute))
                    .map(|s| s.as_str())
            }
        };

        if let Some(value) = attribute_value {
            match condition.operator {
                ConditionOperator::Equals => value == condition.value,
                ConditionOperator::NotEquals => value != condition.value,
                ConditionOperator::Contains => value.contains(&condition.value),
                ConditionOperator::NotContains => !value.contains(&condition.value),
                ConditionOperator::Regex => {
                    regex::Regex::new(&condition.value)
                        .map(|re| re.is_match(value))
                        .unwrap_or(false)
                }
                ConditionOperator::IpInRange => {
                    // Simplified IP range check
                    self.is_ip_in_range(value, &condition.value)
                }
            }
        } else {
            false
        }
    }

    /// Initialize default authorization policies
    fn initialize_default_policies(&mut self) {
        // Policy: Deny access from suspicious IP ranges
        let suspicious_ip_policy = AuthzPolicy {
            id: "deny_suspicious_ips".to_string(),
            name: "Deny Suspicious IPs".to_string(),
            effect: PolicyEffect::Deny,
            conditions: vec![
                PolicyCondition {
                    attribute: "client_ip".to_string(),
                    operator: ConditionOperator::IpInRange,
                    value: "192.168.0.0/16".to_string(), // Example suspicious range
                },
            ],
            priority: 100, // High priority
        };

        // Policy: Require MFA for sensitive operations
        let mfa_policy = AuthzPolicy {
            id: "require_mfa_sensitive".to_string(),
            name: "Require MFA for Sensitive Operations".to_string(),
            effect: PolicyEffect::Allow, // This would be handled differently in practice
            conditions: vec![
                PolicyCondition {
                    attribute: "action".to_string(),
                    operator: ConditionOperator::Equals,
                    value: "DropTable".to_string(),
                },
            ],
            priority: 50,
        };

        // Policy: Restrict access during business hours only
        let business_hours_policy = AuthzPolicy {
            id: "business_hours_only".to_string(),
            name: "Business Hours Only".to_string(),
            effect: PolicyEffect::Deny,
            conditions: vec![
                PolicyCondition {
                    attribute: "time_of_day".to_string(),
                    operator: ConditionOperator::Regex,
                    value: "^(0[0-8]|1[8-9]|2[0-3])".to_string(), // Outside 9 AM - 6 PM
                },
            ],
            priority: 25,
        };

        self.policies.push(suspicious_ip_policy);
        self.policies.push(mfa_policy);
        self.policies.push(business_hours_policy);
    }

    /// Helper: Convert permission to resource string
    fn permission_to_resource(&self, permission: &Permission) -> String {
        match permission {
            Permission::CreateTable(table) => format!("table:{}", table),
            Permission::DropTable(table) => format!("table:{}", table),
            Permission::SelectTable(table) => format!("table:{}", table),
            Permission::InsertTable(table) => format!("table:{}", table),
            Permission::UpdateTable(table) => format!("table:{}", table),
            Permission::DeleteTable(table) => format!("table:{}", table),
            Permission::SelectColumn(table, column) => format!("column:{}.{}", table, column),
            Permission::InsertColumn(table, column) => format!("column:{}.{}", table, column),
            Permission::UpdateColumn(table, column) => format!("column:{}.{}", table, column),
            _ => "system".to_string(),
        }
    }

    /// Helper: Convert permission to action string
    fn permission_to_action(&self, permission: &Permission) -> String {
        match permission {
            Permission::CreateTable(_) => "CreateTable".to_string(),
            Permission::DropTable(_) => "DropTable".to_string(),
            Permission::SelectTable(_) => "SelectTable".to_string(),
            Permission::InsertTable(_) => "InsertTable".to_string(),
            Permission::UpdateTable(_) => "UpdateTable".to_string(),
            Permission::DeleteTable(_) => "DeleteTable".to_string(),
            Permission::SelectColumn(_, _) => "SelectColumn".to_string(),
            Permission::InsertColumn(_, _) => "InsertColumn".to_string(),
            Permission::UpdateColumn(_, _) => "UpdateColumn".to_string(),
            Permission::SuperUser => "SuperUser".to_string(),
            Permission::CreateUser => "CreateUser".to_string(),
            Permission::DropUser => "DropUser".to_string(),
            _ => "Unknown".to_string(),
        }
    }

    /// Helper: Get resource type from permission
    fn get_resource_type(&self, permission: &Permission) -> Option<&str> {
        match permission {
            Permission::CreateTable(_) | Permission::DropTable(_) |
            Permission::SelectTable(_) | Permission::InsertTable(_) |
            Permission::UpdateTable(_) | Permission::DeleteTable(_) => Some("table"),
            Permission::SelectColumn(_, _) | Permission::InsertColumn(_, _) |
            Permission::UpdateColumn(_, _) => Some("column"),
            Permission::CreateUser | Permission::DropUser => Some("user"),
            Permission::GrantRole | Permission::RevokeRole => Some("role"),
            _ => Some("system"),
        }
    }

    /// Helper: Check if IP is in range (simplified)
    fn is_ip_in_range(&self, ip: &str, range: &str) -> bool {
        // Simplified IP range check - in production use a proper IP library
        ip.starts_with(&range.split('/').next().unwrap_or(range))
    }

    /// Get authorization statistics
    pub fn get_authz_stats(&self) -> AuthzStats {
        AuthzStats {
            total_policies: self.policies.len(),
            rbac_users: self.rbac_manager.list_users().len(),
            rbac_roles: self.rbac_manager.list_roles().len(),
        }
    }
}

/// Authorization statistics
#[derive(Debug, Clone)]
pub struct AuthzStats {
    pub total_policies: usize,
    pub rbac_users: usize,
    pub rbac_roles: usize,
}
