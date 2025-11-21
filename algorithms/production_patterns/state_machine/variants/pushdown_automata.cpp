/*
 * Pushdown Automata
 *
 * Source: Compiler design, formal language theory, parsing algorithms
 * Repository: Parser generators, formal verification, language processors
 * Files: Context-free language recognition, parsing algorithms, compiler theory
 * Algorithm: Finite state machine + stack for context-free grammar recognition
 *
 * What Makes It Ingenious:
 * - Can recognize context-free languages (more powerful than regular languages)
 * - Stack-based memory allows nested structure recognition
 * - Foundation of LR parsers and compiler design
 * - Equivalence to context-free grammars
 * - Used in syntax analysis and language processing
 *
 * When to Use:
 * - Context-free language recognition
 * - Parser implementation
 * - Syntax analysis in compilers
 * - Nested structure validation
 * - Mathematical expression evaluation
 * - XML/HTML structure validation
 *
 * Real-World Usage:
 * - YACC/Bison parser generators
 * - Syntax analyzers in compilers
 * - Expression evaluators
 * - XML/HTML parsers
 * - Mathematical formula parsers
 * - Programming language interpreters
 * - Data format validators
 *
 * Time Complexity: O(n) for deterministic PDAs
 * Space Complexity: O(n) stack space
 * Power: Context-free languages (more than regular languages)
 */

#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <iostream>
#include <functional>
#include <memory>
#include <algorithm>

// Pushdown Automata implementation
template<typename StateType, typename InputType, typename StackType>
class PushdownAutomaton {
private:
    StateType current_state_;
    StateType initial_state_;
    std::unordered_set<StateType> accepting_states_;
    StackType stack_;

    // Transition function: (state, input, stack_top) -> (new_state, stack_operations)
    struct TransitionResult {
        StateType new_state;
        std::vector<StackType> push_symbols;  // Symbols to push (in reverse order)
        bool pop_symbol;  // Whether to pop the top symbol
    };

    std::unordered_map<StateType,
        std::unordered_map<InputType,
            std::unordered_map<StackType, TransitionResult>>> transitions_;

public:
    PushdownAutomaton(StateType initial_state = StateType{}, StackType initial_stack = StackType{})
        : current_state_(initial_state), initial_state_(initial_state) {
        stack_.push(initial_stack);
    }

    // Add transition: from_state --(input, stack_top)--> (to_state, pop?, push_symbols...)
    void add_transition(StateType from_state, InputType input, StackType stack_top,
                       StateType to_state, bool pop_symbol = false,
                       const std::vector<StackType>& push_symbols = {}) {
        TransitionResult result = {to_state, push_symbols, pop_symbol};
        transitions_[from_state][input][stack_top] = result;
    }

    // Process a single input symbol
    bool process_input(InputType input) {
        if (stack_.empty()) return false;

        StackType stack_top = stack_.top();
        auto state_it = transitions_.find(current_state_);

        if (state_it == transitions_.end()) return false;

        auto input_it = state_it->second.find(input);
        if (input_it == state_it->second.end()) {
            // Try epsilon transition (empty input)
            input_it = state_it->second.find(InputType{}); // Assuming default-constructible epsilon
            if (input_it == state_it->second.end()) return false;
        }

        auto stack_it = input_it->second.find(stack_top);
        if (stack_it == input_it->second.end()) {
            // Try epsilon stack symbol (don't care about stack top)
            stack_it = input_it->second.find(StackType{}); // Assuming default-constructible epsilon
            if (stack_it == input_it->second.end()) return false;
        }

        const TransitionResult& result = stack_it->second;

        // Apply transition
        if (result.pop_symbol && !stack_.empty()) {
            stack_.pop();
        }

        // Push symbols in reverse order (so they appear in correct order on stack)
        for (auto it = result.push_symbols.rbegin(); it != result.push_symbols.rend(); ++it) {
            stack_.push(*it);
        }

        current_state_ = result.new_state;
        return true;
    }

    // Process a sequence of inputs
    bool process_sequence(const std::vector<InputType>& inputs) {
        for (const auto& input : inputs) {
            if (!process_input(input)) {
                return false;
            }
        }
        return true;
    }

    // Check if current configuration is accepting
    bool is_accepting() const {
        return accepting_states_.count(current_state_) > 0;
    }

    // Check if automaton accepts the input sequence
    bool accepts(const std::vector<InputType>& inputs) {
        StateType original_state = current_state_;
        std::stack<StackType> original_stack = stack_;

        reset();
        bool result = process_sequence(inputs) && is_accepting();

        // Restore original state
        current_state_ = original_state;
        stack_ = original_stack;

        return result;
    }

