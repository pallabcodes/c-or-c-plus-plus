/*
 * Monte Carlo Tree Search (MCTS) - Game Development
 * 
 * Source: Game AI research, UCT algorithm
 * Pattern: Recursive tree search with Monte Carlo simulations
 * 
 * What Makes It Ingenious:
 * - UCT algorithm: Upper Confidence Bound applied to Trees
 * - Monte Carlo simulations: Random playouts for evaluation
 * - Recursive tree building: Builds game tree incrementally
 * - Used in Go, Chess, and other games
 * - Balances exploration and exploitation
 * 
 * When to Use:
 * - Games with large state spaces
 * - Games where evaluation is expensive
 * - Real-time strategy games
 * - Board games (Go, Chess, etc.)
 * - Game AI development
 * 
 * Real-World Usage:
 * - AlphaGo (Google DeepMind)
 * - Chess engines
 * - Game AI frameworks
 * - Real-time strategy game AI
 * 
 * Time Complexity: O(n) where n is number of simulations
 * Space Complexity: O(n) for tree nodes
 */

#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <iostream>
#include <memory>

class MonteCarloTreeSearch {
public:
    // Game state interface
    struct GameState {
        virtual ~GameState() = default;
        virtual bool is_terminal() const = 0;
        virtual double get_reward() const = 0;  // Reward for current player
        virtual std::vector<std::shared_ptr<GameState>> get_children() const = 0;
        virtual std::shared_ptr<GameState> make_move(int move) const = 0;
        virtual int get_current_player() const = 0;
    };
    
    // MCTS Node
    struct MCTSNode {
        std::shared_ptr<GameState> state;
        std::shared_ptr<MCTSNode> parent;
        std::vector<std::shared_ptr<MCTSNode>> children;
        int visits;
        double total_reward;
        int untried_moves;
        std::vector<int> untried_move_list;
        
        MCTSNode(std::shared_ptr<GameState> s, 
                std::shared_ptr<MCTSNode> p = nullptr)
            : state(s), parent(p), visits(0), total_reward(0.0) {
            auto moves = s->get_children();
            untried_moves = moves.size();
            for (size_t i = 0; i < moves.size(); i++) {
                untried_move_list.push_back(i);
            }
        }
        
        bool is_fully_expanded() const {
            return untried_moves == 0;
        }
        
        double ucb_value(double exploration_constant = 1.414) const {
            if (visits == 0) return std::numeric_limits<double>::infinity();
            
            double exploitation = total_reward / visits;
            double exploration = exploration_constant * 
                std::sqrt(std::log(parent->visits) / visits);
            
            return exploitation + exploration;
        }
    };
    
    // MCTS Algorithm
    class MCTS {
    private:
        std::shared_ptr<MCTSNode> root_;
        double exploration_constant_;
        std::mt19937 rng_;
        
        // Selection: Select best child using UCB
        std::shared_ptr<MCTSNode> select(std::shared_ptr<MCTSNode> node) {
            while (node->is_fully_expanded() && !node->state->is_terminal()) {
                node = best_child(node);
            }
            return node;
        }
        
        // Expansion: Add new child node
        std::shared_ptr<MCTSNode> expand(std::shared_ptr<MCTSNode> node) {
            if (node->untried_moves == 0) {
                return node;
            }
            
            // Select random untried move
            std::uniform_int_distribution<int> dist(0, node->untried_moves - 1);
            int move_idx = dist(rng_);
            int move = node->untried_move_list[move_idx];
            
            // Remove move from untried list
            node->untried_move_list.erase(
                node->untried_move_list.begin() + move_idx);
            node->untried_moves--;
            
            // Create new child
            auto new_state = node->state->make_move(move);
            auto child = std::make_shared<MCTSNode>(new_state, node);
            node->children.push_back(child);
            
            return child;
        }
        
        // Simulation: Random playout
        double simulate(std::shared_ptr<GameState> state) {
            auto current = state;
            
            while (!current->is_terminal()) {
                auto children = current->get_children();
                if (children.empty()) break;
                
                std::uniform_int_distribution<int> dist(0, children.size() - 1);
                current = children[dist(rng_)];
            }
            
            return current->get_reward();
        }
        
        // Backpropagation: Update node statistics
        void backpropagate(std::shared_ptr<MCTSNode> node, double reward) {
            while (node != nullptr) {
                node->visits++;
                node->total_reward += reward;
                reward = -reward;  // Alternate for opponent
                node = node->parent;
            }
        }
        
        // Find best child using UCB
        std::shared_ptr<MCTSNode> best_child(std::shared_ptr<MCTSNode> node) {
            if (node->children.empty()) {
                return nullptr;
            }
            
            std::shared_ptr<MCTSNode> best = node->children[0];
            double best_ucb = best->ucb_value(exploration_constant_);
            
            for (auto child : node->children) {
                double ucb = child->ucb_value(exploration_constant_);
                if (ucb > best_ucb) {
                    best_ucb = ucb;
                    best = child;
                }
            }
            
            return best;
        }
        
