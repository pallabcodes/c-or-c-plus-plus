// Suffix Array: Data structure for efficient string operations
// Contains sorted array of all suffixes of a string
// Enables O(m log n) pattern searching where m is pattern length
// Space: O(n)

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

struct Suffix {
    int index;
    int rank[2];
};

// Comparison function for suffix sorting
int cmp(const Suffix& a, const Suffix& b) {
    if (a.rank[0] == b.rank[0]) {
        return a.rank[1] < b.rank[1];
    }
    return a.rank[0] < b.rank[0];
}

// Build suffix array using doubling method
std::vector<int> buildSuffixArray(const std::string& txt) {
    int n = txt.length();
    std::vector<Suffix> suffixes(n);
    
    // Initialize ranks for single characters
    for (int i = 0; i < n; i++) {
        suffixes[i].index = i;
        suffixes[i].rank[0] = txt[i] - 'a';
        suffixes[i].rank[1] = (i + 1 < n) ? (txt[i + 1] - 'a') : -1;
    }
    
    // Sort suffixes by first 2 characters
    std::sort(suffixes.begin(), suffixes.end(), cmp);
    
    // Build suffix array by doubling
    std::vector<int> ind(n);
    for (int k = 4; k < 2 * n; k = k * 2) {
        int rank = 0;
        int prevRank = suffixes[0].rank[0];
        suffixes[0].rank[0] = rank;
        ind[suffixes[0].index] = 0;
        
        for (int i = 1; i < n; i++) {
            if (suffixes[i].rank[0] == prevRank &&
                suffixes[i].rank[1] == suffixes[i - 1].rank[1]) {
                prevRank = suffixes[i].rank[0];
                suffixes[i].rank[0] = rank;
            } else {
                prevRank = suffixes[i].rank[0];
                suffixes[i].rank[0] = ++rank;
            }
            ind[suffixes[i].index] = i;
        }
        
        // Assign next rank
        for (int i = 0; i < n; i++) {
            int nextIndex = suffixes[i].index + k / 2;
            suffixes[i].rank[1] = (nextIndex < n) ? 
                                  suffixes[ind[nextIndex]].rank[0] : -1;
        }
        
        std::sort(suffixes.begin(), suffixes.end(), cmp);
    }
    
    std::vector<int> suffixArr(n);
    for (int i = 0; i < n; i++) {
        suffixArr[i] = suffixes[i].index;
    }
    
    return suffixArr;
}

// Build LCP (Longest Common Prefix) array
std::vector<int> buildLCPArray(const std::string& txt, 
                                const std::vector<int>& suffixArr) {
    int n = txt.length();
    std::vector<int> lcp(n, 0);
    std::vector<int> invSuffix(n);
    
    for (int i = 0; i < n; i++) {
        invSuffix[suffixArr[i]] = i;
    }
    
    int k = 0;
    for (int i = 0; i < n; i++) {
        if (invSuffix[i] == n - 1) {
            k = 0;
            continue;
        }
        
        int j = suffixArr[invSuffix[i] + 1];
        
        while (i + k < n && j + k < n && txt[i + k] == txt[j + k]) {
            k++;
        }
        
        lcp[invSuffix[i]] = k;
        
        if (k > 0) k--;
    }
    
    return lcp;
}

// Search pattern in text using suffix array
std::vector<int> searchPattern(const std::string& txt, 
                               const std::string& pattern,
                               const std::vector<int>& suffixArr) {
    std::vector<int> result;
    int n = txt.length();
    int m = pattern.length();
    
    int left = 0, right = n - 1;
    
    // Binary search for left boundary
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int res = pattern.compare(0, m, txt, suffixArr[mid], m);
        
        if (res == 0) {
            result.push_back(suffixArr[mid]);
        }
        
        if (res <= 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    
    // Binary search for right boundary
    left = 0;
    right = n - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int res = pattern.compare(0, m, txt, suffixArr[mid], m);
        
        if (res <= 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    
    return result;
}

// Find longest repeated substring
std::string longestRepeatedSubstring(const std::string& txt) {
    std::vector<int> suffixArr = buildSuffixArray(txt);
    std::vector<int> lcp = buildLCPArray(txt, suffixArr);
    
    int maxLen = 0;
    int maxIndex = 0;
    
    for (int i = 0; i < txt.length(); i++) {
        if (lcp[i] > maxLen) {
            maxLen = lcp[i];
            maxIndex = suffixArr[i];
        }
    }
    
    return (maxLen > 0) ? txt.substr(maxIndex, maxLen) : "";
}

// Example usage
int main() {
    std::string txt = "banana";
    
    std::cout << "Text: " << txt << std::endl;
    
    std::vector<int> suffixArr = buildSuffixArray(txt);
    
    std::cout << "Suffix Array: ";
    for (int idx : suffixArr) {
        std::cout << idx << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\nSuffixes in sorted order:" << std::endl;
    for (int idx : suffixArr) {
        std::cout << idx << ": " << txt.substr(idx) << std::endl;
    }
    
    std::vector<int> lcp = buildLCPArray(txt, suffixArr);
    std::cout << "\nLCP Array: ";
    for (int len : lcp) {
        std::cout << len << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\nLongest repeated substring: " 
              << longestRepeatedSubstring(txt) << std::endl;
    
    // Search pattern
    std::string pattern = "ana";
    std::vector<int> positions = searchPattern(txt, pattern, suffixArr);
    std::cout << "\nPattern \"" << pattern << "\" found at positions: ";
    for (int pos : positions) {
        std::cout << pos << " ";
    }
    std::cout << std::endl;
    
    return 0;
}

