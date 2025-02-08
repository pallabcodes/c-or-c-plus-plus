#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> dp;

int longestPalindrome(string &s, int i, int j) {
  if (i > j)
    return 0;
  if (i == j)
    return 1;

  if (dp[i][j] != -1)
    return dp[i][j];

  if (s[i] == s[j])
    return dp[i][j] = 2 + longestPalindrome(s, i + 1, j - 1);

  return dp[i][j] = max(longestPalindrome(s, i + 1, j),
                        longestPalindrome(s, i, j - 1));
}

int main() {
  string s = "bbbab";
  int n = s.size();
  dp.assign(n, vector<int>(n, -1));
  cout << "Longest Palindromic Subsequence: " << longestPalindrome(s, 0, n - 1)
       << endl;
  return 0;
}
