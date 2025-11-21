# Security Pattern Recognition Guide

## 🔐 **Decision Tree for Security Pattern Selection**

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    SECURITY PATTERN DECISION TREE                        │
│                 "Choose Your Security Defense Strategy"                 │
└─────────────────────────────────────────────────────────────────────────┘

1. What is your security requirement?
   ├─── Authentication ─────────────────────► User identity verification
   ├─── Authorization ──────────────────────► Access control and permissions
   ├─── Confidentiality ────────────────────► Data encryption and privacy
   ├─── Integrity ──────────────────────────► Data tamper detection
   ├─── Non-repudiation ────────────────────► Action accountability
   ├─── Availability ───────────────────────► DDoS protection, rate limiting
   └─── Auditability ───────────────────────► Security event logging

2. What is your threat model?
   ├─── Network Attacks ────────────────────► Man-in-the-middle, eavesdropping
   ├─── Application Attacks ───────────────► Injection, XSS, CSRF, broken auth
   ├─── Data Attacks ───────────────────────► Data leakage, tampering, poisoning
   ├─── Infrastructure Attacks ────────────► DDoS, privilege escalation
   ├─── Insider Threats ───────────────────► Unauthorized access, sabotage
   ├─── Supply Chain Attacks ──────────────► Third-party compromise
   └─── Advanced Persistent Threats ───────► Long-term infiltration

3. What is your trust model?
   ├─── Zero Trust ─────────────────────────► Never trust, always verify
   ├─── Perimeter Security ────────────────► Trust internal network
   ├─── Federated Trust ───────────────────► Trust external identity providers
   ├─── Decentralized Trust ───────────────► Blockchain, distributed ledger
   ├─── Hierarchical Trust ────────────────► Certificate authorities, PKI
   └─── Peer-to-Peer Trust ────────────────► Mutual authentication

4. What is your deployment model?
   ├─── Web Applications ──────────────────► OWASP Top 10, session management
   ├─── APIs/Microservices ────────────────► JWT, OAuth2, API gateways
   ├─── Mobile Applications ───────────────► Certificate pinning, biometric auth
   ├─── IoT/Embedded ──────────────────────► Hardware security, secure boot
   ├─── Cloud Native ──────────────────────► Service mesh security, secrets management
   ├─── Enterprise ────────────────────────► SSO, LDAP, Active Directory
   └─── Distributed Systems ───────────────► Mutual TLS, SPIFFE

5. What are your compliance requirements?
   ├─── GDPR ──────────────────────────────► Data protection, consent management
   ├─── HIPAA ─────────────────────────────► Health data privacy
   ├─── PCI DSS ───────────────────────────► Payment card data security
   ├─── SOX ───────────────────────────────► Financial reporting controls
   ├─── ISO 27001 ─────────────────────────► Information security management
   ├─── NIST ──────────────────────────────► Cybersecurity framework
   └─── FedRAMP ───────────────────────────► Federal cloud security

6. What is your performance requirement?
   ├─── High Throughput ──────────────────► Symmetric encryption, caching
   ├─── Low Latency ──────────────────────► Fast cryptographic operations
   ├─── Resource Constrained ─────────────► Lightweight algorithms, hardware acceleration
   ├─── Batch Processing ─────────────────► Parallel encryption, streaming
   └─── Real-time ────────────────────────► Asynchronous crypto, precomputation

7. What is your key management requirement?
   ├─── Centralized ───────────────────────► HSM, key management service
   ├─── Distributed ───────────────────────► Shamir's secret sharing, threshold crypto
   ├─── Ephemeral ─────────────────────────► Session keys, perfect forward secrecy
   ├─── Long-term ─────────────────────────► Key rotation, certificate lifecycle
   ├─── Hardware-backed ──────────────────► TPM, Secure Enclave, hardware keys
   └─── Cloud-managed ────────────────────► AWS KMS, Azure Key Vault, GCP KMS

8. What is your monitoring requirement?
   ├─── Real-time Security Events ────────► SIEM, security dashboards
   ├─── Compliance Reporting ─────────────► Audit logs, tamper-evident logging
   ├─── Threat Intelligence ──────────────► IOC feeds, anomaly detection
   ├─── Forensics ────────────────────────► Immutable audit trails
   └─── Incident Response ────────────────► Automated alerting, playbooks
