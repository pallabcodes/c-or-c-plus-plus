#include "utils/utils.h"
#include <algorithm>
#include <random>
#include <sstream>
#include <cctype>
#include <functional>
#include <expected>

namespace utils {

// ================================================================================================
// Logger Implementation
// ================================================================================================

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::set_output_file(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (output_file_.is_open()) {
        output_file_.close();
    }
    
    if (!filename.empty()) {
        output_file_.open(filename, std::ios::app);
        if (!output_file_.is_open()) {
            std::cerr << "Failed to open log file: " << filename << std::endl;
        }
    }
}

// ================================================================================================
// ThreadPool Implementation
// ================================================================================================

ThreadPool::ThreadPool(size_t num_threads) {
    workers_.reserve(num_threads);
    
    for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back([this]() { worker_loop(); });
    }
}

ThreadPool::~ThreadPool() {
    // Signal all workers to stop
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stopping_ = true;
    }
    condition_.notify_all();
    
    // Wait for all workers to finish
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::worker_loop() {
    while (true) {
        std::function<void()> task;
        
        {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this]() { return stopping_ || !tasks_.empty(); });
            
            if (stopping_ && tasks_.empty()) {
                break;
            }
            
            if (!tasks_.empty()) {
                task = std::move(tasks_.front());
                tasks_.pop();
            }
        }
        
        if (task) {
            try {
                task();
            } catch (const std::exception& e) {
                LOG_ERROR("Task execution failed: {}", e.what());
            } catch (...) {
                LOG_ERROR("Task execution failed with unknown exception");
            }
        }
    }
}

// ================================================================================================
// JsonValue Implementation
// ================================================================================================

std::string JsonValue::to_string() const {
    switch (type_) {
        case Type::NULL_VALUE:
            return "null";
            
        case Type::BOOLEAN:
            return bool_value_ ? "true" : "false";
            
        case Type::NUMBER:
            return std::to_string(number_value_);
            
        case Type::STRING: {
            std::string result = "\"";
            for (char c : string_value_) {
                switch (c) {
                    case '"': result += "\\\""; break;
                    case '\\': result += "\\\\"; break;
                    case '\b': result += "\\b"; break;
                    case '\f': result += "\\f"; break;
                    case '\n': result += "\\n"; break;
                    case '\r': result += "\\r"; break;
                    case '\t': result += "\\t"; break;
                    default:
                        if (c < 0x20) {
                            result += std::format("\\u{:04x}", static_cast<unsigned char>(c));
                        } else {
                            result += c;
                        }
                        break;
                }
            }
            result += "\"";
            return result;
        }
        
        case Type::ARRAY: {
            std::string result = "[";
            for (size_t i = 0; i < array_value_.size(); ++i) {
                if (i > 0) result += ",";
                result += array_value_[i].to_string();
            }
            result += "]";
            return result;
        }
        
        case Type::OBJECT: {
            std::string result = "{";
            bool first = true;
            for (const auto& [key, value] : object_value_) {
                if (!first) result += ",";
                first = false;
                
                result += "\"" + key + "\":" + value.to_string();
            }
            result += "}";
            return result;
        }
    }
    
    return "null";
}

// ================================================================================================
// JsonParser Implementation
// ================================================================================================

std::expected<JsonValue, std::string> JsonParser::parse(std::string_view json) {
    JsonParser parser(json);
    
    try {
        parser.skip_whitespace();
        if (parser.pos_ >= parser.json_.length()) {
            return std::unexpected("Empty JSON input");
        }
        
        JsonValue value = parser.parse_value();
        
        parser.skip_whitespace();
        if (parser.pos_ < parser.json_.length()) {
            return std::unexpected("Unexpected characters after JSON value");
        }
        
        return value;
    } catch (const std::exception& e) {
        return std::unexpected(e.what());
    }
}

