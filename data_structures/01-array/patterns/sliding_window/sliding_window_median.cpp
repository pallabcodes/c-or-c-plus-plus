#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
using namespace std;

class SlidingWindowMedian {
private:
  priority_queue<int> low; // Max heap for the smaller half
  priority_queue<int, vector<int>, greater<int>>
      high; // Min heap for the larger half
  unordered_map<int, int>
      delayedRemovals; // Stores elements to be removed later

  // Remove invalid elements that are marked for deletion
  template <typename T> void cleanHeap(T &heap) {
    while (!heap.empty() && delayedRemovals[heap.top()] > 0) {
      delayedRemovals[heap.top()]--;
      heap.pop();
    }
  }

  // Balance the heaps
  void balanceHeaps() {
    while (low.size() > high.size() + 1) {
      high.push(low.top());
      low.pop();
    }
    while (high.size() > low.size() + 1) {
      low.push(high.top());
      high.pop();
    }
  }

  // Get the median value
  double getMedian() {
    if (low.size() == high.size()) {
      return ((double)low.top() + high.top()) / 2.0;
    }
    return low.size() > high.size() ? low.top() : high.top();
  }

public:
  vector<double> medianSlidingWindow(vector<int> &nums, int k) {
    vector<double> result;

    for (int i = 0; i < nums.size(); i++) {
      // Insert new number
      if (low.empty() || nums[i] <= low.top()) {
        low.push(nums[i]);
      } else {
        high.push(nums[i]);
      }

      // Remove outdated element from the window
      if (i >= k) {
        int outNum = nums[i - k];
        delayedRemovals[outNum]++;

        // Clean up both heaps
        cleanHeap(low);
        cleanHeap(high);
      }

      // Balance heaps
      balanceHeaps();

      // Store median when the window is full
      if (i >= k - 1) {
        result.push_back(getMedian());
      }
    }

    return result;
  }
};

int main() {
  vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
  int k = 3;
  SlidingWindowMedian solver;
  vector<double> result = solver.medianSlidingWindow(nums, k);

  cout << "Sliding Window Medians: ";
  for (double median : result) {
    cout << median << " ";
  }
  cout << endl;

  return 0;
}