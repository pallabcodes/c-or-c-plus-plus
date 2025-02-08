#include <iostream>
#include <vector>

using namespace std;

void printRowMajor(vector<vector<int>> &matrix) {
  for (auto row : matrix) {
    for (int val : row) {
      cout << val << " ";
    }
  }
}

void printColumnMajor(vector<vector<int>> &matrix) {
  int rows = matrix.size(), cols = matrix[0].size();
  for (int col = 0; col < cols; col++) {
    for (int row = 0; row < rows; row++) {
      cout << matrix[row][col] << " ";
    }
  }
}

int main() {
  vector<vector<int>> matrix = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

  cout << "Row Major: ";
  printRowMajor(matrix);

  cout << "\nColumn Major: ";
  printColumnMajor(matrix);

  return 0;
}
