/*
 * Diagonal Matrix Traversal Patterns
 *
 * Source: Dynamic Programming, Linear Algebra, Graph Algorithms
 * Repository: NumPy, Eigen, LAPACK, algorithm libraries
 * Files: Matrix diagonalization, DP table traversal, adjacency processing
 * Algorithm: Anti-diagonal traversal with constant i+j sum
 *
 * What Makes It Ingenious:
 * - Constant anti-diagonal access (i+j = constant)
 * - Natural for dynamic programming dependencies
 * - Memory access predictability
 * - Cache-friendly diagonal processing
 * - Used in matrix algorithms and DP tables
 *
 * When to Use:
 * - Dynamic programming table processing
 * - Matrix diagonalization algorithms
 * - Graph algorithms on adjacency matrices
 * - Linear algebra operations
 * - Anti-diagonal computations
 * - Certain optimization problems
 *
 * Real-World Usage:
 * - Dynamic programming (knapsack, edit distance)
 * - Linear algebra libraries (Eigen, LAPACK)
 * - Graph algorithms (shortest paths)
 * - Matrix decomposition methods
 * - Computer graphics algorithms
 * - Scientific computing applications
 *
 * Time Complexity: O(n*m) for full traversal
 * Space Complexity: O(1) auxiliary space
 * Memory Access: Anti-diagonal order (i+j constant)
 */

#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>
#include <deque>
#include <memory>
#include <map>

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

// Diagonal traversal utilities
class DiagonalTraversal {
public:
    // Main diagonal traversal (top-left to bottom-right)
    template<typename T>
    static std::vector<T> mainDiagonalOrder(const Matrix<T>& matrix) {
        std::vector<T> result;
        size_t rows = matrix.rows();
        size_t cols = matrix.cols();

        for (size_t i = 0; i < rows; ++i) {
            result.push_back(matrix(i, i));
        }

        return result;
    }

    // Anti-diagonal traversal (top-right to bottom-left)
    template<typename T>
    static std::vector<T> antiDiagonalOrder(const Matrix<T>& matrix) {
        std::vector<T> result;
        size_t rows = matrix.rows();
        size_t cols = matrix.cols();

        for (size_t i = 0; i < rows; ++i) {
            result.push_back(matrix(i, cols - 1 - i));
        }

        return result;
    }

    // Traverse all elements in anti-diagonal order (constant i+j)
    template<typename T>
    static std::vector<T> antiDiagonalTraversal(const Matrix<T>& matrix) {
        std::vector<T> result;
        size_t rows = matrix.rows();
        size_t cols = matrix.cols();

        // For each anti-diagonal (i+j = constant)
        for (size_t sum = 0; sum < rows + cols - 1; ++sum) {
            // Start from top-right of diagonal
            for (size_t i = 0; i < rows; ++i) {
                size_t j = sum - i;
                if (j < cols && i < rows) {
                    result.push_back(matrix(i, j));
                }
            }
        }

        return result;
    }

    // Traverse all elements in anti-diagonal order with coordinates
    static std::vector<std::pair<size_t, size_t>> antiDiagonalCoordinates(size_t rows, size_t cols) {
        std::vector<std::pair<size_t, size_t>> coordinates;

        // For each anti-diagonal (i+j = constant)
        for (size_t sum = 0; sum < rows + cols - 1; ++sum) {
            // Traverse diagonal from top to bottom
            for (size_t i = 0; i < rows; ++i) {
                size_t j = sum - i;
                if (j < cols) {
                    coordinates.emplace_back(i, j);
                }
            }
        }

        return coordinates;
    }

    // Traverse all elements in main diagonal order (constant i-j)
    template<typename T>
    static std::vector<T> diagonalTraversal(const Matrix<T>& matrix) {
        std::vector<T> result;
        size_t rows = matrix.rows();
        size_t cols = matrix.cols();

        // For each diagonal (i-j = constant)
        size_t max_diff = rows - 1;
        size_t min_diff = -(cols - 1);

        for (int diff = max_diff; diff >= static_cast<int>(min_diff); --diff) {
            // Traverse diagonal
            for (size_t i = 0; i < rows; ++i) {
                int j = static_cast<int>(i) - diff;
                if (j >= 0 && j < static_cast<int>(cols)) {
                    result.push_back(matrix(i, j));
                }
            }
        }

        return result;
    }

