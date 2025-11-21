/*
 * Spiral Matrix Traversal Patterns
 *
 * Source: Computer Graphics, Image Processing, Algorithmic Problems
 * Repository: OpenCV, computer vision libraries, competitive programming
 * Files: Matrix traversal utilities, image morphology, space-filling curves
 * Algorithm: Layer-by-layer boundary traversal with direction changes
 *
 * What Makes It Ingenious:
 * - Space-filling curve properties for cache efficiency
 * - Boundary-first processing for morphological operations
 * - Progressive data access patterns
 * - Memory access locality in spiral order
 * - Used in image processing, computer graphics, and algorithms
 *
 * When to Use:
 * - Image morphology operations (erosion, dilation)
 * - Boundary processing and edge detection
 * - Progressive data transmission
 * - Computer graphics rendering
 * - Algorithmic problems requiring spiral access
 * - Memory-constrained processing
 *
 * Real-World Usage:
 * - OpenCV morphological operations
 * - Image processing pipelines
 * - Computer graphics algorithms
 * - Data compression techniques
 * - Progressive image loading
 * - Matrix printing utilities
 * - Game level generation
 *
 * Time Complexity: O(n*m) for full traversal
 * Space Complexity: O(1) auxiliary space
 * Memory Access: Boundary-first, then inward layers
 */

#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>
#include <deque>
#include <memory>

// Generic matrix class
template<typename T>
class Matrix {
private:
    std::vector<T> data_;
    size_t rows_, cols_;

public:
    Matrix(size_t rows, size_t cols, T init_val = T{})
        : data_(rows * cols, init_val), rows_(rows), cols_(cols) {}

    T& operator()(size_t row, size_t col) {
        return data_[row * cols_ + col];
    }

    const T& operator()(size_t row, size_t col) const {
        return data_[row * cols_ + col];
    }

    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }

    void fill(T value) {
        std::fill(data_.begin(), data_.end(), value);
    }

    void print(const std::string& name = "Matrix") const {
        std::cout << name << " (" << rows_ << "x" << cols_ << "):" << std::endl;
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t j = 0; j < cols_; ++j) {
                std::cout << std::setw(4) << (*this)(i, j) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};

// Spiral traversal utilities
class SpiralTraversal {
public:
    // Direction vectors for spiral movement
    enum Direction { RIGHT = 0, DOWN = 1, LEFT = 2, UP = 3 };
    static const std::vector<std::pair<int, int>> directions;

    // Spiral order traversal (clockwise)
    template<typename T>
    static std::vector<T> spiralOrder(const Matrix<T>& matrix) {
        std::vector<T> result;
        if (matrix.rows() == 0 || matrix.cols() == 0) return result;

        size_t top = 0, bottom = matrix.rows() - 1;
        size_t left = 0, right = matrix.cols() - 1;
        Direction dir = RIGHT;

        while (top <= bottom && left <= right) {
            switch (dir) {
                case RIGHT:
                    for (size_t col = left; col <= right; ++col) {
                        result.push_back(matrix(top, col));
                    }
                    ++top;
                    break;

                case DOWN:
                    for (size_t row = top; row <= bottom; ++row) {
                        result.push_back(matrix(row, right));
                    }
                    --right;
                    break;

                case LEFT:
                    if (top <= bottom) {
                        for (int col = static_cast<int>(right); col >= static_cast<int>(left); --col) {
                            result.push_back(matrix(bottom, col));
                        }
                        --bottom;
                    }
                    break;

                case UP:
                    if (left <= right) {
                        for (int row = static_cast<int>(bottom); row >= static_cast<int>(top); --row) {
                            result.push_back(matrix(row, left));
                        }
                        ++left;
                    }
                    break;
            }

            dir = static_cast<Direction>((dir + 1) % 4);
        }

        return result;
    }