```

## 📊 **Performance Characteristics**

| Security Pattern | Security Level | Performance Impact | Scalability | Complexity |
|------------------|----------------|-------------------|-------------|------------|
| **Basic Authentication** | Low | Low | High | Low |
| **OAuth2/JWT** | Medium | Medium | High | Medium |
| **Mutual TLS** | High | High | Medium | High |
| **AES Encryption** | High | Low | Very High | Low |
| **RSA Signatures** | High | Medium | High | Medium |
| **HMAC** | Medium | Very Low | Very High | Low |
| **RBAC** | Medium | Low | High | Medium |
| **ABAC** | High | Medium | Medium | High |

## 🎯 **Pattern Variants by Security Domain**

### **Authentication Patterns** 🔐
```cpp
// OAuth2 Authorization Code Flow
class OAuth2Provider {
    std::unordered_map<std::string, Client> clients;
    std::unordered_map<std::string, AuthorizationCode> codes;
    std::unordered_map<std::string, AccessToken> tokens;

    std::string initiate_authorization(const std::string& client_id,
                                     const std::string& redirect_uri,
                                     const std::string& scope,
                                     const std::string& state) {
        // Validate client
        if (!clients.count(client_id)) {
            throw std::runtime_error("Invalid client");
        }

        // Generate authorization code
        std::string code = generate_secure_random_string();
        codes[code] = AuthorizationCode{client_id, redirect_uri, scope, state};

        // Redirect to authorization endpoint
        return redirect_uri + "?code=" + code + "&state=" + state;
    }

    TokenResponse exchange_code_for_token(const std::string& code,
                                        const std::string& client_id,
                                        const std::string& client_secret,
                                        const std::string& redirect_uri) {
        // Validate authorization code
        if (!codes.count(code)) {
            throw std::runtime_error("Invalid authorization code");
        }

        auto& auth_code = codes[code];
        if (auth_code.client_id != client_id ||
            auth_code.redirect_uri != redirect_uri) {
            throw std::runtime_error("Code mismatch");
        }

        // Generate access token
        std::string access_token = generate_jwt_token(client_id, auth_code.scope);
        std::string refresh_token = generate_secure_random_string();

        // Clean up used code
        codes.erase(code);

        return {access_token, "Bearer", 3600, refresh_token};
    }
};
```

### **Authorization Patterns** 🛡️
```cpp
// Role-Based Access Control (RBAC)
class RBACSystem {
    struct Role {
        std::string name;
        std::unordered_set<std::string> permissions;
        std::unordered_set<std::string> parent_roles;
    };

    struct User {
        std::string id;
        std::unordered_set<std::string> roles;
        std::unordered_map<std::string, std::string> attributes;
    };

    std::unordered_map<std::string, Role> roles;
    std::unordered_map<std::string, User> users;
    std::unordered_map<std::string, std::unordered_set<std::string>> role_hierarchy;

    bool check_permission(const std::string& user_id,
                         const std::string& resource,
                         const std::string& action) {

        if (!users.count(user_id)) return false;

        const auto& user_roles = users[user_id].roles;
        std::unordered_set<std::string> effective_roles = user_roles;

        // Include parent roles
        for (const auto& role : user_roles) {
            get_all_parent_roles(role, effective_roles);
        }

        // Check permissions
        for (const auto& role_name : effective_roles) {
            if (roles.count(role_name)) {
                const auto& role_perms = roles[role_name].permissions;
                std::string permission = action + ":" + resource;
                if (role_perms.count(permission)) {
                    return true;
                }
            }
        }

        return false;
    }

