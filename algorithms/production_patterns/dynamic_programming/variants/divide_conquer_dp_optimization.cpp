/*
 * Divide and Conquer Optimization for Dynamic Programming
 *
 * Source: Research papers on DP optimization, matrix chain multiplication
 * Algorithm: Optimizes DP when optimal k increases with i
 * Paper: "Optimizing Dynamic Programming" and related research
 *
 * What Makes It Ingenious:
 * - Reduces O(n³) DP to O(n² log n) using divide and conquer
 * - Assumes monotonicity: opt[i][j] ≤ opt[i+1][j+1]
 * - Recursively computes optimal values in ranges
 * - Used in matrix chain multiplication, optimal BST
 * - Combines divide and conquer with DP state computation
 *
 * When to Use:
 * - DP where optimal split point k increases with i
 * - Matrix chain multiplication
 * - Optimal binary search tree
 * - Range DP problems with monotonic optima
 * - When you can prove monotonicity of optimal choices
 *
 * Real-World Usage:
 * - Compiler optimization (code generation)
 * - Database query optimization
 * - Computational biology (sequence alignment)
 * - Resource allocation problems
 * - Matrix operations optimization
 *
 * Time Complexity: O(n² log n) instead of O(n³)
 * Space Complexity: O(n²) for DP table
 */

#include <vector>
#include <iostream>
#include <limits>
#include <functional>

class DivideConquerDPOptimization {
public:
    // Matrix Chain Multiplication with Divide and Conquer Optimization
    static std::vector<std::vector<int>> matrix_chain_multiplication(
        const std::vector<int>& dimensions) {

        int n = dimensions.size() - 1; // Number of matrices
        std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));
        std::vector<std::vector<int>> optimal_k(n, std::vector<int>(n, 0));

        // Fill DP table using divide and conquer optimization
        for (int length = 2; length <= n; ++length) {
            for (int i = 0; i <= n - length; ++i) {
                int j = i + length - 1;

                // Use divide and conquer to find optimal k
                dp[i][j] = compute_optimal_split(dimensions, dp, i, j, optimal_k[i][j]);
            }
        }

        return dp;
    }

private:
    // Compute optimal split for range [i,j] with divide and conquer
    static int compute_optimal_split(const std::vector<int>& dims,
                                   const std::vector<std::vector<int>>& dp,
                                   int i, int j, int& optimal_k) {

        int min_cost = std::numeric_limits<int>::max();
        int low = i;
        int high = j - 1;

        // Binary search for optimal k
        while (low <= high) {
            int mid = low + (high - low) / 2;

            // Compute cost for splitting at mid
            int cost_left = (mid > i) ? dp[i][mid] : 0;
            int cost_right = (mid + 1 < j) ? dp[mid + 1][j] : 0;
            int cost_mult = dims[i] * dims[mid + 1] * dims[j + 1];
            int total_cost = cost_left + cost_right + cost_mult;

            if (total_cost < min_cost) {
                min_cost = total_cost;
                optimal_k = mid;
            }

            // Since optimal k is monotonic, we can prune the search
            // If left cost is better, search left half
            // If right cost is better, search right half
            int left_cost = (mid - 1 >= i) ?
                dp[i][mid - 1] + dims[i] * dims[mid] * dims[j + 1] : std::numeric_limits<int>::max();
            int right_cost = (mid + 1 <= j - 1) ?
                dp[mid + 1][j] + dims[i] * dims[mid + 2] * dims[j + 1] : std::numeric_limits<int>::max();

            if (left_cost < right_cost) {
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }

        return min_cost;
    }

public:
    // Optimal Binary Search Tree with Divide and Conquer Optimization
    static std::vector<std::vector<int>> optimal_bst(
        const std::vector<double>& frequencies) {

        int n = frequencies.size();
        std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));
        std::vector<std::vector<double>> cost(n, std::vector<double>(n, 0.0));
        std::vector<std::vector<int>> root(n, std::vector<int>(n, 0));

        // Precompute frequencies for subtrees
        for (int i = 0; i < n; ++i) {
            cost[i][i] = frequencies[i];
        }

        for (int length = 1; length < n; ++length) {
            for (int i = 0; i < n - length; ++i) {
                int j = i + length;

                // Compute cost of subtree i to j
                double subtree_freq = 0.0;
                for (int k = i; k <= j; ++k) {
                    subtree_freq += frequencies[k];
                }

                // Use divide and conquer to find optimal root
                dp[i][j] = compute_optimal_bst_root(cost, dp, i, j, subtree_freq, root[i][j]);
                cost[i][j] = dp[i][j] + subtree_freq;
            }
        }

        return dp;
    }

