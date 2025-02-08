#include <iostream>
#include <unordered_map>
using namespace std;

unordered_map<int, int> memo;

int expensiveFunction(int n) {
  if (n <= 1)
    return n;

  if (memo.count(n))
    return memo[n]; // Check if result is stored

  return memo[n] = expensiveFunction(n - 1) + expensiveFunction(n - 2);
}

int main() {
  cout << "Result: " << expensiveFunction(50) << endl;
  return 0;
}
