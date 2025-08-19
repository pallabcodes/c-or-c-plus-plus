#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <optional>
#include <expected>
#include <variant>
#include <memory>
#include <span>
#include <chrono>

namespace networking::http {

// ================================================================================================
// HTTP Method Implementation
// ================================================================================================

enum class Method : uint8_t {
    GET,     // RFC 7231
    POST,    // RFC 7231
    PUT,     // RFC 7231
    DELETE,  // RFC 7231
    HEAD,    // RFC 7231
    OPTIONS, // RFC 7231
    PATCH,   // RFC 5789
    TRACE,   // RFC 7231
    CONNECT  // RFC 7231
};

std::string_view to_string(Method method);
std::expected<Method, std::string> parse_method(std::string_view method_str);

// ================================================================================================
// Version Implementation
// ================================================================================================

struct Version {
    int major = 1;
    int minor = 1;
    
    std::string to_string() const;
    static std::expected<Version, std::string> parse(std::string_view version_str);
    bool operator==(const Version& other) const = default;
};

// ================================================================================================
// HeaderMap Implementation
// ================================================================================================

class HeaderMap {
public:
    std::unordered_map<std::string, std::string> headers_;
    
    std::string get(const std::string& name) const;
    void set(std::string name, std::string value);
    void add(std::string name, std::string value);
    void remove(const std::string& name);
    bool contains(const std::string& name) const;
    void clear();
    std::string to_string() const;
    static std::string normalize_header_name(const std::string& name);
};

// ================================================================================================
// Request Implementation
// ================================================================================================

class Request {
public:
    Request(Method method, std::string uri, Version version);
    
    std::string get_header(const std::string& name) const;
    void set_header(std::string name, std::string value);
    void set_body(std::vector<uint8_t> body);
    void set_body(std::string body);
    bool has_body() const;
    std::string to_string() const;
    
    Method method_;
    std::string uri_;
    Version version_;
    HeaderMap headers_;
    std::vector<uint8_t> body_;
};

// ================================================================================================
// Response Implementation
// ================================================================================================

class Response {
public:
    Response(int status_code, std::string reason_phrase, Version version);
    
    std::string get_header(const std::string& name) const;
    void set_header(std::string name, std::string value);
    void set_body(std::vector<uint8_t> body);
    void set_body(std::string body);
    std::string to_string() const;
    
    int status_code_;
    std::string reason_phrase_;
    Version version_;
    HeaderMap headers_;
    std::vector<uint8_t> body_;
};

// ================================================================================================
// Parser Types
// ================================================================================================

enum class ParseError {
    INCOMPLETE,
    INVALID_FORMAT
};

template<typename T>
using ParseResult = std::expected<T, ParseError>;

// ================================================================================================
// RequestParser Implementation
// ================================================================================================

class RequestParser {
public:
    enum class ParseState {
        REQUEST_LINE,
        HEADERS,
        BODY,
        CHUNK_SIZE,
        CHUNK_DATA,
        CHUNK_TRAILERS,
        ERROR
    };
    
    enum class ChunkState {
        SIZE,
        DATA
    };
    
    RequestParser();
    void reset();
    ParseResult<std::pair<Request, size_t>> parse(std::span<const uint8_t> data);
    bool parse_request_line();
    bool parse_headers();
    void setup_body_parsing();
    bool parse_body();
    bool parse_chunk_size();
    bool parse_chunk_data();
    bool parse_chunk_trailers();
    std::optional<size_t> find_line_end() const;
    void consume_line(size_t line_length);
    void set_error(std::string message);
    ParseResult<std::pair<Request, size_t>> finalize_request();
    std::expected<Request, ParseError> get_request() const;
    
    ParseState state_;
    std::optional<Request> current_request_;
    std::vector<uint8_t> buffer_;
    size_t body_bytes_remaining_;
    bool is_chunked_;
    size_t chunk_size_;
    ChunkState chunk_state_;
    std::string error_message_;
};

// ================================================================================================
// Utility Functions
// ================================================================================================

std::string url_decode(std::string_view url);
std::string url_encode(std::string_view str);
std::unordered_map<std::string, std::string> parse_query_string(std::string_view query);

} // namespace networking::http
