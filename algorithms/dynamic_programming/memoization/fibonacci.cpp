#include <iostream>
#include <vector>
using namespace std;

vector<int> dp; // Memoization table

int fibonacci(int n) {
  if (n <= 1)
    return n; // Base case

  if (dp[n] != -1)
    return dp[n]; // Already computed, return stored result

  return dp[n] = fibonacci(n - 1) + fibonacci(n - 2); // Store & return
}

int main() {
  int n = 10;
  dp.resize(n + 1, -1); // Initialize dp array with -1
  cout << "Fibonacci(" << n << ") = " << fibonacci(n) << endl;
  return 0;
}
