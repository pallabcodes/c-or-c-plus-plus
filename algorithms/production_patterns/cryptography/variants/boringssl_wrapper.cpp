/**
 * BoringSSL Cryptography Wrapper - Production Implementation
 *
 * This file provides production-grade wrappers around BoringSSL for:
 * - Authenticated encryption (AES-GCM, ChaCha20-Poly1305)
 * - Hash functions (SHA-256, SHA-512, SHA-3)
 * - HMAC authentication
 * - Digital signatures (ECDSA, Ed25519)
 * - Key exchange (X25519, ECDH)
 * - TLS 1.3 support
 * - Certificate handling
 * - Random number generation
 *
 * BoringSSL is Google's security-focused fork of OpenSSL.
 */

#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/hmac.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/ecdh.h>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <functional>

namespace boringssl {

// BoringSSL initialization
class BoringSSLInit {
public:
    BoringSSLInit() {
        CRYPTO_library_init();
        OpenSSL_add_all_algorithms();
        ERR_load_crypto_strings();
        SSL_load_error_strings();
    }

    ~BoringSSLInit() {
        EVP_cleanup();
        ERR_free_strings();
        SSL_COMP_free_compression_methods();
    }
};

// Error handling
class BoringSSLError : public std::runtime_error {
public:
    explicit BoringSSLError(const std::string& message)
        : std::runtime_error(message + ": " + getBoringSSLErrorString()) {}

private:
    static std::string getBoringSSLErrorString() {
        unsigned long err = ERR_get_error();
        char buffer[256];
        ERR_error_string_n(err, buffer, sizeof(buffer));
        return buffer;
    }
};

// Secure buffer with automatic zeroing
class SecureBuffer {
public:
    explicit SecureBuffer(size_t size) : data_(size), size_(size) {}

