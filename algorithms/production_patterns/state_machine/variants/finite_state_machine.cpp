/*
 * Finite State Machine (FSM)
 *
 * Source: Compiler design, protocol implementations, control systems
 * Repository: Lexical analyzers, network protocols, embedded systems
 * Files: State transition tables, event-driven systems, control logic
 * Algorithm: Deterministic finite automaton with state transition function
 *
 * What Makes It Ingenious:
 * - Complete mathematical foundation (automata theory)
 * - Efficient table-driven implementation
 * - Deterministic behavior guarantees
 * - Minimal state representation
 * - Widely used in production systems
 *
 * When to Use:
 * - Lexical analysis and tokenization
 * - Protocol state management
 * - Control system logic
 * - Pattern recognition
 * - Event-driven programming
 * - Input validation and parsing
 *
 * Real-World Usage:
 * - Compiler lexical analyzers
 * - TCP/IP protocol stacks
 * - Traffic light controllers
 * - Elevator control systems
 * - Vending machine logic
 * - Regular expression engines
 * - Network packet processing
 *
 * Time Complexity: O(1) per transition (table lookup)
 * Space Complexity: O(states × alphabet) for transition table
 * Deterministic: Yes - exactly one transition per state/input
 */

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <string>
#include <iostream>
#include <memory>
#include <queue>
#include <set>
#include <algorithm>

// Generic Finite State Machine implementation
template<typename StateType, typename InputType>
class FiniteStateMachine {
private:
    StateType current_state_;
    StateType initial_state_;
    std::unordered_set<StateType> accepting_states_;
    std::unordered_map<StateType, std::unordered_map<InputType, StateType>> transitions_;

public:
    FiniteStateMachine(StateType initial_state = StateType{})
        : current_state_(initial_state), initial_state_(initial_state) {}

    // Add a transition: from_state --input--> to_state
    void add_transition(StateType from_state, InputType input, StateType to_state) {
        transitions_[from_state][input] = to_state;
    }

    // Add an accepting state
    void add_accepting_state(StateType state) {
        accepting_states_.insert(state);
    }

    // Process a single input
    bool process_input(InputType input) {
        auto state_it = transitions_.find(current_state_);
        if (state_it == transitions_.end()) {
            return false; // No transitions from current state
        }

        auto transition_it = state_it->second.find(input);
        if (transition_it == state_it->second.end()) {
            return false; // No transition for this input
        }

        current_state_ = transition_it->second;
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

    // Check if current state is accepting
    bool is_accepting() const {
        return accepting_states_.count(current_state_) > 0;
    }

    // Reset to initial state
    void reset() {
        current_state_ = initial_state_;
    }

    // Get current state
    StateType current_state() const { return current_state_; }

    // Get all states
    std::vector<StateType> get_states() const {
        std::vector<StateType> states;
        for (const auto& pair : transitions_) {
            states.push_back(pair.first);
        }
        // Add states that are only targets (might not have outgoing transitions)
        for (const auto& pair : transitions_) {
            for (const auto& trans_pair : pair.second) {
                StateType target = trans_pair.second;
                if (std::find(states.begin(), states.end(), target) == states.end()) {
                    states.push_back(target);
                }
            }
        }
        return states;
    }

    // Check if FSM accepts a string (resets state)
    bool accepts(const std::vector<InputType>& inputs) {
        StateType original_state = current_state_;
        reset();
        bool result = process_sequence(inputs) && is_accepting();
        current_state_ = original_state; // Restore original state
        return result;
    }
};

// Specialized FSM for string processing (common use case)
class StringFSM : public FiniteStateMachine<int, char> {
public:
    StringFSM(int initial_state = 0) : FiniteStateMachine(initial_state) {}

    // Build FSM from regular expression (simplified - just concatenations)
    void build_from_string(const std::string& pattern) {
        int current_state = 0;
        for (char c : pattern) {
            add_transition(current_state, c, current_state + 1);
            current_state++;
        }
        add_accepting_state(current_state);
    }

