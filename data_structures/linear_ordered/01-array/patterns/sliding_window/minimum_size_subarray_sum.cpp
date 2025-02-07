#include <climits>
#include <iostream>
#include <vector>
using namespace std;

// Problem: Given an array nums and a target sum, find the length of the
// smallest contiguous subarray whose sum is greater than or equal to target.

int minSubarrayLen(int target, const vector<int> &nums) {
  int left = 0, sum = 0, minLen = INT_MAX;

  for (int right = 0; right < nums.size(); right++) {
    sum += nums[right]; // Expand window
    while (sum >= target) {
      minLen = min(minLen, right - left + 1); // Track minimum length
      sum -= nums[left];                      // Contract window from left
      left++;
    }
  }

  return minLen == INT_MAX ? 0 : minLen;
}

// ✅ Passed by reference (can modify)
void modifyVector(vector<int> &nums) {
  nums[0] = 100; // Modifies original vector
}

// ✅ Passed by const reference (read-only)
void printVector(const vector<int> &nums) {
  for (int num : nums) {
    cout << num << " ";
  }
  cout << endl;
}

int main() {
  vector<int> nums = {2, 3, 1, 2, 4, 3};
  int target = 7;

  cout << "Minimum subarray length: " << minSubarrayLen(target, nums)
       << endl; // Output: 2

  // modifyVector(nums);
  // printVector(nums);

  return 0;
}
