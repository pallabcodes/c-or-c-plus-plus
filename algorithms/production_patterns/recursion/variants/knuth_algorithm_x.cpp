/*
 * Knuth's Algorithm X with Dancing Links
 * 
 * Source: "Dancing Links" by Donald Knuth
 * Paper: "The Art of Computer Programming, Volume 4B"
 * Algorithm: Recursive backtracking with exact cover problem
 * 
 * What Makes It Ingenious:
 * - Dancing Links: Doubly-linked circular lists for efficient backtracking
 * - Exact Cover: Solves constraint satisfaction problems
 * - Recursive Backtracking: Depth-first search with constraint propagation
 * - O(1) undo operations: Links can be removed and restored efficiently
 * - Used in Sudoku solvers, N-Queens, polyomino tiling
 * 
 * When to Use:
 * - Exact cover problems
 * - Constraint satisfaction problems
 * - Sudoku solving
 * - N-Queens problem
 * - Polyomino tiling
 * - Set partitioning problems
 * 
 * Real-World Usage:
 * - Sudoku solvers
 * - Constraint solvers
 * - Combinatorial optimization
 * - Puzzle solving algorithms
 * 
 * Time Complexity: Exponential in worst case, but very efficient in practice
 * Space Complexity: O(n) where n is number of constraints
 */

#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

// Dancing Links node structure
struct DLXNode {
    DLXNode* left;
    DLXNode* right;
    DLXNode* up;
    DLXNode* down;
    DLXNode* column_header;  // Points to column header
    int row_id;              // Row identifier
    int column_id;           // Column identifier
    int size;                // For column headers: number of 1s in column
    
    DLXNode() 
        : left(this), right(this), up(this), down(this),
          column_header(nullptr), row_id(-1), column_id(-1), size(0) {}
};

class KnuthAlgorithmX {
private:
    DLXNode* header_;
    std::vector<std::unique_ptr<DLXNode>> nodes_;
    std::vector<DLXNode*> column_headers_;
    std::vector<int> solution_;
    int num_rows_;
    int num_cols_;
    
    // Cover a column: remove it and all rows that have 1 in this column
    void cover_column(DLXNode* col) {
        // Remove column from header list
        col->right->left = col->left;
        col->left->right = col->right;
        
        // Remove all rows in this column
        for (DLXNode* row = col->down; row != col; row = row->down) {
            for (DLXNode* node = row->right; node != row; node = node->right) {
                // Remove node from its column
                node->down->up = node->up;
                node->up->down = node->down;
                node->column_header->size--;
            }
        }
    }
    
    // Uncover a column: restore it and all rows
    void uncover_column(DLXNode* col) {
        // Restore all rows in reverse order
        for (DLXNode* row = col->up; row != col; row = row->up) {
            for (DLXNode* node = row->left; node != row; node = node->left) {
                // Restore node to its column
                node->column_header->size++;
                node->down->up = node;
                node->up->down = node;
            }
        }
        
        // Restore column to header list
        col->right->left = col;
        col->left->right = col;
    }
    
    // Choose column with minimum size (heuristic)
    DLXNode* choose_column() {
        DLXNode* chosen = header_->right;
        int min_size = chosen->size;
        
        for (DLXNode* col = header_->right; col != header_; col = col->right) {
            if (col->size < min_size) {
                min_size = col->size;
                chosen = col;
            }
        }
        
        return chosen;
    }
    
