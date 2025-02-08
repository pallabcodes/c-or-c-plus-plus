#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> dp;

int uniquePaths(int m, int n) {
  if (m == 1 || n == 1)
    return 1;

  if (dp[m][n] != -1)
    return dp[m][n];

  return dp[m][n] = uniquePaths(m - 1, n) + uniquePaths(m, n - 1);
}

int main() {
  int m = 3, n = 7;
  dp.assign(m + 1, vector<int>(n + 1, -1));
  cout << "Unique paths: " << uniquePaths(m, n) << endl;
  return 0;
}
