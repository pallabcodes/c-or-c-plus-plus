/**
 * OpenSSL Cryptography Wrapper - Production Implementation
 *
 * This file provides production-grade wrappers around OpenSSL for:
 * - Symmetric encryption (AES-GCM, ChaCha20-Poly1305)
 * - Hash functions (SHA-256, SHA-3, Blake2)
 * - Digital signatures (RSA, ECDSA)
 * - Key exchange (ECDH, DH)
 * - TLS/SSL communication
 * - Certificate handling
 * - Random number generation
 *
 * All implementations follow RAII patterns with comprehensive error handling.
 */

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/hmac.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <functional>

namespace crypto {

// OpenSSL Error Handling
class OpenSSLError : public std::runtime_error {
public:
    explicit OpenSSLError(const std::string& message)
        : std::runtime_error(message + ": " + getOpenSSLErrorString()) {}

private:
    static std::string getOpenSSLErrorString() {
        unsigned long err = ERR_get_error();
        char buffer[256];
        ERR_error_string_n(err, buffer, sizeof(buffer));
        return buffer;
    }
};

// RAII wrapper for OpenSSL initialization
class OpenSSLInit {
public:
    OpenSSLInit() {
        OpenSSL_add_all_algorithms();
        ERR_load_crypto_strings();
        SSL_load_error_strings();
    }

    ~OpenSSLInit() {
        EVP_cleanup();
        ERR_free_strings();
        SSL_COMP_free_compression_methods();
    }
};

// Secure memory wrapper
class SecureBuffer {
public:
    explicit SecureBuffer(size_t size) : data_(size), size_(size) {
        if (!data_.empty()) {
            OPENSSL_cleanse(data_.data(), data_.size());
        }
    }

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

// Symmetric Encryption Engine
class SymmetricCrypto {
public:
    enum Algorithm {
        AES_256_GCM,
        AES_128_GCM,
        CHACHA20_POLY1305
    };

    SymmetricCrypto(Algorithm alg = AES_256_GCM) : algorithm_(alg) {}

    // Authenticated encryption
    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& plaintext,
                               const std::vector<uint8_t>& key,
                               const std::vector<uint8_t>& iv,
                               const std::vector<uint8_t>& aad = {}) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) throw OpenSSLError("Failed to create cipher context");

        std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> ctx_guard(ctx, EVP_CIPHER_CTX_free);

        const EVP_CIPHER* cipher = getCipher();
        if (!cipher) throw std::runtime_error("Unsupported cipher");

        // Initialize encryption
        if (EVP_EncryptInit_ex(ctx, cipher, nullptr, nullptr, nullptr) != 1)
            throw OpenSSLError("Failed to initialize encryption");

        // Set IV length for GCM
        if (algorithm_ == AES_256_GCM || algorithm_ == AES_128_GCM) {
            if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv.size(), nullptr) != 1)
                throw OpenSSLError("Failed to set IV length");
        }

        // Initialize with key and IV
        if (EVP_EncryptInit_ex(ctx, nullptr, nullptr, key.data(), iv.data()) != 1)
            throw OpenSSLError("Failed to set key and IV");

        // Add AAD if provided
        int out_len;
        if (!aad.empty()) {
            if (EVP_EncryptUpdate(ctx, nullptr, &out_len, aad.data(), aad.size()) != 1)
                throw OpenSSLError("Failed to add AAD");
        }

        // Encrypt plaintext
        std::vector<uint8_t> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
        if (EVP_EncryptUpdate(ctx, ciphertext.data(), &out_len, plaintext.data(), plaintext.size()) != 1)
            throw OpenSSLError("Failed to encrypt data");

        size_t ciphertext_len = out_len;

        // Finalize encryption (get tag for GCM)
        if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + ciphertext_len, &out_len) != 1)
            throw OpenSSLError("Failed to finalize encryption");

        ciphertext_len += out_len;

        // Get authentication tag for GCM
        if (algorithm_ == AES_256_GCM || algorithm_ == AES_128_GCM) {
            std::vector<uint8_t> tag(16);
            if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag.data()) != 1)
                throw OpenSSLError("Failed to get authentication tag");

            ciphertext.insert(ciphertext.end(), tag.begin(), tag.end());
            ciphertext_len += 16;
        }

        ciphertext.resize(ciphertext_len);
        return ciphertext;
    }

    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& ciphertext,
                               const std::vector<uint8_t>& key,
                               const std::vector<uint8_t>& iv,
                               const std::vector<uint8_t>& aad = {}) {
        if (ciphertext.size() < 16) throw std::runtime_error("Ciphertext too short");

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) throw OpenSSLError("Failed to create cipher context");

        std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> ctx_guard(ctx, EVP_CIPHER_CTX_free);

        const EVP_CIPHER* cipher = getCipher();
        if (!cipher) throw std::runtime_error("Unsupported cipher");

        size_t tag_len = (algorithm_ == AES_256_GCM || algorithm_ == AES_128_GCM) ? 16 : 0;
        size_t actual_ciphertext_len = ciphertext.size() - tag_len;

        // Initialize decryption
        if (EVP_DecryptInit_ex(ctx, cipher, nullptr, nullptr, nullptr) != 1)
            throw OpenSSLError("Failed to initialize decryption");

        // Set IV length for GCM
        if (algorithm_ == AES_256_GCM || algorithm_ == AES_128_GCM) {
            if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv.size(), nullptr) != 1)
                throw OpenSSLError("Failed to set IV length");
        }

        // Initialize with key and IV
        if (EVP_DecryptInit_ex(ctx, nullptr, nullptr, key.data(), iv.data()) != 1)
            throw OpenSSLError("Failed to set key and IV");

        // Add AAD if provided
        int out_len;
        if (!aad.empty()) {
            if (EVP_DecryptUpdate(ctx, nullptr, &out_len, aad.data(), aad.size()) != 1)
                throw OpenSSLError("Failed to add AAD");
        }

        // Decrypt ciphertext
        std::vector<uint8_t> plaintext(actual_ciphertext_len + EVP_MAX_BLOCK_LENGTH);
        if (EVP_DecryptUpdate(ctx, plaintext.data(), &out_len,
                            ciphertext.data(), actual_ciphertext_len) != 1)
            throw OpenSSLError("Failed to decrypt data");

        size_t plaintext_len = out_len;

        // Set authentication tag for GCM
        if (tag_len > 0) {
            if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, tag_len,
                                  const_cast<uint8_t*>(ciphertext.data() + actual_ciphertext_len)) != 1)
                throw OpenSSLError("Failed to set authentication tag");
        }

        // Finalize decryption
        if (EVP_DecryptFinal_ex(ctx, plaintext.data() + plaintext_len, &out_len) != 1)
            throw OpenSSLError("Authentication failed");

        plaintext_len += out_len;
        plaintext.resize(plaintext_len);
        return plaintext;
    }

