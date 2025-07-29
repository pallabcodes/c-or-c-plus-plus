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

/**
 * @brief HTTP method enumeration
 * 
 * Comprehensive list of HTTP methods as defined in various RFCs.
 * Designed for efficient comparison and switch statements.
 */
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

// Method helpers
std::string_view to_string(Method method);
std::expected<Method, std::string> parse_method(std::string_view method_str);

struct Version {
    int major = 1;
    int minor = 1;
    std::string to_string() const;
    static std::expected<Version, std::string> parse(std::string_view version_str);
    bool operator==(const Version& other) const = default;
};

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

enum class ParseError {
    INCOMPLETE,
    INVALID_FORMAT
};

template<typename T>
using ParseResult = std::expected<T, ParseError>;

class RequestParser {
public:
    RequestParser();
    void reset();
    ParseResult parse(std::span<const uint8_t> data);
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
    ParseResult finalize_request();
    std::expected<Request, ParseError> get_request() const;
    enum class ParseState {
        REQUEST_LINE,
        HEADERS,
        BODY,
        CHUNK_SIZE,
        CHUNK_DATA,
        CHUNK_TRAILERS,
        ERROR
    };
    ParseState state_;
    std::optional<Request> current_request_;
    std::vector<uint8_t> buffer_;
    size_t body_bytes_remaining_;
    bool is_chunked_;
    size_t chunk_size_;
    enum class ChunkState { SIZE, DATA } chunk_state_;
    std::string error_message_;
};

// Utility functions
std::string url_decode(std::string_view url);
std::string url_encode(std::string_view str);
std::unordered_map<std::string, std::string> parse_query_string(std::string_view query);

/**
 * @brief HTTP parsing error types
 */
enum class ParseError {
    INCOMPLETE_MESSAGE,      // Need more data
    INVALID_REQUEST_LINE,    // Malformed request line
    INVALID_STATUS_LINE,     // Malformed status line  
    INVALID_HEADER,          // Malformed header
    INVALID_METHOD,          // Unknown HTTP method
    INVALID_VERSION,         // Unsupported HTTP version
    INVALID_STATUS_CODE,     // Invalid status code
    HEADER_TOO_LARGE,        // Headers exceed size limit
    BODY_TOO_LARGE,          // Body exceeds size limit
    INVALID_CONTENT_LENGTH,  // Invalid Content-Length header
    CHUNK_SIZE_INVALID,      // Invalid chunk size in chunked encoding
    PROTOCOL_ERROR           // General protocol violation
};

/**
 * @brief Parse result type
 */
template<typename T>
using ParseResult = std::expected<T, ParseError>;

/**
 * @brief HTTP parser configuration
 */
struct ParserConfig {
    size_t max_header_size = 8192;        // 8KB header limit
    size_t max_body_size = 1024 * 1024;   // 1MB body limit  
    size_t max_headers = 100;             // Maximum number of headers
    bool strict_parsing = true;           // Strict RFC compliance
    bool allow_chunk_extensions = false;  // Allow chunk extensions
};

/**
 * @brief Incremental HTTP request parser
 * 
 * Designed for high-performance parsing with:
 * - Incremental parsing (can handle partial data)
 * - Zero-copy operation where possible
 * - Memory-efficient parsing
 * - Strict RFC compliance with optional relaxed mode
 */
class RequestParser {
public:
    /**
     * @brief Parser state for incremental parsing
     */
    enum class State {
        REQUEST_LINE,
        HEADERS,
        BODY,
        CHUNKED_BODY,
        COMPLETE,
        ERROR
    };
    
    explicit RequestParser(const ParserConfig& config = {});
    
    /**
     * @brief Parse HTTP request from buffer
     * @param data Input data buffer
     * @param length Length of input data
     * @return Parse result with bytes consumed
     */
    ParseResult<std::pair<Request, size_t>> parse(const uint8_t* data, size_t length);
    
    /**
     * @brief Reset parser state for next request
     */
    void reset();
    
    /**
     * @brief Get current parser state
     */
    State state() const { return state_; }
    
    /**
     * @brief Check if parser needs more data
     */
    bool needs_more_data() const;
    
    /**
     * @brief Get number of bytes parsed so far
     */
    size_t bytes_parsed() const { return total_parsed_; }

private:
    ParserConfig config_;
    State state_ = State::REQUEST_LINE;
    
    // Parsing buffers
    std::string buffer_;
    size_t buffer_offset_ = 0;
    size_t total_parsed_ = 0;
    
    // Parsed components
    Method method_;
    std::string target_;
    Version version_;
    HeaderMap headers_;
    std::vector<uint8_t> body_;
    
    // Content-Length parsing
    std::optional<size_t> content_length_;
    size_t body_bytes_read_ = 0;
    
