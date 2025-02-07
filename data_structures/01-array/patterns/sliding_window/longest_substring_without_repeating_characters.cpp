#include <algorithm>
#include <iostream>
#include <unordered_map>
using namespace std;

// Problem: Given a string, find the length of the longest substring without
// repeating characters
int longestUniqueSubstring(string s) {
  unordered_map<char, int> charMap; // Similar to a JS Map(), stores character
                                    // indices for fast lookup
  int left = 0, maxLen = 0; // left -> start of sliding window, maxLen -> stores
                            // the longest unique substring length

  for (int right = 0; right < s.size();
       right++) { // right -> end of sliding window

    // Expand window : charMap.find(s[right]) returns charMap.end() that means
    // the key does not exist (similar to !charMap.has(s[right]) in JS) which is
    // why if find method doesn't return end that mean it has found the key
    // within charMap
    if (charMap.find(s[right]) != charMap.end()) {
      // Move the 'left' pointer to ensure uniqueness (if needed)
      left = max(left,
                 charMap[s[right]] +
                     1); // Contract Window to avoid duplicates : Similar to
                         // moving 'left' pointer in a JS two-pointer approach
    }

    charMap[s[right]] = right; // Update the latest index of the character
    maxLen = max(maxLen, (right - left) + 1); // Track the maximum length
  }

  return maxLen;
}

int main() {
  string s = "abcabcbb";
  cout << "Longest substring without repeating: " << longestUniqueSubstring(s)
       << endl; // Output: 3
  return 0;
}
