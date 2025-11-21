//! AuroraDB Production Security Suite
//!
//! Enterprise-grade security with RBAC, encryption, and comprehensive audit logging.
//! UNIQUENESS: Combines research-backed security approaches with production requirements.
//!
//! Features:
//! - Role-Based Access Control (RBAC) with fine-grained permissions
//! - Data encryption at rest and in transit
//! - Comprehensive audit logging for compliance
//! - Multi-factor authentication support
//! - Security policy enforcement
//! - Threat detection and anomaly monitoring

pub mod rbac;
pub mod encryption;
pub mod audit;
pub mod authentication;
pub mod authorization;
pub mod policy;

pub use rbac::*;
pub use encryption::*;
pub use audit::*;
pub use authentication::*;
pub use authorization::*;
pub use policy::*;