    // Spiral order with coordinates
    static std::vector<std::pair<size_t, size_t>> spiralCoordinates(size_t rows, size_t cols) {
        std::vector<std::pair<size_t, size_t>> coordinates;
        if (rows == 0 || cols == 0) return coordinates;

        size_t top = 0, bottom = rows - 1;
        size_t left = 0, right = cols - 1;
        Direction dir = RIGHT;

        while (top <= bottom && left <= right) {
            switch (dir) {
                case RIGHT:
                    for (size_t col = left; col <= right; ++col) {
                        coordinates.emplace_back(top, col);
                    }
                    ++top;
                    break;

                case DOWN:
                    for (size_t row = top; row <= bottom; ++row) {
                        coordinates.emplace_back(row, right);
                    }
                    --right;
                    break;

                case LEFT:
                    if (top <= bottom) {
                        for (int col = static_cast<int>(right); col >= static_cast<int>(left); --col) {
                            coordinates.emplace_back(bottom, col);
                        }
                        --bottom;
                    }
                    break;

                case UP:
                    if (left <= right) {
                        for (int row = static_cast<int>(bottom); row >= static_cast<int>(top); --row) {
                            coordinates.emplace_back(row, left);
                        }
                        ++left;
                    }
                    break;
            }

            dir = static_cast<Direction>((dir + 1) % 4);
        }

        return coordinates;
    }

    // Anti-clockwise spiral traversal
    template<typename T>
    static std::vector<T> spiralOrderAntiClockwise(const Matrix<T>& matrix) {
        std::vector<T> result;
        if (matrix.rows() == 0 || matrix.cols() == 0) return result;

        size_t top = 0, bottom = matrix.rows() - 1;
        size_t left = 0, right = matrix.cols() - 1;
        Direction dir = DOWN; // Start going down for anti-clockwise

        while (top <= bottom && left <= right) {
            switch (dir) {
                case DOWN:
                    for (size_t row = top; row <= bottom; ++row) {
                        result.push_back(matrix(row, left));
                    }
                    ++left;
                    break;

                case RIGHT:
                    for (size_t col = left; col <= right; ++col) {
                        result.push_back(matrix(bottom, col));
                    }
                    --bottom;
                    break;

                case UP:
                    if (left <= right) {
                        for (int row = static_cast<int>(bottom); row >= static_cast<int>(top); --row) {
                            result.push_back(matrix(row, right));
                        }
                        --right;
                    }
                    break;

                case LEFT:
                    if (top <= bottom) {
                        for (int col = static_cast<int>(right); col >= static_cast<int>(left); --col) {
                            result.push_back(matrix(top, col));
                        }
                        ++top;
                    }
                    break;
            }

            dir = static_cast<Direction>((dir + 3) % 4); // Anti-clockwise direction change
        }

        return result;
    }

    // Spiral fill (fill matrix in spiral order)
    template<typename T>
    static void spiralFill(Matrix<T>& matrix, const std::vector<T>& values) {
        auto coords = spiralCoordinates(matrix.rows(), matrix.cols());
        size_t val_idx = 0;

        for (const auto& [row, col] : coords) {
            if (val_idx < values.size()) {
                matrix(row, col) = values[val_idx++];
            }
        }
    }

    // Layer-by-layer processing (useful for image morphology)
    template<typename T, typename Func>
    static void processLayers(Matrix<T>& matrix, Func processor) {
        if (matrix.rows() == 0 || matrix.cols() == 0) return;

        size_t layers = std::min(matrix.rows(), matrix.cols()) / 2;

        for (size_t layer = 0; layer <= layers; ++layer) {
            size_t top = layer;
            size_t bottom = matrix.rows() - 1 - layer;
            size_t left = layer;
            size_t right = matrix.cols() - 1 - layer;

            // Process current layer
            processor(matrix, top, bottom, left, right, layer);
        }
    }

