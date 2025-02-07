// Problem: Given an array of integers and a number k, find the maximum sum of a
// subarray of size k.

#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

// Brute force approach would look like this (O(N*K))
// Inefficient for large arrays (N ≈ 10^5).
int maxSumSubarrayK_BruteForce(const vector<int> &arr, int k) {
  int maxSum = 0;
  for (int i = 0; i <= arr.size() - k; i++) {
    int sum = 0;
    for (int j = i; j < i + k; j++) {
      sum += arr[j];
    }
    maxSum = max(maxSum, sum);
  }
  return maxSum;
}

// Find the Maximum sum of a window size of k
int findMaxSumSubarray(const vector<int> &arr, int k) {
  if (arr.size() < k)
    return -1; // Edge case: If array size < k, return invalid result

  int windowSum = 0, maxSum = 0;

  // Compute sum of first window
  for (int i = 0; i < k; i++) {
    windowSum += arr[i];
  }

  maxSum = windowSum;

  // Slide window across the array
  for (int i = k; i < arr.size(); i++) {
    windowSum += arr[i] - arr[i - k]; // Slide window: Add new, remove old
    maxSum = max(maxSum, windowSum);
  }

  return maxSum;
}

int findMaxSumSubarrayV2(const vector<int> &arr, int k) {
  if (arr.size() < k) {
    return -1;
  }

  int windowSum = 0, maxSum = 0, left = 0;

  for (int right = 0; right < arr.size(); right++) {
    windowSum += arr[right];

    if (right >= k - 1) { // When we have `k` elements
      maxSum = max(maxSum, windowSum);
      windowSum -= arr[left]; // Remove the left element
      left++;                 // Slide window
    }
  }

  return maxSum;
}

int main() {
  vector<int> arr = {2, 1, 5, 1, 3, 2};
  int k = 3;

  cout << "Maximum sum of subarray of size " << k << ": "
       << findMaxSumSubarray(arr, k) << endl;

  return 0;
}
