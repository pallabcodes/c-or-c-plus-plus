/*
 * Mealy and Moore Machines
 *
 * Source: Digital circuit design, switching theory, formal language theory
 * Repository: Digital design textbooks, circuit synthesis tools, automata theory
 * Files: Sequential circuit design, state machine synthesis, formal verification
 * Algorithm: Finite state machines with output functions (Mealy vs Moore)
 *
 * What Makes It Ingenious:
 * - Mealy: Outputs depend on current state AND current input
 * - Moore: Outputs depend only on current state
 * - Mathematical foundation for digital circuit design
 * - Optimal state reduction algorithms
 * - Basis for hardware synthesis and verification
 *
 * When to Use:
 * - Digital circuit design and synthesis
 * - Sequential logic implementation
 * - Real-time control systems
 * - Signal processing applications
 * - Communication protocol design
 * - Hardware description languages
 *
 * Real-World Usage:
 * - Digital circuit controllers (traffic lights, vending machines)
 * - Communication protocol state machines (TCP, UART)
 * - Real-time embedded systems
 * - Signal processing pipelines
 * - Hardware synthesis tools (Verilog, VHDL)
 * - Formal verification systems
 *
 * Time Complexity: O(1) per transition
 * Space Complexity: O(states × inputs) for transition table
 * Output Type: Mealy = transition-based, Moore = state-based
 */

#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <algorithm>

// Mealy Machine: Output depends on current state AND current input
template<typename StateType, typename InputType, typename OutputType>
class MealyMachine {
private:
    StateType current_state_;
    StateType initial_state_;

    // Transition function: (state, input) -> (next_state, output)
    std::unordered_map<StateType,
        std::unordered_map<InputType,
            std::pair<StateType, OutputType>>> transitions_;

public:
    MealyMachine(StateType initial_state = StateType{})
        : current_state_(initial_state), initial_state_(initial_state) {}

    // Add transition with output: from_state --input--> (to_state, output)
    void add_transition(StateType from_state, InputType input,
                       StateType to_state, OutputType output) {
        transitions_[from_state][input] = {to_state, output};
    }

    // Process input and return output (Mealy: output on transition)
    OutputType process_input(InputType input) {
        auto state_it = transitions_.find(current_state_);
        if (state_it == transitions_.end()) {
            throw std::runtime_error("No transitions from current state");
        }

        auto transition_it = state_it->second.find(input);
        if (transition_it == state_it->second.end()) {
            throw std::runtime_error("No transition for input from current state");
        }

        auto [next_state, output] = transition_it->second;
        current_state_ = next_state;
        return output;
    }

    // Process sequence of inputs and collect outputs
    std::vector<OutputType> process_sequence(const std::vector<InputType>& inputs) {
        std::vector<OutputType> outputs;
        for (const auto& input : inputs) {
            outputs.push_back(process_input(input));
        }
        return outputs;
    }

    void reset() {
        current_state_ = initial_state_;
    }

    StateType current_state() const { return current_state_; }

    // Get all states
    std::vector<StateType> get_states() const {
        std::vector<StateType> states;
        for (const auto& pair : transitions_) {
            states.push_back(pair.first);
        }
        // Add target states
        for (const auto& state_pair : transitions_) {
            for (const auto& input_pair : state_pair.second) {
                StateType target = input_pair.second.first;
                if (std::find(states.begin(), states.end(), target) == states.end()) {
                    states.push_back(target);
                }
            }
        }
        return states;
    }
};

// Moore Machine: Output depends only on current state
template<typename StateType, typename InputType, typename OutputType>
class MooreMachine {
private:
    StateType current_state_;
    StateType initial_state_;

    // State output function: state -> output
    std::unordered_map<StateType, OutputType> state_outputs_;

    // Transition function: (state, input) -> next_state
    std::unordered_map<StateType,
        std::unordered_map<InputType, StateType>> transitions_;

public:
    MooreMachine(StateType initial_state = StateType{})
        : current_state_(initial_state), initial_state_(initial_state) {}