    // Extract matrix boundary in spiral order
    template<typename T>
    static std::vector<T> extractBoundary(const Matrix<T>& matrix) {
        if (matrix.rows() <= 2 || matrix.cols() <= 2) {
            return spiralOrder(matrix); // Small matrix, return all
        }

        std::vector<T> boundary;
        size_t top = 0, bottom = matrix.rows() - 1;
        size_t left = 0, right = matrix.cols() - 1;

        // Top row
        for (size_t col = left; col <= right; ++col) {
            boundary.push_back(matrix(top, col));
        }

        // Right column (excluding corners)
        for (size_t row = top + 1; row <= bottom; ++row) {
            boundary.push_back(matrix(row, right));
        }

        // Bottom row (excluding corners)
        if (bottom > top) {
            for (int col = static_cast<int>(right) - 1; col >= static_cast<int>(left); --col) {
                boundary.push_back(matrix(bottom, col));
            }
        }

        // Left column (excluding corners)
        if (left < right && bottom > top) {
            for (int row = static_cast<int>(bottom) - 1; row > static_cast<int>(top); --row) {
                boundary.push_back(matrix(row, left));
            }
        }

        return boundary;
    }
};

// Direction vectors definition
const std::vector<std::pair<int, int>> SpiralTraversal::directions = {
    {0, 1},   // RIGHT
    {1, 0},   // DOWN
    {0, -1},  // LEFT
    {-1, 0}   // UP
};

// Morphological operations using spiral traversal
template<typename T>
class MorphologicalOps {
public:
    // Erosion using spiral boundary processing
    static void erode(const Matrix<T>& input, Matrix<T>& output, size_t kernel_size = 3) {
        if (kernel_size % 2 == 0) ++kernel_size; // Ensure odd size
        size_t radius = kernel_size / 2;

        SpiralTraversal::processLayers(output,
            [&](Matrix<T>& out, size_t top, size_t bottom, size_t left, size_t right, size_t layer) {
                // Process each element in current layer
                for (size_t row = top; row <= bottom; ++row) {
                    for (size_t col = left; col <= right; ++col) {
                        T min_val = std::numeric_limits<T>::max();

                        // Find minimum in neighborhood
                        for (int dr = -static_cast<int>(radius); dr <= static_cast<int>(radius); ++dr) {
                            for (int dc = -static_cast<int>(radius); dc <= static_cast<int>(radius); ++dc) {
                                int nr = static_cast<int>(row) + dr;
                                int nc = static_cast<int>(col) + dc;

                                if (nr >= 0 && nr < static_cast<int>(input.rows()) &&
                                    nc >= 0 && nc < static_cast<int>(input.cols())) {
                                    min_val = std::min(min_val, input(nr, nc));
                                }
                            }
                        }

                        out(row, col) = min_val;
                    }
                }
            });
    }

    // Dilation using spiral boundary processing
    static void dilate(const Matrix<T>& input, Matrix<T>& output, size_t kernel_size = 3) {
        if (kernel_size % 2 == 0) ++kernel_size;
        size_t radius = kernel_size / 2;

        SpiralTraversal::processLayers(output,
            [&](Matrix<T>& out, size_t top, size_t bottom, size_t left, size_t right, size_t layer) {
                for (size_t row = top; row <= bottom; ++row) {
                    for (size_t col = left; col <= right; ++col) {
                        T max_val = std::numeric_limits<T>::min();

                        // Find maximum in neighborhood
                        for (int dr = -static_cast<int>(radius); dr <= static_cast<int>(radius); ++dr) {
                            for (int dc = -static_cast<int>(radius); dc <= static_cast<int>(radius); ++dc) {
                                int nr = static_cast<int>(row) + dr;
                                int nc = static_cast<int>(col) + dc;

                                if (nr >= 0 && nr < static_cast<int>(input.rows()) &&
                                    nc >= 0 && nc < static_cast<int>(input.cols())) {
                                    max_val = std::max(max_val, input(nr, nc));
                                }
                            }
                        }

                        out(row, col) = max_val;
                    }
                }
            });
    }

    // Opening: erosion followed by dilation
    static void opening(const Matrix<T>& input, Matrix<T>& output,
                       size_t kernel_size = 3) {
        Matrix<T> temp(input.rows(), input.cols());
        erode(input, temp, kernel_size);
        dilate(temp, output, kernel_size);
    }

    // Closing: dilation followed by erosion
    static void closing(const Matrix<T>& input, Matrix<T>& output,
                       size_t kernel_size = 3) {
        Matrix<T> temp(input.rows(), input.cols());
        dilate(input, temp, kernel_size);
        erode(temp, output, kernel_size);
    }
};

