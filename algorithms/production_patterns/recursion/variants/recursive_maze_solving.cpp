/*
 * Recursive Maze Solving - Game Development
 * 
 * Source: Classic pathfinding algorithms
 * Pattern: Recursive backtracking for maze navigation
 * 
 * What Makes It Ingenious:
 * - Recursive backtracking: Natural for maze solving
 * - Multiple algorithms: DFS, BFS, A* variants
 * - Path reconstruction: Builds path recursively
 * - Used in game pathfinding, procedural generation
 * 
 * When to Use:
 * - Maze solving in games
 * - Pathfinding algorithms
 * - Procedural maze generation
 * - Game AI navigation
 * - Puzzle game mechanics
 * 
 * Real-World Usage:
 * - Game pathfinding systems
 * - Maze games
 * - Procedural level generation
 * - AI navigation
 * 
 * Time Complexity: O(V + E) for DFS/BFS
 * Space Complexity: O(V) for recursion stack
 */

#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include <stack>

class RecursiveMazeSolving {
public:
    enum class CellType { WALL, PATH, START, END, VISITED, SOLUTION };
    
    struct Maze {
        std::vector<std::vector<CellType>> grid;
        int rows, cols;
        std::pair<int, int> start, end;
        
        Maze(int r, int c) : rows(r), cols(c) {
            grid.resize(r, std::vector<CellType>(c, CellType::WALL));
        }
        
        bool is_valid(int row, int col) const {
            return row >= 0 && row < rows && col >= 0 && col < cols;
        }
        
        bool is_wall(int row, int col) const {
            return !is_valid(row, col) || grid[row][col] == CellType::WALL;
        }
        
        bool is_visited(int row, int col) const {
            return grid[row][col] == CellType::VISITED || 
                   grid[row][col] == CellType::SOLUTION;
        }
    };
    
    // Recursive DFS maze solving
    static bool solve_dfs_recursive(
        Maze& maze,
        int row, int col,
        std::vector<std::pair<int, int>>& path) {
        
        // Base case: reached end
        if (row == maze.end.first && col == maze.end.second) {
            path.push_back({row, col});
            maze.grid[row][col] = CellType::SOLUTION;
            return true;
        }
        
        // Check bounds and walls
        if (!maze.is_valid(row, col) || maze.is_wall(row, col) || 
            maze.is_visited(row, col)) {
            return false;
        }
        
        // Mark as visited
        maze.grid[row][col] = CellType::VISITED;
        path.push_back({row, col});
        
        // Try all four directions
        int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        
        for (int i = 0; i < 4; i++) {
            int new_row = row + directions[i][0];
            int new_col = col + directions[i][1];
            
            if (solve_dfs_recursive(maze, new_row, new_col, path)) {
                maze.grid[row][col] = CellType::SOLUTION;
                return true;
            }
        }
        
        // Backtrack
        path.pop_back();
        return false;
    }
    
    // Recursive BFS-like with path reconstruction
    static bool solve_bfs_recursive(
        const Maze& maze,
        std::queue<std::pair<int, int>>& queue,
        std::vector<std::vector<std::pair<int, int>>>& parent,
        std::vector<std::pair<int, int>>& path) {
        
        if (queue.empty()) {
            return false;
        }
        
        auto [row, col] = queue.front();
        queue.pop();
        
        // Check if reached end
        if (row == maze.end.first && col == maze.end.second) {
            // Reconstruct path recursively
            reconstruct_path(parent, row, col, path);
            return true;
        }
        
        // Explore neighbors
        int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        
        for (int i = 0; i < 4; i++) {
            int new_row = row + directions[i][0];
            int new_col = col + directions[i][1];
            
            if (maze.is_valid(new_row, new_col) && 
                !maze.is_wall(new_row, new_col) &&
                parent[new_row][new_col].first == -1) {
                
                parent[new_row][new_col] = {row, col};
                queue.push({new_row, new_col});
            }
        }
        
        return solve_bfs_recursive(maze, queue, parent, path);
    }
    
