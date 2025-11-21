/*
 * Maze Generation with Backtracking - Game Development
 * 
 * Source: Maze generation algorithms, recursive backtracking
 * Pattern: Recursive backtracking for procedural maze generation
 * 
 * What Makes It Ingenious:
 * - Recursive backtracking: Carve paths and backtrack on dead ends
 * - Guaranteed solvability: Always creates solvable mazes
 * - Perfect mazes: One unique path between any two points
 * - Used in roguelikes, dungeon crawlers, puzzle games
 * 
 * When to Use:
 * - Procedural maze generation
 * - Dungeon generation
 * - Level generation
 * - Puzzle game mazes
 * - Roguelike games
 * 
 * Real-World Usage:
 * - Roguelike games (Dungeon Crawl, Nethack)
 * - Maze games
 * - Dungeon generators
 * - Procedural level generation
 * - Puzzle games
 * 
 * Time Complexity: O(n) where n is number of cells
 * Space Complexity: O(n) for recursion stack
 */

#include <vector>
#include <random>
#include <algorithm>
#include <iostream>

class MazeGenerationBacktracking {
public:
    enum class CellType { WALL, PATH, START, END };
    
    struct Cell {
        int row, col;
        bool visited;
        CellType type;
        
        Cell(int r, int c) : row(r), col(c), visited(false), type(CellType::WALL) {}
    };
    
    // Maze generator using recursive backtracking
    class MazeGenerator {
    private:
        int rows_, cols_;
        std::vector<std::vector<Cell>> grid_;
        std::mt19937 rng_;
        
        // Get unvisited neighbors
        std::vector<std::pair<int, int>> get_unvisited_neighbors(int row, int col) {
            std::vector<std::pair<int, int>> neighbors;
            std::vector<std::pair<int, int>> directions = {
                {0, 2}, {2, 0}, {0, -2}, {-2, 0}  // Step by 2 (skip walls)
            };
            
            for (const auto& [dr, dc] : directions) {
                int new_row = row + dr;
                int new_col = col + dc;
                
                if (new_row > 0 && new_row < rows_ - 1 &&
                    new_col > 0 && new_col < cols_ - 1 &&
                    !grid_[new_row][new_col].visited) {
                    neighbors.push_back({new_row, new_col});
                }
            }
            
            return neighbors;
        }
        
        // Carve path between two cells
        void carve_path(int r1, int c1, int r2, int c2) {
            // Mark both cells as path
            grid_[r1][c1].type = CellType::PATH;
            grid_[r2][c2].type = CellType::PATH;
            
            // Carve wall between them
            int mid_r = (r1 + r2) / 2;
            int mid_c = (c1 + c2) / 2;
            grid_[mid_r][mid_c].type = CellType::PATH;
        }
        
        // Recursive backtracking generation
        void generate_recursive(int row, int col) {
            grid_[row][col].visited = true;
            grid_[row][col].type = CellType::PATH;
            
            // Get unvisited neighbors
            auto neighbors = get_unvisited_neighbors(row, col);
            
            // Shuffle for randomness
            std::shuffle(neighbors.begin(), neighbors.end(), rng_);
            
            // Visit each neighbor
            for (const auto& [next_row, next_col] : neighbors) {
                if (!grid_[next_row][next_col].visited) {
                    // Carve path
                    carve_path(row, col, next_row, next_col);
                    
                    // Recursively generate
                    generate_recursive(next_row, next_col);
                }
            }
        }
        
    public:
        MazeGenerator(int rows, int cols, int seed = 0)
            : rows_(rows), cols_(cols), rng_(seed) {
            // Initialize grid (must be odd dimensions for perfect maze)
            if (rows_ % 2 == 0) rows_++;
            if (cols_ % 2 == 0) cols_++;
            
            grid_.resize(rows_, std::vector<Cell>(cols_));
            for (int r = 0; r < rows_; r++) {
                for (int c = 0; c < cols_; c++) {
                    grid_[r][c] = Cell(r, c);
                }
            }
        }
        
        void generate() {
            // Start from (1, 1) - must be odd coordinates
            generate_recursive(1, 1);
            
            // Set start and end
            grid_[1][1].type = CellType::START;
            grid_[rows_ - 2][cols_ - 2].type = CellType::END;
        }
        
        std::vector<std::vector<CellType>> get_maze() const {
            std::vector<std::vector<CellType>> maze(rows_, 
                std::vector<CellType>(cols_));
            for (int r = 0; r < rows_; r++) {
                for (int c = 0; c < cols_; c++) {
                    maze[r][c] = grid_[r][c].type;
                }
            }
            return maze;
        }
        
        void print() const {
            for (int r = 0; r < rows_; r++) {
                for (int c = 0; c < cols_; c++) {
                    switch (grid_[r][c].type) {
                        case CellType::WALL:
                            std::cout << "# ";
                            break;
                        case CellType::PATH:
                            std::cout << "  ";
                            break;
                        case CellType::START:
                            std::cout << "S ";
                            break;
                        case CellType::END:
                            std::cout << "E ";
                            break;
                    }
                }
                std::cout << std::endl;
            }
        }
    };
    
    // Maze solver using backtracking
    class MazeSolver {
    private:
        std::vector<std::vector<CellType>> maze_;
        int rows_, cols_;
        std::vector<std::pair<int, int>> solution_path_;
        std::vector<std::vector<bool>> visited_;
        
        bool solve_recursive(int row, int col, 
                            std::vector<std::pair<int, int>>& path) {
            // Check bounds
            if (row < 0 || row >= rows_ || col < 0 || col >= cols_) {
                return false;
            }
            
            // Check if wall or visited
            if (maze_[row][col] == CellType::WALL || visited_[row][col]) {
                return false;
            }
            
            // Check if reached end
            if (maze_[row][col] == CellType::END) {
                path.push_back({row, col});
                return true;
            }
            
            // Mark as visited
            visited_[row][col] = true;
            path.push_back({row, col});
            
            // Try all directions
            std::vector<std::pair<int, int>> directions = {
                {0, 1}, {1, 0}, {0, -1}, {-1, 0}
            };
            
            for (const auto& [dr, dc] : directions) {
                if (solve_recursive(row + dr, col + dc, path)) {
                    return true;
                }
            }
            
            // Backtrack
            path.pop_back();
            return false;
        }
        
    public:
        MazeSolver(const std::vector<std::vector<CellType>>& maze)
            : maze_(maze), rows_(maze.size()), cols_(maze[0].size()) {
            visited_.resize(rows_, std::vector<bool>(cols_, false));
        }
        
        bool solve(int start_row, int start_col) {
            solution_path_.clear();
            for (int r = 0; r < rows_; r++) {
                for (int c = 0; c < cols_; c++) {
                    visited_[r][c] = false;
                }
            }
            
            return solve_recursive(start_row, start_col, solution_path_);
        }
        
        std::vector<std::pair<int, int>> get_solution() const {
            return solution_path_;
        }
    };
};

// Example usage
int main() {
    using namespace MazeGenerationBacktracking;
    
    // Generate maze
    MazeGenerator generator(21, 21, 12345);
    generator.generate();
    
    std::cout << "Generated maze:" << std::endl;
    generator.print();
    
    // Solve maze
    auto maze = generator.get_maze();
    MazeSolver solver(maze);
    
    if (solver.solve(1, 1)) {
        std::cout << "\nSolution found!" << std::endl;
        std::cout << "Path length: " << solver.get_solution().size() << " cells" << std::endl;
    }
    
    return 0;
}

