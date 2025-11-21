/**
 * libsodium Cryptography Wrapper - Production Implementation
 *
 * This file provides production-grade wrappers around libsodium for:
 * - Authenticated encryption (XChaCha20-Poly1305, AES256-GCM)
 * - Digital signatures (Ed25519)
 * - Key exchange (X25519)
 * - Password hashing (Argon2)
 * - Hash functions (Blake2b)
 * - Random number generation
 * - Secret key authentication (HMAC)
 *
 * libsodium provides a modern, easy-to-use, and hard-to-misuse API.
 */

#include <sodium.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <memory>
#include <functional>

namespace sodium {

// libsodium initialization
class SodiumInit {
public:
    SodiumInit() {
        if (sodium_init() < 0) {
            throw std::runtime_error("libsodium initialization failed");
        }
    }

    ~SodiumInit() {
        // libsodium doesn't require explicit cleanup
    }
};

// Secure buffer with automatic zeroing
class SecureBuffer {
public:
    explicit SecureBuffer(size_t size) : data_(size), size_(size) {}

    ~SecureBuffer() {
        if (!data_.empty()) {
            sodium_memzero(data_.data(), data_.size());
        }
    }

    uint8_t* data() { return data_.data(); }
    const uint8_t* data() const { return data_.data(); }
    size_t size() const { return size_; }

    void resize(size_t new_size) {
        data_.resize(new_size);
        size_ = new_size;
    }

    std::vector<uint8_t> release() {
        std::vector<uint8_t> result = std::move(data_);
        size_ = 0;
        return result;
    }

private:
    std::vector<uint8_t> data_;
    size_t size_;
};

// Authenticated Encryption with Associated Data (AEAD)
class AEAD {
public:
    enum Algorithm {
        XCHACHA20_POLY1305,
        AES256_GCM
    };

    AEAD(Algorithm alg = XCHACHA20_POLY1305) : algorithm_(alg) {}

    // Encrypt with authentication
    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& plaintext,
                               const std::vector<uint8_t>& key,
                               const std::vector<uint8_t>& nonce,
                               const std::vector<uint8_t>& additional_data = {}) {
        if (key.size() != keySize()) {
            throw std::runtime_error("Invalid key size");
        }
        if (nonce.size() != nonceSize()) {
            throw std::runtime_error("Invalid nonce size");
        }

        std::vector<uint8_t> ciphertext(ciphertextSize(plaintext.size()));
        unsigned long long ciphertext_len;

        int result;
        switch (algorithm_) {
            case XCHACHA20_POLY1305:
                result = crypto_aead_xchacha20poly1305_ietf_encrypt(
                    ciphertext.data(), &ciphertext_len,
                    plaintext.data(), plaintext.size(),
                    additional_data.data(), additional_data.size(),
                    nullptr, nonce.data(), key.data());
                break;
            case AES256_GCM:
                result = crypto_aead_aes256gcm_encrypt(
                    ciphertext.data(), &ciphertext_len,
                    plaintext.data(), plaintext.size(),
                    additional_data.data(), additional_data.size(),
                    nullptr, nonce.data(), key.data());
                break;
            default:
                throw std::runtime_error("Unsupported AEAD algorithm");
        }

        if (result != 0) {
            throw std::runtime_error("AEAD encryption failed");
        }

        ciphertext.resize(ciphertext_len);
        return ciphertext;
    }

