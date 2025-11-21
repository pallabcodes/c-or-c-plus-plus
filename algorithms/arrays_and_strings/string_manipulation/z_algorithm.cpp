// Z-Algorithm: Linear time pattern matching and string searching
// Builds Z-array where Z[i] is length of longest substring starting at i
// that matches prefix of string
// Time: O(n + m) where n is text length, m is pattern length
// Space: O(n + m)

#include <vector>
#include <string>
#include <iostream>

// Build Z-array for given string
std::vector<int> buildZArray(const std::string& str) {
    int n = str.length();
    std::vector<int> z(n, 0);
    
    int l = 0, r = 0; // [l, r] is the Z-box
    
    for (int i = 1; i < n; i++) {
        if (i > r) {
            // No Z-box, do naive matching
            l = r = i;
            while (r < n && str[r - l] == str[r]) {
                r++;
            }
            z[i] = r - l;
            r--;
        } else {
            // Inside Z-box
            int k = i - l;
            if (z[k] < r - i + 1) {
                // Can use previous Z-value
                z[i] = z[k];
            } else {
                // Need to extend
                l = i;
                while (r < n && str[r - l] == str[r]) {
                    r++;
                }
                z[i] = r - l;
                r--;
            }
        }
    }
    
    return z;
}

// Search pattern in text using Z-algorithm
std::vector<int> zAlgorithmSearch(const std::string& text, const std::string& pattern) {
    std::vector<int> result;
    
    // Create combined string: pattern + '$' + text
    std::string combined = pattern + "$" + text;
    std::vector<int> z = buildZArray(combined);
    
    int patternLen = pattern.length();
    
    // Find all positions where pattern matches
    for (int i = patternLen + 1; i < combined.length(); i++) {
        if (z[i] == patternLen) {
            result.push_back(i - patternLen - 1);
        }
    }
    
    return result;
}

// Count occurrences of pattern in text
int countOccurrences(const std::string& text, const std::string& pattern) {
    return zAlgorithmSearch(text, pattern).size();
}

// Find longest palindromic substring using Z-algorithm trick
std::string longestPalindromicSubstring(const std::string& s) {
    if (s.empty()) return "";
    
    // Create string: s + '#' + reverse(s)
    std::string rev = s;
    std::reverse(rev.begin(), rev.end());
    std::string combined = s + "#" + rev;
    
    std::vector<int> z = buildZArray(combined);
    
    int maxLen = 0;
    int start = 0;
    int n = s.length();
    
    for (int i = n + 1; i < combined.length(); i++) {
        if (z[i] > maxLen && (i - n - 1 + z[i] - 1) == n - 1) {
            maxLen = z[i];
            start = i - n - 1;
        }
    }
    
    return s.substr(start, maxLen);
}

// Example usage
int main() {
    std::string text = "ABABDABACDABABCABCABC";
    std::string pattern = "ABABCABC";
    
    std::cout << "Text: " << text << std::endl;
    std::cout << "Pattern: " << pattern << std::endl;
    
    std::vector<int> positions = zAlgorithmSearch(text, pattern);
    
    std::cout << "Pattern found at positions: ";
    for (int pos : positions) {
        std::cout << pos << " ";
    }
    std::cout << std::endl;
    
    // Test longest palindromic substring
    std::string s = "forgeeksskeegfor";
    std::cout << "\nString: " << s << std::endl;
    std::cout << "Longest palindromic substring: " 
              << longestPalindromicSubstring(s) << std::endl;
    
    return 0;
}

