/*
 * DFS-Based Island Traversal
 *
 * Source: LeetCode, competitive programming, graph algorithms
 * Repository: Standard algorithm libraries, competitive coding platforms
 * Files: Island counting problems, connected component analysis
 * Algorithm: Recursive depth-first search for connected components
 *
 * What Makes It Ingenious:
 * - Natural recursive exploration of connected regions
 * - Stack-based call optimization for deep recursion
 * - Boundary checking with early termination
 * - Visited state tracking to prevent cycles
 * - Simple implementation with powerful connectivity analysis
 *
 * When to Use:
 * - Grid-based island counting (LeetCode-style problems)
 * - Connected component analysis in 2D grids
 * - Maze connectivity analysis
 * - Region growing algorithms
 * - Network reachability in small graphs
 *
 * Real-World Usage:
 * - LeetCode "Number of Islands" and variants
 * - Game level connectivity analysis
 * - Image segmentation preprocessing
 * - Geographic region analysis
 * - Network topology analysis
 *
 * Time Complexity: O(rows * cols) - each cell visited once
 * Space Complexity: O(rows * cols) worst case for recursion stack
 * Connectivity: 4-way (up, down, left, right) or 8-way
 */

#include <vector>
#include <iostream>
#include <functional>
#include <stack>
#include <queue>
#include <algorithm>
#include <memory>

// Grid-based island traversal class
class GridIslandTraversal {
private:
    std::vector<std::vector<int>> grid_;
    int rows_, cols_;
    std::vector<std::vector<bool>> visited_;

    // Direction vectors for different connectivity patterns
    const std::vector<std::pair<int, int>> directions_4 = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0}
    };

    const std::vector<std::pair<int, int>> directions_8 = {
        {0, 1}, {1, 1}, {1, 0}, {1, -1},
        {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}
    };

    // Check if position is valid and unvisited land
    bool isValid(int row, int col, int land_value = 1) {
        return row >= 0 && row < rows_ && col >= 0 && col < cols_ &&
               !visited_[row][col] && grid_[row][col] == land_value;
    }

public:
    GridIslandTraversal(const std::vector<std::vector<int>>& grid)
        : grid_(grid), rows_(grid.size()), cols_(grid.empty() ? 0 : grid[0].size()),
          visited_(rows_, std::vector<bool>(cols_, false)) {}

    // Recursive DFS for island exploration
    int dfsExplore(int row, int col, int land_value = 1) {
        if (!isValid(row, col, land_value)) return 0;

        visited_[row][col] = true;
        int size = 1; // Count this cell

        // Explore all 4 directions (can be modified for 8-way)
        for (const auto& [dr, dc] : directions_4) {
            size += dfsExplore(row + dr, col + dc, land_value);
        }

        return size;
    }

    // Count total number of islands
    int countIslands(int land_value = 1) {
        resetVisited();
        int island_count = 0;

        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                if (isValid(i, j, land_value)) {
                    dfsExplore(i, j, land_value);
                    island_count++;
                }
            }
        }

        return island_count;
    }

    // Find all island sizes
    std::vector<int> getIslandSizes(int land_value = 1) {
        resetVisited();
        std::vector<int> sizes;

        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                if (isValid(i, j, land_value)) {
                    int size = dfsExplore(i, j, land_value);
                    sizes.push_back(size);
                }
            }
        }

        return sizes;
    }

    // Find largest island
    int findLargestIsland(int land_value = 1) {
        auto sizes = getIslandSizes(land_value);
        return sizes.empty() ? 0 : *std::max_element(sizes.begin(), sizes.end());
    }

    // Check if grid is fully connected (single island)
    bool isFullyConnected(int land_value = 1) {
        return countIslands(land_value) == 1;
    }

    // Get island connectivity matrix (adjacency between islands)
    std::vector<std::vector<bool>> getIslandConnectivity(int land_value = 1) {
        auto sizes = getIslandSizes(land_value);
        int num_islands = sizes.size();

        // Reset visited for connectivity analysis
        resetVisited();

        // Assign island IDs
        std::vector<std::vector<int>> island_ids(rows_, std::vector<int>(cols_, -1));
        int current_id = 0;

        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                if (isValid(i, j, land_value)) {
                    floodFillIsland(i, j, current_id++, island_ids, land_value);
                }
            }
        }

        // Build connectivity matrix (simplified - islands are connected if adjacent)
        std::vector<std::vector<bool>> connectivity(num_islands, std::vector<bool>(num_islands, false));

        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                if (island_ids[i][j] != -1) {
                    int id = island_ids[i][j];
                    // Check neighbors for different island IDs
                    for (const auto& [dr, dc] : directions_4) {
                        int ni = i + dr, nj = j + dc;
                        if (ni >= 0 && ni < rows_ && nj >= 0 && nj < cols_ &&
                            island_ids[ni][nj] != -1 && island_ids[ni][nj] != id) {
                            int other_id = island_ids[ni][nj];
                            connectivity[id][other_id] = connectivity[other_id][id] = true;
                        }
                    }
                }
            }
        }

        return connectivity;
    }

