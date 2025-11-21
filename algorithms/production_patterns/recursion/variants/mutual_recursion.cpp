/*
 * Mutual Recursion Patterns
 * 
 * Source: Various production codebases and functional programming
 * Pattern: Functions that call each other recursively
 * 
 * What Makes It Ingenious:
 * - Natural for mutually recursive data structures
 * - Grammar parsing: Non-terminals that reference each other
 * - State machines: States that transition to each other
 * - Even/odd problems: Natural mutual recursion
 * - Used in compilers, parsers, interpreters
 * 
 * When to Use:
 * - Mutually recursive data structures
 * - Grammar parsing with mutually recursive rules
 * - State machines with recursive states
 * - Problems with natural even/odd structure
 * - Compiler AST traversal
 * 
 * Real-World Usage:
 * - Compiler parsers (expression/statement parsing)
 * - Interpreter implementations
 * - Grammar-based code generation
 * - State machine implementations
 * - Tree traversal with different node types
 * 
 * Time Complexity: Depends on problem structure
 * Space Complexity: O(d) where d is recursion depth
 */

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <cctype>

class MutualRecursion {
public:
    // Even/Odd mutual recursion
    static bool is_even(int n) {
        if (n == 0) {
            return true;  // Base case: 0 is even
        } else if (n < 0) {
            return is_even(-n);  // Handle negative numbers
        } else {
            return is_odd(n - 1);  // n is even if (n-1) is odd
        }
    }
    
    static bool is_odd(int n) {
        if (n == 0) {
            return false;  // Base case: 0 is not odd
        } else if (n < 0) {
            return is_odd(-n);  // Handle negative numbers
        } else {
            return is_even(n - 1);  // n is odd if (n-1) is even
        }
    }
    
    // Expression/Statement parsing (simplified)
    // Expression can contain statements, statements can contain expressions
    struct ASTNode {
        virtual ~ASTNode() = default;
        virtual void print(int indent = 0) const = 0;
    };
    
    struct Expression : public ASTNode {
        std::string value;
        Expression(const std::string& v) : value(v) {}
        void print(int indent = 0) const override {
            std::cout << std::string(indent, ' ') << "Expression: " << value << std::endl;
        }
    };
    
    struct Statement : public ASTNode {
        std::vector<std::unique_ptr<ASTNode>> children;
        std::string type;
        Statement(const std::string& t) : type(t) {}
        void print(int indent = 0) const override {
            std::cout << std::string(indent, ' ') << "Statement: " << type << std::endl;
            for (const auto& child : children) {
                child->print(indent + 2);
            }
        }
    };
    
    // Parser with mutual recursion
    class Parser {
    private:
        std::string input_;
        size_t pos_;
        
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
        
    public:
        Parser(const std::string& input) : input_(input), pos_(0) {}
        
        // Parse expression (can call parse_statement)
        std::unique_ptr<ASTNode> parse_expression() {
            skip_whitespace();
            
            if (current() == '{') {
                // Expression contains a statement block
                return parse_statement();
            }
            
            // Simple expression: read until whitespace or special char
            std::string value;
            while (pos_ < input_.size() && 
                   !std::isspace(input_[pos_]) && 
                   input_[pos_] != '}' && 
                   input_[pos_] != ';') {
                value += current();
                advance();
            }
            
            return std::make_unique<Expression>(value);
        }
        
        // Parse statement (can call parse_expression)
        std::unique_ptr<ASTNode> parse_statement() {
            skip_whitespace();
            
            if (current() == '{') {
                // Block statement
                advance();  // skip '{'
                skip_whitespace();
                
                auto stmt = std::make_unique<Statement>("block");
                
                while (current() != '}' && current() != '\0') {
                    // Statement can contain expressions
                    auto child = parse_expression();
                    if (child) {
                        stmt->children.push_back(std::move(child));
                    }
                    skip_whitespace();
                    
                    if (current() == ';') {
                        advance();
                    }
                }
                
                if (current() == '}') {
                    advance();  // skip '}'
                }
                
                return stmt;
            }
            
            // Simple statement: parse as expression
            return parse_expression();
        }
    };
    
