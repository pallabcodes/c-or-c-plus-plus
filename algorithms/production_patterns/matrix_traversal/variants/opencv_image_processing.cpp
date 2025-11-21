/*
 * OpenCV-Style Image Processing Matrix Traversal
 *
 * Source: OpenCV computer vision library
 * Repository: https://github.com/opencv/opencv
 * Files: modules/core/src/matrix.cpp, modules/imgproc/src/*.cpp
 * Algorithm: ROI-based matrix traversal with boundary handling
 *
 * What Makes It Ingenious:
 * - Region of Interest (ROI) processing for efficiency
 * - Boundary handling with padding/mirroring/clamping
 * - Channel-wise operations for color images
 * - SIMD-friendly access patterns
 * - Memory-aligned access for performance
 * - Used in all computer vision and image processing applications
 *
 * When to Use:
 * - Computer vision algorithms
 * - Image filtering and convolution
 * - Pixel manipulation operations
 * - Feature extraction from images
 * - Real-time image processing
 *
 * Real-World Usage:
 * - OpenCV image processing pipeline
 * - Computer vision applications
 * - Photography software
 * - Video processing systems
 * - Medical imaging software
 * - Autonomous vehicle perception
 *
 * Time Complexity: O(width * height * channels)
 * Space Complexity: O(width * height * channels)
 */

#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>
#include <cmath>
#include <memory>

// Image data structure (simplified OpenCV Mat)
template<typename T>
class ImageMatrix {
private:
    std::vector<T> data_;
    int rows_, cols_, channels_;
    int step_; // bytes per row

public:
    ImageMatrix(int rows, int cols, int channels = 1, T init_val = T{})
        : rows_(rows), cols_(cols), channels_(channels), step_(cols * channels) {
        data_.resize(rows * cols * channels, init_val);
    }

    // Access pixel at (row, col, channel)
    T& at(int row, int col, int channel = 0) {
        return data_[row * step_ + col * channels_ + channel];
    }

    const T& at(int row, int col, int channel = 0) const {
        return data_[row * step_ + col * channels_ + channel];
    }

    // Get dimensions
    int rows() const { return rows_; }
    int cols() const { return cols_; }
    int channels() const { return channels_; }
    int total() const { return rows_ * cols_ * channels_; }

    // ROI (Region of Interest)
    struct ROI {
        int x, y, width, height;
        ROI(int x = 0, int y = 0, int w = 0, int h = 0) : x(x), y(y), width(w), height(h) {}
    };

    // Check if point is within image bounds
    bool contains(int row, int col) const {
        return row >= 0 && row < rows_ && col >= 0 && col < cols_;
    }

    // Get raw data pointer
    T* data() { return data_.data(); }
    const T* data() const { return data_.data(); }

    // Copy from another image
    void copyFrom(const ImageMatrix& other) {
        if (rows_ == other.rows_ && cols_ == other.cols_ && channels_ == other.channels_) {
            data_ = other.data_;
        }
    }
};

// Boundary handling strategies (OpenCV-style)
enum BorderType {
    BORDER_CONSTANT,    // Fill with constant value
    BORDER_REPLICATE,   // Replicate edge pixels
    BORDER_REFLECT,     // Reflect over edge
    BORDER_WRAP,        // Wrap around
    BORDER_REFLECT_101  // Reflect with border
};

