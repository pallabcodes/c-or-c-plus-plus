/*
 * Recursive Flood Fill
 *
 * Source: Classic paint programs, image editors, interactive graphics
 * Repository: GIMP, early graphics libraries, educational implementations
 * Files: Bucket fill tools, interactive painting algorithms
 * Algorithm: Recursive depth-first filling of connected regions
 *
 * What Makes It Ingenious:
 * - Natural recursive exploration following connected pixels
 * - Simple and intuitive implementation
 * - Interactive feel for paint applications
 * - Direct mapping to user expectations
 * - Foundation for more advanced filling algorithms
 *
 * When to Use:
 * - Interactive paint applications
 * - Simple image editing tools
 * - Educational demonstrations
 * - Small to medium sized regions
- When recursion depth is manageable
 * - Basic bucket fill functionality
 *
 * Real-World Usage:
 * - Classic paint programs (Paintbrush, early Photoshop)
 * - Simple image editors and drawing tools
 * - Educational algorithm demonstrations
 * - Game level editors
 * - Basic graphics libraries
 * - Interactive design tools
 *
 * Time Complexity: O(width * height) in worst case
 * Space Complexity: O(recursion depth) - can cause stack overflow
 * Connectivity: 4-way or 8-way with tolerance for color matching
 */

#include <vector>
#include <iostream>
#include <functional>
#include <stack>
#include <queue>
#include <algorithm>
#include <cmath>
#include <memory>

// Basic recursive flood fill for grids
class RecursiveFloodFill {
private:
    std::vector<std::vector<int>> grid_;
    int rows_, cols_;

    // Direction vectors for different connectivity
    const std::vector<std::pair<int, int>> directions_4 = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0}
    };

    const std::vector<std::pair<int, int>> directions_8 = {
        {0, 1}, {1, 1}, {1, 0}, {1, -1},
        {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}
    };

    // Check if position is valid and matches fill criteria
    bool canFill(int row, int col, int target_value, int new_value,
                const std::vector<std::vector<bool>>& visited) const {
        return row >= 0 && row < rows_ && col >= 0 && col < cols_ &&
               !visited[row][col] && grid_[row][col] == target_value &&
               grid_[row][col] != new_value;
    }

public:
    RecursiveFloodFill(const std::vector<std::vector<int>>& grid)
        : grid_(grid), rows_(grid.size()), cols_(grid.empty() ? 0 : grid[0].size()) {}

    // Basic recursive flood fill with 4-way connectivity
    void floodFill4Way(int start_row, int start_col, int new_value) {
        if (start_row < 0 || start_row >= rows_ || start_col < 0 || start_col >= cols_) {
            return;
        }

        int target_value = grid_[start_row][start_col];
        if (target_value == new_value) return;

        std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
        floodFillRecursive(start_row, start_col, target_value, new_value, visited, directions_4);
    }

    // Recursive flood fill with 8-way connectivity
    void floodFill8Way(int start_row, int start_col, int new_value) {
        if (start_row < 0 || start_row >= rows_ || start_col < 0 || start_col >= cols_) {
            return;
        }

        int target_value = grid_[start_row][start_col];
        if (target_value == new_value) return;

        std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
        floodFillRecursive(start_row, start_col, target_value, new_value, visited, directions_8);
    }

    // Recursive flood fill with tolerance (for color images)
    void floodFillWithTolerance(int start_row, int start_col, int new_value, int tolerance) {
        if (start_row < 0 || start_row >= rows_ || start_col < 0 || start_col >= cols_) {
            return;
        }

        int target_value = grid_[start_row][start_col];
        if (std::abs(target_value - new_value) <= tolerance) return;

        std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
        floodFillRecursiveWithTolerance(start_row, start_col, target_value, new_value,
                                      tolerance, visited, directions_4);
    }

