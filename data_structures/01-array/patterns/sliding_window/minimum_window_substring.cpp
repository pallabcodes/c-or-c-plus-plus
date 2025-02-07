#include <climits>
#include <iostream>
#include <unordered_map>
using namespace std;

string minWindow(string s, string t) {
  if (s.empty() || t.empty())
    return "";

  unordered_map<char, int> tMap, windowMap;
  for (char c : t)
    tMap[c]++; // Count freq of t

  int left = 0, minLen = INT_MAX, minStart = 0;
  int required = tMap.size(), matched = 0; // Unique char tracking

  for (int right = 0; right < s.size(); right++) {
    char c = s[right];
    windowMap[c]++;

    if (tMap.count(c) && windowMap[c] == tMap[c])
      matched++;

    while (matched == required) { // Contract window
      if (right - left + 1 < minLen) {
        minLen = right - left + 1;
        minStart = left;
      }
      char leftChar = s[left++];
      windowMap[leftChar]--;
      if (tMap.count(leftChar) && windowMap[leftChar] < tMap[leftChar])
        matched--;
    }
  }

  return (minLen == INT_MAX) ? "" : s.substr(minStart, minLen);
}

int main() {
  cout << minWindow("ADOBECODEBANC", "ABC") << endl; // Output: "BANC"
  return 0;
}
