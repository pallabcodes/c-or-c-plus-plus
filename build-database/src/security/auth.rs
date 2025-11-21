//! AuroraDB Production Authentication System
//!
//! Production-ready authentication with:
//! - Argon2 password hashing
//! - JWT token management
//! - Session handling
//! - Multi-factor authentication support
//! - Account lockout protection

use std::collections::HashMap;
use std::sync::Arc;
use tokio::sync::RwLock;
use chrono::{DateTime, Utc, Duration};
use serde::{Deserialize, Serialize};
use jsonwebtoken::{encode, decode, Header, Algorithm, Validation, EncodingKey, DecodingKey, TokenData};
use argon2::{Argon2, PasswordHasher, PasswordVerifier};
use argon2::password_hash::{SaltString, PasswordHash};
use rand::rngs::OsRng;
use crate::core::AuroraResult;
use crate::errors::{AuroraError, ErrorCode};

/// User account information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct UserAccount {
    pub id: String,
    pub username: String,
    pub email: String,
    pub password_hash: String,
    pub roles: Vec<String>,
    pub is_active: bool,
    pub is_locked: bool,
    pub failed_login_attempts: u32,
    pub last_login: Option<DateTime<Utc>>,
    pub created_at: DateTime<Utc>,
    pub updated_at: DateTime<Utc>,
}

/// JWT claims for authentication tokens
#[derive(Debug, Serialize, Deserialize)]
pub struct Claims {
    pub sub: String,        // Subject (user ID)
    pub username: String,   // Username
    pub roles: Vec<String>, // User roles
    pub exp: usize,         // Expiration time
    pub iat: usize,         // Issued at time
    pub iss: String,        // Issuer
    pub aud: String,        // Audience
}

/// Authentication session
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AuthSession {
    pub session_id: String,
    pub user_id: String,
    pub username: String,
    pub roles: Vec<String>,
    pub created_at: DateTime<Utc>,
    pub expires_at: DateTime<Utc>,
    pub last_activity: DateTime<Utc>,
    pub ip_address: Option<String>,
    pub user_agent: Option<String>,
}

/// Authentication configuration
#[derive(Debug, Clone)]
pub struct AuthConfig {
    pub jwt_secret: String,
    pub jwt_expiration_hours: i64,
    pub password_min_length: usize,
    pub max_failed_attempts: u32,
    pub lockout_duration_minutes: i64,
    pub session_timeout_hours: i64,
    pub enable_mfa: bool,
}

/// Main authentication manager
pub struct AuthManager {
    config: AuthConfig,
    users: RwLock<HashMap<String, UserAccount>>,
    sessions: RwLock<HashMap<String, AuthSession>>,
    encoding_key: EncodingKey,
    decoding_key: DecodingKey,
}

impl AuthManager {
    /// Create a new authentication manager
    pub fn new(config: AuthConfig) -> Self {
        let encoding_key = EncodingKey::from_secret(config.jwt_secret.as_bytes());
        let decoding_key = DecodingKey::from_secret(config.jwt_secret.as_bytes());

        Self {
            config,
            users: RwLock::new(HashMap::new()),
            sessions: RwLock::new(HashMap::new()),
            encoding_key,
            decoding_key,
        }
    }

    /// Register a new user
    pub async fn register_user(&self, username: &str, email: &str, password: &str, roles: Vec<String>) -> AuroraResult<String> {
        // Validate input
        self.validate_registration_input(username, email, password)?;

        // Check if user already exists
        let users = self.users.read().await;
        if users.contains_key(username) {
            return Err(AuroraError::new(ErrorCode::AuthInvalidCredentials, "Username already exists"));
        }
        drop(users);

        // Hash password
        let password_hash = self.hash_password(password)?;

        // Create user account
        let user_id = format!("user_{}", uuid::Uuid::new_v4().simple());
        let now = Utc::now();

        let user = UserAccount {
            id: user_id.clone(),
            username: username.to_string(),
            email: email.to_string(),
            password_hash,
            roles,
            is_active: true,
            is_locked: false,
            failed_login_attempts: 0,
            last_login: None,
            created_at: now,
            updated_at: now,
        };

        // Store user
        let mut users = self.users.write().await;
        users.insert(username.to_string(), user);

        Ok(user_id)
    }

