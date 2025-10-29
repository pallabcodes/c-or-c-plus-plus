# Security and Compliance Standards

## Scope
Applies to all security and compliance code including encryption, access control, and audit logging. Extends repository root rules.

## Encryption

### Data at Rest Encryption
* Transparent data encryption (TDE)
* Page level encryption
* Key rotation support
* Hardware accelerated encryption (AES NI)

### Data in Transit Encryption
* TLS/SSL for connections
* Certificate management
* Perfect forward secrecy
* Cipher suite selection

### Encryption Algorithms
* AES 256 for symmetric encryption
* RSA or ECC for asymmetric encryption
* Key derivation functions (PBKDF2, Argon2)
* Secure random number generation

## Access Control

### Authentication
* Password based authentication
* Certificate based authentication
* Token based authentication
* JWT integration
* LDAP/Active Directory integration

### Authorization
* Role based access control (RBAC)
* Attribute based access control (ABAC)
* Row level security
* Column level security
* Granular permissions

### Privilege Management
* Grant and revoke operations
* Permission inheritance
* Privilege escalation prevention
* Least privilege principle

## Audit Logging

### Audit Trail
* Log all security relevant events
* Login and logout events
* Data access events
* Schema change events
* Administrative actions

### Log Integrity
* Tamper proof logging
* Cryptographic signatures
* Log rotation and archival
* Compliance requirements

## Compliance

### Standards
* SOC 2 Type 2
* HIPAA compliance
* PCI DSS compliance
* GDPR compliance
* Data residency requirements

### Data Protection
* Data classification
* Data masking
* Data anonymization
* Right to deletion
* Data retention policies

## Implementation Requirements
* Secure key management
* Proper secret handling
* Input validation
* SQL injection prevention
* XSS prevention
* CSRF protection

## Security Best Practices
* Defense in depth
* Principle of least privilege
* Security by design
* Regular security audits
* Penetration testing
* Security monitoring

