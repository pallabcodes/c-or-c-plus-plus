#include <deque>    // For deque (Double-ended queue)
#include <iostream> // For input/output
#include <vector>   // For vector storage
using namespace std;

// Function to find the maximum in each sliding window of size 'k'
vector<int> slidingWindowMax(const vector<int> &nums, int k) {
  vector<int> result; // Stores the final result
  deque<int> dq;      // Stores **indices** of useful elements (not values)

  for (int i = 0; i < nums.size(); i++) {
    // Result doesn't need to be added when i < k - 1 so just add index to dq

    // I know, dq stores indices, so I can check at any iteration whether the
    // front is outta its window range or not

    // Step 1: check whether at the current iteration the front is valid or not,
    // if not then pop_front as below at i = 3, i - k + 1 = 3 - 3 + 1 = 4 - 3 =
    // 1, at present dq.front() i.e. 1 so remove it
    if (!dq.empty() && dq.front() == i - k) {
      dq.pop_front();
    }

    // Step 2: Remove elements **smaller than nums[i]** from the back
    while (!dq.empty() && nums[dq.back()] <= nums[i]) {
      dq.pop_back();
    }

    // Step 3: Push the index of current element to deque
    dq.push_back(i);

    // Step 4: Record the max value when the first window is filled (i >= k - 1)
    if (i >= k - 1) {
      result.push_back(nums[dq.front()]); // Front of the deque is the max
    }
  }

  return result; // Return final result
}

// Driver code
int main() {
  vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
  int k = 3;
  vector<int> result = slidingWindowMax(nums, k);

  cout << "Sliding window maximums: ";
  for (int num : result)
    cout << num << " "; // Output: 3 3 5 5 6 7
  return 0;
}
