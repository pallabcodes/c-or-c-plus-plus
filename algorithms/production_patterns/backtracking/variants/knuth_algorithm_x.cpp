/*
 * Knuth's Algorithm X with Dancing Links
 * 
 * Source: "Dancing Links" by Donald E. Knuth
 * Paper: "Dancing Links" (2000)
 * Algorithm: Algorithm X with Dancing Links data structure
 * 
 * What Makes It Ingenious:
 * - Dancing Links: Doubly-linked circular lists for efficient backtracking
 * - O(1) insertion and deletion: Can undo moves efficiently
 * - Exact cover problem solver: Finds all solutions to exact cover
 * - Recursive backtracking: Explores solution space systematically
 * - Used in Sudoku solvers, N-queens, and other constraint problems
 * 
 * When to Use:
 * - Exact cover problems
 * - Sudoku solving
 * - N-queens problem
 * - Pentomino tiling
 * - Constraint satisfaction problems
 * - Set covering problems
 * 
 * Real-World Usage:
 * - Sudoku solvers
 * - N-queens solvers
 * - Puzzle solvers
 * - Constraint satisfaction systems
 * - Combinatorial optimization
 * 
 * Time Complexity:
 * - Best case: O(1) if solution found immediately
 * - Worst case: O(2^n) exponential (NP-complete)
 * - Average: Depends on problem structure
 * 
 * Space Complexity: O(n + m) where n is items, m is options
 */

#include <vector>
#include <list>
#include <cstdint>

// Dancing Links node
struct DLXNode {
    DLXNode* left;
    DLXNode* right;
    DLXNode* up;
    DLXNode* down;
    DLXNode* column; // Pointer to column header
    int row_id;      // Row identifier
    int size;        // Column size (for column header)
    
    DLXNode() 
        : left(this), right(this), up(this), down(this)
        , column(this), row_id(-1), size(0) {}
};

class KnuthAlgorithmX {
private:
    std::vector<DLXNode> column_headers_;
    std::vector<std::vector<DLXNode>> nodes_; // Row nodes
    DLXNode* root_;
    int num_items_;
    int num_options_;
    std::vector<int> solution_;
    
    // Cover column: remove column and all rows in it
    void cover_column(DLXNode* col) {
        col->right->left = col->left;
        col->left->right = col->right;
        
        for (DLXNode* row = col->down; row != col; row = row->down) {
            for (DLXNode* node = row->right; node != row; node = node->right) {
                node->down->up = node->up;
                node->up->down = node->down;
                node->column->size--;
            }
        }
    }
    
    // Uncover column: restore column and all rows
    void uncover_column(DLXNode* col) {
        for (DLXNode* row = col->up; row != col; row = row->up) {
            for (DLXNode* node = row->left; node != row; node = node->left) {
                node->column->size++;
                node->down->up = node;
                node->up->down = node;
            }
        }
        col->right->left = col;
        col->left->right = col;
    }
    
    // Choose column with minimum size (heuristic)
    DLXNode* choose_column() {
        DLXNode* best = root_->right;
        int min_size = best->size;
        
        for (DLXNode* col = root_->right; col != root_; col = col->right) {
            if (col->size < min_size) {
                min_size = col->size;
                best = col;
            }
        }
        
        return best;
    }
    
public:
    KnuthAlgorithmX(int num_items) 
        : num_items_(num_items)
        , num_options_(0)
        , root_(new DLXNode()) {
        
        // Create column headers
        column_headers_.resize(num_items_);
        DLXNode* prev = root_;
        
        for (int i = 0; i < num_items_; i++) {
            DLXNode* col = &column_headers_[i];
            col->column = col;
            col->left = prev;
            col->right = root_;
            prev->right = col;
            root_->left = col;
            prev = col;
        }
    }
    
    // Add option (row) to the exact cover matrix
    // items: vector of item indices that this option covers
    void add_option(const std::vector<int>& items) {
        int row_id = num_options_++;
        nodes_.emplace_back();
        auto& row_nodes = nodes_.back();
        row_nodes.resize(items.size());
        
        DLXNode* first = nullptr;
        DLXNode* prev = nullptr;
        
        for (size_t i = 0; i < items.size(); i++) {
            int item = items[i];
            DLXNode* node = &row_nodes[i];
            DLXNode* col = &column_headers_[item];
            
            node->row_id = row_id;
            node->column = col;
            
            // Link horizontally
            if (prev) {
                node->left = prev;
                prev->right = node;
            } else {
                first = node;
            }
            prev = node;
            
            // Link vertically
            node->up = col->up;
            node->down = col;
            col->up->down = node;
            col->up = node;
            
            col->size++;
        }
        
        // Complete horizontal circle
        if (first && prev) {
            first->left = prev;
            prev->right = first;
        }
    }
    
    // Solve exact cover problem
    bool solve() {
        solution_.clear();
        return algorithm_x_recursive();
    }
    
private:
    bool algorithm_x_recursive() {
        // If no columns left, solution found
        if (root_->right == root_) {
            return true;
        }
        
        // Choose column with minimum size
        DLXNode* col = choose_column();
        
        // If column has no rows, no solution
        if (col->down == col) {
            return false;
        }
        
        // Cover chosen column
        cover_column(col);
        
        // Try each row in the column
        for (DLXNode* row = col->down; row != col; row = row->down) {
            solution_.push_back(row->row_id);
            
            // Cover all columns that this row intersects
            for (DLXNode* node = row->right; node != row; node = node->right) {
                cover_column(node->column);
            }
            
            // Recursively solve
            if (algorithm_x_recursive()) {
                return true;
            }
            
            // Backtrack: uncover columns
            for (DLXNode* node = row->left; node != row; node = node->left) {
                uncover_column(node->column);
            }
            
            solution_.pop_back();
        }
        
        // Uncover column
        uncover_column(col);
        
        return false;
    }
    
public:
    const std::vector<int>& get_solution() const {
        return solution_;
    }
    
    int num_solutions() const {
        // This implementation finds first solution
        // Can be modified to count all solutions
        return solution_.empty() ? 0 : 1;
    }
};

// Example usage: Exact Cover Problem
// Items: {0, 1, 2, 3, 4, 5}
// Options:
//   Option 0: covers items {0, 3}
//   Option 1: covers items {1, 4}
//   Option 2: covers items {2, 5}
//   Option 3: covers items {0, 1}
//   Option 4: covers items {2, 3}
// Solution: Options 1 and 4 cover all items exactly once
#include <iostream>

int main() {
    // Example exact cover: 6 items, need to cover all exactly once
    KnuthAlgorithmX solver(6);
    
    // Option 0: covers items 0, 3
    solver.add_option({0, 3});
    
    // Option 1: covers items 1, 4
    solver.add_option({1, 4});
    
    // Option 2: covers items 2, 5
    solver.add_option({2, 5});
    
    // Option 3: covers items 0, 1
    solver.add_option({0, 1});
    
    // Option 4: covers items 2, 3
    solver.add_option({2, 3});
    
    std::cout << "Solving exact cover problem..." << std::endl;
    
    if (solver.solve()) {
        std::cout << "Solution found!" << std::endl;
        const auto& solution = solver.get_solution();
        std::cout << "Selected options: ";
        for (int opt : solution) {
            std::cout << opt << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "No solution found" << std::endl;
    }
    
    return 0;
}