    ~SecureBuffer() {
        if (!data_.empty()) {
            OPENSSL_cleanse(data_.data(), data_.size());
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

// Authenticated Encryption
class AEAD {
public:
    enum Algorithm {
        AES_256_GCM,
        AES_128_GCM,
        CHACHA20_POLY1305
    };

    AEAD(Algorithm alg = AES_256_GCM) : algorithm_(alg) {
        initializeCipher();
    }

    ~AEAD() {
        if (ctx_) EVP_AEAD_CTX_free(ctx_);
    }

    void setKey(const std::vector<uint8_t>& key) {
        if (!EVP_AEAD_CTX_init(ctx_, cipher_, key.data(), key.size(),
                             EVP_AEAD_DEFAULT_TAG_LENGTH, nullptr)) {
            throw BoringSSLError("Failed to set AEAD key");
        }
    }

    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& plaintext,
                               const std::vector<uint8_t>& nonce,
                               const std::vector<uint8_t>& additional_data = {}) {
        std::vector<uint8_t> ciphertext(plaintext.size() + EVP_AEAD_MAX_TAG_LENGTH);

        size_t out_len;
        if (!EVP_AEAD_CTX_seal(ctx_, ciphertext.data(), &out_len,
                             ciphertext.size(), nonce.data(), nonce.size(),
                             plaintext.data(), plaintext.size(),
                             additional_data.data(), additional_data.size())) {
            throw BoringSSLError("AEAD encryption failed");
        }

        ciphertext.resize(out_len);
        return ciphertext;
    }

    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& ciphertext,
                               const std::vector<uint8_t>& nonce,
                               const std::vector<uint8_t>& additional_data = {}) {
        std::vector<uint8_t> plaintext(ciphertext.size());

        size_t out_len;
        if (!EVP_AEAD_CTX_open(ctx_, plaintext.data(), &out_len,
                             plaintext.size(), nonce.data(), nonce.size(),
                             ciphertext.data(), ciphertext.size(),
                             additional_data.data(), additional_data.size())) {
            throw BoringSSLError("AEAD decryption failed - authentication error");
        }

        plaintext.resize(out_len);
        return plaintext;
    }

    // High-level convenience functions
    static std::vector<uint8_t> encrypt(const std::vector<uint8_t>& plaintext,
                                      const std::vector<uint8_t>& key,
                                      const std::vector<uint8_t>& nonce,
                                      Algorithm alg = AES_256_GCM,
                                      const std::vector<uint8_t>& aad = {}) {
        AEAD aead(alg);
        aead.setKey(key);
        return aead.encrypt(plaintext, nonce, aad);
    }

    static std::vector<uint8_t> decrypt(const std::vector<uint8_t>& ciphertext,
                                      const std::vector<uint8_t>& key,
                                      const std::vector<uint8_t>& nonce,
                                      Algorithm alg = AES_256_GCM,
                                      const std::vector<uint8_t>& aad = {}) {
        AEAD aead(alg);
        aead.setKey(key);
        return aead.decrypt(ciphertext, nonce, aad);
    }

    static std::vector<uint8_t> generateKey(Algorithm alg = AES_256_GCM) {
        size_t key_size = keySize(alg);
        std::vector<uint8_t> key(key_size);
        if (RAND_bytes(key.data(), key.size()) != 1) {
            throw BoringSSLError("Failed to generate random key");
        }
        return key;
    }

    static std::vector<uint8_t> generateNonce(Algorithm alg = AES_256_GCM) {
        size_t nonce_size = nonceSize(alg);
        std::vector<uint8_t> nonce(nonce_size);
        if (RAND_bytes(nonce.data(), nonce.size()) != 1) {
            throw BoringSSLError("Failed to generate random nonce");
        }
        return nonce;
    }

private:
    void initializeCipher() {
        cipher_ = getCipher();
        if (!cipher_) throw std::runtime_error("Unsupported AEAD algorithm");

        ctx_ = EVP_AEAD_CTX_new();
        if (!ctx_) throw BoringSSLError("Failed to create AEAD context");
    }

    const EVP_AEAD* getCipher() const {
        switch (algorithm_) {
            case AES_256_GCM: return EVP_aead_aes_256_gcm();
            case AES_128_GCM: return EVP_aead_aes_128_gcm();
            case CHACHA20_POLY1305: return EVP_aead_chacha20_poly1305();
            default: return nullptr;
        }
    }

    static size_t keySize(Algorithm alg) {
        switch (alg) {
            case AES_256_GCM: return 32;
            case AES_128_GCM: return 16;
            case CHACHA20_POLY1305: return 32;
            default: return 32;
        }
    }

    static size_t nonceSize(Algorithm alg) {
        switch (alg) {
            case AES_256_GCM: return 12;
            case AES_128_GCM: return 12;
            case CHACHA20_POLY1305: return 12;
            default: return 12;
        }
    }

    Algorithm algorithm_;
    const EVP_AEAD* cipher_;
    EVP_AEAD_CTX* ctx_;
};

// Hash Functions
class HashFunction {
public:
    enum Algorithm {
        SHA256,
        SHA384,
        SHA512,
        SHA3_256,
        SHA3_512,
        BLAKE2B_256,
        BLAKE2B_512
    };

    HashFunction(Algorithm alg = SHA256) : algorithm_(alg) {
        initializeHash();
    }

    ~HashFunction() {
        if (ctx_) EVP_MD_CTX_free(ctx_);
    }

    void update(const std::vector<uint8_t>& data) {
        if (!EVP_DigestUpdate(ctx_, data.data(), data.size())) {
            throw BoringSSLError("Hash update failed");
        }
    }

    std::vector<uint8_t> finalize() {
        std::vector<uint8_t> digest(EVP_MAX_MD_SIZE);
        unsigned int len;

        if (!EVP_DigestFinal_ex(ctx_, digest.data(), &len)) {
            throw BoringSSLError("Hash finalization failed");
        }

        digest.resize(len);
        return digest;
    }

    void reset() {
        if (!EVP_MD_CTX_reset(ctx_)) {
            throw BoringSSLError("Hash reset failed");
        }
        if (!EVP_DigestInit_ex(ctx_, md_, nullptr)) {
            throw BoringSSLError("Hash re-initialization failed");
        }
    }

    // One-shot hash
    static std::vector<uint8_t> hash(const std::vector<uint8_t>& data,
                                   Algorithm alg = SHA256) {
        HashFunction hasher(alg);
        hasher.update(data);
        return hasher.finalize();
    }

    // Incremental hash
    class IncrementalHash {
    public:
        IncrementalHash(Algorithm alg = SHA256) : hasher_(alg) {}

        void update(const std::vector<uint8_t>& data) {
            hasher_.update(data);
        }

        std::vector<uint8_t> finalize() {
            return hasher_.finalize();
        }

        void reset() {
            hasher_.reset();
        }

