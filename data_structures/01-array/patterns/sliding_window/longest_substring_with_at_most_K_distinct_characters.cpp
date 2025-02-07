// Problem: Given a string s, find the length of the longest substring that
// contains at most K distinct characters

#include <iostream>
#include <unordered_map>
using namespace std;

int lengthOfLongestSubstringKDistinct(string s, int k) {
  unordered_map<char, int> freq;
  int left = 0, maxLen = 0;

  for (int right = 0; right < s.size(); right++) {
    freq[s[right]]++;

    while (freq.size() > k) { // Reduce window if > K distinct chars
      freq[s[left]]--;
      if (freq[s[left]] == 0)
        freq.erase(s[left]);
      left++;
    }

    maxLen = max(maxLen, right - left + 1);
  }

  return maxLen;
}

int main() {
  cout << lengthOfLongestSubstringKDistinct("eceba", 2) << endl; // Output: 3
  return 0;
}
