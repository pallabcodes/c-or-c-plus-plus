// int kthSmallest(vector<vector<int>> &matrix, int k) {
//   int left = matrix[0][0], right = matrix.back().back();
//   while (left < right) {
//     int mid = left + (right - left) / 2, count = 0;
//     for (int i = 0; i < matrix.size(); i++)
//       count += upper_bound(matrix[i].begin(), matrix[i].end(), mid) -
//                matrix[i].begin();
//     if (count < k)
//       left = mid + 1;
//     else
//       right = mid;
//   }
//   return left;
// }
