/*
 * Recursive Dialogue Tree - Game Development
 * 
 * Source: Narrative game systems (RPGs, visual novels, interactive fiction)
 * Pattern: Recursive tree traversal for branching dialogue systems
 * 
 * What Makes It Ingenious:
 * - Branching narratives: Each dialogue node can have multiple responses
 * - Recursive traversal: Navigate dialogue tree recursively
 * - Dynamic dialogue: Dialogue adapts based on player choices
 * - Condition evaluation: Recursively check conditions for dialogue options
 * - Used in RPGs, visual novels, interactive fiction
 * 
 * When to Use:
 * - Branching dialogue systems
 * - Interactive narratives
 * - RPG dialogue systems
 * - Visual novels
 * - Story-driven games
 * 
 * Real-World Usage:
 * - RPG games (Mass Effect, Dragon Age)
 * - Visual novels
 * - Interactive fiction
 * - Adventure games
 * - Narrative-driven games
 * 
 * Time Complexity: O(n) where n is dialogue tree depth
 * Space Complexity: O(n) for dialogue tree
 */

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <iostream>
#include <unordered_map>

class RecursiveDialogueTree {
public:
    // Dialogue condition
    class Condition {
    public:
        virtual ~Condition() = default;
        virtual bool evaluate() const = 0;
    };
    
    // Simple condition
    class SimpleCondition : public Condition {
    private:
        std::function<bool()> evaluator_;
        
    public:
        SimpleCondition(std::function<bool()> eval) : evaluator_(eval) {}
        
        bool evaluate() const override {
            return evaluator_();
        }
    };
    
    // Dialogue option/response
    class DialogueOption {
    private:
        std::string text_;
        std::shared_ptr<Condition> condition_;
        std::shared_ptr<class DialogueNode> next_node_;
        
    public:
        DialogueOption(const std::string& text,
                      std::shared_ptr<Condition> cond = nullptr,
                      std::shared_ptr<class DialogueNode> next = nullptr)
            : text_(text), condition_(cond), next_node_(next) {}
        
        void set_next(std::shared_ptr<class DialogueNode> node) {
            next_node_ = node;
        }
        
        std::string get_text() const { return text_; }
        bool is_available() const {
            return !condition_ || condition_->evaluate();
        }
        std::shared_ptr<class DialogueNode> get_next() const { return next_node_; }
    };
    
    // Dialogue node
    class DialogueNode {
    private:
        std::string speaker_;
        std::string text_;
        std::vector<std::shared_ptr<DialogueOption>> options_;
        std::shared_ptr<DialogueNode> default_next_;
        bool is_terminal_;
        
    public:
        DialogueNode(const std::string& speaker, const std::string& text)
            : speaker_(speaker), text_(text), is_terminal_(false) {}
        
        void add_option(std::shared_ptr<DialogueOption> option) {
            options_.push_back(option);
        }
        
        void set_default_next(std::shared_ptr<DialogueNode> node) {
            default_next_ = node;
        }
        
        void set_terminal(bool terminal) {
            is_terminal_ = terminal;
        }
        
        // Get available options (recursively filter by conditions)
        std::vector<std::shared_ptr<DialogueOption>> get_available_options() const {
            std::vector<std::shared_ptr<DialogueOption>> available;
            for (const auto& option : options_) {
                if (option->is_available()) {
                    available.push_back(option);
                }
            }
            return available;
        }
        
        // Execute dialogue node (recursive traversal)
        std::shared_ptr<DialogueNode> execute(int choice_index) {
            auto available = get_available_options();
            
            if (choice_index >= 0 && choice_index < available.size()) {
                auto selected = available[choice_index];
                auto next = selected->get_next();
                if (next) {
                    return next;
                }
            }
            
            // Use default next if no valid choice
            return default_next_;
        }
        
        // Find node by ID (recursive search)
        std::shared_ptr<DialogueNode> find_node(const std::string& node_id,
                                                std::unordered_map<std::string, 
                                                std::shared_ptr<DialogueNode>>& visited) {
            if (visited.find(node_id) != visited.end()) {
                return nullptr;  // Already visited (cycle detection)
            }
            
            // Check if this is the target node (simplified - would use ID in real implementation)
            visited[node_id] = shared_from_this();
            
            // Search in options
            for (const auto& option : options_) {
                auto next = option->get_next();
                if (next) {
                    auto found = next->find_node(node_id, visited);
                    if (found) {
                        return found;
                    }
                }
            }
            
            // Search in default next
            if (default_next_) {
                return default_next_->find_node(node_id, visited);
            }
            
            return nullptr;
        }
        
        std::string get_speaker() const { return speaker_; }
        std::string get_text() const { return text_; }
        bool is_terminal() const { return is_terminal_; }
    };
    
    // Dialogue system manager
    class DialogueSystem {
    private:
        std::shared_ptr<DialogueNode> current_node_;
        std::shared_ptr<DialogueNode> root_node_;
        std::vector<std::shared_ptr<DialogueNode>> history_;
        
    public:
        DialogueSystem(std::shared_ptr<DialogueNode> root) 
            : root_node_(root), current_node_(root) {}
        
        void start() {
            current_node_ = root_node_;
            history_.clear();
        }
        
        std::shared_ptr<DialogueNode> get_current_node() const {
            return current_node_;
        }
        
        bool make_choice(int choice_index) {
            if (!current_node_ || current_node_->is_terminal()) {
                return false;
            }
            
            // Add to history
            history_.push_back(current_node_);
            
            // Execute choice
            current_node_ = current_node_->execute(choice_index);
            
            return current_node_ != nullptr;
        }
        
        bool can_go_back() const {
            return !history_.empty();
        }
        
        void go_back() {
            if (!history_.empty()) {
                current_node_ = history_.back();
                history_.pop_back();
            }
        }
        
        void reset() {
            start();
        }
    };
};

// Example usage
int main() {
    using namespace RecursiveDialogueTree;
    
    // Create dialogue nodes
    auto greeting = std::make_shared<DialogueNode>("NPC", "Hello! How can I help you?");
    auto quest_accept = std::make_shared<DialogueNode>("NPC", "Great! Here's your quest.");
    auto quest_decline = std::make_shared<DialogueNode>("NPC", "That's okay. Come back if you change your mind.");
    auto goodbye = std::make_shared<DialogueNode>("NPC", "Goodbye!");
    goodbye->set_terminal(true);
    
    // Create options
    auto option1 = std::make_shared<DialogueOption>("Accept quest", nullptr, quest_accept);
    auto option2 = std::make_shared<DialogueOption>("Decline quest", nullptr, quest_decline);
    auto option3 = std::make_shared<DialogueOption>("Goodbye", nullptr, goodbye);
    
    greeting->add_option(option1);
    greeting->add_option(option2);
    greeting->add_option(option3);
    
    quest_accept->set_default_next(goodbye);
    quest_decline->set_default_next(goodbye);
    
    // Create dialogue system
    DialogueSystem dialogue(greeting);
    dialogue.start();
    
    // Display current dialogue
    auto current = dialogue.get_current_node();
    std::cout << current->get_speaker() << ": " << current->get_text() << std::endl;
    
    // Show options
    auto options = current->get_available_options();
    for (size_t i = 0; i < options.size(); i++) {
        std::cout << "  " << i << ". " << options[i]->get_text() << std::endl;
    }
    
    // Make choice
    dialogue.make_choice(0);  // Accept quest
    current = dialogue.get_current_node();
    std::cout << "\n" << current->get_speaker() << ": " << current->get_text() << std::endl;
    
    return 0;
}