template<typename T>
class OpenCVImageProcessing {
private:
    // Get pixel value with boundary handling
    T getPixelWithBorder(const ImageMatrix<T>& img, int row, int col, int channel,
                        BorderType border_type, T border_value = T{}) {
        if (img.contains(row, col)) {
            return img.at(row, col, channel);
        }

        switch (border_type) {
            case BORDER_CONSTANT:
                return border_value;
            case BORDER_REPLICATE:
                row = std::max(0, std::min(row, img.rows() - 1));
                col = std::max(0, std::min(col, img.cols() - 1));
                return img.at(row, col, channel);
            case BORDER_REFLECT:
                if (row < 0) row = -row;
                if (row >= img.rows()) row = 2 * img.rows() - row - 2;
                if (col < 0) col = -col;
                if (col >= img.cols()) col = 2 * img.cols() - col - 2;
                row = std::max(0, std::min(row, img.rows() - 1));
                col = std::max(0, std::min(col, img.cols() - 1));
                return img.at(row, col, channel);
            case BORDER_WRAP:
                row = (row % img.rows() + img.rows()) % img.rows();
                col = (col % img.cols() + img.cols()) % img.cols();
                return img.at(row, col, channel);
            case BORDER_REFLECT_101:
                if (row < 0) row = -row - 1;
                if (row >= img.rows()) row = 2 * img.rows() - row - 1;
                if (col < 0) col = -col - 1;
                if (col >= img.cols()) col = 2 * img.cols() - col - 1;
                row = std::max(0, std::min(row, img.rows() - 1));
                col = std::max(0, std::min(col, img.cols() - 1));
                return img.at(row, col, channel);
            default:
                return border_value;
        }
    }

public:
    // Convolution with kernel (OpenCV filter2D equivalent)
    static void convolution(const ImageMatrix<T>& input, ImageMatrix<T>& output,
                           const std::vector<std::vector<T>>& kernel,
                           BorderType border_type = BORDER_REFLECT_101) {

        int kernel_size = kernel.size();
        int anchor_row = kernel_size / 2;
        int anchor_col = kernel_size / 2;

        for (int row = 0; row < input.rows(); ++row) {
            for (int col = 0; col < input.cols(); ++col) {
                for (int ch = 0; ch < input.channels(); ++ch) {
                    T sum = T{};

                    // Apply kernel
                    for (int k_row = 0; k_row < kernel_size; ++k_row) {
                        for (int k_col = 0; k_col < kernel_size; ++k_col) {
                            int src_row = row + k_row - anchor_row;
                            int src_col = col + k_col - anchor_col;

                            T pixel_val = getPixelWithBorder(input, src_row, src_col, ch,
                                                           border_type);
                            sum += pixel_val * kernel[k_row][k_col];
                        }
                    }

                    output.at(row, col, ch) = sum;
                }
            }
        }
    }

    // Gaussian blur (common OpenCV operation)
    static void gaussianBlur(const ImageMatrix<T>& input, ImageMatrix<T>& output,
                            int kernel_size, double sigma) {

        // Generate Gaussian kernel
        std::vector<std::vector<T>> kernel(kernel_size, std::vector<T>(kernel_size));
        int center = kernel_size / 2;
        T sum = T{};

        for (int i = 0; i < kernel_size; ++i) {
            for (int j = 0; j < kernel_size; ++j) {
                int x = i - center;
                int y = j - center;
                T value = static_cast<T>(std::exp(-(x*x + y*y) / (2 * sigma * sigma)));
                kernel[i][j] = value;
                sum += value;
            }
        }

        // Normalize kernel
        for (auto& row : kernel) {
            for (auto& val : row) {
                val /= sum;
            }
        }

        convolution(input, output, kernel);
    }

    // Sobel edge detection (gradient-based)
    static void sobelEdgeDetection(const ImageMatrix<T>& input, ImageMatrix<T>& output) {
        // Sobel X kernel
        std::vector<std::vector<T>> sobel_x = {
            {-1, 0, 1},
            {-2, 0, 2},
            {-1, 0, 1}
        };

        // Sobel Y kernel
        std::vector<std::vector<T>> sobel_y = {
            {-1, -2, -1},
            {0, 0, 0},
            {1, 2, 1}
        };

        ImageMatrix<T> grad_x(input.rows(), input.cols(), input.channels());
        ImageMatrix<T> grad_y(input.rows(), input.cols(), input.channels());

        convolution(input, grad_x, sobel_x);
        convolution(input, grad_y, sobel_y);

        // Combine gradients
        for (int row = 0; row < input.rows(); ++row) {
            for (int col = 0; col < input.cols(); ++col) {
                for (int ch = 0; ch < input.channels(); ++ch) {
                    T gx = grad_x.at(row, col, ch);
                    T gy = grad_y.at(row, col, ch);
                    T magnitude = static_cast<T>(std::sqrt(gx*gx + gy*gy));
                    output.at(row, col, ch) = magnitude;
                }
            }
        }
    }

    // ROI (Region of Interest) processing
    static void processROI(const ImageMatrix<T>& input, ImageMatrix<T>& output,
                          const typename ImageMatrix<T>::ROI& roi,
                          std::function<void(T&, int, int, int)> processor) {

        int start_row = std::max(0, roi.y);
        int end_row = std::min(input.rows(), roi.y + roi.height);
        int start_col = std::max(0, roi.x);
        int end_col = std::min(input.cols(), roi.x + roi.width);

        for (int row = start_row; row < end_row; ++row) {
            for (int col = start_col; col < end_col; ++col) {
                for (int ch = 0; ch < input.channels(); ++ch) {
                    T& pixel = output.at(row, col, ch);
                    processor(pixel, row, col, ch);
                }
            }
        }
    }

