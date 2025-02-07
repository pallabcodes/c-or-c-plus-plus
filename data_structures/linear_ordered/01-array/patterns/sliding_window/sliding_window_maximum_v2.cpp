#include <deque>
#include <iostream>
#include <vector>

using namespace std;

vector<int> maxSlidingWindow(vector<int> &nums, int k) {
  deque<int> dq; // Stores indices of elements in decreasing order
  vector<int> result;

  for (int i = 0; i < nums.size(); i++) {
    // Remove elements from front if they are out of window bounds
    if (!dq.empty() && dq.front() < i - k + 1) {
      dq.pop_front();
    }

    // Maintain decreasing order: Remove elements from back if they are smaller
    // than nums[i]
    while (!dq.empty() && nums[dq.back()] < nums[i]) {
      dq.pop_back();
    }

    // Add current element index to deque
    dq.push_back(i);

    // Store the maximum of the window when we have at least k elements
    // processed
    if (i >= k - 1) {
      result.push_back(nums[dq.front()]);
    }
  }

  return result;
}

int main() {
  vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
  int k = 3;
  vector<int> result = maxSlidingWindow(nums, k);

  for (int num : result) {
    cout << num << " ";
  }

  return 0;
}
