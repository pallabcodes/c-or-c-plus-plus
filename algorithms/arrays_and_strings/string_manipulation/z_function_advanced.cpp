// Advanced Z-Function: Extended applications
// Based on Z-algorithm research
// Time: O(n + m) for pattern matching
// Space: O(n + m)
// God modded implementation with advanced applications

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

// Standard Z-function
std::vector<int> zFunction(const std::string& s) {
    int n = s.length();
    std::vector<int> z(n, 0);
    
    int l = 0, r = 0;
    for (int i = 1; i < n; i++) {
        if (i <= r) {
            z[i] = std::min(r - i + 1, z[i - l]);
        }
        
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
            z[i]++;
        }
        
        if (i + z[i] - 1 > r) {
            l = i;
            r = i + z[i] - 1;
        }
    }
    
    return z;
}

// Find all occurrences of pattern in text
std::vector<int> findAllOccurrences(const std::string& text, const std::string& pattern) {
    std::string combined = pattern + "$" + text;
    std::vector<int> z = zFunction(combined);
    std::vector<int> occurrences;
    
    int patternLen = pattern.length();
    for (int i = patternLen + 1; i < combined.length(); i++) {
        if (z[i] == patternLen) {
            occurrences.push_back(i - patternLen - 1);
        }
    }
    
    return occurrences;
}

// Longest common prefix array
std::vector<int> longestCommonPrefix(const std::string& s) {
    return zFunction(s);
}

// String compression using Z-function
std::string compressString(const std::string& s) {
    int n = s.length();
    std::vector<int> z = zFunction(s);
    
    for (int len = 1; len <= n / 2; len++) {
        if (n % len == 0 && z[len] == n - len) {
            return s.substr(0, len);
        }
    }
    
    return s;
}

// Find period of string
int findPeriod(const std::string& s) {
    int n = s.length();
    std::vector<int> z = zFunction(s);
    
    for (int len = 1; len <= n / 2; len++) {
        if (n % len == 0 && z[len] == n - len) {
            return len;
        }
    }
    
    return n;
}

// Count distinct substrings using Z-function
long long countDistinctSubstrings(const std::string& s) {
    int n = s.length();
    long long count = 0;
    
    for (int i = 0; i < n; i++) {
        std::string suffix = s.substr(i);
        std::vector<int> z = zFunction(suffix);
        
        int maxZ = 0;
        for (int j = 1; j < z.size(); j++) {
            maxZ = std::max(maxZ, z[j]);
        }
        
        count += (suffix.length() - maxZ);
    }
    
    return count;
}

// Example usage
int main() {
    std::string text = "abababab";
    std::string pattern = "aba";
    
    std::vector<int> occurrences = findAllOccurrences(text, pattern);
    
    std::cout << "Pattern \"" << pattern << "\" found at positions: ";
    for (int pos : occurrences) {
        std::cout << pos << " ";
    }
    std::cout << std::endl;
    
    std::string test = "abcabcabc";
    std::cout << "\nPeriod of \"" << test << "\": " << findPeriod(test) << std::endl;
    
    std::string compressed = compressString(test);
    std::cout << "Compressed form: \"" << compressed << "\"" << std::endl;
    
    std::string s = "abc";
    std::cout << "\nDistinct substrings in \"" << s << "\": " 
              << countDistinctSubstrings(s) << std::endl;
    
    return 0;
}

