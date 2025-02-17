#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> fourSum(vector<int> &nums, int target) {
  vector<vector<int>> result;
  sort(nums.begin(), nums.end());

  int n = nums.size();
  for (int i = 0; i < n - 3; i++) {
    if (i > 0 && nums[i] == nums[i - 1])
      continue; // Skip duplicates

    for (int j = i + 1; j < n - 2; j++) {
      if (j > i + 1 && nums[j] == nums[j - 1])
        continue; // Skip duplicates

      int left = j + 1, right = n - 1;
      while (left < right) {
        long sum = (long)nums[i] + nums[j] + nums[left] + nums[right];
        if (sum == target) {
          result.push_back({nums[i], nums[j], nums[left], nums[right]});

          while (left < right && nums[left] == nums[left + 1])
            left++; // Skip duplicates
          while (left < right && nums[right] == nums[right - 1])
            right--; // Skip duplicates

          left++, right--;
        } else if (sum < target)
          left++;
        else
          right--;
      }
    }
  }
  return result;
}

int main() {
  vector<int> arr = {1, 0, -1, 0, -2, 2};
  int target = 0;
  vector<vector<int>> res = fourSum(arr, target);

  for (auto quad : res) {
    cout << "[" << quad[0] << ", " << quad[1] << ", " << quad[2] << ", "
         << quad[3] << "]\n";
  }
  return 0;
}