    // Threshold operation (OpenCV threshold)
    static void threshold(const ImageMatrix<T>& input, ImageMatrix<T>& output,
                         T thresh, T max_val, int threshold_type) {

        auto threshold_func = [&](T& pixel, int row, int col, int ch) {
            switch (threshold_type) {
                case 0: // THRESH_BINARY
                    pixel = (pixel > thresh) ? max_val : 0;
                    break;
                case 1: // THRESH_BINARY_INV
                    pixel = (pixel > thresh) ? 0 : max_val;
                    break;
                case 2: // THRESH_TRUNC
                    if (pixel > thresh) pixel = thresh;
                    break;
                case 3: // THRESH_TOZERO
                    if (pixel < thresh) pixel = 0;
                    break;
                case 4: // THRESH_TOZERO_INV
                    if (pixel > thresh) pixel = 0;
                    break;
            }
        };

        typename ImageMatrix<T>::ROI full_roi(0, 0, input.cols(), input.rows());
        processROI(input, output, full_roi, threshold_func);
    }

    // Morphological operations (erosion/dilation)
    static void morphologyEx(const ImageMatrix<T>& input, ImageMatrix<T>& output,
                           int operation, const std::vector<std::vector<T>>& kernel) {

        int kernel_size = kernel.size();
        int anchor_row = kernel_size / 2;
        int anchor_col = kernel_size / 2;

        for (int row = 0; row < input.rows(); ++row) {
            for (int col = 0; col < input.cols(); ++col) {
                for (int ch = 0; ch < input.channels(); ++ch) {
                    T result = (operation == 0) ? std::numeric_limits<T>::max() :
                                                   std::numeric_limits<T>::min();

                    // Apply morphological operation
                    for (int k_row = 0; k_row < kernel_size; ++k_row) {
                        for (int k_col = 0; k_col < kernel_size; ++k_col) {
                            if (kernel[k_row][k_col] == 0) continue;

                            int src_row = row + k_row - anchor_row;
                            int src_col = col + k_col - anchor_col;

                            T pixel_val = getPixelWithBorder(input, src_row, src_col, ch,
                                                           BORDER_CONSTANT);

                            if (operation == 0) { // EROSION (min)
                                result = std::min(result, pixel_val);
                            } else { // DILATION (max)
                                result = std::max(result, pixel_val);
                            }
                        }
                    }

                    output.at(row, col, ch) = result;
                }
            }
        }
    }

    // Canny edge detection (simplified)
    static void cannyEdgeDetection(const ImageMatrix<T>& input, ImageMatrix<T>& output,
                                  T low_thresh, T high_thresh) {

        // Step 1: Gaussian blur
        ImageMatrix<T> blurred(input.rows(), input.cols(), input.channels());
        gaussianBlur(input, blurred, 5, 1.4);

        // Step 2: Gradient computation (Sobel)
        ImageMatrix<T> edges(input.rows(), input.cols(), input.channels());
        sobelEdgeDetection(blurred, edges);

        // Step 3: Non-maximum suppression and hysteresis thresholding
        // (Simplified version)
        threshold(edges, output, (low_thresh + high_thresh) / 2, 255, 0);
    }
};

// Specialized for game engine grid traversal
class GameGridTraversal {
private:
    std::vector<std::vector<int>> grid_;
    int rows_, cols_;

    // Direction vectors for 4-way and 8-way movement
    const std::vector<std::pair<int, int>> directions_4 = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0}
    };

    const std::vector<std::pair<int, int>> directions_8 = {
        {0, 1}, {1, 1}, {1, 0}, {1, -1},
        {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}
    };