    void add_accepting_state(StateType state) {
        accepting_states_.insert(state);
    }

    void reset() {
        current_state_ = initial_state_;
        while (!stack_.empty()) stack_.pop();
        // Push initial stack symbol (assuming default-constructible)
        stack_.push(StackType{});
    }

    StateType current_state() const { return current_state_; }
    const std::stack<StackType>& stack() const { return stack_; }
    bool stack_empty() const { return stack_.empty(); }
};

// Specialized PDA for balanced parentheses
class BalancedParenthesesPDA {
private:
    enum class State { START, PROCESSING };
    enum class Input { LPAREN, RPAREN, END };
    enum class StackSymbol { BOTTOM, LPAREN };

    PushdownAutomaton<State, Input, StackSymbol> pda_;

public:
    BalancedParenthesesPDA() : pda_(State::START, StackSymbol::BOTTOM) {
        setup_transitions();
        pda_.add_accepting_state(State::PROCESSING);
    }

    void setup_transitions() {
        // Start state: push bottom marker
        pda_.add_transition(State::START, Input::LPAREN, StackSymbol::BOTTOM,
                           State::PROCESSING, false, {StackSymbol::LPAREN});

        // Processing state transitions
        pda_.add_transition(State::PROCESSING, Input::LPAREN, StackSymbol::BOTTOM,
                           State::PROCESSING, false, {StackSymbol::LPAREN});
        pda_.add_transition(State::PROCESSING, Input::LPAREN, StackSymbol::LPAREN,
                           State::PROCESSING, false, {StackSymbol::LPAREN});

        // Match right parenthesis
        pda_.add_transition(State::PROCESSING, Input::RPAREN, StackSymbol::LPAREN,
                           State::PROCESSING, true, {});  // Pop LPAREN

        // End of input: accept if only bottom marker remains
        pda_.add_transition(State::PROCESSING, Input::END, StackSymbol::BOTTOM,
                           State::PROCESSING, false, {});
    }

    bool check_balanced(const std::string& expression) {
        std::vector<Input> inputs;

        for (char c : expression) {
            if (c == '(') inputs.push_back(Input::LPAREN);
            else if (c == ')') inputs.push_back(Input::RPAREN);
            else continue; // Skip other characters
        }
        inputs.push_back(Input::END);

        return pda_.accepts(inputs);
    }
};

// PDA for arithmetic expression parsing (simplified)
class ExpressionPDA {
private:
    enum class State { START, EXPRESSION, TERM, FACTOR };
    enum class Input { DIGIT, PLUS, MULTIPLY, LPAREN, RPAREN, END };
    enum class StackSymbol { BOTTOM, EXPR, TERM, FACTOR, LPAREN };

    PushdownAutomaton<State, Input, StackSymbol> pda_;

public:
    ExpressionPDA() : pda_(State::START, StackSymbol::BOTTOM) {
        setup_transitions();
        pda_.add_accepting_state(State::EXPRESSION);
    }

    void setup_transitions() {
        // Simplified expression grammar PDA
        // This is a very basic example - real parsers are much more complex

        // Start -> Expression
        pda_.add_transition(State::START, Input::DIGIT, StackSymbol::BOTTOM,
                           State::EXPRESSION, false, {StackSymbol::EXPR});
        pda_.add_transition(State::START, Input::LPAREN, StackSymbol::BOTTOM,
                           State::EXPRESSION, false, {StackSymbol::EXPR});

        // Expression -> Term (+ Term)*
        pda_.add_transition(State::EXPRESSION, Input::DIGIT, StackSymbol::EXPR,
                           State::TERM, false, {StackSymbol::TERM});
        pda_.add_transition(State::EXPRESSION, Input::LPAREN, StackSymbol::EXPR,
                           State::TERM, false, {StackSymbol::TERM});
        pda_.add_transition(State::EXPRESSION, Input::PLUS, StackSymbol::EXPR,
                           State::EXPRESSION, false, {StackSymbol::EXPR});

        // Term -> Factor (* Factor)*
        pda_.add_transition(State::TERM, Input::DIGIT, StackSymbol::TERM,
                           State::FACTOR, false, {StackSymbol::FACTOR});
        pda_.add_transition(State::TERM, Input::LPAREN, StackSymbol::TERM,
                           State::FACTOR, false, {StackSymbol::FACTOR});
        pda_.add_transition(State::TERM, Input::MULTIPLY, StackSymbol::TERM,
                           State::TERM, false, {StackSymbol::TERM});

        // Factor -> digit | (Expression)
        pda_.add_transition(State::FACTOR, Input::DIGIT, StackSymbol::FACTOR,
                           State::FACTOR, true, {});  // Consume digit
        pda_.add_transition(State::FACTOR, Input::LPAREN, StackSymbol::FACTOR,
                           State::EXPRESSION, false, {StackSymbol::LPAREN, StackSymbol::EXPR});
        pda_.add_transition(State::FACTOR, Input::RPAREN, StackSymbol::LPAREN,
                           State::FACTOR, true, {});  // Match parentheses

        // End of input
        pda_.add_transition(State::EXPRESSION, Input::END, StackSymbol::BOTTOM,
                           State::EXPRESSION, false, {});
    }

