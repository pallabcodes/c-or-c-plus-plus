#include <iostream>
#include <stack>
#include <vector>

using namespace std;

// Problem: Given an array, find the sum of the minimum element in every
// subarray

// Solve (using Montonic Increasing Stack)

int sumSubarrayMins(vector<int> &arr) {
  stack<int> s;
  int n = arr.size();
  vector<int> left(n), right(n);

  for (int i = 0; i < n; i++) {
    while (!s.empty() && arr[s.top()] > arr[i])
      s.pop();
    left[i] = s.empty() ? i + 1 : i - s.top();
    s.push(i);
  }

  while (!s.empty())
    s.pop();

  for (int i = n - 1; i >= 0; i--) {
    while (!s.empty() && arr[s.top()] >= arr[i])
      s.pop();
    right[i] = s.empty() ? n - i : s.top() - i;
    s.push(i);
  }

  long result = 0, MOD = 1e9 + 7;
  for (int i = 0; i < n; i++)
    result = (result + (long)arr[i] * left[i] * right[i]) % MOD;
  return result;
}

int main() { return 0; }