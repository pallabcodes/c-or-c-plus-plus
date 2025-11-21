/*
 * OWASP Input Validation Patterns
 *
 * Source: OWASP Top 10, ESAPI, Input Validation Cheat Sheet
 * Algorithm: Multi-layered validation with sanitization and encoding
 *
 * What Makes It Ingenious:
 * - Positive validation (allow lists) over negative validation
 * - Context-aware validation and encoding
 * - Multi-layer defense (input, process, output)
 * - Canonicalization and normalization
 * - Type-safe validation with constraints
 * - Attack pattern recognition
 *
 * When to Use:
 * - Web application input handling
 * - API request validation
 * - Form data processing
 * - File upload validation
 * - User-generated content sanitization
 *
 * Real-World Usage:
 * - Web frameworks (Django, Rails, Spring)
 * - API gateways (Kong, Apigee)
 * - Content management systems
 * - E-commerce platforms
 * - Social media applications
 *
 * Time Complexity: O(n) for input size, O(m) for pattern matching
 * Space Complexity: O(n) for input processing, O(k) for validation rules
 */

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <algorithm>
#include <memory>
#include <functional>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <locale>

// Validation result
struct ValidationResult {
    bool valid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::string sanitized_value;

    void add_error(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }

    void add_warning(const std::string& warning) {
        warnings.push_back(warning);
    }

    bool has_errors() const { return !errors.empty(); }
    bool has_warnings() const { return !warnings.empty(); }
};

// Input types for context-aware validation
enum class InputType {
    GENERIC_TEXT,
    EMAIL,
    URL,
    SQL_IDENTIFIER,
    HTML_CONTENT,
    JAVASCRIPT_CODE,
    CSS_STYLESHEET,
    JSON_DATA,
    XML_DATA,
    FILE_PATH,
    COMMAND_LINE,
    NUMERIC,
    ALPHANUMERIC,
    CREDIT_CARD,
    PHONE_NUMBER,
    POSTAL_CODE,
    DATE,
    IP_ADDRESS,
    USERNAME,
    PASSWORD
};

// Validation severity levels
enum class ValidationSeverity {
    PERMISSIVE,    // Allow minor issues, add warnings
    STRICT,        // Reject on any validation issue
    SECURITY       // Maximum security, reject suspicious input
};

// Input context for validation
struct ValidationContext {
    InputType type = InputType::GENERIC_TEXT;
    ValidationSeverity severity = ValidationSeverity::STRICT;
    size_t max_length = 1000;
    size_t min_length = 0;
    bool allow_empty = false;
    std::string charset = "UTF-8";
    std::unordered_map<std::string, std::string> custom_rules;
    bool canonicalize = true;
};

// Base validator interface
class InputValidator {
public:
    virtual ~InputValidator() = default;
    virtual ValidationResult validate(const std::string& input,
                                    const ValidationContext& context) = 0;
    virtual std::string sanitize(const std::string& input,
                               const ValidationContext& context) = 0;
};

// OWASP-compliant input validator
class OWASPInputValidator : public InputValidator {
public:
    ValidationResult validate(const std::string& input,
                            const ValidationContext& context) override {
        ValidationResult result;
        result.sanitized_value = input;

        // Basic length checks
        if (input.length() > context.max_length) {
            result.add_error("Input exceeds maximum length of " +
                           std::to_string(context.max_length) + " characters");
        }

        if (input.length() < context.min_length) {
            result.add_error("Input is shorter than minimum length of " +
                           std::to_string(context.min_length) + " characters");
        }

        if (input.empty() && !context.allow_empty) {
            result.add_error("Input cannot be empty");
        }

        // Canonicalization (normalize input)
        if (context.canonicalize) {
            result.sanitized_value = canonicalize(input);
        }

        // Type-specific validation
        switch (context.type) {
            case InputType::EMAIL:
                validate_email(result, result.sanitized_value);
                break;
            case InputType::URL:
                validate_url(result, result.sanitized_value);
                break;
            case InputType::SQL_IDENTIFIER:
                validate_sql_identifier(result, result.sanitized_value);
                break;
            case InputType::HTML_CONTENT:
                validate_html(result, result.sanitized_value);
                break;
            case InputType::JAVASCRIPT_CODE:
                validate_javascript(result, result.sanitized_value);
                break;
            case InputType::JSON_DATA:
                validate_json(result, result.sanitized_value);
                break;
            case InputType::XML_DATA:
                validate_xml(result, result.sanitized_value);
                break;
            case InputType::FILE_PATH:
                validate_file_path(result, result.sanitized_value);
                break;
            case InputType::COMMAND_LINE:
                validate_command_line(result, result.sanitized_value);
                break;
            case InputType::NUMERIC:
                validate_numeric(result, result.sanitized_value);
                break;
            case InputType::CREDIT_CARD:
                validate_credit_card(result, result.sanitized_value);
                break;
            case InputType::IP_ADDRESS:
                validate_ip_address(result, result.sanitized_value);
                break;
            case InputType::USERNAME:
                validate_username(result, result.sanitized_value);
                break;
            case InputType::PASSWORD:
                validate_password(result, result.sanitized_value);
                break;
            default:
                validate_generic_text(result, result.sanitized_value, context);
                break;
        }

        // Security checks (apply to all types)
        perform_security_checks(result, result.sanitized_value, context);

        return result;
    }

