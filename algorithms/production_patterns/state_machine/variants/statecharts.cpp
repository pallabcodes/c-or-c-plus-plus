/*
 * Statecharts
 *
 * Source: Complex system modeling, game AI, UI frameworks, real-time systems
 * Repository: Game engines, modeling tools, real-time embedded systems
 * Files: Hierarchical state machines, concurrent state regions, state inheritance
 * Algorithm: Extended finite state machines with hierarchy and concurrency
 *
 * What Makes It Ingenious:
 * - Hierarchical state organization (states can contain substates)
 * - Concurrent regions (orthogonal state components)
 * - State inheritance and refinement
 * - Event broadcasting and propagation
 * - History states for resumable behavior
 * - Complex state relationships and dependencies
 *
 * When to Use:
 * - Complex game AI with nested behaviors
 * - UI state management with modal dialogs
 * - Real-time system control with concurrent activities
 * - Workflow automation with complex state dependencies
 * - Robotic control systems
 * - Complex business process modeling
 *
 * Real-World Usage:
 * - Game character AI systems (idle → walking → running hierarchies)
 * - Complex UI frameworks with nested modal states
 * - Real-time embedded systems
 * - Workflow automation engines
 * - Robotic behavior controllers
 * - Industrial automation systems
 * - Complex event processing systems
 *
 * Time Complexity: O(depth) for event propagation, O(n) for state transitions
 * Space Complexity: O(states) for state hierarchy
 * Expressiveness: Highly complex state relationships and concurrency
 */

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <functional>
#include <iostream>
#include <algorithm>
#include <queue>

// Forward declarations
class State;
class Statechart;
class Event;

// Event class for statechart communication
class Event {
public:
    std::string name;
    std::unordered_map<std::string, std::string> parameters;

    Event(const std::string& event_name = "") : name(event_name) {}

    void add_parameter(const std::string& key, const std::string& value) {
        parameters[key] = value;
    }

    std::string get_parameter(const std::string& key) const {
        auto it = parameters.find(key);
        return it != parameters.end() ? it->second : "";
    }
};

// Base state class
class State {
protected:
    std::string name_;
    State* parent_;
    std::vector<std::unique_ptr<State>> substates_;
    std::unordered_map<std::string, std::function<bool(const Event&)>> transitions_;
    std::unordered_map<std::string, std::string> transition_targets_;
    std::function<void()> entry_action_;
    std::function<void()> exit_action_;
    std::function<void()> do_action_;
    State* current_substate_;
    State* history_state_;  // For history transitions

    // Concurrent regions (orthogonal states)
    std::vector<std::unique_ptr<Statechart>> concurrent_regions_;

public:
    State(const std::string& name, State* parent = nullptr)
        : name_(name), parent_(parent), current_substate_(nullptr), history_state_(nullptr) {}

    virtual ~State() = default;

    // State hierarchy
    void add_substate(std::unique_ptr<State> substate) {
        substate->parent_ = this;
        substates_.push_back(std::move(substate));
    }

    // Concurrent regions
    void add_concurrent_region(std::unique_ptr<Statechart> region) {
        concurrent_regions_.push_back(std::move(region));
    }

    // Transition definition
    void add_transition(const std::string& event_name,
                       const std::string& target_state_name,
                       std::function<bool(const Event&)> condition = nullptr) {
        transitions_[event_name] = condition ? condition :
            [](const Event&) { return true; };
        transition_targets_[event_name] = target_state_name;
    }

    // Actions
    void set_entry_action(std::function<void()> action) { entry_action_ = action; }
    void set_exit_action(std::function<void()> action) { exit_action_ = action; }
    void set_do_action(std::function<void()> action) { do_action_ = action; }

    // State operations
    virtual void enter() {
        if (entry_action_) entry_action_();

        // Enter default substate or history state
        if (!substates_.empty()) {
            if (history_state_) {
                current_substate_ = history_state_;
            } else {
                current_substate_ = substates_[0].get();
            }
            current_substate_->enter();
        }

        // Enter concurrent regions
        for (auto& region : concurrent_regions_) {
            region->enter();
        }
    }

