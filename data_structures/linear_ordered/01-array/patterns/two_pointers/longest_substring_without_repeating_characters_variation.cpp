#include <iostream>
#include <unordered_map>
using namespace std;

int lengthOfLongestSubstring(string s) {
  unordered_map<char, int> charMap;
  int left = 0, maxLen = 0;

  for (int right = 0; right < s.size(); right++) {
    charMap[s[right]]++;

    while (charMap[s[right]] > 1) {
      charMap[s[left]]--;
      left++;
    }

    maxLen = max(maxLen, right - left + 1);
  }

  return maxLen;
}

int main() {
  string s = "abcabcbb";
  cout << "Length of Longest Substring: " << lengthOfLongestSubstring(s);
}