public:
    GameGridTraversal(int rows, int cols)
        : grid_(rows, std::vector<int>(cols, 0)), rows_(rows), cols_(cols) {}

    // Set obstacle (1 = obstacle, 0 = free)
    void setObstacle(int row, int col, bool obstacle) {
        if (row >= 0 && row < rows_ && col >= 0 && col < cols_) {
            grid_[row][col] = obstacle ? 1 : 0;
        }
    }

    // Check if position is valid and free
    bool isValid(int row, int col) const {
        return row >= 0 && row < rows_ && col >= 0 && col < cols_ && grid_[row][col] == 0;
    }

    // Breadth-first search for pathfinding
    std::vector<std::pair<int, int>> findPath(int start_row, int start_col,
                                             int end_row, int end_col) {
        if (!isValid(start_row, start_col) || !isValid(end_row, end_col)) {
            return {};
        }

        std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
        std::vector<std::vector<std::pair<int, int>>> parent(
            rows_, std::vector<std::pair<int, int>>(cols_, {-1, -1}));

        std::queue<std::pair<int, int>> q;
        q.push({start_row, start_col});
        visited[start_row][start_col] = true;

        bool found = false;
        while (!q.empty() && !found) {
            auto [current_row, current_col] = q.front();
            q.pop();

            // Check all 4 directions
            for (const auto& [dr, dc] : directions_4) {
                int new_row = current_row + dr;
                int new_col = current_col + dc;

                if (isValid(new_row, new_col) && !visited[new_row][new_col]) {
                    visited[new_row][new_col] = true;
                    parent[new_row][new_col] = {current_row, current_col};
                    q.push({new_row, new_col});

                    if (new_row == end_row && new_col == end_col) {
                        found = true;
                        break;
                    }
                }
            }
        }

        // Reconstruct path
        if (!found) return {};

        std::vector<std::pair<int, int>> path;
        auto current = std::make_pair(end_row, end_col);

        while (current.first != -1) {
            path.push_back(current);
            current = parent[current.first][current.second];
        }

        std::reverse(path.begin(), path.end());
        return path;
    }

    // Flood fill algorithm
    void floodFill(int start_row, int start_col, int new_value) {
        if (!isValid(start_row, start_col)) return;

        int old_value = grid_[start_row][start_col];
        if (old_value == new_value) return;

        std::queue<std::pair<int, int>> q;
        q.push({start_row, start_col});
        grid_[start_row][start_col] = new_value;

        while (!q.empty()) {
            auto [row, col] = q.front();
            q.pop();

            // Check all 4 directions
            for (const auto& [dr, dc] : directions_4) {
                int new_row = row + dr;
                int new_col = col + dc;

                if (new_row >= 0 && new_row < rows_ && new_col >= 0 && new_col < cols_ &&
                    grid_[new_row][new_col] == old_value) {
                    grid_[new_row][new_col] = new_value;
                    q.push({new_row, new_col});
                }
            }
        }
    }

    // Print grid
    void print() const {
        for (int row = 0; row < rows_; ++row) {
            for (int col = 0; col < cols_; ++col) {
                std::cout << grid_[row][col] << " ";
            }
            std::cout << std::endl;
        }
    }
};

// Example usage
int main() {
    std::cout << "OpenCV-Style Image Processing and Game Grid Traversal:" << std::endl;

    // Image processing example
    ImageMatrix<float> input(10, 10, 1, 0.5f);
    ImageMatrix<float> output(10, 10, 1);

    // Add some pattern to input
    for (int i = 3; i < 7; ++i) {
        for (int j = 3; j < 7; ++j) {
            input.at(i, j, 0) = 1.0f;
        }
    }

    std::cout << "Applying Gaussian blur..." << std::endl;
    OpenCVImageProcessing<float>::gaussianBlur(input, output, 3, 1.0);

    std::cout << "Applying edge detection..." << std::endl;
    ImageMatrix<float> edges(10, 10, 1);
    OpenCVImageProcessing<float>::sobelEdgeDetection(output, edges);

    // Game grid example
    std::cout << "\nGame Grid Pathfinding:" << std::endl;
    GameGridTraversal grid(8, 8);

    // Add some obstacles
    for (int i = 2; i < 6; ++i) {
        grid.setObstacle(3, i, true);
        grid.setObstacle(i, 3, true);
    }

    std::cout << "Grid with obstacles:" << std::endl;
    grid.print();

    // Find path
    auto path = grid.findPath(0, 0, 7, 7);
    std::cout << "\nPath from (0,0) to (7,7):" << std::endl;
    for (const auto& [row, col] : path) {
        std::cout << "(" << row << "," << col << ") ";
    }
    std::cout << std::endl;

    // Flood fill
    std::cout << "\nFlood fill from (5,5) with value 2:" << std::endl;
    grid.floodFill(5, 5, 2);
    grid.print();

    std::cout << "\nDemonstrates:" << std::endl;
    std::cout << "- OpenCV-style ROI processing with boundary handling" << std::endl;
    std::cout << "- Convolution operations (blur, edge detection)" << std::endl;
    std::cout << "- Game grid traversal for pathfinding" << std::endl;
    std::cout << "- Flood fill algorithms for area processing" << std::endl;
    std::cout << "- Production-grade matrix traversal patterns" << std::endl;

    return 0;
}