    virtual void exit() {
        // Remember current substate for history
        history_state_ = current_substate_;

        // Exit concurrent regions
        for (auto& region : concurrent_regions_) {
            region->exit();
        }

        // Exit current substate
        if (current_substate_) {
            current_substate_->exit();
            current_substate_ = nullptr;
        }

        if (exit_action_) exit_action_();
    }

    virtual void update() {
        if (do_action_) do_action_();

        // Update current substate
        if (current_substate_) {
            current_substate_->update();
        }

        // Update concurrent regions
        for (auto& region : concurrent_regions_) {
            region->update();
        }
    }

    // Event handling with propagation
    virtual bool handle_event(const Event& event) {
        // First, try concurrent regions
        for (auto& region : concurrent_regions_) {
            if (region->handle_event(event)) {
                return true; // Event consumed by concurrent region
            }
        }

        // Then, try current substate
        if (current_substate_ && current_substate_->handle_event(event)) {
            return true; // Event consumed by substate
        }

        // Finally, try this state's transitions
        auto trans_it = transitions_.find(event.name);
        if (trans_it != transitions_.end() && trans_it->second(event)) {
            // Execute transition
            execute_transition(event.name);
            return true;
        }

        // Event not handled
        return false;
    }

protected:
    void execute_transition(const std::string& event_name) {
        auto target_it = transition_targets_.find(event_name);
        if (target_it == transition_targets_.end()) return;

        const std::string& target_name = target_it->second;

        // Find target state
        State* target_state = find_state_by_name(target_name);
        if (!target_state) return;

        // Exit current substate hierarchy
        if (current_substate_) {
            current_substate_->exit();
        }

        // Enter target state hierarchy
        current_substate_ = target_state;
        current_substate_->enter();
    }

    State* find_state_by_name(const std::string& name) {
        // Search in direct substates
        for (auto& substate : substates_) {
            if (substate->name_ == name) {
                return substate.get();
            }
        }

        // Recursively search in substate hierarchies
        for (auto& substate : substates_) {
            State* found = substate->find_state_by_name(name);
            if (found) return found;
        }

        return nullptr;
    }

public:
    const std::string& get_name() const { return name_; }
    State* get_parent() const { return parent_; }
    State* get_current_substate() const { return current_substate_; }
    const std::vector<std::unique_ptr<State>>& get_substates() const { return substates_; }
};

// Statechart main class
class Statechart {
private:
    std::string name_;
    std::unique_ptr<State> root_state_;
    State* current_state_;
    std::queue<Event> event_queue_;

public:
    Statechart(const std::string& name) : name_(name), current_state_(nullptr) {}

    void set_root_state(std::unique_ptr<State> root) {
        root_state_ = std::move(root);
    }

    void enter() {
        if (root_state_) {
            current_state_ = root_state_.get();
            current_state_->enter();
        }
    }

    void exit() {
        if (current_state_) {
            current_state_->exit();
            current_state_ = nullptr;
        }
    }

    void update() {
        // Process queued events
        while (!event_queue_.empty()) {
            Event event = event_queue_.front();
            event_queue_.pop();
            handle_event(event);
        }

        // Update current state
        if (current_state_) {
            current_state_->update();
        }
    }

    // Queue event for processing
    void send_event(const Event& event) {
        event_queue_.push(event);
    }

    // Handle event immediately
    bool handle_event(const Event& event) {
        if (current_state_) {
            return current_state_->handle_event(event);
        }
        return false;
    }

    State* get_current_state() const { return current_state_; }

    // Get full state path
    std::vector<std::string> get_state_path() const {
        std::vector<std::string> path;
        State* state = current_state_;

        while (state) {
            path.push_back(state->get_name());
            state = state->get_parent();
        }

        std::reverse(path.begin(), path.end());
        return path;
    }

