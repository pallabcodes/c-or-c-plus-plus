// vector<int> firstAndLast(vector<int> &nums, int target) {
//   int first = -1, last = -1;
//   int left = 0, right = nums.size() - 1;

//   // Find first occurrence
//   while (left <= right) {
//     int mid = left + (right - left) / 2;
//     if (nums[mid] >= target)
//       right = mid - 1;
//     else
//       left = mid + 1;
//     if (nums[mid] == target)
//       first = mid;
//   }

//   left = 0, right = nums.size() - 1;
//   // Find last occurrence
//   while (left <= right) {
//     int mid = left + (right - left) / 2;
//     if (nums[mid] <= target)
//       left = mid + 1;
//     else
//       right = mid - 1;
//     if (nums[mid] == target)
//       last = mid;
//   }

//   return {first, last};
// }
