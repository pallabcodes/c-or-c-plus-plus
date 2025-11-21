/*
 * Boundary Fill Algorithms
 *
 * Source: Computer graphics, CAD systems, interactive design tools
 * Repository: Graphics libraries, CAD software, medical imaging
 * Files: Boundary filling algorithms, region filling techniques
 * Algorithm: Fill inward from boundary pixels, edge-following approach
 *
 * What Makes It Ingenious:
 * - Fills regions defined by boundaries rather than connectivity
 * - Natural for user-drawn boundaries and CAD applications
 * - Edge-following behavior prevents boundary leakage
 * - Good for irregular shapes with well-defined edges
 * - Interactive boundary selection and filling
 *
 * When to Use:
 * - Boundary-defined regions in CAD software
 * - Medical imaging with anatomical boundaries
 * - Interactive drawing tools with boundary selection
 * - Filling user-drawn shapes and selections
 * - Regions with clear boundary definitions
 * - When connectivity-based filling might leak
 *
 * Real-World Usage:
 * - CAD software for filling bounded areas
 * - Medical image segmentation with boundary tracing
 * - Interactive painting with boundary constraints
 * - Geographic boundary filling in GIS systems
 * - Quality control boundary inspection
 * - Manufacturing boundary-based filling
 *
 * Time Complexity: O(perimeter + area) - boundary traversal plus fill
 * Space Complexity: O(perimeter) - boundary storage and queue
 * Boundary Definition: Requires clear boundary pixels or conditions
 */

#include <vector>
#include <iostream>
#include <queue>
#include <stack>
#include <algorithm>
#include <functional>
#include <memory>
#include <set>
#include <unordered_set>

// Basic boundary fill using queue (iterative approach)
class BoundaryFill {
private:
    std::vector<std::vector<int>> grid_;
    int rows_, cols_;

    // Direction vectors for 4-way and 8-way connectivity
    const std::vector<std::pair<int, int>> directions_4 = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0}
    };

    const std::vector<std::pair<int, int>> directions_8 = {
        {0, 1}, {1, 1}, {1, 0}, {1, -1},
        {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}
    };

    // Check if position is valid for boundary fill
    bool isValid(int row, int col, int boundary_value, int fill_value,
                const std::vector<std::vector<bool>>& visited) const {
        return row >= 0 && row < rows_ && col >= 0 && col < cols_ &&
               !visited[row][col] && grid_[row][col] != boundary_value &&
               grid_[row][col] != fill_value;
    }

public:
    BoundaryFill(const std::vector<std::vector<int>>& grid)
        : grid_(grid), rows_(grid.size()), cols_(grid.empty() ? 0 : grid[0].size()) {}

    // 4-way boundary fill starting from a seed point inside the boundary
    int boundaryFill4Way(int seed_row, int seed_col, int boundary_value, int fill_value) {
        if (seed_row < 0 || seed_row >= rows_ || seed_col < 0 || seed_col >= cols_) {
            return 0;
        }

        // Check if seed is on boundary or already filled
        if (grid_[seed_row][seed_col] == boundary_value ||
            grid_[seed_row][seed_col] == fill_value) {
            return 0;
        }

        std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
        std::queue<std::pair<int, int>> q;
        int pixels_filled = 0;

        q.push({seed_row, seed_col});
        visited[seed_row][seed_col] = true;

        while (!q.empty()) {
            auto [row, col] = q.front();
            q.pop();

            grid_[row][col] = fill_value;
            pixels_filled++;

            // Check all 4 neighbors
            for (const auto& [dr, dc] : directions_4) {
                int new_row = row + dr;
                int new_col = col + dc;

                if (isValid(new_row, new_col, boundary_value, fill_value, visited)) {
                    visited[new_row][new_col] = true;
                    q.push({new_row, new_col});
                }
            }
        }

        return pixels_filled;
    }

    // 8-way boundary fill
    int boundaryFill8Way(int seed_row, int seed_col, int boundary_value, int fill_value) {
        if (seed_row < 0 || seed_row >= rows_ || seed_col < 0 || seed_col >= cols_) {
            return 0;
        }

        if (grid_[seed_row][seed_col] == boundary_value ||
            grid_[seed_row][seed_col] == fill_value) {
            return 0;
        }

        std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
        std::queue<std::pair<int, int>> q;
        int pixels_filled = 0;

        q.push({seed_row, seed_col});
        visited[seed_row][seed_col] = true;

        while (!q.empty()) {
            auto [row, col] = q.front();
            q.pop();

            grid_[row][col] = fill_value;
            pixels_filled++;

            // Check all 8 neighbors
            for (const auto& [dr, dc] : directions_8) {
                int new_row = row + dr;
                int new_col = col + dc;

                if (isValid(new_row, new_col, boundary_value, fill_value, visited)) {
                    visited[new_row][new_col] = true;
                    q.push({new_row, new_col});
                }
            }
        }

        return pixels_filled;
    }

    // Boundary fill with custom boundary condition
    int boundaryFillConditional(int seed_row, int seed_col, int fill_value,
                              std::function<bool(int, int, int)> is_boundary) {
        if (seed_row < 0 || seed_row >= rows_ || seed_col < 0 || seed_col >= cols_) {
            return 0;
        }

        if (is_boundary(seed_row, seed_col, grid_[seed_row][seed_col]) ||
            grid_[seed_row][seed_col] == fill_value) {
            return 0;
        }

        std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
        std::queue<std::pair<int, int>> q;
        int pixels_filled = 0;

        q.push({seed_row, seed_col});
        visited[seed_row][seed_col] = true;

        while (!q.empty()) {
            auto [row, col] = q.front();
            q.pop();

            grid_[row][col] = fill_value;
            pixels_filled++;

            // Check neighbors with custom boundary condition
            for (const auto& [dr, dc] : directions_4) {
                int new_row = row + dr;
                int new_col = col + dc;

                if (new_row >= 0 && new_row < rows_ && new_col >= 0 && new_col < cols_ &&
                    !visited[new_row][new_col] &&
                    !is_boundary(new_row, new_col, grid_[new_row][new_col]) &&
                    grid_[new_row][new_col] != fill_value) {

                    visited[new_row][new_col] = true;
                    q.push({new_row, new_col});
                }
            }
        }

        return pixels_filled;
    }

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