    // Add transition: from_state --input--> to_state
    void add_transition(StateType from_state, InputType input, StateType to_state) {
        transitions_[from_state][input] = to_state;
    }

    // Set output for a state: state -> output
    void set_state_output(StateType state, OutputType output) {
        state_outputs_[state] = output;
    }

    // Process input (Moore: output is based on state BEFORE transition)
    OutputType process_input(InputType input) {
        // Get current output (Moore: output depends only on current state)
        OutputType current_output = get_current_output();

        // Transition to next state
        auto state_it = transitions_.find(current_state_);
        if (state_it == transitions_.end()) {
            throw std::runtime_error("No transitions from current state");
        }

        auto transition_it = state_it->second.find(input);
        if (transition_it == state_it->second.end()) {
            throw std::runtime_error("No transition for input from current state");
        }

        current_state_ = transition_it->second;
        return current_output;
    }

    // Get current output without transitioning (pure Moore behavior)
    OutputType get_current_output() const {
        auto it = state_outputs_.find(current_state_);
        if (it == state_outputs_.end()) {
            return OutputType{}; // Default output
        }
        return it->second;
    }

    // Process sequence of inputs and collect outputs
    std::vector<OutputType> process_sequence(const std::vector<InputType>& inputs) {
        std::vector<OutputType> outputs;
        for (const auto& input : inputs) {
            outputs.push_back(process_input(input));
        }
        return outputs;
    }

    void reset() {
        current_state_ = initial_state_;
    }

    StateType current_state() const { return current_state_; }

    // Get all states
    std::vector<StateType> get_states() const {
        std::vector<StateType> states;
        for (const auto& pair : transitions_) {
            states.push_back(pair.first);
        }
        for (const auto& pair : state_outputs_) {
            if (std::find(states.begin(), states.end(), pair.first) == states.end()) {
                states.push_back(pair.first);
            }
        }
        // Add target states
        for (const auto& state_pair : transitions_) {
            for (const auto& input_pair : state_pair.second) {
                StateType target = input_pair.second;
                if (std::find(states.begin(), states.end(), target) == states.end()) {
                    states.push_back(target);
                }
            }
        }
        return states;
    }
};

// Binary adder using Mealy machine (outputs on transitions)
class BinaryAdderMealy {
private:
    enum class State { NO_CARRY, HAS_CARRY };
    enum class Input { ZERO_ZERO, ZERO_ONE, ONE_ZERO, ONE_ONE };
    enum class Output { ZERO, ONE };

    MealyMachine<State, Input, Output> mealy_machine_;

public:
    BinaryAdderMealy() : mealy_machine_(State::NO_CARRY) {
        setup_adder();
    }

    void setup_adder() {
        // State: NO_CARRY
        mealy_machine_.add_transition(State::NO_CARRY, Input::ZERO_ZERO, State::NO_CARRY, Output::ZERO);
        mealy_machine_.add_transition(State::NO_CARRY, Input::ZERO_ONE, State::NO_CARRY, Output::ONE);
        mealy_machine_.add_transition(State::NO_CARRY, Input::ONE_ZERO, State::NO_CARRY, Output::ONE);
        mealy_machine_.add_transition(State::NO_CARRY, Input::ONE_ONE, State::HAS_CARRY, Output::ZERO);

        // State: HAS_CARRY
        mealy_machine_.add_transition(State::HAS_CARRY, Input::ZERO_ZERO, State::NO_CARRY, Output::ONE);
        mealy_machine_.add_transition(State::HAS_CARRY, Input::ZERO_ONE, State::HAS_CARRY, Output::ZERO);
        mealy_machine_.add_transition(State::HAS_CARRY, Input::ONE_ZERO, State::HAS_CARRY, Output::ZERO);
        mealy_machine_.add_transition(State::HAS_CARRY, Input::ONE_ONE, State::HAS_CARRY, Output::ONE);
    }