    std::string sanitize(const std::string& input,
                       const ValidationContext& context) override {
        std::string sanitized = input;

        // Canonicalize
        if (context.canonicalize) {
            sanitized = canonicalize(sanitized);
        }

        // Type-specific sanitization
        switch (context.type) {
            case InputType::HTML_CONTENT:
                sanitized = sanitize_html(sanitized);
                break;
            case InputType::JAVASCRIPT_CODE:
                sanitized = sanitize_javascript(sanitized);
                break;
            case InputType::SQL_IDENTIFIER:
                sanitized = sanitize_sql_identifier(sanitized);
                break;
            case InputType::COMMAND_LINE:
                sanitized = sanitize_command_line(sanitized);
                break;
            case InputType::FILE_PATH:
                sanitized = sanitize_file_path(sanitized);
                break;
            default:
                sanitized = sanitize_generic_text(sanitized);
                break;
        }

        // Length limiting
        if (sanitized.length() > context.max_length) {
            sanitized = sanitized.substr(0, context.max_length);
        }

        return sanitized;
    }

private:
    // Canonicalization (normalize Unicode and encoding)
    std::string canonicalize(const std::string& input) {
        // Basic canonicalization - normalize whitespace and case
        std::string result = input;

        // Normalize whitespace
        auto it = std::unique(result.begin(), result.end(),
                            [](char a, char b) {
                                return std::isspace(a) && std::isspace(b);
                            });
        result.erase(it, result.end());

        // Trim leading/trailing whitespace
        result.erase(result.begin(),
                    std::find_if(result.begin(), result.end(),
                               [](char c) { return !std::isspace(c); }));
        result.erase(std::find_if(result.rbegin(), result.rend(),
                                [](char c) { return !std::isspace(c); }).base(),
                    result.end());

        return result;
    }

    // Email validation
    void validate_email(ValidationResult& result, const std::string& email) {
        static const std::regex email_pattern(
            R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
        );

        if (!std::regex_match(email, email_pattern)) {
            result.add_error("Invalid email format");
        }

        // Additional checks
        if (email.length() > 254) {
            result.add_error("Email address too long");
        }

        // Check for suspicious patterns
        if (email.find("..") != std::string::npos ||
            email.find("@.") != std::string::npos ||
            email.find(".@") != std::string::npos) {
            result.add_error("Invalid email structure");
        }
    }

    // URL validation
    void validate_url(ValidationResult& result, const std::string& url) {
        static const std::regex url_pattern(
            R"(^(https?|ftp)://[^\s/$.?#].[^\s]*$)"
        );

        if (!std::regex_match(url, url_pattern)) {
            result.add_error("Invalid URL format");
        }

        // Check for dangerous schemes
        if (url.find("javascript:") == 0 ||
            url.find("data:") == 0 ||
            url.find("vbscript:") == 0) {
            result.add_error("Dangerous URL scheme detected");
        }
    }

    // SQL identifier validation
    void validate_sql_identifier(ValidationResult& result, const std::string& identifier) {
        static const std::regex sql_id_pattern(R"(^[a-zA-Z_][a-zA-Z0-9_]*$)");

        if (!std::regex_match(identifier, sql_id_pattern)) {
            result.add_error("Invalid SQL identifier format");
        }

        // Check for SQL injection patterns
        if (identifier.find(";") != std::string::npos ||
            identifier.find("--") != std::string::npos ||
            identifier.find("/*") != std::string::npos) {
            result.add_error("Potential SQL injection detected");
        }
    }

    // HTML validation and sanitization
    void validate_html(ValidationResult& result, const std::string& html) {
        // Check for dangerous tags
        std::vector<std::string> dangerous_tags = {
            "<script", "<iframe", "<object", "<embed", "<form",
            "<input", "<button", "<link", "<meta"
        };

        std::string lower_html = html;
        std::transform(lower_html.begin(), lower_html.end(), lower_html.begin(), ::tolower);

        for (const auto& tag : dangerous_tags) {
            if (lower_html.find(tag) != std::string::npos) {
                result.add_error("Dangerous HTML tag detected: " + tag);
            }
        }

        // Check for event handlers
        if (lower_html.find("on") != std::string::npos) {
            std::vector<std::string> event_handlers = {
                "onclick", "onload", "onerror", "onmouseover", "onsubmit"
            };

            for (const auto& handler : event_handlers) {
                if (lower_html.find(handler) != std::string::npos) {
                    result.add_error("Dangerous event handler detected: " + handler);
                }
            }
        }
    }

