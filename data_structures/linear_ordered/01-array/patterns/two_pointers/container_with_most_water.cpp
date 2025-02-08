#include <iostream>
#include <vector>
using namespace std;

int maxArea(vector<int> &height) {
  int left = 0, right = height.size() - 1, maxWater = 0;

  while (left < right) {
    int h = min(height[left], height[right]);
    maxWater = max(maxWater, h * (right - left));

    (height[left] < height[right]) ? left++ : right--;
  }
  return maxWater;
}

int main() {
  vector<int> heights = {1, 8, 6, 2, 5, 4, 8, 3, 7};
  cout << "Max Area: " << maxArea(heights) << endl;
  return 0;
}