    // Build FSM for keyword recognition
    void build_keyword_recognizer(const std::string& keyword) {
        int state = 0;
        for (char c : keyword) {
            add_transition(state, c, state + 1);
            state++;
        }
        add_accepting_state(state);
    }
};

// Table-driven FSM (more efficient for large alphabets)
class TableDrivenFSM {
private:
    std::vector<std::vector<int>> transition_table_; // [state][input] -> next_state
    std::vector<bool> accepting_states_;
    int current_state_;
    int num_states_;
    int alphabet_size_;

public:
    TableDrivenFSM(int num_states, int alphabet_size, int initial_state = 0)
        : transition_table_(num_states, std::vector<int>(alphabet_size, -1)),
          accepting_states_(num_states, false),
          current_state_(initial_state),
          num_states_(num_states),
          alphabet_size_(alphabet_size) {}

    // Add transition: from_state --input_index--> to_state
    void add_transition(int from_state, int input_index, int to_state) {
        if (from_state >= 0 && from_state < num_states_ &&
            input_index >= 0 && input_index < alphabet_size_) {
            transition_table_[from_state][input_index] = to_state;
        }
    }

    // Set accepting state
    void set_accepting(int state, bool accepting = true) {
        if (state >= 0 && state < num_states_) {
            accepting_states_[state] = accepting;
        }
    }

    // Process input (returns true if transition exists)
    bool process_input(int input_index) {
        if (current_state_ < 0 || current_state_ >= num_states_ ||
            input_index < 0 || input_index >= alphabet_size_) {
            return false;
        }

        int next_state = transition_table_[current_state_][input_index];
        if (next_state == -1) {
            return false; // No transition
        }

        current_state_ = next_state;
        return true;
    }

    // Process string of input indices
    bool process_sequence(const std::vector<int>& inputs) {
        for (int input : inputs) {
            if (!process_input(input)) {
                return false;
            }
        }
        return true;
    }

    bool is_accepting() const {
        return current_state_ >= 0 && current_state_ < num_states_ &&
               accepting_states_[current_state_];
    }

    void reset(int initial_state = 0) {
        current_state_ = initial_state;
    }

    int current_state() const { return current_state_; }
};

// Lexical analyzer using FSM (real-world example)
class LexicalAnalyzer {
private:
    enum class TokenType {
        IDENTIFIER,
        NUMBER,
        OPERATOR,
        KEYWORD,
        WHITESPACE,
        UNKNOWN
    };

    enum class State {
        START,
        IN_IDENTIFIER,
        IN_NUMBER,
        IN_OPERATOR,
        ACCEPT,
        ERROR
    };

    struct Token {
        TokenType type;
        std::string value;
        size_t position;

        Token(TokenType t, const std::string& v, size_t pos)
            : type(t), value(v), position(pos) {}
    };

    FiniteStateMachine<State, char> fsm_;
    std::string input_;
    size_t position_;

public:
    LexicalAnalyzer() : fsm_(State::START), position_(0) {
        setup_lexer();
    }

    void set_input(const std::string& input) {
        input_ = input;
        position_ = 0;
        fsm_.reset();
    }

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        size_t start_pos = 0;

        while (position_ < input_.size()) {
            start_pos = position_;
            std::string current_token;

            // Process characters until we reach an accepting state or error
            while (position_ < input_.size()) {
                char c = input_[position_];
                current_token += c;

                if (!fsm_.process_input(c)) {
                    // Invalid transition - back up and try to accept
                    if (fsm_.is_accepting() && !current_token.empty()) {
                        current_token.pop_back(); // Remove the invalid character
                        position_--; // Back up position
                    }
                    break;
                }

                position_++;

                // Check for accepting states that indicate token boundaries
                if (is_token_boundary(fsm_.current_state())) {
                    break;
                }
            }

            if (!current_token.empty()) {
                TokenType type = classify_token(current_token);
                tokens.emplace_back(type, current_token, start_pos);
            } else {
                // Skip invalid character
                position_++;
            }

            fsm_.reset();
        }