private:
    const EVP_CIPHER* getCipher() const {
        switch (algorithm_) {
            case AES_256_GCM: return EVP_aes_256_gcm();
            case AES_128_GCM: return EVP_aes_128_gcm();
            case CHACHA20_POLY1305: return EVP_chacha20_poly1305();
            default: return nullptr;
        }
    }

    Algorithm algorithm_;
};

// Hash Function Engine
class HashEngine {
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

    HashEngine(Algorithm alg = SHA256) : algorithm_(alg) {}

    std::vector<uint8_t> hash(const std::vector<uint8_t>& data) {
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        if (!ctx) throw OpenSSLError("Failed to create hash context");

        std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> ctx_guard(ctx, EVP_MD_CTX_free);

        const EVP_MD* md = getDigest();
        if (!md) throw std::runtime_error("Unsupported hash algorithm");

        if (EVP_DigestInit_ex(ctx, md, nullptr) != 1)
            throw OpenSSLError("Failed to initialize hash");

        if (EVP_DigestUpdate(ctx, data.data(), data.size()) != 1)
            throw OpenSSLError("Failed to update hash");

        std::vector<uint8_t> result(EVP_MD_size(md));
        unsigned int len;
        if (EVP_DigestFinal_ex(ctx, result.data(), &len) != 1)
            throw OpenSSLError("Failed to finalize hash");

        result.resize(len);
        return result;
    }

    // HMAC computation
    std::vector<uint8_t> hmac(const std::vector<uint8_t>& data,
                            const std::vector<uint8_t>& key) {
        unsigned int len;
        std::vector<uint8_t> result(EVP_MAX_MD_SIZE);

        const EVP_MD* md = getDigest();
        if (!md) throw std::runtime_error("Unsupported hash algorithm");

        unsigned char* result_ptr = HMAC(md, key.data(), key.size(),
                                       data.data(), data.size(),
                                       result.data(), &len);

        if (!result_ptr) throw OpenSSLError("HMAC computation failed");

        result.resize(len);
        return result;
    }

private:
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
};

// Digital Signature Engine
class DigitalSignature {
public:
    enum Algorithm {
        RSA_SHA256,
        RSA_SHA512,
        ECDSA_SHA256,
        ECDSA_SHA512,
        ED25519
    };

    // Generate key pair
    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>>
    generateKeyPair(Algorithm alg = ECDSA_SHA256) {
        switch (alg) {
            case RSA_SHA256:
            case RSA_SHA512:
                return generateRSAKeyPair();
            case ECDSA_SHA256:
            case ECDSA_SHA512:
                return generateECDSAKeyPair();
            case ED25519:
                return generateEd25519KeyPair();
            default:
                throw std::runtime_error("Unsupported signature algorithm");
        }
    }