    // JavaScript validation
    void validate_javascript(ValidationResult& result, const std::string& js) {
        // Check for dangerous constructs
        std::vector<std::string> dangerous_constructs = {
            "eval(", "Function(", "setTimeout(", "setInterval(",
            "document.", "window.", "location.", "XMLHttpRequest"
        };

        for (const auto& construct : dangerous_constructs) {
            if (js.find(construct) != std::string::npos) {
                result.add_error("Dangerous JavaScript construct detected: " + construct);
            }
        }
    }

    // JSON validation
    void validate_json(ValidationResult& result, const std::string& json) {
        // Basic JSON structure validation
        if (json.empty()) return;

        int brace_count = 0;
        int bracket_count = 0;
        bool in_string = false;
        char prev_char = '\0';

        for (char c : json) {
            if (c == '"' && prev_char != '\\') {
                in_string = !in_string;
            } else if (!in_string) {
                if (c == '{') brace_count++;
                else if (c == '}') brace_count--;
                else if (c == '[') bracket_count++;
                else if (c == ']') bracket_count--;

                if (brace_count < 0 || bracket_count < 0) {
                    result.add_error("Invalid JSON structure");
                    break;
                }
            }
            prev_char = c;
        }

        if (brace_count != 0 || bracket_count != 0) {
            result.add_error("Unbalanced JSON braces/brackets");
        }
    }

    // XML validation
    void validate_xml(ValidationResult& result, const std::string& xml) {
        // Basic XML structure validation
        std::vector<std::string> dangerous_entities = {
            "<!ENTITY", "<!DOCTYPE", "<?xml-stylesheet"
        };

        for (const auto& entity : dangerous_entities) {
            if (xml.find(entity) != std::string::npos) {
                result.add_error("Dangerous XML construct detected: " + entity);
            }
        }

        // Check for balanced tags (basic check)
        int tag_depth = 0;
        size_t pos = 0;
        while ((pos = xml.find('<', pos)) != std::string::npos) {
            if (xml[pos + 1] != '/' && xml[pos + 1] != '!' && xml[pos + 1] != '?') {
                tag_depth++;
            } else if (xml[pos + 1] == '/') {
                tag_depth--;
            }
            pos++;
        }

        if (tag_depth != 0) {
            result.add_error("Unbalanced XML tags");
        }
    }

    // File path validation
    void validate_file_path(ValidationResult& result, const std::string& path) {
        // Check for directory traversal
        if (path.find("..") != std::string::npos) {
            result.add_error("Directory traversal detected");
        }

        // Check for absolute paths
        if (path.find("/") == 0 || path.find("\\") == 0 ||
            (path.length() >= 3 && path[1] == ':' && (path[2] == '\\' || path[2] == '/'))) {
            result.add_error("Absolute path not allowed");
        }

        // Check for null bytes
        if (path.find('\0') != std::string::npos) {
            result.add_error("Null byte in path");
        }
    }

    // Command line validation
    void validate_command_line(ValidationResult& result, const std::string& cmd) {
        // Check for command injection patterns
        std::vector<std::string> dangerous_patterns = {
            ";", "|", "&", "`", "$(", "${", "&&", "||"
        };

        for (const auto& pattern : dangerous_patterns) {
            if (cmd.find(pattern) != std::string::npos) {
                result.add_error("Command injection pattern detected: " + pattern);
            }
        }
    }

    // Numeric validation
    void validate_numeric(ValidationResult& result, const std::string& num) {
        // Check if it's a valid number
        try {
            std::stod(num);
        } catch (const std::exception&) {
            result.add_error("Invalid numeric format");
        }

        // Check for leading zeros (potential octal interpretation)
        if (num.length() > 1 && num[0] == '0' && std::isdigit(num[1])) {
            result.add_warning("Leading zero in numeric input");
        }
    }

    // Credit card validation (basic)
    void validate_credit_card(ValidationResult& result, const std::string& card) {
        // Remove spaces and dashes
        std::string clean_card = card;
        clean_card.erase(std::remove_if(clean_card.begin(), clean_card.end(),
                                      [](char c) { return std::isspace(c) || c == '-'; }),
                        clean_card.end());

        // Basic length check
        if (clean_card.length() < 13 || clean_card.length() > 19) {
            result.add_error("Invalid credit card number length");
        }

        // Luhn algorithm check
        if (!luhn_check(clean_card)) {
            result.add_error("Invalid credit card number (failed Luhn check)");
        }

        // Don't store full card numbers in logs/warnings
        result.sanitized_value = std::string(clean_card.length(), 'X');
    }