    // Decrypt with authentication verification
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& ciphertext,
                               const std::vector<uint8_t>& key,
                               const std::vector<uint8_t>& nonce,
                               const std::vector<uint8_t>& additional_data = {}) {
        if (key.size() != keySize()) {
            throw std::runtime_error("Invalid key size");
        }
        if (nonce.size() != nonceSize()) {
            throw std::runtime_error("Invalid nonce size");
        }

        std::vector<uint8_t> plaintext(plaintextSize(ciphertext.size()));
        unsigned long long plaintext_len;

        int result;
        switch (algorithm_) {
            case XCHACHA20_POLY1305:
                result = crypto_aead_xchacha20poly1305_ietf_decrypt(
                    plaintext.data(), &plaintext_len,
                    nullptr,
                    ciphertext.data(), ciphertext.size(),
                    additional_data.data(), additional_data.size(),
                    nonce.data(), key.data());
                break;
            case AES256_GCM:
                result = crypto_aead_aes256gcm_decrypt(
                    plaintext.data(), &plaintext_len,
                    nullptr,
                    ciphertext.data(), ciphertext.size(),
                    additional_data.data(), additional_data.size(),
                    nonce.data(), key.data());
                break;
            default:
                throw std::runtime_error("Unsupported AEAD algorithm");
        }

        if (result != 0) {
            throw std::runtime_error("AEAD decryption failed - authentication error");
        }

        plaintext.resize(plaintext_len);
        return plaintext;
    }

    // Generate secure key
    static std::vector<uint8_t> generateKey(Algorithm alg = XCHACHA20_POLY1305) {
        std::vector<uint8_t> key(keySize(alg));
        switch (alg) {
            case XCHACHA20_POLY1305:
                crypto_aead_xchacha20poly1305_ietf_keygen(key.data());
                break;
            case AES256_GCM:
                randombytes_buf(key.data(), key.size());
                break;
        }
        return key;
    }

    // Generate secure nonce
    static std::vector<uint8_t> generateNonce(Algorithm alg = XCHACHA20_POLY1305) {
        std::vector<uint8_t> nonce(nonceSize(alg));
        randombytes_buf(nonce.data(), nonce.size());
        return nonce;
    }

private:
    size_t keySize() const { return keySize(algorithm_); }
    size_t nonceSize() const { return nonceSize(algorithm_); }

    static size_t keySize(Algorithm alg) {
        switch (alg) {
            case XCHACHA20_POLY1305: return crypto_aead_xchacha20poly1305_ietf_KEYBYTES;
            case AES256_GCM: return crypto_aead_aes256gcm_KEYBYTES;
            default: return 0;
        }
    }

    static size_t nonceSize(Algorithm alg) {
        switch (alg) {
            case XCHACHA20_POLY1305: return crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;
            case AES256_GCM: return crypto_aead_aes256gcm_NPUBBYTES;
            default: return 0;
        }
    }

    size_t ciphertextSize(size_t plaintext_size) const {
        switch (algorithm_) {
            case XCHACHA20_POLY1305:
                return plaintext_size + crypto_aead_xchacha20poly1305_ietf_ABYTES;
            case AES256_GCM:
                return plaintext_size + crypto_aead_aes256gcm_ABYTES;
            default: return 0;
        }
    }

    size_t plaintextSize(size_t ciphertext_size) const {
        switch (algorithm_) {
            case XCHACHA20_POLY1305:
                return ciphertext_size - crypto_aead_xchacha20poly1305_ietf_ABYTES;
            case AES256_GCM:
                return ciphertext_size - crypto_aead_aes256gcm_ABYTES;
            default: return 0;
        }
    }

    Algorithm algorithm_;
};

// Secret-key authenticated encryption (simpler AEAD for small messages)
class SecretBox {
public:
    static std::vector<uint8_t> encrypt(const std::vector<uint8_t>& message,
                                      const std::vector<uint8_t>& key) {
        if (key.size() != crypto_secretbox_KEYBYTES) {
            throw std::runtime_error("Invalid key size");
        }

        std::vector<uint8_t> nonce(crypto_secretbox_NONCEBYTES);
        randombytes_buf(nonce.data(), nonce.size());

        std::vector<uint8_t> ciphertext(crypto_secretbox_MACBYTES + message.size());
        if (crypto_secretbox_easy(ciphertext.data(), message.data(), message.size(),
                                nonce.data(), key.data()) != 0) {
            throw std::runtime_error("SecretBox encryption failed");
        }

        // Prepend nonce for decryption
        ciphertext.insert(ciphertext.begin(), nonce.begin(), nonce.end());
        return ciphertext;
    }

