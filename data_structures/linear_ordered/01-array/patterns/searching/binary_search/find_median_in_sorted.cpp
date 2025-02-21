#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
  double findMedianSortedArrays(vector<int> &nums1, vector<int> &nums2) {
    if (nums1.size() > nums2.size())
      swap(nums1, nums2); // Ensure nums1 is smaller

    int x = nums1.size(), y = nums2.size();
    int left = 0, right = x;

    while (left <= right) {
      int partitionX = left + (right - left) / 2;
      int partitionY =
          (x + y + 1) / 2 - partitionX; // Ensures balanced partition

      int maxLeftX = (partitionX == 0) ? INT_MIN : nums1[partitionX - 1];
      int minRightX = (partitionX == x) ? INT_MAX : nums1[partitionX];

      int maxLeftY = (partitionY == 0) ? INT_MIN : nums2[partitionY - 1];
      int minRightY = (partitionY == y) ? INT_MAX : nums2[partitionY];

      if (maxLeftX <= minRightY && maxLeftY <= minRightX) {
        // Found correct partition
        if ((x + y) % 2 == 0) {
          return (max(maxLeftX, maxLeftY) + min(minRightX, minRightY)) / 2.0;
        } else {
          return max(maxLeftX, maxLeftY);
        }
      } else if (maxLeftX > minRightY) {
        right = partitionX - 1; // Move left
      } else {
        left = partitionX + 1; // Move right
      }
    }

    throw invalid_argument(
        "Input arrays are not sorted!"); // Should never reach here
  }
};

// Driver Code
int main() {
  Solution solution;
  vector<int> nums1 = {1, 3};
  vector<int> nums2 = {2};

  cout << fixed << setprecision(5)
       << "Median: " << solution.findMedianSortedArrays(nums1, nums2) << endl;
  return 0;
}
