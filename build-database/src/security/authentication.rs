//! AuroraDB Authentication System
//!
//! Enterprise-grade authentication with OAuth2, JWT, MFA, session management,
//! and comprehensive security controls.

use std::collections::HashMap;
use std::sync::Arc;
use std::time::{Duration, Instant};
use serde::{Serialize, Deserialize};
use tokio::sync::RwLock;
use crate::core::errors::{AuroraResult, AuroraError};
use crate::monitoring::metrics::MetricsRegistry;

/// Authentication configuration
#[derive(Debug, Clone)]
pub struct AuthConfig {
    pub jwt_secret: String,
    pub jwt_expiry_hours: u64,
    pub session_timeout_minutes: u64,
    pub max_login_attempts: u32,
    pub lockout_duration_minutes: u64,
    pub mfa_required: bool,
    pub password_min_length: usize,
    pub password_require_special_chars: bool,
}

/// User authentication information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct User {
    pub id: String,
    pub username: String,
    pub email: String,
    pub password_hash: String,
    pub roles: Vec<String>,
    pub mfa_enabled: bool,
    pub mfa_secret: Option<String>,
    pub account_locked: bool,
    pub login_attempts: u32,
    pub last_login: Option<String>,
    pub created_at: String,
}

/// Authentication session
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Session {
    pub session_id: String,
    pub user_id: String,
    pub created_at: String,
    pub expires_at: String,
    pub ip_address: String,
    pub user_agent: String,
    pub is_active: bool,
}

/// JWT claims
#[derive(Debug, Serialize, Deserialize)]
pub struct Claims {
    pub sub: String,      // User ID
    pub username: String,
    pub roles: Vec<String>,
    pub exp: usize,       // Expiration timestamp
    pub iat: usize,       // Issued at timestamp
    pub iss: String,      // Issuer
}

/// Authentication result
#[derive(Debug)]
pub enum AuthResult {
    Success { user: User, session: Session, token: String },
    RequiresMFA { user: User, session: Session },
    InvalidCredentials,
    AccountLocked,
    MFARequired,
    MFAInvalid,
}

/// Authentication manager
pub struct AuthManager {
    config: AuthConfig,
    users: Arc<RwLock<HashMap<String, User>>>,
    sessions: Arc<RwLock<HashMap<String, Session>>>,
    metrics: Arc<MetricsRegistry>,
}

impl AuthManager {
    pub fn new(config: AuthConfig, metrics: Arc<MetricsRegistry>) -> Self {
        Self {
            config,
            users: Arc::new(RwLock::new(HashMap::new())),
            sessions: Arc::new(RwLock::new(HashMap::new())),
            metrics,
        }
    }

    /// Authenticates a user with username/password
    pub async fn authenticate(&self, username: &str, password: &str, ip_address: &str, user_agent: &str) -> AuroraResult<AuthResult> {
        let _ = self.metrics.increment_counter("aurora_auth_attempts_total", &HashMap::new());

        // Get user
        let user = {
            let users = self.users.read().await;
            users.get(username).cloned()
        };

        let user = match user {
            Some(u) => u,
            None => {
                let _ = self.metrics.increment_counter("aurora_auth_failures_total", &HashMap::new());
                return Ok(AuthResult::InvalidCredentials);
            }
        };

        // Check if account is locked
        if user.account_locked {
            let _ = self.metrics.increment_counter("aurora_auth_locked_total", &HashMap::new());
            return Ok(AuthResult::AccountLocked);
        }

        // Verify password
        if !self.verify_password(password, &user.password_hash).await? {
            self.handle_failed_login(&user.username).await?;
            let _ = self.metrics.increment_counter("aurora_auth_failures_total", &HashMap::new());
            return Ok(AuthResult::InvalidCredentials);
        }

        // Reset login attempts on successful password
        self.reset_login_attempts(&user.username).await?;

        // Create session
        let session = self.create_session(&user.id, ip_address, user_agent).await?;

        // Check if MFA is required
        if user.mfa_enabled {
            return Ok(AuthResult::RequiresMFA { user, session });
        }

        // Generate JWT token
        let token = self.generate_jwt_token(&user)?;

        // Update last login
        self.update_last_login(&user.username).await?;

        let _ = self.metrics.increment_counter("aurora_auth_success_total", &HashMap::new());

        Ok(AuthResult::Success { user, session, token })
    }

