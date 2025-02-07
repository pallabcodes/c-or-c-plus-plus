#include <iostream>
#include <stack>
#include <vector>

using namespace std;

// Trapping the Rain Water (using Monotonic Decreasing Stack)

// Problem: Given an array of heights, calculare how much water can be trapped
// between buildings after raining

int trap(vector<int> &height) {
  stack<int> s;
  int water = 0, n = height.size();

  for (int i = 0; i < n; i++) {
    while (!s.empty() && height[i] > height[s.top()]) {
      int top = s.top();
      s.pop();
      if (s.empty())
        break;

      int distance = i - s.top() - 1;
      int boundedHeight = min(height[i], height[s.top()]) - height[top];
      water += distance * boundedHeight;
    }
    s.push(i);
  }
  return water;
}

int main() {
  vector<int> heights = {};
  int result = trap(heights);

  cout << result << endl;

  return 0;
}