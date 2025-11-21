// Convex Hull Trick: Optimize DP with linear functions
// Also known as Li Chao Tree or CHT
// Used for DP equations of form: dp[i] = min(dp[j] + cost(j, i))
// Time: O(n log n) or O(n) amortized
// Space: O(n)
// God modded technique from competitive programming

#include <vector>
#include <iostream>
#include <algorithm>
#include <climits>
#include <deque>

struct Line {
    long long m, b;
    
    Line(long long m, long long b) : m(m), b(b) {}
    
    long long eval(long long x) const {
        return m * x + b;
    }
};

class ConvexHullTrick {
private:
    std::deque<Line> lines;
    
    bool isBad(const Line& l1, const Line& l2, const Line& l3) {
        return (l3.b - l1.b) * (l1.m - l2.m) <= (l2.b - l1.b) * (l1.m - l3.m);
    }
    
public:
    void addLine(long long m, long long b) {
        Line newLine(m, b);
        
        while (lines.size() >= 2 && isBad(lines[lines.size() - 2], lines[lines.size() - 1], newLine)) {
            lines.pop_back();
        }
        
        lines.push_back(newLine);
    }
    
    long long query(long long x) {
        while (lines.size() >= 2 && lines[0].eval(x) >= lines[1].eval(x)) {
            lines.pop_front();
        }
        
        return lines[0].eval(x);
    }
    
    long long queryBinary(long long x) {
        int left = 0, right = lines.size() - 1;
        
        while (right - left > 1) {
            int mid = (left + right) / 2;
            if (lines[mid].eval(x) < lines[mid + 1].eval(x)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        
        return std::min(lines[left].eval(x), lines[right].eval(x));
    }
};

// Example: DP optimization for problems like "Breaking Strings"
// dp[i] = min(dp[j] + cost(j, i)) for j < i
long long solveDPWithCHT(const std::vector<long long>& arr, long long c) {
    int n = arr.size();
    ConvexHullTrick cht;
    
    std::vector<long long> dp(n);
    std::vector<long long> prefix(n + 1, 0);
    
    for (int i = 0; i < n; i++) {
        prefix[i + 1] = prefix[i] + arr[i];
    }
    
    dp[0] = 0;
    cht.addLine(-2 * prefix[0], dp[0] + prefix[0] * prefix[0]);
    
    for (int i = 1; i < n; i++) {
        dp[i] = cht.query(prefix[i]) + prefix[i] * prefix[i] + c;
        cht.addLine(-2 * prefix[i], dp[i] + prefix[i] * prefix[i]);
    }
    
    return dp[n - 1];
}

// Example usage
int main() {
    ConvexHullTrick cht;
    
    cht.addLine(2, 1);
    cht.addLine(-1, 5);
    cht.addLine(1, 3);
    
    std::cout << "Query at x=1: " << cht.query(1) << std::endl;
    std::cout << "Query at x=2: " << cht.query(2) << std::endl;
    std::cout << "Query at x=3: " << cht.query(3) << std::endl;
    
    std::vector<long long> arr = {1, 2, 3, 4, 5};
    long long result = solveDPWithCHT(arr, 10);
    std::cout << "\nDP solution with CHT: " << result << std::endl;
    
    return 0;
}

