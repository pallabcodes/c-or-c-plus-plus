#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

int subarraySum(vector<int> &nums, int k) {
  unordered_map<int, int> prefixSumCount;
  prefixSumCount[0] = 1;

  int sum = 0, count = 0;
  for (int num : nums) {
    sum += num;
    if (prefixSumCount.count(sum - k))
      count += prefixSumCount[sum - k];
    prefixSumCount[sum]++;
  }

  return count;
}

int main() {
  vector<int> nums = {1, 1, 1};
  cout << subarraySum(nums, 2) << endl; // Output: 2
  return 0;
}