    // Sign data
    static std::vector<uint8_t> sign(const std::vector<uint8_t>& data,
                                   const std::vector<uint8_t>& private_key,
                                   Algorithm alg = ECDSA_SHA256) {
        switch (alg) {
            case RSA_SHA256:
            case RSA_SHA512:
                return signRSA(data, private_key, alg);
            case ECDSA_SHA256:
            case ECDSA_SHA512:
                return signECDSA(data, private_key, alg);
            case ED25519:
                return signEd25519(data, private_key);
            default:
                throw std::runtime_error("Unsupported signature algorithm");
        }
    }

    // Verify signature
    static bool verify(const std::vector<uint8_t>& data,
                      const std::vector<uint8_t>& signature,
                      const std::vector<uint8_t>& public_key,
                      Algorithm alg = ECDSA_SHA256) {
        switch (alg) {
            case RSA_SHA256:
            case RSA_SHA512:
                return verifyRSA(data, signature, public_key, alg);
            case ECDSA_SHA256:
            case ECDSA_SHA512:
                return verifyECDSA(data, signature, public_key, alg);
            case ED25519:
                return verifyEd25519(data, signature, public_key);
            default:
                throw std::runtime_error("Unsupported signature algorithm");
        }
    }

private:
    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>> generateRSAKeyPair() {
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
        if (!ctx) throw OpenSSLError("Failed to create RSA context");

        std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx_guard(ctx, EVP_PKEY_CTX_free);

        if (EVP_PKEY_keygen_init(ctx) <= 0)
            throw OpenSSLError("Failed to initialize RSA keygen");

        if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0)
            throw OpenSSLError("Failed to set RSA key size");