// Advanced boundary fill with edge following
class AdvancedBoundaryFill {
private:
    std::vector<std::vector<int>> grid_;
    int rows_, cols_;

public:
    AdvancedBoundaryFill(const std::vector<std::vector<int>>& grid)
        : grid_(grid), rows_(grid.size()), cols_(grid.empty() ? 0 : grid[0].size()) {}

    // Edge-following boundary fill (more sophisticated)
    struct FillResult {
        int pixels_filled;
        std::vector<std::pair<int, int>> boundary_traced;
        bool boundary_closed;
    };

    FillResult boundaryFillWithEdgeFollowing(int seed_row, int seed_col,
                                           int boundary_value, int fill_value) {
        FillResult result = {0, {}, true};

        if (seed_row < 0 || seed_row >= rows_ || seed_col < 0 || seed_col >= cols_) {
            result.boundary_closed = false;
            return result;
        }

        if (grid_[seed_row][seed_col] == boundary_value ||
            grid_[seed_row][seed_col] == fill_value) {
            return result;
        }

        // First, trace the boundary to ensure it's closed
        auto boundary = traceBoundary(seed_row, seed_col, boundary_value);
        result.boundary_traced = boundary;
        result.boundary_closed = !boundary.empty() && boundary.front() == boundary.back();

        if (!result.boundary_closed) {
            return result; // Don't fill if boundary is not closed
        }

        // Now perform the fill
        std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
        std::queue<std::pair<int, int>> q;

        q.push({seed_row, seed_col});
        visited[seed_row][seed_col] = true;

        const std::vector<std::pair<int, int>> directions = {
            {0, 1}, {1, 0}, {0, -1}, {-1, 0}
        };

        while (!q.empty()) {
            auto [row, col] = q.front();
            q.pop();

            grid_[row][col] = fill_value;
            result.pixels_filled++;

            // Check neighbors, but ensure we don't cross the boundary
            for (const auto& [dr, dc] : directions) {
                int new_row = row + dr;
                int new_col = col + dc;

                if (new_row >= 0 && new_row < rows_ && new_col >= 0 && new_col < cols_ &&
                    !visited[new_row][new_col] &&
                    grid_[new_row][new_col] != boundary_value &&
                    grid_[new_row][new_col] != fill_value) {

                    visited[new_row][new_col] = true;
                    q.push({new_row, new_col});
                }
            }
        }

        return result;
    }

