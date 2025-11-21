/*
 * Recursive Behavior Trees - Game Development
 * 
 * Source: Game AI systems (Halo, Spore, Unreal Engine)
 * Pattern: Recursive tree traversal for AI decision making
 * 
 * What Makes It Ingenious:
 * - Hierarchical AI: Complex behaviors from simple nodes
 * - Recursive evaluation: Traverse tree to determine action
 * - Composable: Combine behaviors recursively
 * - Reusable: Behavior nodes can be shared
 * - Used in game AI, NPC behavior, enemy AI
 * 
 * When to Use:
 * - Game AI systems
 * - NPC behavior
 * - Enemy AI
 * - Decision making systems
 * - State machines with complex logic
 * 
 * Real-World Usage:
 * - Halo series (Bungie)
 * - Spore (Maxis)
 * - Unreal Engine behavior trees
 * - Unity behavior trees
 * - Game AI frameworks
 * 
 * Time Complexity: O(n) where n is tree depth
 * Space Complexity: O(n) for recursion stack
 */

#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <string>

class RecursiveBehaviorTree {
public:
    // Behavior status
    enum class Status {
        SUCCESS,
        FAILURE,
        RUNNING
    };
    
    // Base behavior node
    class BehaviorNode {
    public:
        virtual ~BehaviorNode() = default;
        virtual Status execute() = 0;
        virtual std::string get_name() const = 0;
    };
    
    // Leaf node: Action
    class ActionNode : public BehaviorNode {
    private:
        std::string name_;
        std::function<Status()> action_;
        
    public:
        ActionNode(const std::string& name, std::function<Status()> action)
            : name_(name), action_(action) {}
        
        Status execute() override {
            return action_();
        }
        
        std::string get_name() const override {
            return name_;
        }
    };
    
    // Leaf node: Condition
    class ConditionNode : public BehaviorNode {
    private:
        std::string name_;
        std::function<bool()> condition_;
        
    public:
        ConditionNode(const std::string& name, std::function<bool()> condition)
            : name_(name), condition_(condition) {}
        
        Status execute() override {
            return condition_() ? Status::SUCCESS : Status::FAILURE;
        }
        
        std::string get_name() const override {
            return name_;
        }
    };
    
    // Composite node: Sequence (all must succeed)
    class SequenceNode : public BehaviorNode {
    private:
        std::string name_;
        std::vector<std::shared_ptr<BehaviorNode>> children_;
        
    public:
        SequenceNode(const std::string& name) : name_(name) {}
        
        void add_child(std::shared_ptr<BehaviorNode> child) {
            children_.push_back(child);
        }
        
        Status execute() override {
            for (auto& child : children_) {
                Status result = child->execute();
                if (result != Status::SUCCESS) {
                    return result;  // Stop on first failure
                }
            }
            return Status::SUCCESS;
        }
        
        std::string get_name() const override {
            return name_;
        }
    };
    
    // Composite node: Selector (first success wins)
    class SelectorNode : public BehaviorNode {
    private:
        std::string name_;
        std::vector<std::shared_ptr<BehaviorNode>> children_;
        
    public:
        SelectorNode(const std::string& name) : name_(name) {}
        
        void add_child(std::shared_ptr<BehaviorNode> child) {
            children_.push_back(child);
        }
        
        Status execute() override {
            for (auto& child : children_) {
                Status result = child->execute();
                if (result != Status::FAILURE) {
                    return result;  // Return on first success or running
                }
            }
            return Status::FAILURE;
        }
        
        std::string get_name() const override {
            return name_;
        }
    };
    
    // Composite node: Parallel (all run, return based on policy)
    class ParallelNode : public BehaviorNode {
    public:
        enum class Policy {
            SUCCEED_ON_ONE,   // Succeed if any succeeds
            SUCCEED_ON_ALL,   // Succeed only if all succeed
            FAIL_ON_ONE       // Fail if any fails
        };
        
    private:
        std::string name_;
        std::vector<std::shared_ptr<BehaviorNode>> children_;
        Policy policy_;
        
    public:
        ParallelNode(const std::string& name, Policy p = Policy::SUCCEED_ON_ALL)
            : name_(name), policy_(p) {}
        
        void add_child(std::shared_ptr<BehaviorNode> child) {
            children_.push_back(child);
        }
        
        Status execute() override {
            int success_count = 0;
            int failure_count = 0;
            
            for (auto& child : children_) {
                Status result = child->execute();
                if (result == Status::SUCCESS) {
                    success_count++;
                } else if (result == Status::FAILURE) {
                    failure_count++;
                }
            }
            
            switch (policy_) {
                case Policy::SUCCEED_ON_ONE:
                    return success_count > 0 ? Status::SUCCESS : Status::FAILURE;
                case Policy::SUCCEED_ON_ALL:
                    return failure_count == 0 ? Status::SUCCESS : Status::FAILURE;
                case Policy::FAIL_ON_ONE:
                    return failure_count > 0 ? Status::FAILURE : Status::SUCCESS;
            }
            
            return Status::FAILURE;
        }
        