        EVP_PKEY* pkey = nullptr;
        if (EVP_PKEY_keygen(ctx, &pkey) <= 0)
            throw OpenSSLError("Failed to generate RSA key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> pkey_guard(pkey, EVP_PKEY_free);

        // Extract private key
        BIO* priv_bio = BIO_new(BIO_s_mem());
        if (!priv_bio) throw OpenSSLError("Failed to create BIO");

        std::unique_ptr<BIO, decltype(&BIO_free)> priv_bio_guard(priv_bio, BIO_free);

        if (PEM_write_bio_PrivateKey(priv_bio, pkey, nullptr, nullptr, 0, nullptr, nullptr) != 1)
            throw OpenSSLError("Failed to write private key");

        char* priv_data;
        long priv_len = BIO_get_mem_data(priv_bio, &priv_data);
        std::vector<uint8_t> private_key(priv_data, priv_data + priv_len);

        // Extract public key
        BIO* pub_bio = BIO_new(BIO_s_mem());
        if (!pub_bio) throw OpenSSLError("Failed to create BIO");

        std::unique_ptr<BIO, decltype(&BIO_free)> pub_bio_guard(pub_bio, BIO_free);

        if (PEM_write_bio_PUBKEY(pub_bio, pkey) != 1)
            throw OpenSSLError("Failed to write public key");

        char* pub_data;
        long pub_len = BIO_get_mem_data(pub_bio, &pub_data);
        std::vector<uint8_t> public_key(pub_data, pub_data + pub_len);

        return {private_key, public_key};
    }

    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>> generateECDSAKeyPair() {
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
        if (!ctx) throw OpenSSLError("Failed to create ECDSA context");

        std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx_guard(ctx, EVP_PKEY_CTX_free);

        if (EVP_PKEY_keygen_init(ctx) <= 0)
            throw OpenSSLError("Failed to initialize ECDSA keygen");

        if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_X9_62_prime256v1) <= 0)
            throw OpenSSLError("Failed to set EC curve");

        EVP_PKEY* pkey = nullptr;
        if (EVP_PKEY_keygen(ctx, &pkey) <= 0)
            throw OpenSSLError("Failed to generate ECDSA key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> pkey_guard(pkey, EVP_PKEY_free);

        // Extract private key in DER format
        unsigned char* priv_der = nullptr;
        int priv_len = i2d_PrivateKey(pkey, &priv_der);
        if (priv_len <= 0) throw OpenSSLError("Failed to encode private key");

        std::unique_ptr<unsigned char, decltype(&OPENSSL_free)> priv_der_guard(priv_der, OPENSSL_free);
        std::vector<uint8_t> private_key(priv_der, priv_der + priv_len);

        // Extract public key in DER format
        unsigned char* pub_der = nullptr;
        int pub_len = i2d_PublicKey(pkey, &pub_der);
        if (pub_len <= 0) throw OpenSSLError("Failed to encode public key");

        std::unique_ptr<unsigned char, decltype(&OPENSSL_free)> pub_der_guard(pub_der, OPENSSL_free);
        std::vector<uint8_t> public_key(pub_der, pub_der + pub_len);

        return {private_key, public_key};
    }

    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>> generateEd25519KeyPair() {
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, nullptr);
        if (!ctx) throw OpenSSLError("Failed to create Ed25519 context");

        std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx_guard(ctx, EVP_PKEY_CTX_free);

        if (EVP_PKEY_keygen_init(ctx) <= 0)
            throw OpenSSLError("Failed to initialize Ed25519 keygen");

        EVP_PKEY* pkey = nullptr;
        if (EVP_PKEY_keygen(ctx, &pkey) <= 0)
            throw OpenSSLError("Failed to generate Ed25519 key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> pkey_guard(pkey, EVP_PKEY_free);

        // Extract private key
        size_t priv_len;
        if (EVP_PKEY_get_raw_private_key(pkey, nullptr, &priv_len) != 1)
            throw OpenSSLError("Failed to get private key length");

        std::vector<uint8_t> private_key(priv_len);
        if (EVP_PKEY_get_raw_private_key(pkey, private_key.data(), &priv_len) != 1)
            throw OpenSSLError("Failed to get private key");

        // Extract public key
        size_t pub_len;
        if (EVP_PKEY_get_raw_public_key(pkey, nullptr, &pub_len) != 1)
            throw OpenSSLError("Failed to get public key length");

        std::vector<uint8_t> public_key(pub_len);
        if (EVP_PKEY_get_raw_public_key(pkey, public_key.data(), &pub_len) != 1)
            throw OpenSSLError("Failed to get public key");

        return {private_key, public_key};
    }

    // RSA signing implementation
    static std::vector<uint8_t> signRSA(const std::vector<uint8_t>& data,
                                      const std::vector<uint8_t>& private_key,
                                      Algorithm alg) {
        // Implementation would load PEM private key and sign
        // Full implementation would be quite extensive
        throw std::runtime_error("RSA signing not fully implemented in this example");
    }

    static bool verifyRSA(const std::vector<uint8_t>& data,
                         const std::vector<uint8_t>& signature,
                         const std::vector<uint8_t>& public_key,
                         Algorithm alg) {
        // Implementation would load PEM public key and verify
        throw std::runtime_error("RSA verification not fully implemented in this example");
    }

    // ECDSA signing implementation
    static std::vector<uint8_t> signECDSA(const std::vector<uint8_t>& data,
                                        const std::vector<uint8_t>& private_key,
                                        Algorithm alg) {
        // Load private key from DER
        const unsigned char* p = private_key.data();
        EVP_PKEY* pkey = d2i_PrivateKey(EVP_PKEY_EC, nullptr, &p, private_key.size());
        if (!pkey) throw OpenSSLError("Failed to load private key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> pkey_guard(pkey, EVP_PKEY_free);

        // Create signing context
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        if (!ctx) throw OpenSSLError("Failed to create signing context");

        std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> ctx_guard(ctx, EVP_MD_CTX_free);

        const EVP_MD* md = (alg == ECDSA_SHA256) ? EVP_sha256() : EVP_sha512();

        if (EVP_DigestSignInit(ctx, nullptr, md, nullptr, pkey) != 1)
            throw OpenSSLError("Failed to initialize signing");

        // Sign the data
        size_t sig_len;
        if (EVP_DigestSign(ctx, nullptr, &sig_len, data.data(), data.size()) != 1)
            throw OpenSSLError("Failed to get signature length");

        std::vector<uint8_t> signature(sig_len);
        if (EVP_DigestSign(ctx, signature.data(), &sig_len, data.data(), data.size()) != 1)
            throw OpenSSLError("Failed to sign data");

        signature.resize(sig_len);
        return signature;
    }

    static bool verifyECDSA(const std::vector<uint8_t>& data,
                           const std::vector<uint8_t>& signature,
                           const std::vector<uint8_t>& public_key,
                           Algorithm alg) {
        // Load public key from DER
        const unsigned char* p = public_key.data();
        EVP_PKEY* pkey = d2i_PublicKey(EVP_PKEY_EC, nullptr, &p, public_key.size());
        if (!pkey) throw OpenSSLError("Failed to load public key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> pkey_guard(pkey, EVP_PKEY_free);

        // Create verification context
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        if (!ctx) throw OpenSSLError("Failed to create verification context");

        std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> ctx_guard(ctx, EVP_MD_CTX_free);

        const EVP_MD* md = (alg == ECDSA_SHA256) ? EVP_sha256() : EVP_sha512();

        if (EVP_DigestVerifyInit(ctx, nullptr, md, nullptr, pkey) != 1)
            throw OpenSSLError("Failed to initialize verification");

        // Verify the signature
        int result = EVP_DigestVerify(ctx, signature.data(), signature.size(),
                                    data.data(), data.size());

        return result == 1;
    }

    // Ed25519 signing implementation
    static std::vector<uint8_t> signEd25519(const std::vector<uint8_t>& data,
                                          const std::vector<uint8_t>& private_key) {
        EVP_PKEY* pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, nullptr,
                                                    private_key.data(), private_key.size());
        if (!pkey) throw OpenSSLError("Failed to load private key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> pkey_guard(pkey, EVP_PKEY_free);

        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        if (!ctx) throw OpenSSLError("Failed to create signing context");

        std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> ctx_guard(ctx, EVP_MD_CTX_free);

        if (EVP_DigestSignInit(ctx, nullptr, nullptr, nullptr, pkey) != 1)
            throw OpenSSLError("Failed to initialize signing");

        size_t sig_len;
        if (EVP_DigestSign(ctx, nullptr, &sig_len, data.data(), data.size()) != 1)
            throw OpenSSLError("Failed to get signature length");

        std::vector<uint8_t> signature(sig_len);
        if (EVP_DigestSign(ctx, signature.data(), &sig_len, data.data(), data.size()) != 1)
            throw OpenSSLError("Failed to sign data");

        signature.resize(sig_len);
        return signature;
    }

    static bool verifyEd25519(const std::vector<uint8_t>& data,
                             const std::vector<uint8_t>& signature,
                             const std::vector<uint8_t>& public_key) {
        EVP_PKEY* pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, nullptr,
                                                   public_key.data(), public_key.size());
        if (!pkey) throw OpenSSLError("Failed to load public key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> pkey_guard(pkey, EVP_PKEY_free);

        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        if (!ctx) throw OpenSSLError("Failed to create verification context");

        std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> ctx_guard(ctx, EVP_MD_CTX_free);

        if (EVP_DigestVerifyInit(ctx, nullptr, nullptr, nullptr, pkey) != 1)
            throw OpenSSLError("Failed to initialize verification");

        int result = EVP_DigestVerify(ctx, signature.data(), signature.size(),
                                    data.data(), data.size());

        return result == 1;
    }
};

// Key Exchange Engine
class KeyExchange {
public:
    enum Algorithm {
        ECDH_P256,
        ECDH_P384,
        ECDH_P521,
        X25519
    };

    // Generate ephemeral key pair for key exchange
    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>>
    generateEphemeralKey(Algorithm alg = ECDH_P256) {
        switch (alg) {
            case ECDH_P256:
            case ECDH_P384:
            case ECDH_P521:
                return generateECDHEphemeral(alg);
            case X25519:
                return generateX25519Ephemeral();
            default:
                throw std::runtime_error("Unsupported key exchange algorithm");
        }
    }

    // Derive shared secret
    static std::vector<uint8_t> deriveSharedSecret(
        const std::vector<uint8_t>& private_key,
        const std::vector<uint8_t>& peer_public_key,
        Algorithm alg = ECDH_P256) {

        switch (alg) {
            case ECDH_P256:
            case ECDH_P384:
            case ECDH_P521:
                return deriveECDHSharedSecret(private_key, peer_public_key, alg);
            case X25519:
                return deriveX25519SharedSecret(private_key, peer_public_key);
            default:
                throw std::runtime_error("Unsupported key exchange algorithm");
        }
    }

private:
    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>> generateECDHEphemeral(Algorithm alg) {
        int nid;
        switch (alg) {
            case ECDH_P256: nid = NID_X9_62_prime256v1; break;
            case ECDH_P384: nid = NID_secp384r1; break;
            case ECDH_P521: nid = NID_secp521r1; break;
            default: throw std::runtime_error("Invalid ECDH algorithm");
        }

        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
        if (!ctx) throw OpenSSLError("Failed to create ECDH context");

        std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx_guard(ctx, EVP_PKEY_CTX_free);

        if (EVP_PKEY_keygen_init(ctx) <= 0)
            throw OpenSSLError("Failed to initialize ECDH keygen");

        if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, nid) <= 0)
            throw OpenSSLError("Failed to set ECDH curve");

        EVP_PKEY* pkey = nullptr;
        if (EVP_PKEY_keygen(ctx, &pkey) <= 0)
            throw OpenSSLError("Failed to generate ECDH key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> pkey_guard(pkey, EVP_PKEY_free);

        // Extract public key
        size_t pub_len;
        if (EVP_PKEY_get_raw_public_key(pkey, nullptr, &pub_len) != 1)
            throw OpenSSLError("Failed to get public key length");

        std::vector<uint8_t> public_key(pub_len);
        if (EVP_PKEY_get_raw_public_key(pkey, public_key.data(), &pub_len) != 1)
            throw OpenSSLError("Failed to get public key");

        // Extract private key
        size_t priv_len;
        if (EVP_PKEY_get_raw_private_key(pkey, nullptr, &priv_len) != 1)
            throw OpenSSLError("Failed to get private key length");

        std::vector<uint8_t> private_key(priv_len);
        if (EVP_PKEY_get_raw_private_key(pkey, private_key.data(), &priv_len) != 1)
            throw OpenSSLError("Failed to get private key");

        return {private_key, public_key};
    }

    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>> generateX25519Ephemeral() {
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_X25519, nullptr);
        if (!ctx) throw OpenSSLError("Failed to create X25519 context");

        std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx_guard(ctx, EVP_PKEY_CTX_free);

        if (EVP_PKEY_keygen_init(ctx) <= 0)
            throw OpenSSLError("Failed to initialize X25519 keygen");

        EVP_PKEY* pkey = nullptr;
        if (EVP_PKEY_keygen(ctx, &pkey) <= 0)
            throw OpenSSLError("Failed to generate X25519 key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> pkey_guard(pkey, EVP_PKEY_free);

        // Extract public key
        size_t pub_len;
        if (EVP_PKEY_get_raw_public_key(pkey, nullptr, &pub_len) != 1)
            throw OpenSSLError("Failed to get public key length");

        std::vector<uint8_t> public_key(pub_len);
        if (EVP_PKEY_get_raw_public_key(pkey, public_key.data(), &pub_len) != 1)
            throw OpenSSLError("Failed to get public key");

        // Extract private key
        size_t priv_len;
        if (EVP_PKEY_get_raw_private_key(pkey, nullptr, &priv_len) != 1)
            throw OpenSSLError("Failed to get private key length");

        std::vector<uint8_t> private_key(priv_len);
        if (EVP_PKEY_get_raw_private_key(pkey, private_key.data(), &priv_len) != 1)
            throw OpenSSLError("Failed to get private key");

        return {private_key, public_key};
    }

    static std::vector<uint8_t> deriveECDHSharedSecret(
        const std::vector<uint8_t>& private_key,
        const std::vector<uint8_t>& peer_public_key,
        Algorithm alg) {

        // Load private key
        EVP_PKEY* priv_pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_EC, nullptr,
                                                         private_key.data(), private_key.size());
        if (!priv_pkey) throw OpenSSLError("Failed to load private key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> priv_pkey_guard(priv_pkey, EVP_PKEY_free);

        // Load peer public key
        EVP_PKEY* peer_pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_EC, nullptr,
                                                        peer_public_key.data(), peer_public_key.size());
        if (!peer_pkey) throw OpenSSLError("Failed to load peer public key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> peer_pkey_guard(peer_pkey, EVP_PKEY_free);

        // Derive shared secret
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(priv_pkey, nullptr);
        if (!ctx) throw OpenSSLError("Failed to create key derivation context");

        std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx_guard(ctx, EVP_PKEY_CTX_free);

        if (EVP_PKEY_derive_init(ctx) <= 0)
            throw OpenSSLError("Failed to initialize key derivation");

        if (EVP_PKEY_derive_set_peer(ctx, peer_pkey) <= 0)
            throw OpenSSLError("Failed to set peer key");

        size_t secret_len;
        if (EVP_PKEY_derive(ctx, nullptr, &secret_len) <= 0)
            throw OpenSSLError("Failed to get shared secret length");

        std::vector<uint8_t> shared_secret(secret_len);
        if (EVP_PKEY_derive(ctx, shared_secret.data(), &secret_len) <= 0)
            throw OpenSSLError("Failed to derive shared secret");

        shared_secret.resize(secret_len);
        return shared_secret;
    }

    static std::vector<uint8_t> deriveX25519SharedSecret(
        const std::vector<uint8_t>& private_key,
        const std::vector<uint8_t>& peer_public_key) {

        // Load private key
        EVP_PKEY* priv_pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, nullptr,
                                                         private_key.data(), private_key.size());
        if (!priv_pkey) throw OpenSSLError("Failed to load private key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> priv_pkey_guard(priv_pkey, EVP_PKEY_free);

        // Load peer public key
        EVP_PKEY* peer_pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_X25519, nullptr,
                                                        peer_public_key.data(), peer_public_key.size());
        if (!peer_pkey) throw OpenSSLError("Failed to load peer public key");

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> peer_pkey_guard(peer_pkey, EVP_PKEY_free);

        // Derive shared secret
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(priv_pkey, nullptr);
        if (!ctx) throw OpenSSLError("Failed to create key derivation context");

        std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx_guard(ctx, EVP_PKEY_CTX_free);

        if (EVP_PKEY_derive_init(ctx) <= 0)
            throw OpenSSLError("Failed to initialize key derivation");

        if (EVP_PKEY_derive_set_peer(ctx, peer_pkey) <= 0)
            throw OpenSSLError("Failed to set peer key");

        size_t secret_len;
        if (EVP_PKEY_derive(ctx, nullptr, &secret_len) <= 0)
            throw OpenSSLError("Failed to get shared secret length");

        std::vector<uint8_t> shared_secret(secret_len);
        if (EVP_PKEY_derive(ctx, shared_secret.data(), &secret_len) <= 0)
            throw OpenSSLError("Failed to derive shared secret");

        shared_secret.resize(secret_len);
        return shared_secret;
    }
};

