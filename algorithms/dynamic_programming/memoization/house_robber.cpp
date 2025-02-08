#include <iostream>
#include <vector>
using namespace std;

vector<int> dp;

int rob(vector<int> &nums, int i) {
  if (i < 0)
    return 0;
  if (dp[i] != -1)
    return dp[i];

  return dp[i] = max(rob(nums, i - 1), nums[i] + rob(nums, i - 2));
}

int main() {
  vector<int> nums = {2, 7, 9, 3, 1};
  dp.resize(nums.size(), -1);
  cout << "Max money robbed: " << rob(nums, nums.size() - 1) << endl;
  return 0;
}