private:
    void resetVisited() {
        for (auto& row : visited_) {
            std::fill(row.begin(), row.end(), false);
        }
    }

    // Helper for flood fill with island ID assignment
    void floodFillIsland(int row, int col, int island_id,
                        std::vector<std::vector<int>>& island_ids, int land_value) {
        if (!isValid(row, col, land_value) || island_ids[row][col] != -1) return;

        visited_[row][col] = true;
        island_ids[row][col] = island_id;

        for (const auto& [dr, dc] : directions_4) {
            floodFillIsland(row + dr, col + dc, island_id, island_ids, land_value);
        }
    }
};

// Advanced DFS with stack-based iterative implementation (memory safe)
class IterativeDFSIsland {
private:
    std::vector<std::vector<int>> grid_;
    int rows_, cols_;

    const std::vector<std::pair<int, int>> directions_4 = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0}
    };

public:
    IterativeDFSIsland(const std::vector<std::vector<int>>& grid)
        : grid_(grid), rows_(grid.size()), cols_(grid.empty() ? 0 : grid[0].size()) {}

    // Iterative DFS using explicit stack (avoids recursion depth limits)
    int countIslandsIterative(int land_value = 1) {
        std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
        int island_count = 0;

        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                if (grid_[i][j] == land_value && !visited[i][j]) {
                    iterativeDFS(i, j, visited, land_value);
                    island_count++;
                }
            }
        }

        return island_count;
    }

    // Get island sizes with iterative DFS
    std::vector<int> getIslandSizesIterative(int land_value = 1) {
        std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
        std::vector<int> sizes;

        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                if (grid_[i][j] == land_value && !visited[i][j]) {
                    int size = iterativeDFS(i, j, visited, land_value);
                    sizes.push_back(size);
                }
            }
        }

        return sizes;
    }

private:
    int iterativeDFS(int start_row, int start_col, std::vector<std::vector<bool>>& visited, int land_value) {
        std::stack<std::pair<int, int>> stk;
        stk.push({start_row, start_col});
        visited[start_row][start_col] = true;
        int size = 0;

        while (!stk.empty()) {
            auto [row, col] = stk.top();
            stk.pop();
            size++;

            // Check all 4 directions
            for (const auto& [dr, dc] : directions_4) {
                int new_row = row + dr;
                int new_col = col + dc;

                if (new_row >= 0 && new_row < rows_ && new_col >= 0 && new_col < cols_ &&
                    grid_[new_row][new_col] == land_value && !visited[new_row][new_col]) {
                    visited[new_row][new_col] = true;
                    stk.push({new_row, new_col});
                }
            }
        }

        return size;
    }
};

// Advanced features for production use
class ProductionIslandAnalyzer {
private:
    std::vector<std::vector<int>> grid_;
    int rows_, cols_;

public:
    struct IslandStats {
        int id;
        int size;
        std::pair<int, int> centroid;
        std::pair<int, int> bounds_min;
        std::pair<int, int> bounds_max;
        double circularity; // Perimeter^2 / (4*PI*Area)
    };

    ProductionIslandAnalyzer(const std::vector<std::vector<int>>& grid)
        : grid_(grid), rows_(grid.size()), cols_(grid.empty() ? 0 : grid[0].size()) {}

    // Comprehensive island analysis
    std::vector<IslandStats> analyzeIslands(int land_value = 1) {
        std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
        std::vector<IslandStats> stats;

        const std::vector<std::pair<int, int>> directions_4 = {
            {0, 1}, {1, 0}, {0, -1}, {-1, 0}
        };

        int island_id = 0;
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                if (grid_[i][j] == land_value && !visited[i][j]) {
                    IslandStats stat = analyzeSingleIsland(i, j, island_id++, visited, land_value, directions_4);
                    stats.push_back(stat);
                }
            }
        }

        return stats;
    }