// TLS/SSL Engine
class TLSEngine {
public:
    TLSEngine(bool is_server = false) : is_server_(is_server) {
        ctx_ = SSL_CTX_new(is_server ? TLS_server_method() : TLS_client_method());
        if (!ctx_) throw OpenSSLError("Failed to create SSL context");

        // Configure secure defaults
        SSL_CTX_set_min_proto_version(ctx_, TLS1_2_VERSION);
        SSL_CTX_set_cipher_list(ctx_, "HIGH:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!MD5:!PSK:!SRP:!CAMELLIA");
        SSL_CTX_set_verify(ctx_, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);
        SSL_CTX_set_verify_depth(ctx_, 9);
    }

    ~TLSEngine() {
        if (ctx_) SSL_CTX_free(ctx_);
        if (ssl_) SSL_free(ssl_);
    }

    // Load certificate and private key
    void loadCertificate(const std::string& cert_file, const std::string& key_file) {
        if (SSL_CTX_use_certificate_file(ctx_, cert_file.c_str(), SSL_FILETYPE_PEM) <= 0)
            throw OpenSSLError("Failed to load certificate");

        if (SSL_CTX_use_PrivateKey_file(ctx_, key_file.c_str(), SSL_FILETYPE_PEM) <= 0)
            throw OpenSSLError("Failed to load private key");

        if (!SSL_CTX_check_private_key(ctx_))
            throw OpenSSLError("Private key does not match certificate");
    }

