// bool isPossible(vector<int> &books, int students, int maxPages) {
//   int studentCount = 1, sum = 0;
//   for (int pages : books) {
//     if (pages > maxPages)
//       return false;
//     if (sum + pages > maxPages) {
//       studentCount++;
//       sum = pages;
//       if (studentCount > students)
//         return false;
//     } else
//       sum += pages;
//   }
//   return true;
// }

// int minPages(vector<int> &books, int students) {
//   int left = *max_element(books.begin(), books.end()),
//       right = accumulate(books.begin(), books.end(), 0);
//   while (left < right) {
//     int mid = left + (right - left) / 2;
//     if (isPossible(books, students, mid))
//       right = mid;
//     else
//       left = mid + 1;
//   }
//   return left;
// }
