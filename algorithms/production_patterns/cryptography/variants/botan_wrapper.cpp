/**
 * Botan Cryptography Wrapper - Production Implementation
 *
 * This file provides production-grade wrappers around Botan for:
 * - Symmetric encryption (AES, ChaCha, Serpent)
 * - Authenticated encryption (AES-GCM, ChaCha20-Poly1305)
 * - Hash functions (SHA-256, SHA-3, Blake2)
 * - Message Authentication Codes (HMAC, CMAC, GMAC)
 * - Digital signatures (RSA, ECDSA, Ed25519)
 * - Key exchange (ECDH, X25519)
 * - Password hashing (PBKDF2, Argon2, Scrypt)
 * - Post-quantum cryptography (Kyber, Dilithium)
 * - Random number generation
 *
 * Botan provides a comprehensive, clean C++ API with extensive algorithm support.
 */

#include <botan/auto_rng.h>
#include <botan/system_rng.h>
#include <botan/cipher_mode.h>
#include <botan/hash.h>
#include <botan/mac.h>
#include <botan/pubkey.h>
#include <botan/pk_keys.h>
#include <botan/pkcs8.h>
#include <botan/x509cert.h>
#include <botan/pwdhash.h>
#include <botan/kdf.h>
#include <botan/hex.h>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <cstring>

namespace botan {

// Botan initialization and error handling
class BotanInit {
public:
    BotanInit() {
        // Botan automatically initializes on first use
    }

    ~BotanInit() {
        // Botan cleanup is automatic
    }
};

class BotanError : public std::runtime_error {
public:
    explicit BotanError(const std::string& message)
        : std::runtime_error(message + ": " + Botan::get_last_error()) {}
};

// Secure buffer with automatic zeroing
class SecureBuffer {
public:
    explicit SecureBuffer(size_t size) : data_(size) {}

