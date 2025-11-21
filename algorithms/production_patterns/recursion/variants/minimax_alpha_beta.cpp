/*
 * Minimax with Alpha-Beta Pruning (Game Development)
 * 
 * Source: Game AI algorithms, adversarial search
 * Pattern: Recursive game tree search with pruning
 * 
 * What Makes It Ingenious:
 * - Minimax: Optimal play assuming opponent is optimal
 * - Alpha-Beta Pruning: Prunes branches that can't affect result
 * - Recursive evaluation: Recursively evaluates game tree
 * - Used in chess, checkers, tic-tac-toe, and other games
 * - Dramatically reduces search space
 * 
 * When to Use:
 * - Two-player zero-sum games
 * - Turn-based games with perfect information
 * - Game AI development
 * - Adversarial search problems
 * - Decision-making in games
 * 
 * Real-World Usage:
 * - Chess engines (Stockfish, etc.)
 * - Checkers AI
 * - Tic-tac-toe solvers
 * - Connect Four AI
 * - Game AI frameworks
 * 
 * Time Complexity: O(b^d) without pruning, O(b^(d/2)) with pruning
 * Space Complexity: O(d) for recursion depth
 */

#include <vector>
#include <algorithm>
#include <climits>
#include <iostream>

class MinimaxAlphaBeta {
public:
    // Game state representation
    enum class Player { MAX, MIN };
    enum class Cell { EMPTY, X, O };
    
    struct GameState {
        std::vector<std::vector<Cell>> board;
        Player current_player;
        int size;
        
        GameState(int n) : size(n), current_player(Player::MAX) {
            board.resize(n, std::vector<Cell>(n, Cell::EMPTY));
        }
        
        bool is_terminal() const {
            return check_winner() != Cell::EMPTY || is_full();
        }
        
        Cell check_winner() const {
            // Check rows
            for (int i = 0; i < size; i++) {
                if (board[i][0] != Cell::EMPTY) {
                    bool win = true;
                    for (int j = 1; j < size; j++) {
                        if (board[i][j] != board[i][0]) {
                            win = false;
                            break;
                        }
                    }
                    if (win) return board[i][0];
                }
            }
            
            // Check columns
            for (int j = 0; j < size; j++) {
                if (board[0][j] != Cell::EMPTY) {
                    bool win = true;
                    for (int i = 1; i < size; i++) {
                        if (board[i][j] != board[0][j]) {
                            win = false;
                            break;
                        }
                    }
                    if (win) return board[0][j];
                }
            }
            
            // Check diagonals
            if (board[0][0] != Cell::EMPTY) {
                bool win = true;
                for (int i = 1; i < size; i++) {
                    if (board[i][i] != board[0][0]) {
                        win = false;
                        break;
                    }
                }
                if (win) return board[0][0];
            }
            
            if (board[0][size - 1] != Cell::EMPTY) {
                bool win = true;
                for (int i = 1; i < size; i++) {
                    if (board[i][size - 1 - i] != board[0][size - 1]) {
                        win = false;
                        break;
                    }
                }
                if (win) return board[0][size - 1];
            }
            
            return Cell::EMPTY;
        }
        
        bool is_full() const {
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    if (board[i][j] == Cell::EMPTY) {
                        return false;
                    }
                }
            }
            return true;
        }
        
        std::vector<std::pair<int, int>> get_moves() const {
            std::vector<std::pair<int, int>> moves;
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    if (board[i][j] == Cell::EMPTY) {
                        moves.push_back({i, j});
                    }
                }
            }
            return moves;
        }
        
        GameState make_move(int row, int col) const {
            GameState new_state = *this;
            new_state.board[row][col] = 
                (current_player == Player::MAX) ? Cell::X : Cell::O;
            new_state.current_player = 
                (current_player == Player::MAX) ? Player::MIN : Player::MAX;
            return new_state;
        }
    };
    
    // Minimax with alpha-beta pruning
    static int minimax_alpha_beta(
        const GameState& state, 
        int depth, 
        int alpha, 
        int beta, 
        bool maximizing) {
        
        // Terminal state evaluation
        if (state.is_terminal() || depth == 0) {
            return evaluate_state(state);
        }
        
        if (maximizing) {
            int max_eval = INT_MIN;
            auto moves = state.get_moves();
            
            for (const auto& move : moves) {
                GameState new_state = state.make_move(move.first, move.second);
                int eval = minimax_alpha_beta(new_state, depth - 1, 
                                             alpha, beta, false);
                max_eval = std::max(max_eval, eval);
                alpha = std::max(alpha, eval);
                
                // Alpha-beta pruning
                if (beta <= alpha) {
                    break;  // Prune remaining branches
                }
            }
            
            return max_eval;
        } else {
            int min_eval = INT_MAX;
            auto moves = state.get_moves();
            
            for (const auto& move : moves) {
                GameState new_state = state.make_move(move.first, move.second);
                int eval = minimax_alpha_beta(new_state, depth - 1, 
                                             alpha, beta, true);
                min_eval = std::min(min_eval, eval);
                beta = std::min(beta, eval);
                
                // Alpha-beta pruning
                if (beta <= alpha) {
                    break;  // Prune remaining branches
                }
            }
            
            return min_eval;
        }
    }
    
    // Find best move using minimax
    static std::pair<int, int> find_best_move(
        const GameState& state, int depth) {
        
        int best_eval = INT_MIN;
        std::pair<int, int> best_move = {-1, -1};
        
        auto moves = state.get_moves();
        for (const auto& move : moves) {
            GameState new_state = state.make_move(move.first, move.second);
            int eval = minimax_alpha_beta(new_state, depth - 1, 
                                         INT_MIN, INT_MAX, false);
            
            if (eval > best_eval) {
                best_eval = eval;
                best_move = move;
            }
        }
        
        return best_move;
    }
    
private:
    // Evaluate game state (heuristic)
    static int evaluate_state(const GameState& state) {
        Cell winner = state.check_winner();
        
        if (winner == Cell::X) {
            return 10;  // MAX wins
        } else if (winner == Cell::O) {
            return -10;  // MIN wins
        } else {
            return 0;  // Draw or ongoing
        }
    }
    
public:
    // Negamax variant (simplified minimax)
    static int negamax(
        const GameState& state,
        int depth,
        int alpha,
        int beta) {
        
        if (state.is_terminal() || depth == 0) {
            int score = evaluate_state(state);
            return (state.current_player == Player::MAX) ? score : -score;
        }
        
        int max_eval = INT_MIN;
        auto moves = state.get_moves();
        
        for (const auto& move : moves) {
            GameState new_state = state.make_move(move.first, move.second);
            int eval = -negamax(new_state, depth - 1, -beta, -alpha);
            max_eval = std::max(max_eval, eval);
            alpha = std::max(alpha, eval);
            
            if (alpha >= beta) {
                break;  // Prune
            }
        }
        
        return max_eval;
    }
};

// Example usage
int main() {
    // Tic-tac-toe example
    MinimaxAlphaBeta::GameState game(3);
    
    // Make some moves
    game = game.make_move(0, 0);  // X
    game = game.make_move(1, 1);  // O
    game = game.make_move(0, 1);  // X
    
    // Find best move for current player
    auto best_move = MinimaxAlphaBeta::find_best_move(game, 5);
    std::cout << "Best move: (" << best_move.first << ", " 
              << best_move.second << ")" << std::endl;
    
    return 0;
}

