#include <bits/stdc++.h>
using namespace std;

int findPeakElement(vector<int> &nums) {
  int n = nums.size();
  if (n == 1)
    return 0; // Single element is always a peak

  int left = 0, right = n - 1;

  while (left < right) {
    int mid = left + (right - left) / 2;

    // If mid is greater than the next element, peak lies on the left side
    if (nums[mid] > nums[mid + 1]) {
      right = mid;
    } else {
      // Else peak lies on the right side
      left = mid + 1;
    }
  }

  return left; // Left and right converge to the peak element index
}

int main() {
  vector<int> nums = {1, 2, 3, 1};
  cout << "Peak Element Index: " << findPeakElement(nums) << endl;
  return 0;
}