    void get_all_parent_roles(const std::string& role,
                            std::unordered_set<std::string>& result) {
        if (role_hierarchy.count(role)) {
            for (const auto& parent : role_hierarchy[role]) {
                if (result.insert(parent).second) {
                    get_all_parent_roles(parent, result);
                }
            }
        }
    }
};
```

### **Cryptography Patterns** 🔒
```cpp
// AES Encryption with Key Derivation
class AESEncryption {
    const size_t KEY_SIZE = 32;  // 256 bits
    const size_t SALT_SIZE = 16;
    const int ITERATIONS = 10000;

    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& plaintext,
                               const std::string& password) {
        // Generate salt
        std::vector<uint8_t> salt = generate_random_bytes(SALT_SIZE);

        // Derive key using PBKDF2
        std::vector<uint8_t> key = pbkdf2(password, salt, ITERATIONS, KEY_SIZE);

        // Generate IV
        std::vector<uint8_t> iv = generate_random_bytes(16);

        // Encrypt using AES-256-CBC
        std::vector<uint8_t> ciphertext = aes_encrypt(plaintext, key, iv);

        // Combine salt + IV + ciphertext
        std::vector<uint8_t> result;
        result.insert(result.end(), salt.begin(), salt.end());
        result.insert(result.end(), iv.begin(), iv.end());
        result.insert(result.end(), ciphertext.begin(), ciphertext.end());

        return result;
    }

    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& ciphertext_with_metadata,
                               const std::string& password) {
        // Extract salt, IV, and ciphertext
        size_t pos = 0;
        std::vector<uint8_t> salt(ciphertext_with_metadata.begin(),
                                ciphertext_with_metadata.begin() + SALT_SIZE);
        pos += SALT_SIZE;

        std::vector<uint8_t> iv(ciphertext_with_metadata.begin() + pos,
                              ciphertext_with_metadata.begin() + pos + 16);
        pos += 16;

        std::vector<uint8_t> ciphertext(ciphertext_with_metadata.begin() + pos,
                                      ciphertext_with_metadata.end());

        // Derive key
        std::vector<uint8_t> key = pbkdf2(password, salt, ITERATIONS, KEY_SIZE);

        // Decrypt
        return aes_decrypt(ciphertext, key, iv);
    }
};
```

### **Secure Communication Patterns** 🌐
```cpp
// Mutual TLS Authentication
class MutualTLSConnection {
    SSL_CTX* ssl_ctx;
    std::string server_cert_path;
    std::string server_key_path;
    std::string client_ca_path;

    void initialize_ssl_context() {
        // Create SSL context
        ssl_ctx = SSL_CTX_new(TLS_server_method());

        // Load server certificate and private key
        if (SSL_CTX_use_certificate_file(ssl_ctx, server_cert_path.c_str(), SSL_FILETYPE_PEM) <= 0) {
            throw std::runtime_error("Failed to load server certificate");
        }

        if (SSL_CTX_use_PrivateKey_file(ssl_ctx, server_key_path.c_str(), SSL_FILETYPE_PEM) <= 0) {
            throw std::runtime_error("Failed to load server private key");
        }

        // Verify private key matches certificate
        if (!SSL_CTX_check_private_key(ssl_ctx)) {
            throw std::runtime_error("Private key does not match certificate");
        }

        // Load client CA certificates for client certificate verification
        if (SSL_CTX_load_verify_locations(ssl_ctx, client_ca_path.c_str(), nullptr) <= 0) {
            throw std::runtime_error("Failed to load client CA certificates");
        }

        // Require client certificates
        SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);

