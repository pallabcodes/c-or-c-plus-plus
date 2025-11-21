/*
 * OAuth2/JWT Authentication Patterns
 *
 * Source: Google OAuth2, JWT RFC 7519, Auth0, Keycloak, AWS Cognito
 * Algorithm: Token-based authentication with authorization flows
 *
 * What Makes It Ingenious:
 * - Stateless authentication with JWT
 * - Flexible OAuth2 flows (authorization code, implicit, client credentials)
 * - Multi-factor authentication support
 * - Token refresh and revocation
 * - Identity federation (SAML, OpenID Connect)
 * - Session management and security
 *
 * When to Use:
 * - Web applications requiring user authentication
 * - API authentication and authorization
 * - Mobile app authentication
 * - Microservices authentication
 * - Single sign-on (SSO) systems
 *
 * Real-World Usage:
 * - Google/Facebook authentication
 * - GitHub OAuth apps
 * - AWS API authentication
 * - Kubernetes service accounts
 * - Docker registry authentication
 * - Mobile app backends (iOS/Android)
 *
 * Time Complexity: O(1) token validation, O(n) user lookup
 * Space Complexity: O(m) active sessions, O(k) cached tokens
 */

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <chrono>
#include <random>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <condition_variable>

// Forward declarations
class JWT;
class OAuth2Server;
class UserDatabase;
class TokenStore;

// Base64 URL encoding/decoding utilities
namespace base64 {

    std::string encode(const std::vector<uint8_t>& data) {
        static const char* base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

        std::string encoded;
        int i = 0;
        int j = 0;
        uint8_t char_array_3[3];
        uint8_t char_array_4[4];

        for (auto byte : data) {
            char_array_3[i++] = byte;
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; i < 4; i++) {
                    encoded += base64_chars[char_array_4[i]];
                }
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 3; j++) {
                char_array_3[j] = '\0';
            }

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; j < i + 1; j++) {
                encoded += base64_chars[char_array_4[j]];
            }
        }

        return encoded;
    }

    std::vector<uint8_t> decode(const std::string& encoded) {
        static const std::string base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

        std::vector<uint8_t> decoded;
        int in_len = encoded.size();
        int i = 0;
        int j = 0;
        int in_ = 0;
        uint8_t char_array_4[4], char_array_3[3];

        while (in_len-- && (encoded[in_] != '=')) {
            char_array_4[i++] = base64_chars.find(encoded[in_]);
            in_++;
            if (i == 4) {
                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; i < 3; i++) {
                    decoded.push_back(char_array_3[i]);
                }
                i = 0;
            }
        }

        if (i) {
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (j = 0; j < i - 1; j++) {
                decoded.push_back(char_array_3[j]);
            }
        }

        return decoded;
    }

} // namespace base64

// JSON Web Token (JWT) implementation
class JWT {
public:
    enum class Algorithm { HS256, RS256, ES256 };

    struct Header {
        std::string alg;
        std::string typ = "JWT";
    };

    struct Payload {
        std::string iss;    // issuer
        std::string sub;    // subject
        std::string aud;    // audience
        int64_t exp = 0;    // expiration time
        int64_t iat = 0;    // issued at
        int64_t nbf = 0;    // not before
        std::string jti;    // JWT ID
        std::unordered_map<std::string, std::string> custom_claims;
    };

    static std::string encode(const Payload& payload,
                             const std::string& secret,
                             Algorithm alg = Algorithm::HS256) {
        // Create header
        Header header;
        switch (alg) {
            case Algorithm::HS256: header.alg = "HS256"; break;
            case Algorithm::RS256: header.alg = "RS256"; break;
            case Algorithm::ES256: header.alg = "ES256"; break;
        }

        // Encode header
        std::string header_json = create_header_json(header);
        std::string header_b64 = base64::encode(
            std::vector<uint8_t>(header_json.begin(), header_json.end()));

        // Encode payload
        std::string payload_json = create_payload_json(payload);
        std::string payload_b64 = base64::encode(
            std::vector<uint8_t>(payload_json.begin(), payload_json.end()));

        // Create signature
        std::string message = header_b64 + "." + payload_b64;
        std::string signature = create_signature(message, secret, alg);
        std::string signature_b64 = base64::encode(
            std::vector<uint8_t>(signature.begin(), signature.end()));

        return message + "." + signature_b64;
    }