private:
    IslandStats analyzeSingleIsland(int start_row, int start_col, int id,
                                   std::vector<std::vector<bool>>& visited, int land_value,
                                   const std::vector<std::pair<int, int>>& directions) {
        IslandStats stats;
        stats.id = id;
        stats.size = 0;
        stats.bounds_min = {INT_MAX, INT_MAX};
        stats.bounds_max = {INT_MIN, INT_MIN};

        long long sum_row = 0, sum_col = 0;
        int perimeter = 0;

        std::stack<std::pair<int, int>> stk;
        stk.push({start_row, start_col});
        visited[start_row][start_col] = true;

        while (!stk.empty()) {
            auto [row, col] = stk.top();
            stk.pop();

            stats.size++;
            sum_row += row;
            sum_col += col;

            // Update bounds
            stats.bounds_min.first = std::min(stats.bounds_min.first, row);
            stats.bounds_min.second = std::min(stats.bounds_min.second, col);
            stats.bounds_max.first = std::max(stats.bounds_max.first, row);
            stats.bounds_max.second = std::max(stats.bounds_max.second, col);

            // Count perimeter edges
            int neighbor_count = 0;
            for (const auto& [dr, dc] : directions) {
                int nr = row + dr, nc = col + dc;

                if (nr >= 0 && nr < rows_ && nc >= 0 && nc < cols_) {
                    if (grid_[nr][nc] == land_value) {
                        neighbor_count++;
                        if (!visited[nr][nc]) {
                            visited[nr][nc] = true;
                            stk.push({nr, nc});
                        }
                    }
                }
            }

            // Each missing neighbor contributes to perimeter
            perimeter += 4 - neighbor_count;
        }

        // Calculate centroid
        stats.centroid = {
            static_cast<int>(sum_row / stats.size),
            static_cast<int>(sum_col / stats.size)
        };

        // Calculate circularity (compactness measure)
        if (stats.size > 0) {
            stats.circularity = static_cast<double>(perimeter * perimeter) / (4 * M_PI * stats.size);
        }

        return stats;
    }
};

// Example usage and testing
int main() {
    std::cout << "DFS-Based Island Traversal:" << std::endl;

    // Example grid (1 = land, 0 = water)
    std::vector<std::vector<int>> grid = {
        {1, 1, 0, 0, 0},
        {1, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 1},
        {0, 0, 0, 1, 1}
    };

    GridIslandTraversal island_analyzer(grid);

    std::cout << "Grid:" << std::endl;
    for (const auto& row : grid) {
        for (int cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "\nIsland Analysis:" << std::endl;
    std::cout << "Number of islands: " << island_analyzer.countIslands() << std::endl;

    auto sizes = island_analyzer.getIslandSizes();
    std::cout << "Island sizes: ";
    for (int size : sizes) std::cout << size << " ";
    std::cout << std::endl;

    std::cout << "Largest island: " << island_analyzer.findLargestIsland() << std::endl;
    std::cout << "Is fully connected: " << (island_analyzer.isFullyConnected() ? "Yes" : "No") << std::endl;

    // Iterative version for comparison
    IterativeDFSIsland iterative_analyzer(grid);
    std::cout << "\nIterative DFS Results:" << std::endl;
    std::cout << "Number of islands: " << iterative_analyzer.countIslandsIterative() << std::endl;

    auto iter_sizes = iterative_analyzer.getIslandSizesIterative();
    std::cout << "Island sizes: ";
    for (int size : iter_sizes) std::cout << size << " ";
    std::cout << std::endl;

    // Production analyzer
    ProductionIslandAnalyzer prod_analyzer(grid);
    auto stats = prod_analyzer.analyzeIslands();

    std::cout << "\nDetailed Island Statistics:" << std::endl;
    for (size_t i = 0; i < stats.size(); ++i) {
        const auto& stat = stats[i];
        std::cout << "Island " << stat.id << ":" << std::endl;
        std::cout << "  Size: " << stat.size << std::endl;
        std::cout << "  Centroid: (" << stat.centroid.first << ", " << stat.centroid.second << ")" << std::endl;
        std::cout << "  Bounds: (" << stat.bounds_min.first << "," << stat.bounds_min.second
                  << ") to (" << stat.bounds_max.first << "," << stat.bounds_max.second << ")" << std::endl;
        std::cout << "  Circularity: " << std::fixed << std::setprecision(2) << stat.circularity << std::endl;
    }

    // Test with larger grid
    std::cout << "\nTesting with larger grid:" << std::endl;
    std::vector<std::vector<int>> large_grid(10, std::vector<int>(10, 0));

    // Create some islands
    for (int i = 1; i < 4; ++i) {
        for (int j = 1; j < 4; ++j) large_grid[i][j] = 1;
    }
    for (int i = 6; i < 9; ++i) {
        for (int j = 6; j < 9; ++j) large_grid[i][j] = 1;
    }
    large_grid[5][5] = 1; // Single cell island

    GridIslandTraversal large_analyzer(large_grid);
    std::cout << "Large grid islands: " << large_analyzer.countIslands() << std::endl;
    auto large_sizes = large_analyzer.getIslandSizes();
    std::cout << "Sizes: ";
    for (int size : large_sizes) std::cout << size << " ";
    std::cout << std::endl;

    std::cout << "\nDemonstrates:" << std::endl;
    std::cout << "- Recursive DFS island traversal" << std::endl;
    std::cout << "- Iterative DFS for memory safety" << std::endl;
    std::cout << "- Island size calculation and statistics" << std::endl;
    std::cout << "- Production-grade island analysis" << std::endl;
    std::cout << "- Connectivity and boundary analysis" << std::endl;
    std::cout << "- Real-world island counting algorithms" << std::endl;

    return 0;
}