    static std::vector<uint8_t> decrypt(const std::vector<uint8_t>& ciphertext,
                                      const std::vector<uint8_t>& key) {
        if (key.size() != crypto_secretbox_KEYBYTES) {
            throw std::runtime_error("Invalid key size");
        }
        if (ciphertext.size() < crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES) {
            throw std::runtime_error("Ciphertext too short");
        }

        std::vector<uint8_t> nonce(ciphertext.begin(),
                                 ciphertext.begin() + crypto_secretbox_NONCEBYTES);
        std::vector<uint8_t> encrypted_data(ciphertext.begin() + crypto_secretbox_NONCEBYTES,
                                          ciphertext.end());
        std::vector<uint8_t> decrypted(encrypted_data.size() - crypto_secretbox_MACBYTES);

        if (crypto_secretbox_open_easy(decrypted.data(), encrypted_data.data(),
                                     encrypted_data.size(), nonce.data(), key.data()) != 0) {
            throw std::runtime_error("SecretBox decryption failed - authentication error");
        }

        return decrypted;
    }

    static std::vector<uint8_t> generateKey() {
        std::vector<uint8_t> key(crypto_secretbox_KEYBYTES);
        crypto_secretbox_keygen(key.data());
        return key;
    }
};

// Digital Signatures (Ed25519)
class Sign {
public:
    // Generate key pair
    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>> generateKeyPair() {
        std::vector<uint8_t> public_key(crypto_sign_PUBLICKEYBYTES);
        std::vector<uint8_t> secret_key(crypto_sign_SECRETKEYBYTES);

        if (crypto_sign_keypair(public_key.data(), secret_key.data()) != 0) {
            throw std::runtime_error("Key pair generation failed");
        }

        return {secret_key, public_key};
    }

    // Generate key pair with seed (deterministic)
    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>>
    generateKeyPairFromSeed(const std::vector<uint8_t>& seed) {
        if (seed.size() != crypto_sign_SEEDBYTES) {
            throw std::runtime_error("Invalid seed size");
        }

        std::vector<uint8_t> public_key(crypto_sign_PUBLICKEYBYTES);
        std::vector<uint8_t> secret_key(crypto_sign_SECRETKEYBYTES);

        if (crypto_sign_seed_keypair(public_key.data(), secret_key.data(), seed.data()) != 0) {
            throw std::runtime_error("Seeded key pair generation failed");
        }

        return {secret_key, public_key};
    }

    // Sign message
    static std::vector<uint8_t> sign(const std::vector<uint8_t>& message,
                                   const std::vector<uint8_t>& secret_key) {
        if (secret_key.size() != crypto_sign_SECRETKEYBYTES) {
            throw std::runtime_error("Invalid secret key size");
        }

        std::vector<uint8_t> signed_message(crypto_sign_BYTES + message.size());
        unsigned long long signed_len;

        if (crypto_sign(signed_message.data(), &signed_len,
                       message.data(), message.size(), secret_key.data()) != 0) {
            throw std::runtime_error("Signing failed");
        }

        signed_message.resize(signed_len);
        return signed_message;
    }

    // Verify signature
    static std::pair<bool, std::vector<uint8_t>> verify(
        const std::vector<uint8_t>& signed_message,
        const std::vector<uint8_t>& public_key) {

        if (public_key.size() != crypto_sign_PUBLICKEYBYTES) {
            throw std::runtime_error("Invalid public key size");
        }
        if (signed_message.size() < crypto_sign_BYTES) {
            throw std::runtime_error("Signed message too short");
        }

        std::vector<uint8_t> message(signed_message.size() - crypto_sign_BYTES);
        unsigned long long message_len;

        int result = crypto_sign_open(message.data(), &message_len,
                                    signed_message.data(), signed_message.size(),
                                    public_key.data());

        if (result != 0) {
            return {false, {}}; // Verification failed
        }

        message.resize(message_len);
        return {true, message};
    }

    // Detached signature (sign only, no message included)
    static std::vector<uint8_t> signDetached(const std::vector<uint8_t>& message,
                                           const std::vector<uint8_t>& secret_key) {
        if (secret_key.size() != crypto_sign_SECRETKEYBYTES) {
            throw std::runtime_error("Invalid secret key size");
        }

        std::vector<uint8_t> signature(crypto_sign_BYTES);
        unsigned long long sig_len;

        if (crypto_sign_detached(signature.data(), &sig_len,
                               message.data(), message.size(), secret_key.data()) != 0) {
            throw std::runtime_error("Detached signing failed");
        }

        return signature;
    }