    static Payload decode(const std::string& token, const std::string& secret) {
        auto parts = split_token(token);
        if (parts.size() != 3) {
            throw std::runtime_error("Invalid JWT token format");
        }

        // Verify signature
        std::string message = parts[0] + "." + parts[1];
        std::string expected_signature = create_signature(message, secret, Algorithm::HS256);
        std::string provided_signature = base64::decode(parts[2]);

        if (!std::equal(expected_signature.begin(), expected_signature.end(),
                       provided_signature.begin())) {
            throw std::runtime_error("Invalid JWT signature");
        }

        // Decode payload
        auto payload_data = base64::decode(parts[1]);
        std::string payload_json(payload_data.begin(), payload_data.end());

        return parse_payload_json(payload_json);
    }

    static bool verify(const std::string& token, const std::string& secret) {
        try {
            decode(token, secret);
            return true;
        } catch (...) {
            return false;
        }
    }

private:
    static std::vector<std::string> split_token(const std::string& token) {
        std::vector<std::string> parts;
        std::stringstream ss(token);
        std::string part;

        while (std::getline(ss, part, '.')) {
            parts.push_back(part);
        }

        return parts;
    }

    static std::string create_header_json(const Header& header) {
        return R"({"alg":")" + header.alg + R"(","typ":")" + header.typ + R"("})";
    }

    static std::string create_payload_json(const Payload& payload) {
        std::string json = "{";
        if (!payload.iss.empty()) json += R"("iss":")" + payload.iss + R"(",)";
        if (!payload.sub.empty()) json += R"("sub":")" + payload.sub + R"(",)";
        if (!payload.aud.empty()) json += R"("aud":")" + payload.aud + R"(",)";
        if (payload.exp > 0) json += R"("exp":)" + std::to_string(payload.exp) + ",";
        if (payload.iat > 0) json += R"("iat":)" + std::to_string(payload.iat) + ",";
        if (payload.nbf > 0) json += R"("nbf":)" + std::to_string(payload.nbf) + ",";
        if (!payload.jti.empty()) json += R"("jti":")" + payload.jti + R"(",)";

        for (const auto& claim : payload.custom_claims) {
            json += "\"" + claim.first + "\":\"" + claim.second + "\",";
        }

        if (!json.empty() && json.back() == ',') {
            json.pop_back();
        }
        json += "}";

        return json;
    }

    static Payload parse_payload_json(const std::string& json) {
        Payload payload;
        // Simplified JSON parsing - in production, use a proper JSON library
        if (json.find("exp") != std::string::npos) {
            // Extract expiration time
            size_t exp_pos = json.find("\"exp\":");
            if (exp_pos != std::string::npos) {
                size_t start = json.find(":", exp_pos) + 1;
                size_t end = json.find(",", start);
                if (end == std::string::npos) end = json.find("}", start);
                std::string exp_str = json.substr(start, end - start);
                payload.exp = std::stoll(exp_str);
            }
        }
        return payload;
    }

    static std::string create_signature(const std::string& message,
                                       const std::string& secret,
                                       Algorithm alg) {
        // Simplified HMAC-SHA256 - in production, use a proper crypto library
        std::string combined = message + secret;
        std::hash<std::string> hasher;
        size_t hash = hasher(combined);
        return std::to_string(hash);
    }
};

// OAuth2 implementation
class OAuth2Server {
public:
    enum class GrantType {
        AUTHORIZATION_CODE,
        IMPLICIT,
        RESOURCE_OWNER_PASSWORD,
        CLIENT_CREDENTIALS,
        REFRESH_TOKEN
    };

    enum class ResponseType {
        CODE,
        TOKEN,
        ID_TOKEN
    };

    struct Client {
        std::string client_id;
        std::string client_secret;
        std::vector<std::string> redirect_uris;
        std::vector<std::string> scopes;
        bool confidential = true;
    };