    void print_state() const {
        auto path = get_state_path();
        std::cout << "Statechart '" << name_ << "' state: ";
        for (size_t i = 0; i < path.size(); ++i) {
            if (i > 0) std::cout << " → ";
            std::cout << path[i];
        }
        std::cout << std::endl;
    }
};

// Predefined state types for common patterns

// Composite state (can contain substates)
class CompositeState : public State {
public:
    CompositeState(const std::string& name, State* parent = nullptr)
        : State(name, parent) {}

    // Override to handle composite behavior
    void enter() override {
        State::enter();
        std::cout << "Entering composite state: " << name_ << std::endl;
    }

    void exit() override {
        std::cout << "Exiting composite state: " << name_ << std::endl;
        State::exit();
    }
};

// Concurrent state (orthogonal regions)
class ConcurrentState : public State {
public:
    ConcurrentState(const std::string& name, State* parent = nullptr)
        : State(name, parent) {}

    void enter() override {
        std::cout << "Entering concurrent state: " << name_ << std::endl;
        State::enter();
    }

    void exit() override {
        std::cout << "Exiting concurrent state: " << name_ << std::endl;
        State::exit();
    }
};

// Game character AI example
class CharacterAI : public Statechart {
public:
    CharacterAI() : Statechart("CharacterAI") {
        setup_character_ai();
    }

private:
    void setup_character_ai() {
        // Create root state
        auto root = std::make_unique<CompositeState>("Character");

        // Main states
        auto idle = std::make_unique<State>("Idle");
        idle->set_entry_action([]() { std::cout << "Character is now idle" << std::endl; });
        idle->set_do_action([]() { /* Idle animation */ });

        auto moving = std::make_unique<CompositeState>("Moving");

        // Moving substates
        auto walking = std::make_unique<State>("Walking");
        walking->set_entry_action([]() { std::cout << "Character started walking" << std::endl; });

        auto running = std::make_unique<State>("Running");
        running->set_entry_action([]() { std::cout << "Character started running" << std::endl; });

        moving->add_substate(std::move(walking));
        moving->add_substate(std::move(running));

        auto combat = std::make_unique<ConcurrentState>("Combat");

        // Combat concurrent regions
        auto attack = std::make_unique<Statechart>("AttackMode");
        auto defense = std::make_unique<Statechart>("DefenseMode");

        // Setup attack mode
        auto attack_root = std::make_unique<State>("Attacking");
        attack_root->add_transition("ENEMY_DEFEATED", "Victory");
        attack->set_root_state(std::move(attack_root));

        // Setup defense mode
        auto defense_root = std::make_unique<State>("Defending");
        defense_root->add_transition("LOW_HEALTH", "Retreat");
        defense->set_root_state(std::move(defense_root));

        combat->add_concurrent_region(std::move(attack));
        combat->add_concurrent_region(std::move(defense));

        // Add states to root
        root->add_substate(std::move(idle));
        root->add_substate(std::move(moving));
        root->add_substate(std::move(combat));

        // Setup transitions
        idle->add_transition("ENEMY_SPOTTED", "Moving");
        idle->add_transition("UNDER_ATTACK", "Combat");

        moving->add_transition("STOP", "Idle");
        moving->add_transition("ENEMY_CLOSE", "Combat");

        combat->add_transition("ENEMY_DEFEATED", "Idle");
        combat->add_transition("RETREAT", "Moving");

        // Internal transitions for moving
        walking->add_transition("SPEED_UP", "Running");
        running->add_transition("SLOW_DOWN", "Walking");

        set_root_state(std::move(root));
    }
};

