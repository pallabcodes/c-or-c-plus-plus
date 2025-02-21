#include <bits/stdc++.h>
using namespace std;

int kthSmallest(vector<vector<int>> &matrix, int k) {
  if (matrix.empty() || matrix[0].empty())
    return -1; // Handle empty matrix edge case

  int left = matrix[0][0], right = matrix.back().back();

  while (left < right) {
    int mid = left + (right - left) / 2;
    int count = 0;

    // Count how many numbers in the matrix are <= mid
    for (int i = 0; i < matrix.size(); i++) {
      count += upper_bound(matrix[i].begin(), matrix[i].end(), mid) -
               matrix[i].begin();
    }

    // If count is less than k, search in the right half
    if (count < k)
      left = mid + 1;
    else
      right = mid;
  }

  return left; // When left == right, we've found the kth smallest element
}

int main() {
  vector<vector<int>> matrix = {{1, 5, 9}, {10, 11, 13}, {12, 13, 15}};
  int k = 8;

  int result = kthSmallest(matrix, k);
  if (result != -1)
    cout << "The " << k << "th smallest element is: " << result << endl;
  else
    cout << "Invalid input." << endl;

  return 0;
}
