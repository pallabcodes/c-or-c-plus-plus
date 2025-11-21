// Manacher's Algorithm: Linear time algorithm for finding longest palindromic substring
// Uses expansion around centers with optimization to avoid recomputation
// Time: O(n) where n is string length
// Space: O(n)

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <cmath>

// Transform string to handle even-length palindromes
std::string transformString(const std::string& s) {
    std::string transformed = "#";
    for (char c : s) {
        transformed += c;
        transformed += "#";
    }
    return transformed;
}

// Manacher's algorithm to find longest palindromic substring
std::string manacherLongestPalindrome(const std::string& s) {
    if (s.empty()) return "";
    
    std::string transformed = transformString(s);
    int n = transformed.length();
    std::vector<int> p(n, 0);
    
    int center = 0, right = 0;
    int maxLen = 0, centerIndex = 0;
    
    for (int i = 0; i < n; i++) {
        int mirror = 2 * center - i;
        
        if (i < right) {
            p[i] = std::min(right - i, p[mirror]);
        }
        
        // Try to expand palindrome centered at i
        int leftBound = i - (1 + p[i]);
        int rightBound = i + (1 + p[i]);
        
        while (leftBound >= 0 && rightBound < n && 
               transformed[leftBound] == transformed[rightBound]) {
            p[i]++;
            leftBound--;
            rightBound++;
        }
        
        // Update center and right if palindrome extends beyond right
        if (i + p[i] > right) {
            center = i;
            right = i + p[i];
        }
        
        // Update maximum length palindrome
        if (p[i] > maxLen) {
            maxLen = p[i];
            centerIndex = i;
        }
    }
    
    // Extract longest palindrome from original string
    int start = (centerIndex - maxLen) / 2;
    return s.substr(start, maxLen);
}

// Count all palindromic substrings using Manacher's algorithm
int countPalindromicSubstrings(const std::string& s) {
    if (s.empty()) return 0;
    
    std::string transformed = transformString(s);
    int n = transformed.length();
    std::vector<int> p(n, 0);
    
    int center = 0, right = 0;
    int count = 0;
    
    for (int i = 0; i < n; i++) {
        int mirror = 2 * center - i;
        
        if (i < right) {
            p[i] = std::min(right - i, p[mirror]);
        }
        
        int leftBound = i - (1 + p[i]);
        int rightBound = i + (1 + p[i]);
        
        while (leftBound >= 0 && rightBound < n && 
               transformed[leftBound] == transformed[rightBound]) {
            p[i]++;
            leftBound--;
            rightBound++;
        }
        
        if (i + p[i] > right) {
            center = i;
            right = i + p[i];
        }
        
        // Count palindromes: (p[i] + 1) / 2 for each center
        count += (p[i] + 1) / 2;
    }
    
    return count;
}

// Find longest palindromic subsequence length (different from substring)
int longestPalindromicSubsequence(const std::string& s) {
    int n = s.length();
    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));
    
    // Every single character is a palindrome of length 1
    for (int i = 0; i < n; i++) {
        dp[i][i] = 1;
    }
    
    // Fill table for substrings of length 2 and more
    for (int len = 2; len <= n; len++) {
        for (int i = 0; i < n - len + 1; i++) {
            int j = i + len - 1;
            
            if (s[i] == s[j] && len == 2) {
                dp[i][j] = 2;
            } else if (s[i] == s[j]) {
                dp[i][j] = dp[i + 1][j - 1] + 2;
            } else {
                dp[i][j] = std::max(dp[i][j - 1], dp[i + 1][j]);
            }
        }
    }
    
    return dp[0][n - 1];
}

// Example usage
int main() {
    std::string s = "forgeeksskeegfor";
    
    std::cout << "String: " << s << std::endl;
    std::cout << "Longest palindromic substring: " 
              << manacherLongestPalindrome(s) << std::endl;
    std::cout << "Number of palindromic substrings: " 
              << countPalindromicSubstrings(s) << std::endl;
    std::cout << "Longest palindromic subsequence length: " 
              << longestPalindromicSubsequence(s) << std::endl;
    
    // Test with another example
    std::string s2 = "babad";
    std::cout << "\nString: " << s2 << std::endl;
    std::cout << "Longest palindromic substring: " 
              << manacherLongestPalindrome(s2) << std::endl;
    
    return 0;
}