        // Set cipher suites (prefer forward secrecy)
        SSL_CTX_set_cipher_list(ssl_ctx, "ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256");
    }

    SSL* accept_connection(int client_socket) {
        SSL* ssl = SSL_new(ssl_ctx);
        SSL_set_fd(ssl, client_socket);

        if (SSL_accept(ssl) <= 0) {
            // Handle SSL handshake failure
            SSL_free(ssl);
            throw std::runtime_error("SSL handshake failed");
        }

        // Verify client certificate
        X509* client_cert = SSL_get_peer_certificate(ssl);
        if (!client_cert) {
            SSL_free(ssl);
            throw std::runtime_error("Client certificate not provided");
        }

        // Additional certificate validation logic here
        X509_free(client_cert);

        return ssl;
    }
};
```

## 🏆 **Real-World Production Examples**

### **Authentication Patterns**
- **OAuth2**: Google, Facebook, GitHub, AWS Cognito
- **JWT**: Auth0, Firebase, Keycloak, Okta
- **SAML**: Enterprise SSO, Active Directory, Shibboleth
- **Multi-Factor Authentication**: Google Authenticator, YubiKey, Duo
- **Biometric Authentication**: iOS Face ID, Android BiometricPrompt
- **Certificate-based**: Smart cards, client certificates, mutual TLS

### **Authorization Patterns**
- **RBAC**: AWS IAM, Kubernetes RBAC, Linux permissions
- **ABAC**: Google Zanzibar, AWS IAM policies, XACML
- **ACLs**: File system permissions, database grants, network ACLs
- **Policy-based**: Open Policy Agent (OPA), AWS IAM Policies
- **Capability-based**: Capsicum, seL4, Pony language

### **Cryptography Patterns**
- **Symmetric Encryption**: AES (AES-256-GCM), ChaCha20-Poly1305
- **Asymmetric Encryption**: RSA, ECDSA, Ed25519
- **Hash Functions**: SHA-256, SHA-3, Blake2
- **HMAC**: JWT signatures, API authentication, webhook verification
- **Key Exchange**: ECDHE, X25519, quantum-resistant algorithms
- **Digital Signatures**: ECDSA, EdDSA, RSA-PSS

### **Secure Communication Patterns**
- **TLS 1.3**: HTTPS, secure APIs, VPN tunnels
- **Mutual TLS**: Service mesh (Istio), Kubernetes, Linkerd
- **IPsec**: VPN, site-to-site encryption, network security
- **WireGuard**: Modern VPN, peer-to-peer encryption
- **QUIC**: HTTP/3, connection migration, 0-RTT handshake
- **Noise Protocol**: WhatsApp, WireGuard, secure messaging

### **Input Validation Patterns**
- **OWASP Validation**: Input sanitization, output encoding, CSRF protection
- **Schema Validation**: JSON Schema, XML Schema, Protocol Buffers
- **Type-safe Parsing**: Rust ownership, TypeScript strict mode
- **Boundary Checking**: Buffer overflow prevention, integer overflow detection
- **Format Validation**: Email validation, URL parsing, SQL injection prevention

### **Session Management Patterns**
- **Secure Cookies**: HttpOnly, Secure, SameSite, signed cookies
- **JWT Tokens**: Stateless sessions, microservices authentication
- **Session Stores**: Redis sessions, database sessions, encrypted client-side
- **Token Refresh**: OAuth2 refresh tokens, sliding expiration
- **Session Fixation Protection**: Session regeneration, token binding

### **Audit Logging Patterns**
- **Immutable Logs**: Blockchain-based logging, WORM storage, cryptographic hashing
- **Structured Logging**: JSON logs, log aggregation (ELK stack), correlation IDs
- **Security Event Logging**: Failed authentication, privilege changes, data access
- **Compliance Logging**: SOX logging, HIPAA audit trails, PCI DSS requirements
- **Log Integrity**: Log signing, tamper detection, secure log shipping

### **Threat Modeling Patterns**
- **STRIDE**: Spoofing, Tampering, Repudiation, Information disclosure, Denial of service, Elevation of privilege
- **PASTA**: Process for Attack Simulation and Threat Analysis
- **OWASP Threat Modeling**: Application threat modeling, risk assessment
- **Microsoft Threat Modeling**: Data flow diagrams, trust boundaries
- **CIA Triad**: Confidentiality, Integrity, Availability analysis

### **Secure Coding Patterns**
- **Input Validation**: Whitelist validation, parameterized queries, output encoding
- **Error Handling**: Secure error messages, exception safety, resource cleanup
- **Authentication**: Secure password storage, session management, access control
- **Cryptography**: Proper key management, secure random generation, algorithm selection
- **Data Protection**: Encryption at rest, encryption in transit, data classification

### **Identity Management Patterns**
- **Identity Providers**: SAML IdP, OpenID Connect, Active Directory
- **User Management**: SCIM provisioning, user lifecycle, role management
- **Federation**: SAML federation, cross-domain trust, identity brokering
- **Directory Services**: LDAP, Active Directory, OpenLDAP
- **Attribute-based Access**: X.509 attributes, SAML assertions, JWT claims

## ⚡ **Advanced Security Patterns**

### **1. Zero Trust Architecture**
```cpp
class ZeroTrustSecurity {
    struct IdentityContext {
        std::string user_id;
        std::vector<std::string> roles;
        std::unordered_map<std::string, std::string> attributes;
        std::chrono::steady_clock::time_point authenticated_at;
        std::string session_token;
    };

    struct ResourceContext {
        std::string resource_id;
        std::string resource_type;
        std::unordered_map<std::string, std::string> attributes;
        std::string classification;  // public, internal, confidential, restricted
    };

    struct AccessRequest {
        IdentityContext identity;
        ResourceContext resource;
        std::string action;
        std::unordered_map<std::string, std::string> environment;
    };

    class PolicyEngine {
        std::vector<std::function<bool(const AccessRequest&)>> policies;