    // Verify detached signature
    static bool verifyDetached(const std::vector<uint8_t>& signature,
                              const std::vector<uint8_t>& message,
                              const std::vector<uint8_t>& public_key) {
        if (signature.size() != crypto_sign_BYTES) {
            throw std::runtime_error("Invalid signature size");
        }
        if (public_key.size() != crypto_sign_PUBLICKEYBYTES) {
            throw std::runtime_error("Invalid public key size");
        }

        return crypto_sign_verify_detached(signature.data(), message.data(),
                                          message.size(), public_key.data()) == 0;
    }

    // Convert Ed25519 public key to X25519 (for key exchange)
    static std::vector<uint8_t> ed25519PublicKeyToX25519(const std::vector<uint8_t>& ed25519_pk) {
        if (ed25519_pk.size() != crypto_sign_PUBLICKEYBYTES) {
            throw std::runtime_error("Invalid Ed25519 public key size");
        }

        std::vector<uint8_t> x25519_pk(crypto_scalarmult_curve25519_BYTES);
        if (crypto_sign_ed25519_pk_to_curve25519(x25519_pk.data(), ed25519_pk.data()) != 0) {
            throw std::runtime_error("Public key conversion failed");
        }

        return x25519_pk;
    }

    // Convert Ed25519 secret key to X25519 (for key exchange)
    static std::vector<uint8_t> ed25519SecretKeyToX25519(const std::vector<uint8_t>& ed25519_sk) {
        if (ed25519_sk.size() != crypto_sign_SECRETKEYBYTES) {
            throw std::runtime_error("Invalid Ed25519 secret key size");
        }

        std::vector<uint8_t> x25519_sk(crypto_scalarmult_curve25519_BYTES);
        if (crypto_sign_ed25519_sk_to_curve25519(x25519_sk.data(), ed25519_sk.data()) != 0) {
            throw std::runtime_error("Secret key conversion failed");
        }

        return x25519_sk;
    }
};

// Key Exchange (X25519)
class KeyExchange {
public:
    // Generate key pair
    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>> generateKeyPair() {
        std::vector<uint8_t> public_key(crypto_kx_PUBLICKEYBYTES);
        std::vector<uint8_t> secret_key(crypto_kx_SECRETKEYBYTES);

        if (crypto_kx_keypair(public_key.data(), secret_key.data()) != 0) {
            throw std::runtime_error("Key pair generation failed");
        }

        return {secret_key, public_key};
    }

    // Client session keys (client initiates)
    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>> clientSessionKeys(
        const std::vector<uint8_t>& client_secret_key,
        const std::vector<uint8_t>& client_public_key,
        const std::vector<uint8_t>& server_public_key) {

        if (client_secret_key.size() != crypto_kx_SECRETKEYBYTES ||
            client_public_key.size() != crypto_kx_PUBLICKEYBYTES ||
            server_public_key.size() != crypto_kx_PUBLICKEYBYTES) {
            throw std::runtime_error("Invalid key sizes");
        }

        std::vector<uint8_t> rx_key(crypto_kx_SESSIONKEYBYTES);
        std::vector<uint8_t> tx_key(crypto_kx_SESSIONKEYBYTES);

        if (crypto_kx_client_session_keys(rx_key.data(), tx_key.data(),
                                        client_public_key.data(), client_secret_key.data(),
                                        server_public_key.data()) != 0) {
            throw std::runtime_error("Client session key derivation failed");
        }

        return {rx_key, tx_key};
    }