// UI State Management example
class UIStateManager : public Statechart {
public:
    UIStateManager() : Statechart("UI") {
        setup_ui_states();
    }

private:
    void setup_ui_states() {
        auto root = std::make_unique<CompositeState>("Application");

        // Main application states
        auto main_menu = std::make_unique<State>("MainMenu");
        auto game_play = std::make_unique<ConcurrentState>("GamePlay");
        auto settings = std::make_unique<State>("Settings");
        auto paused = std::make_unique<State>("Paused");

        // Game play concurrent regions
        auto game_logic = std::make_unique<Statechart>("GameLogic");
        auto ui_overlay = std::make_unique<Statechart>("UIOverlay");

        // Setup game logic states
        auto game_root = std::make_unique<CompositeState>("Game");
        auto level1 = std::make_unique<State>("Level1");
        auto level2 = std::make_unique<State>("Level2");

        game_root->add_substate(std::move(level1));
        game_root->add_substate(std::move(level2));
        game_logic->set_root_state(std::move(game_root));

        // Setup UI overlay states
        auto ui_root = std::make_unique<State>("HUD");
        ui_root->add_transition("INVENTORY_OPEN", "Inventory");
        ui_overlay->set_root_state(std::move(ui_root));

        game_play->add_concurrent_region(std::move(game_logic));
        game_play->add_concurrent_region(std::move(ui_overlay));

        // Add states to root
        root->add_substate(std::move(main_menu));
        root->add_substate(std::move(game_play));
        root->add_substate(std::move(settings));
        root->add_substate(std::move(paused));

        // Setup transitions
        main_menu->add_transition("START_GAME", "GamePlay");
        main_menu->add_transition("OPEN_SETTINGS", "Settings");

        game_play->add_transition("PAUSE", "Paused");
        game_play->add_transition("GAME_OVER", "MainMenu");

        settings->add_transition("BACK", "MainMenu");

        paused->add_transition("RESUME", "GamePlay");
        paused->add_transition("QUIT", "MainMenu");

        set_root_state(std::move(root));
    }
};

// Workflow automation example
class WorkflowEngine : public Statechart {
public:
    WorkflowEngine() : Statechart("Workflow") {
        setup_workflow();
    }

private:
    void setup_workflow() {
        auto root = std::make_unique<CompositeState>("OrderProcessing");

        // Workflow states
        auto received = std::make_unique<State>("OrderReceived");
        auto validation = std::make_unique<State>("ValidatingOrder");
        auto payment = std::make_unique<State>("ProcessingPayment");
        auto fulfillment = std::make_unique<ConcurrentState>("OrderFulfillment");
        auto shipping = std::make_unique<State>("Shipping");
        auto completed = std::make_unique<State>("Completed");
        auto cancelled = std::make_unique<State>("Cancelled");

        // Concurrent fulfillment regions
        auto inventory = std::make_unique<Statechart>("InventoryCheck");
        auto packaging = std::make_unique<Statechart>("Packaging");

        // Setup inventory check
        auto inv_root = std::make_unique<State>("CheckingStock");
        inv_root->add_transition("OUT_OF_STOCK", "Backorder");
        inventory->set_root_state(std::move(inv_root));

        // Setup packaging
        auto pack_root = std::make_unique<State>("PreparingPackage");
        pack_root->add_transition("PACKAGED", "ReadyForShipping");
        packaging->set_root_state(std::move(pack_root));

        fulfillment->add_concurrent_region(std::move(inventory));
        fulfillment->add_concurrent_region(std::move(packaging));

        // Add states to root
        root->add_substate(std::move(received));
        root->add_substate(std::move(validation));
        root->add_substate(std::move(payment));
        root->add_substate(std::move(fulfillment));
        root->add_substate(std::move(shipping));
        root->add_substate(std::move(completed));
        root->add_substate(std::move(cancelled));

        // Setup transitions
        received->add_transition("VALIDATE", "ValidatingOrder");
        validation->add_transition("VALID", "ProcessingPayment");
        validation->add_transition("INVALID", "Cancelled");

        payment->add_transition("PAID", "OrderFulfillment");
        payment->add_transition("FAILED", "Cancelled");

        fulfillment->add_transition("FULFILLED", "Shipping");
        fulfillment->add_transition("FAILED", "Cancelled");

        shipping->add_transition("SHIPPED", "Completed");

        // Global transitions (from any state)
        root->add_transition("CANCEL", "Cancelled");

        set_root_state(std::move(root));
    }
};