JsonValue JsonParser::parse_value() {
    skip_whitespace();
    char c = peek();
    
    switch (c) {
        case '"':
            return parse_string();
        case '{':
            return parse_object();
        case '[':
            return parse_array();
        case 't':
        case 'f':
        case 'n':
            return parse_literal();
        default:
            if (c == '-' || std::isdigit(c)) {
                return parse_number();
            }
            throw std::runtime_error("Unexpected character");
    }
}

JsonValue JsonParser::parse_object() {
    consume(); // '{'
    JsonValue object;
    
    skip_whitespace();
    if (peek() == '}') {
        consume();
        return object;
    }
    
    while (true) {
        skip_whitespace();
        
        // Parse key
        if (peek() != '"') {
            throw std::runtime_error("Expected string key in object");
        }
        JsonValue key_value = parse_string();
        std::string key = key_value.as_string();
        
        skip_whitespace();
        if (consume() != ':') {
            throw std::runtime_error("Expected ':' after object key");
        }
        
        // Parse value
        JsonValue value = parse_value();
        object.set(key, std::move(value));
        
        skip_whitespace();
        char next = consume();
        if (next == '}') {
            break;
        } else if (next == ',') {
            continue;
        } else {
            throw std::runtime_error("Expected ',' or '}' in object");
        }
    }
    
    return object;
}

JsonValue JsonParser::parse_array() {
    consume(); // '['
    JsonValue array;
    
    skip_whitespace();
    if (peek() == ']') {
        consume();
        return array;
    }
    
    while (true) {
        JsonValue value = parse_value();
        array.push_back(std::move(value));
        
        skip_whitespace();
        char next = consume();
        if (next == ']') {
            break;
        } else if (next == ',') {
            continue;
        } else {
            throw std::runtime_error("Expected ',' or ']' in array");
        }
    }
    
    return array;
}

JsonValue JsonParser::parse_string() {
    consume(); // '"'
    std::string content = parse_string_content();
    return JsonValue(std::move(content));
}

JsonValue JsonParser::parse_number() {
    size_t start = pos_;
    
    if (peek() == '-') {
        consume();
    }
    
    if (!std::isdigit(peek())) {
        throw std::runtime_error("Invalid number format");
    }
    
    if (peek() == '0') {
        consume();
    } else {
        while (std::isdigit(peek())) {
            consume();
        }
    }
    
    if (peek() == '.') {
        consume();
        if (!std::isdigit(peek())) {
            throw std::runtime_error("Invalid number format: no digits after decimal");
        }
        while (std::isdigit(peek())) {
            consume();
        }
    }
    
    if (peek() == 'e' || peek() == 'E') {
        consume();
        if (peek() == '+' || peek() == '-') {
            consume();
        }
        if (!std::isdigit(peek())) {
            throw std::runtime_error("Invalid number format: no digits in exponent");
        }
        while (std::isdigit(peek())) {
            consume();
        }
    }
    
    std::string number_str(json_.substr(start, pos_ - start));
    double value = std::stod(number_str);
    return JsonValue(value);
}

JsonValue JsonParser::parse_literal() {
    if (json_.substr(pos_, 4) == "true") {
        pos_ += 4;
        return JsonValue(true);
    } else if (json_.substr(pos_, 5) == "false") {
        pos_ += 5;
        return JsonValue(false);
    } else if (json_.substr(pos_, 4) == "null") {
        pos_ += 4;
        return JsonValue();
    } else {
        throw std::runtime_error("Invalid literal");
    }
}

void JsonParser::skip_whitespace() {
    while (pos_ < json_.length() && std::isspace(json_[pos_])) {
        pos_++;
    }
}

char JsonParser::peek() const {
    return pos_ < json_.length() ? json_[pos_] : '\0';
}

char JsonParser::consume() {
    return pos_ < json_.length() ? json_[pos_++] : '\0';
}

