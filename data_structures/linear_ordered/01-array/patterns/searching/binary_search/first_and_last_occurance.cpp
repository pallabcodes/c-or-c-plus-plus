#include <bits/stdc++.h>
using namespace std;

vector<int> findFirstAndLast(vector<int> &nums, int target) {
  int first = -1, last = -1;
  int left = 0, right = nums.size() - 1;

  // Find first occurrence
  while (left <= right) {
    int mid = left + (right - left) / 2;
    if (nums[mid] >= target)
      right = mid - 1;
    else
      left = mid + 1;

    if (nums[mid] == target)
      first = mid;
  }

  left = 0, right = nums.size() - 1;

  // Find last occurrence
  while (left <= right) {
    int mid = left + (right - left) / 2;
    if (nums[mid] <= target)
      left = mid + 1;
    else
      right = mid - 1;

    if (nums[mid] == target)
      last = mid;
  }

  return {first, last};
}

int main() {
  vector<int> nums = {5, 7, 7, 8, 8, 10};
  int target = 8;

  vector<int> result = findFirstAndLast(nums, target);
  cout << "First occurrence: " << result[0]
       << ", Last occurrence: " << result[1] << endl;

  return 0;
}

// https://www.youtube.com/watch?v=oZerk5iKh18
// https://www.youtube.com/watch?v=w5LYB2u9-ho

// https://www.youtube.com/watch?v=aA9V-sMsmJo
// https://www.youtube.com/watch?v=lToTMeemSmE

// https://www.youtube.com/watch?v=ju-AwFMzqaU
// https://www.youtube.com/watch?v=nXyPLFQeddA

// https://www.youtube.com/watch?v=zwzrt_3JUxU
// https://www.youtube.com/watch?v=9tDkkKDQWlg

// https://www.youtube.com/watch?v=eFcdH93oFD8
// https://www.youtube.com/watch?v=_86WEtX9GCU

// https: // www.youtube.com/watch?v=QUwLguVG_uQ
// https://www.youtube.com/watch?v=ua7Y7cgm1Cg