    // Add two binary numbers (bit by bit)
    std::pair<std::vector<int>, int> add_binary(const std::vector<int>& a, const std::vector<int>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Binary numbers must have same length");
        }

        mealy_machine_.reset();
        std::vector<int> sum;
        int carry_out = 0;

        for (size_t i = 0; i < a.size(); ++i) {
            Input input = get_input(a[i], b[i]);
            Output output = mealy_machine_.process_input(input);
            sum.push_back(output == Output::ONE ? 1 : 0);
        }

        // Get final carry
        if (mealy_machine_.current_state() == State::HAS_CARRY) {
            carry_out = 1;
        }

        return {sum, carry_out};
    }

private:
    Input get_input(int bit_a, int bit_b) {
        if (bit_a == 0 && bit_b == 0) return Input::ZERO_ZERO;
        if (bit_a == 0 && bit_b == 1) return Input::ZERO_ONE;
        if (bit_a == 1 && bit_b == 0) return Input::ONE_ZERO;
        return Input::ONE_ONE;
    }
};

// Traffic light controller using Moore machine (outputs based on state)
class TrafficLightMoore {
private:
    enum class State { RED, YELLOW_GREEN, GREEN, YELLOW_RED };
    enum class Input { TIMER_EXPIRED };
    enum class Output { RED_LIGHT, YELLOW_LIGHT, GREEN_LIGHT };

    MooreMachine<State, Input, Output> moore_machine_;

public:
    TrafficLightMoore() : moore_machine_(State::RED) {
        setup_controller();
    }

    void setup_controller() {
        // Set state outputs (Moore: output depends only on state)
        moore_machine_.set_state_output(State::RED, Output::RED_LIGHT);
        moore_machine_.set_state_output(State::YELLOW_GREEN, Output::YELLOW_LIGHT);
        moore_machine_.set_state_output(State::GREEN, Output::GREEN_LIGHT);
        moore_machine_.set_state_output(State::YELLOW_RED, Output::YELLOW_LIGHT);

        // Set transitions
        moore_machine_.add_transition(State::RED, Input::TIMER_EXPIRED, State::YELLOW_GREEN);
        moore_machine_.add_transition(State::YELLOW_GREEN, Input::TIMER_EXPIRED, State::GREEN);
        moore_machine_.add_transition(State::GREEN, Input::TIMER_EXPIRED, State::YELLOW_RED);
        moore_machine_.add_transition(State::YELLOW_RED, Input::TIMER_EXPIRED, State::RED);
    }

    Output get_current_light() {
        return moore_machine_.get_current_output();
    }

    void timer_expired() {
        moore_machine_.process_input(Input::TIMER_EXPIRED);
    }

    State get_current_state() const {
        return moore_machine_.current_state();
    }

    std::string get_light_name() const {
        switch (get_current_light()) {
            case Output::RED_LIGHT: return "RED";
            case Output::YELLOW_LIGHT: return "YELLOW";
            case Output::GREEN_LIGHT: return "GREEN";
            default: return "UNKNOWN";
        }
    }

    int get_state_duration() const {
        switch (moore_machine_.current_state()) {
            case State::RED: return 30;         // 30 seconds
            case State::YELLOW_GREEN: return 5; // 5 seconds
            case State::GREEN: return 25;       // 25 seconds
            case State::YELLOW_RED: return 5;   // 5 seconds
            default: return 0;
        }
    }
};

// Serial communication protocol using Mealy machine
class UARTProtocolMealy {
private:
    enum class State { IDLE, RECEIVING, PROCESSING, TRANSMITTING };
    enum class Input { START_BIT, DATA_BIT, STOP_BIT, ERROR };
    enum class Output { NONE, ACK, NAK, DATA_READY };