// Progressive data processing (useful for streaming)
template<typename T>
class ProgressiveProcessor {
private:
    Matrix<T> matrix_;
    std::vector<std::pair<size_t, size_t>> spiral_coords_;

public:
    ProgressiveProcessor(size_t rows, size_t cols)
        : matrix_(rows, cols), spiral_coords_(SpiralTraversal::spiralCoordinates(rows, cols)) {}

    // Process data progressively (simulate streaming)
    template<typename Func>
    void processProgressive(const std::vector<T>& data_stream, Func processor) {
        size_t data_idx = 0;
        size_t total_elements = matrix_.rows() * matrix_.cols();

        // Process in spiral order
        for (const auto& [row, col] : spiral_coords_) {
            if (data_idx < data_stream.size()) {
                matrix_(row, col) = data_stream[data_idx];
                processor(matrix_, row, col, data_idx);
                ++data_idx;
            }
        }
    }

    // Get current state of matrix
    const Matrix<T>& getMatrix() const { return matrix_; }

    // Get processing progress (0.0 to 1.0)
    double getProgress(size_t processed_elements) const {
        return static_cast<double>(processed_elements) /
               (matrix_.rows() * matrix_.cols());
    }
};

// Example usage
int main() {
    std::cout << "Spiral Matrix Traversal Patterns:" << std::endl;

    // Create a sample matrix
    Matrix<int> matrix(5, 5);
    int counter = 1;
    for (size_t i = 0; i < matrix.rows(); ++i) {
        for (size_t j = 0; j < matrix.cols(); ++j) {
            matrix(i, j) = counter++;
        }
    }

    matrix.print("Original Matrix");

    // Spiral traversal
    auto spiral = SpiralTraversal::spiralOrder(matrix);
    std::cout << "Spiral order (clockwise): ";
    for (int val : spiral) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    // Anti-clockwise spiral
    auto anti_spiral = SpiralTraversal::spiralOrderAntiClockwise(matrix);
    std::cout << "Spiral order (anti-clockwise): ";
    for (int val : anti_spiral) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    // Boundary extraction
    auto boundary = SpiralTraversal::extractBoundary(matrix);
    std::cout << "Boundary elements: ";
    for (int val : boundary) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    // Spiral fill
    Matrix<int> filled(5, 5);
    std::vector<int> fill_values;
    for (int i = 100; i < 125; ++i) fill_values.push_back(i);
    SpiralTraversal::spiralFill(filled, fill_values);
    filled.print("Spiral Filled Matrix");

    // Morphological operations
    Matrix<int> eroded(5, 5);
    MorphologicalOps<int>::erode(matrix, eroded, 3);
    eroded.print("Eroded Matrix");

    Matrix<int> dilated(5, 5);
    MorphologicalOps<int>::dilate(matrix, dilated, 3);
    dilated.print("Dilated Matrix");

    // Progressive processing example
    std::cout << "\nProgressive Processing:" << std::endl;
    ProgressiveProcessor<double> processor(4, 4);

    std::vector<double> data_stream;
    for (int i = 0; i < 16; ++i) {
        data_stream.push_back(i * 0.5);
    }

    processor.processProgressive(data_stream,
        [](const Matrix<double>& mat, size_t row, size_t col, size_t idx) {
            if (idx % 4 == 0) { // Print every 4 elements
                std::cout << "Processed " << idx + 1 << " elements, progress: "
                          << (idx + 1) * 100.0 / (mat.rows() * mat.cols()) << "%" << std::endl;
            }
        });

    processor.getMatrix().print("Progressively Processed Matrix");

    std::cout << "\nDemonstrates:" << std::endl;
    std::cout << "- Clockwise and anti-clockwise spiral traversal" << std::endl;
    std::cout << "- Boundary extraction and processing" << std::endl;
    std::cout << "- Layer-by-layer morphological operations" << std::endl;
    std::cout << "- Progressive data processing" << std::endl;
    std::cout << "- Space-filling curve properties" << std::endl;
    std::cout << "- Production-grade matrix traversal patterns" << std::endl;

    return 0;
}