private:
    static int compute_optimal_bst_root(const std::vector<std::vector<double>>& cost,
                                       std::vector<std::vector<int>>& dp,
                                       int i, int j, double subtree_freq,
                                       int& optimal_root) {

        int min_cost = std::numeric_limits<int>::max();
        int low = i;
        int high = j;

        // Binary search for optimal root
        while (low <= high) {
            int mid = low + (high - low) / 2;

            // Cost = cost of left subtree + cost of right subtree + subtree frequency
            int left_cost = (mid > i) ? dp[i][mid - 1] : 0;
            int right_cost = (mid < j) ? dp[mid + 1][j] : 0;
            int total_cost = left_cost + right_cost + static_cast<int>(subtree_freq);

            if (total_cost < min_cost) {
                min_cost = total_cost;
                optimal_root = mid;
            }

            // Monotonicity assumption: optimal root increases with range
            // Compare with neighbors to decide search direction
            int left_neighbor = (mid - 1 >= i) ?
                dp[i][mid - 2] + dp[mid][j] + static_cast<int>(subtree_freq) : std::numeric_limits<int>::max();
            int right_neighbor = (mid + 1 <= j) ?
                dp[i][mid] + dp[mid + 2][j] + static_cast<int>(subtree_freq) : std::numeric_limits<int>::max();

            if (left_neighbor < right_neighbor) {
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }

        return min_cost;
    }

public:
    // Generic Divide and Conquer DP Optimization
    // Assumes: dp[i][j] = min over k in [left[i], right[j]] of (dp[i][k] + dp[k+1][j] + cost(i,j,k))
    // And optimal k is monotonic: opt[i][j] <= opt[i+1][j+1]
    static std::vector<std::vector<int>> optimize_dp(
        int n,
        const std::function<int(int, int, int)>& cost_function) {

        std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));

        for (int length = 2; length <= n; ++length) {
            for (int i = 0; i <= n - length; ++i) {
                int j = i + length - 1;

                // Use divide and conquer to find optimal split
                int optimal_k = 0;
                dp[i][j] = find_optimal_split(dp, cost_function, i, j, optimal_k);
            }
        }

        return dp;
    }

private:
    static int find_optimal_split(std::vector<std::vector<int>>& dp,
                                const std::function<int(int, int, int)>& cost,
                                int i, int j, int& optimal_k) {

        int min_cost = std::numeric_limits<int>::max();
        int low = i;
        int high = j - 1;

        // Binary search for optimal split point
        while (low <= high) {
            int mid = low + (high - low) / 2;

            // Compute cost for this split
            int left_cost = (mid >= i) ? dp[i][mid] : 0;
            int right_cost = (mid + 1 <= j) ? dp[mid + 1][j] : 0;
            int split_cost = cost(i, j, mid);
            int total_cost = left_cost + right_cost + split_cost;

            if (total_cost < min_cost) {
                min_cost = total_cost;
                optimal_k = mid;
            }

            // Use monotonicity to decide search direction
            // This is a simplified version - real implementation would
            // check neighboring costs to determine search direction
            int left_mid = mid - 1;
            int right_mid = mid + 1;

            if (left_mid >= i) {
                int left_total = dp[i][left_mid] +
                    ((left_mid + 1 <= j) ? dp[left_mid + 1][j] : 0) +
                    cost(i, j, left_mid);

                if (left_total < total_cost) {
                    high = mid - 1;
                    continue;
                }
            }

            if (right_mid <= j - 1) {
                int right_total = dp[i][right_mid] +
                    ((right_mid + 1 <= j) ? dp[right_mid + 1][j] : 0) +
                    cost(i, j, right_mid);

                if (right_total < total_cost) {
                    low = mid + 1;
                    continue;
                }
            }

            break; // Found optimal
        }

        return min_cost;
    }

public:
    // Demonstrate the optimization
    static void demonstrate() {
        std::cout << "Divide and Conquer DP Optimization Demonstration:" << std::endl;

        // Matrix Chain Multiplication
        std::vector<int> dimensions = {10, 20, 30, 40, 50};
        auto dp = matrix_chain_multiplication(dimensions);

        std::cout << "\nMatrix Chain Multiplication DP Table:" << std::endl;
        for (const auto& row : dp) {
            for (int val : row) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }

        // Optimal BST
        std::vector<double> frequencies = {0.1, 0.2, 0.4, 0.3};
        auto bst_dp = optimal_bst(frequencies);

        std::cout << "\nOptimal BST DP Table:" << std::endl;
        for (const auto& row : bst_dp) {
            for (int val : row) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
    }
};

// Example usage
int main() {
    DivideConquerDPOptimization::demonstrate();
    return 0;
}

