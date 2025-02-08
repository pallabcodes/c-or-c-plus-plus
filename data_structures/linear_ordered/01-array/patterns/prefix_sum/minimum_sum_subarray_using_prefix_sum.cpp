#include <climits>
#include <iostream>
#include <vector>

using namespace std;

int minSumSubarray(vector<int> &arr, int k) {
  int n = arr.size();
  vector<int> prefix = arr;

  for (int i = 1; i < n; i++)
    prefix[i] += prefix[i - 1];

  int minSum = INT_MAX;
  for (int i = k - 1; i < n; i++) {
    int sum = (i == k - 1) ? prefix[i] : prefix[i] - prefix[i - k];
    minSum = min(minSum, sum);
  }
  return minSum;
}

int main() {
  vector<int> arr = {3, -1, 2, 5, -3, 7};
  int k = 3;
  cout << "Min sum of subarray of size " << k << ": " << minSumSubarray(arr, k)
       << endl;
  return 0;
}