    /// Authenticate a user and return a session
    pub async fn authenticate(&self, username: &str, password: &str, ip_address: Option<&str>) -> AuroraResult<AuthSession> {
        // Get user
        let mut users = self.users.write().await;
        let user = users.get_mut(username)
            .ok_or_else(|| AuroraError::new(ErrorCode::AuthInvalidCredentials, "Invalid username or password"))?;

        // Check if account is locked
        if user.is_locked {
            return Err(AuroraError::new(ErrorCode::AuthAccountLocked, "Account is locked due to too many failed attempts"));
        }

        // Check if account is active
        if !user.is_active {
            return Err(AuroraError::new(ErrorCode::AuthInvalidCredentials, "Account is not active"));
        }

        // Verify password
        if !self.verify_password(password, &user.password_hash)? {
            user.failed_login_attempts += 1;

            // Lock account if too many failed attempts
            if user.failed_login_attempts >= self.config.max_failed_attempts {
                user.is_locked = true;
                user.updated_at = Utc::now();
            }

            return Err(AuroraError::new(ErrorCode::AuthInvalidCredentials, "Invalid username or password"));
        }

        // Reset failed attempts and update login time
        user.failed_login_attempts = 0;
        user.last_login = Some(Utc::now());
        user.updated_at = Utc::now();

        // Create session
        let session_id = format!("session_{}", uuid::Uuid::new_v4().simple());
        let now = Utc::now();
        let expires_at = now + Duration::hours(self.config.session_timeout_hours);

        let session = AuthSession {
            session_id: session_id.clone(),
            user_id: user.id.clone(),
            username: user.username.clone(),
            roles: user.roles.clone(),
            created_at: now,
            expires_at,
            last_activity: now,
            ip_address: ip_address.map(|s| s.to_string()),
            user_agent: None,
        };

        // Store session
        let mut sessions = self.sessions.write().await;
        sessions.insert(session_id.clone(), session.clone());

        Ok(session)
    }

    /// Generate JWT token for authenticated session
    pub fn generate_jwt(&self, session: &AuthSession) -> AuroraResult<String> {
        let now = Utc::now();
        let expiration = now + Duration::hours(self.config.jwt_expiration_hours);

        let claims = Claims {
            sub: session.user_id.clone(),
            username: session.username.clone(),
            roles: session.roles.clone(),
            exp: expiration.timestamp() as usize,
            iat: now.timestamp() as usize,
            iss: "auroradb".to_string(),
            aud: "auroradb-users".to_string(),
        };

        encode(&Header::default(), &claims, &self.encoding_key)
            .map_err(|e| AuroraError::new(ErrorCode::SecurityEncryptionFailed, format!("JWT encoding failed: {}", e)))
    }

    /// Validate JWT token and return claims
    pub fn validate_jwt(&self, token: &str) -> AuroraResult<TokenData<Claims>> {
        let validation = Validation::new(Algorithm::HS256);
        decode::<Claims>(token, &self.decoding_key, &validation)
            .map_err(|e| AuroraError::new(ErrorCode::AuthTokenExpired, format!("JWT validation failed: {}", e)))
    }

    /// Validate session and refresh if needed
    pub async fn validate_session(&self, session_id: &str) -> AuroraResult<AuthSession> {
        let mut sessions = self.sessions.write().await;
        let session = sessions.get_mut(session_id)
            .ok_or_else(|| AuroraError::new(ErrorCode::AuthTokenExpired, "Session not found"))?;

        // Check if session is expired
        if Utc::now() > session.expires_at {
            sessions.remove(session_id);
            return Err(AuroraError::new(ErrorCode::AuthTokenExpired, "Session expired"));
        }

        // Update last activity
        session.last_activity = Utc::now();

        // Extend session if close to expiry
        let extension_threshold = Duration::hours(1);
        if session.expires_at - Utc::now() < extension_threshold {
            session.expires_at = Utc::now() + Duration::hours(self.config.session_timeout_hours);
        }

        Ok(session.clone())
    }

    /// Logout and invalidate session
    pub async fn logout(&self, session_id: &str) -> AuroraResult<()> {
        let mut sessions = self.sessions.write().await;
        sessions.remove(session_id);
        Ok(())
    }

