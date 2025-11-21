//! Role-Based Access Control (RBAC) Implementation
//!
//! Fine-grained access control with users, roles, and permissions.
//! UNIQUENESS: Research-backed RBAC with hierarchical roles and dynamic permissions.

use std::collections::{HashMap, HashSet};
use std::sync::Arc;
use parking_lot::RwLock;
use serde::{Serialize, Deserialize};
use crate::core::{AuroraResult, AuroraError, ErrorCode};

/// User identity with authentication information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct User {
    pub id: String,
    pub username: String,
    pub email: String,
    pub roles: HashSet<String>,
    pub is_active: bool,
    pub created_at: u64,
    pub last_login: Option<u64>,
}

/// Role with associated permissions
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Role {
    pub id: String,
    pub name: String,
    pub description: String,
    pub permissions: HashSet<Permission>,
    pub parent_roles: HashSet<String>, // Hierarchical roles
    pub created_at: u64,
}

/// Fine-grained permissions
#[derive(Debug, Clone, Hash, Eq, PartialEq, Serialize, Deserialize)]
pub enum Permission {
    // Database-level permissions
    CreateDatabase,
    DropDatabase,
    AlterDatabase,

    // Schema-level permissions
    CreateSchema(String), // schema name
    DropSchema(String),
    AlterSchema(String),

    // Table-level permissions
    CreateTable(String), // table name
    DropTable(String),
    AlterTable(String),
    SelectTable(String),
    InsertTable(String),
    UpdateTable(String),
    DeleteTable(String),

    // Column-level permissions
    SelectColumn(String, String), // table.column
    InsertColumn(String, String),
    UpdateColumn(String, String),

    // System permissions
    CreateUser,
    DropUser,
    GrantRole,
    RevokeRole,
    ViewAuditLogs,
    ManageSecurityPolicies,

    // Administrative permissions
    SuperUser,
    Backup,
    Restore,
}

/// Permission check result
#[derive(Debug, Clone)]
pub enum PermissionResult {
    Granted,
    Denied(String),
    RequiresMFA,
}

/// RBAC manager
pub struct RBACManager {
    users: RwLock<HashMap<String, User>>,
    roles: RwLock<HashMap<String, Role>>,
    user_sessions: RwLock<HashMap<String, UserSession>>,
}

#[derive(Debug, Clone)]
pub struct UserSession {
    pub user_id: String,
    pub session_id: String,
    pub created_at: u64,
    pub expires_at: u64,
    pub mfa_verified: bool,
}

impl RBACManager {
    /// Create a new RBAC manager with default roles
    pub fn new() -> Self {
        let mut manager = Self {
            users: RwLock::new(HashMap::new()),
            roles: RwLock::new(HashMap::new()),
            user_sessions: RwLock::new(HashMap::new()),
        };

        // Create default roles
        manager.create_default_roles();
        manager
    }

    /// Create default system roles
    fn create_default_roles(&mut self) {
        let admin_role = Role {
            id: "admin".to_string(),
            name: "Administrator".to_string(),
            description: "Full system access".to_string(),
            permissions: vec![
                Permission::SuperUser,
                Permission::CreateUser,
                Permission::DropUser,
                Permission::GrantRole,
                Permission::RevokeRole,
                Permission::ViewAuditLogs,
                Permission::ManageSecurityPolicies,
                Permission::Backup,
                Permission::Restore,
            ].into_iter().collect(),
            parent_roles: HashSet::new(),
            created_at: std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_secs(),
        };

        let user_role = Role {
            id: "user".to_string(),
            name: "Standard User".to_string(),
            description: "Basic database access".to_string(),
            permissions: vec![
                Permission::SelectTable("*".to_string()), // Can select from any table
                Permission::InsertTable("*".to_string()),
                Permission::UpdateTable("*".to_string()),
                Permission::DeleteTable("*".to_string()),
            ].into_iter().collect(),
            parent_roles: HashSet::new(),
            created_at: std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_secs(),
        };

        let readonly_role = Role {
            id: "readonly".to_string(),
            name: "Read Only".to_string(),
            description: "Read-only database access".to_string(),
            permissions: vec![
                Permission::SelectTable("*".to_string()),
            ].into_iter().collect(),
            parent_roles: HashSet::new(),
            created_at: std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_secs(),
        };

        let mut roles = self.roles.write();
        roles.insert(admin_role.id.clone(), admin_role);
        roles.insert(user_role.id.clone(), user_role);
        roles.insert(readonly_role.id.clone(), readonly_role);
    }

    /// Create a new user
    pub fn create_user(&self, username: String, email: String, password_hash: String) -> AuroraResult<User> {
        let user_id = format!("user_{}", username);
        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_secs();

        let user = User {
            id: user_id.clone(),
            username: username.clone(),
            email,
            roles: HashSet::new(),
            is_active: true,
            created_at: now,
            last_login: None,
        };

        let mut users = self.users.write();
        users.insert(user_id, user.clone());

        Ok(user)
    }

    /// Create a new role
    pub fn create_role(&self, name: String, description: String, permissions: HashSet<Permission>) -> AuroraResult<Role> {
        let role_id = format!("role_{}", name.to_lowercase().replace(" ", "_"));
        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_secs();

        let role = Role {
            id: role_id.clone(),
            name,
            description,
            permissions,
            parent_roles: HashSet::new(),
            created_at: now,
        };

        let mut roles = self.roles.write();
        roles.insert(role_id, role.clone());

        Ok(role)
    }

    /// Grant role to user
    pub fn grant_role_to_user(&self, user_id: &str, role_id: &str) -> AuroraResult<()> {
        let mut users = self.users.write();
        if let Some(user) = users.get_mut(user_id) {
            user.roles.insert(role_id.to_string());
            Ok(())
        } else {
            Err(AuroraError::new(
                ErrorCode::Authentication,
                format!("User {} not found", user_id)
            ))
        }
    }

