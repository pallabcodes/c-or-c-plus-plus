# Security and Compliance

## Transport Security
* TLS 1.2+ with modern ciphers; mTLS optional for client auth
* Secure defaults; certificate validation; OCSP stapling where possible

## Authentication
* Username/password, SCRAM, JWT/OIDC, x509 subject mapping
* Rate limiting and ban policies on auth failures

## Authorization
* ACL DSL on topic filters and QoS caps; deny by default
* Per-tenant namespaces and isolation

## Hardening
* Strict parsing; length and range checks; fail closed
* Resource limits per client and tenant (conns, inflight, msg/s, bytes/s)

## Secrets Management
* Do not embed secrets in code; support file/env/secret stores

## Compliance
* Logging and audit trails for admin operations
* Data protection guidance for retained payloads and PII