    // State machine with mutual recursion
    enum class State { A, B, C };
    
    static void state_a(int count) {
        if (count <= 0) {
            std::cout << "State A: done" << std::endl;
            return;
        }
        std::cout << "State A: count = " << count << std::endl;
        state_b(count - 1);  // Transition to state B
    }
    
    static void state_b(int count) {
        if (count <= 0) {
            std::cout << "State B: done" << std::endl;
            return;
        }
        std::cout << "State B: count = " << count << std::endl;
        state_c(count - 1);  // Transition to state C
    }
    
    static void state_c(int count) {
        if (count <= 0) {
            std::cout << "State C: done" << std::endl;
            return;
        }
        std::cout << "State C: count = " << count << std::endl;
        state_a(count - 1);  // Transition back to state A
    }
    
    // Tree traversal with different node types
    struct BaseNode {
        virtual ~BaseNode() = default;
        virtual void traverse() = 0;
    };
    
    struct InternalNode;
    struct LeafNode;
    
    struct InternalNode : public BaseNode {
        std::vector<std::unique_ptr<BaseNode>> children;
        
        void traverse() override {
            std::cout << "InternalNode: traversing " << children.size() << " children" << std::endl;
            for (auto& child : children) {
                child->traverse();  // Calls traverse on child (could be Internal or Leaf)
            }
        }
    };
    
    struct LeafNode : public BaseNode {
        int value;
        LeafNode(int v) : value(v) {}
        
        void traverse() override {
            std::cout << "LeafNode: value = " << value << std::endl;
        }
    };
    
    // Ackermann function (mutual recursion variant)
    static int ackermann(int m, int n) {
        if (m == 0) {
            return n + 1;
        } else if (n == 0) {
            return ackermann(m - 1, 1);  // Recursive call
        } else {
            return ackermann(m - 1, ackermann(m, n - 1));  // Nested mutual recursion
        }
    }
    
    // Hofstadter Q sequence (mutual recursion)
    static int hofstadter_q(int n) {
        if (n <= 2) {
            return 1;
        }
        // Q(n) = Q(n - Q(n-1)) + Q(n - Q(n-2))
        return hofstadter_q(n - hofstadter_q(n - 1)) + 
               hofstadter_q(n - hofstadter_q(n - 2));
    }
};

// Example usage
int main() {
    // Even/Odd
    std::cout << "Even/Odd mutual recursion:" << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << i << " is " 
                  << (MutualRecursion::is_even(i) ? "even" : "odd") << std::endl;
    }
    
    // Parser
    std::cout << "\nParser with mutual recursion:" << std::endl;
    std::string code = "{ x y z; { a b; } }";
    MutualRecursion::Parser parser(code);
    auto ast = parser.parse_statement();
    if (ast) {
        ast->print();
    }
    
    // State machine
    std::cout << "\nState machine with mutual recursion:" << std::endl;
    MutualRecursion::state_a(5);
    
    // Tree traversal
    std::cout << "\nTree traversal with different node types:" << std::endl;
    auto root = std::make_unique<MutualRecursion::InternalNode>();
    auto child1 = std::make_unique<MutualRecursion::InternalNode>();
    child1->children.push_back(std::make_unique<MutualRecursion::LeafNode>(1));
    child1->children.push_back(std::make_unique<MutualRecursion::LeafNode>(2));
    root->children.push_back(std::move(child1));
    root->children.push_back(std::make_unique<MutualRecursion::LeafNode>(3));
    root->traverse();
    
    // Ackermann function
    std::cout << "\nAckermann function (mutual recursion):" << std::endl;
    std::cout << "A(2, 2) = " << MutualRecursion::ackermann(2, 2) << std::endl;
    std::cout << "A(3, 1) = " << MutualRecursion::ackermann(3, 1) << std::endl;
    
    // Hofstadter Q sequence
    std::cout << "\nHofstadter Q sequence:" << std::endl;
    for (int i = 1; i <= 10; i++) {
        std::cout << "Q(" << i << ") = " << MutualRecursion::hofstadter_q(i) << std::endl;
    }
    
    return 0;
}