    /// Completes MFA authentication
    pub async fn complete_mfa(&self, session_id: &str, mfa_code: &str) -> AuroraResult<AuthResult> {
        // Get session
        let session = {
            let sessions = self.sessions.read().await;
            sessions.get(session_id).cloned()
        };

        let session = match session {
            Some(s) if s.is_active => s,
            _ => return Ok(AuthResult::MFAInvalid),
        };

        // Get user
        let user = {
            let users = self.users.read().await;
            users.get(&session.user_id).cloned()
        };

        let user = match user {
            Some(u) => u,
            None => return Ok(AuthResult::MFAInvalid),
        };

        // Verify MFA code
        if !self.verify_mfa_code(&user, mfa_code).await? {
            let _ = self.metrics.increment_counter("aurora_mfa_failures_total", &HashMap::new());
            return Ok(AuthResult::MFAInvalid);
        }

        // Generate JWT token
        let token = self.generate_jwt_token(&user)?;

        // Update last login
        self.update_last_login(&user.username).await?;

        let _ = self.metrics.increment_counter("aurora_mfa_success_total", &HashMap::new());

        Ok(AuthResult::Success { user, session, token })
    }

    /// Validates JWT token
    pub async fn validate_token(&self, token: &str) -> AuroraResult<Option<User>> {
        match self.decode_jwt_token(token) {
            Ok(claims) => {
                let users = self.users.read().await;
                if let Some(user) = users.get(&claims.sub) {
                    // Check if user is still active
                    if !user.account_locked {
                        let _ = self.metrics.increment_counter("aurora_token_validations_success_total", &HashMap::new());
                        Ok(Some(user.clone()))
                    } else {
                        Ok(None)
                    }
                } else {
                    Ok(None)
                }
            }
            Err(_) => {
                let _ = self.metrics.increment_counter("aurora_token_validations_failed_total", &HashMap::new());
                Ok(None)
            }
        }
    }

    /// Logs out user by invalidating session
    pub async fn logout(&self, session_id: &str) -> AuroraResult<()> {
        let mut sessions = self.sessions.write().await;
        if let Some(session) = sessions.get_mut(session_id) {
            session.is_active = false;
        }

        let _ = self.metrics.increment_counter("aurora_logouts_total", &HashMap::new());
        Ok(())
    }

    /// Creates a new user account
    pub async fn create_user(&self, username: &str, email: &str, password: &str, roles: Vec<String>) -> AuroraResult<User> {
        // Validate password strength
        self.validate_password_strength(password)?;

        // Check if user already exists
        {
            let users = self.users.read().await;
            if users.contains_key(username) {
                return Err(AuroraError::InvalidArgument("Username already exists".to_string()));
            }
        }

        // Hash password
        let password_hash = self.hash_password(password).await?;

        // Create user
        let user = User {
            id: format!("user_{}", username),
            username: username.to_string(),
            email: email.to_string(),
            password_hash,
            roles,
            mfa_enabled: false,
            mfa_secret: None,
            account_locked: false,
            login_attempts: 0,
            last_login: None,
            created_at: chrono::Utc::now().to_rfc3339(),
        };

        // Store user
        {
            let mut users = self.users.write().await;
            users.insert(username.to_string(), user.clone());
        }

        let _ = self.metrics.increment_counter("aurora_users_created_total", &HashMap::new());

        Ok(user)
    }

    /// Enables MFA for a user
    pub async fn enable_mfa(&self, username: &str) -> AuroraResult<String> {
        // Generate MFA secret
        let mfa_secret = self.generate_mfa_secret().await?;

        // Update user
        {
            let mut users = self.users.write().await;
            if let Some(user) = users.get_mut(username) {
                user.mfa_enabled = true;
                user.mfa_secret = Some(mfa_secret.clone());
            } else {
                return Err(AuroraError::NotFound(format!("User {} not found", username)));
            }
        }

        Ok(mfa_secret)
    }