    bool validate_expression(const std::string& expr) {
        std::vector<Input> inputs;

        for (char c : expr) {
            if (std::isdigit(c)) inputs.push_back(Input::DIGIT);
            else if (c == '+') inputs.push_back(Input::PLUS);
            else if (c == '*') inputs.push_back(Input::MULTIPLY);
            else if (c == '(') inputs.push_back(Input::LPAREN);
            else if (c == ')') inputs.push_back(Input::RPAREN);
            else continue; // Skip whitespace
        }
        inputs.push_back(Input::END);

        return pda_.accepts(inputs);
    }
};

// PDA for palindrome recognition (using stack to reverse string)
class PalindromePDA {
private:
    enum class State { START, READING_FIRST_HALF, READING_SECOND_HALF, DONE };
    enum class Input { SYMBOL, END };
    enum class StackSymbol { BOTTOM, SYMBOL };

    PushdownAutomaton<State, Input, StackSymbol> pda_;

public:
    PalindromePDA() : pda_(State::START, StackSymbol::BOTTOM) {
        setup_transitions();
        pda_.add_accepting_state(State::DONE);
    }

    void setup_transitions() {
        // Read first half and push to stack
        pda_.add_transition(State::START, Input::SYMBOL, StackSymbol::BOTTOM,
                           State::READING_FIRST_HALF, false, {StackSymbol::SYMBOL});

        pda_.add_transition(State::READING_FIRST_HALF, Input::SYMBOL, StackSymbol::BOTTOM,
                           State::READING_FIRST_HALF, false, {StackSymbol::SYMBOL});
        pda_.add_transition(State::READING_FIRST_HALF, Input::SYMBOL, StackSymbol::SYMBOL,
                           State::READING_FIRST_HALF, false, {StackSymbol::SYMBOL});

        // Switch to second half when we see a special marker (simplified)
        // In a real implementation, we'd need a way to know the midpoint
        pda_.add_transition(State::READING_FIRST_HALF, Input::END, StackSymbol::BOTTOM,
                           State::READING_SECOND_HALF, false, {});

        // Read second half and match with stack
        pda_.add_transition(State::READING_SECOND_HALF, Input::SYMBOL, StackSymbol::SYMBOL,
                           State::READING_SECOND_HALF, true, {});  // Pop and match

        // Accept when stack is empty and input is consumed
        pda_.add_transition(State::READING_SECOND_HALF, Input::END, StackSymbol::BOTTOM,
                           State::DONE, false, {});
    }

    // Note: This is a simplified palindrome PDA
    // Real palindrome recognition requires knowing the string length
    bool check_palindrome(const std::string& str) {
        std::vector<Input> inputs;
        for (char c : str) {
            inputs.push_back(Input::SYMBOL);
        }
        inputs.push_back(Input::END);

        return pda_.accepts(inputs);
    }
};

// Generic PDA for language recognition
class LanguagePDA {
private:
    enum class State { Q0, Q1, Q2, Q3 };
    enum class Input { A, B, END };
    enum class StackSymbol { BOTTOM, A, B };

    PushdownAutomaton<State, Input, StackSymbol> pda_;

public:
    // PDA for language { a^n b^n | n >= 0 } (equal number of a's followed by b's)
    LanguagePDA() : pda_(State::Q0, StackSymbol::BOTTOM) {
        setup_an_bn_transitions();
        pda_.add_accepting_state(State::Q3);
    }