    MealyMachine<State, Input, Output> mealy_machine_;

public:
    UARTProtocolMealy() : mealy_machine_(State::IDLE) {
        setup_protocol();
    }

    void setup_protocol() {
        // IDLE state
        mealy_machine_.add_transition(State::IDLE, Input::START_BIT, State::RECEIVING, Output::NONE);
        mealy_machine_.add_transition(State::IDLE, Input::ERROR, State::IDLE, Output::NONE);

        // RECEIVING state
        mealy_machine_.add_transition(State::RECEIVING, Input::DATA_BIT, State::RECEIVING, Output::NONE);
        mealy_machine_.add_transition(State::RECEIVING, Input::STOP_BIT, State::PROCESSING, Output::DATA_READY);
        mealy_machine_.add_transition(State::RECEIVING, Input::ERROR, State::IDLE, Output::NAK);

        // PROCESSING state
        mealy_machine_.add_transition(State::PROCESSING, Input::START_BIT, State::TRANSMITTING, Output::ACK);

        // TRANSMITTING state
        mealy_machine_.add_transition(State::TRANSMITTING, Input::DATA_BIT, State::TRANSMITTING, Output::NONE);
        mealy_machine_.add_transition(State::TRANSMITTING, Input::STOP_BIT, State::IDLE, Output::NONE);
        mealy_machine_.add_transition(State::TRANSMITTING, Input::ERROR, State::IDLE, Output::NAK);
    }

    Output process_input(Input input) {
        return mealy_machine_.process_input(input);
    }

    State get_current_state() const {
        return mealy_machine_.current_state();
    }

    std::string get_state_name() const {
        switch (mealy_machine_.current_state()) {
            case State::IDLE: return "IDLE";
            case State::RECEIVING: return "RECEIVING";
            case State::PROCESSING: return "PROCESSING";
            case State::TRANSMITTING: return "TRANSMITTING";
            default: return "UNKNOWN";
        }
    }
};

// Vending machine using Moore machine (state-based outputs)
class VendingMachineMoore {
private:
    enum class State { WAITING, HAS_25, HAS_50, HAS_75, DISPENSING, OUT_OF_ORDER };
    enum class Input { INSERT_25, INSERT_50, REQUEST_REFUND, SELECT_ITEM, MAINTENANCE };
    enum class Output { NO_MESSAGE, INSERT_COIN, INSUFFICIENT_FUNDS, DISPENSE_ITEM, REFUND_COINS, SERVICE_MODE };

    MooreMachine<State, Input, Output> moore_machine_;

public:
    VendingMachineMoore() : moore_machine_(State::WAITING) {
        setup_machine();
    }