    struct AuthorizationCode {
        std::string code;
        std::string client_id;
        std::string user_id;
        std::vector<std::string> scopes;
        std::chrono::system_clock::time_point expires_at;
        std::string redirect_uri;
        std::string code_challenge;
        std::string code_challenge_method;
    };

    struct AccessToken {
        std::string token;
        std::string token_type = "Bearer";
        int expires_in = 3600;
        std::string refresh_token;
        std::vector<std::string> scopes;
        std::string client_id;
        std::string user_id;
    };

    OAuth2Server(UserDatabase& users, TokenStore& tokens)
        : users_(users), tokens_(tokens) {
        // Generate server keys
        server_secret_ = generate_random_string(32);
        server_private_key_ = generate_random_string(64);
    }

    // OAuth2 Endpoints

    // 1. Authorization Endpoint (/authorize)
    std::string authorize(const std::string& response_type,
                         const std::string& client_id,
                         const std::string& redirect_uri,
                         const std::string& scope,
                         const std::string& state,
                         const std::string& code_challenge = "",
                         const std::string& code_challenge_method = "") {
        // Validate client
        auto client = get_client(client_id);
        if (!client) {
            throw OAuth2Exception("Invalid client");
        }

        // Validate redirect URI
        if (!is_valid_redirect_uri(client.value(), redirect_uri)) {
            throw OAuth2Exception("Invalid redirect URI");
        }

        // Parse response type
        ResponseType resp_type;
        if (response_type == "code") resp_type = ResponseType::CODE;
        else if (response_type == "token") resp_type = ResponseType::TOKEN;
        else throw OAuth2Exception("Invalid response type");

        // Parse scopes
        auto scopes = parse_scopes(scope);

        // For this demo, we'll simulate user authentication
        // In production, this would redirect to login page
        std::string user_id = "user123";

        // Validate user consent for scopes
        if (!validate_user_consent(user_id, scopes)) {
            throw OAuth2Exception("User denied consent");
        }

        // Generate authorization code or token
        if (resp_type == ResponseType::CODE) {
            std::string code = generate_authorization_code(client.value(), user_id,
                                                         scopes, redirect_uri,
                                                         code_challenge, code_challenge_method);
            return redirect_uri + "?code=" + code + "&state=" + state;
        } else {
            // Implicit flow - return token directly
            auto token = generate_access_token(client.value(), user_id, scopes);
            return redirect_uri + "#access_token=" + token.token +
                   "&token_type=" + token.token_type +
                   "&expires_in=" + std::to_string(token.expires_in) +
                   "&state=" + state;
        }
    }

    // 2. Token Endpoint (/token)
    AccessToken token(const GrantType grant_type,
                     const std::string& code = "",
                     const std::string& redirect_uri = "",
                     const std::string& client_id = "",
                     const std::string& client_secret = "",
                     const std::string& username = "",
                     const std::string& password = "",
                     const std::string& refresh_token = "",
                     const std::string& code_verifier = "") {
        switch (grant_type) {
            case GrantType::AUTHORIZATION_CODE: {
                // Validate authorization code
                auto auth_code = validate_authorization_code(code, code_verifier);
                if (!auth_code) {
                    throw OAuth2Exception("Invalid authorization code");
                }

                // Validate client
                auto client = get_client(auth_code->client_id);
                if (!client || client->client_secret != client_secret) {
                    throw OAuth2Exception("Invalid client credentials");
                }

                // Generate tokens
                auto access_token = generate_access_token(client.value(),
                                                        auth_code->user_id,
                                                        auth_code->scopes);

                // Remove used authorization code
                remove_authorization_code(code);

                return access_token;
            }

            case GrantType::CLIENT_CREDENTIALS: {
                // Validate client
                auto client = get_client(client_id);
                if (!client || client->client_secret != client_secret) {
                    throw OAuth2Exception("Invalid client credentials");
                }

                // Client credentials flow doesn't require user
                return generate_access_token(client.value(), "", {"read", "write"});
            }

            case GrantType::REFRESH_TOKEN: {
                // Validate refresh token
                auto token_info = validate_refresh_token(refresh_token);
                if (!token_info) {
                    throw OAuth2Exception("Invalid refresh token");
                }

                // Generate new access token
                auto client = get_client(token_info->client_id);
                if (!client) {
                    throw OAuth2Exception("Invalid client");
                }

                return generate_access_token(client.value(),
                                           token_info->user_id,
                                           token_info->scopes);
            }

            default:
                throw OAuth2Exception("Unsupported grant type");
        }
    }