    void setup_an_bn_transitions() {
        // Push a's onto stack
        pda_.add_transition(State::Q0, Input::A, StackSymbol::BOTTOM,
                           State::Q1, false, {StackSymbol::A});
        pda_.add_transition(State::Q1, Input::A, StackSymbol::BOTTOM,
                           State::Q1, false, {StackSymbol::A});
        pda_.add_transition(State::Q1, Input::A, StackSymbol::A,
                           State::Q1, false, {StackSymbol::A});

        // Switch to popping b's
        pda_.add_transition(State::Q1, Input::B, StackSymbol::A,
                           State::Q2, true, {});  // Pop A, match B

        // Continue popping b's
        pda_.add_transition(State::Q2, Input::B, StackSymbol::A,
                           State::Q2, true, {});  // Pop A, match B

        // Accept when stack is empty
        pda_.add_transition(State::Q2, Input::END, StackSymbol::BOTTOM,
                           State::Q3, false, {});
    }

    bool recognizes_an_bn(const std::string& str) {
        std::vector<Input> inputs;
        for (char c : str) {
            if (c == 'a') inputs.push_back(Input::A);
            else if (c == 'b') inputs.push_back(Input::B);
            else return false; // Invalid character
        }
        inputs.push_back(Input::END);

        return pda_.accepts(inputs);
    }
};

// PDA-based parser for simple arithmetic expressions
class ArithmeticParser {
private:
    enum class State { START, EXPRESSION, TERM, FACTOR, END };
    enum class Input { DIGIT, PLUS, MULTIPLY, LPAREN, RPAREN, END };
    enum class StackSymbol { BOTTOM, E, T, F, PLUS, MULTIPLY, LPAREN };

    PushdownAutomaton<State, Input, StackSymbol> pda_;

public:
    ArithmeticParser() : pda_(State::START, StackSymbol::BOTTOM) {
        setup_grammar();
        pda_.add_accepting_state(State::END);
    }

    void setup_grammar() {
        // Very simplified arithmetic expression PDA
        // E -> T { + T }*
        // T -> F { * F }*
        // F -> digit | (E)

        // Start -> Expression
        pda_.add_transition(State::START, Input::DIGIT, StackSymbol::BOTTOM,
                           State::EXPRESSION, false, {StackSymbol::E});
        pda_.add_transition(State::START, Input::LPAREN, StackSymbol::BOTTOM,
                           State::EXPRESSION, false, {StackSymbol::E});

        // Expression -> Term { + Term }*
        pda_.add_transition(State::EXPRESSION, Input::DIGIT, StackSymbol::E,
                           State::TERM, false, {StackSymbol::T});
        pda_.add_transition(State::EXPRESSION, Input::LPAREN, StackSymbol::E,
                           State::TERM, false, {StackSymbol::T});
        pda_.add_transition(State::EXPRESSION, Input::PLUS, StackSymbol::E,
                           State::EXPRESSION, false, {StackSymbol::PLUS, StackSymbol::E});

        // Handle PLUS reduction
        pda_.add_transition(State::EXPRESSION, Input::END, StackSymbol::PLUS,
                           State::EXPRESSION, true, {});
        pda_.add_transition(State::EXPRESSION, Input::RPAREN, StackSymbol::PLUS,
                           State::EXPRESSION, true, {});

        // Term -> Factor { * Factor }*
        pda_.add_transition(State::TERM, Input::DIGIT, StackSymbol::T,
                           State::FACTOR, false, {StackSymbol::F});
        pda_.add_transition(State::TERM, Input::LPAREN, StackSymbol::T,
                           State::FACTOR, false, {StackSymbol::F});
        pda_.add_transition(State::TERM, Input::MULTIPLY, StackSymbol::T,
                           State::TERM, false, {StackSymbol::MULTIPLY, StackSymbol::T});

        // Handle MULTIPLY reduction
        pda_.add_transition(State::TERM, Input::PLUS, StackSymbol::MULTIPLY,
                           State::TERM, true, {});
        pda_.add_transition(State::TERM, Input::END, StackSymbol::MULTIPLY,
                           State::TERM, true, {});
        pda_.add_transition(State::TERM, Input::RPAREN, StackSymbol::MULTIPLY,
                           State::TERM, true, {});

        // Factor -> digit | (Expression)
        pda_.add_transition(State::FACTOR, Input::DIGIT, StackSymbol::F,
                           State::FACTOR, true, {});  // Consume digit
        pda_.add_transition(State::FACTOR, Input::LPAREN, StackSymbol::F,
                           State::EXPRESSION, false, {StackSymbol::LPAREN, StackSymbol::E});

        // Handle parentheses
        pda_.add_transition(State::FACTOR, Input::RPAREN, StackSymbol::LPAREN,
                           State::FACTOR, true, {});  // Match parentheses

        // End of input
        pda_.add_transition(State::EXPRESSION, Input::END, StackSymbol::BOTTOM,
                           State::END, false, {});
    }