    // Recursive search with backtracking
    bool search_recursive() {
        // Base case: no columns left (exact cover found)
        if (header_->right == header_) {
            return true;
        }
        
        // Choose column with minimum size
        DLXNode* col = choose_column();
        
        // If column has no rows, no solution
        if (col->down == col) {
            return false;
        }
        
        // Cover this column
        cover_column(col);
        
        // Try each row in this column
        for (DLXNode* row = col->down; row != col; row = row->down) {
            // Add row to solution
            solution_.push_back(row->row_id);
            
            // Cover all columns that this row intersects
            for (DLXNode* node = row->right; node != row; node = node->right) {
                cover_column(node->column_header);
            }
            
            // Recursively search
            if (search_recursive()) {
                return true;
            }
            
            // Backtrack: remove row from solution
            solution_.pop_back();
            
            // Uncover all columns that this row intersects (reverse order)
            for (DLXNode* node = row->left; node != row; node = node->left) {
                uncover_column(node->column_header);
            }
        }
        
        // Uncover column
        uncover_column(col);
        
        return false;
    }
    
public:
    KnuthAlgorithmX(int rows, int cols) 
        : num_rows_(rows), num_cols_(cols) {
        // Create header node
        header_ = new DLXNode();
        nodes_.emplace_back(header_);
        
        // Create column headers
        column_headers_.resize(num_cols_);
        DLXNode* prev = header_;
        
        for (int i = 0; i < num_cols_; i++) {
            DLXNode* col = new DLXNode();
            col->column_id = i;
            col->column_header = col;
            nodes_.emplace_back(col);
            column_headers_[i] = col;
            
            // Link column headers
            prev->right = col;
            col->left = prev;
            prev = col;
        }
        
        // Link last column to header
        prev->right = header_;
        header_->left = prev;
    }
    
    // Add a row to the matrix
    void add_row(int row_id, const std::vector<int>& columns) {
        if (columns.empty()) return;
        
        DLXNode* first_node = nullptr;
        DLXNode* prev_node = nullptr;
        
        for (int col_id : columns) {
            if (col_id < 0 || col_id >= num_cols_) continue;
            
            DLXNode* node = new DLXNode();
            node->row_id = row_id;
            node->column_id = col_id;
            node->column_header = column_headers_[col_id];
            nodes_.emplace_back(node);
            
            // Link horizontally
            if (first_node == nullptr) {
                first_node = node;
                prev_node = node;
            } else {
                prev_node->right = node;
                node->left = prev_node;
                prev_node = node;
            }
            
            // Link vertically
            DLXNode* col_header = column_headers_[col_id];
            DLXNode* last_in_col = col_header->up;
            
            last_in_col->down = node;
            node->up = last_in_col;
            node->down = col_header;
            col_header->up = node;
            
            col_header->size++;
        }
        
        // Complete horizontal circle
        if (first_node != nullptr) {
            first_node->left = prev_node;
            prev_node->right = first_node;
        }
    }
    
    // Solve exact cover problem
    bool solve() {
        solution_.clear();
        return search_recursive();
    }
    
    // Get solution
    const std::vector<int>& get_solution() const {
        return solution_;
    }
};

// Example: Solve N-Queens using Algorithm X
class NQueensSolver {
public:
    static std::vector<std::vector<int>> solve(int n) {
        // Exact cover formulation:
        // Columns: 2n-1 diagonals (main and anti), n rows, n columns
        // Rows: each queen placement (n*n possibilities)
        
        int num_cols = 4 * n - 2;  // n rows + n cols + (2n-1) diag1 + (2n-1) diag2
        int num_rows = n * n;
        
        KnuthAlgorithmX solver(num_rows, num_cols);
        
        // Add rows for each queen placement
        for (int row = 0; row < n; row++) {
            for (int col = 0; col < n; col++) {
                int row_id = row * n + col;
                std::vector<int> constraints;
                
                // Row constraint
                constraints.push_back(row);
                // Column constraint
                constraints.push_back(n + col);
                // Main diagonal (row - col + n - 1)
                constraints.push_back(2 * n + (row - col + n - 1));
                // Anti diagonal (row + col)
                constraints.push_back(4 * n - 1 + (row + col));
                
                solver.add_row(row_id, constraints);
            }
        }
        
        if (solver.solve()) {
            std::vector<std::vector<int>> result;
            const auto& solution = solver.get_solution();
            
            for (int row_id : solution) {
                int row = row_id / n;
                int col = row_id % n;
                result.push_back({row, col});
            }
            
            return result;
        }
        
        return {};
    }
};

// Example usage
int main() {
    // Solve 4-Queens problem
    auto solution = NQueensSolver::solve(4);
    
    std::cout << "4-Queens solution:" << std::endl;
    for (const auto& queen : solution) {
        std::cout << "Queen at row " << queen[0] 
                  << ", column " << queen[1] << std::endl;
    }
    
    return 0;
}