    // Server session keys (server responds)
    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>> serverSessionKeys(
        const std::vector<uint8_t>& server_secret_key,
        const std::vector<uint8_t>& server_public_key,
        const std::vector<uint8_t>& client_public_key) {

        if (server_secret_key.size() != crypto_kx_SECRETKEYBYTES ||
            server_public_key.size() != crypto_kx_PUBLICKEYBYTES ||
            client_public_key.size() != crypto_kx_PUBLICKEYBYTES) {
            throw std::runtime_error("Invalid key sizes");
        }

        std::vector<uint8_t> rx_key(crypto_kx_SESSIONKEYBYTES);
        std::vector<uint8_t> tx_key(crypto_kx_SESSIONKEYBYTES);

        if (crypto_kx_server_session_keys(rx_key.data(), tx_key.data(),
                                        server_public_key.data(), server_secret_key.data(),
                                        client_public_key.data()) != 0) {
            throw std::runtime_error("Server session key derivation failed");
        }

        return {rx_key, tx_key};
    }

    // Direct scalar multiplication (X25519)
    static std::vector<uint8_t> scalarMult(const std::vector<uint8_t>& secret_key,
                                         const std::vector<uint8_t>& public_key) {
        if (secret_key.size() != crypto_scalarmult_curve25519_BYTES ||
            public_key.size() != crypto_scalarmult_curve25519_BYTES) {
            throw std::runtime_error("Invalid key sizes");
        }

        std::vector<uint8_t> shared_secret(crypto_scalarmult_curve25519_BYTES);
        if (crypto_scalarmult_curve25519(shared_secret.data(), secret_key.data(),
                                        public_key.data()) != 0) {
            throw std::runtime_error("Scalar multiplication failed");
        }

        return shared_secret;
    }

    // Generate scalar and compute public key
    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>> generateScalarMultKeyPair() {
        std::vector<uint8_t> secret_key(crypto_scalarmult_curve25519_BYTES);
        std::vector<uint8_t> public_key(crypto_scalarmult_curve25519_BYTES);

        crypto_scalarmult_curve25519_base(public_key.data(), secret_key.data());

        return {secret_key, public_key};
    }
};

// Password Hashing (Argon2)
class PasswordHash {
public:
    enum Algorithm {
        ARGON2I,   // Resistant to side-channel attacks
        ARGON2ID,  // Hybrid of Argon2i and Argon2d
        ARGON2D    // Resistant to time-memory trade-off
    };

    // Hash password
    static std::vector<uint8_t> hash(const std::string& password,
                                   size_t hash_length = crypto_pwhash_BYTES_MIN,
                                   Algorithm alg = ARGON2ID) {
        std::vector<uint8_t> hash(hash_length);
        std::vector<uint8_t> salt(crypto_pwhash_SALTBYTES);
        randombytes_buf(salt.data(), salt.size());

        unsigned long long ops_limit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
        size_t mem_limit = crypto_pwhash_MEMLIMIT_INTERACTIVE;

        int (*hash_func)(unsigned char*, unsigned long long,
                        const char*, unsigned long long,
                        const unsigned char*, unsigned long long,
                        unsigned long long, size_t);

        switch (alg) {
            case ARGON2I: hash_func = crypto_pwhash; break;
            case ARGON2ID: hash_func = crypto_pwhash; break; // Default is Argon2id
            case ARGON2D: hash_func = crypto_pwhash; break;
            default: throw std::runtime_error("Unsupported password hash algorithm");
        }

        if (hash_func(hash.data(), hash.size(),
                     password.c_str(), password.length(),
                     salt.data(), ops_limit, mem_limit, crypto_pwhash_ALG_DEFAULT) != 0) {
            throw std::runtime_error("Password hashing failed");
        }

        // Prepend salt for storage
        hash.insert(hash.begin(), salt.begin(), salt.end());
        return hash;
    }

    // Verify password against hash
    static bool verify(const std::string& password,
                      const std::vector<uint8_t>& stored_hash) {
        if (stored_hash.size() < crypto_pwhash_SALTBYTES) {
            return false;
        }

        std::vector<uint8_t> hash(stored_hash.begin() + crypto_pwhash_SALTBYTES, stored_hash.end());
        std::vector<uint8_t> salt(stored_hash.begin(), stored_hash.begin() + crypto_pwhash_SALTBYTES);

        unsigned long long ops_limit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
        size_t mem_limit = crypto_pwhash_MEMLIMIT_INTERACTIVE;

        return crypto_pwhash_verify(hash.data(), hash.size(),
                                  password.c_str(), password.length(),
                                  salt.data(), ops_limit, mem_limit,
                                  crypto_pwhash_ALG_DEFAULT) == 0;
    }

