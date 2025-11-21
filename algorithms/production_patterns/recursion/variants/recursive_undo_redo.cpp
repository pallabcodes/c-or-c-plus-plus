/*
 * Recursive Undo/Redo System - Game Development
 * 
 * Source: Game editors, command pattern implementations
 * Pattern: Recursive command history with undo/redo stacks
 * 
 * What Makes It Ingenious:
 * - Command pattern: Encapsulate operations as commands
 * - Recursive undo: Undo composite commands recursively
 * - Command grouping: Group commands for atomic operations
 * - Macro commands: Execute multiple commands as one
 * - Used in game editors, level editors, undo systems
 * 
 * When to Use:
 * - Game editors
 * - Level editors
 * - Undo/redo functionality
 * - Command history
 * - Transaction systems
 * 
 * Real-World Usage:
 * - Game level editors
 * - 3D modeling software
 * - Game development tools
 * - UI frameworks with undo
 * - Version control systems
 * 
 * Time Complexity: O(n) where n is command history depth
 * Space Complexity: O(n) for command history
 */

#include <vector>
#include <memory>
#include <stack>
#include <functional>
#include <iostream>
#include <string>

class RecursiveUndoRedo {
public:
    // Base command interface
    class Command {
    protected:
        std::string description_;
        
    public:
        Command(const std::string& desc) : description_(desc) {}
        virtual ~Command() = default;
        
        virtual void execute() = 0;
        virtual void undo() = 0;
        virtual bool can_undo() const { return true; }
        
        std::string get_description() const { return description_; }
    };
    
    // Simple command
    class SimpleCommand : public Command {
    private:
        std::function<void()> execute_func_;
        std::function<void()> undo_func_;
        
    public:
        SimpleCommand(const std::string& desc,
                     std::function<void()> exec,
                     std::function<void()> undo)
            : Command(desc), execute_func_(exec), undo_func_(undo) {}
        
        void execute() override {
            if (execute_func_) {
                execute_func_();
            }
        }
        
        void undo() override {
            if (undo_func_) {
                undo_func_();
            }
        }
    };
    
    // Macro command (composite command)
    class MacroCommand : public Command {
    private:
        std::vector<std::shared_ptr<Command>> commands_;
        
    public:
        MacroCommand(const std::string& desc) : Command(desc) {}
        
        void add_command(std::shared_ptr<Command> cmd) {
            commands_.push_back(cmd);
        }
        
        void execute() override {
            // Execute all commands in order
            for (auto& cmd : commands_) {
                cmd->execute();
            }
        }
        
        void undo() override {
            // Undo all commands in reverse order (recursive)
            for (auto it = commands_.rbegin(); it != commands_.rend(); ++it) {
                (*it)->undo();
            }
        }
        
        size_t get_command_count() const {
            return commands_.size();
        }
    };
    
    // Command manager with undo/redo stacks
    class CommandManager {
    private:
        std::stack<std::shared_ptr<Command>> undo_stack_;
        std::stack<std::shared_ptr<Command>> redo_stack_;
        int max_history_size_;
        
        void clear_redo_stack() {
            while (!redo_stack_.empty()) {
                redo_stack_.pop();
            }
        }
        
        void limit_history() {
            // Limit undo stack size
            while (undo_stack_.size() > max_history_size_) {
                undo_stack_.pop();
            }
        }
        
    public:
        CommandManager(int max_history = 100) 
            : max_history_size_(max_history) {}
        
        void execute_command(std::shared_ptr<Command> cmd) {
            if (!cmd) return;
            
            // Execute command
            cmd->execute();
            
            // Push to undo stack
            undo_stack_.push(cmd);
            
            // Clear redo stack (new command invalidates redo)
            clear_redo_stack();
            
            // Limit history
            limit_history();
        }
        
        bool undo() {
            if (undo_stack_.empty()) {
                return false;
            }
            
            auto cmd = undo_stack_.top();
            undo_stack_.pop();
            
            // Undo command (recursive for macro commands)
            cmd->undo();
            
            // Push to redo stack
            redo_stack_.push(cmd);
            
            return true;
        }
        
        bool redo() {
            if (redo_stack_.empty()) {
                return false;
            }
            
            auto cmd = redo_stack_.top();
            redo_stack_.pop();
            
            // Re-execute command (recursive for macro commands)
            cmd->execute();
            
            // Push back to undo stack
            undo_stack_.push(cmd);
            
            return true;
        }
        
        bool can_undo() const {
            return !undo_stack_.empty();
        }
        
        bool can_redo() const {
            return !redo_stack_.empty();
        }
        
        size_t undo_count() const {
            return undo_stack_.size();
        }
        
        size_t redo_count() const {
            return redo_stack_.size();
        }
        
        void clear() {
            while (!undo_stack_.empty()) {
                undo_stack_.pop();
            }
            clear_redo_stack();
        }
    };
    
    // Example: Game object property change command
    class SetPropertyCommand : public Command {
    private:
        int* target_;
        int old_value_;
        int new_value_;
        
    public:
        SetPropertyCommand(int* target, int new_val, const std::string& desc)
            : Command(desc), target_(target), new_value_(new_val) {
            if (target_) {
                old_value_ = *target_;
            }
        }
        
        void execute() override {
            if (target_) {
                *target_ = new_value_;
            }
        }
        
        void undo() override {
            if (target_) {
                *target_ = old_value_;
            }
        }
    };
};

// Example usage
int main() {
    using namespace RecursiveUndoRedo;
    
    // Create command manager
    CommandManager manager;
    
    // Example: Modify game object properties
    int health = 100;
    int mana = 50;
    
    // Create commands
    auto cmd1 = std::make_shared<SetPropertyCommand>(
        &health, 80, "Set health to 80");
    auto cmd2 = std::make_shared<SetPropertyCommand>(
        &mana, 30, "Set mana to 30");
    
    // Create macro command
    auto macro = std::make_shared<MacroCommand>("Update player stats");
    macro->add_command(cmd1);
    macro->add_command(cmd2);
    
    // Execute macro
    std::cout << "Before: health=" << health << ", mana=" << mana << std::endl;
    manager.execute_command(macro);
    std::cout << "After execute: health=" << health << ", mana=" << mana << std::endl;
    
    // Undo
    manager.undo();
    std::cout << "After undo: health=" << health << ", mana=" << mana << std::endl;
    
    // Redo
    manager.redo();
    std::cout << "After redo: health=" << health << ", mana=" << mana << std::endl;
    
    return 0;
}