    private:
        HashFunction hasher_;
    };

private:
    void initializeHash() {
        md_ = getDigest();
        if (!md_) throw std::runtime_error("Unsupported hash algorithm");

        ctx_ = EVP_MD_CTX_new();
        if (!ctx_) throw BoringSSLError("Failed to create hash context");

        if (!EVP_DigestInit_ex(ctx_, md_, nullptr)) {
            throw BoringSSLError("Failed to initialize hash");
        }
    }

    const EVP_MD* getDigest() const {
        switch (algorithm_) {
            case SHA256: return EVP_sha256();
            case SHA384: return EVP_sha384();
            case SHA512: return EVP_sha512();
            case SHA3_256: return EVP_sha3_256();
            case SHA3_512: return EVP_sha3_512();
            case BLAKE2B_256: return EVP_blake2b256();
            case BLAKE2B_512: return EVP_blake2b512();
            default: return nullptr;
        }
    }

    Algorithm algorithm_;
    const EVP_MD* md_;
    EVP_MD_CTX* ctx_;
};

// HMAC
class HMAC {
public:
    enum Algorithm {
        HMAC_SHA256,
        HMAC_SHA512
    };

    HMAC(Algorithm alg = HMAC_SHA256) : algorithm_(alg) {
        initializeHMAC();
    }

    ~HMAC() {
        if (ctx_) HMAC_CTX_free(ctx_);
    }

    void setKey(const std::vector<uint8_t>& key) {
        if (!HMAC_Init_ex(ctx_, key.data(), key.size(), md_, nullptr)) {
            throw BoringSSLError("Failed to set HMAC key");
        }
    }

    void update(const std::vector<uint8_t>& data) {
        if (!HMAC_Update(ctx_, data.data(), data.size())) {
            throw BoringSSLError("HMAC update failed");
        }
    }

    std::vector<uint8_t> finalize() {
        std::vector<uint8_t> mac(EVP_MAX_MD_SIZE);
        unsigned int len;

        if (!HMAC_Final(ctx_, mac.data(), &len)) {
            throw BoringSSLError("HMAC finalization failed");
        }

        mac.resize(len);
        return mac;
    }

    void reset() {
        if (!HMAC_Init_ex(ctx_, nullptr, 0, nullptr, nullptr)) {
            throw BoringSSLError("HMAC reset failed");
        }
    }

    // One-shot HMAC
    static std::vector<uint8_t> compute(const std::vector<uint8_t>& data,
                                      const std::vector<uint8_t>& key,
                                      Algorithm alg = HMAC_SHA256) {
        HMAC hmac(alg);
        hmac.setKey(key);
        hmac.update(data);
        return hmac.finalize();
    }

private:
    void initializeHMAC() {
        md_ = getDigest();
        if (!md_) throw std::runtime_error("Unsupported HMAC algorithm");

        ctx_ = HMAC_CTX_new();
        if (!ctx_) throw BoringSSLError("Failed to create HMAC context");
    }

    const EVP_MD* getDigest() const {
        switch (algorithm_) {
            case HMAC_SHA256: return EVP_sha256();
            case HMAC_SHA512: return EVP_sha512();
            default: return nullptr;
        }
    }

    Algorithm algorithm_;
    const EVP_MD* md_;
    HMAC_CTX* ctx_;
};

// Digital Signatures
class DigitalSignature {
public:
    enum Algorithm {
        ECDSA_SHA256,
        ECDSA_SHA512,
        ED25519
    };

    // Generate key pair
    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>>
    generateKeyPair(Algorithm alg = ECDSA_SHA256) {
        EVP_PKEY_CTX* pctx = nullptr;
        EVP_PKEY* pkey = nullptr;

        try {
            int nid = getNID(alg);
            pctx = EVP_PKEY_CTX_new_id(nid, nullptr);
            if (!pctx) throw BoringSSLError("Failed to create key context");

            if (EVP_PKEY_keygen_init(pctx) <= 0)
                throw BoringSSLError("Failed to initialize keygen");

            if (alg == ECDSA_SHA256 || alg == ECDSA_SHA512) {
                if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_X9_62_prime256v1) <= 0)
                    throw BoringSSLError("Failed to set EC curve");
            }

            pkey = nullptr;
            if (EVP_PKEY_keygen(pctx, &pkey) <= 0)
                throw BoringSSLError("Failed to generate key");