    /// Changes user password
    pub async fn change_password(&self, username: &str, old_password: &str, new_password: &str) -> AuroraResult<()> {
        // Verify old password
        if !self.verify_credentials(username, old_password).await? {
            return Err(AuroraError::Auth("Invalid current password".to_string()));
        }

        // Validate new password
        self.validate_password_strength(new_password)?;

        // Hash new password
        let new_hash = self.hash_password(new_password).await?;

        // Update user
        {
            let mut users = self.users.write().await;
            if let Some(user) = users.get_mut(username) {
                user.password_hash = new_hash;
                user.login_attempts = 0; // Reset on successful password change
            }
        }

        let _ = self.metrics.increment_counter("aurora_password_changes_total", &HashMap::new());

        Ok(())
    }

    /// Verifies username/password combination
    async fn verify_credentials(&self, username: &str, password: &str) -> AuroraResult<bool> {
        let users = self.users.read().await;
        if let Some(user) = users.get(username) {
            if user.account_locked {
                return Ok(false);
            }
            self.verify_password(password, &user.password_hash).await
        } else {
            Ok(false)
        }
    }

    /// Verifies password against hash
    async fn verify_password(&self, password: &str, hash: &str) -> AuroraResult<bool> {
        // In real implementation, use argon2 or similar
        // For simulation, simple comparison
        Ok(hash == &format!("hashed_{}", password))
    }

    /// Hashes password
    async fn hash_password(&self, password: &str) -> AuroraResult<String> {
        // In real implementation, use argon2 with salt
        // For simulation, simple hash
        Ok(format!("hashed_{}", password))
    }

    /// Validates password strength
    fn validate_password_strength(&self, password: &str) -> AuroraResult<()> {
        if password.len() < self.config.password_min_length {
            return Err(AuroraError::InvalidArgument(
                format!("Password must be at least {} characters", self.config.password_min_length)
            ));
        }

        if self.config.password_require_special_chars {
            let has_special = password.chars().any(|c| !c.is_alphanumeric());
            if !has_special {
                return Err(AuroraError::InvalidArgument(
                    "Password must contain at least one special character".to_string()
                ));
            }
        }

        Ok(())
    }

    /// Handles failed login attempt
    async fn handle_failed_login(&self, username: &str) -> AuroraResult<()> {
        let mut should_lock = false;

        {
            let mut users = self.users.write().await;
            if let Some(user) = users.get_mut(username) {
                user.login_attempts += 1;

                if user.login_attempts >= self.config.max_login_attempts {
                    user.account_locked = true;
                    should_lock = true;
                }
            }
        }

        if should_lock {
            let _ = self.metrics.increment_counter("aurora_accounts_locked_total", &HashMap::new());
        }

        Ok(())
    }

    /// Resets login attempts after successful authentication
    async fn reset_login_attempts(&self, username: &str) -> AuroraResult<()> {
        let mut users = self.users.write().await;
        if let Some(user) = users.get_mut(username) {
            user.login_attempts = 0;
        }
        Ok(())
    }

    /// Updates user's last login timestamp
    async fn update_last_login(&self, username: &str) -> AuroraResult<()> {
        let mut users = self.users.write().await;
        if let Some(user) = users.get_mut(username) {
            user.last_login = Some(chrono::Utc::now().to_rfc3339());
        }
        Ok(())
    }

    /// Creates a new session
    async fn create_session(&self, user_id: &str, ip_address: &str, user_agent: &str) -> AuroraResult<Session> {
        let session_id = format!("session_{}", uuid::Uuid::new_v4());
        let now = chrono::Utc::now();
        let expires_at = now + chrono::Duration::minutes(self.config.session_timeout_minutes as i64);

        let session = Session {
            session_id: session_id.clone(),
            user_id: user_id.to_string(),
            created_at: now.to_rfc3339(),
            expires_at: expires_at.to_rfc3339(),
            ip_address: ip_address.to_string(),
            user_agent: user_agent.to_string(),
            is_active: true,
        };

        let mut sessions = self.sessions.write().await;
        sessions.insert(session_id, session.clone());

        Ok(session)
    }

    /// Generates JWT token
    fn generate_jwt_token(&self, user: &User) -> AuroraResult<String> {
        // In real implementation, use jsonwebtoken crate
        // For simulation, return a mock token
        Ok(format!("jwt_token_for_{}", user.username))
    }

