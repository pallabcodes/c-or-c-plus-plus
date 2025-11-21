// Convex Hull Trick: Optimize DP transitions of form dp[i] = min(dp[j] + a[i]*b[j] + c[j])
// Maintains lower envelope of lines for O(1) amortized queries
// Time: O(n) for n insertions and queries
// Space: O(n)

#include <vector>
#include <deque>
#include <iostream>
#include <algorithm>

struct Line {
    long long m, b; // y = mx + b
    
    long long eval(long long x) const {
        return m * x + b;
    }
    
    double intersect(const Line& other) const {
        return (double)(other.b - b) / (m - other.m);
    }
};

class ConvexHullTrick {
private:
    std::deque<Line> lines;
    
    bool isBad(const Line& l1, const Line& l2, const Line& l3) {
        // Check if l2 is never optimal
        return l1.intersect(l3) <= l1.intersect(l2);
    }
    
public:
    void addLine(long long m, long long b) {
        Line newLine = {m, b};
        
        // Remove lines that are never optimal
        while (lines.size() >= 2 && 
               isBad(lines[lines.size() - 2], lines[lines.size() - 1], newLine)) {
            lines.pop_back();
        }
        
        lines.push_back(newLine);
    }
    
    long long query(long long x) {
        // Binary search for optimal line (if lines are sorted by slope)
        // For simplicity, using linear search here
        // Can be optimized to O(log n) with binary search
        
        while (lines.size() >= 2 && 
               lines[0].eval(x) >= lines[1].eval(x)) {
            lines.pop_front();
        }
        
        return lines[0].eval(x);
    }
    
    // Query with binary search for O(log n) per query
    long long queryBinary(long long x) {
        int left = 0, right = lines.size() - 1;
        
        while (left < right) {
            int mid = (left + right) / 2;
            if (lines[mid].eval(x) < lines[mid + 1].eval(x)) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        
        return lines[left].eval(x);
    }
};

// Example: DP[i] = min(DP[j] + (a[i] - a[j])^2) for j < i
// Can be rewritten as: DP[i] = a[i]^2 + min(-2*a[j]*a[i] + DP[j] + a[j]^2)
std::vector<long long> solveDPWithCHT(const std::vector<long long>& a) {
    int n = a.size();
    std::vector<long long> dp(n);
    ConvexHullTrick cht;
    
    dp[0] = 0;
    cht.addLine(-2 * a[0], a[0] * a[0] + dp[0]);
    
    for (int i = 1; i < n; i++) {
        dp[i] = a[i] * a[i] + cht.query(a[i]);
        cht.addLine(-2 * a[i], a[i] * a[i] + dp[i]);
    }
    
    return dp;
}

// Example usage
int main() {
    ConvexHullTrick cht;
    
    // Add lines: y = 2x + 1, y = -x + 5, y = 0.5x + 2
    cht.addLine(2, 1);
    cht.addLine(-1, 5);
    cht.addLine(0, 2); // Actually 0.5, but using integer for simplicity
    
    std::cout << "Query at x=1: " << cht.query(1) << std::endl;
    std::cout << "Query at x=2: " << cht.query(2) << std::endl;
    std::cout << "Query at x=3: " << cht.query(3) << std::endl;
    
    // Example DP problem
    std::vector<long long> a = {1, 2, 3, 4, 5};
    std::vector<long long> dp = solveDPWithCHT(a);
    
    std::cout << "\nDP values: ";
    for (long long val : dp) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    return 0;
}