private:
    void floodFillRecursive(int row, int col, int target_value, int new_value,
                           std::vector<std::vector<bool>>& visited,
                           const std::vector<std::pair<int, int>>& directions) {
        if (!canFill(row, col, target_value, new_value, visited)) {
            return;
        }

        visited[row][col] = true;
        grid_[row][col] = new_value;

        // Recursively fill all neighbors
        for (const auto& [dr, dc] : directions) {
            floodFillRecursive(row + dr, col + dc, target_value, new_value, visited, directions);
        }
    }

    void floodFillRecursiveWithTolerance(int row, int col, int target_value, int new_value,
                                       int tolerance, std::vector<std::vector<bool>>& visited,
                                       const std::vector<std::pair<int, int>>& directions) {
        if (row < 0 || row >= rows_ || col < 0 || col >= cols_ ||
            visited[row][col] || std::abs(grid_[row][col] - target_value) > tolerance) {
            return;
        }

        visited[row][col] = true;
        int old_value = grid_[row][col];
        grid_[row][col] = new_value;

        // Recursively fill all neighbors within tolerance
        for (const auto& [dr, dc] : directions) {
            floodFillRecursiveWithTolerance(row + dr, col + dc, target_value, new_value,
                                          tolerance, visited, directions);
        }
    }

