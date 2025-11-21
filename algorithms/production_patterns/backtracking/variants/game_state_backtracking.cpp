/*
 * Game State Backtracking - Game Development
 * 
 * Source: Game engines, undo/redo systems, save/load systems
 * Pattern: Backtracking through game state history
 * 
 * What Makes It Ingenious:
 * - State snapshots: Save game states for backtracking
 * - Incremental state: Store only changes (delta compression)
 * - Time travel: Go back to any previous state
 * - Branching timelines: Multiple state branches
 * - Used in game engines, replay systems, debugging
 * 
 * When to Use:
 * - Undo/redo systems
 * - Save/load systems
 * - Replay systems
 * - Game state debugging
 * - Time manipulation mechanics
 * 
 * Real-World Usage:
 * - Game engines (Unity, Unreal)
 * - Strategy games (undo moves)
 * - Puzzle games (reset to checkpoint)
 * - Replay systems
 * - Game debugging tools
 * 
 * Time Complexity: O(1) for state access, O(n) for state creation
 * Space Complexity: O(n) for state history
 */

#include <vector>
#include <memory>
#include <unordered_map>
#include <stack>
#include <iostream>
#include <functional>

class GameStateBacktracking {
public:
    // Game state interface
    class GameState {
    public:
        virtual ~GameState() = default;
        virtual std::unique_ptr<GameState> clone() const = 0;
        virtual bool equals(const GameState& other) const = 0;
        virtual void apply() = 0;  // Apply this state
        virtual void revert() = 0;  // Revert to previous state
    };
    
    // Simple game state (example: board game)
    class BoardGameState : public GameState {
    private:
        std::vector<std::vector<int>> board_;
        int current_player_;
        int move_count_;
        
    public:
        BoardGameState(int size, int players)
            : current_player_(0), move_count_(0) {
            board_.resize(size, std::vector<int>(size, -1));
        }
        
        BoardGameState(const BoardGameState& other)
            : board_(other.board_), current_player_(other.current_player_),
              move_count_(other.move_count_) {}
        
        std::unique_ptr<GameState> clone() const override {
            return std::make_unique<BoardGameState>(*this);
        }
        
        bool equals(const GameState& other) const override {
            const BoardGameState* other_state = 
                dynamic_cast<const BoardGameState*>(&other);
            if (!other_state) return false;
            
            return board_ == other_state->board_ &&
                   current_player_ == other_state->current_player_ &&
                   move_count_ == other_state->move_count_;
        }
        
        void apply() override {
            // State is already applied (it IS the state)
        }
        
        void revert() override {
            // Would revert to previous state
        }
        
        bool make_move(int row, int col) {
            if (row < 0 || row >= board_.size() ||
                col < 0 || col >= board_[0].size() ||
                board_[row][col] != -1) {
                return false;
            }
            
            board_[row][col] = current_player_;
            current_player_ = (current_player_ + 1) % 2;
            move_count_++;
            return true;
        }
        
        void print() const {
            for (const auto& row : board_) {
                for (int cell : row) {
                    if (cell == -1) {
                        std::cout << ". ";
                    } else {
                        std::cout << cell << " ";
                    }
                }
                std::cout << std::endl;
            }
        }
    };
    
    // State manager with backtracking
    class StateManager {
    private:
        std::stack<std::unique_ptr<GameState>> state_history_;
        std::unique_ptr<GameState> current_state_;
        int max_history_size_;
        
    public:
        StateManager(std::unique_ptr<GameState> initial_state, int max_history = 100)
            : current_state_(std::move(initial_state)), max_history_size_(max_history) {}
        
        // Save current state
        void save_state() {
            if (current_state_) {
                state_history_.push(current_state_->clone());
                
                // Limit history size
                if (state_history_.size() > max_history_size_) {
                    // Remove oldest (would need deque for this, simplified here)
                }
            }
        }
        
        // Backtrack to previous state
        bool backtrack() {
            if (state_history_.empty()) {
                return false;
            }
            
            current_state_ = std::move(state_history_.top());
            state_history_.pop();
            current_state_->apply();
            return true;
        }
        
        // Get current state
        GameState* get_current_state() {
            return current_state_.get();
        }
        
        // Check if can backtrack
        bool can_backtrack() const {
            return !state_history_.empty();
        }
        
        int history_size() const {
            return state_history_.size();
        }
    };
    
    // Branching timeline (multiple state branches)
    class BranchingTimeline {
    private:
        struct TimelineNode {
            std::unique_ptr<GameState> state;
            std::vector<std::unique_ptr<TimelineNode>> branches;
            TimelineNode* parent;
            int branch_id;
            
            TimelineNode(std::unique_ptr<GameState> s, TimelineNode* p = nullptr, int id = 0)
                : state(std::move(s)), parent(p), branch_id(id) {}
        };
        
        TimelineNode* current_node_;
        TimelineNode* root_node_;
        int next_branch_id_;
        
    public:
        BranchingTimeline(std::unique_ptr<GameState> initial_state)
            : next_branch_id_(0) {
            root_node_ = new TimelineNode(std::move(initial_state));
            current_node_ = root_node_;
        }
        
        ~BranchingTimeline() {
            // Cleanup (simplified - would need proper tree deletion)
        }
        
        // Create branch from current state
        int create_branch() {
            if (!current_node_) return -1;
            
            auto branch_state = current_node_->state->clone();
            auto branch_node = std::make_unique<TimelineNode>(
                std::move(branch_state), current_node_, next_branch_id_);
            
            int branch_id = next_branch_id_++;
            current_node_->branches.push_back(std::move(branch_node));
            
            return branch_id;
        }
        
        // Switch to branch
        bool switch_to_branch(int branch_id) {
            if (!current_node_) return false;
            
            for (auto& branch : current_node_->branches) {
                if (branch->branch_id == branch_id) {
                    current_node_ = branch.get();
                    current_node_->state->apply();
                    return true;
                }
            }
            
            return false;
        }
        
        // Go back to parent
        bool go_to_parent() {
            if (!current_node_ || !current_node_->parent) {
                return false;
            }
            
            current_node_ = current_node_->parent;
            current_node_->state->apply();
            return true;
        }
        
        // Update current state
        void update_state(std::unique_ptr<GameState> new_state) {
            if (current_node_) {
                current_node_->state = std::move(new_state);
            }
        }
        
        GameState* get_current_state() {
            return current_node_ ? current_node_->state.get() : nullptr;
        }
    };
};

// Example usage
int main() {
    using namespace GameStateBacktracking;
    
    // Create state manager
    auto initial_state = std::make_unique<BoardGameState>(3, 2);
    StateManager manager(std::move(initial_state));
    
    // Make some moves
    auto* state = dynamic_cast<BoardGameState*>(manager.get_current_state());
    if (state) {
        manager.save_state();
        state->make_move(0, 0);
        
        manager.save_state();
        state->make_move(1, 1);
        
        std::cout << "After 2 moves:" << std::endl;
        state->print();
        
        // Backtrack
        if (manager.backtrack()) {
            std::cout << "\nAfter backtrack:" << std::endl;
            state = dynamic_cast<BoardGameState*>(manager.get_current_state());
            if (state) {
                state->print();
            }
        }
    }
    
    return 0;
}