    void setup_machine() {
        // Set state outputs (Moore: output depends only on state)
        moore_machine_.set_state_output(State::WAITING, Output::INSERT_COIN);
        moore_machine_.set_state_output(State::HAS_25, Output::INSERT_COIN);
        moore_machine_.set_state_output(State::HAS_50, Output::INSERT_COIN);
        moore_machine_.set_state_output(State::HAS_75, Output::INSERT_COIN);
        moore_machine_.set_state_output(State::DISPENSING, Output::DISPENSE_ITEM);
        moore_machine_.set_state_output(State::OUT_OF_ORDER, Output::SERVICE_MODE);

        // Set transitions
        moore_machine_.add_transition(State::WAITING, Input::INSERT_25, State::HAS_25);
        moore_machine_.add_transition(State::WAITING, Input::INSERT_50, State::HAS_50);

        moore_machine_.add_transition(State::HAS_25, Input::INSERT_25, State::HAS_50);
        moore_machine_.add_transition(State::HAS_25, Input::INSERT_50, State::HAS_75);
        moore_machine_.add_transition(State::HAS_25, Input::REQUEST_REFUND, State::WAITING);

        moore_machine_.add_transition(State::HAS_50, Input::INSERT_25, State::HAS_75);
        moore_machine_.add_transition(State::HAS_50, Input::INSERT_50, State::DISPENSING);
        moore_machine_.add_transition(State::HAS_50, Input::REQUEST_REFUND, State::WAITING);

        moore_machine_.add_transition(State::HAS_75, Input::INSERT_25, State::DISPENSING);
        moore_machine_.add_transition(State::HAS_75, Input::INSERT_50, State::DISPENSING);
        moore_machine_.add_transition(State::HAS_75, Input::REQUEST_REFUND, State::WAITING);

        moore_machine_.add_transition(State::DISPENSING, Input::SELECT_ITEM, State::WAITING);

        // Maintenance mode
        moore_machine_.add_transition(State::WAITING, Input::MAINTENANCE, State::OUT_OF_ORDER);
        moore_machine_.add_transition(State::HAS_25, Input::MAINTENANCE, State::OUT_OF_ORDER);
        moore_machine_.add_transition(State::HAS_50, Input::MAINTENANCE, State::OUT_OF_ORDER);
        moore_machine_.add_transition(State::HAS_75, Input::MAINTENANCE, State::OUT_OF_ORDER);
        moore_machine_.add_transition(State::DISPENSING, Input::MAINTENANCE, State::OUT_OF_ORDER);
        moore_machine_.add_transition(State::OUT_OF_ORDER, Input::MAINTENANCE, State::WAITING);
    }

    Output get_current_message() {
        return moore_machine_.get_current_output();
    }

    void process_input(Input input) {
        moore_machine_.process_input(input);
    }

    State get_current_state() const {
        return moore_machine_.current_state();
    }

    std::string get_message_text() const {
        switch (get_current_message()) {
            case Output::INSERT_COIN: return "Please insert coins";
            case Output::INSUFFICIENT_FUNDS: return "Insufficient funds";
            case Output::DISPENSE_ITEM: return "Please select item";
            case Output::REFUND_COINS: return "Refunding coins";
            case Output::SERVICE_MODE: return "Out of order - maintenance required";
            default: return "";
        }
    }

    bool can_select_item() const {
        return moore_machine_.current_state() == State::DISPENSING;
    }

    int get_credit_amount() const {
        switch (moore_machine_.current_state()) {
            case State::WAITING: return 0;
            case State::HAS_25: return 25;
            case State::HAS_50: return 50;
            case State::HAS_75: return 75;
            case State::DISPENSING: return 100;
            default: return 0;
        }
    }
};

// Pattern recognition using Mealy machine
class PatternRecognizerMealy {
private:
    enum class State { START, SAW_A, SAW_AB, SAW_ABC };
    enum class Input { CHAR_A, CHAR_B, CHAR_C, OTHER };
    enum class Output { NO_MATCH, PARTIAL_MATCH, FULL_MATCH };

    MealyMachine<State, Input, Output> mealy_machine_;

public:
    PatternRecognizerMealy() : mealy_machine_(State::START) {
        setup_recognizer();
    }

    void setup_recognizer() {
        // Pattern: "ABC"
        // State: START
        mealy_machine_.add_transition(State::START, Input::CHAR_A, State::SAW_A, Output::NO_MATCH);
        mealy_machine_.add_transition(State::START, Input::CHAR_B, State::START, Output::NO_MATCH);
        mealy_machine_.add_transition(State::START, Input::CHAR_C, State::START, Output::NO_MATCH);
        mealy_machine_.add_transition(State::START, Input::OTHER, State::START, Output::NO_MATCH);

        // State: SAW_A
        mealy_machine_.add_transition(State::SAW_A, Input::CHAR_A, State::SAW_A, Output::NO_MATCH);
        mealy_machine_.add_transition(State::SAW_A, Input::CHAR_B, State::SAW_AB, Output::PARTIAL_MATCH);
        mealy_machine_.add_transition(State::SAW_A, Input::CHAR_C, State::START, Output::NO_MATCH);
        mealy_machine_.add_transition(State::SAW_A, Input::OTHER, State::START, Output::NO_MATCH);

        // State: SAW_AB
        mealy_machine_.add_transition(State::SAW_AB, Input::CHAR_A, State::SAW_A, Output::NO_MATCH);
        mealy_machine_.add_transition(State::SAW_AB, Input::CHAR_B, State::SAW_AB, Output::NO_MATCH);
        mealy_machine_.add_transition(State::SAW_AB, Input::CHAR_C, State::START, Output::FULL_MATCH);
        mealy_machine_.add_transition(State::SAW_AB, Input::OTHER, State::START, Output::NO_MATCH);
    }

