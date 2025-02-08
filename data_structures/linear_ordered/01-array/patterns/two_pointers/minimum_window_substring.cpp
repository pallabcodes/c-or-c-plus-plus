#include <climits>
#include <iostream>
#include <unordered_map>
using namespace std;

string minWindow(string s, string t) {
  unordered_map<char, int> charCount;
  for (char c : t)
    charCount[c]++; // Store frequency of t

  int left = 0, minLen = INT_MAX, start = 0, count = 0;

  for (int right = 0; right < s.size(); right++) {
    if (--charCount[s[right]] >= 0)
      count++;

    while (count == t.size()) {
      if (right - left + 1 < minLen) {
        minLen = right - left + 1;
        start = left;
      }
      if (++charCount[s[left++]] > 0)
        count--;
    }
  }

  return minLen == INT_MAX ? "" : s.substr(start, minLen);
}

int main() {
  string s = "ADOBECODEBANC", t = "ABC";
  cout << "Min Window Substring: " << minWindow(s, t);
}