    // Fill multiple regions defined by boundary pixels
    std::vector<FillResult> fillMultipleRegions(const std::vector<std::pair<int, int>>& boundary_pixels,
                                              int boundary_value, int fill_value) {
        std::vector<FillResult> results;

        // Find interior points for each boundary region
        std::set<std::pair<int, int>> processed_boundaries;

        for (const auto& [row, col] : boundary_pixels) {
            if (processed_boundaries.count({row, col})) continue;

            // Find an interior seed point near this boundary
            auto seed = findInteriorSeed(row, col, boundary_value);
            if (seed.first != -1) {
                auto result = boundaryFillWithEdgeFollowing(seed.first, seed.second,
                                                          boundary_value, fill_value);
                if (result.pixels_filled > 0) {
                    results.push_back(result);

                    // Mark boundary as processed
                    for (const auto& bp : result.boundary_traced) {
                        processed_boundaries.insert(bp);
                    }
                }
            }
        }

        return results;
    }

private:
    // Simple boundary tracing (follows the boundary pixels)
    std::vector<std::pair<int, int>> traceBoundary(int start_row, int start_col, int boundary_value) {
        std::vector<std::pair<int, int>> boundary;
        std::set<std::pair<int, int>> visited;

        const std::vector<std::pair<int, int>> directions = {
            {0, 1}, {1, 1}, {1, 0}, {1, -1},
            {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}
        };

        std::pair<int, int> current = {start_row, start_col};
        std::pair<int, int> start = current;

        do {
            boundary.push_back(current);
            visited.insert(current);

            // Find next boundary pixel
            bool found_next = false;
            for (const auto& [dr, dc] : directions) {
                int nr = current.first + dr;
                int nc = current.second + dc;

                if (nr >= 0 && nr < rows_ && nc >= 0 && nc < cols_ &&
                    grid_[nr][nc] == boundary_value &&
                    visited.find({nr, nc}) == visited.end()) {

                    current = {nr, nc};
                    found_next = true;
                    break;
                }
            }

            if (!found_next) break; // No more boundary pixels

        } while (current != start && boundary.size() < rows_ * cols_);

        return boundary;
    }