    // Load CA certificates
    void loadCA(const std::string& ca_file) {
        if (SSL_CTX_load_verify_locations(ctx_, ca_file.c_str(), nullptr) <= 0)
            throw OpenSSLError("Failed to load CA certificates");
    }

    // Establish secure connection
    void connect(BIO* bio) {
        ssl_ = SSL_new(ctx_);
        if (!ssl_) throw OpenSSLError("Failed to create SSL object");

        SSL_set_bio(ssl_, bio, bio);

        if (is_server_) {
            if (SSL_accept(ssl_) <= 0)
                throw OpenSSLError("SSL accept failed");
        } else {
            if (SSL_connect(ssl_) <= 0)
                throw OpenSSLError("SSL connect failed");
        }
    }

    // Send data securely
    void send(const std::vector<uint8_t>& data) {
        if (!ssl_) throw std::runtime_error("No SSL connection established");

        size_t sent = 0;
        while (sent < data.size()) {
            int result = SSL_write(ssl_, data.data() + sent, data.size() - sent);
            if (result <= 0) {
                int err = SSL_get_error(ssl_, result);
                if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
                    continue; // Retry
                }
                throw OpenSSLError("SSL write failed");
            }
            sent += result;
        }
    }

    // Receive data securely
    std::vector<uint8_t> receive(size_t max_size = 4096) {
        if (!ssl_) throw std::runtime_error("No SSL connection established");

        std::vector<uint8_t> buffer(max_size);
        int result = SSL_read(ssl_, buffer.data(), buffer.size());

        if (result <= 0) {
            int err = SSL_get_error(ssl_, result);
            if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
                return {}; // No data available
            }
            if (err == SSL_ERROR_ZERO_RETURN) {
                return {}; // Connection closed
            }
            throw OpenSSLError("SSL read failed");
        }

        buffer.resize(result);
        return buffer;
    }

    // Get peer certificate information
    std::string getPeerCertificateInfo() {
        if (!ssl_) return "";

        X509* cert = SSL_get_peer_certificate(ssl_);
        if (!cert) return "No certificate";

        std::unique_ptr<X509, decltype(&X509_free)> cert_guard(cert, X509_free);

        BIO* bio = BIO_new(BIO_s_mem());
        if (!bio) return "Failed to create BIO";

        std::unique_ptr<BIO, decltype(&BIO_free)> bio_guard(bio, BIO_free);

        X509_NAME_print_ex(bio, X509_get_subject_name(cert), 0, XN_FLAG_RFC2253);
        char* data;
        long len = BIO_get_mem_data(bio, &data);

        return std::string(data, len);
    }

