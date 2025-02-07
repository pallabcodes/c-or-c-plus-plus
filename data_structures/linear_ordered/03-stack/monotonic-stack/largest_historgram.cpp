#include <iostream>
#include <stack>
#include <vector>

using namespace std;

int largestRectangleArea(vector<int> &heights) {
  stack<int> s;
  int maxArea = 0, n = heights.size();

  for (int i = 0; i <= n; i++) {
    while (!s.empty() && (i == n || heights[s.top()] >= heights[i])) {
      int height = heights[s.top()];
      s.pop();
      int width = s.empty() ? i : i - s.top() - 1;
      maxArea = max(maxArea, height * width);
    }
    s.push(i);
  }
  return maxArea;
}

int main() {

  vector<int> nums = {2, 1, 5, 6, 2, 3};
  int result = largestRectangleArea(nums);

  cout << result << endl;

  return 0;
}