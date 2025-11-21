//! Security & Cryptography: UNIQUENESS Secure Coordination
//!
//! Research-backed security for distributed coordination:
//! - **Cryptographic Consensus**: Secure consensus with digital signatures
//! - **TLS 1.3**: Perfect forward secrecy for all communications
//! - **Mutual Authentication**: Certificate-based node authentication
//! - **Audit Logging**: Cryptographically verifiable operation logs
//! - **Key Management**: Automated key rotation and secure storage

pub mod crypto_consensus;
pub mod tls_transport;
pub mod certificate_authority;
pub mod audit_logging;
pub mod key_management;
pub mod secure_communication;

pub use crypto_consensus::{CryptoConsensus, SignedLogEntry, VRFLeaderElection};
pub use tls_transport::{TLSTransport, TLSConnectionStats};
pub use certificate_authority::{CertificateAuthority, CertificateRequest};
pub use audit_logging::{AuditLogger, AuditEntry, AuditEventType};
pub use key_management::{KeyManager, KeySet};
pub use secure_communication::{SecureChannel, EncryptedMessage, ChannelStats};

// UNIQUENESS Research Citations:
// - **Ed25519**: Bernstein et al. (2011) - High-performance digital signatures
// - **TLS 1.3**: RFC 8446 - Perfect forward secrecy and modern cryptography
// - **VRF**: Micali et al. (1999) - Verifiable random functions
// - **Certificate Transparency**: Laurie et al. (2014) - Public certificate auditing
// - **Merkle Trees**: Merkle (1988) - Cryptographic integrity verification
// - **ECDH**: ANSI X9.62 - Elliptic curve Diffie-Hellman key exchange
