#include <iostream>
#include <vector>
using namespace std;

int characterReplacement(string s, int k) {
  vector<int> freq(26, 0);
  int maxCount = 0, left = 0, maxLen = 0;

  for (int right = 0; right < s.size(); right++) {
    maxCount = max(maxCount, ++freq[s[right] - 'A']);

    while ((right - left + 1) - maxCount > k) {
      freq[s[left] - 'A']--;
      left++;
    }

    maxLen = max(maxLen, right - left + 1);
  }

  return maxLen;
}

int main() {
  cout << characterReplacement("AABABBA", 1) << endl; // Output: 4
  return 0;
}
