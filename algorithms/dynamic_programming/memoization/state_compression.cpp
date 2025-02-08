#include <iostream>
using namespace std;

int fibonacci(int n) {
  if (n <= 1)
    return n;

  int prev2 = 0, prev1 = 1, curr;
  for (int i = 2; i <= n; i++) {
    curr = prev1 + prev2;
    prev2 = prev1;
    prev1 = curr;
  }
  return curr;
}

int main() {
  cout << "Fibonacci(10) = " << fibonacci(10) << endl;
  return 0;
}