    // IP address validation
    void validate_ip_address(ValidationResult& result, const std::string& ip) {
        static const std::regex ipv4_pattern(
            R"(^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)"
        );

        if (!std::regex_match(ip, ipv4_pattern)) {
            result.add_error("Invalid IPv4 address format");
        }
    }

    // Username validation
    void validate_username(ValidationResult& result, const std::string& username) {
        // Length checks
        if (username.length() < 3) {
            result.add_error("Username too short (minimum 3 characters)");
        }

        if (username.length() > 32) {
            result.add_error("Username too long (maximum 32 characters)");
        }

        // Character checks
        static const std::regex username_pattern(R"(^[a-zA-Z0-9_-]+$)");

        if (!std::regex_match(username, username_pattern)) {
            result.add_error("Username contains invalid characters");
        }

        // Reserved names
        std::vector<std::string> reserved = {"admin", "root", "system", "guest"};

        std::string lower_username = username;
        std::transform(lower_username.begin(), lower_username.end(),
                      lower_username.begin(), ::tolower);

        for (const auto& reserved_name : reserved) {
            if (lower_username == reserved_name) {
                result.add_error("Username is reserved");
                break;
            }
        }
    }

    // Password validation
    void validate_password(ValidationResult& result, const std::string& password) {
        if (password.length() < 8) {
            result.add_error("Password too short (minimum 8 characters)");
        }

        bool has_upper = false, has_lower = false, has_digit = false, has_special = false;

        for (char c : password) {
            if (std::isupper(c)) has_upper = true;
            else if (std::islower(c)) has_lower = true;
            else if (std::isdigit(c)) has_digit = true;
            else if (!std::isspace(c)) has_special = true;
        }

        if (!has_upper) result.add_error("Password must contain uppercase letter");
        if (!has_lower) result.add_error("Password must contain lowercase letter");
        if (!has_digit) result.add_error("Password must contain digit");
        if (!has_special) result.add_error("Password must contain special character");

        // Check for common passwords
        std::vector<std::string> common_passwords = {
            "password", "123456", "qwerty", "admin", "letmein"
        };

        std::string lower_password = password;
        std::transform(lower_password.begin(), lower_password.end(),
                      lower_password.begin(), ::tolower);

        for (const auto& common : common_passwords) {
            if (lower_password == common) {
                result.add_error("Password is too common");
                break;
            }
        }
    }

    // Generic text validation
    void validate_generic_text(ValidationResult& result, const std::string& text,
                             const ValidationContext& context) {
        // Check for null bytes
        if (text.find('\0') != std::string::npos) {
            result.add_error("Null byte detected in input");
        }

        // Check for control characters
        for (char c : text) {
            if (std::iscntrl(c) && c != '\t' && c != '\n' && c != '\r') {
                result.add_error("Control character detected");
                break;
            }
        }
    }

    // Security checks applied to all inputs
    void perform_security_checks(ValidationResult& result, const std::string& input,
                               const ValidationContext& context) {
        // SQL injection patterns
        std::vector<std::string> sql_patterns = {
            "UNION SELECT", "DROP TABLE", "DELETE FROM", "UPDATE ", "INSERT INTO",
            "SELECT * FROM", "--", "/*", "*/", "XP_CMDSHELL", "EXEC("
        };

        for (const auto& pattern : sql_patterns) {
            if (input.find(pattern) != std::string::npos) {
                result.add_error("Potential SQL injection detected: " + pattern);
            }
        }

        // XSS patterns
        std::vector<std::string> xss_patterns = {
            "<script", "javascript:", "vbscript:", "onload=", "onerror=",
            "onmouseover=", "<iframe", "<object", "<embed"
        };

        std::string lower_input = input;
        std::transform(lower_input.begin(), lower_input.end(),
                      lower_input.begin(), ::tolower);

        for (const auto& pattern : xss_patterns) {
            if (lower_input.find(pattern) != std::string::npos) {
                result.add_error("Potential XSS attack detected: " + pattern);
            }
        }

        // Command injection patterns
        std::vector<std::string> cmd_patterns = {
            "|", ";", "&", "`", "$(", "${"
        };

        for (const auto& pattern : cmd_patterns) {
            if (input.find(pattern) != std::string::npos) {
                result.add_error("Potential command injection detected: " + pattern);
            }
        }

        // Path traversal
        if (input.find("..") != std::string::npos ||
            input.find("\\") != std::string::npos) {
            result.add_error("Potential path traversal detected");
        }
    }

