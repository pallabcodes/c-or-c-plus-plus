#include <iostream>
#include <vector>

using namespace std;

vector<int> computePrefixSum(vector<int> &arr) {
  int n = arr.size();
  vector<int> prefix(n);
  prefix[0] = arr[0];

  for (int i = 1; i < n; i++) {
    prefix[i] = prefix[i - 1] + arr[i];
  }
  return prefix;
}

int main() {
  vector<int> arr = {2, 4, 6, 8, 10};
  vector<int> prefix = computePrefixSum(arr);

  for (int sum : prefix) {
    cout << sum << " ";
  }
  return 0;
}
