/*
 * Advanced Sudoku Solver with Backtracking - Production Grade
 * 
 * Source: Production Sudoku solvers, optimization techniques
 * Pattern: Backtracking with multiple optimization techniques
 * 
 * What Makes It Ingenious:
 * - Constraint propagation: Eliminate impossible values
 * - Naked singles: Fill cells with only one possibility
 * - Hidden singles: Find unique values in row/column/box
 * - Backtracking: Systematic search when propagation fails
 * - Used in Sudoku solvers, puzzle games
 * 
 * When to Use:
 * - Sudoku solving
 * - Constraint satisfaction puzzles
 * - Puzzle games
 * - Educational tools
 * 
 * Real-World Usage:
 * - Sudoku solver applications
 * - Puzzle game engines
 * - Educational software
 * - Constraint satisfaction systems
 * 
 * Time Complexity: O(9^m) worst case where m is empty cells
 * Space Complexity: O(81) for 9x9 grid
 */

#include <vector>
#include <unordered_set>
#include <algorithm>
#include <iostream>

class AdvancedSudokuSolver {
private:
    static const int SIZE = 9;
    static const int BOX_SIZE = 3;
    std::vector<std::vector<int>> grid_;
    std::vector<std::vector<std::unordered_set<int>>> candidates_;
    
    // Initialize candidates for each cell
    void initialize_candidates() {
        candidates_.resize(SIZE, std::vector<std::unordered_set<int>>(SIZE));
        
        for (int r = 0; r < SIZE; r++) {
            for (int c = 0; c < SIZE; c++) {
                if (grid_[r][c] == 0) {
                    // All numbers are candidates initially
                    for (int num = 1; num <= 9; num++) {
                        candidates_[r][c].insert(num);
                    }
                } else {
                    candidates_[r][c].insert(grid_[r][c]);
                }
            }
        }
    }
    
    // Remove candidate from row
    void remove_candidate_from_row(int row, int num, int exclude_col) {
        for (int c = 0; c < SIZE; c++) {
            if (c != exclude_col) {
                candidates_[row][c].erase(num);
            }
        }
    }
    
    // Remove candidate from column
    void remove_candidate_from_col(int col, int num, int exclude_row) {
        for (int r = 0; r < SIZE; r++) {
            if (r != exclude_row) {
                candidates_[r][col].erase(num);
            }
        }
    }
    
    // Remove candidate from box
    void remove_candidate_from_box(int box_row, int box_col, int num, 
                                   int exclude_row, int exclude_col) {
        int start_row = box_row * BOX_SIZE;
        int start_col = box_col * BOX_SIZE;
        
        for (int r = start_row; r < start_row + BOX_SIZE; r++) {
            for (int c = start_col; c < start_col + BOX_SIZE; c++) {
                if (r != exclude_row || c != exclude_col) {
                    candidates_[r][c].erase(num);
                }
            }
        }
    }
    
