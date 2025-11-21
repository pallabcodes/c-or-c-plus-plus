//! Compliance & Governance: UNIQUENESS Enterprise Compliance
//!
//! Research-backed compliance frameworks for regulated industries:
//! - **GDPR Compliance**: Data protection and privacy regulations
//! - **SOC 2**: Security, availability, and confidentiality controls
//! - **HIPAA**: Healthcare data protection and audit trails
//! - **PCI DSS**: Payment card industry data security standards
//! - **Zero Trust**: Identity verification and least privilege access
//! - **Audit Automation**: Automated compliance reporting and monitoring

pub mod gdpr_compliance;
pub mod soc2_controls;
pub mod hipaa_compliance;
pub mod pci_dss;
pub mod zero_trust;
pub mod audit_automation;

pub use gdpr_compliance::GDPRManager;
pub use soc2_controls::SOC2Controller;
pub use hipaa_compliance::HIPAACompliance;
pub use pci_dss::PCIDSSCompliance;
pub use zero_trust::ZeroTrustManager;
pub use audit_automation::AuditAutomator;

// UNIQUENESS Research Citations:
// - **GDPR**: EU General Data Protection Regulation research
// - **Zero Trust**: Google BeyondCorp, Forrester Zero Trust research
// - **Compliance Automation**: Security compliance research papers