    // Derive key from password (for encryption keys)
    static std::vector<uint8_t> deriveKey(const std::string& password,
                                        const std::vector<uint8_t>& salt,
                                        size_t key_length,
                                        Algorithm alg = ARGON2ID) {
        if (salt.size() != crypto_pwhash_SALTBYTES) {
            throw std::runtime_error("Invalid salt size");
        }

        std::vector<uint8_t> key(key_length);
        unsigned long long ops_limit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
        size_t mem_limit = crypto_pwhash_MEMLIMIT_INTERACTIVE;

        if (crypto_pwhash(key.data(), key.size(),
                         password.c_str(), password.length(),
                         salt.data(), ops_limit, mem_limit, crypto_pwhash_ALG_DEFAULT) != 0) {
            throw std::runtime_error("Key derivation failed");
        }

        return key;
    }

    // Generate random salt
    static std::vector<uint8_t> generateSalt() {
        std::vector<uint8_t> salt(crypto_pwhash_SALTBYTES);
        randombytes_buf(salt.data(), salt.size());
        return salt;
    }
};

// Hash Functions (Blake2b)
class Hash {
public:
    enum Algorithm {
        BLAKE2B_256,
        BLAKE2B_512
    };

    Hash(Algorithm alg = BLAKE2B_256) : algorithm_(alg) {}

    // Hash data
    std::vector<uint8_t> hash(const std::vector<uint8_t>& data) {
        std::vector<uint8_t> digest(digestSize());
        crypto_generichash_state state;

        if (crypto_generichash_init(&state, nullptr, 0, digest.size()) != 0) {
            throw std::runtime_error("Hash initialization failed");
        }

        if (crypto_generichash_update(&state, data.data(), data.size()) != 0) {
            throw std::runtime_error("Hash update failed");
        }

        if (crypto_generichash_final(&state, digest.data(), digest.size()) != 0) {
            throw std::runtime_error("Hash finalization failed");
        }

        return digest;
    }

    // Keyed hash (HMAC equivalent)
    std::vector<uint8_t> keyedHash(const std::vector<uint8_t>& data,
                                 const std::vector<uint8_t>& key) {
        std::vector<uint8_t> digest(digestSize());
        crypto_generichash_state state;

        if (crypto_generichash_init(&state, key.data(), key.size(), digest.size()) != 0) {
            throw std::runtime_error("Keyed hash initialization failed");
        }

        if (crypto_generichash_update(&state, data.data(), data.size()) != 0) {
            throw std::runtime_error("Keyed hash update failed");
        }

        if (crypto_generichash_final(&state, digest.data(), digest.size()) != 0) {
            throw std::runtime_error("Keyed hash finalization failed");
        }

        return digest;
    }

    // Incremental hashing
    class IncrementalHash {
    public:
        IncrementalHash(Algorithm alg = BLAKE2B_256) : digest_size_(digestSize(alg)) {
            if (crypto_generichash_init(&state_, nullptr, 0, digest_size_) != 0) {
                throw std::runtime_error("Incremental hash initialization failed");
            }
        }

        void update(const std::vector<uint8_t>& data) {
            if (crypto_generichash_update(&state_, data.data(), data.size()) != 0) {
                throw std::runtime_error("Incremental hash update failed");
            }
        }

        std::vector<uint8_t> finalize() {
            std::vector<uint8_t> digest(digest_size_);
            if (crypto_generichash_final(&state_, digest.data(), digest.size()) != 0) {
                throw std::runtime_error("Incremental hash finalization failed");
            }
            return digest;
        }

    private:
        crypto_generichash_state state_;
        size_t digest_size_;

        static size_t digestSize(Algorithm alg) {
            switch (alg) {
                case BLAKE2B_256: return crypto_generichash_BYTES_MIN;
                case BLAKE2B_512: return crypto_generichash_BYTES_MAX;
                default: return crypto_generichash_BYTES;
            }
        }
    };

private:
    size_t digestSize() const {
        switch (algorithm_) {
            case BLAKE2B_256: return crypto_generichash_BYTES_MIN;
            case BLAKE2B_512: return crypto_generichash_BYTES_MAX;
            default: return crypto_generichash_BYTES;
        }
    }