        return tokens;
    }

private:
    void setup_lexer() {
        // Simple lexical analyzer states and transitions
        // This is a simplified example - real lexers are much more complex

        // From START state
        fsm_.add_transition(State::START, 'a', State::IN_IDENTIFIER);
        fsm_.add_transition(State::START, 'b', State::IN_IDENTIFIER);
        fsm_.add_transition(State::START, 'c', State::IN_IDENTIFIER);
        // ... (would add all letters)

        fsm_.add_transition(State::START, '0', State::IN_NUMBER);
        fsm_.add_transition(State::START, '1', State::IN_NUMBER);
        // ... (would add all digits)

        fsm_.add_transition(State::START, '+', State::IN_OPERATOR);
        fsm_.add_transition(State::START, '-', State::IN_OPERATOR);
        fsm_.add_transition(State::START, '*', State::IN_OPERATOR);
        fsm_.add_transition(State::START, '/', State::IN_OPERATOR);

        fsm_.add_transition(State::START, ' ', State::ACCEPT); // Whitespace

        // From IN_IDENTIFIER state
        for (char c = 'a'; c <= 'z'; ++c) {
            fsm_.add_transition(State::IN_IDENTIFIER, c, State::IN_IDENTIFIER);
        }
        for (char c = '0'; c <= '9'; ++c) {
            fsm_.add_transition(State::IN_IDENTIFIER, c, State::IN_IDENTIFIER);
        }
        fsm_.add_transition(State::IN_IDENTIFIER, '_', State::IN_IDENTIFIER);

        // From IN_NUMBER state
        for (char c = '0'; c <= '9'; ++c) {
            fsm_.add_transition(State::IN_NUMBER, c, State::IN_NUMBER);
        }
        fsm_.add_transition(State::IN_NUMBER, '.', State::IN_NUMBER); // Decimals

        // Accepting states
        fsm_.add_accepting_state(State::IN_IDENTIFIER);
        fsm_.add_accepting_state(State::IN_NUMBER);
        fsm_.add_accepting_state(State::IN_OPERATOR);
        fsm_.add_accepting_state(State::ACCEPT);
    }

    bool is_token_boundary(State state) {
        return state == State::ACCEPT ||
               state == State::IN_IDENTIFIER ||
               state == State::IN_NUMBER ||
               state == State::IN_OPERATOR;
    }

    TokenType classify_token(const std::string& token) {
        if (token.empty()) return TokenType::UNKNOWN;

        // Check for keywords
        if (token == "if" || token == "while" || token == "for") {
            return TokenType::KEYWORD;
        }

        // Check for numbers
        if (std::isdigit(token[0]) ||
            (token.size() > 1 && token[0] == '.' && std::isdigit(token[1]))) {
            return TokenType::NUMBER;
        }

        // Check for operators
        if (token.size() == 1 &&
            std::string("+-*/=<>!&|^").find(token[0]) != std::string::npos) {
            return TokenType::OPERATOR;
        }

        // Check for whitespace
        if (std::all_of(token.begin(), token.end(), ::isspace)) {
            return TokenType::WHITESPACE;
        }

        // Default to identifier
        return TokenType::IDENTIFIER;
    }
};

// Traffic light controller (classic FSM example)
class TrafficLightController {
private:
    enum class State { RED, YELLOW_TO_GREEN, GREEN, YELLOW_TO_RED };
    enum class Event { TIMER_EXPIRED, PEDESTRIAN_BUTTON };

    FiniteStateMachine<State, Event> fsm_;
    int red_duration_ = 30;    // seconds
    int green_duration_ = 25;  // seconds
    int yellow_duration_ = 5;  // seconds

public:
    TrafficLightController() : fsm_(State::RED) {
        setup_transitions();
    }

    void setup_transitions() {
        // RED -> GREEN (via yellow)
        fsm_.add_transition(State::RED, Event::TIMER_EXPIRED, State::YELLOW_TO_GREEN);

        // YELLOW_TO_GREEN -> GREEN
        fsm_.add_transition(State::YELLOW_TO_GREEN, Event::TIMER_EXPIRED, State::GREEN);

        // GREEN -> YELLOW_TO_RED
        fsm_.add_transition(State::GREEN, Event::TIMER_EXPIRED, State::YELLOW_TO_RED);

        // YELLOW_TO_RED -> RED
        fsm_.add_transition(State::YELLOW_TO_RED, Event::TIMER_EXPIRED, State::RED);

        // Emergency pedestrian crossing (from any state to RED)
        fsm_.add_transition(State::YELLOW_TO_GREEN, Event::PEDESTRIAN_BUTTON, State::RED);
        fsm_.add_transition(State::GREEN, Event::PEDESTRIAN_BUTTON, State::RED);
        fsm_.add_transition(State::YELLOW_TO_RED, Event::PEDESTRIAN_BUTTON, State::RED);
        // RED already goes to RED, so no change needed
    }