    ~SecureBuffer() {
        if (!data_.empty()) {
            Botan::secure_scrub_memory(data_.data(), data_.size());
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

// Symmetric Encryption
class SymmetricCipher {
public:
    enum Algorithm {
        AES_256_GCM,
        AES_128_GCM,
        CHACHA20_POLY1305,
        SERPENT_GCM,
        TWOFISH_GCM
    };

    SymmetricCipher(Algorithm alg = AES_256_GCM, bool encrypt = true)
        : algorithm_(alg), encrypt_mode_(encrypt) {
        initializeCipher();
    }

    void setKey(const std::vector<uint8_t>& key) {
        cipher_->set_key(key);
    }

    void setIV(const std::vector<uint8_t>& iv) {
        cipher_->start(iv);
    }

    std::vector<uint8_t> process(const std::vector<uint8_t>& data,
                               const std::vector<uint8_t>& aad = {}) {
        if (!aad.empty()) {
            cipher_->set_associated_data(aad);
        }

        cipher_->update(data);
        return cipher_->final();
    }

    void processInPlace(std::vector<uint8_t>& data,
                       const std::vector<uint8_t>& aad = {}) {
        if (!aad.empty()) {
            cipher_->set_associated_data(aad);
        }

        cipher_->update(data);
        auto final_block = cipher_->final();
        data.insert(data.end(), final_block.begin(), final_block.end());
    }

    static std::vector<uint8_t> generateKey(Algorithm alg = AES_256_GCM) {
        Botan::AutoSeeded_RNG rng;
        size_t key_size = keySize(alg);
        std::vector<uint8_t> key(key_size);
        rng.randomize(key.data(), key.size());
        return key;
    }

    static std::vector<uint8_t> generateIV(Algorithm alg = AES_256_GCM) {
        Botan::AutoSeeded_RNG rng;
        size_t iv_size = ivSize(alg);
        std::vector<uint8_t> iv(iv_size);
        rng.randomize(iv.data(), iv.size());
        return iv;
    }

    // High-level encrypt/decrypt functions
    static std::vector<uint8_t> encrypt(const std::vector<uint8_t>& plaintext,
                                      const std::vector<uint8_t>& key,
                                      const std::vector<uint8_t>& iv,
                                      Algorithm alg = AES_256_GCM,
                                      const std::vector<uint8_t>& aad = {}) {
        SymmetricCipher cipher(alg, true);
        cipher.setKey(key);
        cipher.setIV(iv);
        return cipher.process(plaintext, aad);
    }

    static std::vector<uint8_t> decrypt(const std::vector<uint8_t>& ciphertext,
                                      const std::vector<uint8_t>& key,
                                      const std::vector<uint8_t>& iv,
                                      Algorithm alg = AES_256_GCM,
                                      const std::vector<uint8_t>& aad = {}) {
        SymmetricCipher cipher(alg, false);
        cipher.setKey(key);
        cipher.setIV(iv);
        return cipher.process(ciphertext, aad);
    }

private:
    void initializeCipher() {
        std::string cipher_name = getCipherName();
        cipher_ = Botan::Cipher_Mode::create(cipher_name, encrypt_mode_ ?
                                           Botan::Cipher_Dir::Encryption :
                                           Botan::Cipher_Dir::Decryption);
        if (!cipher_) {
            throw BotanError("Failed to create cipher: " + cipher_name);
        }
    }

    std::string getCipherName() const {
        switch (algorithm_) {
            case AES_256_GCM: return "AES-256/GCM";
            case AES_128_GCM: return "AES-128/GCM";
            case CHACHA20_POLY1305: return "ChaCha20Poly1305";
            case SERPENT_GCM: return "Serpent/GCM";
            case TWOFISH_GCM: return "Twofish/GCM";
            default: return "AES-256/GCM";
        }
    }

    static size_t keySize(Algorithm alg) {
        switch (alg) {
            case AES_256_GCM: return 32;
            case AES_128_GCM: return 16;
            case CHACHA20_POLY1305: return 32;
            case SERPENT_GCM: return 32;
            case TWOFISH_GCM: return 32;
            default: return 32;
        }
    }

    static size_t ivSize(Algorithm alg) {
        switch (alg) {
            case AES_256_GCM: return 12;
            case AES_128_GCM: return 12;
            case CHACHA20_POLY1305: return 12;
            case SERPENT_GCM: return 12;
            case TWOFISH_GCM: return 12;
            default: return 12;
        }
    }

    Algorithm algorithm_;
    bool encrypt_mode_;
    std::unique_ptr<Botan::Cipher_Mode> cipher_;
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
        BLAKE2B_512,
        WHIRLPOOL
    };

    HashFunction(Algorithm alg = SHA256) : algorithm_(alg) {
        initializeHash();
    }

    void update(const std::vector<uint8_t>& data) {
        hash_->update(data);
    }

    void update(const uint8_t* data, size_t length) {
        hash_->update(data, length);
    }

    std::vector<uint8_t> finalize() {
        return hash_->final();
    }

    void reset() {
        hash_->reset();
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
        std::string hash_name = getHashName();
        hash_ = Botan::HashFunction::create(hash_name);
        if (!hash_) {
            throw BotanError("Failed to create hash: " + hash_name);
        }
    }

    std::string getHashName() const {
        switch (algorithm_) {
            case SHA256: return "SHA-256";
            case SHA384: return "SHA-384";
            case SHA512: return "SHA-512";
            case SHA3_256: return "SHA-3(256)";
            case SHA3_512: return "SHA-3(512)";
            case BLAKE2B_256: return "Blake2b(256)";
            case BLAKE2B_512: return "Blake2b(512)";
            case WHIRLPOOL: return "Whirlpool";
            default: return "SHA-256";
        }
    }

    Algorithm algorithm_;
    std::unique_ptr<Botan::HashFunction> hash_;
};

// Message Authentication Codes
class MAC {
public:
    enum Algorithm {
        HMAC_SHA256,
        HMAC_SHA512,
        CMAC_AES,
        GMAC_AES,
        POLY1305
    };

    MAC(Algorithm alg = HMAC_SHA256) : algorithm_(alg) {
        initializeMAC();
    }

    void setKey(const std::vector<uint8_t>& key) {
        mac_->set_key(key);
    }

    void update(const std::vector<uint8_t>& data) {
        mac_->update(data);
    }

    std::vector<uint8_t> finalize() {
        return mac_->final();
    }

    void reset() {
        mac_->reset();
    }

    // One-shot MAC
    static std::vector<uint8_t> compute(const std::vector<uint8_t>& data,
                                      const std::vector<uint8_t>& key,
                                      Algorithm alg = HMAC_SHA256) {
        MAC mac(alg);
        mac.setKey(key);
        mac.update(data);
        return mac.finalize();
    }

    // Verify MAC
    static bool verify(const std::vector<uint8_t>& data,
                      const std::vector<uint8_t>& key,
                      const std::vector<uint8_t>& mac_value,
                      Algorithm alg = HMAC_SHA256) {
        auto computed_mac = compute(data, key, alg);
        return Botan::constant_time_compare(computed_mac.data(), mac_value.data(),
                                          std::min(computed_mac.size(), mac_value.size()));
    }

private:
    void initializeMAC() {
        std::string mac_name = getMACName();
        mac_ = Botan::MessageAuthenticationCode::create(mac_name);
        if (!mac_) {
            throw BotanError("Failed to create MAC: " + mac_name);
        }
    }

    std::string getMACName() const {
        switch (algorithm_) {
            case HMAC_SHA256: return "HMAC(SHA-256)";
            case HMAC_SHA512: return "HMAC(SHA-512)";
            case CMAC_AES: return "CMAC(AES-256)";
            case GMAC_AES: return "GMAC(AES-256)";
            case POLY1305: return "Poly1305";
            default: return "HMAC(SHA-256)";
        }
    }

    Algorithm algorithm_;
    std::unique_ptr<Botan::MessageAuthenticationCode> mac_;
};

// Digital Signatures
class DigitalSignature {
public:
    enum Algorithm {
        RSA_SHA256,
        RSA_SHA512,
        ECDSA_SHA256,
        ECDSA_SHA512,
        ED25519,
        DILITHIUM  // Post-quantum
    };

    // Generate key pair
    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>>
    generateKeyPair(Algorithm alg = ECDSA_SHA256) {
        Botan::AutoSeeded_RNG rng;
        std::string alg_name = getAlgorithmName(alg);

        auto keygen = Botan::KeyPair::generate(rng, alg_name);
        if (!keygen) {
            throw BotanError("Failed to generate key pair for: " + alg_name);
        }

        // Serialize private key
        std::string private_pem = Botan::PKCS8::PEM_encode(*keygen->private_key(), rng, "");

        // Serialize public key
        std::string public_pem = Botan::X509::PEM_encode(*keygen->public_key());

        std::vector<uint8_t> private_key(private_pem.begin(), private_pem.end());
        std::vector<uint8_t> public_key(public_pem.begin(), public_pem.end());

        return {private_key, public_key};
    }

    // Sign data
    static std::vector<uint8_t> sign(const std::vector<uint8_t>& data,
                                   const std::vector<uint8_t>& private_key_pem,
                                   Algorithm alg = ECDSA_SHA256) {
        Botan::AutoSeeded_RNG rng;

        // Load private key
        Botan::DataSource_Memory key_source(private_key_pem.data(), private_key_pem.size());
        auto private_key = Botan::PKCS8::load_key(key_source, rng);
        if (!private_key) {
            throw BotanError("Failed to load private key");
        }

        std::string padding = getSignaturePadding(alg);
        auto signer = Botan::PK_Signer(*private_key, rng, padding);

        signer.update(data);
        return signer.signature(rng);
    }

    // Verify signature
    static bool verify(const std::vector<uint8_t>& data,
                      const std::vector<uint8_t>& signature,
                      const std::vector<uint8_t>& public_key_pem,
                      Algorithm alg = ECDSA_SHA256) {
        // Load public key
        Botan::DataSource_Memory key_source(public_key_pem.data(), public_key_pem.size());
        auto public_key = Botan::X509::load_key(key_source);
        if (!public_key) {
            throw BotanError("Failed to load public key");
        }

        std::string padding = getSignaturePadding(alg);
        auto verifier = Botan::PK_Verifier(*public_key, padding);

        verifier.update(data);
        return verifier.check_signature(signature);
    }

private:
    static std::string getAlgorithmName(Algorithm alg) {
        switch (alg) {
            case RSA_SHA256:
            case RSA_SHA512:
                return "RSA";
            case ECDSA_SHA256:
            case ECDSA_SHA512:
                return "ECDSA";
            case ED25519:
                return "Ed25519";
            case DILITHIUM:
                return "Dilithium";
            default:
                return "ECDSA";
        }
    }

    static std::string getSignaturePadding(Algorithm alg) {
        switch (alg) {
            case RSA_SHA256: return "PKCS1v15(SHA-256)";
            case RSA_SHA512: return "PKCS1v15(SHA-512)";
            case ECDSA_SHA256: return "EMSA1(SHA-256)";
            case ECDSA_SHA512: return "EMSA1(SHA-512)";
            case ED25519: return "Pure";
            case DILITHIUM: return "Dilithium";
            default: return "EMSA1(SHA-256)";
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
        X25519,
        KYBER  // Post-quantum
    };

    // Generate ephemeral key pair
    static std::pair<std::vector<uint8_t>, std::vector<uint8_t>>
    generateEphemeralKey(Algorithm alg = ECDH_P256) {
        Botan::AutoSeeded_RNG rng;
        std::string alg_name = getAlgorithmName(alg);

        auto keygen = Botan::KeyPair::generate(rng, alg_name);
        if (!keygen) {
            throw BotanError("Failed to generate key pair for: " + alg_name);
        }

        // Serialize private key
        std::string private_pem = Botan::PKCS8::PEM_encode(*keygen->private_key(), rng, "");

        // Serialize public key
        std::string public_pem = Botan::X509::PEM_encode(*keygen->public_key());

        std::vector<uint8_t> private_key(private_pem.begin(), private_pem.end());
        std::vector<uint8_t> public_key(public_pem.begin(), public_pem.end());

        return {private_key, public_key};
    }

    // Derive shared secret
    static std::vector<uint8_t> deriveSharedSecret(
        const std::vector<uint8_t>& private_key_pem,
        const std::vector<uint8_t>& peer_public_key_pem,
        Algorithm alg = ECDH_P256) {

        Botan::AutoSeeded_RNG rng;

        // Load private key
        Botan::DataSource_Memory priv_source(private_key_pem.data(), private_key_pem.size());
        auto private_key = Botan::PKCS8::load_key(priv_source, rng);
        if (!private_key) {
            throw BotanError("Failed to load private key");
        }

        // Load peer public key
        Botan::DataSource_Memory pub_source(peer_public_key_pem.data(), peer_public_key_pem.size());
        auto peer_public_key = Botan::X509::load_key(pub_source);
        if (!peer_public_key) {
            throw BotanError("Failed to load peer public key");
        }

        std::string kdf_name = getKDFName(alg);
        auto ka = Botan::Key_Agreement(*private_key, rng, kdf_name);

        return ka.derive_key(32, peer_public_key->public_key_bits()).bits_of();
    }

private:
    static std::string getAlgorithmName(Algorithm alg) {
        switch (alg) {
            case ECDH_P256: return "ECDH";
            case ECDH_P384: return "ECDH";
            case ECDH_P521: return "ECDH";
            case X25519: return "X25519";
            case KYBER: return "Kyber";
            default: return "ECDH";
        }
    }

    static std::string getKDFName(Algorithm alg) {
        switch (alg) {
            case ECDH_P256:
            case ECDH_P384:
            case ECDH_P521:
            case X25519:
                return "HKDF(SHA-256)";
            case KYBER:
                return "HKDF(SHA-3(256))";
            default:
                return "HKDF(SHA-256)";
        }
    }
};

// Password Hashing
class PasswordHash {
public:
    enum Algorithm {
        PBKDF2_SHA256,
        PBKDF2_SHA512,
        ARGON2I,
        ARGON2D,
        ARGON2ID,
        SCRYPT
    };

    // Hash password
    static std::vector<uint8_t> hash(const std::string& password,
                                   Algorithm alg = ARGON2ID,
                                   size_t output_length = 32) {
        Botan::AutoSeeded_RNG rng;
        std::string alg_name = getAlgorithmName(alg);

        auto pwdhash = Botan::PasswordHashFamily::create(alg_name);
        if (!pwdhash) {
            throw BotanError("Failed to create password hash: " + alg_name);
        }

        std::vector<uint8_t> salt(16);
        rng.randomize(salt.data(), salt.size());

        std::vector<uint8_t> hash(output_length);
        pwdhash->derive_key(hash.data(), hash.size(),
                           password.c_str(), password.length(),
                           salt.data(), salt.size());

        // Prepend salt for storage
        hash.insert(hash.begin(), salt.begin(), salt.end());
        return hash;
    }

    // Verify password
    static bool verify(const std::string& password,
                      const std::vector<uint8_t>& stored_hash,
                      Algorithm alg = ARGON2ID) {
        if (stored_hash.size() < 16) return false;

        std::vector<uint8_t> hash(stored_hash.begin() + 16, stored_hash.end());
        std::vector<uint8_t> salt(stored_hash.begin(), stored_hash.begin() + 16);

        Botan::AutoSeeded_RNG rng;
        std::string alg_name = getAlgorithmName(alg);

        auto pwdhash = Botan::PasswordHashFamily::create(alg_name);
        if (!pwdhash) {
            throw BotanError("Failed to create password hash: " + alg_name);
        }

        std::vector<uint8_t> computed_hash(hash.size());
        pwdhash->derive_key(computed_hash.data(), computed_hash.size(),
                           password.c_str(), password.length(),
                           salt.data(), salt.size());

        return Botan::constant_time_compare(hash.data(), computed_hash.data(), hash.size());
    }

    // Derive key from password
    static std::vector<uint8_t> deriveKey(const std::string& password,
                                        const std::vector<uint8_t>& salt,
                                        size_t key_length,
                                        Algorithm alg = ARGON2ID) {
        std::string alg_name = getAlgorithmName(alg);
        auto pwdhash = Botan::PasswordHashFamily::create(alg_name);
        if (!pwdhash) {
            throw BotanError("Failed to create password hash: " + alg_name);
        }

        std::vector<uint8_t> key(key_length);
        pwdhash->derive_key(key.data(), key.size(),
                           password.c_str(), password.length(),
                           salt.data(), salt.size());
        return key;
    }

    // Generate random salt
    static std::vector<uint8_t> generateSalt(size_t length = 16) {
        Botan::AutoSeeded_RNG rng;
        std::vector<uint8_t> salt(length);
        rng.randomize(salt.data(), salt.size());
        return salt;
    }

private:
    static std::string getAlgorithmName(Algorithm alg) {
        switch (alg) {
            case PBKDF2_SHA256: return "PBKDF2(SHA-256)";
            case PBKDF2_SHA512: return "PBKDF2(SHA-512)";
            case ARGON2I: return "Argon2i";
            case ARGON2D: return "Argon2d";
            case ARGON2ID: return "Argon2id";
            case SCRYPT: return "Scrypt";
            default: return "Argon2id";
        }
    }
};

// Random Number Generation
class Random {
public:
    static std::vector<uint8_t> bytes(size_t count) {
        Botan::AutoSeeded_RNG rng;
        std::vector<uint8_t> buffer(count);
        rng.randomize(buffer.data(), buffer.size());
        return buffer;
    }

    static uint32_t uniform(uint32_t upper_bound) {
        Botan::AutoSeeded_RNG rng;
        return rng.next_byte() % upper_bound;
    }

    static std::vector<uint8_t> generateKey(size_t length = 32) {
        return bytes(length);
    }

    static std::vector<uint8_t> generateIV(size_t length = 16) {
        return bytes(length);
    }
};

// Key Derivation Functions
class KDF {
public:
    enum Algorithm {
        HKDF_SHA256,
        HKDF_SHA512,
        PBKDF2_SHA256,
        SCRYPT
    };

    // Derive key
    static std::vector<uint8_t> derive(const std::vector<uint8_t>& secret,
                                     const std::vector<uint8_t>& salt,
                                     size_t output_length,
                                     Algorithm alg = HKDF_SHA256,
                                     const std::vector<uint8_t>& label = {}) {
        std::string kdf_name = getKDFName(alg);
        auto kdf = Botan::KDF::create(kdf_name);
        if (!kdf) {
            throw BotanError("Failed to create KDF: " + kdf_name);
        }

        Botan::secure_vector<uint8_t> output = kdf->derive_key(output_length, secret, salt, label);
        return std::vector<uint8_t>(output.begin(), output.end());
    }

private:
    static std::string getKDFName(Algorithm alg) {
        switch (alg) {
            case HKDF_SHA256: return "HKDF(SHA-256)";
            case HKDF_SHA512: return "HKDF(SHA-512)";
            case PBKDF2_SHA256: return "PBKDF2(SHA-256)";
            case SCRYPT: return "Scrypt";
            default: return "HKDF(SHA-256)";
        }
    }
};

// Main crypto facade class
class Crypto {
public:
    static void initialize() {
        static BotanInit init;
    }

    // High-level symmetric encryption
    static std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data,
                                      const std::vector<uint8_t>& key,
                                      const std::vector<uint8_t>& iv,
                                      const std::vector<uint8_t>& aad = {}) {
        return SymmetricCipher::encrypt(data, key, iv, SymmetricCipher::AES_256_GCM, aad);
    }

    static std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data,
                                      const std::vector<uint8_t>& key,
                                      const std::vector<uint8_t>& iv,
                                      const std::vector<uint8_t>& aad = {}) {
        return SymmetricCipher::decrypt(data, key, iv, SymmetricCipher::AES_256_GCM, aad);
    }

    // Hash functions
    static std::vector<uint8_t> hash(const std::vector<uint8_t>& data,
                                   HashFunction::Algorithm alg = HashFunction::SHA256) {
        return HashFunction::hash(data, alg);
    }

    // MAC functions
    static std::vector<uint8_t> mac(const std::vector<uint8_t>& data,
                                  const std::vector<uint8_t>& key,
                                  MAC::Algorithm alg = MAC::HMAC_SHA256) {
        return MAC::compute(data, key, alg);
    }

    static bool verifyMAC(const std::vector<uint8_t>& data,
                         const std::vector<uint8_t>& key,
                         const std::vector<uint8_t>& mac_value,
                         MAC::Algorithm alg = MAC::HMAC_SHA256) {
        return MAC::verify(data, key, mac_value, alg);
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

    // Password hashing
    static std::vector<uint8_t> hashPassword(const std::string& password,
                                           PasswordHash::Algorithm alg = PasswordHash::ARGON2ID) {
        return PasswordHash::hash(password, alg);
    }

    static bool verifyPassword(const std::string& password,
                             const std::vector<uint8_t>& hash,
                             PasswordHash::Algorithm alg = PasswordHash::ARGON2ID) {
        return PasswordHash::verify(password, hash, alg);
    }

    // Random generation
    static std::vector<uint8_t> randomBytes(size_t count) {
        return Random::bytes(count);
    }

    static std::vector<uint8_t> generateKey(size_t length = 32) {
        return Random::generateKey(length);
    }

    static std::vector<uint8_t> generateIV(size_t length = 16) {
        return Random::generateIV(length);
    }
};

} // namespace botan

// Example usage and test functions
namespace botan_examples {

// Basic authenticated encryption
void basicEncryptionExample() {
    botan::Crypto::initialize();

    std::string message = "Hello, World!";
    std::vector<uint8_t> data(message.begin(), message.end());
    auto key = botan::Crypto::generateKey(32);
    auto iv = botan::Crypto::generateIV(16);

    // Encrypt
    auto encrypted = botan::Crypto::encrypt(data, key, iv);
    std::cout << "Encrypted size: " << encrypted.size() << " bytes" << std::endl;

    // Decrypt
    auto decrypted = botan::Crypto::decrypt(encrypted, key, iv);
    std::string result(decrypted.begin(), decrypted.end());
    std::cout << "Decrypted: " << result << std::endl;

    assert(result == message);
}

// Hash function example
void hashExample() {
    botan::Crypto::initialize();

    std::string message = "Hash me!";
    std::vector<uint8_t> data(message.begin(), message.end());

    auto sha256_hash = botan::Crypto::hash(data, botan::HashFunction::SHA256);
    auto sha3_hash = botan::Crypto::hash(data, botan::HashFunction::SHA3_256);

    std::cout << "SHA-256: " << Botan::hex_encode(sha256_hash) << std::endl;
    std::cout << "SHA-3:   " << Botan::hex_encode(sha3_hash) << std::endl;

    // Incremental hashing
    botan::HashFunction::IncrementalHash hasher(botan::HashFunction::BLAKE2B_256);
    hasher.update(std::vector<uint8_t>(data.begin(), data.begin() + 4));
    hasher.update(std::vector<uint8_t>(data.begin() + 4, data.end()));
    auto incremental_hash = hasher.finalize();

    std::cout << "Blake2b: " << Botan::hex_encode(incremental_hash) << std::endl;
}

// MAC example
void macExample() {
    botan::Crypto::initialize();

    std::string message = "Authenticate me!";
    std::vector<uint8_t> data(message.begin(), message.end());
    auto key = botan::Crypto::generateKey(32);

    // Compute HMAC
    auto hmac = botan::Crypto::mac(data, key, botan::MAC::HMAC_SHA256);
    std::cout << "HMAC: " << Botan::hex_encode(hmac) << std::endl;

    // Verify HMAC
    bool valid = botan::Crypto::verifyMAC(data, key, hmac, botan::MAC::HMAC_SHA256);
    std::cout << "HMAC verification: " << (valid ? "Success" : "Failed") << std::endl;

    assert(valid);
}

// Digital signature example
void digitalSignatureExample() {
    botan::Crypto::initialize();

    std::string message = "This message will be signed";
    std::vector<uint8_t> data(message.begin(), message.end());

    // Generate key pair
    auto [private_key, public_key] = botan::Crypto::generateKeyPair();

    // Sign
    auto signature = botan::Crypto::sign(data, private_key);
    std::cout << "Signature size: " << signature.size() << " bytes" << std::endl;

    // Verify
    bool valid = botan::Crypto::verify(data, signature, public_key);
    std::cout << "Signature valid: " << (valid ? "Yes" : "No") << std::endl;

    assert(valid);
}

// Password hashing example
void passwordHashingExample() {
    botan::Crypto::initialize();

    std::string password = "mySecurePassword123!";

    // Hash with Argon2
    auto hash = botan::Crypto::hashPassword(password, botan::PasswordHash::ARGON2ID);
    std::cout << "Password hash size: " << hash.size() << " bytes" << std::endl;

    // Verify password
    bool valid = botan::Crypto::verifyPassword(password, hash, botan::PasswordHash::ARGON2ID);
    std::cout << "Password verification: " << (valid ? "Success" : "Failed") << std::endl;

    assert(valid);
}

// Key exchange example
void keyExchangeExample() {
    botan::Crypto::initialize();

    // Alice generates key pair
    auto [alice_private, alice_public] = botan::Crypto::generateKeyExchangePair();

    // Bob generates key pair
    auto [bob_private, bob_public] = botan::Crypto::generateKeyExchangePair();

    // Alice derives shared secret
    auto alice_secret = botan::Crypto::deriveSharedSecret(alice_private, bob_public);

    // Bob derives shared secret
    auto bob_secret = botan::Crypto::deriveSharedSecret(bob_private, alice_public);

    // Shared secrets should be identical
    assert(alice_secret == bob_secret);
    std::cout << "Key exchange successful - shared secret: "
              << Botan::hex_encode(alice_secret) << std::endl;
}

} // namespace botan_examples

#endif // CRYPTO_BOTAN_WRAPPER_HPP