            // Export private key
            BIO* priv_bio = BIO_new(BIO_s_mem());
            if (!priv_bio) throw BoringSSLError("Failed to create BIO");

            std::unique_ptr<BIO, decltype(&BIO_free)> priv_bio_guard(priv_bio, BIO_free);

            if (PEM_write_bio_PrivateKey(priv_bio, pkey, nullptr, nullptr, 0, nullptr, nullptr) != 1)
                throw BoringSSLError("Failed to write private key");

            char* priv_data;
            long priv_len = BIO_get_mem_data(priv_bio, &priv_data);
            std::vector<uint8_t> private_key(priv_data, priv_data + priv_len);

            // Export public key
            BIO* pub_bio = BIO_new(BIO_s_mem());
            if (!pub_bio) throw BoringSSLError("Failed to create BIO");

            std::unique_ptr<BIO, decltype(&BIO_free)> pub_bio_guard(pub_bio, BIO_free);

            if (PEM_write_bio_PUBKEY(pub_bio, pkey) != 1)
                throw BoringSSLError("Failed to write public key");

            char* pub_data;
            long pub_len = BIO_get_mem_data(pub_bio, &pub_data);
            std::vector<uint8_t> public_key(pub_data, pub_data + pub_len);

            EVP_PKEY_CTX_free(pctx);
            EVP_PKEY_free(pkey);

            return {private_key, public_key};

        } catch (...) {
            if (pctx) EVP_PKEY_CTX_free(pctx);
            if (pkey) EVP_PKEY_free(pkey);
            throw;
        }
    }

