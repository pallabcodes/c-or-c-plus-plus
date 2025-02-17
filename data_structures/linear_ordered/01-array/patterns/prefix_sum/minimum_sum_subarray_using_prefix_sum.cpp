#include <climits>
#include <iostream>
#include <vector>

using namespace std;

int minSumSubarray(vector<int> &arr, int k) {
  int n = arr.size();

  // Ste 1: copy the original array
  vector<int> prefix = arr;

  // Step 2 : complte the preix sum
  for (int i = 1; i < n; i++) {
    prefix[i] += prefix[i - 1];
  }

  int minSum = INT_MAX;

  // Step 3: compute minimum subaray sum of size k
  for (int i = k - 1; i < n; i++) {
    int sum = (i == k - 1) ? prefix[i] : prefix[i] - prefix[i - k];
    minSum = min(minSum, sum);
  }

  return minSum;
}

int main() {
  vector<int> arr = {3, -1, 2, 5, -3, 7};
  int k = 3;
  cout << "Min sum of subarray of window size k for " << k << ": "
       << minSumSubarray(arr, k) << endl;
  return 0;
}