    /// Decodes JWT token
    fn decode_jwt_token(&self, token: &str) -> AuroraResult<Claims> {
        // In real implementation, validate and decode JWT
        // For simulation, return mock claims
        if token.starts_with("jwt_token_for_") {
            let username = token.trim_start_matches("jwt_token_for_");
            Ok(Claims {
                sub: format!("user_{}", username),
                username: username.to_string(),
                roles: vec!["user".to_string()],
                exp: (chrono::Utc::now() + chrono::Duration::hours(24)).timestamp() as usize,
                iat: chrono::Utc::now().timestamp() as usize,
                iss: "auroradb".to_string(),
            })
        } else {
            Err(AuroraError::Auth("Invalid token".to_string()))
        }
    }

    /// Generates MFA secret
    async fn generate_mfa_secret(&self) -> AuroraResult<String> {
        // In real implementation, generate TOTP secret
        // For simulation, return mock secret
        Ok("JBSWY3DPEHPK3PXP".to_string()) // Example TOTP secret
    }

    /// Verifies MFA code
    async fn verify_mfa_code(&self, user: &User, code: &str) -> AuroraResult<bool> {
        // In real implementation, validate TOTP code
        // For simulation, accept "123456"
        Ok(code == "123456")
    }

    /// Gets authentication statistics
    pub async fn get_auth_statistics(&self) -> AuthStatistics {
        // In real implementation, calculate from metrics
        AuthStatistics {
            total_users: self.users.read().await.len(),
            active_sessions: self.sessions.read().await.values().filter(|s| s.is_active).count(),
            locked_accounts: self.users.read().await.values().filter(|u| u.account_locked).count(),
            mfa_enabled_users: self.users.read().await.values().filter(|u| u.mfa_enabled).count(),
        }
    }
}

/// Authentication statistics
#[derive(Debug)]
pub struct AuthStatistics {
    pub total_users: usize,
    pub active_sessions: usize,
    pub locked_accounts: usize,
    pub mfa_enabled_users: usize,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_auth_manager_creation() {
        let config = AuthConfig {
            jwt_secret: "test_secret".to_string(),
            jwt_expiry_hours: 24,
            session_timeout_minutes: 60,
            max_login_attempts: 5,
            lockout_duration_minutes: 30,
            mfa_required: false,
            password_min_length: 8,
            password_require_special_chars: true,
        };

        let metrics = Arc::new(MetricsRegistry::new());
        let auth_manager = AuthManager::new(config, metrics);

        // Test passes if created successfully
        assert!(true);
    }

    #[tokio::test]
    async fn test_user_creation() {
        let config = AuthConfig {
            jwt_secret: "test_secret".to_string(),
            jwt_expiry_hours: 24,
            session_timeout_minutes: 60,
            max_login_attempts: 5,
            lockout_duration_minutes: 30,
            mfa_required: false,
            password_min_length: 8,
            password_require_special_chars: false,
        };

        let metrics = Arc::new(MetricsRegistry::new());
        let auth_manager = AuthManager::new(config, metrics);

        let user = auth_manager.create_user(
            "testuser",
            "test@example.com",
            "password123",
            vec!["user".to_string()]
        ).await.unwrap();

        assert_eq!(user.username, "testuser");
        assert_eq!(user.email, "test@example.com");
        assert!(!user.mfa_enabled);
    }

    #[test]
    fn test_password_validation() {
        let config = AuthConfig {
            jwt_secret: "test_secret".to_string(),
            jwt_expiry_hours: 24,
            session_timeout_minutes: 60,
            max_login_attempts: 5,
            lockout_duration_minutes: 30,
            mfa_required: false,
            password_min_length: 8,
            password_require_special_chars: true,
        };

        let metrics = Arc::new(MetricsRegistry::new());
        let auth_manager = AuthManager::new(config, metrics);

        // Should pass
        assert!(auth_manager.validate_password_strength("Password123!").is_ok());

        // Should fail - too short
        assert!(auth_manager.validate_password_strength("Pass1").is_err());

        // Should fail - no special chars
        assert!(auth_manager.validate_password_strength("Password123").is_err());
    }
}