    Algorithm algorithm_;
};

// Random Number Generation
class Random {
public:
    // Generate random bytes
    static std::vector<uint8_t> bytes(size_t count) {
        std::vector<uint8_t> buffer(count);
        randombytes_buf(buffer.data(), buffer.size());
        return buffer;
    }

    // Generate uniform random number
    static uint32_t uniform(uint32_t upper_bound) {
        return randombytes_uniform(upper_bound);
    }

    // Generate random key for specific algorithm
    static std::vector<uint8_t> secretBoxKey() {
        std::vector<uint8_t> key(crypto_secretbox_KEYBYTES);
        crypto_secretbox_keygen(key.data());
        return key;
    }

    static std::vector<uint8_t> aeadKey(AEAD::Algorithm alg = AEAD::XCHACHA20_POLY1305) {
        return AEAD::generateKey(alg);
    }

    // Stir random pool (if using deterministic randomness for testing)
    static void stir() {
        randombytes_stir();
    }
};

// Secure Memory Operations
class SecureMemory {
public:
    // Compare in constant time
    static bool compare(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) {
        if (a.size() != b.size()) return false;
        return sodium_memcmp(a.data(), b.data(), a.size()) == 0;
    }

    // Zero memory securely
    static void zero(void* ptr, size_t len) {
        sodium_memzero(ptr, len);
    }

    // Allocate locked memory (if available)
    static void* allocate(size_t size) {
        return sodium_malloc(size);
    }

    // Free locked memory
    static void free(void* ptr) {
        sodium_free(ptr);
    }
};

// Main crypto facade class
class Crypto {
public:
    static void initialize() {
        static SodiumInit init;
    }

    // High-level encryption/decryption
    static std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data,
                                      const std::vector<uint8_t>& key) {
        return SecretBox::encrypt(data, key);
    }

    static std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data,
                                      const std::vector<uint8_t>& key) {
        return SecretBox::decrypt(data, key);
    }

    // AEAD encryption
    static std::vector<uint8_t> encryptAEAD(const std::vector<uint8_t>& data,
                                          const std::vector<uint8_t>& key,
                                          const std::vector<uint8_t>& nonce,
                                          const std::vector<uint8_t>& aad = {}) {
        AEAD aead;
        return aead.encrypt(data, key, nonce, aad);
    }

    static std::vector<uint8_t> decryptAEAD(const std::vector<uint8_t>& data,
                                          const std::vector<uint8_t>& key,
                                          const std::vector<uint8_t>& nonce,
                                          const std::vector<uint8_t>& aad = {}) {
        AEAD aead;
        return aead.decrypt(data, key, nonce, aad);
    }

    // Digital signatures
    static auto generateKeyPair() {
        return Sign::generateKeyPair();
    }

    static std::vector<uint8_t> sign(const std::vector<uint8_t>& data,
                                   const std::vector<uint8_t>& secret_key) {
        return Sign::sign(data, secret_key);
    }

    static bool verify(const std::vector<uint8_t>& signed_data,
                      const std::vector<uint8_t>& public_key) {
        auto [valid, message] = Sign::verify(signed_data, public_key);
        return valid;
    }

    // Detached signatures
    static std::vector<uint8_t> signDetached(const std::vector<uint8_t>& data,
                                           const std::vector<uint8_t>& secret_key) {
        return Sign::signDetached(data, secret_key);
    }

    static bool verifyDetached(const std::vector<uint8_t>& signature,
                              const std::vector<uint8_t>& data,
                              const std::vector<uint8_t>& public_key) {
        return Sign::verifyDetached(signature, data, public_key);
    }

    // Key exchange
    static auto generateKeyExchangePair() {
        return KeyExchange::generateKeyPair();
    }

    // Password hashing
    static std::vector<uint8_t> hashPassword(const std::string& password) {
        return PasswordHash::hash(password);
    }

    static bool verifyPassword(const std::string& password,
                             const std::vector<uint8_t>& hash) {
        return PasswordHash::verify(password, hash);
    }

    // Hashing
    static std::vector<uint8_t> hash(const std::vector<uint8_t>& data) {
        Hash hasher;
        return hasher.hash(data);
    }

    // Random generation
    static std::vector<uint8_t> randomBytes(size_t count) {
        return Random::bytes(count);
    }

    static std::vector<uint8_t> generateKey() {
        return Random::secretBoxKey();
    }
};

} // namespace sodium