    // Sanitization functions
    std::string sanitize_html(const std::string& html) {
        std::string sanitized = html;

        // Remove dangerous tags
        std::vector<std::string> dangerous_tags = {
            "script", "iframe", "object", "embed", "form", "input", "button"
        };

        for (const auto& tag : dangerous_tags) {
            std::string start_tag = "<" + tag;
            std::string end_tag = "</" + tag + ">";

            size_t pos = 0;
            while ((pos = sanitized.find(start_tag, pos)) != std::string::npos) {
                size_t end_pos = sanitized.find(">", pos);
                if (end_pos != std::string::npos) {
                    sanitized.erase(pos, end_pos - pos + 1);
                } else {
                    break;
                }
            }
        }

        // HTML encode special characters
        std::unordered_map<char, std::string> html_entities = {
            {'&', "&amp;"}, {'<', "&lt;"}, {'>', "&gt;"},
            {'"', "&quot;"}, {'\'', "&#x27;"}
        };

        for (const auto& entity : html_entities) {
            size_t pos = 0;
            while ((pos = sanitized.find(entity.first, pos)) != std::string::npos) {
                sanitized.replace(pos, 1, entity.second);
                pos += entity.second.length();
            }
        }

        return sanitized;
    }

    std::string sanitize_javascript(const std::string& js) {
        // Basic JavaScript sanitization - remove dangerous constructs
        std::string sanitized = js;

        std::vector<std::string> dangerous_constructs = {
            "eval", "Function", "setTimeout", "setInterval",
            "document", "window", "location"
        };

        for (const auto& construct : dangerous_constructs) {
            size_t pos = 0;
            while ((pos = sanitized.find(construct, pos)) != std::string::npos) {
                sanitized.replace(pos, construct.length(), "[REMOVED]");
                pos += 9; // length of "[REMOVED]"
            }
        }

        return sanitized;
    }

    std::string sanitize_sql_identifier(const std::string& identifier) {
        // Remove dangerous characters from SQL identifiers
        std::string sanitized;
        for (char c : identifier) {
            if (std::isalnum(c) || c == '_') {
                sanitized += c;
            }
        }
        return sanitized;
    }

    std::string sanitize_command_line(const std::string& cmd) {
        // Remove shell metacharacters
        std::string sanitized;
        for (char c : cmd) {
            if (c != '|' && c != ';' && c != '&' && c != '`' && c != '$') {
                sanitized += c;
            }
        }
        return sanitized;
    }

    std::string sanitize_file_path(const std::string& path) {
        // Remove dangerous path components
        std::string sanitized = path;

        // Remove .. sequences
        size_t pos = 0;
        while ((pos = sanitized.find("..", pos)) != std::string::npos) {
            sanitized.erase(pos, 2);
        }

        // Remove leading slashes/backslashes
        while (!sanitized.empty() && (sanitized[0] == '/' || sanitized[0] == '\\')) {
            sanitized.erase(0, 1);
        }

        return sanitized;
    }

    std::string sanitize_generic_text(const std::string& text) {
        std::string sanitized = text;

        // Remove null bytes
        sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), '\0'),
                       sanitized.end());

        // Remove control characters (except tabs, newlines, carriage returns)
        sanitized.erase(std::remove_if(sanitized.begin(), sanitized.end(),
                                     [](char c) {
                                         return std::iscntrl(c) &&
                                                c != '\t' && c != '\n' && c != '\r';
                                     }),
                       sanitized.end());

        return sanitized;
    }

    // Utility functions
    bool luhn_check(const std::string& card_number) {
        int sum = 0;
        bool alternate = false;

        for (int i = card_number.length() - 1; i >= 0; --i) {
            int digit = card_number[i] - '0';

            if (alternate) {
                digit *= 2;
                if (digit > 9) {
                    digit -= 9;
                }
            }

            sum += digit;
            alternate = !alternate;
        }

        return (sum % 10) == 0;
    }
};

// Multi-layer validation framework
class ValidationFramework {
public:
    ValidationFramework(std::unique_ptr<InputValidator> validator)
        : validator_(std::move(validator)) {}

    // Validate input with multiple layers
    ValidationResult validate_multilayer(const std::string& input,
                                       const ValidationContext& context) {
        ValidationResult result = validator_->validate(input, context);

        if (!result.valid) {
            return result; // Early exit on validation failure
        }

        // Layer 2: Business rule validation
        validate_business_rules(result, result.sanitized_value, context);

        // Layer 3: Threat detection
        validate_threat_patterns(result, result.sanitized_value, context);

        return result;
    }

