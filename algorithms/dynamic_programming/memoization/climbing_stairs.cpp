#include <iostream>
#include <vector>
using namespace std;

vector<int> dp;

int climbStairs(int n) {
  if (n <= 2)
    return n;

  if (dp[n] != -1)
    return dp[n];

  return dp[n] = climbStairs(n - 1) + climbStairs(n - 2);
}

int main() {
  int n = 5;
  dp.resize(n + 1, -1);
  cout << "Ways to climb " << n << " stairs: " << climbStairs(n) << endl;
  return 0;
}
