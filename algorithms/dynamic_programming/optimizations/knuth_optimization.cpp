// Knuth's Optimization for DP
// Optimizes DP of form: dp[i][j] = min(dp[i][k] + dp[k+1][j] + cost(i, j))
// Requires: cost(i, j) satisfies quadrangle inequality and monotonicity
// Time: O(n^2) instead of O(n^3)
// Space: O(n^2)
// God modded optimization from Knuth's research

#include <vector>
#include <iostream>
#include <algorithm>
#include <climits>

// Standard O(n^3) matrix chain multiplication
long long matrixChainStandard(const std::vector<int>& dims) {
    int n = dims.size() - 1;
    std::vector<std::vector<long long>> dp(n + 1, std::vector<long long>(n + 1, 0));
    
    for (int len = 2; len <= n; len++) {
        for (int i = 1; i <= n - len + 1; i++) {
            int j = i + len - 1;
            dp[i][j] = LLONG_MAX;
            
            for (int k = i; k < j; k++) {
                long long cost = dp[i][k] + dp[k + 1][j] + 
                                (long long)dims[i - 1] * dims[k] * dims[j];
                dp[i][j] = std::min(dp[i][j], cost);
            }
        }
    }
    
    return dp[1][n];
}

// Knuth optimized O(n^2) version
long long matrixChainKnuth(const std::vector<int>& dims) {
    int n = dims.size() - 1;
    std::vector<std::vector<long long>> dp(n + 1, std::vector<long long>(n + 1, 0));
    std::vector<std::vector<int>> opt(n + 1, std::vector<int>(n + 1, 0));
    
    for (int i = 1; i <= n; i++) {
        opt[i][i] = i;
    }
    
    for (int len = 2; len <= n; len++) {
        for (int i = 1; i <= n - len + 1; i++) {
            int j = i + len - 1;
            dp[i][j] = LLONG_MAX;
            
            int left = opt[i][j - 1];
            int right = opt[i + 1][j];
            
            for (int k = left; k <= right; k++) {
                long long cost = dp[i][k] + dp[k + 1][j] + 
                                (long long)dims[i - 1] * dims[k] * dims[j];
                if (cost < dp[i][j]) {
                    dp[i][j] = cost;
                    opt[i][j] = k;
                }
            }
        }
    }
    
    return dp[1][n];
}

// Example: Optimal Binary Search Tree with Knuth optimization
long long optimalBSTKnuth(const std::vector<long long>& freq) {
    int n = freq.size();
    std::vector<long long> prefix(n + 1, 0);
    
    for (int i = 0; i < n; i++) {
        prefix[i + 1] = prefix[i] + freq[i];
    }
    
    std::vector<std::vector<long long>> dp(n + 1, std::vector<long long>(n + 1, 0));
    std::vector<std::vector<int>> opt(n + 1, std::vector<int>(n + 1, 0));
    
    for (int i = 0; i < n; i++) {
        opt[i][i] = i;
        dp[i][i] = freq[i];
    }
    
    for (int len = 2; len <= n; len++) {
        for (int i = 0; i <= n - len; i++) {
            int j = i + len - 1;
            dp[i][j] = LLONG_MAX;
            
            int left = opt[i][j - 1];
            int right = opt[i + 1][j];
            
            for (int k = left; k <= right; k++) {
                long long cost = (k > i ? dp[i][k - 1] : 0) + 
                                (k < j ? dp[k + 1][j] : 0) + 
                                prefix[j + 1] - prefix[i];
                
                if (cost < dp[i][j]) {
                    dp[i][j] = cost;
                    opt[i][j] = k;
                }
            }
        }
    }
    
    return dp[0][n - 1];
}

// Example usage
int main() {
    std::vector<int> dims = {1, 2, 3, 4, 5};
    
    long long result1 = matrixChainStandard(dims);
    long long result2 = matrixChainKnuth(dims);
    
    std::cout << "Matrix Chain Multiplication (standard): " << result1 << std::endl;
    std::cout << "Matrix Chain Multiplication (Knuth): " << result2 << std::endl;
    
    std::vector<long long> freq = {34, 8, 50};
    long long bstCost = optimalBSTKnuth(freq);
    std::cout << "\nOptimal BST cost (Knuth): " << bstCost << std::endl;
    
    return 0;
}

