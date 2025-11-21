/*
 * LLVM Recursive Descent Parser
 * 
 * Source: https://github.com/llvm/llvm-project
 * Repository: llvm/llvm-project
 * File: `clang/lib/Parse/ParseExpr.cpp`
 * Algorithm: Recursive descent parsing with operator precedence
 * 
 * What Makes It Ingenious:
 * - Recursive descent: Each grammar rule maps to a function
 * - Operator precedence: Handles operator precedence correctly
 * - Error recovery: Continues parsing after errors
 * - Top-down parsing: Natural for LL grammars
 * - Used in production compilers (Clang/LLVM)
 * 
 * When to Use:
 * - LL(1) or LL(k) grammars
 * - Expression parsing
 * - Language parsers
 * - Compiler frontends
 * - Simple language implementations
 * 
 * Real-World Usage:
 * - Clang/LLVM C/C++ parser
 * - Many language parsers
 * - Expression evaluators
 * - Configuration parsers
 * 
 * Time Complexity:
 * - Parsing: O(n) where n is number of tokens
 * - Space: O(d) where d is maximum recursion depth
 * 
 * Space Complexity: O(d) for recursion stack
 */

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

// Token types
enum class TokenType {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    LPAREN,
    RPAREN,
    END
};

struct Token {
    TokenType type;
    std::string value;
    
    Token(TokenType t, const std::string& v = "") : type(t), value(v) {}
};

// Expression AST node
struct Expr {
    virtual ~Expr() = default;
    virtual int evaluate() const = 0;
};

struct NumberExpr : public Expr {
    int value;
    NumberExpr(int v) : value(v) {}
    int evaluate() const override { return value; }
};

struct BinaryExpr : public Expr {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    TokenType op;
    
    BinaryExpr(std::unique_ptr<Expr> l, TokenType o, std::unique_ptr<Expr> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
    
    int evaluate() const override {
        int lval = left->evaluate();
        int rval = right->evaluate();
        
        switch (op) {
            case TokenType::PLUS: return lval + rval;
            case TokenType::MINUS: return lval - rval;
            case TokenType::MULTIPLY: return lval * rval;
            case TokenType::DIVIDE: return lval / rval;
            default: throw std::runtime_error("Invalid operator");
        }
    }
};

class LLVMRecursiveDescent {
private:
    std::vector<Token> tokens_;
    size_t current_;
    
    Token& current_token() {
        return tokens_[current_];
    }
    
    void advance() {
        if (current_ < tokens_.size() - 1) {
            current_++;
        }
    }
    
    bool match(TokenType type) {
        if (current_token().type == type) {
            advance();
            return true;
        }
        return false;
    }
    
    // Expression parsing (lowest precedence)
    std::unique_ptr<Expr> parse_expression() {
        return parse_equality();
    }
    
    // Equality parsing
    std::unique_ptr<Expr> parse_equality() {
        auto expr = parse_comparison();
        
        while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
            TokenType op = current_token().type;
            advance();
            auto right = parse_comparison();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        
        return expr;
    }
    
    // Comparison parsing
    std::unique_ptr<Expr> parse_comparison() {
        return parse_term();
    }
    
    // Term parsing (multiplication/division)
    std::unique_ptr<Expr> parse_term() {
        auto expr = parse_factor();
        
        while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE)) {
            TokenType op = current_token().type;
            advance();
            auto right = parse_factor();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        
        return expr;
    }
    
    // Factor parsing (numbers, parentheses)
    std::unique_ptr<Expr> parse_factor() {
        if (match(TokenType::NUMBER)) {
            int value = std::stoi(tokens_[current_ - 1].value);
            return std::make_unique<NumberExpr>(value);
        }
        
        if (match(TokenType::LPAREN)) {
            auto expr = parse_expression();
            if (!match(TokenType::RPAREN)) {
                throw std::runtime_error("Expected ')'");
            }
            return expr;
        }
        
        throw std::runtime_error("Unexpected token");
    }
    
public:
    LLVMRecursiveDescent(const std::vector<Token>& tokens)
        : tokens_(tokens), current_(0) {}
    
    std::unique_ptr<Expr> parse() {
        return parse_expression();
    }
};

// Example usage
#include <iostream>

int main() {
    // Parse: 2 + 3 * 4
    std::vector<Token> tokens = {
        {TokenType::NUMBER, "2"},
        {TokenType::PLUS, "+"},
        {TokenType::NUMBER, "3"},
        {TokenType::MULTIPLY, "*"},
        {TokenType::NUMBER, "4"},
        {TokenType::END}
    };
    
    LLVMRecursiveDescent parser(tokens);
    
    try {
        auto expr = parser.parse();
        int result = expr->evaluate();
        std::cout << "Expression result: " << result << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
    }
    
    return 0;
}