    /// Revoke role from user
    pub fn revoke_role_from_user(&self, user_id: &str, role_id: &str) -> AuroraResult<()> {
        let mut users = self.users.write();
        if let Some(user) = users.get_mut(user_id) {
            user.roles.remove(role_id);
            Ok(())
        } else {
            Err(AuroraError::new(
                ErrorCode::Authentication,
                format!("User {} not found", user_id)
            ))
        }
    }

    /// Check if user has permission
    pub fn check_permission(&self, user_id: &str, permission: &Permission) -> PermissionResult {
        let users = self.users.read();
        let roles = self.roles.read();

        if let Some(user) = users.get(user_id) {
            if !user.is_active {
                return PermissionResult::Denied("User account is inactive".to_string());
            }

            // Check direct permissions from all user's roles
            for role_id in &user.roles {
                if let Some(role) = roles.get(role_id) {
                    // Check direct permissions
                    if role.permissions.contains(permission) {
                        return PermissionResult::Granted;
                    }

                    // Check wildcard permissions
                    if self.check_wildcard_permission(permission, &role.permissions) {
                        return PermissionResult::Granted;
                    }

                    // Check parent role permissions (recursive)
                    if self.check_parent_role_permissions(permission, role, &roles) {
                        return PermissionResult::Granted;
                    }
                }
            }
        }

        PermissionResult::Denied(format!("Permission {:?} denied for user {}", permission, user_id))
    }

    /// Check wildcard permissions
    fn check_wildcard_permission(&self, permission: &Permission, role_permissions: &HashSet<Permission>) -> bool {
        match permission {
            Permission::SelectTable(table) if table != "*" => {
                role_permissions.contains(&Permission::SelectTable("*".to_string()))
            }
            Permission::InsertTable(table) if table != "*" => {
                role_permissions.contains(&Permission::InsertTable("*".to_string()))
            }
            Permission::UpdateTable(table) if table != "*" => {
                role_permissions.contains(&Permission::UpdateTable("*".to_string()))
            }
            Permission::DeleteTable(table) if table != "*" => {
                role_permissions.contains(&Permission::DeleteTable("*".to_string()))
            }
            Permission::SelectColumn(table, _) if table != "*" => {
                role_permissions.contains(&Permission::SelectTable("*".to_string()))
            }
            Permission::InsertColumn(table, _) if table != "*" => {
                role_permissions.contains(&Permission::InsertTable("*".to_string()))
            }
            Permission::UpdateColumn(table, _) if table != "*" => {
                role_permissions.contains(&Permission::UpdateTable("*".to_string()))
            }
            _ => false,
        }
    }

    /// Check parent role permissions recursively
    fn check_parent_role_permissions(&self, permission: &Permission, role: &Role, all_roles: &HashMap<String, Role>) -> bool {
        for parent_role_id in &role.parent_roles {
            if let Some(parent_role) = all_roles.get(parent_role_id) {
                if parent_role.permissions.contains(permission) {
                    return true;
                }
                // Recursive check
                if self.check_parent_role_permissions(permission, parent_role, all_roles) {
                    return true;
                }
            }
        }
        false
    }

    /// Authenticate user and create session
    pub fn authenticate_user(&self, username: &str, password_hash: &str) -> AuroraResult<UserSession> {
        let users = self.users.read();

        // Find user by username
        for user in users.values() {
            if user.username == username && user.is_active {
                // In production, verify password hash here
                // For demo, accept any password

                let now = std::time::SystemTime::now()
                    .duration_since(std::time::UNIX_EPOCH)
                    .unwrap()
                    .as_secs();

                let session = UserSession {
                    user_id: user.id.clone(),
                    session_id: format!("session_{}_{}", user.id, now),
                    created_at: now,
                    expires_at: now + 3600, // 1 hour
                    mfa_verified: false, // Could be enhanced with MFA
                };

                let mut sessions = self.user_sessions.write();
                sessions.insert(session.session_id.clone(), session.clone());

                return Ok(session);
            }
        }

        Err(AuroraError::new(
            ErrorCode::Authentication,
            "Invalid username or password".to_string()
        ))
    }

    /// Validate session
    pub fn validate_session(&self, session_id: &str) -> AuroraResult<String> {
        let sessions = self.user_sessions.read();
        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_secs();

        if let Some(session) = sessions.get(session_id) {
            if session.expires_at > now {
                Ok(session.user_id.clone())
            } else {
                Err(AuroraError::new(
                    ErrorCode::Authentication,
                    "Session expired".to_string()
                ))
            }
        } else {
            Err(AuroraError::new(
                ErrorCode::Authentication,
                "Invalid session".to_string()
            ))
        }
    }

    /// Get user by ID
    pub fn get_user(&self, user_id: &str) -> Option<User> {
        let users = self.users.read();
        users.get(user_id).cloned()
    }

    /// Get role by ID
    pub fn get_role(&self, role_id: &str) -> Option<Role> {
        let roles = self.roles.read();
        roles.get(role_id).cloned()
    }

    /// List all users
    pub fn list_users(&self) -> Vec<User> {
        let users = self.users.read();
        users.values().cloned().collect()
    }

    /// List all roles
    pub fn list_roles(&self) -> Vec<Role> {
        let roles = self.roles.read();
        roles.values().cloned().collect()
    }

    /// Check if user is administrator
    pub fn is_admin(&self, user_id: &str) -> bool {
        matches!(self.check_permission(user_id, &Permission::SuperUser), PermissionResult::Granted)
    }
}