    bool parse_expression(const std::string& expr) {
        std::vector<Input> inputs;

        for (char c : expr) {
            if (std::isdigit(c)) inputs.push_back(Input::DIGIT);
            else if (c == '+') inputs.push_back(Input::PLUS);
            else if (c == '*') inputs.push_back(Input::MULTIPLY);
            else if (c == '(') inputs.push_back(Input::LPAREN);
            else if (c == ')') inputs.push_back(Input::RPAREN);
            else if (!std::isspace(c)) return false; // Invalid character
        }
        inputs.push_back(Input::END);

        return pda_.accepts(inputs);
    }
};

// Example usage
int main() {
    std::cout << "Pushdown Automata:" << std::endl;

    // 1. Balanced parentheses
    std::cout << "\n1. Balanced Parentheses Recognition:" << std::endl;
    BalancedParenthesesPDA paren_pda;

    std::vector<std::string> test_expressions = {
        "()",
        "(())",
        "(()())",
        "(()",
        "())",
        "((())",
        "",
        "(((())))"
    };

    for (const auto& expr : test_expressions) {
        bool balanced = paren_pda.check_balanced(expr);
        std::cout << "\"" << expr << "\" is " << (balanced ? "balanced" : "unbalanced") << std::endl;
    }

    // 2. Language recognition (a^n b^n)
    std::cout << "\n2. Language { a^n b^n | n >= 0 } Recognition:" << std::endl;
    LanguagePDA lang_pda;

    std::vector<std::string> test_strings = {
        "",
        "ab",
        "aabb",
        "aaabbb",
        "aaaabbbb",
        "aab",
        "aba",
        "ba",
        "aaab"
    };

    for (const auto& str : test_strings) {
        bool accepted = lang_pda.recognizes_an_bn(str);
        std::cout << "\"" << str << "\" is " << (accepted ? "accepted" : "rejected") << std::endl;
    }

    // 3. Simple arithmetic expression parsing
    std::cout << "\n3. Arithmetic Expression Parsing:" << std::endl;
    ArithmeticParser expr_parser;

    std::vector<std::string> expressions = {
        "1+2",
        "1+2*3",
        "(1+2)*3",
        "1+(2*3)",
        "((1+2)*3)",
        "1+",
        "+1",
        "(1+2",
        "1+2)",
        "1 2"  // Invalid spacing would be ignored
    };

    for (const auto& expr : expressions) {
        bool valid = expr_parser.parse_expression(expr);
        std::cout << "\"" << expr << "\" is " << (valid ? "valid" : "invalid") << std::endl;
    }

    // 4. Demonstrate PDA power vs FSM
    std::cout << "\n4. PDA Power Demonstration:" << std::endl;
    std::cout << "Context-free languages that PDAs can recognize but FSMs cannot:" << std::endl;
    std::cout << "- Balanced parentheses: any nesting depth" << std::endl;
    std::cout << "- a^n b^n: equal number of a's followed by b's" << std::endl;
    std::cout << "- Palindromes (with center marker)" << std::endl;
    std::cout << "- Arithmetic expressions with nested parentheses" << std::endl;
    std::cout << "- XML/HTML tag matching" << std::endl;
    std::cout << "- Mathematical expression parsing" << std::endl;

    std::cout << "\nPDA Components:" << std::endl;
    std::cout << "- States: finite set (like FSM)" << std::endl;
    std::cout << "- Input alphabet: finite set" << std::endl;
    std::cout << "- Stack alphabet: finite set" << std::endl;
    std::cout << "- Stack: LIFO memory (unlimited in theory)" << std::endl;
    std::cout << "- Transition function: state × input × stack_top → state × stack_operations" << std::endl;
    std::cout << "- Start state and initial stack symbol" << std::endl;
    std::cout << "- Accepting states" << std::endl;

    std::cout << "\nDemonstrates:" << std::endl;
    std::cout << "- Context-free language recognition beyond regular languages" << std::endl;
    std::cout << "- Stack-based memory for nested structure processing" << std::endl;
    std::cout << "- Balanced parentheses and bracket matching" << std::endl;
    std::cout << "- Equal symbol counting (a^n b^n)" << std::endl;
    std::cout << "- Arithmetic expression parsing foundations" << std::endl;
    std::cout << "- Compiler design and parser implementation" << std::endl;
    std::cout << "- Formal language theory applications" << std::endl;
    std::cout << "- Production-grade parsing algorithms" << std::endl;

    return 0;
}