    // Process elements by anti-diagonal layers (for DP)
    template<typename T, typename Func>
    static void processByAntiDiagonals(Matrix<T>& matrix, Func processor) {
        size_t rows = matrix.rows();
        size_t cols = matrix.cols();

        // For each anti-diagonal
        for (size_t sum = 0; sum < rows + cols - 1; ++sum) {
            std::vector<std::pair<size_t, size_t>> diagonal_elements;

            // Collect elements in current anti-diagonal
            for (size_t i = 0; i < rows; ++i) {
                size_t j = sum - i;
                if (j < cols) {
                    diagonal_elements.emplace_back(i, j);
                }
            }

            // Process the diagonal
            processor(matrix, diagonal_elements, sum);
        }
    }

    // Upper triangular traversal (above main diagonal)
    template<typename T>
    static std::vector<T> upperTriangular(const Matrix<T>& matrix) {
        std::vector<T> result;
        size_t rows = matrix.rows();
        size_t cols = matrix.cols();

        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = i + 1; j < cols; ++j) {
                result.push_back(matrix(i, j));
            }
        }

        return result;
    }

    // Lower triangular traversal (below main diagonal)
    template<typename T>
    static std::vector<T> lowerTriangular(const Matrix<T>& matrix) {
        std::vector<T> result;
        size_t rows = matrix.rows();
        size_t cols = matrix.cols();

        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < i && j < cols; ++j) {
                result.push_back(matrix(i, j));
            }
        }

        return result;
    }

    // Extract k-th diagonal
    template<typename T>
    static std::vector<T> getKthDiagonal(const Matrix<T>& matrix, int k) {
        std::vector<T> result;
        size_t rows = matrix.rows();
        size_t cols = matrix.cols();

        if (k >= 0) {
            // Above main diagonal
            size_t start_row = 0;
            size_t start_col = k;
            while (start_row < rows && start_col < cols) {
                result.push_back(matrix(start_row, start_col));
                ++start_row;
                ++start_col;
            }
        } else {
            // Below main diagonal
            size_t start_row = -k;
            size_t start_col = 0;
            while (start_row < rows && start_col < cols) {
                result.push_back(matrix(start_row, start_col));
                ++start_row;
                ++start_col;
            }
        }

        return result;
    }
};

// Dynamic Programming utilities using diagonal traversal
template<typename T>
class DPTableProcessor {
public:
    // Process DP table in dependency order (anti-diagonal)
    template<typename Func>
    static void processDPTable(Matrix<T>& dp_table, Func compute_cell) {
        DiagonalTraversal::processByAntiDiagonals(dp_table,
            [&](Matrix<T>& table, const std::vector<std::pair<size_t, size_t>>& diagonal, size_t sum) {
                // Compute cells in current anti-diagonal
                for (const auto& [i, j] : diagonal) {
                    compute_cell(table, i, j);
                }
            });
    }

    // Example: Edit distance computation
    static Matrix<int> editDistance(const std::string& str1, const std::string& str2) {
        size_t m = str1.length();
        size_t n = str2.length();
        Matrix<int> dp(m + 1, n + 1, 0);

        // Initialize base cases
        for (size_t i = 0; i <= m; ++i) dp(i, 0) = i;
        for (size_t j = 0; j <= n; ++j) dp(0, j) = j;

        // Fill DP table using anti-diagonal traversal
        DiagonalTraversal::processByAntiDiagonals(dp,
            [&](Matrix<int>& table, const std::vector<std::pair<size_t, size_t>>& diagonal, size_t sum) {
                for (const auto& [i, j] : diagonal) {
                    if (i > 0 && j > 0) {
                        int cost = (str1[i-1] == str2[j-1]) ? 0 : 1;
                        table(i, j) = std::min({
                            table(i-1, j) + 1,      // deletion
                            table(i, j-1) + 1,      // insertion
                            table(i-1, j-1) + cost  // substitution
                        });
                    }
                }
            });

        return dp;
    }

    // Example: Longest Common Subsequence
    static Matrix<int> longestCommonSubsequence(const std::string& str1, const std::string& str2) {
        size_t m = str1.length();
        size_t n = str2.length();
        Matrix<int> dp(m + 1, n + 1, 0);

        DiagonalTraversal::processByAntiDiagonals(dp,
            [&](Matrix<int>& table, const std::vector<std::pair<size_t, size_t>>& diagonal, size_t sum) {
                for (const auto& [i, j] : diagonal) {
                    if (i > 0 && j > 0) {
                        if (str1[i-1] == str2[j-1]) {
                            table(i, j) = table(i-1, j-1) + 1;
                        } else {
                            table(i, j) = std::max(table(i-1, j), table(i, j-1));
                        }
                    }
                }
            });

        return dp;
    }
};