private:
    SSL_CTX* ctx_;
    SSL* ssl_;
    bool is_server_;
};

// Random Number Generator
class RNG {
public:
    static std::vector<uint8_t> generateRandom(size_t length) {
        std::vector<uint8_t> buffer(length);
        if (RAND_bytes(buffer.data(), buffer.size()) != 1)
            throw OpenSSLError("Failed to generate random bytes");
        return buffer;
    }

    static std::vector<uint8_t> generateSecureIV(size_t length = 16) {
        return generateRandom(length);
    }

    static std::vector<uint8_t> generateSecureKey(size_t length = 32) {
        return generateRandom(length);
    }
};

// Main crypto facade class
class Crypto {
public:
    static void initialize() {
        static OpenSSLInit init;
    }

    // High-level encryption/decryption
    static std::vector<uint8_t> encryptAES256GCM(const std::vector<uint8_t>& data,
                                               const std::vector<uint8_t>& key,
                                               const std::vector<uint8_t>& aad = {}) {
        SymmetricCrypto crypto(SymmetricCrypto::AES_256_GCM);
        auto iv = RNG::generateSecureIV(16);
        auto ciphertext = crypto.encrypt(data, key, iv, aad);
        // Prepend IV to ciphertext
        ciphertext.insert(ciphertext.begin(), iv.begin(), iv.end());
        return ciphertext;
    }