    void process_event(Event event) {
        fsm_.process_input(event);
    }

    State get_current_state() const {
        return fsm_.current_state();
    }

    std::string get_state_name() const {
        switch (fsm_.current_state()) {
            case State::RED: return "RED";
            case State::YELLOW_TO_GREEN: return "YELLOW (to green)";
            case State::GREEN: return "GREEN";
            case State::YELLOW_TO_RED: return "YELLOW (to red)";
            default: return "UNKNOWN";
        }
    }

    int get_state_duration() const {
        switch (fsm_.current_state()) {
            case State::RED: return red_duration_;
            case State::YELLOW_TO_GREEN: return yellow_duration_;
            case State::GREEN: return green_duration_;
            case State::YELLOW_TO_RED: return yellow_duration_;
            default: return 0;
        }
    }
};

// Vending machine FSM
class VendingMachine {
private:
    enum class State { WAITING, HAS_25, HAS_50, HAS_75, DISPENSING };
    enum class Event { INSERT_25, INSERT_50, REQUEST_REFUND, SELECT_ITEM };

    FiniteStateMachine<State, Event> fsm_;
    int balance_ = 0;

public:
    VendingMachine() : fsm_(State::WAITING) {
        setup_machine();
    }

    void setup_machine() {
        // Coin insertion transitions
        fsm_.add_transition(State::WAITING, Event::INSERT_25, State::HAS_25);
        fsm_.add_transition(State::HAS_25, Event::INSERT_25, State::HAS_50);
        fsm_.add_transition(State::HAS_50, Event::INSERT_25, State::HAS_75);
        fsm_.add_transition(State::HAS_75, Event::INSERT_25, State::DISPENSING);

        fsm_.add_transition(State::WAITING, Event::INSERT_50, State::HAS_50);
        fsm_.add_transition(State::HAS_25, Event::INSERT_50, State::HAS_75);
        fsm_.add_transition(State::HAS_50, Event::INSERT_50, State::DISPENSING);

        // Item selection (requires $1.00)
        fsm_.add_transition(State::DISPENSING, Event::SELECT_ITEM, State::WAITING);

        // Refund from any state
        fsm_.add_transition(State::HAS_25, Event::REQUEST_REFUND, State::WAITING);
        fsm_.add_transition(State::HAS_50, Event::REQUEST_REFUND, State::WAITING);
        fsm_.add_transition(State::HAS_75, Event::REQUEST_REFUND, State::WAITING);
        fsm_.add_transition(State::DISPENSING, Event::REQUEST_REFUND, State::WAITING);

        // Accepting state
        fsm_.add_accepting_state(State::DISPENSING);
    }

    bool insert_coin(int amount) {
        balance_ += amount;
        Event event = (amount == 25) ? Event::INSERT_25 : Event::INSERT_50;
        return fsm_.process_input(event);
    }

    bool select_item() {
        if (fsm_.current_state() == State::DISPENSING) {
            balance_ -= 100; // Item costs $1.00
            bool success = fsm_.process_input(Event::SELECT_ITEM);
            if (success) {
                balance_ = 0; // Reset after successful purchase
            }
            return success;
        }
        return false;
    }

    bool request_refund() {
        int refund_amount = balance_;
        balance_ = 0;
        bool success = fsm_.process_input(Event::REQUEST_REFUND);
        if (!success) {
            balance_ = refund_amount; // Restore balance if refund failed
        }
        return success;
    }

    State get_current_state() const {
        return fsm_.current_state();
    }

    int get_balance() const { return balance_; }

