// Divide and Conquer DP Optimization
// Optimizes DP transitions when cost function satisfies certain properties
// Reduces O(n^2) to O(n log n) for certain problems
// Applicable when: dp[i][j] = min(dp[i-1][k] + cost(k, j)) for k < j

#include <vector>
#include <iostream>
#include <climits>
#include <algorithm>

// Example: Optimal Partition Problem
// Partition array into k segments to minimize sum of segment costs
void solve(int l, int r, int optL, int optR, int layer,
           const std::vector<int>& arr,
           std::vector<std::vector<int>>& dp,
           std::vector<std::vector<int>>& cost) {
    
    if (l > r) return;
    
    int mid = (l + r) / 2;
    int bestK = optL;
    int bestVal = INT_MAX;
    
    // Find optimal k for mid
    for (int k = optL; k <= std::min(mid, optR); k++) {
        int val = dp[layer - 1][k] + cost[k][mid];
        if (val < bestVal) {
            bestVal = val;
            bestK = k;
        }
    }
    
    dp[layer][mid] = bestVal;
    
    // Recurse
    solve(l, mid - 1, optL, bestK, layer, arr, dp, cost);
    solve(mid + 1, r, bestK, optR, layer, arr, dp, cost);
}

std::vector<std::vector<int>> optimalPartition(const std::vector<int>& arr, int k) {
    int n = arr.size();
    std::vector<std::vector<int>> dp(k + 1, std::vector<int>(n + 1, INT_MAX));
    std::vector<std::vector<int>> cost(n + 1, std::vector<int>(n + 1, 0));
    
    // Precompute cost[i][j] = cost of segment from i to j
    // Example: sum of squares
    std::vector<int> prefixSum(n + 1, 0);
    for (int i = 1; i <= n; i++) {
        prefixSum[i] = prefixSum[i - 1] + arr[i - 1];
    }
    
    for (int i = 1; i <= n; i++) {
        for (int j = i; j <= n; j++) {
            int sum = prefixSum[j] - prefixSum[i - 1];
            cost[i][j] = sum * sum; // Example cost function
        }
    }
    
    // Base case: single segment
    for (int i = 1; i <= n; i++) {
        dp[1][i] = cost[1][i];
    }
    
    // Fill DP using divide and conquer
    for (int layer = 2; layer <= k; layer++) {
        solve(1, n, 1, n, layer, arr, dp, cost);
    }
    
    return dp;
}

// Example: Longest Increasing Subsequence with D&C optimization
// When we need to find LIS with certain constraints
std::vector<int> lisDivideConquer(const std::vector<int>& arr) {
    int n = arr.size();
    std::vector<int> tail(n, 0);
    std::vector<int> prev(n, -1);
    int len = 0;
    
    for (int i = 0; i < n; i++) {
        // Binary search for position
        int left = 0, right = len;
        while (left < right) {
            int mid = (left + right) / 2;
            if (arr[tail[mid]] < arr[i]) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        
        prev[i] = (left > 0) ? tail[left - 1] : -1;
        tail[left] = i;
        
        if (left == len) {
            len++;
        }
    }
    
    // Reconstruct LIS
    std::vector<int> lis;
    int idx = tail[len - 1];
    while (idx != -1) {
        lis.push_back(arr[idx]);
        idx = prev[idx];
    }
    std::reverse(lis.begin(), lis.end());
    
    return lis;
}

// Example usage
int main() {
    // Optimal Partition example
    std::vector<int> arr = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int k = 3;
    
    std::vector<std::vector<int>> dp = optimalPartition(arr, k);
    
    std::cout << "Optimal partition cost: " << dp[k][arr.size()] << std::endl;
    
    // LIS example
    std::vector<int> arr2 = {10, 9, 2, 5, 3, 7, 101, 18};
    std::vector<int> lis = lisDivideConquer(arr2);
    
    std::cout << "Longest Increasing Subsequence: ";
    for (int x : lis) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    
    return 0;
}