    // Sanitize with multiple passes
    std::string sanitize_multilayer(const std::string& input,
                                  const ValidationContext& context) {
        std::string sanitized = validator_->sanitize(input, context);

        // Additional sanitization passes
        sanitized = apply_additional_sanitization(sanitized, context);

        return sanitized;
    }

private:
    void validate_business_rules(ValidationResult& result, const std::string& input,
                               const ValidationContext& context) {
        // Check custom business rules
        for (const auto& rule : context.custom_rules) {
            if (rule.first == "max_words") {
                int max_words = std::stoi(rule.second);
                int word_count = std::count_if(input.begin(), input.end(),
                                             [](char c) { return std::isspace(c); }) + 1;
                if (word_count > max_words) {
                    result.add_error("Too many words (maximum " + rule.second + ")");
                }
            }
            // Add more business rules as needed
        }
    }

    void validate_threat_patterns(ValidationResult& result, const std::string& input,
                                const ValidationContext& context) {
        if (context.severity == ValidationSeverity::SECURITY) {
            // Advanced threat detection for maximum security

            // Check for encoded attacks
            if (contains_encoded_attacks(input)) {
                result.add_error("Encoded attack patterns detected");
            }

            // Check for polymorphic attacks
            if (contains_polymorphic_attacks(input)) {
                result.add_error("Polymorphic attack patterns detected");
            }

            // Check for zero-width characters (steganography)
            if (contains_zero_width_chars(input)) {
                result.add_error("Suspicious Unicode characters detected");
            }
        }
    }

    std::string apply_additional_sanitization(const std::string& input,
                                            const ValidationContext& context) {
        std::string sanitized = input;

        // Apply context-specific additional sanitization
        if (context.type == InputType::HTML_CONTENT) {
            // Additional HTML sanitization
            sanitized = remove_data_urls(sanitized);
        }

        return sanitized;
    }

    bool contains_encoded_attacks(const std::string& input) {
        // Check for URL encoding of dangerous characters
        std::vector<std::string> encoded_attacks = {
            "%3C", "%3E", "%22", "%27", "%3B", "%7C"  // < > " ' ; |
        };

        for (const auto& attack : encoded_attacks) {
            if (input.find(attack) != std::string::npos) {
                return true;
            }
        }

        return false;
    }

    bool contains_polymorphic_attacks(const std::string& input) {
        // Check for attacks that change form
        // This is a simplified check - real implementation would be more sophisticated
        std::vector<std::string> polymorphic_patterns = {
            "<scr<script>ipt>", "&#x3C;script&#x3E;", "\\u003cscript\\u003e"
        };

        for (const auto& pattern : polymorphic_patterns) {
            if (input.find(pattern) != std::string::npos) {
                return true;
            }
        }

        return false;
    }

    bool contains_zero_width_chars(const std::string& input) {
        // Check for zero-width Unicode characters
        std::vector<char32_t> zero_width_chars = {
            0x200B, 0x200C, 0x200D, 0x200E, 0x200F,  // Various zero-width chars
            0xFEFF  // Zero-width no-break space
        };

        // Simplified check - real implementation would handle UTF-8 properly
        for (char32_t zwc : zero_width_chars) {
            if (zwc < 256) {  // Only check ASCII-range for demo
                if (input.find(static_cast<char>(zwc)) != std::string::npos) {
                    return true;
                }
            }
        }

        return false;
    }

    std::string remove_data_urls(const std::string& html) {
        std::string result = html;

        // Remove data: URLs which can contain embedded scripts
        size_t pos = 0;
        while ((pos = result.find("data:", pos)) != std::string::npos) {
            size_t end_pos = result.find("\"", pos);
            if (end_pos == std::string::npos) end_pos = result.find("'", pos);
            if (end_pos == std::string::npos) break;

            result.erase(pos, end_pos - pos);
        }

        return result;
    }

    std::unique_ptr<InputValidator> validator_;
};

// Example usage with web application
class WebApplicationValidator {
public:
    WebApplicationValidator() {
        framework_ = std::make_unique<ValidationFramework>(
            std::make_unique<OWASPInputValidator>());
    }

