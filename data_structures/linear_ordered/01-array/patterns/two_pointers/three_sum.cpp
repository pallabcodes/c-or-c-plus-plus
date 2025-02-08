#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> threeSum(vector<int> &nums) {
  vector<vector<int>> result;
  sort(nums.begin(), nums.end());

  for (int i = 0; i < nums.size() - 2; i++) {
    if (i > 0 && nums[i] == nums[i - 1])
      continue; // Avoid duplicate triplets

    int left = i + 1, right = nums.size() - 1;
    while (left < right) {
      int sum = nums[i] + nums[left] + nums[right];
      if (sum == 0) {
        result.push_back({nums[i], nums[left], nums[right]});
        while (left < right && nums[left] == nums[left + 1])
          left++; // Skip duplicates
        while (left < right && nums[right] == nums[right - 1])
          right--; // Skip duplicates
        left++, right--;
      } else if (sum < 0)
        left++;
      else
        right--;
    }
  }
  return result;
}

int main() {
  vector<int> arr = {-4, -1, -1, 0, 1, 2};
  vector<vector<int>> res = threeSum(arr);

  for (auto triplet : res) {
    cout << "[" << triplet[0] << ", " << triplet[1] << ", " << triplet[2] << "]"
         << endl;
  }
  return 0;
}