    // Sign data
    static std::vector<uint8_t> sign(const std::vector<uint8_t>& data,
                                   const std::vector<uint8_t>& private_key_pem,
                                   Algorithm alg = ECDSA_SHA256) {
        // Load private key
        BIO* key_bio = BIO_new_mem_buf(private_key_pem.data(), private_key_pem.size());
        if (!key_bio) throw BoringSSLError("Failed to create key BIO");

        std::unique_ptr<BIO, decltype(&BIO_free)> key_bio_guard(key_bio, BIO_free);

        EVP_PKEY* pkey = PEM_read_bio_PrivateKey(key_bio, nullptr, nullptr, nullptr);
        if (!pkey) throw BoringSSLError("Failed to load private key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> pkey_guard(pkey, EVP_PKEY_free);

        // Create signing context
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        if (!ctx) throw BoringSSLError("Failed to create signing context");

        std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> ctx_guard(ctx, EVP_MD_CTX_free);

        const EVP_MD* md = getDigest(alg);
        if (EVP_DigestSignInit(ctx, nullptr, md, nullptr, pkey) != 1)
            throw BoringSSLError("Failed to initialize signing");

        // Sign the data
        size_t sig_len;
        if (EVP_DigestSign(ctx, nullptr, &sig_len, data.data(), data.size()) != 1)
            throw BoringSSLError("Failed to get signature length");

        std::vector<uint8_t> signature(sig_len);
        if (EVP_DigestSign(ctx, signature.data(), &sig_len, data.data(), data.size()) != 1)
            throw BoringSSLError("Failed to sign data");

        signature.resize(sig_len);
        return signature;
    }

    // Verify signature
    static bool verify(const std::vector<uint8_t>& data,
                      const std::vector<uint8_t>& signature,
                      const std::vector<uint8_t>& public_key_pem,
                      Algorithm alg = ECDSA_SHA256) {
        // Load public key
        BIO* key_bio = BIO_new_mem_buf(public_key_pem.data(), public_key_pem.size());
        if (!key_bio) throw BoringSSLError("Failed to create key BIO");

        std::unique_ptr<BIO, decltype(&BIO_free)> key_bio_guard(key_bio, BIO_free);

        EVP_PKEY* pkey = PEM_read_bio_PUBKEY(key_bio, nullptr, nullptr, nullptr);
        if (!pkey) throw BoringSSLError("Failed to load public key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> pkey_guard(pkey, EVP_PKEY_free);

        // Create verification context
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        if (!ctx) throw BoringSSLError("Failed to create verification context");

        std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> ctx_guard(ctx, EVP_MD_CTX_free);

        const EVP_MD* md = getDigest(alg);
        if (EVP_DigestVerifyInit(ctx, nullptr, md, nullptr, pkey) != 1)
            throw BoringSSLError("Failed to initialize verification");

        // Verify the signature
        int result = EVP_DigestVerify(ctx, signature.data(), signature.size(),
                                    data.data(), data.size());

        return result == 1;
    }

private:
    static int getNID(Algorithm alg) {
        switch (alg) {
            case ECDSA_SHA256:
            case ECDSA_SHA512:
                return EVP_PKEY_EC;
            case ED25519:
                return EVP_PKEY_ED25519;
            default:
                return EVP_PKEY_EC;
        }
    }

    static const EVP_MD* getDigest(Algorithm alg) {
        switch (alg) {
            case ECDSA_SHA256: return EVP_sha256();
            case ECDSA_SHA512: return EVP_sha512();
            case ED25519: return nullptr; // Ed25519 uses Pure
            default: return EVP_sha256();
        }
    }
};

// Key Exchange
class KeyExchange {
public:
    enum Algorithm {
        ECDH_P256,
        ECDH_P384,
        ECDH_P521,
        X25519
    };

    // Generate ephemeral key pair
    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>>
    generateEphemeralKey(Algorithm alg = ECDH_P256) {
        EVP_PKEY_CTX* pctx = nullptr;
        EVP_PKEY* pkey = nullptr;

        try {
            int nid = getNID(alg);
            pctx = EVP_PKEY_CTX_new_id(nid, nullptr);
            if (!pctx) throw BoringSSLError("Failed to create key context");

            if (EVP_PKEY_keygen_init(pctx) <= 0)
                throw BoringSSLError("Failed to initialize keygen");

            if (alg == ECDH_P256 || alg == ECDH_P384 || alg == ECDH_P521) {
                int curve_nid = getCurveNID(alg);
                if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, curve_nid) <= 0)
                    throw BoringSSLError("Failed to set EC curve");
            }

            pkey = nullptr;
            if (EVP_PKEY_keygen(pctx, &pkey) <= 0)
                throw BoringSSLError("Failed to generate key");

            // Export private key
            BIO* priv_bio = BIO_new(BIO_s_mem());
            if (!priv_bio) throw BoringSSLError("Failed to create BIO");

            std::unique_ptr<BIO, decltype(&BIO_free)> priv_bio_guard(priv_bio, BIO_free);

            if (PEM_write_bio_PrivateKey(priv_bio, pkey, nullptr, nullptr, 0, nullptr, nullptr) != 1)
                throw BoringSSLError("Failed to write private key");

            char* priv_data;
            long priv_len = BIO_get_mem_data(priv_bio, &priv_data);
            std::vector<uint8_t> private_key(priv_data, priv_data + priv_len);

            // Export public key
            BIO* pub_bio = BIO_new(BIO_s_mem());
            if (!pub_bio) throw BoringSSLError("Failed to create BIO");

            std::unique_ptr<BIO, decltype(&BIO_free)> pub_bio_guard(pub_bio, BIO_free);

            if (PEM_write_bio_PUBKEY(pub_bio, pkey) != 1)
                throw BoringSSLError("Failed to write public key");

            char* pub_data;
            long pub_len = BIO_get_mem_data(pub_bio, &pub_data);
            std::vector<uint8_t> public_key(pub_data, pub_data + pub_len);

            EVP_PKEY_CTX_free(pctx);
            EVP_PKEY_free(pkey);

            return {private_key, public_key};

        } catch (...) {
            if (pctx) EVP_PKEY_CTX_free(pctx);
            if (pkey) EVP_PKEY_free(pkey);
            throw;
        }
    }