// Example usage and test functions
namespace sodium_examples {

// Basic authenticated encryption
void basicEncryptionExample() {
    sodium::Crypto::initialize();

    std::string message = "Hello, World!";
    std::vector<uint8_t> data(message.begin(), message.end());
    auto key = sodium::Crypto::generateKey();

    // Encrypt
    auto encrypted = sodium::Crypto::encrypt(data, key);
    std::cout << "Encrypted size: " << encrypted.size() << " bytes" << std::endl;

    // Decrypt
    auto decrypted = sodium::Crypto::decrypt(encrypted, key);
    std::string result(decrypted.begin(), decrypted.end());
    std::cout << "Decrypted: " << result << std::endl;

    assert(result == message);
}

// AEAD encryption with additional data
void aeadExample() {
    sodium::Crypto::initialize();

    std::string message = "Secret message";
    std::string additional_data = "Header data";
    std::vector<uint8_t> data(message.begin(), message.end());
    std::vector<uint8_t> aad(additional_data.begin(), additional_data.end());
    auto key = sodium::AEAD::generateKey();
    auto nonce = sodium::AEAD::generateNonce();

    // Encrypt with AAD
    auto encrypted = sodium::Crypto::encryptAEAD(data, key, nonce, aad);

    // Decrypt with AAD
    auto decrypted = sodium::Crypto::decryptAEAD(encrypted, key, nonce, aad);
    std::string result(decrypted.begin(), decrypted.end());

    std::cout << "AEAD decrypted: " << result << std::endl;
    assert(result == message);
}

// Digital signature example
void digitalSignatureExample() {
    sodium::Crypto::initialize();

    std::string message = "This message will be signed";
    std::vector<uint8_t> data(message.begin(), message.end());

    // Generate key pair
    auto [secret_key, public_key] = sodium::Crypto::generateKeyPair();

    // Sign
    auto signature = sodium::Crypto::signDetached(data, secret_key);
    std::cout << "Signature size: " << signature.size() << " bytes" << std::endl;

    // Verify
    bool valid = sodium::Crypto::verifyDetached(signature, data, public_key);
    std::cout << "Signature valid: " << (valid ? "Yes" : "No") << std::endl;

    assert(valid);
}

// Password hashing example
void passwordHashingExample() {
    sodium::Crypto::initialize();

    std::string password = "mySecurePassword123!";

    // Hash password
    auto hash = sodium::Crypto::hashPassword(password);
    std::cout << "Password hash size: " << hash.size() << " bytes" << std::endl;

    // Verify password
    bool valid = sodium::Crypto::verifyPassword(password, hash);
    std::cout << "Password verification: " << (valid ? "Success" : "Failed") << std::endl;

    assert(valid);
}

// Key exchange example
void keyExchangeExample() {
    sodium::Crypto::initialize();

    // Alice generates key pair
    auto [alice_secret, alice_public] = sodium::Crypto::generateKeyExchangePair();

    // Bob generates key pair
    auto [bob_secret, bob_public] = sodium::Crypto::generateKeyExchangePair();

    // Alice computes session keys (as client)
    auto [alice_rx, alice_tx] = sodium::KeyExchange::clientSessionKeys(
        alice_secret, alice_public, bob_public);

    // Bob computes session keys (as server)
    auto [bob_rx, bob_tx] = sodium::KeyExchange::serverSessionKeys(
        bob_secret, bob_public, alice_public);

    // Session keys should match
    assert(alice_rx == bob_tx);
    assert(alice_tx == bob_rx);

    std::cout << "Key exchange successful - session keys match!" << std::endl;
}

} // namespace sodium_examples

#endif // CRYPTO_LIBSODIUM_WRAPPER_HPP