public:
    // Get current grid state
    const std::vector<std::vector<int>>& getGrid() const { return grid_; }

    // Print grid
    void printGrid(const std::string& title = "Grid") const {
        std::cout << title << " (" << rows_ << "x" << cols_ << "):" << std::endl;
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                std::cout << std::setw(3) << grid_[i][j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};

// Advanced recursive flood fill with features
class AdvancedRecursiveFloodFill {
private:
    std::vector<std::vector<int>> grid_;
    int rows_, cols_;

    // Statistics tracking
    struct FillStats {
        int pixels_filled;
        int recursion_depth;
        std::pair<int, int> bounds_min;
        std::pair<int, int> bounds_max;
    };

public:
    AdvancedRecursiveFloodFill(const std::vector<std::vector<int>>& grid)
        : grid_(grid), rows_(grid.size()), cols_(grid.empty() ? 0 : grid[0].size()) {}

    // Flood fill with statistics and bounds checking
    FillStats floodFillWithStats(int start_row, int start_col, int new_value,
                               bool use_8_way = false) {
        if (start_row < 0 || start_row >= rows_ || start_col < 0 || start_col >= cols_) {
            return {0, 0, {0, 0}, {0, 0}};
        }

        int target_value = grid_[start_row][start_col];
        if (target_value == new_value) return {0, 0, {0, 0}, {0, 0}};

        std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
        FillStats stats = {0, 0, {INT_MAX, INT_MAX}, {INT_MIN, INT_MIN}};

        const auto& directions = use_8_way ? getDirections8() : getDirections4();

        floodFillRecursiveStats(start_row, start_col, target_value, new_value, visited,
                              directions, stats, 0);

        return stats;
    }

    // Flood fill with custom fill condition
    int floodFillConditional(int start_row, int start_col, int new_value,
                           std::function<bool(int, int, int)> condition) {
        if (start_row < 0 || start_row >= rows_ || start_col < 0 || start_col >= cols_) {
            return 0;
        }

        std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
        int pixels_filled = 0;

        floodFillRecursiveConditional(start_row, start_col, new_value, visited,
                                    condition, pixels_filled);

        return pixels_filled;
    }

    // Preview flood fill (returns affected pixels without modifying)
    std::vector<std::pair<int, int>> previewFloodFill(int start_row, int start_col,
                                                     bool use_8_way = false) {
        if (start_row < 0 || start_row >= rows_ || start_col < 0 || start_col >= cols_) {
            return {};
        }

        int target_value = grid_[start_row][start_col];
        std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
        std::vector<std::pair<int, int>> affected_pixels;

        const auto& directions = use_8_way ? getDirections8() : getDirections4();

        previewFloodFillRecursive(start_row, start_col, target_value, visited,
                                directions, affected_pixels);

        return affected_pixels;
    }

private:
    const std::vector<std::pair<int, int>>& getDirections4() {
        static const std::vector<std::pair<int, int>> dirs = {
            {0, 1}, {1, 0}, {0, -1}, {-1, 0}
        };
        return dirs;
    }

    const std::vector<std::pair<int, int>>& getDirections8() {
        static const std::vector<std::pair<int, int>> dirs = {
            {0, 1}, {1, 1}, {1, 0}, {1, -1},
            {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}
        };
        return dirs;
    }

    void floodFillRecursiveStats(int row, int col, int target_value, int new_value,
                               std::vector<std::vector<bool>>& visited,
                               const std::vector<std::pair<int, int>>& directions,
                               FillStats& stats, int depth) {

        if (row < 0 || row >= rows_ || col < 0 || col >= cols_ ||
            visited[row][col] || grid_[row][col] != target_value) {
            return;
        }

        visited[row][col] = true;
        grid_[row][col] = new_value;
        stats.pixels_filled++;
        stats.recursion_depth = std::max(stats.recursion_depth, depth);

        // Update bounds
        stats.bounds_min.first = std::min(stats.bounds_min.first, row);
        stats.bounds_min.second = std::min(stats.bounds_min.second, col);
        stats.bounds_max.first = std::max(stats.bounds_max.first, row);
        stats.bounds_max.second = std::max(stats.bounds_max.second, col);

        // Recursively fill neighbors
        for (const auto& [dr, dc] : directions) {
            floodFillRecursiveStats(row + dr, col + dc, target_value, new_value,
                                  visited, directions, stats, depth + 1);
        }
    }

    void floodFillRecursiveConditional(int row, int col, int new_value,
                                     std::vector<std::vector<bool>>& visited,
                                     std::function<bool(int, int, int)>& condition,
                                     int& pixels_filled) {

        if (row < 0 || row >= rows_ || col < 0 || col >= cols_ ||
            visited[row][col] || !condition(row, col, grid_[row][col])) {
            return;
        }

        visited[row][col] = true;
        int old_value = grid_[row][col];
        grid_[row][col] = new_value;
        pixels_filled++;

        // Recursively fill neighbors
        const auto& directions = getDirections4();
        for (const auto& [dr, dc] : directions) {
            floodFillRecursiveConditional(row + dr, col + dc, new_value, visited,
                                        condition, pixels_filled);
        }
    }

    void previewFloodFillRecursive(int row, int col, int target_value,
                                 std::vector<std::vector<bool>>& visited,
                                 const std::vector<std::pair<int, int>>& directions,
                                 std::vector<std::pair<int, int>>& affected_pixels) {

        if (row < 0 || row >= rows_ || col < 0 || col >= cols_ ||
            visited[row][col] || grid_[row][col] != target_value) {
            return;
        }

        visited[row][col] = true;
        affected_pixels.emplace_back(row, col);

        // Recursively check neighbors
        for (const auto& [dr, dc] : directions) {
            previewFloodFillRecursive(row + dr, col + dc, target_value, visited,
                                    directions, affected_pixels);
        }
    }

public:
    const std::vector<std::vector<int>>& getGrid() const { return grid_; }

    void printGrid(const std::string& title = "Grid") const {
        std::cout << title << " (" << rows_ << "x" << cols_ << "):" << std::endl;
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                std::cout << std::setw(3) << grid_[i][j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};

// Interactive paint application simulation
class PaintApplication {
private:
    RecursiveFloodFill canvas_;
    std::vector<std::vector<int>> undo_stack_;

public:
    PaintApplication(int width, int height)
        : canvas_(std::vector<std::vector<int>>(height, std::vector<int>(width, 0))) {}

    // Bucket fill tool
    void bucketFill(int x, int y, int new_color) {
        saveState();
        canvas_.floodFill4Way(y, x, new_color); // Note: row, col order
    }

    // Bucket fill with tolerance
    void bucketFillWithTolerance(int x, int y, int new_color, int tolerance) {
        saveState();
        AdvancedRecursiveFloodFill advanced(canvas_.getGrid());
        advanced.floodFillWithTolerance(y, x, new_color, tolerance);
        canvas_ = RecursiveFloodFill(advanced.getGrid());
    }

    // Preview fill area
    std::vector<std::pair<int, int>> previewFill(int x, int y) {
        AdvancedRecursiveFloodFill preview(canvas_.getGrid());
        return preview.floodFillWithStats(y, x, -1).pixels_filled > 0 ?
               preview.previewFloodFill(y, x) : std::vector<std::pair<int, int>>{};
    }

    // Undo last operation
    bool undo() {
        if (undo_stack_.empty()) return false;
        canvas_ = RecursiveFloodFill(undo_stack_.back());
        undo_stack_.pop_back();
        return true;
    }

    void displayCanvas() {
        canvas_.printGrid("Paint Canvas");
    }

private:
    void saveState() {
        undo_stack_.push_back(canvas_.getGrid());
        // Limit undo history
        if (undo_stack_.size() > 10) {
            undo_stack_.erase(undo_stack_.begin());
        }
    }
};

// Example usage
int main() {
    std::cout << "Recursive Flood Fill:" << std::endl;

    // Create a test grid (like a simple image)
    std::vector<std::vector<int>> grid = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0, 0, 1, 0},
        {0, 1, 0, 1, 0, 1, 1, 0},
        {0, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 0},
        {0, 1, 1, 1, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    RecursiveFloodFill flood_fill(grid);
    flood_fill.printGrid("Original Grid");

    // Test 4-way flood fill
    flood_fill.floodFill4Way(1, 1, 5); // Fill the '1's region with 5
    flood_fill.printGrid("After 4-way flood fill at (1,1) with value 5");

    // Test 8-way flood fill
    flood_fill.floodFill8Way(2, 5, 7); // Fill another region
    flood_fill.printGrid("After 8-way flood fill at (2,5) with value 7");

    // Test tolerance-based fill
    flood_fill.floodFillWithTolerance(4, 4, 9, 0); // Exact match
    flood_fill.printGrid("After tolerance fill at (4,4) with value 9");

    // Advanced flood fill with statistics
    std::cout << "\nAdvanced Recursive Flood Fill:" << std::endl;

    std::vector<std::vector<int>> test_grid = {
        {1, 1, 1, 0, 0},
        {1, 0, 1, 0, 0},
        {1, 1, 1, 0, 2},
        {0, 0, 0, 2, 2},
        {0, 0, 0, 0, 2}
    };

    AdvancedRecursiveFloodFill advanced_fill(test_grid);
    advanced_fill.printGrid("Test Grid");

    // Fill with statistics
    auto stats = advanced_fill.floodFillWithStats(0, 0, 5, false);
    advanced_fill.printGrid("After filling with stats");

    std::cout << "Fill Statistics:" << std::endl;
    std::cout << "Pixels filled: " << stats.pixels_filled << std::endl;
    std::cout << "Recursion depth: " << stats.recursion_depth << std::endl;
    std::cout << "Bounds: (" << stats.bounds_min.first << "," << stats.bounds_min.second
              << ") to (" << stats.bounds_max.first << "," << stats.bounds_max.second << ")" << std::endl;

    // Conditional fill
    int pixels = advanced_fill.floodFillConditional(4, 4, 8,
        [](int row, int col, int value) { return value == 2; });
    advanced_fill.printGrid("After conditional fill (value == 2)");
    std::cout << "Pixels filled with condition: " << pixels << std::endl;

    // Preview fill
    auto preview_pixels = advanced_fill.previewFloodFill(2, 4);
    std::cout << "Preview fill at (2,4) would affect " << preview_pixels.size() << " pixels:" << std::endl;
    for (const auto& [r, c] : preview_pixels) {
        std::cout << "(" << r << "," << c << ") ";
    }
    std::cout << std::endl;

    // Paint application simulation
    std::cout << "\nPaint Application Simulation:" << std::endl;
    PaintApplication paint(8, 6);

    // Create some shapes
    paint.bucketFill(1, 1, 1);
    paint.bucketFill(5, 1, 2);
    paint.bucketFill(1, 4, 3);
    paint.displayCanvas();

    // Preview a fill
    auto preview = paint.previewFill(3, 2);
    std::cout << "Preview fill at (3,2) would affect " << preview.size() << " pixels" << std::endl;

    // Fill with tolerance
    paint.bucketFillWithTolerance(3, 2, 4, 1);
    paint.displayCanvas();

    std::cout << "\nDemonstrates:" << std::endl;
    std::cout << "- Classic recursive flood fill algorithm" << std::endl;
    std::cout << "- 4-way and 8-way connectivity options" << std::endl;
    std::cout << "- Tolerance-based filling for color ranges" << std::endl;
    std::cout << "- Advanced statistics and bounds tracking" << std::endl;
    std::cout << "- Conditional filling with custom predicates" << std::endl;
    std::cout << "- Preview functionality without modification" << std::endl;
    std::cout << "- Interactive paint application simulation" << std::endl;

    return 0;
}