        std::string get_name() const override {
            return name_;
        }
    };
    
    // Decorator node: Inverter (negate result)
    class InverterNode : public BehaviorNode {
    private:
        std::string name_;
        std::shared_ptr<BehaviorNode> child_;
        
    public:
        InverterNode(const std::string& name, std::shared_ptr<BehaviorNode> child)
            : name_(name), child_(child) {}
        
        Status execute() override {
            Status result = child_->execute();
            if (result == Status::SUCCESS) {
                return Status::FAILURE;
            } else if (result == Status::FAILURE) {
                return Status::SUCCESS;
            }
            return Status::RUNNING;
        }
        
        std::string get_name() const override {
            return name_;
        }
    };
    
    // Decorator node: Repeater (repeat N times)
    class RepeaterNode : public BehaviorNode {
    private:
        std::string name_;
        std::shared_ptr<BehaviorNode> child_;
        int count_;
        int current_;
        
    public:
        RepeaterNode(const std::string& name, std::shared_ptr<BehaviorNode> child, int count)
            : name_(name), child_(child), count_(count), current_(0) {}
        
        Status execute() override {
            while (current_ < count_) {
                Status result = child_->execute();
                if (result == Status::FAILURE) {
                    return Status::FAILURE;
                }
                current_++;
            }
            current_ = 0;  // Reset for next execution
            return Status::SUCCESS;
        }
        
        std::string get_name() const override {
            return name_;
        }
    };
    
    // Behavior tree executor
    class BehaviorTree {
    private:
        std::shared_ptr<BehaviorNode> root_;
        
    public:
        BehaviorTree(std::shared_ptr<BehaviorNode> root) : root_(root) {}
        
        Status execute() {
            if (root_) {
                return root_->execute();
            }
            return Status::FAILURE;
        }
    };
};

// Example: Game AI behavior tree
class GameAI {
private:
    bool has_enemy_in_range_;
    bool has_ammo_;
    bool is_health_low_;
    int health_;
    
public:
    GameAI() : has_enemy_in_range_(false), has_ammo_(true), 
               is_health_low_(false), health_(50) {}
    
    void set_enemy_in_range(bool value) { has_enemy_in_range_ = value; }
    void set_ammo(bool value) { has_ammo_ = value; }
    void set_health(int value) { 
        health_ = value;
        is_health_low_ = (health_ < 30);
    }
    
    // Build behavior tree for combat AI
    std::shared_ptr<RecursiveBehaviorTree::BehaviorTree> build_combat_tree() {
        using namespace RecursiveBehaviorTree;
        
        // Root: Selector (try combat, then retreat)
        auto root = std::make_shared<SelectorNode>("Root");
        
        // Combat sequence
        auto combat = std::make_shared<SequenceNode>("Combat");
        combat->add_child(std::make_shared<ConditionNode>("Has Enemy",
            [this]() { return has_enemy_in_range_; }));
        combat->add_child(std::make_shared<ConditionNode>("Has Ammo",
            [this]() { return has_ammo_; }));
        combat->add_child(std::make_shared<ActionNode>("Shoot",
            []() { 
                std::cout << "Shooting!" << std::endl;
                return Status::SUCCESS;
            }));
        
        // Retreat sequence
        auto retreat = std::make_shared<SequenceNode>("Retreat");
        retreat->add_child(std::make_shared<ConditionNode>("Low Health",
            [this]() { return is_health_low_; }));
        retreat->add_child(std::make_shared<ActionNode>("Find Cover",
            []() {
                std::cout << "Finding cover!" << std::endl;
                return Status::SUCCESS;
            }));
        retreat->add_child(std::make_shared<ActionNode>("Heal",
            []() {
                std::cout << "Healing!" << std::endl;
                return Status::SUCCESS;
            }));
        
        root->add_child(combat);
        root->add_child(retreat);
        
        return std::make_shared<BehaviorTree>(root);
    }
};

// Example usage
int main() {
    GameAI ai;
    
    // Scenario 1: Enemy in range, has ammo
    ai.set_enemy_in_range(true);
    ai.set_ammo(true);
    ai.set_health(80);
    
    auto tree = ai.build_combat_tree();
    std::cout << "Scenario 1: " << std::endl;
    tree->execute();
    
    // Scenario 2: Low health, no enemy
    ai.set_enemy_in_range(false);
    ai.set_health(20);
    
    std::cout << "\nScenario 2: " << std::endl;
    tree->execute();
    
    return 0;
}