    // Derive shared secret
    static std::vector<uint8_t> deriveSharedSecret(
        const std::vector<uint8_t>& private_key_pem,
        const std::vector<uint8_t>& peer_public_key_pem,
        Algorithm alg = ECDH_P256) {

        // Load private key
        BIO* priv_bio = BIO_new_mem_buf(private_key_pem.data(), private_key_pem.size());
        if (!priv_bio) throw BoringSSLError("Failed to create private key BIO");

        std::unique_ptr<BIO, decltype(&BIO_free)> priv_bio_guard(priv_bio, BIO_free);

        EVP_PKEY* priv_key = PEM_read_bio_PrivateKey(priv_bio, nullptr, nullptr, nullptr);
        if (!priv_key) throw BoringSSLError("Failed to load private key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> priv_key_guard(priv_key, EVP_PKEY_free);

        // Load peer public key
        BIO* pub_bio = BIO_new_mem_buf(peer_public_key_pem.data(), peer_public_key_pem.size());
        if (!pub_bio) throw BoringSSLError("Failed to create public key BIO");

        std::unique_ptr<BIO, decltype(&BIO_free)> pub_bio_guard(pub_bio, BIO_free);

        EVP_PKEY* peer_key = PEM_read_bio_PUBKEY(pub_bio, nullptr, nullptr, nullptr);
        if (!peer_key) throw BoringSSLError("Failed to load peer public key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> peer_key_guard(peer_key, EVP_PKEY_free);

        // Derive shared secret
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(priv_key, nullptr);
        if (!ctx) throw BoringSSLError("Failed to create key derivation context");

        std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx_guard(ctx, EVP_PKEY_CTX_free);

        if (EVP_PKEY_derive_init(ctx) != 1)
            throw BoringSSLError("Failed to initialize key derivation");

        if (EVP_PKEY_derive_set_peer(ctx, peer_key) != 1)
            throw BoringSSLError("Failed to set peer key");

        size_t secret_len;
        if (EVP_PKEY_derive(ctx, nullptr, &secret_len) != 1)
            throw BoringSSLError("Failed to get shared secret length");

        std::vector<uint8_t> shared_secret(secret_len);
        if (EVP_PKEY_derive(ctx, shared_secret.data(), &secret_len) != 1)
            throw BoringSSLError("Failed to derive shared secret");

        shared_secret.resize(secret_len);
        return shared_secret;
    }

private:
    static int getNID(Algorithm alg) {
        switch (alg) {
            case ECDH_P256:
            case ECDH_P384:
            case ECDH_P521:
                return EVP_PKEY_EC;
            case X25519:
                return EVP_PKEY_X25519;
            default:
                return EVP_PKEY_EC;
        }
    }

    static int getCurveNID(Algorithm alg) {
        switch (alg) {
            case ECDH_P256: return NID_X9_62_prime256v1;
            case ECDH_P384: return NID_secp384r1;
            case ECDH_P521: return NID_secp521r1;
            default: return NID_X9_62_prime256v1;
        }
    }
};

// TLS 1.3 Connection
class TLSConnection {
public:
    TLSConnection(bool is_server = false) : ctx_(nullptr), ssl_(nullptr), is_server_(is_server) {
        // Create context
        ctx_ = SSL_CTX_new(is_server ? TLS_server_method() : TLS_client_method());
        if (!ctx_) throw BoringSSLError("Failed to create SSL context");

        // Configure secure defaults for TLS 1.3
        SSL_CTX_set_min_proto_version(ctx_, TLS1_3_VERSION);
        SSL_CTX_set_max_proto_version(ctx_, TLS1_3_VERSION);

        // Set secure cipher suites
        if (SSL_CTX_set_ciphersuites(ctx_, "TLS_AES_256_GCM_SHA384:TLS_AES_128_GCM_SHA256") != 1)
            throw BoringSSLError("Failed to set cipher suites");

        // Create SSL object
        ssl_ = SSL_new(ctx_);
        if (!ssl_) throw BoringSSLError("Failed to create SSL object");
    }

    ~TLSConnection() {
        if (ssl_) SSL_free(ssl_);
        if (ctx_) SSL_CTX_free(ctx_);
    }

    // Load certificate and key
    void loadCertificate(const std::string& cert_file, const std::string& key_file) {
        if (SSL_CTX_use_certificate_file(ctx_, cert_file.c_str(), SSL_FILETYPE_PEM) != 1)
            throw BoringSSLError("Failed to load certificate");

        if (SSL_CTX_use_PrivateKey_file(ctx_, key_file.c_str(), SSL_FILETYPE_PEM) != 1)
            throw BoringSSLError("Failed to load private key");

        if (!SSL_CTX_check_private_key(ctx_))
            throw BoringSSLError("Private key does not match certificate");
    }

    // Establish connection
    void connect(BIO* bio) {
        SSL_set_bio(ssl_, bio, bio);
        int result = is_server_ ? SSL_accept(ssl_) : SSL_connect(ssl_);
        if (result != 1) {
            throw BoringSSLError("TLS connection failed");
        }
    }

    // Send data
    void send(const std::vector<uint8_t>& data) {
        size_t sent = 0;
        while (sent < data.size()) {
            int result = SSL_write(ssl_, data.data() + sent, data.size() - sent);
            if (result <= 0) {
                throw BoringSSLError("TLS write failed");
            }
            sent += result;
        }
    }

    // Receive data
    std::vector<uint8_t> receive(size_t max_size = 4096) {
        std::vector<uint8_t> buffer(max_size);
        int result = SSL_read(ssl_, buffer.data(), buffer.size());

        if (result < 0) {
            int err = SSL_get_error(ssl_, result);
            if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                throw BoringSSLError("TLS read failed");
            }
            return {}; // No data available
        }

        if (result == 0) {
            return {}; // Connection closed
        }

        buffer.resize(result);
        return buffer;
    }

    // Get connection info
    std::string getCipherSuite() const {
        const SSL_CIPHER* cipher = SSL_get_current_cipher(ssl_);
        if (!cipher) return "Unknown";
        return SSL_CIPHER_get_name(cipher);
    }

    std::string getProtocolVersion() const {
        return SSL_get_version(ssl_);
    }

private:
    SSL_CTX* ctx_;
    SSL* ssl_;
    bool is_server_;
};

// Random Number Generation
class Random {
public:
    static std::vector<uint8_t> bytes(size_t count) {
        std::vector<uint8_t> buffer(count);
        if (RAND_bytes(buffer.data(), buffer.size()) != 1) {
            throw BoringSSLError("Failed to generate random bytes");
        }
        return buffer;
    }

