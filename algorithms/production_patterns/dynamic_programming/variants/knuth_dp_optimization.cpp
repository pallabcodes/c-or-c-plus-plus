/*
 * Knuth Optimization for Dynamic Programming
 *
 * Source: Donald Knuth's research on DP optimization
 * Paper: "Optimum binary search trees" and related work
 * Algorithm: Optimizes DP when quadrangle inequality holds
 *
 * What Makes It Ingenious:
 * - Reduces O(n³) DP to O(n²) using cost function properties
 * - Assumes quadrangle inequality: C[a,c] + C[b,d] ≤ C[a,d] + C[b,c] for a≤b≤c≤d
 * - Divides inequality: C[b,c] ≤ C[b,d] + C[a,c] - C[a,d] or similar
 * - Used in matrix chain multiplication, optimal BST, polygon triangulation
 * - Mathematical proof required for correctness
 *
 * When to Use:
 * - When cost function satisfies quadrangle inequality
 * - Matrix chain multiplication
 * - Optimal binary search tree
 * - Polygon triangulation
 * - Context-free grammar parsing
 * - Any DP with the inequality property
 *
 * Real-World Usage:
 * - Compiler optimization (code generation)
 * - Database query optimization
 * - Computational geometry
 * - Natural language processing
 * - Bioinformatics (sequence alignment)
 *
 * Time Complexity: O(n²) instead of O(n³)
 * Space Complexity: O(n²) for DP table
 */

#include <vector>
#include <iostream>
#include <limits>
#include <functional>
#include <cassert>