    public:
        void add_policy(std::function<bool(const AccessRequest&)> policy) {
            policies.push_back(policy);
        }

        AuthorizationDecision evaluate(const AccessRequest& request) {
            // Evaluate all policies
            std::vector<std::string> reasons;

            for (const auto& policy : policies) {
                if (!policy(request)) {
                    reasons.push_back("Policy violation");
                }
            }

            // Additional context checks
            if (!is_request_from_trusted_network(request)) {
                reasons.push_back("Untrusted network");
            }

            if (is_user_session_expired(request.identity)) {
                reasons.push_back("Session expired");
            }

            if (!is_device_compliant(request)) {
                reasons.push_back("Non-compliant device");
            }

            // Continuous verification
            if (!perform_continuous_verification(request)) {
                reasons.push_back("Continuous verification failed");
            }

            return AuthorizationDecision{reasons.empty(), reasons};
        }

    private:
        bool is_request_from_trusted_network(const AccessRequest& request) {
            // Check IP reputation, geo-location, network trust score
            return true; // Simplified
        }

        bool is_user_session_expired(const IdentityContext& identity) {
            auto now = std::chrono::steady_clock::now();
            auto session_duration = now - identity.authenticated_at;
            return session_duration > std::chrono::hours(8);
        }

        bool is_device_compliant(const AccessRequest& request) {
            // Check device posture, security patches, antivirus status
            return true; // Simplified
        }

        bool perform_continuous_verification(const AccessRequest& request) {
            // Risk-based authentication, behavioral analysis, step-up authentication
            return true; // Simplified
        }
    };

    PolicyEngine policy_engine_;

public:
    AuthorizationDecision authorize_access(const AccessRequest& request) {
        return policy_engine_.evaluate(request);
    }

    void add_access_policy(std::function<bool(const AccessRequest&)> policy) {
        policy_engine_.add_policy(policy);
    }
};
```

### **2. Homomorphic Encryption**
```cpp
class HomomorphicEncryption {
    // Simplified Paillier cryptosystem for demonstration
    struct PublicKey {
        BigInteger n;      // n = p * q
        BigInteger g;      // generator
        BigInteger n_squared;
    };

    struct PrivateKey {
        BigInteger lambda; // lcm(p-1, q-1)
        BigInteger mu;     // modular inverse
    };

    std::pair<PublicKey, PrivateKey> generate_keypair(int key_size = 2048) {
        // Generate two large primes
        BigInteger p = generate_large_prime(key_size / 2);
        BigInteger q = generate_large_prime(key_size / 2);
        BigInteger n = p * q;

        BigInteger g = n + 1;  // Simple generator
        BigInteger n_squared = n * n;

        // Private key components
        BigInteger p_minus_1 = p - 1;
        BigInteger q_minus_1 = q - 1;
        BigInteger lambda = lcm(p_minus_1, q_minus_1);
        BigInteger mu = modular_inverse(L(g, n), n);

        return {
            {n, g, n_squared},
            {lambda, mu}
        };
    }

    BigInteger encrypt(const BigInteger& plaintext, const PublicKey& pub_key) {
        BigInteger r = generate_random_in_range(BigInteger(1), pub_key.n - 1);
        BigInteger ciphertext = mod_pow(pub_key.g, plaintext, pub_key.n_squared) *
                              mod_pow(r, pub_key.n, pub_key.n_squared);
        return ciphertext % pub_key.n_squared;
    }

    BigInteger decrypt(const BigInteger& ciphertext, const PrivateKey& priv_key, const PublicKey& pub_key) {
        BigInteger u = L(mod_pow(ciphertext, priv_key.lambda, pub_key.n_squared), pub_key.n);
        return (u * priv_key.mu) % pub_key.n;
    }

    // Homomorphic addition: D(E(a) * E(b)) = a + b
    BigInteger add_encrypted(const BigInteger& cipher_a, const BigInteger& cipher_b,
                           const PublicKey& pub_key) {
        return (cipher_a * cipher_b) % pub_key.n_squared;
    }

    // Homomorphic multiplication by constant: D(E(a)^k) = k * a
    BigInteger multiply_by_constant(const BigInteger& ciphertext, const BigInteger& constant,
                                  const PublicKey& pub_key) {
        return mod_pow(ciphertext, constant, pub_key.n_squared);
    }

private:
    BigInteger L(const BigInteger& x, const BigInteger& n) {
        return (x - 1) / n;
    }