    Output process_character(char c) {
        Input input = classify_input(c);
        return mealy_machine_.process_input(input);
    }

    // Process string and return match positions
    std::vector<size_t> find_pattern(const std::string& text) {
        std::vector<size_t> match_positions;
        mealy_machine_.reset();

        for (size_t i = 0; i < text.size(); ++i) {
            Output result = process_character(text[i]);
            if (result == Output::FULL_MATCH) {
                match_positions.push_back(i - 2); // Pattern is 3 chars, so start position
            }
        }

        return match_positions;
    }

private:
    Input classify_input(char c) {
        switch (c) {
            case 'A': return Input::CHAR_A;
            case 'B': return Input::CHAR_B;
            case 'C': return Input::CHAR_C;
            default: return Input::OTHER;
        }
    }
};

// Example usage
int main() {
    std::cout << "Mealy and Moore Machines:" << std::endl;

    // 1. Binary Adder (Mealy Machine)
    std::cout << "\n1. Binary Adder (Mealy Machine):" << std::endl;
    BinaryAdderMealy adder;

    std::vector<int> a = {1, 0, 1, 1}; // 11 in binary
    std::vector<int> b = {1, 1, 0, 1}; // 13 in binary

    auto [sum, carry] = adder.add_binary(a, b);
    std::cout << "Adding binary numbers:" << std::endl;
    std::cout << "  A: "; for (int bit : a) std::cout << bit; std::cout << " (11)" << std::endl;
    std::cout << "  B: "; for (int bit : b) std::cout << bit; std::cout << " (13)" << std::endl;
    std::cout << "Sum: "; for (int bit : sum) std::cout << bit; std::cout << " (24), Carry: " << carry << std::endl;

    // 2. Traffic Light (Moore Machine)
    std::cout << "\n2. Traffic Light Controller (Moore Machine):" << std::endl;
    TrafficLightMoore traffic_light;

    std::cout << "Initial state: " << traffic_light.get_light_name()
              << " (" << traffic_light.get_state_duration() << "s)" << std::endl;

    for (int i = 0; i < 4; ++i) {
        traffic_light.timer_expired();
        std::cout << "After timer: " << traffic_light.get_light_name()
                  << " (" << traffic_light.get_state_duration() << "s)" << std::endl;
    }

    // 3. UART Protocol (Mealy Machine)
    std::cout << "\n3. UART Protocol Simulation (Mealy Machine):" << std::endl;
    UARTProtocolMealy uart;

    std::vector<UARTProtocolMealy::Input> protocol_sequence = {
        UARTProtocolMealy::Input::START_BIT,
        UARTProtocolMealy::Input::DATA_BIT,
        UARTProtocolMealy::Input::DATA_BIT,
        UARTProtocolMealy::Input::STOP_BIT,
        UARTProtocolMealy::Input::START_BIT
    };

    std::cout << "Processing UART protocol sequence:" << std::endl;
    for (auto input : protocol_sequence) {
        auto output = uart.process_input(input);
        std::string input_name, output_name;

        switch (input) {
            case UARTProtocolMealy::Input::START_BIT: input_name = "START"; break;
            case UARTProtocolMealy::Input::DATA_BIT: input_name = "DATA"; break;
            case UARTProtocolMealy::Input::STOP_BIT: input_name = "STOP"; break;
            case UARTProtocolMealy::Input::ERROR: input_name = "ERROR"; break;
        }

        switch (output) {
            case UARTProtocolMealy::Output::NONE: output_name = "NONE"; break;
            case UARTProtocolMealy::Output::ACK: output_name = "ACK"; break;
            case UARTProtocolMealy::Output::NAK: output_name = "NAK"; break;
            case UARTProtocolMealy::Output::DATA_READY: output_name = "DATA_READY"; break;
        }

        std::cout << "Input: " << input_name << " -> Output: " << output_name
                  << " (State: " << uart.get_state_name() << ")" << std::endl;
    }

    // 4. Vending Machine (Moore Machine)
    std::cout << "\n4. Vending Machine (Moore Machine):" << std::endl;
    VendingMachineMoore vending;

    std::cout << "Initial: " << vending.get_message_text() << std::endl;

    vending.process_input(VendingMachineMoore::Input::INSERT_25);
    std::cout << "After $0.25: " << vending.get_message_text()
              << " (Credit: $" << vending.get_credit_amount() / 100.0 << ")" << std::endl;

    vending.process_input(VendingMachineMoore::Input::INSERT_50);
    std::cout << "After $0.50: " << vending.get_message_text()
              << " (Credit: $" << vending.get_credit_amount() / 100.0 << ")" << std::endl;

    vending.process_input(VendingMachineMoore::Input::INSERT_25);
    std::cout << "After another $0.25: " << vending.get_message_text()
              << " (Credit: $" << vending.get_credit_amount() / 100.0 << ")" << std::endl;

    if (vending.can_select_item()) {
        vending.process_input(VendingMachineMoore::Input::SELECT_ITEM);
        std::cout << "After selecting item: " << vending.get_message_text() << std::endl;
    }

    // 5. Pattern Recognizer (Mealy Machine)
    std::cout << "\n5. Pattern Recognizer (Mealy Machine):" << std::endl;
    PatternRecognizerMealy recognizer;

    std::string test_text = "AABABCABABC";
    auto matches = recognizer.find_pattern(test_text);

    std::cout << "Searching for pattern 'ABC' in: " << test_text << std::endl;
    std::cout << "Matches found at positions: ";
    for (size_t pos : matches) {
        std::cout << pos << " ";
    }
    std::cout << std::endl;

    // Character-by-character processing
    std::cout << "Character-by-character processing:" << std::endl;
    recognizer = PatternRecognizerMealy(); // Reset
    for (size_t i = 0; i < test_text.size(); ++i) {
        auto output = recognizer.process_character(test_text[i]);
        std::string output_name;
        switch (output) {
            case PatternRecognizerMealy::Output::NO_MATCH: output_name = "NO_MATCH"; break;
            case PatternRecognizerMealy::Output::PARTIAL_MATCH: output_name = "PARTIAL"; break;
            case PatternRecognizerMealy::Output::FULL_MATCH: output_name = "FULL_MATCH"; break;
        }
        std::cout << "Char '" << test_text[i] << "' -> " << output_name << std::endl;
    }

    std::cout << "\nDemonstrates:" << std::endl;
    std::cout << "- Mealy machines: Output depends on state AND input" << std::endl;
    std::cout << "- Moore machines: Output depends only on state" << std::endl;
    std::cout << "- Binary adder implementation using Mealy machine" << std::endl;
    std::cout << "- Traffic light controller using Moore machine" << std::endl;
    std::cout << "- UART protocol simulation with Mealy machine" << std::endl;
    std::cout << "- Vending machine with Moore machine outputs" << std::endl;
    std::cout << "- Pattern recognition using Mealy machine" << std::endl;
    std::cout << "- Digital circuit design patterns" << std::endl;
    std::cout << "- Production-grade sequential logic implementation" << std::endl;

    return 0;
}