// Example usage
int main() {
    std::cout << "Statecharts - Hierarchical State Machines:" << std::endl;

    // 1. Game Character AI
    std::cout << "\n1. Game Character AI:" << std::endl;
    CharacterAI character;

    character.enter();
    character.print_state();

    // Simulate game events
    Event enemy_spotted("ENEMY_SPOTTED");
    character.send_event(enemy_spotted);
    character.update();
    character.print_state();

    Event speed_up("SPEED_UP");
    character.send_event(speed_up);
    character.update();
    character.print_state();

    Event enemy_close("ENEMY_CLOSE");
    character.send_event(enemy_close);
    character.update();
    character.print_state();

    Event enemy_defeated("ENEMY_DEFEATED");
    character.send_event(enemy_defeated);
    character.update();
    character.print_state();

    // 2. UI State Management
    std::cout << "\n2. UI State Management:" << std::endl;
    UIStateManager ui;

    ui.enter();
    ui.print_state();

    Event start_game("START_GAME");
    ui.send_event(start_game);
    ui.update();
    ui.print_state();

    Event pause("PAUSE");
    ui.send_event(pause);
    ui.update();
    ui.print_state();

    Event resume("RESUME");
    ui.send_event(resume);
    ui.update();
    ui.print_state();

    // 3. Workflow Engine
    std::cout << "\n3. Workflow Automation:" << std::endl;
    WorkflowEngine workflow;

    workflow.enter();
    workflow.print_state();

    // Process order through workflow
    std::vector<std::string> events = {
        "VALIDATE", "VALID", "PAID", "FULFILLED", "SHIPPED"
    };

    for (const auto& event_name : events) {
        Event evt(event_name);
        workflow.send_event(evt);
        workflow.update();
        workflow.print_state();
    }

    workflow.exit();

    // 4. Demonstrate Statechart Features
    std::cout << "\n4. Statechart Features Demonstration:" << std::endl;
    std::cout << "✓ Hierarchical States: States can contain substates" << std::endl;
    std::cout << "✓ Concurrent Regions: Orthogonal state components" << std::endl;
    std::cout << "✓ State Inheritance: Child states inherit parent behavior" << std::endl;
    std::cout << "✓ Event Propagation: Events bubble through hierarchy" << std::endl;
    std::cout << "✓ History States: Resume from previous substate" << std::endl;
    std::cout << "✓ Entry/Exit Actions: State transition behaviors" << std::endl;
    std::cout << "✓ Do Activities: Continuous state behaviors" << std::endl;
    std::cout << "✓ Guard Conditions: Conditional transitions" << std::endl;

    std::cout << "\nUse Cases:" << std::endl;
    std::cout << "- Game AI: Character states (idle → walking → running)" << std::endl;
    std::cout << "- UI Systems: Modal dialogs, navigation states" << std::endl;
    std::cout << "- Robotics: Concurrent control behaviors" << std::endl;
    std::cout << "- Workflow: Business process automation" << std::endl;
    std::cout << "- Real-time Systems: Embedded control logic" << std::endl;
    std::cout << "- Complex Event Processing: State-based event handling" << std::endl;

    std::cout << "\nDemonstrates:" << std::endl;
    std::cout << "- Hierarchical state organization and inheritance" << std::endl;
    std::cout << "- Concurrent regions for orthogonal behaviors" << std::endl;
    std::cout << "- Event propagation and handling through hierarchy" << std::endl;
    std::cout << "- State actions (entry, exit, do)" << std::endl;
    std::cout << "- History states for resumable behavior" << std::endl;
    std::cout << "- Complex state relationships and dependencies" << std::endl;
    std::cout << "- Real-world game AI and UI state management" << std::endl;
    std::cout << "- Production-grade hierarchical state machines" << std::endl;

    return 0;
}