class KnuthDPOptimization {
public:
    // Matrix Chain Multiplication with Knuth Optimization
    // Cost function: C[i][j] = cost to multiply matrices i to j
    // Satisfies quadrangle inequality for standard matrix multiplication
    static std::vector<std::vector<int>> matrix_chain_knuth(
        const std::vector<int>& dimensions) {

        int n = dimensions.size() - 1; // Number of matrices
        std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));
        std::vector<std::vector<int>> optimal_split(n, std::vector<int>(n, 0));

        // Base cases
        for (int i = 0; i < n; ++i) {
            dp[i][i] = 0;
        }

        // Fill DP table with Knuth optimization
        for (int length = 2; length <= n; ++length) {
            for (int i = 0; i <= n - length; ++i) {
                int j = i + length - 1;

                // Knuth optimization: optimal k is between opt[i][j-1] and opt[i+1][j]
                int k_start = (i < n-1) ? optimal_split[i][j-1] : i;
                int k_end = (j > 0) ? optimal_split[i+1][j] : j-1;
                k_start = std::max(k_start, i);
                k_end = std::min(k_end, j-1);

                // Find optimal split in the restricted range
                dp[i][j] = std::numeric_limits<int>::max();
                for (int k = k_start; k <= k_end; ++k) {
                    int cost = dp[i][k] + dp[k+1][j] +
                              dimensions[i] * dimensions[k+1] * dimensions[j+1];
                    if (cost < dp[i][j]) {
                        dp[i][j] = cost;
                        optimal_split[i][j] = k;
                    }
                }
            }
        }

        return dp;
    }

    // Optimal Binary Search Tree with Knuth Optimization
    // Frequencies satisfy quadrangle inequality properties
    static std::vector<std::vector<int>> optimal_bst_knuth(
        const std::vector<double>& frequencies) {

        int n = frequencies.size();
        std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));
        std::vector<std::vector<int>> optimal_root(n, std::vector<int>(n, 0));
        std::vector<std::vector<double>> subtree_freq(n, std::vector<double>(n, 0.0));

        // Precompute subtree frequencies
        for (int i = 0; i < n; ++i) {
            subtree_freq[i][i] = frequencies[i];
            for (int j = i + 1; j < n; ++j) {
                subtree_freq[i][j] = subtree_freq[i][j-1] + frequencies[j];
            }
        }

        // Fill DP table with Knuth optimization
        for (int length = 1; length < n; ++length) {
            for (int i = 0; i < n - length; ++i) {
                int j = i + length;

                // Knuth optimization: optimal root is between opt[i][j-1] and opt[i+1][j]
                int r_start = (i < n-1 && j-1 >= 0) ? optimal_root[i][j-1] : i;
                int r_end = (i+1 < n && j < n) ? optimal_root[i+1][j] : j;
                r_start = std::max(r_start, i);
                r_end = std::min(r_end, j);

                // Find optimal root in the restricted range
                dp[i][j] = std::numeric_limits<int>::max();
                for (int r = r_start; r <= r_end; ++r) {
                    int left_cost = (r > i) ? dp[i][r-1] : 0;
                    int right_cost = (r < j) ? dp[r+1][j] : 0;
                    int total_cost = left_cost + right_cost +
                                   static_cast<int>(subtree_freq[i][j]);

                    if (total_cost < dp[i][j]) {
                        dp[i][j] = total_cost;
                        optimal_root[i][j] = r;
                    }
                }
            }
        }

        return dp;
    }

    // Polygon Triangulation with Knuth Optimization
    // Triangulation cost satisfies quadrangle inequality
    static std::vector<std::vector<int>> polygon_triangulation_knuth(
        const std::vector<int>& vertices) {

        int n = vertices.size() - 1; // Number of sides (assuming closed polygon)
        std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));
        std::vector<std::vector<int>> optimal_split(n, std::vector<int>(n, 0));

        // Cost function for triangle (i,j,k)
        auto triangle_cost = [&](int i, int j, int k) {
            return vertices[i] * vertices[j] * vertices[k];
        };

        // Fill DP table with Knuth optimization
        for (int length = 3; length <= n; ++length) {
            for (int i = 0; i <= n - length; ++i) {
                int j = i + length - 1;

                // Knuth optimization for triangulation
                int k_start = (i < n-1) ? optimal_split[i][j-1] : i+1;
                int k_end = (j > 0) ? optimal_split[i+1][j] : j-1;
                k_start = std::max(k_start, i+1);
                k_end = std::min(k_end, j-1);

                dp[i][j] = std::numeric_limits<int>::max();
                for (int k = k_start; k <= k_end; ++k) {
                    int cost = dp[i][k] + dp[k][j] + triangle_cost(i, k, j);
                    if (cost < dp[i][j]) {
                        dp[i][j] = cost;
                        optimal_split[i][j] = k;
                    }
                }
            }
        }

        return dp;
    }

    // Generic Knuth Optimization Framework
    // Requires: cost_function satisfies quadrangle inequality
    static std::vector<std::vector<int>> knuth_optimize(
        int n,
        const std::function<int(int, int, int)>& cost_function,
        const std::function<int(int, int)>& base_case = nullptr) {

        std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));
        std::vector<std::vector<int>> optimal_k(n, std::vector<int>(n, 0));

        // Base cases
        if (base_case) {
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    if (i == j) {
                        dp[i][j] = base_case(i, j);
                    }
                }
            }
        }

        // Fill DP table using Knuth optimization
        for (int length = 2; length <= n; ++length) {
            for (int i = 0; i <= n - length; ++i) {
                int j = i + length - 1;

                // Knuth optimization: restrict search range
                int k_start = i;
                int k_end = j - 1;

                // If we have previous optimal splits, use them
                if (length > 2) {
                    k_start = std::max(k_start, optimal_k[i][j-1]);
                    k_end = std::min(k_end, optimal_k[i+1][j]);
                }

                dp[i][j] = std::numeric_limits<int>::max();
                for (int k = k_start; k <= k_end; ++k) {
                    int left_cost = (k > i) ? dp[i][k] : 0;
                    int right_cost = (k + 1 < j) ? dp[k+1][j] : 0;
                    int split_cost = cost_function(i, j, k);
                    int total_cost = left_cost + right_cost + split_cost;

                    if (total_cost < dp[i][j]) {
                        dp[i][j] = total_cost;
                        optimal_k[i][j] = k;
                    }
                }
            }
        }

        return dp;
    }

public:
    // Demonstrate Knuth optimization
    static void demonstrate() {
        std::cout << "Knuth DP Optimization Demonstration:" << std::endl;

        // Matrix Chain Multiplication
        std::vector<int> dimensions = {10, 20, 30, 40, 50};
        auto dp = matrix_chain_knuth(dimensions);

        std::cout << "\nMatrix Chain Multiplication (Knuth optimized):" << std::endl;
        for (const auto& row : dp) {
            for (int val : row) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }

        // Optimal BST
        std::vector<double> frequencies = {0.1, 0.2, 0.4, 0.3};
        auto bst_dp = optimal_bst_knuth(frequencies);

        std::cout << "\nOptimal BST (Knuth optimized):" << std::endl;
        for (const auto& row : bst_dp) {
            for (int val : row) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }

        // Polygon Triangulation
        std::vector<int> vertices = {3, 4, 5, 6, 7}; // Pentagon
        auto tri_dp = polygon_triangulation_knuth(vertices);

        std::cout << "\nPolygon Triangulation (Knuth optimized):" << std::endl;
        for (const auto& row : tri_dp) {
            for (int val : row) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
    }
};

// Example usage
int main() {
    KnuthDPOptimization::demonstrate();
    return 0;
}

