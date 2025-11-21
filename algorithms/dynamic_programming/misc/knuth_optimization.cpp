// Knuth's Optimization: Optimize DP with cost function C(i, j)
// Applicable when: C(i, j) satisfies quadrangle inequality and monotonicity
// Reduces O(n^3) to O(n^2) for certain DP problems
// Used in: Optimal Binary Search Tree, Matrix Chain Multiplication variants

#include <vector>
#include <iostream>
#include <climits>
#include <algorithm>

// Example: Optimal Binary Search Tree
// dp[i][j] = min cost to construct BST from keys i to j
// Cost function satisfies quadrangle inequality
std::vector<std::vector<int>> optimalBST(const std::vector<int>& keys, 
                                          const std::vector<int>& freq) {
    int n = keys.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(n + 1, 0));
    std::vector<std::vector<int>> root(n + 1, std::vector<int>(n + 1, 0));
    std::vector<int> prefixSum(n + 1, 0);
    
    // Build prefix sum
    for (int i = 1; i <= n; i++) {
        prefixSum[i] = prefixSum[i - 1] + freq[i - 1];
    }
    
    // Base case: single node
    for (int i = 1; i <= n; i++) {
        dp[i][i] = freq[i - 1];
        root[i][i] = i;
    }
    
    // Fill DP table with Knuth optimization
    for (int len = 2; len <= n; len++) {
        for (int i = 1; i <= n - len + 1; i++) {
            int j = i + len - 1;
            dp[i][j] = INT_MAX;
            
            // Use root from previous iteration (Knuth's optimization)
            int leftBound = root[i][j - 1];
            int rightBound = (i < j - 1) ? root[i + 1][j] : i;
            
            for (int r = leftBound; r <= rightBound; r++) {
                int cost = dp[i][r - 1] + dp[r + 1][j] + 
                          (prefixSum[j] - prefixSum[i - 1]);
                
                if (cost < dp[i][j]) {
                    dp[i][j] = cost;
                    root[i][j] = r;
                }
            }
        }
    }
    
    return dp;
}

// Matrix Chain Multiplication with Knuth optimization
std::vector<std::vector<int>> matrixChainOrder(const std::vector<int>& dims) {
    int n = dims.size() - 1;
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(n + 1, 0));
    std::vector<std::vector<int>> split(n + 1, std::vector<int>(n + 1, 0));
    
    for (int len = 2; len <= n; len++) {
        for (int i = 1; i <= n - len + 1; i++) {
            int j = i + len - 1;
            dp[i][j] = INT_MAX;
            
            // Knuth optimization: narrow search range
            int leftBound = (i < j - 1) ? split[i][j - 1] : i;
            int rightBound = (i + 1 < j) ? split[i + 1][j] : j - 1;
            
            for (int k = leftBound; k <= rightBound; k++) {
                int cost = dp[i][k] + dp[k + 1][j] + 
                          dims[i - 1] * dims[k] * dims[j];
                
                if (cost < dp[i][j]) {
                    dp[i][j] = cost;
                    split[i][j] = k;
                }
            }
        }
    }
    
    return dp;
}

// Example usage
int main() {
    // Optimal BST example
    std::vector<int> keys = {10, 12, 20};
    std::vector<int> freq = {34, 8, 50};
    
    std::vector<std::vector<int>> dp = optimalBST(keys, freq);
    
    std::cout << "Optimal BST cost: " << dp[1][keys.size()] << std::endl;
    
    // Matrix Chain Multiplication example
    std::vector<int> dims = {1, 2, 3, 4, 5};
    std::vector<std::vector<int>> mcm = matrixChainOrder(dims);
    
    std::cout << "Matrix Chain Multiplication cost: " 
              << mcm[1][dims.size() - 1] << std::endl;
    
    return 0;
}

