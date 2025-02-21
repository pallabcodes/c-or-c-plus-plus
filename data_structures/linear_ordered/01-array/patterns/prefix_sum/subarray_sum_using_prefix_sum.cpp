#include <iostream>
#include <vector>

using namespace std;

vector<int> computePrefixSum(vector<int> &arr) {
  int n = arr.size();
  vector<int> prefix(n);
  prefix[0] = arr[0];

  // creating the prefix sum
  for (int i = 1; i < n; i++) {
    prefix[i] = prefix[i - 1] + arr[i];
  }

  return prefix;
}

int rangeSum(vector<int> &prefix, int L, int R) {
  return (L == 0) ? prefix[R] : prefix[R] - prefix[L - 1];
}

int main() {
  vector<int> arr = {2, 4, 6, 8, 10};
  vector<int> prefix = computePrefixSum(arr); // Compute prefix sum

  cout << "Sum(1, 3): " << rangeSum(prefix, 1, 3) << endl; // Output: 18
  cout << "Sum(2, 4): " << rangeSum(prefix, 2, 4) << endl; // Output: 24
  return 0;
}
