/*
 * Recursive Descent Parser with Backtracking (PEG Style)
 * 
 * Source: Parsing Expression Grammars (PEG), Packrat parsing
 * Pattern: Recursive descent with memoization and backtracking
 * 
 * What Makes It Ingenious:
 * - Packrat parsing: Memoization prevents exponential backtracking
 * - Ordered choice: First match wins (PEG semantics)
 * - Left recursion handling: Transforms left-recursive rules
 * - Memoization: O(n) time for unambiguous grammars
 * - Used in PEG parsers, parser combinators, language implementations
 * 
 * When to Use:
 * - Parsing Expression Grammars (PEG)
 * - Parser combinators
 * - Language parsers
 * - Expression parsing with precedence
 * - Packrat parsing
 * 
 * Real-World Usage:
 * - PEG parser generators (PEG.js, pyparsing)
 * - Parser combinators (Parsec, attoparsec)
 * - Language implementations
 * - Expression parsers
 * 
 * Time Complexity: O(n) with memoization, O(2^n) without
 * Space Complexity: O(n) for memoization table
 */

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <iostream>
#include <optional>

struct ParseResult {
    bool success;
    size_t position;
    std::string value;
    std::shared_ptr<void> ast;  // Abstract syntax tree node
    
    ParseResult(bool s, size_t p, const std::string& v = "")
        : success(s), position(p), value(v), ast(nullptr) {}
    
    static ParseResult success_result(size_t pos, const std::string& val = "") {
        return ParseResult(true, pos, val);
    }
    
    static ParseResult failure_result(size_t pos) {
        return ParseResult(false, pos);
    }
};

class PackratParser {
private:
    std::string input_;
    size_t pos_;
    
    // Memoization table: (rule_name, position) -> result
    std::unordered_map<std::string, 
                      std::unordered_map<size_t, ParseResult>> memo_;
    
    char current() const {
        return (pos_ < input_.size()) ? input_[pos_] : '\0';
    }
    
    void advance() {
        if (pos_ < input_.size()) pos_++;
    }
    
    void skip_whitespace() {
        while (pos_ < input_.size() && std::isspace(input_[pos_])) {
            pos_++;
        }
    }
    
    // Check memoization
    std::optional<ParseResult> get_memo(const std::string& rule, size_t pos) {
        auto it = memo_.find(rule);
        if (it != memo_.end()) {
            auto pos_it = it->second.find(pos);
            if (pos_it != it->second.end()) {
                return pos_it->second;
            }
        }
        return std::nullopt;
    }
    
    // Store in memoization
    void set_memo(const std::string& rule, size_t pos, const ParseResult& result) {
        memo_[rule][pos] = result;
    }
    
public:
    PackratParser(const std::string& input) : input_(input), pos_(0) {}
    
    void reset() {
        pos_ = 0;
        memo_.clear();
    }
    
    // Terminal: match literal string
    ParseResult match_literal(const std::string& literal) {
        size_t start_pos = pos_;
        
        for (char c : literal) {
            if (current() != c) {
                pos_ = start_pos;
                return ParseResult::failure_result(start_pos);
            }
            advance();
        }
        
        return ParseResult::success_result(pos_, literal);
    }
    
    // Terminal: match character class
    ParseResult match_char_class(std::function<bool(char)> predicate, 
                                 const std::string& name = "") {
        size_t start_pos = pos_;
        
        if (predicate(current())) {
            std::string value(1, current());
            advance();
            return ParseResult::success_result(pos_, value);
        }
        
        pos_ = start_pos;
        return ParseResult::failure_result(start_pos);
    }
    
    // Non-terminal with memoization
    ParseResult parse_with_memo(const std::string& rule_name,
                                std::function<ParseResult()> parser) {
        size_t start_pos = pos_;
        
        // Check memoization
        auto memo_result = get_memo(rule_name, start_pos);
        if (memo_result.has_value()) {
            pos_ = memo_result->position;
            return memo_result.value();
        }
        
        // Parse
        ParseResult result = parser();
        
        // Store in memoization
        set_memo(rule_name, start_pos, result);
        
        return result;
    }
    
    // Ordered choice (PEG: first match wins)
    ParseResult ordered_choice(const std::vector<std::function<ParseResult()>>& alternatives) {
        size_t start_pos = pos_;
        
        for (auto& alt : alternatives) {
            pos_ = start_pos;  // Reset position for each alternative
            ParseResult result = alt();
            
            if (result.success) {
                return result;
            }
        }
        
        pos_ = start_pos;
        return ParseResult::failure_result(start_pos);
    }
    
    // Sequence: all must succeed
    ParseResult sequence(const std::vector<std::function<ParseResult()>>& parsers) {
        size_t start_pos = pos_;
        std::string combined_value;
        
        for (auto& parser : parsers) {
            ParseResult result = parser();
            if (!result.success) {
                pos_ = start_pos;
                return ParseResult::failure_result(start_pos);
            }
            combined_value += result.value;
        }
        
        return ParseResult::success_result(pos_, combined_value);
    }
    