    // BigInteger arithmetic operations would be implemented here
    // For demonstration, using placeholder implementations
    BigInteger generate_large_prime(int bits) { return BigInteger(1); }
    BigInteger lcm(const BigInteger& a, const BigInteger& b) { return BigInteger(1); }
    BigInteger modular_inverse(const BigInteger& a, const BigInteger& n) { return BigInteger(1); }
    BigInteger mod_pow(const BigInteger& base, const BigInteger& exp, const BigInteger& mod) { return BigInteger(1); }
    BigInteger generate_random_in_range(const BigInteger& min, const BigInteger& max) { return BigInteger(1); }
};
```

### **3. Secure Multi-Party Computation (MPC)**
```cpp
class SecureMultiPartyComputation {
    struct Share {
        int party_id;
        BigInteger value;
        std::vector<uint8_t> mac;  // Message authentication code
    };

    class SecretSharing {
    public:
        // Shamir's secret sharing
        std::vector<Share> share_secret(const BigInteger& secret, int threshold, int num_shares) {
            // Generate random polynomial: f(x) = secret + a1*x + a2*x^2 + ... + a_{t-1}*x^{t-1}
            std::vector<BigInteger> coefficients(threshold);
            coefficients[0] = secret;

            for (int i = 1; i < threshold; ++i) {
                coefficients[i] = generate_random_bigint();
            }

            std::vector<Share> shares;
            for (int i = 1; i <= num_shares; ++i) {
                BigInteger x_i = BigInteger(i);
                BigInteger y_i = evaluate_polynomial(coefficients, x_i);

                Share share{i, y_i};
                shares.push_back(share);
            }

            return shares;
        }

        BigInteger reconstruct_secret(const std::vector<Share>& shares) {
            // Lagrange interpolation at x = 0
            BigInteger result = BigInteger(0);

            for (size_t i = 0; i < shares.size(); ++i) {
                BigInteger lagrange_coeff = BigInteger(1);

                for (size_t j = 0; j < shares.size(); ++j) {
                    if (i != j) {
                        BigInteger x_i = BigInteger(shares[i].party_id);
                        BigInteger x_j = BigInteger(shares[j].party_id);

                        lagrange_coeff = lagrange_coeff * (x_j) * modular_inverse(x_j - x_i, PRIME);
                        lagrange_coeff = lagrange_coeff % PRIME;
                    }
                }

                result = (result + shares[i].value * lagrange_coeff) % PRIME;
            }

            return result;
        }

    private:
        const BigInteger PRIME = BigInteger("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");  // secp256k1 prime

        BigInteger evaluate_polynomial(const std::vector<BigInteger>& coeffs, const BigInteger& x) {
            BigInteger result = coeffs.back();

            for (int i = static_cast<int>(coeffs.size()) - 2; i >= 0; --i) {
                result = (result * x + coeffs[i]) % PRIME;
            }

            return result;
        }

        BigInteger generate_random_bigint() {
            // Generate random BigInteger
            return BigInteger(1);  // Placeholder
        }

        BigInteger modular_inverse(const BigInteger& a, const BigInteger& m) {
            // Extended Euclidean algorithm
            return BigInteger(1);  // Placeholder
        }
    };

    class YaoGarbleCircuit {
        // Yao's garbled circuits for secure two-party computation
        struct GarbledGate {
            std::vector<std::vector<uint8_t>> garbled_table;
            std::vector<uint8_t> output_mask;
        };

        std::vector<GarbledGate> garbled_circuit;

    public:
        void garble_circuit(const Circuit& circuit) {
            // Convert boolean circuit to garbled circuit
            for (const auto& gate : circuit.gates) {
                GarbledGate garbled_gate;

                // Generate random masks for wires
                std::vector<uint8_t> input_masks(2);
                std::vector<uint8_t> output_mask(1);

                generate_random_masks(input_masks.data(), input_masks.size());
                generate_random_masks(output_mask.data(), output_mask.size());

                // Garble the truth table
                garbled_gate.garbled_table.resize(4);  // 2^2 entries for 2-input gate

                for (int i = 0; i < 4; ++i) {
                    int input0 = (i >> 0) & 1;
                    int input1 = (i >> 1) & 1;
                    int output = evaluate_gate(gate.type, input0, input1);

                    // Encrypt: E(K_{i0,i1}, output ⊕ R)
                    uint8_t key[16];
                    derive_key(key, input_masks[0] ^ (input0 ? 0xFF : 0x00),
                              input_masks[1] ^ (input1 ? 0xFF : 0x00));

                    uint8_t plaintext = (output ? 0xFF : 0x00) ^ output_mask[0];
                    encrypt_aes(garbled_gate.garbled_table[i].data(), plaintext, key);
                }

                garbled_gate.output_mask = output_mask;
                garbled_circuit.push_back(garbled_gate);
            }
        }

