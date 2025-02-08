#include <iostream>
#include <vector>

using namespace std;

bool searchMatrix(vector<vector<int>> &matrix, int target) {
  int row = 0, col = matrix[0].size() - 1;

  while (row < matrix.size() && col >= 0) {
    if (matrix[row][col] == target)
      return true;
    else if (matrix[row][col] > target)
      col--;
    else
      row++;
  }
  return false;
}

int main() {
  vector<vector<int>> matrix = {
      {1, 4, 7, 11}, {2, 5, 8, 12}, {3, 6, 9, 13}, {10, 14, 15, 16}};

  cout << "Element Found: " << searchMatrix(matrix, 9);

  return 0;
}