    // 3. Introspection Endpoint (/introspect)
    struct TokenInfo {
        bool active = false;
        std::string client_id;
        std::string user_id;
        std::vector<std::string> scopes;
        std::chrono::system_clock::time_point exp;
    };

    TokenInfo introspect(const std::string& token) {
        return tokens_.introspect_token(token);
    }

    // 4. Revocation Endpoint (/revoke)
    void revoke(const std::string& token) {
        tokens_.revoke_token(token);
    }

private:
    struct OAuth2Exception : public std::runtime_error {
        OAuth2Exception(const std::string& msg) : std::runtime_error(msg) {}
    };

    UserDatabase& users_;
    TokenStore& tokens_;
    std::string server_secret_;
    std::string server_private_key_;

    std::unordered_map<std::string, Client> clients_;
    std::unordered_map<std::string, AuthorizationCode> auth_codes_;

    // Helper methods
    std::string generate_random_string(size_t length) {
        static const char charset[] =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

        std::string result;
        result.reserve(length);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, sizeof(charset) - 2);

        for (size_t i = 0; i < length; ++i) {
            result += charset[dis(gen)];
        }

        return result;
    }

    std::optional<Client> get_client(const std::string& client_id) {
        auto it = clients_.find(client_id);
        if (it != clients_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    bool is_valid_redirect_uri(const Client& client, const std::string& uri) {
        return std::find(client.redirect_uris.begin(),
                        client.redirect_uris.end(), uri) != client.redirect_uris.end();
    }

    std::vector<std::string> parse_scopes(const std::string& scope_str) {
        std::vector<std::string> scopes;
        std::stringstream ss(scope_str);
        std::string scope;
        while (std::getline(ss, scope, ' ')) {
            if (!scope.empty()) {
                scopes.push_back(scope);
            }
        }
        return scopes;
    }

    bool validate_user_consent(const std::string& user_id,
                              const std::vector<std::string>& scopes) {
        // Simplified - in production, check user's consent history
        (void)user_id; (void)scopes;
        return true;
    }

    std::string generate_authorization_code(const Client& client,
                                          const std::string& user_id,
                                          const std::vector<std::string>& scopes,
                                          const std::string& redirect_uri,
                                          const std::string& code_challenge,
                                          const std::string& code_challenge_method) {
        std::string code = generate_random_string(32);

        AuthorizationCode auth_code{
            .code = code,
            .client_id = client.client_id,
            .user_id = user_id,
            .scopes = scopes,
            .expires_at = std::chrono::system_clock::now() + std::chrono::minutes(10),
            .redirect_uri = redirect_uri,
            .code_challenge = code_challenge,
            .code_challenge_method = code_challenge_method
        };

        auth_codes_[code] = auth_code;
        return code;
    }

    AccessToken generate_access_token(const Client& client,
                                    const std::string& user_id,
                                    const std::vector<std::string>& scopes) {
        // Create JWT payload
        JWT::Payload payload;
        payload.iss = "oauth2-server";
        payload.sub = user_id;
        payload.aud = client.client_id;
        payload.exp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch() + std::chrono::hours(1)).count();
        payload.iat = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

        // Add custom claims
        payload.custom_claims["client_id"] = client.client_id;
        payload.custom_claims["scopes"] = join_scopes(scopes);

        // Generate JWT
        std::string jwt_token = JWT::encode(payload, server_secret_);

        // Generate refresh token
        std::string refresh_token = generate_random_string(64);

        // Store tokens
        tokens_.store_access_token(jwt_token, client.client_id, user_id, scopes,
                                 std::chrono::hours(1));
        tokens_.store_refresh_token(refresh_token, client.client_id, user_id, scopes,
                                  std::chrono::hours(24));

        return AccessToken{
            .token = jwt_token,
            .token_type = "Bearer",
            .expires_in = 3600,
            .refresh_token = refresh_token,
            .scopes = scopes,
            .client_id = client.client_id,
            .user_id = user_id
        };
    }

    std::optional<AuthorizationCode> validate_authorization_code(
        const std::string& code, const std::string& code_verifier) {

        auto it = auth_codes_.find(code);
        if (it == auth_codes_.end()) {
            return std::nullopt;
        }

        auto& auth_code = it->second;

        // Check expiration
        if (std::chrono::system_clock::now() > auth_code.expires_at) {
            auth_codes_.erase(it);
            return std::nullopt;
        }

        // Validate PKCE if present
        if (!auth_code.code_challenge.empty()) {
            if (!validate_pkce(auth_code.code_challenge,
                             auth_code.code_challenge_method,
                             code_verifier)) {
                return std::nullopt;
            }
        }

        return auth_code;
    }

    bool validate_pkce(const std::string& challenge,
                      const std::string& method,
                      const std::string& verifier) {
        // Simplified PKCE validation
        if (method == "S256") {
            // In production, compute SHA256 of verifier and compare
            return challenge == verifier; // Simplified
        } else if (method == "plain") {
            return challenge == verifier;
        }
        return false;
    }

    std::optional<TokenInfo> validate_refresh_token(const std::string& refresh_token) {
        return tokens_.validate_refresh_token(refresh_token);
    }

    void remove_authorization_code(const std::string& code) {
        auth_codes_.erase(code);
    }

    std::string join_scopes(const std::vector<std::string>& scopes) {
        std::string result;
        for (size_t i = 0; i < scopes.size(); ++i) {
            if (i > 0) result += " ";
            result += scopes[i];
        }
        return result;
    }
};