        uint8_t evaluate_garbled(const std::vector<uint8_t>& input_labels) {
            std::vector<uint8_t> current_labels = input_labels;

            for (const auto& gate : garbled_circuit) {
                // Find the correct garbled entry
                uint8_t key[16];
                derive_key(key, current_labels[0], current_labels[1]);

                uint8_t decrypted_output;
                bool found = false;

                for (const auto& entry : gate.garbled_table) {
                    if (decrypt_and_check(entry.data(), &decrypted_output, key)) {
                        current_labels[0] = decrypted_output ^ gate.output_mask[0];
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    throw std::runtime_error("Garbling error");
                }
            }

            return current_labels[0];
        }

    private:
        int evaluate_gate(const std::string& gate_type, int a, int b) {
            if (gate_type == "AND") return a & b;
            if (gate_type == "OR") return a | b;
            if (gate_type == "XOR") return a ^ b;
            return 0;
        }

        void generate_random_masks(uint8_t* masks, size_t count) {
            // Generate random masks for wire labels
            for (size_t i = 0; i < count; ++i) {
                masks[i] = static_cast<uint8_t>(rand() % 256);
            }
        }

        void derive_key(uint8_t* key, uint8_t label0, uint8_t label1) {
            // Simple key derivation (insecure - for demonstration only)
            for (int i = 0; i < 16; ++i) {
                key[i] = label0 ^ label1 ^ static_cast<uint8_t>(i);
            }
        }

        void encrypt_aes(uint8_t* ciphertext, uint8_t plaintext, const uint8_t* key) {
            // Simplified AES encryption (placeholder)
            ciphertext[0] = plaintext ^ key[0];
        }

        bool decrypt_and_check(const uint8_t* ciphertext, uint8_t* plaintext, const uint8_t* key) {
            // Simplified AES decryption (placeholder)
            *plaintext = ciphertext[0] ^ key[0];
            return true;
        }
    };
};
```

### **4. Post-Quantum Cryptography**
```cpp
class PostQuantumCryptography {
    // Kyber (key encapsulation mechanism) - lattice-based
    class KyberKEM {
        struct PublicKey {
            std::vector<uint8_t> pk;
            std::vector<uint8_t> seed;
        };

        struct PrivateKey {
            std::vector<uint8_t> sk;
            std::vector<uint8_t> pk;
            std::vector<uint8_t> hpk;
            std::vector<uint8_t> z;
        };

        struct Ciphertext {
            std::vector<uint8_t> ct;
            std::vector<uint8_t> ss;  // Shared secret
        };

        static constexpr size_t KYBER_N = 256;
        static constexpr size_t KYBER_K = 3;  // Security level

    public:
        std::pair<PublicKey, PrivateKey> generate_keypair() {
            // Generate random seed
            std::vector<uint8_t> seed(32);
            generate_random_bytes(seed.data(), seed.size());

            // Generate A matrix (uniform random from seed)
            auto A = generate_matrix_A(seed);

            // Generate secret vector s
            auto s = generate_secret_vector();

            // Generate error vector e
            auto e = generate_error_vector();

            // Compute t = A*s + e
            auto t = matrix_vector_add(matrix_vector_multiply(A, s), e);

            PublicKey pk;
            pk.seed = seed;
            pk.pk = compress_public_key(t);

            PrivateKey sk;
            sk.sk = compress_secret_key(s);
            sk.pk = pk.pk;
            sk.hpk = hash_public_key(pk.pk);
            sk.z = generate_random_bytes(32);

            return {pk, sk};
        }