    // Zero or more (Kleene star)
    ParseResult zero_or_more(std::function<ParseResult()> parser) {
        std::string value;
        size_t start_pos = pos_;
        
        while (true) {
            ParseResult result = parser();
            if (!result.success) {
                break;
            }
            value += result.value;
        }
        
        pos_ = start_pos + value.length();
        return ParseResult::success_result(pos_, value);
    }
    
    // One or more (Kleene plus)
    ParseResult one_or_more(std::function<ParseResult()> parser) {
        ParseResult first = parser();
        if (!first.success) {
            return ParseResult::failure_result(pos_);
        }
        
        std::string value = first.value;
        
        while (true) {
            ParseResult result = parser();
            if (!result.success) {
                break;
            }
            value += result.value;
        }
        
        return ParseResult::success_result(pos_, value);
    }
    
    // Optional
    ParseResult optional(std::function<ParseResult()> parser) {
        size_t start_pos = pos_;
        ParseResult result = parser();
        
        if (!result.success) {
            pos_ = start_pos;
            return ParseResult::success_result(start_pos, "");
        }
        
        return result;
    }
    
    // And predicate (lookahead, doesn't consume)
    ParseResult and_predicate(std::function<ParseResult()> parser) {
        size_t start_pos = pos_;
        ParseResult result = parser();
        pos_ = start_pos;  // Don't consume input
        
        if (result.success) {
            return ParseResult::success_result(start_pos, "");
        } else {
            return ParseResult::failure_result(start_pos);
        }
    }
    
    // Not predicate (negative lookahead)
    ParseResult not_predicate(std::function<ParseResult()> parser) {
        size_t start_pos = pos_;
        ParseResult result = parser();
        pos_ = start_pos;  // Don't consume input
        
        if (!result.success) {
            return ParseResult::success_result(start_pos, "");
        } else {
            return ParseResult::failure_result(start_pos);
        }
    }
    
    // Expression parser with precedence (using memoization)
    ParseResult parse_expression() {
        return parse_with_memo("expression", [this]() {
            return parse_additive();
        });
    }
    
    ParseResult parse_additive() {
        return parse_with_memo("additive", [this]() {
            return ordered_choice({
                [this]() {
                    auto left = parse_multiplicative();
                    if (!left.success) return left;
                    
                    auto op = ordered_choice({
                        [this]() { return match_literal("+"); },
                        [this]() { return match_literal("-"); }
                    });
                    if (!op.success) return left;
                    
                    auto right = parse_additive();
                    if (!right.success) return ParseResult::failure_result(pos_);
                    
                    return ParseResult::success_result(right.position, 
                        left.value + op.value + right.value);
                },
                [this]() { return parse_multiplicative(); }
            });
        });
    }
    
    ParseResult parse_multiplicative() {
        return parse_with_memo("multiplicative", [this]() {
            return ordered_choice({
                [this]() {
                    auto left = parse_primary();
                    if (!left.success) return left;
                    
                    auto op = ordered_choice({
                        [this]() { return match_literal("*"); },
                        [this]() { return match_literal("/"); }
                    });
                    if (!op.success) return left;
                    
                    auto right = parse_multiplicative();
                    if (!right.success) return ParseResult::failure_result(pos_);
                    
                    return ParseResult::success_result(right.position,
                        left.value + op.value + right.value);
                },
                [this]() { return parse_primary(); }
            });
        });
    }
    
    ParseResult parse_primary() {
        return parse_with_memo("primary", [this]() {
            return ordered_choice({
                [this]() {
                    if (!match_literal("(").success) {
                        return ParseResult::failure_result(pos_);
                    }
                    auto expr = parse_expression();
                    if (!expr.success) {
                        return ParseResult::failure_result(pos_);
                    }
                    if (!match_literal(")").success) {
                        return ParseResult::failure_result(pos_);
                    }
                    return ParseResult::success_result(pos_, "(" + expr.value + ")");
                },
                [this]() {
                    return match_char_class([](char c) { return std::isdigit(c); }, "digit");
                }
            });
        });
    }
};

// Example usage
int main() {
    // Test expression parsing
    std::string expression = "1+2*3";
    PackratParser parser(expression);
    
    ParseResult result = parser.parse_expression();
    
    if (result.success) {
        std::cout << "Parsed expression: " << expression << std::endl;
        std::cout << "Result: " << result.value << std::endl;
        std::cout << "Position: " << result.position << " / " << expression.length() << std::endl;
    } else {
        std::cout << "Failed to parse expression" << std::endl;
    }
    
    // Test with parentheses
    std::string expr2 = "(1+2)*3";
    parser = PackratParser(expr2);
    result = parser.parse_expression();
    
    if (result.success) {
        std::cout << "\nParsed expression: " << expr2 << std::endl;
        std::cout << "Result: " << result.value << std::endl;
    } else {
        std::cout << "Failed to parse expression" << std::endl;
    }
    
    return 0;
}