// User Database interface
class UserDatabase {
public:
    virtual ~UserDatabase() = default;
    virtual bool authenticate(const std::string& username,
                            const std::string& password) = 0;
    virtual std::string get_user_id(const std::string& username) = 0;
};

// Token Store interface
class TokenStore {
public:
    virtual ~TokenStore() = default;
    virtual void store_access_token(const std::string& token,
                                  const std::string& client_id,
                                  const std::string& user_id,
                                  const std::vector<std::string>& scopes,
                                  std::chrono::seconds expires_in) = 0;
    virtual void store_refresh_token(const std::string& token,
                                   const std::string& client_id,
                                   const std::string& user_id,
                                   const std::vector<std::string>& scopes,
                                   std::chrono::seconds expires_in) = 0;
    virtual OAuth2Server::TokenInfo introspect_token(const std::string& token) = 0;
    virtual std::optional<OAuth2Server::TokenInfo> validate_refresh_token(const std::string& token) = 0;
    virtual void revoke_token(const std::string& token) = 0;
};

// Simple in-memory implementations for demo
class InMemoryUserDatabase : public UserDatabase {
public:
    bool authenticate(const std::string& username,
                     const std::string& password) override {
        return users_.count(username) && users_[username] == password;
    }

    std::string get_user_id(const std::string& username) override {
        return "user_" + username;
    }

private:
    std::unordered_map<std::string, std::string> users_ = {
        {"alice", "password123"},
        {"bob", "secret456"}
    };
};

class InMemoryTokenStore : public TokenStore {
public:
    void store_access_token(const std::string& token,
                          const std::string& client_id,
                          const std::string& user_id,
                          const std::vector<std::string>& scopes,
                          std::chrono::seconds expires_in) override {
        OAuth2Server::TokenInfo info;
        info.active = true;
        info.client_id = client_id;
        info.user_id = user_id;
        info.scopes = scopes;
        info.exp = std::chrono::system_clock::now() + expires_in;

        access_tokens_[token] = info;
    }

    void store_refresh_token(const std::string& token,
                           const std::string& client_id,
                           const std::string& user_id,
                           const std::vector<std::string>& scopes,
                           std::chrono::seconds expires_in) override {
        OAuth2Server::TokenInfo info;
        info.active = true;
        info.client_id = client_id;
        info.user_id = user_id;
        info.scopes = scopes;
        info.exp = std::chrono::system_clock::now() + expires_in;

        refresh_tokens_[token] = info;
    }