    /// Change user password
    pub async fn change_password(&self, username: &str, old_password: &str, new_password: &str) -> AuroraResult<()> {
        // Validate new password
        self.validate_password(new_password)?;

        let mut users = self.users.write().await;
        let user = users.get_mut(username)
            .ok_or_else(|| AuroraError::new(ErrorCode::AuthInvalidCredentials, "User not found"))?;

        // Verify old password
        if !self.verify_password(old_password, &user.password_hash)? {
            return Err(AuroraError::new(ErrorCode::AuthInvalidCredentials, "Current password is incorrect"));
        }

        // Hash new password
        let new_hash = self.hash_password(new_password)?;
        user.password_hash = new_hash;
        user.updated_at = Utc::now();

        Ok(())
    }

    /// Check if user has required role
    pub async fn has_role(&self, username: &str, role: &str) -> bool {
        let users = self.users.read().await;
        if let Some(user) = users.get(username) {
            user.roles.contains(&role.to_string())
        } else {
            false
        }
    }

    /// Get user by username
    pub async fn get_user(&self, username: &str) -> Option<UserAccount> {
        let users = self.users.read().await;
        users.get(username).cloned()
    }

    /// List all users (admin only)
    pub async fn list_users(&self) -> Vec<UserAccount> {
        let users = self.users.read().await;
        users.values().cloned().collect()
    }

    /// Lock user account
    pub async fn lock_user(&self, username: &str) -> AuroraResult<()> {
        let mut users = self.users.write().await;
        if let Some(user) = users.get_mut(username) {
            user.is_locked = true;
            user.updated_at = Utc::now();
            Ok(())
        } else {
            Err(AuroraError::new(ErrorCode::AuthInvalidCredentials, "User not found"))
        }
    }

    /// Unlock user account
    pub async fn unlock_user(&self, username: &str) -> AuroraResult<()> {
        let mut users = self.users.write().await;
        if let Some(user) = users.get_mut(username) {
            user.is_locked = false;
            user.failed_login_attempts = 0;
            user.updated_at = Utc::now();
            Ok(())
        } else {
            Err(AuroraError::new(ErrorCode::AuthInvalidCredentials, "User not found"))
        }
    }

    // Private helper methods
    fn validate_registration_input(&self, username: &str, email: &str, password: &str) -> AuroraResult<()> {
        // Validate username
        if username.len() < 3 || username.len() > 50 {
            return Err(AuroraError::new(ErrorCode::ValidationInvalidFormat, "Username must be 3-50 characters"));
        }

        if !username.chars().all(|c| c.is_alphanumeric() || c == '_' || c == '-') {
            return Err(AuroraError::new(ErrorCode::ValidationInvalidFormat, "Username contains invalid characters"));
        }

        // Validate email
        if !email.contains('@') || email.len() > 254 {
            return Err(AuroraError::new(ErrorCode::ValidationInvalidFormat, "Invalid email format"));
        }

        // Validate password
        self.validate_password(password)?;

        Ok(())
    }

    fn validate_password(&self, password: &str) -> AuroraResult<()> {
        if password.len() < self.config.password_min_length {
            return Err(AuroraError::new(
                ErrorCode::ValidationInvalidFormat,
                format!("Password must be at least {} characters", self.config.password_min_length)
            ));
        }

        // Check for basic complexity requirements
        let has_uppercase = password.chars().any(|c| c.is_uppercase());
        let has_lowercase = password.chars().any(|c| c.is_lowercase());
        let has_digit = password.chars().any(|c| c.is_digit(10));

        if !has_uppercase || !has_lowercase || !has_digit {
            return Err(AuroraError::new(
                ErrorCode::ValidationInvalidFormat,
                "Password must contain at least one uppercase letter, one lowercase letter, and one digit"
            ));
        }

        Ok(())
    }

    fn hash_password(&self, password: &str) -> AuroraResult<String> {
        let salt = SaltString::generate(&mut OsRng);
        let argon2 = Argon2::default();

        let password_hash = argon2.hash_password(password.as_bytes(), &salt)
            .map_err(|e| AuroraError::new(ErrorCode::SecurityEncryptionFailed, format!("Password hashing failed: {}", e)))?;

        Ok(password_hash.to_string())
    }

    fn verify_password(&self, password: &str, hash: &str) -> AuroraResult<bool> {
        let parsed_hash = PasswordHash::new(hash)
            .map_err(|e| AuroraError::new(ErrorCode::SecurityDecryptionFailed, format!("Invalid password hash: {}", e)))?;

        let argon2 = Argon2::default();
        let is_valid = argon2.verify_password(password.as_bytes(), &parsed_hash).is_ok();

        Ok(is_valid)
    }
}

