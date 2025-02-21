#include <bits/stdc++.h>
using namespace std;

int searchRotatedArray(vector<int> &nums, int target) {
  if (nums.empty())
    return -1; // Handle empty array edge case

  int left = 0, right = nums.size() - 1;

  while (left <= right) {
    int mid = left + (right - left) / 2;

    if (nums[mid] == target)
      return mid; // Target found

    // Determine which half is sorted
    if (nums[left] <= nums[mid]) { // Left half is sorted
      if (nums[left] <= target && target < nums[mid])
        right = mid - 1; // Search in left half
      else
        left = mid + 1; // Search in right half
    } else {            // Right half is sorted
      if (nums[mid] < target && target <= nums[right])
        left = mid + 1; // Search in right half
      else
        right = mid - 1; // Search in left half
    }
  }

  return -1; // Target not found
}

int main() {
  vector<int> nums = {4, 5, 6, 7, 0, 1, 2};
  int target = 0;

  int index = searchRotatedArray(nums, target);
  if (index != -1)
    cout << "Target " << target << " found at index " << index << endl;
  else
    cout << "Target " << target << " not found in the array." << endl;

  return 0;
}