    // Recursive path reconstruction
    static void reconstruct_path(
        const std::vector<std::vector<std::pair<int, int>>>& parent,
        int row, int col,
        std::vector<std::pair<int, int>>& path) {
        
        if (parent[row][col].first == -1) {
            path.push_back({row, col});
            return;
        }
        
        auto [prev_row, prev_col] = parent[row][col];
        reconstruct_path(parent, prev_row, prev_col, path);
        path.push_back({row, col});
    }
    
    // Recursive maze generation (backtracking)
    static void generate_maze_recursive(
        Maze& maze,
        int row, int col,
        std::vector<std::vector<bool>>& visited) {
        
        visited[row][col] = true;
        maze.grid[row][col] = CellType::PATH;
        
        // Random directions
        int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        std::vector<int> indices = {0, 1, 2, 3};
        std::random_shuffle(indices.begin(), indices.end());
        
        for (int idx : indices) {
            int new_row = row + 2 * directions[idx][0];
            int new_col = col + 2 * directions[idx][1];
            
            if (maze.is_valid(new_row, new_col) && 
                !visited[new_row][new_col]) {
                
                // Carve path
                int wall_row = row + directions[idx][0];
                int wall_col = col + directions[idx][1];
                maze.grid[wall_row][wall_col] = CellType::PATH;
                
                generate_maze_recursive(maze, new_row, new_col, visited);
            }
        }
    }
    
    // Count paths recursively (number of paths from start to end)
    static int count_paths_recursive(
        const Maze& maze,
        int row, int col,
        std::vector<std::vector<bool>>& visited) {
        
        // Base case: reached end
        if (row == maze.end.first && col == maze.end.second) {
            return 1;
        }
        
        // Check bounds and walls
        if (!maze.is_valid(row, col) || maze.is_wall(row, col) || 
            visited[row][col]) {
            return 0;
        }
        
        visited[row][col] = true;
        int count = 0;
        
        // Try all four directions
        int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        
        for (int i = 0; i < 4; i++) {
            int new_row = row + directions[i][0];
            int new_col = col + directions[i][1];
            
            count += count_paths_recursive(maze, new_row, new_col, visited);
        }
        
        visited[row][col] = false;  // Backtrack
        return count;
    }
    
    // Find shortest path length recursively
    static int shortest_path_length_recursive(
        const Maze& maze,
        int row, int col,
        std::vector<std::vector<bool>>& visited,
        int current_length) {
        
        // Base case: reached end
        if (row == maze.end.first && col == maze.end.second) {
            return current_length;
        }
        
        // Check bounds and walls
        if (!maze.is_valid(row, col) || maze.is_wall(row, col) || 
            visited[row][col]) {
            return -1;  // Invalid path
        }
        
        visited[row][col] = true;
        int min_length = -1;
        
        // Try all four directions
        int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        
        for (int i = 0; i < 4; i++) {
            int new_row = row + directions[i][0];
            int new_col = col + directions[i][1];
            
            int length = shortest_path_length_recursive(
                maze, new_row, new_col, visited, current_length + 1);
            
            if (length != -1) {
                if (min_length == -1 || length < min_length) {
                    min_length = length;
                }
            }
        }
        
        visited[row][col] = false;  // Backtrack
        return min_length;
    }
};

// Example usage
int main() {
    // Create simple maze
    RecursiveMazeSolving::Maze maze(5, 5);
    
    // Set start and end
    maze.start = {0, 0};
    maze.end = {4, 4};
    
    // Create simple path
    for (int i = 0; i < 5; i++) {
        maze.grid[0][i] = RecursiveMazeSolving::CellType::PATH;
        maze.grid[i][4] = RecursiveMazeSolving::CellType::PATH;
    }
    
    // Solve maze
    std::vector<std::pair<int, int>> path;
    bool solved = RecursiveMazeSolving::solve_dfs_recursive(
        maze, maze.start.first, maze.start.second, path);
    
    if (solved) {
        std::cout << "Maze solved! Path length: " << path.size() << std::endl;
    } else {
        std::cout << "No solution found" << std::endl;
    }
    
    return 0;
}