std::string JsonParser::parse_string_content() {
    std::string result;
    
    while (peek() != '"' && peek() != '\0') {
        char c = consume();
        
        if (c == '\\') {
            char escaped = consume();
            switch (escaped) {
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                case 'u': {
                    // Unicode escape sequence
                    std::string hex;
                    for (int i = 0; i < 4; ++i) {
                        char hex_char = consume();
                        if (!std::isxdigit(hex_char)) {
                            throw std::runtime_error("Invalid unicode escape sequence");
                        }
                        hex += hex_char;
                    }
                    
                    uint16_t codepoint = static_cast<uint16_t>(std::stoul(hex, nullptr, 16));
                    
                    // Convert to UTF-8
                    if (codepoint < 0x80) {
                        result += static_cast<char>(codepoint);
                    } else if (codepoint < 0x800) {
                        result += static_cast<char>(0xC0 | (codepoint >> 6));
                        result += static_cast<char>(0x80 | (codepoint & 0x3F));
                    } else {
                        result += static_cast<char>(0xE0 | (codepoint >> 12));
                        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                        result += static_cast<char>(0x80 | (codepoint & 0x3F));
                    }
                    break;
                }
                default:
                    throw std::runtime_error("Invalid escape sequence");
            }
        } else {
            result += c;
        }
    }
    
    if (consume() != '"') {
        throw std::runtime_error("Unterminated string");
    }
    
    return result;
}

// ================================================================================================
// String Utilities Implementation
// ================================================================================================

std::string trim(std::string_view str) {
    auto start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string_view::npos) {
        return "";
    }
    
    auto end = str.find_last_not_of(" \t\n\r\f\v");
    return std::string(str.substr(start, end - start + 1));
}

std::vector<std::string> split(std::string_view str, char delimiter) {
    std::vector<std::string> result;
    size_t start = 0;
    
    while (start < str.length()) {
        size_t end = str.find(delimiter, start);
        if (end == std::string_view::npos) {
            end = str.length();
        }
        
        result.emplace_back(str.substr(start, end - start));
        start = end + 1;
    }
    
    return result;
}

std::string join(const std::vector<std::string>& parts, std::string_view delimiter) {
    if (parts.empty()) {
        return "";
    }
    
    std::string result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result += delimiter;
        result += parts[i];
    }
    
    return result;
}

std::string to_lower(std::string_view str) {
    std::string result;
    result.reserve(str.length());
    
    std::transform(str.begin(), str.end(), std::back_inserter(result),
                   [](char c) { return std::tolower(c); });
    
    return result;
}

std::string to_upper(std::string_view str) {
    std::string result;
    result.reserve(str.length());
    
    std::transform(str.begin(), str.end(), std::back_inserter(result),
                   [](char c) { return std::toupper(c); });
    
    return result;
}

bool starts_with(std::string_view str, std::string_view prefix) {
    return str.length() >= prefix.length() && 
           str.substr(0, prefix.length()) == prefix;
}

bool ends_with(std::string_view str, std::string_view suffix) {
    return str.length() >= suffix.length() && 
           str.substr(str.length() - suffix.length()) == suffix;
}

// ================================================================================================
// Random Utilities Implementation
// ================================================================================================

std::string random_string(size_t length, std::string_view charset) {
    static thread_local std::random_device rd;
    static thread_local std::mt19937 gen(rd());
    
    std::uniform_int_distribution<size_t> dis(0, charset.length() - 1);
    
    std::string result;
    result.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        result += charset[dis(gen)];
    }
    
    return result;
}

int random_int(int min, int max) {
    static thread_local std::random_device rd;
    static thread_local std::mt19937 gen(rd());
    
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}

std::string random_uuid() {
    static thread_local std::random_device rd;
    static thread_local std::mt19937 gen(rd());
    static thread_local std::uniform_int_distribution<int> dis(0, 15);
    
    constexpr const char* hex_chars = "0123456789abcdef";
    
    std::string uuid = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
    
    for (char& c : uuid) {
        if (c == 'x') {
            c = hex_chars[dis(gen)];
        } else if (c == 'y') {
            c = hex_chars[(dis(gen) & 0x3) | 0x8]; // Set variant bits
        }
    }
    
    return uuid;
}

} // namespace utils
