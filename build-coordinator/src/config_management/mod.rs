//! Configuration Management: UNIQUENESS Production Config
//!
//! Research-backed configuration management for distributed systems:
//! - **Hot Reloading**: Runtime configuration updates without restarts
//! - **Configuration Validation**: Schema validation and semantic checking
//! - **Environment Overrides**: Environment-specific configuration
//! - **Configuration Encryption**: Secure storage of sensitive values
//! - **Configuration Auditing**: Track all configuration changes
//! - **GitOps Integration**: Configuration as code with Git versioning

pub mod hot_reload;
pub mod validation;
pub mod encryption;
pub mod gitops;
pub mod schema_registry;
pub mod config_auditing;

pub use hot_reload::HotReloader;
pub use validation::ConfigValidator;
pub use encryption::ConfigEncryption;
pub use gitops::GitOpsManager;
pub use schema_registry::SchemaRegistry;
pub use config_auditing::ConfigAuditor;

// UNIQUENESS Research Citations:
// - **Configuration as Code**: GitOps principles and practices
// - **Schema Validation**: JSON Schema, OpenAPI specifications
// - **Hot Reloading**: Netflix Archaius, Spring Cloud Config research