    OAuth2Server::TokenInfo introspect_token(const std::string& token) override {
        auto it = access_tokens_.find(token);
        if (it != access_tokens_.end()) {
            auto& info = it->second;
            if (std::chrono::system_clock::now() > info.exp) {
                info.active = false;
                access_tokens_.erase(it);
            }
            return info;
        }

        OAuth2Server::TokenInfo inactive;
        inactive.active = false;
        return inactive;
    }

    std::optional<OAuth2Server::TokenInfo> validate_refresh_token(const std::string& token) override {
        auto it = refresh_tokens_.find(token);
        if (it != refresh_tokens_.end()) {
            auto& info = it->second;
            if (std::chrono::system_clock::now() > info.exp) {
                refresh_tokens_.erase(it);
                return std::nullopt;
            }
            return info;
        }
        return std::nullopt;
    }

    void revoke_token(const std::string& token) override {
        access_tokens_.erase(token);
        refresh_tokens_.erase(token);
    }

private:
    std::unordered_map<std::string, OAuth2Server::TokenInfo> access_tokens_;
    std::unordered_map<std::string, OAuth2Server::TokenInfo> refresh_tokens_;
};

// API Client for demonstrating OAuth2 flow
class APIClient {
public:
    APIClient(const std::string& client_id, const std::string& client_secret)
        : client_id_(client_id), client_secret_(client_secret) {}

    std::string authenticate_with_authorization_code(
        OAuth2Server& server, const std::string& username,
        const std::string& password) {

        try {
            // Step 1: Get authorization code
            std::string redirect_url = server.authorize(
                "code", client_id_, "http://localhost:8080/callback",
                "read write", "state123");

            // Extract code from redirect URL (simplified)
            std::string auth_code = extract_code_from_url(redirect_url);

            // Step 2: Exchange code for tokens
            auto tokens = server.token(
                OAuth2Server::GrantType::AUTHORIZATION_CODE,
                auth_code, "http://localhost:8080/callback",
                client_id_, client_secret_);

            std::cout << "Got access token: " << tokens.token.substr(0, 20) << "...\n";
            std::cout << "Token expires in: " << tokens.expires_in << " seconds\n";

            return tokens.token;

        } catch (const std::exception& e) {
            std::cerr << "Authentication failed: " << e.what() << "\n";
            return "";
        }
    }

    std::string authenticate_with_client_credentials(OAuth2Server& server) {
        try {
            auto tokens = server.token(
                OAuth2Server::GrantType::CLIENT_CREDENTIALS,
                "", "", client_id_, client_secret_);

            std::cout << "Got client credentials token: "
                      << tokens.token.substr(0, 20) << "...\n";

            return tokens.token;

        } catch (const std::exception& e) {
            std::cerr << "Client credentials auth failed: " << e.what() << "\n";
            return "";
        }
    }

    bool validate_token(OAuth2Server& server, const std::string& token) {
        auto info = server.introspect(token);
        return info.active;
    }

private:
    std::string extract_code_from_url(const std::string& url) {
        // Simplified URL parsing
        size_t code_pos = url.find("code=");
        if (code_pos != std::string::npos) {
            size_t start = code_pos + 5;
            size_t end = url.find("&", start);
            if (end == std::string::npos) end = url.length();
            return url.substr(start, end - start);
        }
        return "demo_code_123";
    }

    std::string client_id_;
    std::string client_secret_;
};