    // Validate user registration
    ValidationResult validate_user_registration(const std::string& username,
                                              const std::string& email,
                                              const std::string& password) {
        ValidationResult combined_result;
        combined_result.valid = true;

        // Validate username
        ValidationContext username_ctx{InputType::USERNAME, ValidationSeverity::STRICT, 32, 3};
        auto username_result = framework_->validate_multilayer(username, username_ctx);
        if (!username_result.valid) {
            combined_result.valid = false;
            combined_result.errors.insert(combined_result.errors.end(),
                                        username_result.errors.begin(),
                                        username_result.errors.end());
        }

        // Validate email
        ValidationContext email_ctx{InputType::EMAIL, ValidationSeverity::STRICT, 254};
        auto email_result = framework_->validate_multilayer(email, email_ctx);
        if (!email_result.valid) {
            combined_result.valid = false;
            combined_result.errors.insert(combined_result.errors.end(),
                                        email_result.errors.begin(),
                                        email_result.errors.end());
        }

        // Validate password
        ValidationContext password_ctx{InputType::PASSWORD, ValidationSeverity::STRICT, 128, 8};
        auto password_result = framework_->validate_multilayer(password, password_ctx);
        if (!password_result.valid) {
            combined_result.valid = false;
            combined_result.errors.insert(combined_result.errors.end(),
                                        password_result.errors.begin(),
                                        password_result.errors.end());
        }

        return combined_result;
    }

    // Validate blog post
    ValidationResult validate_blog_post(const std::string& title, const std::string& content) {
        ValidationResult combined_result;
        combined_result.valid = true;

        // Validate title
        ValidationContext title_ctx{InputType::GENERIC_TEXT, ValidationSeverity::STRICT, 200, 1};
        auto title_result = framework_->validate_multilayer(title, title_ctx);
        if (!title_result.valid) {
            combined_result.valid = false;
            combined_result.errors.insert(combined_result.errors.end(),
                                        title_result.errors.begin(),
                                        title_result.errors.end());
        }

        // Validate content (HTML allowed but sanitized)
        ValidationContext content_ctx{InputType::HTML_CONTENT, ValidationSeverity::STRICT, 10000, 1};
        auto content_result = framework_->validate_multilayer(content, content_ctx);
        if (!content_result.valid) {
            combined_result.valid = false;
            combined_result.errors.insert(combined_result.errors.end(),
                                        content_result.errors.begin(),
                                        content_result.errors.end());
        }

        return combined_result;
    }

    // Validate API request
    ValidationResult validate_api_request(const std::string& endpoint,
                                        const std::string& json_data) {
        ValidationResult combined_result;
        combined_result.valid = true;

        // Validate endpoint (should be a safe path)
        ValidationContext endpoint_ctx{InputType::FILE_PATH, ValidationSeverity::STRICT, 1000};
        auto endpoint_result = framework_->validate_multilayer(endpoint, endpoint_ctx);
        if (!endpoint_result.valid) {
            combined_result.valid = false;
            combined_result.errors.insert(combined_result.errors.end(),
                                        endpoint_result.errors.begin(),
                                        endpoint_result.errors.end());
        }

        // Validate JSON data
        ValidationContext json_ctx{InputType::JSON_DATA, ValidationSeverity::STRICT, 10000};
        auto json_result = framework_->validate_multilayer(json_data, json_ctx);
        if (!json_result.valid) {
            combined_result.valid = false;
            combined_result.errors.insert(combined_result.errors.end(),
                                        json_result.errors.begin(),
                                        json_result.errors.end());
        }

        return combined_result;
    }

    // Sanitize user input for display
    std::string sanitize_for_display(const std::string& input, InputType type) {
        ValidationContext context{type, ValidationSeverity::STRICT, 10000};
        return framework_->sanitize_multilayer(input, context);
    }

private:
    std::unique_ptr<ValidationFramework> framework_;
};