    static std::vector<uint8_t> generateKey(size_t length = 32) {
        return bytes(length);
    }

    static std::vector<uint8_t> generateIV(size_t length = 12) {
        return bytes(length);
    }
};

// Main crypto facade class
class Crypto {
public:
    static void initialize() {
        static BoringSSLInit init;
    }

    // Authenticated encryption
    static std::vector<uint8_t> encryptAEAD(const std::vector<uint8_t>& data,
                                          const std::vector<uint8_t>& key,
                                          const std::vector<uint8_t>& nonce,
                                          const std::vector<uint8_t>& aad = {}) {
        return AEAD::encrypt(data, key, nonce, AEAD::AES_256_GCM, aad);
    }

    static std::vector<uint8_t> decryptAEAD(const std::vector<uint8_t>& data,
                                          const std::vector<uint8_t>& key,
                                          const std::vector<uint8_t>& nonce,
                                          const std::vector<uint8_t>& aad = {}) {
        return AEAD::decrypt(data, key, nonce, AEAD::AES_256_GCM, aad);
    }

    // Hash functions
    static std::vector<uint8_t> hash(const std::vector<uint8_t>& data,
                                   HashFunction::Algorithm alg = HashFunction::SHA256) {
        return HashFunction::hash(data, alg);
    }

    // HMAC
    static std::vector<uint8_t> hmac(const std::vector<uint8_t>& data,
                                   const std::vector<uint8_t>& key,
                                   HMAC::Algorithm alg = HMAC::HMAC_SHA256) {
        return HMAC::compute(data, key, alg);
    }

    // Digital signatures
    static auto generateKeyPair(DigitalSignature::Algorithm alg = DigitalSignature::ECDSA_SHA256) {
        return DigitalSignature::generateKeyPair(alg);
    }

    static std::vector<uint8_t> sign(const std::vector<uint8_t>& data,
                                   const std::vector<uint8_t>& private_key,
                                   DigitalSignature::Algorithm alg = DigitalSignature::ECDSA_SHA256) {
        return DigitalSignature::sign(data, private_key, alg);
    }

    static bool verify(const std::vector<uint8_t>& data,
                      const std::vector<uint8_t>& signature,
                      const std::vector<uint8_t>& public_key,
                      DigitalSignature::Algorithm alg = DigitalSignature::ECDSA_SHA256) {
        return DigitalSignature::verify(data, signature, public_key, alg);
    }

    // Key exchange
    static auto generateKeyExchangePair(KeyExchange::Algorithm alg = KeyExchange::ECDH_P256) {
        return KeyExchange::generateEphemeralKey(alg);
    }

    static std::vector<uint8_t> deriveSharedSecret(
        const std::vector<uint8_t>& private_key,
        const std::vector<uint8_t>& peer_public_key,
        KeyExchange::Algorithm alg = KeyExchange::ECDH_P256) {
        return KeyExchange::deriveSharedSecret(private_key, peer_public_key, alg);
    }

    // Random generation
    static std::vector<uint8_t> randomBytes(size_t count) {
        return Random::bytes(count);
    }

