// #scnario : Imagine needing to calculate something dynamic inside the window(e.g., sums, max, or more) while the boundaries(left and right) don’t always move at a regular pace.You could use two pointers that move with different speeds depending on conditions.

// This "hacky" approach allows one pointer to move slowly when certain conditions aren't met, then suddenly speed up once conditions are satisfied. It’s dynamic boundary adjustment.

#include <iostream>
#include <vector>
#include <climits>

// Question: Find the maximum sum subarray with sum at most x using sliding window with dynamic bounds.

int slidingWindowMaxSumAtMostX(const std::vector<int> &arr, int x)
{
  int n = arr.size();
  int maxSum = INT_MIN;
  int start = 0;
  int currentSum = 0;

  for (int end = 0; end < n; ++end)
  {
    currentSum += arr[end]; // Add the current element to the window.

    // While the sum exceeds 'x', shrink the window from the left
    while (currentSum > x && start <= end)
    {
      currentSum -= arr[start++];
    }

    // Update maxSum for valid windows
    if (currentSum <= x)
    {
      maxSum = std::max(maxSum, currentSum);
    }
  }

  return maxSum == INT_MIN ? -1 : maxSum; // Return -1 if no valid subarray found.
}

int main()
{
  std::vector<int> arr = {1, 2, 3, 4, 5};
  int x = 11;

  std::cout << "Max sum with sum <= " << x << ": " << slidingWindowMaxSumAtMostX(arr, x) << std::endl;

  return 0;
}