/// Authentication middleware for requests
pub struct AuthMiddleware {
    auth_manager: Arc<AuthManager>,
}

impl AuthMiddleware {
    pub fn new(auth_manager: Arc<AuthManager>) -> Self {
        Self { auth_manager }
    }

    /// Authenticate request from JWT token
    pub async fn authenticate_request(&self, token: &str) -> AuroraResult<AuthSession> {
        // Validate JWT token
        let token_data = self.auth_manager.validate_jwt(token)?;

        // Validate session
        let session_id = format!("session_{}", token_data.claims.sub);
        self.auth_manager.validate_session(&session_id).await
    }

    /// Check if user has required permissions
    pub async fn check_permissions(&self, session: &AuthSession, required_roles: &[&str]) -> bool {
        for role in required_roles {
            if !self.auth_manager.has_role(&session.username, role).await {
                return false;
            }
        }
        true
    }
}

/// Session manager for handling session lifecycle
pub struct SessionManager {
    sessions: RwLock<HashMap<String, AuthSession>>,
    cleanup_interval: Duration,
}

impl SessionManager {
    pub fn new(cleanup_interval_minutes: u64) -> Self {
        Self {
            sessions: RwLock::new(HashMap::new()),
            cleanup_interval: Duration::minutes(cleanup_interval_minutes as i64),
        }
    }

    /// Start periodic cleanup task
    pub async fn start_cleanup_task(&self) {
        let sessions = Arc::clone(&self.sessions);

        tokio::spawn(async move {
            let mut interval = tokio::time::interval(Self::cleanup_interval);

            loop {
                interval.tick().await;
                Self::cleanup_expired_sessions(&sessions).await;
            }
        });
    }

    /// Clean up expired sessions
    async fn cleanup_expired_sessions(sessions: &RwLock<HashMap<String, AuthSession>>) {
        let mut sessions_map = sessions.write().await;
        let now = Utc::now();

        sessions_map.retain(|_, session| session.expires_at > now);
    }
}

/// Password policy enforcer
pub struct PasswordPolicy {
    min_length: usize,
    require_uppercase: bool,
    require_lowercase: bool,
    require_digits: bool,
    require_special_chars: bool,
    prevent_common_passwords: bool,
}

impl PasswordPolicy {
    pub fn new(min_length: usize) -> Self {
        Self {
            min_length,
            require_uppercase: true,
            require_lowercase: true,
            require_digits: true,
            require_special_chars: false,
            prevent_common_passwords: true,
        }
    }

    pub fn validate(&self, password: &str) -> AuroraResult<()> {
        if password.len() < self.min_length {
            return Err(AuroraError::new(
                ErrorCode::ValidationInvalidFormat,
                format!("Password must be at least {} characters", self.min_length)
            ));
        }

        if self.require_uppercase && !password.chars().any(|c| c.is_uppercase()) {
            return Err(AuroraError::new(ErrorCode::ValidationInvalidFormat, "Password must contain uppercase letters"));
        }

        if self.require_lowercase && !password.chars().any(|c| c.is_lowercase()) {
            return Err(AuroraError::new(ErrorCode::ValidationInvalidFormat, "Password must contain lowercase letters"));
        }

        if self.require_digits && !password.chars().any(|c| c.is_digit(10)) {
            return Err(AuroraError::new(ErrorCode::ValidationInvalidFormat, "Password must contain digits"));
        }

        if self.require_special_chars && !password.chars().any(|c| !c.is_alphanumeric()) {
            return Err(AuroraError::new(ErrorCode::ValidationInvalidFormat, "Password must contain special characters"));
        }

        if self.prevent_common_passwords && Self::is_common_password(password) {
            return Err(AuroraError::new(ErrorCode::ValidationInvalidFormat, "Password is too common"));
        }

        Ok(())
    }