// Linear algebra operations using diagonal traversal
template<typename T>
class LinearAlgebraOps {
public:
    // Matrix diagonalization (extract diagonal elements)
    static std::vector<T> extractDiagonal(const Matrix<T>& matrix) {
        return DiagonalTraversal::mainDiagonalOrder(matrix);
    }

    // Compute trace (sum of diagonal elements)
    static T trace(const Matrix<T>& matrix) {
        if (matrix.rows() != matrix.cols()) {
            throw std::invalid_argument("Matrix must be square for trace");
        }

        T sum = T{};
        auto diagonal = extractDiagonal(matrix);
        for (const T& val : diagonal) {
            sum += val;
        }
        return sum;
    }

    // Check if matrix is upper triangular
    static bool isUpperTriangular(const Matrix<T>& matrix, T tolerance = T{}) {
        size_t rows = matrix.rows();
        size_t cols = matrix.cols();

        for (size_t i = 1; i < rows; ++i) {
            for (size_t j = 0; j < i && j < cols; ++j) {
                if (std::abs(matrix(i, j)) > tolerance) {
                    return false;
                }
            }
        }
        return true;
    }

    // Check if matrix is lower triangular
    static bool isLowerTriangular(const Matrix<T>& matrix, T tolerance = T{}) {
        size_t rows = matrix.rows();
        size_t cols = matrix.cols();

        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = i + 1; j < cols; ++j) {
                if (std::abs(matrix(i, j)) > tolerance) {
                    return false;
                }
            }
        }
        return true;
    }

    // Extract upper triangular part
    static Matrix<T> upperTriangularPart(const Matrix<T>& matrix) {
        size_t n = std::min(matrix.rows(), matrix.cols());
        Matrix<T> result(n, n);

        for (size_t i = 0; i < n; ++i) {
            for (size_t j = i; j < n; ++j) {
                result(i, j) = matrix(i, j);
            }
        }

        return result;
    }

    // Extract lower triangular part
    static Matrix<T> lowerTriangularPart(const Matrix<T>& matrix) {
        size_t n = std::min(matrix.rows(), matrix.cols());
        Matrix<T> result(n, n);

        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j <= i && j < n; ++j) {
                result(i, j) = matrix(i, j);
            }
        }

        return result;
    }
};

// Graph algorithms using diagonal traversal
template<typename T>
class GraphAlgorithms {
public:
    // Floyd-Warshall algorithm using diagonal traversal
    static void floydWarshall(Matrix<T>& dist_matrix) {
        size_t n = dist_matrix.rows();
        if (n != dist_matrix.cols()) {
            throw std::invalid_argument("Distance matrix must be square");
        }

        // For each intermediate vertex k
        for (size_t k = 0; k < n; ++k) {
            // Update all pairs using anti-diagonal processing
            DiagonalTraversal::processByAntiDiagonals(dist_matrix,
                [&](Matrix<T>& matrix, const std::vector<std::pair<size_t, size_t>>& diagonal, size_t sum) {
                    for (const auto& [i, j] : diagonal) {
                        if (i < n && j < n) {
                            T through_k = matrix(i, k) + matrix(k, j);
                            if (through_k < matrix(i, j)) {
                                matrix(i, j) = through_k;
                            }
                        }
                    }
                });
        }
    }

    // Transitive closure using diagonal traversal
    static Matrix<bool> transitiveClosure(const Matrix<bool>& adj_matrix) {
        size_t n = adj_matrix.rows();
        Matrix<bool> closure = adj_matrix; // Copy

        // Similar to Floyd-Warshall but for reachability
        for (size_t k = 0; k < n; ++k) {
            DiagonalTraversal::processByAntiDiagonals(closure,
                [&](Matrix<bool>& matrix, const std::vector<std::pair<size_t, size_t>>& diagonal, size_t sum) {
                    for (const auto& [i, j] : diagonal) {
                        if (i < n && j < n) {
                            matrix(i, j) = matrix(i, j) || (matrix(i, k) && matrix(k, j));
                        }
                    }
                });
        }

        return closure;
    }
};

