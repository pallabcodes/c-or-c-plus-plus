#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

// An anagram means a permutation of characters (same frequency, different
// order). Example: "abc" and "bca" are anagrams. Sliding Window allows us to
// efficiently check substrings without recomputing frequencies every time.

vector<int> findAnagrams(string s, string p) {
  vector<int> result;
  int sLen = s.size(), pLen = p.size();
  if (sLen < pLen)
    return result; // Edge case: p is longer than s

  unordered_map<char, int> pFreq, windowFreq;

  // Build frequency map for pattern p
  for (char c : p)
    pFreq[c]++;

  // Initialize frequency map for the first window in s
  for (int i = 0; i < pLen; i++)
    windowFreq[s[i]]++;

  // Check if the first window is an anagram
  if (pFreq == windowFreq)
    result.push_back(0);

  // Slide window across s
  for (int i = pLen; i < sLen; i++) {
    char newChar = s[i];        // New character to include
    char oldChar = s[i - pLen]; // Old character to remove

    windowFreq[newChar]++; // Expand window
    windowFreq[oldChar]--; // Contract window

    if (windowFreq[oldChar] == 0) // Clean up zero frequency
      windowFreq.erase(oldChar);

    if (pFreq == windowFreq) // Check for match
      result.push_back(i - pLen + 1);
  }

  return result;
}

vector<int> findAnagramsV2(const string &s, const string &p) {
  vector<int> result;
  int sLen = s.size(), pLen = p.size();
  if (sLen < pLen)
    return result;

  unordered_map<char, int> pFreq, windowFreq;

  for (char c : p)
    pFreq[c]++;
  for (int i = 0; i < pLen; i++)
    windowFreq[s[i]]++;

  if (pFreq == windowFreq)
    result.push_back(0);

  for (int i = pLen; i < sLen; i++) {
    char newChar = s[i];
    char oldChar = s[i - pLen];

    windowFreq[newChar]++;
    if (--windowFreq[oldChar] == 0)
      windowFreq.erase(oldChar);

    if (pFreq == windowFreq)
      result.push_back(i - pLen + 1);
  }

  return result;
}

int main() {
  string s = "cbaebabacd", p = "abc";
  vector<int> result = findAnagrams(s, p);
  cout << "Anagrams found at indices: ";

  // whenever a substring matched, its starting index taken thus 0, 6
  for (int idx : result) {
    cout << idx << " "; // Output: 0 6
  }

  return 0;
}