// Demo application
int main() {
    std::cout << "OWASP Input Validation Patterns Demo\n";
    std::cout << "===================================\n\n";

    WebApplicationValidator validator;

    // 1. User registration validation
    std::cout << "1. User Registration Validation:\n";

    std::vector<std::tuple<std::string, std::string, std::string>> test_registrations = {
        {"alice", "alice@example.com", "MySecurePass123!"},
        {"", "invalid-email", "weak"},
        {"admin", "admin@system.com", "password123"},
        {"user<script>", "user@example.com", "ValidPass123!"}
    };

    for (const auto& [username, email, password] : test_registrations) {
        std::cout << "Testing registration: " << username << ", " << email << "\n";
        auto result = validator.validate_user_registration(username, email, password);

        if (result.valid) {
            std::cout << "  ✓ Registration valid\n";
        } else {
            std::cout << "  ✗ Registration invalid:\n";
            for (const auto& error : result.errors) {
                std::cout << "    - " << error << "\n";
            }
        }
        std::cout << "\n";
    }

    // 2. Blog post validation
    std::cout << "2. Blog Post Validation:\n";

    std::string dangerous_html = R"(
        <h1>My Blog Post</h1>
        <p>This is safe content</p>
        <script>alert('XSS Attack!');</script>
        <iframe src="dangerous.com"></iframe>
        <p>More safe content & special chars < > " '</p>
    )";

    auto blog_result = validator.validate_blog_post("My Blog Post", dangerous_html);

    if (blog_result.valid) {
        std::cout << "✓ Blog post valid\n";
    } else {
        std::cout << "✗ Blog post invalid:\n";
        for (const auto& error : blog_result.errors) {
            std::cout << "  - " << error << "\n";
        }
    }

    std::string sanitized_html = validator.sanitize_for_display(dangerous_html, InputType::HTML_CONTENT);
    std::cout << "\nSanitized HTML:\n" << sanitized_html.substr(0, 200) << "...\n\n";

    // 3. API request validation
    std::cout << "3. API Request Validation:\n";

    std::string safe_json = R"(
        {
            "user_id": 123,
            "action": "update_profile",
            "data": {
                "name": "John Doe",
                "email": "john@example.com"
            }
        }
    )";

    std::string malicious_json = R"(
        {
            "user_id": 123,
            "action": "update_profile",
            "data": {
                "name": "<script>alert('XSS')</script>",
                "email": "john@example.com",
                "sql_injection": "'; DROP TABLE users; --"
            }
        }
    )";

    auto safe_api_result = validator.validate_api_request("/api/users/123", safe_json);
    std::cout << "Safe API request: " << (safe_api_result.valid ? "VALID" : "INVALID") << "\n";

    auto malicious_api_result = validator.validate_api_request("/api/users/123/../../../etc/passwd", malicious_json);
    std::cout << "Malicious API request: " << (malicious_api_result.valid ? "VALID" : "INVALID") << "\n";

    if (!malicious_api_result.valid) {
        for (const auto& error : malicious_api_result.errors) {
            std::cout << "  - " << error << "\n";
        }
    }

    // 4. Various input type validations
    std::cout << "\n4. Various Input Type Validations:\n";

    std::vector<std::pair<std::string, InputType>> test_inputs = {
        {"user@example.com", InputType::EMAIL},
        {"192.168.1.100", InputType::IP_ADDRESS},
        {"1234567890123456", InputType::CREDIT_CARD},
        {"SELECT * FROM users", InputType::SQL_IDENTIFIER},
        {"<script>alert('xss')</script>", InputType::HTML_CONTENT},
        {"../etc/passwd", InputType::FILE_PATH},
        {"ls; rm -rf /", InputType::COMMAND_LINE}
    };

    for (const auto& [input, type] : test_inputs) {
        ValidationContext ctx{type, ValidationSeverity::STRICT, 1000};
        auto result = validator.framework_->validate_multilayer(input, ctx);

        std::cout << "Input: \"" << input.substr(0, 30) << "\" ("
                  << (type == InputType::EMAIL ? "EMAIL" :
                      type == InputType::IP_ADDRESS ? "IP" :
                      type == InputType::CREDIT_CARD ? "CARD" :
                      type == InputType::SQL_IDENTIFIER ? "SQL" :
                      type == InputType::HTML_CONTENT ? "HTML" :
                      type == InputType::FILE_PATH ? "PATH" :
                      type == InputType::COMMAND_LINE ? "CMD" : "UNKNOWN")
                  << "): " << (result.valid ? "VALID" : "INVALID");

        if (!result.valid) {
            std::cout << " - " << result.errors[0];
        }
        std::cout << "\n";
    }

    std::cout << "\nDemo completed!\n";

    return 0;
}

/*
 * Key Features Demonstrated:
 *
 * 1. Multi-Layer Validation:
 *    - Input validation (format, length, type)
 *    - Business rule validation
 *    - Threat pattern detection
 *    - Context-aware sanitization
 *
 * 2. OWASP Compliance:
 *    - Positive validation (allow lists)
 *    - Safe encoding and sanitization
 *    - Attack pattern recognition
 *    - Canonicalization
 *
 * 3. Type-Specific Validation:
 *    - Email, URL, IP address validation
 *    - Credit card number validation (Luhn)
 *    - SQL identifier safety
 *    - HTML/JavaScript sanitization
 *    - File path security
 *
 * 4. Security Checks:
 *    - SQL injection prevention
 *    - XSS attack detection
 *    - Command injection protection
 *    - Path traversal prevention
 *    - Null byte attack prevention
 *
 * 5. Production Patterns:
 *    - Validation result aggregation
 *    - Sanitization with multiple passes
 *    - Context-aware severity levels
 *    - Comprehensive error reporting
 *
 * Real-World Applications:
 * - Web application frameworks (Django, Rails, Spring)
 * - API gateways (Kong, Apigee, AWS API Gateway)
 * - Content management systems (WordPress, Drupal)
 * - E-commerce platforms (Shopify, WooCommerce)
 * - Social media platforms (input sanitization)
 */