    // Chunked encoding state
    enum class ChunkState {
        SIZE,
        EXTENSION,
        DATA,
        TRAILER,
        DONE
    } chunk_state_ = ChunkState::SIZE;
    
    size_t current_chunk_size_ = 0;
    size_t chunk_bytes_read_ = 0;
    
    // Internal parsing methods
    ParseResult<size_t> parse_request_line(const uint8_t* data, size_t length);
    ParseResult<size_t> parse_headers(const uint8_t* data, size_t length);
    ParseResult<size_t> parse_body(const uint8_t* data, size_t length);
    ParseResult<size_t> parse_chunked_body(const uint8_t* data, size_t length);
    
    // Helper methods
    std::optional<size_t> find_line_end(const uint8_t* data, size_t length, size_t offset);
    ParseResult<void> parse_header_line(std::string_view line);
    ParseResult<size_t> parse_chunk_size(const uint8_t* data, size_t length, size_t offset);
    
    bool is_complete() const;
    void transition_to_body();
};

/**
 * @brief HTTP response parser (similar to request parser)
 */
class ResponseParser {
public:
    enum class State {
        STATUS_LINE,
        HEADERS, 
        BODY,
        CHUNKED_BODY,
        COMPLETE,
        ERROR
    };
    
    explicit ResponseParser(const ParserConfig& config = {});
    
    ParseResult<std::pair<Response, size_t>> parse(const uint8_t* data, size_t length);
    void reset();
    
    State state() const { return state_; }
    bool needs_more_data() const;
    size_t bytes_parsed() const { return total_parsed_; }

private:
    ParserConfig config_;
    State state_ = State::STATUS_LINE;
    
    std::string buffer_;
    size_t buffer_offset_ = 0;
    size_t total_parsed_ = 0;
    
    Version version_;
    uint16_t status_code_;
    std::string reason_phrase_;
    HeaderMap headers_;
    std::vector<uint8_t> body_;
    
    std::optional<size_t> content_length_;
    size_t body_bytes_read_ = 0;
    
    // Similar chunked encoding state as RequestParser
    // ... (implementation details)
    
    ParseResult<size_t> parse_status_line(const uint8_t* data, size_t length);
    ParseResult<size_t> parse_headers(const uint8_t* data, size_t length);
    ParseResult<size_t> parse_body(const uint8_t* data, size_t length);
};

/**
 * @brief HTTP message builder for creating requests/responses
 */
class RequestBuilder {
public:
    RequestBuilder() = default;
    
    RequestBuilder& method(Method method);
    RequestBuilder& target(std::string target);
    RequestBuilder& version(Version version);
    RequestBuilder& header(std::string name, std::string value);
    RequestBuilder& headers(HeaderMap headers);
    RequestBuilder& body(std::vector<uint8_t> body);
    RequestBuilder& body(std::string_view body);
    
    Request build();

private:
    Method method_ = Method::GET;
    std::string target_ = "/";
    Version version_{1, 1};
    HeaderMap headers_;
    std::vector<uint8_t> body_;
};

class ResponseBuilder {
public:
    ResponseBuilder() = default;
    
    ResponseBuilder& version(Version version);
    ResponseBuilder& status(uint16_t status_code, std::string reason_phrase = "");
    ResponseBuilder& header(std::string name, std::string value);
    ResponseBuilder& headers(HeaderMap headers);
    ResponseBuilder& body(std::vector<uint8_t> body);
    ResponseBuilder& body(std::string_view body);
    
    Response build();

private:
    Version version_{1, 1};
    uint16_t status_code_ = 200;
    std::string reason_phrase_ = "OK";
    HeaderMap headers_;
    std::vector<uint8_t> body_;
};

/**
 * @brief Utility functions for HTTP
 */
namespace utils {

/**
 * @brief Get standard reason phrase for status code
 */
std::string_view get_reason_phrase(uint16_t status_code);

/**
 * @brief Check if status code indicates success (2xx)
 */
bool is_success_status(uint16_t status_code);

/**
 * @brief Check if status code indicates client error (4xx)
 */
bool is_client_error_status(uint16_t status_code);

/**
 * @brief Check if status code indicates server error (5xx)
 */
bool is_server_error_status(uint16_t status_code);

/**
 * @brief URL decode string
 */
std::string url_decode(std::string_view encoded);

/**
 * @brief URL encode string
 */
std::string url_encode(std::string_view str);

/**
 * @brief Parse query string into key-value pairs
 */
std::unordered_map<std::string, std::string> parse_query_string(std::string_view query);

/**
 * @brief Format HTTP date string (RFC 7231)
 */
std::string format_http_date(std::chrono::system_clock::time_point time);

/**
 * @brief Parse HTTP date string
 */
std::optional<std::chrono::system_clock::time_point> parse_http_date(std::string_view date_str);

} // namespace utils

} // namespace networking::http