    // Constraint propagation: eliminate impossible values
    bool propagate_constraints() {
        bool changed = true;
        bool progress = false;
        
        while (changed) {
            changed = false;
            
            // Naked singles: cell with only one candidate
            for (int r = 0; r < SIZE; r++) {
                for (int c = 0; c < SIZE; c++) {
                    if (grid_[r][c] == 0 && candidates_[r][c].size() == 1) {
                        int num = *candidates_[r][c].begin();
                        if (is_valid(r, c, num)) {
                            grid_[r][c] = num;
                            remove_candidate_from_row(r, num, c);
                            remove_candidate_from_col(c, num, r);
                            remove_candidate_from_box(r / BOX_SIZE, c / BOX_SIZE, 
                                                     num, r, c);
                            changed = true;
                            progress = true;
                        }
                    }
                }
            }
            
            // Hidden singles: unique candidate in row/column/box
            for (int r = 0; r < SIZE; r++) {
                for (int c = 0; c < SIZE; c++) {
                    if (grid_[r][c] == 0) {
                        for (int num : candidates_[r][c]) {
                            // Check if unique in row
                            bool unique_in_row = true;
                            for (int c2 = 0; c2 < SIZE; c2++) {
                                if (c2 != c && candidates_[r][c2].count(num)) {
                                    unique_in_row = false;
                                    break;
                                }
                            }
                            
                            // Check if unique in column
                            bool unique_in_col = true;
                            for (int r2 = 0; r2 < SIZE; r2++) {
                                if (r2 != r && candidates_[r2][c].count(num)) {
                                    unique_in_col = false;
                                    break;
                                }
                            }
                            
                            // Check if unique in box
                            bool unique_in_box = true;
                            int box_row = r / BOX_SIZE;
                            int box_col = c / BOX_SIZE;
                            int start_row = box_row * BOX_SIZE;
                            int start_col = box_col * BOX_SIZE;
                            for (int r2 = start_row; r2 < start_row + BOX_SIZE; r2++) {
                                for (int c2 = start_col; c2 < start_col + BOX_SIZE; c2++) {
                                    if ((r2 != r || c2 != c) && 
                                        candidates_[r2][c2].count(num)) {
                                        unique_in_box = false;
                                        break;
                                    }
                                }
                                if (!unique_in_box) break;
                            }
                            
                            if (unique_in_row || unique_in_col || unique_in_box) {
                                if (is_valid(r, c, num)) {
                                    grid_[r][c] = num;
                                    candidates_[r][c].clear();
                                    candidates_[r][c].insert(num);
                                    remove_candidate_from_row(r, num, c);
                                    remove_candidate_from_col(c, num, r);
                                    remove_candidate_from_box(box_row, box_col, num, r, c);
                                    changed = true;
                                    progress = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        
        return progress;
    }
    
    // Check if number is valid at position
    bool is_valid(int row, int col, int num) {
        // Check row
        for (int c = 0; c < SIZE; c++) {
            if (grid_[row][c] == num) return false;
        }
        
        // Check column
        for (int r = 0; r < SIZE; r++) {
            if (grid_[r][col] == num) return false;
        }
        
        // Check box
        int box_row = (row / BOX_SIZE) * BOX_SIZE;
        int box_col = (col / BOX_SIZE) * BOX_SIZE;
        for (int r = box_row; r < box_row + BOX_SIZE; r++) {
            for (int c = box_col; c < box_col + BOX_SIZE; c++) {
                if (grid_[r][c] == num) return false;
            }
        }
        
        return true;
    }
    
    // Find cell with fewest candidates (MRV heuristic)
    std::pair<int, int> find_best_cell() {
        int min_candidates = SIZE + 1;
        std::pair<int, int> best_cell = {-1, -1};
        
        for (int r = 0; r < SIZE; r++) {
            for (int c = 0; c < SIZE; c++) {
                if (grid_[r][c] == 0) {
                    int candidate_count = candidates_[r][c].size();
                    if (candidate_count > 0 && candidate_count < min_candidates) {
                        min_candidates = candidate_count;
                        best_cell = {r, c};
                    }
                }
            }
        }
        
        return best_cell;
    }
    
    // Backtracking search
    bool backtrack_search() {
        // Try constraint propagation first
        propagate_constraints();
        
        // Find best cell to try
        auto [row, col] = find_best_cell();
        if (row == -1) {
            return true;  // Solved
        }
        
        // Try each candidate
        std::vector<int> candidate_list(candidates_[row][col].begin(), 
                                       candidates_[row][col].end());
        
        for (int num : candidate_list) {
            if (is_valid(row, col, num)) {
                grid_[row][col] = num;
                
                // Save state
                auto saved_candidates = candidates_;
                candidates_[row][col].clear();
                candidates_[row][col].insert(num);
                remove_candidate_from_row(row, num, col);
                remove_candidate_from_col(col, num, row);
                remove_candidate_from_box(row / BOX_SIZE, col / BOX_SIZE, 
                                         num, row, col);
                
                if (backtrack_search()) {
                    return true;
                }
                
                // Backtrack
                grid_[row][col] = 0;
                candidates_ = saved_candidates;
            }
        }
        
        return false;
    }
    
public:
    AdvancedSudokuSolver(const std::vector<std::vector<int>>& initial_grid)
        : grid_(initial_grid) {
        initialize_candidates();
    }
    
    bool solve() {
        return backtrack_search();
    }
    
    std::vector<std::vector<int>> get_solution() const {
        return grid_;
    }
    
    void print_grid() const {
        for (int r = 0; r < SIZE; r++) {
            if (r % 3 == 0 && r > 0) {
                std::cout << "------+-------+------" << std::endl;
            }
            for (int c = 0; c < SIZE; c++) {
                if (c % 3 == 0 && c > 0) {
                    std::cout << "| ";
                }
                std::cout << grid_[r][c] << " ";
            }
            std::cout << std::endl;
        }
    }
};

// Example usage
int main() {
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
    
    AdvancedSudokuSolver solver(sudoku);
    
    std::cout << "Original puzzle:" << std::endl;
    solver.print_grid();
    
    if (solver.solve()) {
        std::cout << "\nSolved:" << std::endl;
        solver.print_grid();
    } else {
        std::cout << "\nNo solution found" << std::endl;
    }
    
    return 0;
}

