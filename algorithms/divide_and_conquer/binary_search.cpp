// int binarySearch(vector<int> &arr, int left, int right, int target) {
//   if (left > right)
//     return -1;
//   int mid = left + (right - left) / 2;
//   if (arr[mid] == target)
//     return mid;
//   return (arr[mid] > target) ? binarySearch(arr, left, mid - 1, target)
//                              : binarySearch(arr, mid + 1, right, target);
// }