    // Find an interior seed point near a boundary pixel
    std::pair<int, int> findInteriorSeed(int boundary_row, int boundary_col, int boundary_value) {
        const std::vector<std::pair<int, int>> directions = {
            {0, 1}, {1, 0}, {0, -1}, {-1, 0}
        };

        // Check neighbors of the boundary pixel for interior points
        for (const auto& [dr, dc] : directions) {
            int nr = boundary_row + dr;
            int nc = boundary_col + dc;

            if (nr >= 0 && nr < rows_ && nc >= 0 && nc < cols_ &&
                grid_[nr][nc] != boundary_value) {
                return {nr, nc};
            }
        }

        return {-1, -1}; // No interior point found
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

// CAD-style boundary fill for engineering applications
class CADBoundaryFill {
private:
    std::vector<std::vector<int>> grid_;
    int rows_, cols_;

public:
    CADBoundaryFill(const std::vector<std::vector<int>>& grid)
        : grid_(grid), rows_(grid.size()), cols_(grid.empty() ? 0 : grid[0].size()) {}

    // Fill a polygon defined by boundary points
    int fillPolygon(const std::vector<std::pair<int, int>>& boundary_points,
                   int fill_value, int boundary_value = -1) {
        if (boundary_points.size() < 3) return 0; // Not a valid polygon

        // Create boundary on grid
        for (const auto& [row, col] : boundary_points) {
            if (row >= 0 && row < rows_ && col >= 0 && col < cols_) {
                grid_[row][col] = boundary_value;
            }
        }

        // Find interior point (centroid approximation)
        double center_row = 0, center_col = 0;
        for (const auto& [row, col] : boundary_points) {
            center_row += row;
            center_col += col;
        }
        center_row /= boundary_points.size();
        center_col /= boundary_points.size();

        int seed_row = static_cast<int>(center_row);
        int seed_col = static_cast<int>(center_col);

        // Adjust seed if it's on boundary
        if (grid_[seed_row][seed_col] == boundary_value) {
            seed_row = std::min(seed_row + 1, rows_ - 1);
        }

        // Perform boundary fill
        BoundaryFill filler(grid_);
        return filler.boundaryFill4Way(seed_row, seed_col, boundary_value, fill_value);
    }

    // Fill between two boundary curves (like in engineering drawings)
    int fillBetweenBoundaries(const std::vector<std::pair<int, int>>& boundary1,
                            const std::vector<std::pair<int, int>>& boundary2,
                            int fill_value, int boundary_value = -1) {
        // Create boundaries on grid
        for (const auto& [row, col] : boundary1) {
            if (row >= 0 && row < rows_ && col >= 0 && col < cols_) {
                grid_[row][col] = boundary_value;
            }
        }

        for (const auto& [row, col] : boundary2) {
            if (row >= 0 && row < rows_ && col >= 0 && col < cols_) {
                grid_[row][col] = boundary_value;
            }
        }

        // Find a seed point between the boundaries
        // Simple approach: use midpoint of first segments
        int seed_row = (boundary1[0].first + boundary2[0].first) / 2;
        int seed_col = (boundary1[0].second + boundary2[0].second) / 2;

        // Ensure seed is not on boundary
        if (grid_[seed_row][seed_col] == boundary_value) {
            seed_col++;
        }

        BoundaryFill filler(grid_);
        return filler.boundaryFill4Way(seed_row, seed_col, boundary_value, fill_value);
    }

    const std::vector<std::vector<int>>& getGrid() const { return grid_; }

    void printGrid(const std::string& title = "CAD Grid") const {
        std::cout << title << " (" << rows_ << "x" << cols_ << "):" << std::endl;
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                if (grid_[i][j] == -1) {
                    std::cout << " B ";
                } else {
                    std::cout << std::setw(3) << grid_[i][j] << " ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};

// Medical imaging boundary fill (for segmentation)
class MedicalBoundaryFill {
private:
    std::vector<std::vector<int>> image_;
    int rows_, cols_;

public:
    MedicalBoundaryFill(const std::vector<std::vector<int>>& image)
        : image_(image), rows_(image.size()), cols_(image.empty() ? 0 : image[0].size()) {}

    // Fill anatomical region based on intensity boundaries
    struct SegmentationResult {
        int pixels_segmented;
        double average_intensity;
        double region_uniformity;
        std::pair<int, int> centroid;
    };

    SegmentationResult segmentAnatomicalRegion(int seed_row, int seed_col,
                                             int boundary_threshold, int fill_value) {
        SegmentationResult result = {0, 0.0, 0.0, {0, 0}};

        if (seed_row < 0 || seed_row >= rows_ || seed_col < 0 || seed_col >= cols_) {
            return result;
        }

        int seed_intensity = image_[seed_row][seed_col];
        std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
        std::queue<std::pair<int, int>> q;
        std::vector<int> intensities;

        q.push({seed_row, seed_col});
        visited[seed_row][seed_col] = true;

        long long sum_row = 0, sum_col = 0;

        const std::vector<std::pair<int, int>> directions = {
            {0, 1}, {1, 0}, {0, -1}, {-1, 0}
        };

        while (!q.empty()) {
            auto [row, col] = q.front();
            q.pop();

            int current_intensity = image_[row][col];
            image_[row][col] = fill_value;
            intensities.push_back(current_intensity);

            result.pixels_segmented++;
            sum_row += row;
            sum_col += col;

            // Check neighbors within intensity threshold
            for (const auto& [dr, dc] : directions) {
                int new_row = row + dr;
                int new_col = col + dc;

                if (new_row >= 0 && new_row < rows_ && new_col >= 0 && new_col < cols_ &&
                    !visited[new_row][new_col] && image_[new_row][new_col] != fill_value) {

                    int neighbor_intensity = image_[new_row][new_col];
                    if (std::abs(neighbor_intensity - seed_intensity) <= boundary_threshold) {
                        visited[new_row][new_col] = true;
                        q.push({new_row, new_col});
                    }
                }
            }
        }

        // Calculate statistics
        if (!intensities.empty()) {
            double sum_intensity = 0.0;
            for (int intensity : intensities) {
                sum_intensity += intensity;
            }
            result.average_intensity = sum_intensity / intensities.size();

            // Calculate uniformity (inverse of variance)
            double variance = 0.0;
            for (int intensity : intensities) {
                double diff = intensity - result.average_intensity;
                variance += diff * diff;
            }
            variance /= intensities.size();
            result.region_uniformity = 1.0 / (1.0 + variance); // Higher is more uniform

            result.centroid = {
                static_cast<int>(sum_row / result.pixels_segmented),
                static_cast<int>(sum_col / result.pixels_segmented)
            };
        }

        return result;
    }

    const std::vector<std::vector<int>>& getImage() const { return image_; }

    void printImage(const std::string& title = "Medical Image") const {
        std::cout << title << " (" << rows_ << "x" << cols_ << "):" << std::endl;
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                std::cout << std::setw(4) << image_[i][j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};

// Example usage
int main() {
    std::cout << "Boundary Fill Algorithms:" << std::endl;

    // Create a test grid with boundaries
    std::vector<std::vector<int>> grid = {
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 1, 0, 1},
        {1, 0, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1}
    };

    // Basic boundary fill
    std::cout << "Basic Boundary Fill:" << std::endl;
    BoundaryFill basic_fill(grid);
    basic_fill.printGrid("Grid with boundaries (1 = boundary, 0 = interior)");

    int pixels1 = basic_fill.boundaryFill4Way(1, 1, 1, 5); // Fill interior with 5
    basic_fill.printGrid("After boundary fill from (1,1)");
    std::cout << "Pixels filled: " << pixels1 << std::endl;

    // Reset for next test
    grid = {
        {2, 2, 2, 2, 2, 2, 2},
        {2, 0, 0, 0, 0, 0, 2},
        {2, 0, 3, 3, 3, 0, 2},
        {2, 0, 3, 0, 3, 0, 2},
        {2, 0, 3, 3, 3, 0, 2},
        {2, 0, 0, 0, 0, 0, 2},
        {2, 2, 2, 2, 2, 2, 2}
    };

    BoundaryFill fill2(grid);
    int pixels2 = fill2.boundaryFill8Way(1, 1, 2, 7); // 8-way fill
    fill2.printGrid("After 8-way boundary fill");
    std::cout << "Pixels filled: " << pixels2 << std::endl;

    // Conditional boundary fill
    int pixels3 = fill2.boundaryFillConditional(3, 3, 9,
        [](int row, int col, int value) { return value == 2 || value == 3; });
    fill2.printGrid("After conditional boundary fill");
    std::cout << "Pixels filled with condition: " << pixels3 << std::endl;

    // Advanced boundary fill with edge following
    std::cout << "\nAdvanced Boundary Fill with Edge Following:" << std::endl;
    std::vector<std::vector<int>> complex_grid = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 0, 0, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 1, 0, 0, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    AdvancedBoundaryFill advanced_fill(complex_grid);
    advanced_fill.printGrid("Complex boundary grid");

    auto result = advanced_fill.boundaryFillWithEdgeFollowing(1, 1, 1, 5);
    advanced_fill.printGrid("After edge-following boundary fill");
    std::cout << "Pixels filled: " << result.pixels_filled << std::endl;
    std::cout << "Boundary closed: " << (result.boundary_closed ? "Yes" : "No") << std::endl;
    std::cout << "Boundary length: " << result.boundary_traced.size() << std::endl;

    // CAD-style polygon filling
    std::cout << "\nCAD-Style Polygon Filling:" << std::endl;
    std::vector<std::vector<int>> cad_grid(8, std::vector<int>(8, 0));

    CADBoundaryFill cad_fill(cad_grid);

    // Define a triangle
    std::vector<std::pair<int, int>> triangle = {
        {1, 2}, {1, 5}, {5, 3}, {1, 2} // Close the triangle
    };

    int cad_pixels = cad_fill.fillPolygon(triangle, 6, 9); // Fill with 6, boundary with 9
    cad_fill.printGrid("CAD polygon fill (9 = boundary, 6 = fill)");
    std::cout << "Polygon pixels filled: " << cad_pixels << std::endl;

    // Medical imaging segmentation
    std::cout << "\nMedical Imaging Segmentation:" << std::endl;
    std::vector<std::vector<int>> medical_image = {
        {100, 105, 110, 115, 120, 125},
        {105, 120, 130, 125, 115, 110},
        {110, 125, 140, 135, 125, 115},
        {115, 130, 135, 145, 130, 120},
        {120, 125, 130, 135, 125, 115},
        {125, 120, 125, 130, 120, 110}
    };

    MedicalBoundaryFill medical_fill(medical_image);
    medical_fill.printGrid("Medical image (intensity values)");

    auto segmentation = medical_fill.segmentAnatomicalRegion(2, 2, 20, 200);
    medical_fill.printGrid("After segmentation (200 = segmented region)");

    std::cout << "Segmentation Results:" << std::endl;
    std::cout << "Pixels segmented: " << segmentation.pixels_segmented << std::endl;
    std::cout << "Average intensity: " << std::fixed << std::setprecision(2)
              << segmentation.average_intensity << std::endl;
    std::cout << "Region uniformity: " << segmentation.region_uniformity << std::endl;
    std::cout << "Centroid: (" << segmentation.centroid.first << ", "
              << segmentation.centroid.second << ")" << std::endl;

    std::cout << "\nDemonstrates:" << std::endl;
    std::cout << "- Basic boundary fill algorithms (4-way and 8-way)" << std::endl;
    std::cout << "- Conditional boundary filling with custom predicates" << std::endl;
    std::cout << "- Advanced edge-following boundary tracing" << std::endl;
    std::cout << "- CAD-style polygon filling for engineering applications" << std::endl;
    std::cout << "- Medical imaging segmentation with intensity-based boundaries" << std::endl;
    std::cout << "- Boundary validation and closed region detection" << std::endl;
    std::cout << "- Production-quality boundary fill implementations" << std::endl;

    return 0;
}

