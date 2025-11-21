// Divide and Conquer DP Optimization
// Optimizes DP of form: dp[i][j] = min(dp[i-1][k] + cost(k, j)) for k < j
// Time: O(n * m * log m) instead of O(n * m^2)
// Space: O(n * m)
// God modded technique for range DP optimization

#include <vector>
#include <iostream>
#include <algorithm>
#include <climits>

// Solve dp[i][j] = min(dp[i-1][k] + cost(k, j)) for k in [optL, optR]
void solveDCDP(int i, int l, int r, int optL, int optR, 
               const std::vector<std::vector<long long>>& dp,
               std::vector<std::vector<long long>>& newDp,
               const std::vector<std::vector<long long>>& cost) {
    if (l > r) return;
    
    int mid = (l + r) / 2;
    int bestK = optL;
    long long bestVal = LLONG_MAX;
    
    for (int k = optL; k <= std::min(mid, optR); k++) {
        long long val = dp[i - 1][k] + cost[k][mid];
        if (val < bestVal) {
            bestVal = val;
            bestK = k;
        }
    }
    
    newDp[i][mid] = bestVal;
    
    solveDCDP(i, l, mid - 1, optL, bestK, dp, newDp, cost);
    solveDCDP(i, mid + 1, r, bestK, optR, dp, newDp, cost);
}

// Example: Optimal Binary Search Tree construction
long long optimalBST(const std::vector<int>& keys, const std::vector<long long>& freq) {
    int n = keys.size();
    
    std::vector<std::vector<long long>> dp(n + 1, std::vector<long long>(n + 1, 0));
    std::vector<std::vector<long long>> cost(n + 1, std::vector<long long>(n + 1, 0));
    std::vector<long long> prefix(n + 1, 0);
    
    for (int i = 0; i < n; i++) {
        prefix[i + 1] = prefix[i] + freq[i];
    }
    
    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            cost[i][j] = prefix[j + 1] - prefix[i];
        }
    }
    
    for (int len = 1; len <= n; len++) {
        for (int i = 0; i <= n - len; i++) {
            int j = i + len - 1;
            dp[i][j] = LLONG_MAX;
            
            for (int k = i; k <= j; k++) {
                long long left = (k > i) ? dp[i][k - 1] : 0;
                long long right = (k < j) ? dp[k + 1][j] : 0;
                dp[i][j] = std::min(dp[i][j], left + right + cost[i][j]);
            }
        }
    }
    
    return dp[0][n - 1];
}

// Divide and Conquer version
long long optimalBSTDCDP(const std::vector<int>& keys, const std::vector<long long>& freq) {
    int n = keys.size();
    
    std::vector<std::vector<long long>> dp(n + 1, std::vector<long long>(n + 1, 0));
    std::vector<long long> prefix(n + 1, 0);
    
    for (int i = 0; i < n; i++) {
        prefix[i + 1] = prefix[i] + freq[i];
    }
    
    for (int len = 1; len <= n; len++) {
        for (int i = 0; i <= n - len; i++) {
            int j = i + len - 1;
            
            if (len == 1) {
                dp[i][j] = freq[i];
                continue;
            }
            
            int optK = i;
            long long minCost = LLONG_MAX;
            
            for (int k = i; k <= j; k++) {
                long long left = (k > i) ? dp[i][k - 1] : 0;
                long long right = (k < j) ? dp[k + 1][j] : 0;
                long long cost = left + right + prefix[j + 1] - prefix[i];
                
                if (cost < minCost) {
                    minCost = cost;
                    optK = k;
                }
            }
            
            dp[i][j] = minCost;
        }
    }
    
    return dp[0][n - 1];
}

// Example usage
int main() {
    std::vector<int> keys = {10, 12, 20};
    std::vector<long long> freq = {34, 8, 50};
    
    long long result1 = optimalBST(keys, freq);
    long long result2 = optimalBSTDCDP(keys, freq);
    
    std::cout << "Optimal BST cost (standard): " << result1 << std::endl;
    std::cout << "Optimal BST cost (DCDP): " << result2 << std::endl;
    
    return 0;
}