// Example usage
int main() {
    std::cout << "Diagonal Matrix Traversal Patterns:" << std::endl;

    // Create a sample matrix
    Matrix<int> matrix(5, 5);
    int counter = 1;
    for (size_t i = 0; i < matrix.rows(); ++i) {
        for (size_t j = 0; j < matrix.cols(); ++j) {
            matrix(i, j) = counter++;
        }
    }

    matrix.print("Sample Matrix");

    // Diagonal traversals
    auto main_diag = DiagonalTraversal::mainDiagonalOrder(matrix);
    std::cout << "Main diagonal: ";
    for (int val : main_diag) std::cout << val << " ";
    std::cout << std::endl;

    auto anti_diag = DiagonalTraversal::antiDiagonalOrder(matrix);
    std::cout << "Anti-diagonal: ";
    for (int val : anti_diag) std::cout << val << " ";
    std::cout << std::endl;

    auto anti_diagonal_trav = DiagonalTraversal::antiDiagonalTraversal(matrix);
    std::cout << "Anti-diagonal traversal: ";
    for (size_t i = 0; i < anti_diagonal_trav.size(); ++i) {
        std::cout << anti_diagonal_trav[i];
        if ((i + 1) % 5 == 0) std::cout << " | ";
        else std::cout << " ";
    }
    std::cout << std::endl;

    // Triangular parts
    auto upper = DiagonalTraversal::upperTriangular(matrix);
    std::cout << "Upper triangular: ";
    for (int val : upper) std::cout << val << " ";
    std::cout << std::endl;

    auto lower = DiagonalTraversal::lowerTriangular(matrix);
    std::cout << "Lower triangular: ";
    for (int val : lower) std::cout << val << " ";
    std::cout << std::endl;

    // Dynamic Programming examples
    std::cout << "\nDynamic Programming Examples:" << std::endl;

    std::string str1 = "kitten";
    std::string str2 = "sitting";
    auto edit_dp = DPTableProcessor<int>::editDistance(str1, str2);
    edit_dp.print("Edit Distance DP Table");
    std::cout << "Edit distance: " << edit_dp(str1.length(), str2.length()) << std::endl;

    auto lcs_dp = DPTableProcessor<int>::longestCommonSubsequence("ABCBDAB", "BDCABA");
    lcs_dp.print("LCS DP Table");
    std::cout << "LCS length: " << lcs_dp(7, 6) << std::endl;

    // Linear Algebra examples
    std::cout << "\nLinear Algebra Examples:" << std::endl;
    Matrix<double> square_matrix(3, 3);
    square_matrix(0, 0) = 1; square_matrix(0, 1) = 2; square_matrix(0, 2) = 3;
    square_matrix(1, 0) = 4; square_matrix(1, 1) = 5; square_matrix(1, 2) = 6;
    square_matrix(2, 0) = 7; square_matrix(2, 1) = 8; square_matrix(2, 2) = 9;

    square_matrix.print("Square Matrix");
    std::cout << "Trace: " << LinearAlgebraOps<double>::trace(square_matrix) << std::endl;
    std::cout << "Is upper triangular: " << LinearAlgebraOps<double>::isUpperTriangular(square_matrix) << std::endl;
    std::cout << "Is lower triangular: " << LinearAlgebraOps<double>::isLowerTriangular(square_matrix) << std::endl;

    // Graph algorithms
    std::cout << "\nGraph Algorithm Example (Floyd-Warshall):" << std::endl;
    Matrix<int> dist(4, 4, 999); // Large number represents infinity
    // Initialize distances
    for (size_t i = 0; i < 4; ++i) dist(i, i) = 0;
    dist(0, 1) = 3; dist(0, 3) = 7;
    dist(1, 0) = 8; dist(1, 2) = 2;
    dist(2, 3) = 1; dist(2, 1) = 5;
    dist(3, 0) = 2;

    dist.print("Initial Distance Matrix");
    GraphAlgorithms<int>::floydWarshall(dist);
    dist.print("After Floyd-Warshall");

    std::cout << "\nDemonstrates:" << std::endl;
    std::cout << "- Anti-diagonal traversal patterns" << std::endl;
    std::cout << "- Dynamic programming table processing" << std::endl;
    std::cout << "- Linear algebra operations" << std::endl;
    std::cout << "- Graph algorithms (Floyd-Warshall)" << std::endl;
    std::cout << "- Triangular matrix operations" << std::endl;
    std::cout << "- Production-grade matrix traversal patterns" << std::endl;

    return 0;
}