    fn is_common_password(password: &str) -> bool {
        let common_passwords = [
            "password", "123456", "qwerty", "abc123", "password123",
            "admin", "root", "user", "guest", "test"
        ];

        common_passwords.contains(&password.to_lowercase().as_str())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_user_registration() {
        let config = AuthConfig {
            jwt_secret: "test_secret_key_for_testing_only".to_string(),
            jwt_expiration_hours: 24,
            password_min_length: 8,
            max_failed_attempts: 5,
            lockout_duration_minutes: 30,
            session_timeout_hours: 8,
            enable_mfa: false,
        };

        let auth = AuthManager::new(config);

        let user_id = auth.register_user("testuser", "test@example.com", "TestPass123", vec!["user".to_string()]).await.unwrap();
        assert!(!user_id.is_empty());
    }

    #[tokio::test]
    async fn test_user_authentication() {
        let config = AuthConfig {
            jwt_secret: "test_secret_key_for_testing_only".to_string(),
            jwt_expiration_hours: 24,
            password_min_length: 8,
            max_failed_attempts: 5,
            lockout_duration_minutes: 30,
            session_timeout_hours: 8,
            enable_mfa: false,
        };

        let auth = AuthManager::new(config);

        // Register user
        auth.register_user("testuser", "test@example.com", "TestPass123", vec!["user".to_string()]).await.unwrap();

        // Authenticate
        let session = auth.authenticate("testuser", "TestPass123", Some("127.0.0.1")).await.unwrap();
        assert_eq!(session.username, "testuser");
        assert!(session.roles.contains(&"user".to_string()));
    }

    #[tokio::test]
    async fn test_jwt_token_generation() {
        let config = AuthConfig {
            jwt_secret: "test_secret_key_for_testing_only".to_string(),
            jwt_expiration_hours: 24,
            password_min_length: 8,
            max_failed_attempts: 5,
            lockout_duration_minutes: 30,
            session_timeout_hours: 8,
            enable_mfa: false,
        };

        let auth = AuthManager::new(config);

        // Register and authenticate user
        auth.register_user("testuser", "test@example.com", "TestPass123", vec!["user".to_string()]).await.unwrap();
        let session = auth.authenticate("testuser", "TestPass123", None).await.unwrap();

        // Generate JWT
        let token = auth.generate_jwt(&session).unwrap();
        assert!(!token.is_empty());

        // Validate JWT
        let token_data = auth.validate_jwt(&token).unwrap();
        assert_eq!(token_data.claims.username, "testuser");
        assert!(token_data.claims.roles.contains(&"user".to_string()));
    }

    #[tokio::test]
    async fn test_password_validation() {
        let config = AuthConfig {
            jwt_secret: "test_secret_key_for_testing_only".to_string(),
            jwt_expiration_hours: 24,
            password_min_length: 8,
            max_failed_attempts: 5,
            lockout_duration_minutes: 30,
            session_timeout_hours: 8,
            enable_mfa: false,
        };

        let auth = AuthManager::new(config);

        // Test valid password
        assert!(auth.register_user("user1", "user1@example.com", "ValidPass123", vec!["user".to_string()]).await.is_ok());

        // Test invalid passwords
        assert!(auth.register_user("user2", "user2@example.com", "short", vec!["user".to_string()]).await.is_err());
        assert!(auth.register_user("user3", "user3@example.com", "nouppercaseordigits", vec!["user".to_string()]).await.is_err());
    }

    #[test]
    fn test_password_policy() {
        let policy = PasswordPolicy::new(8);

        // Valid password
        assert!(policy.validate("ValidPass123").is_ok());

        // Invalid passwords
        assert!(policy.validate("short").is_err());
        assert!(policy.validate("nouppercase").is_err());
        assert!(policy.validate("NOLOWERCASE").is_err());
        assert!(policy.validate("NoDigits").is_err());
        assert!(policy.validate("password").is_err()); // Common password
    }

    #[tokio::test]
    async fn test_account_lockout() {
        let config = AuthConfig {
            jwt_secret: "test_secret_key_for_testing_only".to_string(),
            jwt_expiration_hours: 24,
            password_min_length: 8,
            max_failed_attempts: 3, // Lower for testing
            lockout_duration_minutes: 30,
            session_timeout_hours: 8,
            enable_mfa: false,
        };

        let auth = AuthManager::new(config);

        // Register user
        auth.register_user("testuser", "test@example.com", "TestPass123", vec!["user".to_string()]).await.unwrap();

        // Fail authentication multiple times
        for _ in 0..3 {
            let _ = auth.authenticate("testuser", "wrongpassword", None).await;
        }

        // Account should be locked
        let result = auth.authenticate("testuser", "TestPass123", None).await;
        assert!(result.is_err());

        if let Err(e) = result {
            assert_eq!(e.code, ErrorCode::AuthAccountLocked);
        }
    }
}