    public:
        MCTS(std::shared_ptr<GameState> root_state, 
             double exploration = 1.414,
             unsigned int seed = 0)
            : exploration_constant_(exploration), rng_(seed) {
            root_ = std::make_shared<MCTSNode>(root_state);
        }
        
        // Run one iteration of MCTS
        void iterate() {
            // Selection
            auto node = select(root_);
            
            // Expansion
            node = expand(node);
            
            // Simulation
            double reward = simulate(node->state);
            
            // Backpropagation
            backpropagate(node, reward);
        }
        
        // Run multiple iterations
        void run(int iterations) {
            for (int i = 0; i < iterations; i++) {
                iterate();
            }
        }
        
        // Get best move
        int get_best_move() {
            if (root_->children.empty()) {
                return -1;
            }
            
            std::shared_ptr<MCTSNode> best = root_->children[0];
            int best_visits = best->visits;
            
            for (size_t i = 0; i < root_->children.size(); i++) {
                if (root_->children[i]->visits > best_visits) {
                    best_visits = root_->children[i]->visits;
                    best = root_->children[i];
                }
            }
            
            // Find index of best child
            for (size_t i = 0; i < root_->children.size(); i++) {
                if (root_->children[i] == best) {
                    return i;
                }
            }
            
            return -1;
        }
        
        // Get root node statistics
        int get_root_visits() const {
            return root_->visits;
        }
    };
};

// Example: Simple Tic-Tac-Toe state
class TicTacToeState : public MonteCarloTreeSearch::GameState {
private:
    std::vector<std::vector<int>> board_;
    int current_player_;
    int size_;
    
public:
    TicTacToeState(int n = 3) : size_(n), current_player_(1) {
        board_.resize(n, std::vector<int>(n, 0));
    }
    
    bool is_terminal() const override {
        return check_winner() != 0 || is_full();
    }
    
    double get_reward() const override {
        int winner = check_winner();
        if (winner == current_player_) return 1.0;
        if (winner == -current_player_) return -1.0;
        return 0.0;  // Draw
    }
    
    std::vector<std::shared_ptr<GameState>> get_children() const override {
        std::vector<std::shared_ptr<GameState>> children;
        
        for (int i = 0; i < size_; i++) {
            for (int j = 0; j < size_; j++) {
                if (board_[i][j] == 0) {
                    auto child = std::make_shared<TicTacToeState>(*this);
                    child->board_[i][j] = current_player_;
                    child->current_player_ = -current_player_;
                    children.push_back(child);
                }
            }
        }
        
        return children;
    }
    
    std::shared_ptr<GameState> make_move(int move) const override {
        auto children = get_children();
        if (move >= 0 && move < children.size()) {
            return children[move];
        }
        return nullptr;
    }
    
    int get_current_player() const override {
        return current_player_;
    }
    
private:
    int check_winner() const {
        // Check rows and columns
        for (int i = 0; i < size_; i++) {
            if (board_[i][0] != 0) {
                bool row_win = true;
                for (int j = 1; j < size_; j++) {
                    if (board_[i][j] != board_[i][0]) {
                        row_win = false;
                        break;
                    }
                }
                if (row_win) return board_[i][0];
            }
            
            if (board_[0][i] != 0) {
                bool col_win = true;
                for (int j = 1; j < size_; j++) {
                    if (board_[j][i] != board_[0][i]) {
                        col_win = false;
                        break;
                    }
                }
                if (col_win) return board_[0][i];
            }
        }
        
        // Check diagonals
        if (board_[0][0] != 0) {
            bool diag_win = true;
            for (int i = 1; i < size_; i++) {
                if (board_[i][i] != board_[0][0]) {
                    diag_win = false;
                    break;
                }
            }
            if (diag_win) return board_[0][0];
        }
        
        if (board_[0][size_ - 1] != 0) {
            bool diag_win = true;
            for (int i = 1; i < size_; i++) {
                if (board_[i][size_ - 1 - i] != board_[0][size_ - 1]) {
                    diag_win = false;
                    break;
                }
            }
            if (diag_win) return board_[0][size_ - 1];
        }
        
        return 0;
    }
    
    bool is_full() const {
        for (int i = 0; i < size_; i++) {
            for (int j = 0; j < size_; j++) {
                if (board_[i][j] == 0) {
                    return false;
                }
            }
        }
        return true;
    }
};

// Example usage
int main() {
    auto game_state = std::make_shared<TicTacToeState>(3);
    MonteCarloTreeSearch::MCTS mcts(game_state, 1.414, 42);
    
    // Run MCTS
    mcts.run(1000);
    
    // Get best move
    int best_move = mcts.get_best_move();
    std::cout << "Best move after 1000 iterations: " << best_move << std::endl;
    std::cout << "Root visits: " << mcts.get_root_visits() << std::endl;
    
    return 0;
}

