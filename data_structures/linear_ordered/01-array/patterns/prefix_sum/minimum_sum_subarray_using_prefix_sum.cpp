#include <climits> // For INT_MAX
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
    int sum = (i == k - 1) ? prefix[i] : prefix[i] - prefix[i - k]; // formula
    minSum = min(minSum, sum);
  }

  return minSum;
}

int findMinKSum(const vector<int> &nums, int k) {
  int n = nums.size();
  if (k > n)
    return -1;

  // Step 1: compute the prefix sum
  vector<int> prefix(n, 0); // creates vector of size n and fills it with 0

  prefix[0] = nums[0];

  for (int i = 1; i < n; i++) {
    prefix[i] = prefix[i] + prefix[i - 1];
  }

  // Initialize minSum with first k elements sum

  // N.B: prefix[k-1] below directly gives the sum of the first k elements

  int minSum = prefix[k - 1];

  // Step 2: Iterate from index k onwards, updating minSum

  for (int i = k; k < n; i++) {
    int currSum = prefix[i] - prefix[i - k]; // formula
    minSum = min(minSum, currSum);
  }

  return minSum;
}

int main() {
  vector<int> arr = {3, -1, 2, 5, -3, 7};
  int k = 3;
  cout << "Min sum of subarray of window size k for " << k << ": "
       << minSumSubarray(arr, k) << endl;

  cout << "Min sum of subarray of window size k for " << k << ": "
       << findMinKSum(arr, k) << endl;
  return 0;
}