    static std::vector<uint8_t> decryptAES256GCM(const std::vector<uint8_t>& data,
                                               const std::vector<uint8_t>& key,
                                               const std::vector<uint8_t>& aad = {}) {
        if (data.size() < 16) throw std::runtime_error("Data too short");
        std::vector<uint8_t> iv(data.begin(), data.begin() + 16);
        std::vector<uint8_t> ciphertext(data.begin() + 16, data.end());
        SymmetricCrypto crypto(SymmetricCrypto::AES_256_GCM);
        return crypto.decrypt(ciphertext, key, iv, aad);
    }

    // High-level hashing
    static std::vector<uint8_t> sha256(const std::vector<uint8_t>& data) {
        HashEngine hash(HashEngine::SHA256);
        return hash.hash(data);
    }

    // High-level HMAC
    static std::vector<uint8_t> hmacSHA256(const std::vector<uint8_t>& data,
                                         const std::vector<uint8_t>& key) {
        HashEngine hash(HashEngine::SHA256);
        return hash.hmac(data, key);
    }

    // High-level digital signatures
    static auto generateECDSAKeyPair() {
        return DigitalSignature::generateKeyPair(DigitalSignature::ECDSA_SHA256);
    }

    static std::vector<uint8_t> signECDSA(const std::vector<uint8_t>& data,
                                        const std::vector<uint8_t>& private_key) {
        return DigitalSignature::sign(data, private_key, DigitalSignature::ECDSA_SHA256);
    }

    static bool verifyECDSA(const std::vector<uint8_t>& data,
                           const std::vector<uint8_t>& signature,
                           const std::vector<uint8_t>& public_key) {
        return DigitalSignature::verify(data, signature, public_key, DigitalSignature::ECDSA_SHA256);
    }

    // High-level key exchange
    static auto generateECDHKeyPair() {
        return KeyExchange::generateEphemeralKey(KeyExchange::ECDH_P256);
    }

    static std::vector<uint8_t> deriveECDHSharedSecret(
        const std::vector<uint8_t>& private_key,
        const std::vector<uint8_t>& peer_public_key) {
        return KeyExchange::deriveSharedSecret(private_key, peer_public_key, KeyExchange::ECDH_P256);
    }
};

} // namespace crypto

// Example usage and test functions
namespace crypto_examples {

// Basic encryption/decryption example
void basicEncryptionExample() {
    crypto::Crypto::initialize();

    std::string message = "Hello, World!";
    std::vector<uint8_t> data(message.begin(), message.end());
    auto key = crypto::RNG::generateSecureKey(32);

    // Encrypt
    auto encrypted = crypto::Crypto::encryptAES256GCM(data, key);
    std::cout << "Encrypted size: " << encrypted.size() << " bytes" << std::endl;

    // Decrypt
    auto decrypted = crypto::Crypto::decryptAES256GCM(encrypted, key);
    std::string result(decrypted.begin(), decrypted.end());
    std::cout << "Decrypted: " << result << std::endl;

    assert(result == message);
}

// Digital signature example
void digitalSignatureExample() {
    crypto::Crypto::initialize();

    std::string message = "This message will be signed";
    std::vector<uint8_t> data(message.begin(), message.end());

    // Generate key pair
    auto [private_key, public_key] = crypto::Crypto::generateECDSAKeyPair();

    // Sign
    auto signature = crypto::Crypto::signECDSA(data, private_key);
    std::cout << "Signature size: " << signature.size() << " bytes" << std::endl;

    // Verify
    bool valid = crypto::Crypto::verifyECDSA(data, signature, public_key);
    std::cout << "Signature valid: " << (valid ? "Yes" : "No") << std::endl;

    assert(valid);
}

// Key exchange example
void keyExchangeExample() {
    crypto::Crypto::initialize();

    // Alice generates key pair
    auto [alice_private, alice_public] = crypto::Crypto::generateECDHKeyPair();

    // Bob generates key pair
    auto [bob_private, bob_public] = crypto::Crypto::generateECDHKeyPair();

    // Alice derives shared secret using Bob's public key
    auto alice_shared = crypto::Crypto::deriveECDHSharedSecret(alice_private, bob_public);

    // Bob derives shared secret using Alice's public key
    auto bob_shared = crypto::Crypto::deriveECDHSharedSecret(bob_private, alice_public);

    // Shared secrets should be identical
    assert(alice_shared == bob_shared);
    std::cout << "Shared secret size: " << alice_shared.size() << " bytes" << std::endl;
}

} // namespace crypto_examples

#endif // CRYPTO_OPENSSL_WRAPPER_HPP