    std::string get_state_description() const {
        switch (fsm_.current_state()) {
            case State::WAITING: return "Waiting for coins";
            case State::HAS_25: return "Has $0.25";
            case State::HAS_50: return "Has $0.50";
            case State::HAS_75: return "Has $0.75";
            case State::DISPENSING: return "Ready to dispense";
            default: return "Unknown state";
        }
    }
};

// Example usage
int main() {
    std::cout << "Finite State Machine Examples:" << std::endl;

    // 1. Basic FSM for string recognition
    std::cout << "\n1. String Pattern Recognition:" << std::endl;
    StringFSM pattern_matcher;
    pattern_matcher.build_keyword_recognizer("hello");

    std::string test_str = "hello";
    std::vector<char> input(test_str.begin(), test_str.end());

    std::cout << "Testing pattern 'hello' on input '" << test_str << "': ";
    if (pattern_matcher.accepts(input)) {
        std::cout << "ACCEPTED" << std::endl;
    } else {
        std::cout << "REJECTED" << std::endl;
    }

    // Test partial matches
    std::string partial = "hell";
    std::vector<char> partial_input(partial.begin(), partial.end());
    std::cout << "Testing partial 'hell': ";
    pattern_matcher.reset();
    if (pattern_matcher.process_sequence(partial_input)) {
        std::cout << "Processed (at state: " << static_cast<int>(pattern_matcher.current_state())
                  << ", accepting: " << (pattern_matcher.is_accepting() ? "yes" : "no") << ")" << std::endl;
    }

    // 2. Lexical Analyzer
    std::cout << "\n2. Lexical Analyzer:" << std::endl;
    LexicalAnalyzer lexer;
    lexer.set_input("int x = 42 + y;");

    auto tokens = lexer.tokenize();
    std::cout << "Tokens found:" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "  " << token.value << " (position: " << token.position << ")" << std::endl;
    }

    // 3. Traffic Light Controller
    std::cout << "\n3. Traffic Light Controller:" << std::endl;
    TrafficLightController traffic_light;

    std::cout << "Initial state: " << traffic_light.get_state_name() << std::endl;

    // Simulate timer events
    traffic_light.process_event(TrafficLightController::Event::TIMER_EXPIRED);
    std::cout << "After timer: " << traffic_light.get_state_name()
              << " (duration: " << traffic_light.get_state_duration() << "s)" << std::endl;

    traffic_light.process_event(TrafficLightController::Event::TIMER_EXPIRED);
    std::cout << "After timer: " << traffic_light.get_state_name()
              << " (duration: " << traffic_light.get_state_duration() << "s)" << std::endl;

    // Emergency pedestrian crossing
    traffic_light.process_event(TrafficLightController::Event::PEDESTRIAN_BUTTON);
    std::cout << "After pedestrian button: " << traffic_light.get_state_name() << std::endl;

    // 4. Vending Machine
    std::cout << "\n4. Vending Machine:" << std::endl;
    VendingMachine vending;

    std::cout << "Initial: " << vending.get_state_description() << std::endl;

    vending.insert_coin(25);
    std::cout << "After $0.25: " << vending.get_state_description() << std::endl;

    vending.insert_coin(25);
    std::cout << "After another $0.25: " << vending.get_state_description() << std::endl;

    vending.insert_coin(50);
    std::cout << "After $0.50: " << vending.get_state_description() << std::endl;

    if (vending.select_item()) {
        std::cout << "Item dispensed! " << vending.get_state_description() << std::endl;
    }

    // Try refund
    vending.insert_coin(25);
    vending.insert_coin(25);
    std::cout << "After coins: " << vending.get_state_description() << std::endl;

    vending.request_refund();
    std::cout << "After refund: " << vending.get_state_description() << std::endl;

    std::cout << "\nDemonstrates:" << std::endl;
    std::cout << "- Generic finite state machine implementation" << std::endl;
    std::cout << "- Table-driven FSM for efficiency" << std::endl;
    std::cout << "- String pattern recognition" << std::endl;
    std::cout << "- Lexical analysis for compilers" << std::endl;
    std::cout << "- Traffic light control system" << std::endl;
    std::cout << "- Vending machine state logic" << std::endl;
    std::cout << "- Deterministic state transitions" << std::endl;
    std::cout << "- Production-grade state machine patterns" << std::endl;

    return 0;
}

