/*
 * Recursive State Machine - Game Development
 * 
 * Source: Game AI and state management systems
 * Pattern: Hierarchical state machines with recursive state handling
 * 
 * What Makes It Ingenious:
 * - Hierarchical states: States can contain substates
 * - Recursive state transitions: Handle nested state changes
 * - State inheritance: Child states inherit parent behavior
 * - Recursive event handling: Events propagate through hierarchy
 * - Used in game AI, character state management, UI systems
 * 
 * When to Use:
 * - Complex game AI states
 * - Character state management
 * - UI state machines
 * - Game flow control
 * - Nested state systems
 * 
 * Real-World Usage:
 * - Game AI systems
 * - Character controllers
 * - UI frameworks
 * - Game state management
 * - Animation state machines
 * 
 * Time Complexity: O(h) where h is state hierarchy depth
 * Space Complexity: O(n) where n is number of states
 */

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <iostream>

class RecursiveStateMachine {
public:
    // State event
    enum class Event {
        ENTER,
        EXIT,
        UPDATE,
        CUSTOM
    };
    
    // Base state class
    class State {
    protected:
        std::string name_;
        std::weak_ptr<State> parent_;
        std::vector<std::shared_ptr<State>> children_;
        std::shared_ptr<State> current_child_;
        
    public:
        State(const std::string& name) : name_(name) {}
        virtual ~State() = default;
        
        virtual void on_enter() {}
        virtual void on_exit() {}
        virtual void on_update(float delta_time) {}
        virtual void on_event(const std::string& event_name, void* data = nullptr) {}
        
        void set_parent(std::shared_ptr<State> parent) {
            parent_ = parent;
        }
        
        void add_child(std::shared_ptr<State> child) {
            child->set_parent(shared_from_this());
            children_.push_back(child);
        }
        
        // Recursively enter state
        void enter() {
            on_enter();
            
            // Enter first child if exists
            if (!children_.empty() && !current_child_) {
                current_child_ = children_[0];
                current_child_->enter();
            }
        }
        
        // Recursively exit state
        void exit() {
            // Exit current child first
            if (current_child_) {
                current_child_->exit();
                current_child_ = nullptr;
            }
            
            on_exit();
        }
        
        // Recursively update state
        void update(float delta_time) {
            on_update(delta_time);
            
            // Update current child
            if (current_child_) {
                current_child_->update(delta_time);
            }
        }
        
        // Recursively handle event
        bool handle_event(const std::string& event_name, void* data = nullptr) {
            // Try to handle in current child first
            if (current_child_ && current_child_->handle_event(event_name, data)) {
                return true;
            }
            
            // Handle in this state
            on_event(event_name, data);
            
            // Try parent if not handled
            auto parent = parent_.lock();
            if (parent) {
                return parent->handle_event(event_name, data);
            }
            
            return false;
        }
        
        // Transition to child state
        bool transition_to(const std::string& state_name) {
            for (auto& child : children_) {
                if (child->name_ == state_name) {
                    if (current_child_) {
                        current_child_->exit();
                    }
                    current_child_ = child;
                    current_child_->enter();
                    return true;
                }
            }
            return false;
        }
        
        // Recursively find state
        std::shared_ptr<State> find_state(const std::string& name) {
            if (name_ == name) {
                return shared_from_this();
            }
            
            for (auto& child : children_) {
                auto found = child->find_state(name);
                if (found) {
                    return found;
                }
            }
            
            return nullptr;
        }
        
        std::string get_name() const { return name_; }
        std::shared_ptr<State> get_current_child() const { return current_child_; }
    };
    
    // State machine manager
    class StateMachine {
    private:
        std::shared_ptr<State> root_state_;
        std::shared_ptr<State> current_state_;
        
    public:
        StateMachine(std::shared_ptr<State> root) 
            : root_state_(root), current_state_(root) {
            if (root_state_) {
                root_state_->enter();
            }
        }
        
        void update(float delta_time) {
            if (current_state_) {
                current_state_->update(delta_time);
            }
        }
        
        bool transition_to(const std::string& state_name) {
            if (current_state_) {
                // Try to transition in current state hierarchy
                if (current_state_->transition_to(state_name)) {
                    return true;
                }
                
                // Try to find state in entire tree
                auto target = root_state_->find_state(state_name);
                if (target) {
                    // Exit current state
                    current_state_->exit();
                    
                    // Enter target state
                    current_state_ = target;
                    current_state_->enter();
                    return true;
                }
            }
            return false;
        }
        
        void send_event(const std::string& event_name, void* data = nullptr) {
            if (current_state_) {
                current_state_->handle_event(event_name, data);
            }
        }
        
        std::shared_ptr<State> get_current_state() const {
            return current_state_;
        }
    };
    
    // Example: Character state machine
    class CharacterState : public State {
    public:
        CharacterState(const std::string& name) : State(name) {}
    };
    
    class IdleState : public CharacterState {
    public:
        IdleState() : CharacterState("Idle") {}
        
        void on_enter() override {
            std::cout << "Entering Idle state" << std::endl;
        }
        
        void on_update(float delta_time) override {
            // Idle animation, etc.
        }
    };
    
    class MoveState : public CharacterState {
    public:
        MoveState() : CharacterState("Move") {}
        
        void on_enter() override {
            std::cout << "Entering Move state" << std::endl;
        }
        
        void on_update(float delta_time) override {
            // Movement logic
        }
    };
    
    class CombatState : public CharacterState {
    public:
        CombatState() : CharacterState("Combat") {}
        
        void on_enter() override {
            std::cout << "Entering Combat state" << std::endl;
        }
    };
    
    class AttackState : public CharacterState {
    public:
        AttackState() : CharacterState("Attack") {}
        
        void on_enter() override {
            std::cout << "Entering Attack state" << std::endl;
        }
    };
    
    class BlockState : public CharacterState {
    public:
        BlockState() : CharacterState("Block") {}
        
        void on_enter() override {
            std::cout << "Entering Block state" << std::endl;
        }
    };
};

// Example usage
int main() {
    using namespace RecursiveStateMachine;
    
    // Create character state machine
    auto root = std::make_shared<CharacterState>("Root");
    auto idle = std::make_shared<IdleState>();
    auto move = std::make_shared<MoveState>();
    auto combat = std::make_shared<CombatState>();
    auto attack = std::make_shared<AttackState>();
    auto block = std::make_shared<BlockState>();
    
    // Build hierarchy
    root->add_child(idle);
    root->add_child(move);
    root->add_child(combat);
    combat->add_child(attack);
    combat->add_child(block);
    
    // Create state machine
    StateMachine machine(root);
    
    // Update
    machine.update(0.016f);  // ~60 FPS
    
    // Transition to combat
    machine.transition_to("Combat");
    machine.update(0.016f);
    
    // Transition to attack within combat
    machine.transition_to("Attack");
    machine.update(0.016f);
    
    return 0;
}

