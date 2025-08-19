#include "http/http_parser.h"
#include <algorithm>
#include <sstream>
#include <cctype>

namespace networking::http {

// ================================================================================================
// HTTP Method Implementation
// ================================================================================================

std::string_view to_string(Method method) {
    switch (method) {
        case Method::GET: return "GET";
        case Method::POST: return "POST";
        case Method::PUT: return "PUT";
        case Method::DELETE: return "DELETE";
        case Method::HEAD: return "HEAD";
        case Method::OPTIONS: return "OPTIONS";
        case Method::PATCH: return "PATCH";
        case Method::TRACE: return "TRACE";
        case Method::CONNECT: return "CONNECT";
        default: return "UNKNOWN";
    }
}

std::expected<Method, std::string> parse_method(std::string_view method_str) {
    if (method_str == "GET") return Method::GET;
    if (method_str == "POST") return Method::POST;
    if (method_str == "PUT") return Method::PUT;
    if (method_str == "DELETE") return Method::DELETE;
    if (method_str == "HEAD") return Method::HEAD;
    if (method_str == "OPTIONS") return Method::OPTIONS;
    if (method_str == "PATCH") return Method::PATCH;
    if (method_str == "TRACE") return Method::TRACE;
    if (method_str == "CONNECT") return Method::CONNECT;
    
    return std::unexpected("Invalid HTTP method: " + std::string(method_str));
}

// ================================================================================================
// Version Implementation
// ================================================================================================

std::string Version::to_string() const {
    return "HTTP/" + std::to_string(major) + "." + std::to_string(minor);
}

std::expected<Version, std::string> Version::parse(std::string_view version_str) {
    if (!version_str.starts_with("HTTP/")) {
        return std::unexpected("Invalid HTTP version format");
    }
    
    version_str.remove_prefix(5); // Remove "HTTP/"
    
    auto dot_pos = version_str.find('.');
    if (dot_pos == std::string_view::npos) {
        return std::unexpected("Invalid HTTP version format: missing dot");
    }
    
    try {
        int major = std::stoi(std::string(version_str.substr(0, dot_pos)));
        int minor = std::stoi(std::string(version_str.substr(dot_pos + 1)));
        
        if (major < 0 || major > 9 || minor < 0 || minor > 9) {
            return std::unexpected("Invalid HTTP version numbers");
        }
        
        return Version{major, minor};
    } catch (const std::exception&) {
        return std::unexpected("Invalid HTTP version format: non-numeric");
    }
}

// ================================================================================================
// Request Implementation
// ================================================================================================

Request::Request(Method method, std::string uri, Version version)
    : method_(method), uri_(std::move(uri)), version_(version) {}

std::string Request::get_header(const std::string& name) const {
    return headers_.get(name);
}

void Request::set_header(std::string name, std::string value) {
    headers_.set(std::move(name), std::move(value));
}

void Request::set_body(std::vector<uint8_t> body) {
    body_ = std::move(body);
    set_header("Content-Length", std::to_string(body_.size()));
}

void Request::set_body(std::string body) {
    body_.assign(body.begin(), body.end());
    set_header("Content-Length", std::to_string(body_.size()));
}

bool Request::has_body() const {
    return !body_.empty() || 
           (!get_header("Content-Length").empty() && get_header("Content-Length") != "0") ||
           get_header("Transfer-Encoding") == "chunked";
}

std::string Request::to_string() const {
    std::ostringstream ss;
    
    // Request line
    ss << ::networking::http::to_string(method_) << " " << uri_ << " " 
       << version_.to_string() << "\r\n";
    
    // Headers
    ss << headers_.to_string();
    
    // Empty line
    ss << "\r\n";
    
    // Body
    if (!body_.empty()) {
        ss.write(reinterpret_cast<const char*>(body_.data()), body_.size());
    }
    
    return ss.str();
}

// ================================================================================================
// Response Implementation
// ================================================================================================

Response::Response(int status_code, std::string reason_phrase, Version version)
    : status_code_(status_code), reason_phrase_(std::move(reason_phrase)), version_(version) {}

std::string Response::get_header(const std::string& name) const {
    return headers_.get(name);
}

void Response::set_header(std::string name, std::string value) {
    headers_.set(std::move(name), std::move(value));
}

void Response::set_body(std::vector<uint8_t> body) {
    body_ = std::move(body);
    set_header("Content-Length", std::to_string(body_.size()));
}

void Response::set_body(std::string body) {
    body_.assign(body.begin(), body.end());
    set_header("Content-Length", std::to_string(body_.size()));
}

std::string Response::to_string() const {
    std::ostringstream ss;
    
    // Status line
    ss << version_.to_string() << " " << status_code_ << " " << reason_phrase_ << "\r\n";
    
    // Headers
    ss << headers_.to_string();
    
    // Empty line
    ss << "\r\n";
    
    // Body
    if (!body_.empty()) {
        ss.write(reinterpret_cast<const char*>(body_.data()), body_.size());
    }
    
    return ss.str();
}

// ================================================================================================
// HeaderMap Implementation
// ================================================================================================

std::string HeaderMap::get(const std::string& name) const {
    auto it = headers_.find(normalize_header_name(name));
    return it != headers_.end() ? it->second : "";
}

void HeaderMap::set(std::string name, std::string value) {
    headers_[normalize_header_name(name)] = std::move(value);
}

void HeaderMap::add(std::string name, std::string value) {
    std::string normalized = normalize_header_name(name);
    auto it = headers_.find(normalized);
    
    if (it != headers_.end()) {
        it->second += ", " + value; // HTTP allows multiple values separated by commas
    } else {
        headers_[std::move(normalized)] = std::move(value);
    }
}

void HeaderMap::remove(const std::string& name) {
    headers_.erase(normalize_header_name(name));
}

bool HeaderMap::contains(const std::string& name) const {
    return headers_.find(normalize_header_name(name)) != headers_.end();
}

void HeaderMap::clear() {
    headers_.clear();
}

std::string HeaderMap::to_string() const {
    std::ostringstream ss;
    
    for (const auto& [name, value] : headers_) {
        ss << name << ": " << value << "\r\n";
    }
    
    return ss.str();
}

std::string HeaderMap::normalize_header_name(const std::string& name) {
    std::string normalized;
    normalized.reserve(name.length());
    
    bool capitalize_next = true;
    for (char c : name) {
        if (c == '-') {
            normalized += '-';
            capitalize_next = true;
        } else if (capitalize_next) {
            normalized += std::toupper(c);
            capitalize_next = false;
        } else {
            normalized += std::tolower(c);
        }
    }
    
    return normalized;
}

// ================================================================================================
// RequestParser Implementation
// ================================================================================================

RequestParser::RequestParser() { reset(); }

void RequestParser::reset() {
    state_ = ParseState::REQUEST_LINE;
    current_request_ = std::nullopt;
    buffer_.clear();
    body_bytes_remaining_ = 0;
    is_chunked_ = false;
    chunk_size_ = 0;
    chunk_state_ = ChunkState::SIZE;
    error_message_.clear();
}

ParseResult<std::pair<Request, size_t>> RequestParser::parse(std::span<const uint8_t> data) {
    // Append new data to buffer
    buffer_.insert(buffer_.end(), data.begin(), data.end());
    
    while (true) {
        switch (state_) {
            case ParseState::REQUEST_LINE:
                if (auto result = parse_request_line(); !result) {
                    return std::unexpected(ParseError::INCOMPLETE);
                }
                state_ = ParseState::HEADERS;
                break;
                
            case ParseState::HEADERS:
                if (auto result = parse_headers(); !result) {
                    return std::unexpected(ParseError::INCOMPLETE);
                }
                
                // Check if request has body
                setup_body_parsing();
                
                if (body_bytes_remaining_ == 0 && !is_chunked_) {
                    // No body, request is complete
                    return finalize_request();
                }
                
                state_ = is_chunked_ ? ParseState::CHUNK_SIZE : ParseState::BODY;
                break;
                
            case ParseState::BODY:
                if (auto result = parse_body(); !result) {
                    return std::unexpected(ParseError::INCOMPLETE);
                }
                return finalize_request();
                
            case ParseState::CHUNK_SIZE:
                if (auto result = parse_chunk_size(); !result) {
                    return std::unexpected(ParseError::INCOMPLETE);
                }
                
                if (chunk_size_ == 0) {
                    state_ = ParseState::CHUNK_TRAILERS;
                } else {
                    state_ = ParseState::CHUNK_DATA;
                }
                break;
                
            case ParseState::CHUNK_DATA:
                if (auto result = parse_chunk_data(); !result) {
                    return std::unexpected(ParseError::INCOMPLETE);
                }
                state_ = ParseState::CHUNK_SIZE;
                break;
                
            case ParseState::CHUNK_TRAILERS:
                if (auto result = parse_chunk_trailers(); !result) {
                    return std::unexpected(ParseError::INCOMPLETE);
                }
                return finalize_request();
                
            case ParseState::ERROR:
                return std::unexpected(ParseError::INVALID_FORMAT);
        }
    }
}

bool RequestParser::parse_request_line() {
    auto line_end = find_line_end();
    if (!line_end) return false;
    
    std::string line(buffer_.begin(), buffer_.begin() + *line_end);
    consume_line(*line_end);
    
    // Parse "METHOD URI HTTP/VERSION"
    std::istringstream iss(line);
    std::string method_str, uri, version_str;
    
    if (!(iss >> method_str >> uri >> version_str)) {
        set_error("Invalid request line format");
        return false;
    }
    
    // Parse method
    auto method_result = parse_method(method_str);
    if (!method_result) {
        set_error(method_result.error());
        return false;
    }
    
    // Parse version
    auto version_result = Version::parse(version_str);
    if (!version_result) {
        set_error(version_result.error());
        return false;
    }
    
    // Create request
    current_request_ = Request(*method_result, std::move(uri), *version_result);
    return true;
}

bool RequestParser::parse_headers() {
    while (true) {
        auto line_end = find_line_end();
        if (!line_end) return false;
        
        if (*line_end == 0) {
            // Empty line, headers are complete
            consume_line(*line_end);
            return true;
        }
        
        std::string line(buffer_.begin(), buffer_.begin() + *line_end);
        consume_line(*line_end);
        
        // Parse "Name: Value"
        auto colon_pos = line.find(':');
        if (colon_pos == std::string::npos) {
            set_error("Invalid header format");
            return false;
        }
        
        std::string name = line.substr(0, colon_pos);
        std::string value = line.substr(colon_pos + 1);
        
        // Trim whitespace
        name.erase(name.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        current_request_->set_header(std::move(name), std::move(value));
    }
}

void RequestParser::setup_body_parsing() {
    std::string transfer_encoding = current_request_->get_header("Transfer-Encoding");
    std::string content_length = current_request_->get_header("Content-Length");
    
    if (transfer_encoding == "chunked") {
        is_chunked_ = true;
        body_bytes_remaining_ = 0;
    } else if (!content_length.empty()) {
        try {
            body_bytes_remaining_ = std::stoull(content_length);
            is_chunked_ = false;
        } catch (const std::exception&) {
            set_error("Invalid Content-Length header");
        }
    } else {
        body_bytes_remaining_ = 0;
        is_chunked_ = false;
    }
}

bool RequestParser::parse_body() {
    if (buffer_.size() < body_bytes_remaining_) {
        return false; // Need more data
    }
    
    // Extract body
    std::vector<uint8_t> body(buffer_.begin(), buffer_.begin() + body_bytes_remaining_);
    buffer_.erase(buffer_.begin(), buffer_.begin() + body_bytes_remaining_);
    
    current_request_->set_body(std::move(body));
    body_bytes_remaining_ = 0;
    
    return true;
}

bool RequestParser::parse_chunk_size() {
    auto line_end = find_line_end();
    if (!line_end) return false;
    
    std::string line(buffer_.begin(), buffer_.begin() + *line_end);
    consume_line(*line_end);
    
    // Parse hex chunk size (ignore extensions after ';')
    auto semicolon_pos = line.find(';');
    if (semicolon_pos != std::string::npos) {
        line = line.substr(0, semicolon_pos);
    }
    
    try {
        chunk_size_ = std::stoull(line, nullptr, 16);
    } catch (const std::exception&) {
        set_error("Invalid chunk size");
        return false;
    }
    
    return true;
}

bool RequestParser::parse_chunk_data() {
    if (buffer_.size() < chunk_size_ + 2) { // +2 for CRLF after chunk
        return false; // Need more data
    }
    
    // Extract chunk data
    auto& body = current_request_->body_;
    size_t old_size = body.size();
    body.resize(old_size + chunk_size_);
    std::copy(buffer_.begin(), buffer_.begin() + chunk_size_, body.begin() + old_size);
    
    // Consume chunk data and CRLF
    buffer_.erase(buffer_.begin(), buffer_.begin() + chunk_size_ + 2);
    
    return true;
}

bool RequestParser::parse_chunk_trailers() {
    // For now, just consume until empty line (no trailer support)
    while (true) {
        auto line_end = find_line_end();
        if (!line_end) return false;
        
        if (*line_end == 0) {
            // Empty line, chunked encoding is complete
            consume_line(*line_end);
            return true;
        }
        
        consume_line(*line_end); // Ignore trailer headers for now
    }
}

std::optional<size_t> RequestParser::find_line_end() const {
    for (size_t i = 0; i < buffer_.size() - 1; ++i) {
        if (buffer_[i] == '\r' && buffer_[i + 1] == '\n') {
            return i;
        }
    }
    return std::nullopt;
}

void RequestParser::consume_line(size_t line_length) {
    buffer_.erase(buffer_.begin(), buffer_.begin() + line_length + 2); // +2 for CRLF
}

void RequestParser::set_error(std::string message) {
    state_ = ParseState::ERROR;
    error_message_ = std::move(message);
}

ParseResult<std::pair<Request, size_t>> RequestParser::finalize_request() {
    if (!current_request_) {
        return std::unexpected(ParseError::INCOMPLETE);
    }
    
    // Calculate consumed bytes
    // This is a simplification - in practice, you'd track this more precisely
    size_t consumed = 0; // Would need to implement proper tracking
    
    Request request = std::move(*current_request_);
    reset(); // Reset for next request
    
    return std::make_pair(std::move(request), consumed);
}

std::expected<Request, ParseError> RequestParser::get_request() const {
    if (state_ == ParseState::ERROR) {
        return std::unexpected(ParseError::INVALID_FORMAT);
    }
    
    if (!current_request_) {
        return std::unexpected(ParseError::INCOMPLETE);
    }
    
    return *current_request_;
}

// ================================================================================================
// Utility Functions
// ================================================================================================

std::string url_decode(std::string_view url) {
    std::string decoded;
    decoded.reserve(url.length());
    
    for (size_t i = 0; i < url.length(); ++i) {
        if (url[i] == '%' && i + 2 < url.length()) {
            // Parse hex encoded character
            char hex_str[3] = {static_cast<char>(url[i + 1]), static_cast<char>(url[i + 2]), '\0'};
            char* end;
            unsigned long value = std::strtoul(hex_str, &end, 16);
            
            if (end == hex_str + 2) {
                decoded += static_cast<char>(value);
                i += 2; // Skip the hex digits
            } else {
                decoded += url[i]; // Invalid encoding, keep as-is
            }
        } else if (url[i] == '+') {
            decoded += ' '; // '+' represents space in form encoding
        } else {
            decoded += url[i];
        }
    }
    
    return decoded;
}

std::string url_encode(std::string_view str) {
    std::string encoded;
    encoded.reserve(str.length() * 3); // Worst case: all characters encoded
    
    for (char c : str) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += c; // Unreserved characters
        } else {
            // Encode as %XX
            encoded += '%';
            
            char hex[3];
            std::sprintf(hex, "%02X", static_cast<unsigned char>(c));
            encoded += hex;
        }
    }
    
    return encoded;
}

std::unordered_map<std::string, std::string> parse_query_string(std::string_view query) {
    std::unordered_map<std::string, std::string> params;
    
    if (query.empty()) return params;
    
    // Remove leading '?' if present
    if (query.starts_with('?')) {
        query.remove_prefix(1);
    }
    
    size_t start = 0;
    while (start < query.length()) {
        // Find next '&' or end of string
        size_t end = query.find('&', start);
        if (end == std::string_view::npos) {
            end = query.length();
        }
        
        std::string_view param = query.substr(start, end - start);
        
        // Split on '='
        size_t equals = param.find('=');
        if (equals != std::string_view::npos) {
            std::string name = url_decode(param.substr(0, equals));
            std::string value = url_decode(param.substr(equals + 1));
            params[std::move(name)] = std::move(value);
        } else {
            // Parameter without value
            params[url_decode(param)] = "";
        }
        
        start = end + 1;
    }
    
    return params;
}

} // namespace networking::http
