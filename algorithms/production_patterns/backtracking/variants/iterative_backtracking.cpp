/*
 * Iterative Backtracking - Non-Recursive Backtracking
 * 
 * Source: Production backtracking implementations
 * Pattern: Stack-based iterative backtracking instead of recursion
 * 
 * What Makes It Ingenious:
 * - Stack-based: Use explicit stack instead of recursion
 * - No stack overflow: Can handle deeper search trees
 * - Better control: Explicit control over backtracking
 * - Memory efficient: Can limit stack size
 * - Used in production systems, embedded systems
 * 
 * When to Use:
 * - Deep search trees
 * - Stack-constrained environments
 * - Need explicit control over backtracking
 * - Production systems requiring stability
 * 
 * Real-World Usage:
 * - Production CSP solvers
 * - Embedded systems
 * - Systems with limited stack space
 * - Large-scale backtracking problems
 * 
 * Time Complexity: Same as recursive version
 * Space Complexity: O(d) where d is depth (explicit stack)
 */

#include <vector>
#include <stack>
#include <iostream>

class IterativeBacktracking {
public:
    // Search state
    struct SearchState {
        int variable_index;
        int value_index;
        std::vector<int> assignment;
        
        SearchState(int var_idx, int val_idx, const std::vector<int>& assn)
            : variable_index(var_idx), value_index(val_idx), assignment(assn) {}
    };
    
    // N-Queens solver using iterative backtracking
    class NQueensSolver {
    private:
        int n_;
        std::vector<int> solution_;
        
        bool is_safe(const std::vector<int>& board, int row, int col) {
            // Check column
            for (int i = 0; i < row; i++) {
                if (board[i] == col) {
                    return false;
                }
            }
            
            // Check diagonals
            for (int i = 0; i < row; i++) {
                int diff = row - i;
                if (board[i] == col - diff || board[i] == col + diff) {
                    return false;
                }
            }
            
            return true;
        }
        
    public:
        NQueensSolver(int n) : n_(n) {
            solution_.resize(n, -1);
        }
        
        bool solve() {
            std::stack<SearchState> stack;
            
            // Initialize: try first row
            for (int col = 0; col < n_; col++) {
                std::vector<int> initial_assignment(n_, -1);
                initial_assignment[0] = col;
                stack.push(SearchState(0, col, initial_assignment));
            }
            
            while (!stack.empty()) {
                SearchState state = stack.top();
                stack.pop();
                
                int row = state.variable_index;
                int col = state.value_index;
                
                // Check if this is a complete solution
                if (row == n_ - 1) {
                    solution_ = state.assignment;
                    return true;
                }
                
                // Try next row
                int next_row = row + 1;
                for (int next_col = 0; next_col < n_; next_col++) {
                    if (is_safe(state.assignment, next_row, next_col)) {
                        std::vector<int> new_assignment = state.assignment;
                        new_assignment[next_row] = next_col;
                        stack.push(SearchState(next_row, next_col, new_assignment));
                    }
                }
            }
            
            return false;
        }
        
        std::vector<int> get_solution() const {
            return solution_;
        }
    };
    
    // Sudoku solver using iterative backtracking
    class SudokuSolver {
    private:
        static const int SIZE = 9;
        std::vector<std::vector<int>> grid_;
        
        bool is_valid(int row, int col, int num) {
            // Check row
            for (int c = 0; c < SIZE; c++) {
                if (grid_[row][c] == num) return false;
            }
            
            // Check column
            for (int r = 0; r < SIZE; r++) {
                if (grid_[r][col] == num) return false;
            }
            
            // Check 3x3 box
            int box_row = (row / 3) * 3;
            int box_col = (col / 3) * 3;
            for (int r = box_row; r < box_row + 3; r++) {
                for (int c = box_col; c < box_col + 3; c++) {
                    if (grid_[r][c] == num) return false;
                }
            }
            
            return true;
        }
        
        std::pair<int, int> find_empty() {
            for (int r = 0; r < SIZE; r++) {
                for (int c = 0; c < SIZE; c++) {
                    if (grid_[r][c] == 0) {
                        return {r, c};
                    }
                }
            }
            return {-1, -1};
        }
        
    public:
        SudokuSolver(const std::vector<std::vector<int>>& initial_grid)
            : grid_(initial_grid) {}
        
        bool solve() {
            std::stack<SearchState> stack;
            
            auto [row, col] = find_empty();
            if (row == -1) {
                return true;  // Already solved
            }
            
            // Initialize stack with first empty cell
            for (int num = 1; num <= 9; num++) {
                if (is_valid(row, col, num)) {
                    std::vector<int> initial_assignment = {row, col, num};
                    stack.push(SearchState(0, num, initial_assignment));
                }
            }
            
            while (!stack.empty()) {
                SearchState state = stack.top();
                stack.pop();
                
                int r = state.assignment[0];
                int c = state.assignment[1];
                int num = state.value_index;
                
                // Place number
                grid_[r][c] = num;
                
                // Find next empty cell
                auto [next_row, next_col] = find_empty();
                if (next_row == -1) {
                    return true;  // Solved
                }
                
                // Try numbers for next cell
                bool found = false;
                for (int next_num = 1; next_num <= 9; next_num++) {
                    if (is_valid(next_row, next_col, next_num)) {
                        std::vector<int> new_assignment = {next_row, next_col, next_num};
                        stack.push(SearchState(0, next_num, new_assignment));
                        found = true;
                    }
                }
                
                // If no valid number found, backtrack
                if (!found) {
                    grid_[r][c] = 0;  // Unassign
                }
            }
            
            return false;
        }
        
        std::vector<std::vector<int>> get_solution() const {
            return grid_;
        }
    };
};

// Example usage
int main() {
    // N-Queens
    IterativeBacktracking::NQueensSolver queens(4);
    if (queens.solve()) {
        std::cout << "4-Queens solution:" << std::endl;
        auto solution = queens.get_solution();
        for (size_t i = 0; i < solution.size(); i++) {
            std::cout << "Row " << i << ", Column " << solution[i] << std::endl;
        }
    }
    
    // Sudoku
    std::vector<std::vector<int>> sudoku = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };
    
    IterativeBacktracking::SudokuSolver solver(sudoku);
    if (solver.solve()) {
        std::cout << "\nSudoku solved!" << std::endl;
    } else {
        std::cout << "\nSudoku unsolvable" << std::endl;
    }
    
    return 0;
}

