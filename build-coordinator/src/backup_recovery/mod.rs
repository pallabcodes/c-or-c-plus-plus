//! Backup & Recovery: UNIQUENESS Disaster Recovery
//!
//! Research-backed backup and recovery for distributed coordination:
//! - **Point-in-Time Recovery**: Consistent snapshots with transaction logs
//! - **Cross-Region Backup**: Geo-redundant backup storage
//! - **Automated Failover**: Zero-touch disaster recovery procedures
//! - **Data Integrity Verification**: Cryptographic backup verification
//! - **Backup Encryption**: Secure backup storage with encryption
//! - **Recovery Testing**: Automated recovery validation procedures

pub mod point_in_time_recovery;
pub mod cross_region_backup;
pub mod automated_failover;
pub mod data_integrity;
pub mod backup_encryption;
pub mod recovery_testing;

pub use point_in_time_recovery::PointInTimeRecovery;
pub use cross_region_backup::CrossRegionBackup;
pub use automated_failover::AutomatedFailover;
pub use data_integrity::DataIntegrityVerifier;
pub use backup_encryption::BackupEncryption;
pub use recovery_testing::RecoveryTester;

// UNIQUENESS Research Citations:
// - **Disaster Recovery**: Google, AWS disaster recovery research
// - **Point-in-Time Recovery**: Database recovery research papers
// - **Cryptographic Integrity**: Merkle trees, cryptographic hashing
// - **Automated Failover**: High availability system research