    static std::vector<uint8_t> generateKey(size_t length = 32) {
        return Random::generateKey(length);
    }

    static std::vector<uint8_t> generateNonce(size_t length = 12) {
        return Random::generateIV(length);
    }
};

} // namespace boringssl

// Example usage and test functions
namespace boringssl_examples {

// Authenticated encryption example
void aeadExample() {
    boringssl::Crypto::initialize();

    std::string message = "Secret message with authentication";
    std::string additional_data = "Header information";
    std::vector<uint8_t> data(message.begin(), message.end());
    std::vector<uint8_t> aad(additional_data.begin(), additional_data.end());
    auto key = boringssl::AEAD::generateKey();
    auto nonce = boringssl::AEAD::generateNonce();

    // Encrypt with AEAD
    auto encrypted = boringssl::Crypto::encryptAEAD(data, key, nonce, aad);
    std::cout << "AEAD encrypted size: " << encrypted.size() << " bytes" << std::endl;

    // Decrypt with AEAD
    auto decrypted = boringssl::Crypto::decryptAEAD(encrypted, key, nonce, aad);
    std::string result(decrypted.begin(), decrypted.end());
    std::cout << "AEAD decrypted: " << result << std::endl;

    assert(result == message);
}

// Hash function example
void hashExample() {
    boringssl::Crypto::initialize();

    std::string message = "Hash this message";
    std::vector<uint8_t> data(message.begin(), message.end());

    auto sha256_hash = boringssl::Crypto::hash(data, boringssl::HashFunction::SHA256);
    auto sha3_hash = boringssl::Crypto::hash(data, boringssl::HashFunction::SHA3_256);

    std::cout << "SHA-256 size: " << sha256_hash.size() << " bytes" << std::endl;
    std::cout << "SHA-3 size: " << sha3_hash.size() << " bytes" << std::endl;

    // Incremental hashing
    boringssl::HashFunction::IncrementalHash hasher(boringssl::HashFunction::BLAKE2B_256);
    hasher.update(std::vector<uint8_t>(data.begin(), data.begin() + 8));
    hasher.update(std::vector<uint8_t>(data.begin() + 8, data.end()));
    auto incremental_hash = hasher.finalize();

    std::cout << "Blake2b incremental size: " << incremental_hash.size() << " bytes" << std::endl;
}

// HMAC example
void hmacExample() {
    boringssl::Crypto::initialize();

    std::string message = "Authenticate this message";
    std::vector<uint8_t> data(message.begin(), message.end());
    auto key = boringssl::Crypto::generateKey(32);

    auto hmac = boringssl::Crypto::hmac(data, key, boringssl::HMAC::HMAC_SHA256);
    std::cout << "HMAC size: " << hmac.size() << " bytes" << std::endl;
}

// Digital signature example
void digitalSignatureExample() {
    boringssl::Crypto::initialize();

    std::string message = "This message will be signed with BoringSSL";
    std::vector<uint8_t> data(message.begin(), message.end());

    // Generate key pair
    auto [private_key, public_key] = boringssl::Crypto::generateKeyPair();

    // Sign
    auto signature = boringssl::Crypto::sign(data, private_key);
    std::cout << "Signature size: " << signature.size() << " bytes" << std::endl;

    // Verify
    bool valid = boringssl::Crypto::verify(data, signature, public_key);
    std::cout << "Signature valid: " << (valid ? "Yes" : "No") << std::endl;

    assert(valid);
}

// Key exchange example
void keyExchangeExample() {
    boringssl::Crypto::initialize();

    // Alice generates key pair
    auto [alice_private, alice_public] = boringssl::Crypto::generateKeyExchangePair();

    // Bob generates key pair
    auto [bob_private, bob_public] = boringssl::Crypto::generateKeyExchangePair();

    // Alice derives shared secret
    auto alice_secret = boringssl::Crypto::deriveSharedSecret(alice_private, bob_public);

    // Bob derives shared secret
    auto bob_secret = boringssl::Crypto::deriveSharedSecret(bob_private, alice_public);

    // Shared secrets should be identical
    assert(alice_secret == bob_secret);
    std::cout << "Key exchange successful - shared secret size: "
              << alice_secret.size() << " bytes" << std::endl;
}

} // namespace boringssl_examples

#endif // CRYPTO_BORINGSSL_WRAPPER_HPP