        Ciphertext encapsulate(const PublicKey& pk) {
            // Generate random message m
            std::vector<uint8_t> m(32);
            generate_random_bytes(m.data(), m.size());

            // Generate random r
            std::vector<uint8_t> r(32);
            generate_random_bytes(r.data(), r.size());

            // Decompress public key
            auto t = decompress_public_key(pk.pk);
            auto A = generate_matrix_A(pk.seed);

            // Generate error vectors
            auto e1 = generate_error_vector();
            auto e2 = generate_error_vector();

            // Compute u = A*r + e1
            auto u = matrix_vector_add(matrix_vector_multiply(A, expand_vector(r)), e1);

            // Compute v = t*r + e2 + compress(m)
            auto tr = vector_multiply(t, expand_vector(r));
            auto compressed_m = compress_message(m);
            auto v = vector_add(vector_add(tr, e2), compressed_m);

            Ciphertext ct;
            ct.ct = encode_ciphertext(u, v);
            ct.ss = kdf(m, encode_ciphertext(u, v));  // Key derivation function

            return ct;
        }

        std::vector<uint8_t> decapsulate(const Ciphertext& ct, const PrivateKey& sk) {
            // Decompress ciphertext
            auto [u, v] = decode_ciphertext(ct.ct);

            // Decompress secret key
            auto s = decompress_secret_key(sk.sk);

            // Compute m' = v - s*u
            auto su = vector_multiply(s, u);
            auto m_prime = decompress_message(vector_subtract(v, su));

            // Verify ciphertext validity (simplified)
            if (!verify_ciphertext(u, v, sk.pk)) {
                // Return random value for security
                std::vector<uint8_t> random_ss(32);
                generate_random_bytes(random_ss.data(), random_ss.size());
                return random_ss;
            }

            return kdf(m_prime, ct.ct);
        }

    private:
        // Placeholder implementations for lattice operations
        using Vector = std::vector<int16_t>;
        using Matrix = std::vector<Vector>;

        Matrix generate_matrix_A(const std::vector<uint8_t>& seed) {
            Matrix A(KYBER_K, Vector(KYBER_K));
            // Implementation would generate uniform random matrix
            return A;
        }

        Vector generate_secret_vector() {
            Vector s(KYBER_K);
            // Sample from centered binomial distribution
            return s;
        }

        Vector generate_error_vector() {
            Vector e(KYBER_K);
            // Sample from discrete Gaussian
            return e;
        }

        Vector matrix_vector_multiply(const Matrix& A, const Vector& v) {
            Vector result(KYBER_K);
            // Matrix-vector multiplication in ring
            return result;
        }

        Vector matrix_vector_add(const Vector& a, const Vector& b) {
            Vector result(KYBER_K);
            for (size_t i = 0; i < a.size(); ++i) {
                result[i] = (a[i] + b[i]) % KYBER_Q;
            }
            return result;
        }

        std::vector<uint8_t> compress_public_key(const Vector& t) {
            // Compress and serialize public key
            return std::vector<uint8_t>(KYBER_PUBLICKEYBYTES);
        }

        Vector decompress_public_key(const std::vector<uint8_t>& pk) {
            // Decompress public key
            return Vector(KYBER_K);
        }

        // Additional placeholder methods...
        std::vector<uint8_t> kdf(const std::vector<uint8_t>& m, const std::vector<uint8_t>& ct) {
            return std::vector<uint8_t>(32);  // SHAKE-256 output
        }
    };

    // Dilithium (digital signature algorithm) - lattice-based
    class DilithiumSignature {
        // Implementation similar to Kyber but for signatures
        // Uses Fiat-Shamir with aborts for zero-knowledge proofs
    };
};
```

## 📚 **Further Reading**

- **"Security Engineering"** - Ross Anderson
- **"Cryptography Engineering"** - Niels Ferguson, Bruce Schneier, Tadayoshi Kohno
- **"OAuth 2.0 Security Best Current Practice"** - IETF RFC 8725
- **"JSON Web Token (JWT) Profile for OAuth 2.0 Access Tokens"** - IETF RFC 9068
- **"The OAuth 2.0 Authorization Framework"** - IETF RFC 6749
- **"Vehicle Safety Communications Project"** - Security credential management system
- **"Applied Cryptography"** - Bruce Schneier
- **"Serious Cryptography"** - Jean-Philippe Aumasson
- **"Real-World Cryptography"** - David Wong

---

*"Security is not a product, but a process - it's about building systems that remain secure even when assumptions are violated."* 🔐⚡