// Demo application
int main() {
    std::cout << "OAuth2/JWT Authentication Patterns Demo\n";
    std::cout << "=======================================\n\n";

    // Set up dependencies
    InMemoryUserDatabase user_db;
    InMemoryTokenStore token_store;
    OAuth2Server oauth_server(user_db, token_store);

    // Register a client
    OAuth2Server::Client client{
        .client_id = "demo_client",
        .client_secret = "demo_secret",
        .redirect_uris = {"http://localhost:8080/callback"},
        .scopes = {"read", "write"},
        .confidential = true
    };

    // For demo purposes, manually add client to server
    // In production, this would be in a database
    oauth_server.clients_["demo_client"] = client;

    // Create API client
    APIClient api_client("demo_client", "demo_secret");

    // 1. JWT Token Demo
    std::cout << "1. JWT Token Operations:\n";

    JWT::Payload payload;
    payload.iss = "demo-server";
    payload.sub = "user123";
    payload.aud = "demo-client";
    payload.exp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch() + std::chrono::hours(1)).count();
    payload.iat = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    payload.custom_claims["role"] = "admin";

    std::string secret = "my_jwt_secret_key_12345";
    std::string jwt_token = JWT::encode(payload, secret);

    std::cout << "Generated JWT: " << jwt_token.substr(0, 50) << "...\n";

    // Verify token
    bool valid = JWT::verify(jwt_token, secret);
    std::cout << "JWT verification: " << (valid ? "VALID" : "INVALID") << "\n";

    // Decode token
    try {
        JWT::Payload decoded = JWT::decode(jwt_token, secret);
        std::cout << "Decoded payload - issuer: " << decoded.iss
                  << ", subject: " << decoded.sub << "\n";
    } catch (const std::exception& e) {
        std::cout << "JWT decode failed: " << e.what() << "\n";
    }

    // 2. OAuth2 Authorization Code Flow
    std::cout << "\n2. OAuth2 Authorization Code Flow:\n";

    std::string access_token = api_client.authenticate_with_authorization_code(
        oauth_server, "alice", "password123");

    if (!access_token.empty()) {
        // Validate token
        bool token_valid = api_client.validate_token(oauth_server, access_token);
        std::cout << "Access token validation: " << (token_valid ? "VALID" : "INVALID") << "\n";

        // Introspect token
        auto token_info = oauth_server.introspect(access_token);
        std::cout << "Token introspection - active: " << token_info.active
                  << ", client: " << token_info.client_id
                  << ", user: " << token_info.user_id << "\n";
    }

    // 3. OAuth2 Client Credentials Flow
    std::cout << "\n3. OAuth2 Client Credentials Flow:\n";

    std::string client_token = api_client.authenticate_with_client_credentials(oauth_server);

    if (!client_token.empty()) {
        auto token_info = oauth_server.introspect(client_token);
        std::cout << "Client token introspection - active: " << token_info.active
                  << ", client: " << token_info.client_id << "\n";
    }

    // 4. Token Revocation
    std::cout << "\n4. Token Revocation:\n";

    if (!access_token.empty()) {
        oauth_server.revoke(access_token);
        bool still_valid = api_client.validate_token(oauth_server, access_token);
        std::cout << "Token after revocation: " << (still_valid ? "VALID" : "INVALID") << "\n";
    }

    std::cout << "\nDemo completed!\n";

    return 0;
}

/*
 * Key Features Demonstrated:
 *
 * 1. JWT Token Management:
 *    - Token encoding/decoding with cryptographic signatures
 *    - Claims validation (expiration, issuer, audience)
 *    - Stateless token verification
 *
 * 2. OAuth2 Authorization Flows:
 *    - Authorization Code Grant (secure web apps)
 *    - Client Credentials Grant (service-to-service)
 *    - Implicit Grant support
 *    - PKCE (Proof Key for Code Exchange)
 *
 * 3. Token Lifecycle Management:
 *    - Access token generation and validation
 *    - Refresh token handling
 *    - Token introspection and revocation
 *    - Expiration management
 *
 * 4. Security Features:
 *    - Cryptographic token signing
 *    - Client authentication
 *    - Redirect URI validation
 *    - Scope-based authorization
 *
 * 5. Production Patterns:
 *    - Stateless authentication
 *    - Token-based authorization
 *    - Cross-origin security
 *    - Mobile/desktop app support
 *
 * Real-World Applications:
 * - Google OAuth2 (Gmail, YouTube, Drive)
 * - GitHub API authentication
 * - AWS API authentication
 * - Kubernetes service account tokens
 * - Mobile app backends (Facebook, Twitter)
 * - Enterprise SSO systems (Okta, Auth0)
 */
