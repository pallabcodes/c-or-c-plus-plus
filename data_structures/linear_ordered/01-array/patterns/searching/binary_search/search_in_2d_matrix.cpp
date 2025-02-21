#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
  bool searchMatrix(vector<vector<int>> &matrix, int target) {
    if (matrix.empty() || matrix[0].empty())
      return false;

    int rows = matrix.size(), cols = matrix[0].size();
    int left = 0, right = rows * cols - 1;

    while (left <= right) {
      int mid = left + (right - left) / 2;    // Avoid overflow
      int row = mid / cols, col = mid % cols; // Convert 1D index to 2D

      if (matrix[row][col] == target)
        return true;
      if (matrix[row][col] < target)
        left = mid + 1;
      else
        right = mid - 1;
    }

    return false; // Not found
  }
};

// Driver Code
int main() {
  Solution solution;
  vector<vector<int>> matrix = {
      {1, 3, 5, 7}, {10, 11, 16, 20}, {23, 30, 34, 60}};
  int target = 3;

  cout << (solution.searchMatrix(matrix, target) ? "Found" : "Not Found")
       << endl;
  return 0;
}
