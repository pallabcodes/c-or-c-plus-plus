#include <bits/stdc++.h>
using namespace std;

bool isPossible(vector<int> &books, int students, int maxPages) {
  int studentCount = 1, sum = 0;

  for (int pages : books) {
    if (pages > maxPages)
      return false; // A single book exceeds maxPages, impossible case

    if (sum + pages > maxPages) {
      studentCount++; // Assign new student
      sum = pages;
      if (studentCount > students) {
        return false; // More students needed than available
      }
    } else {
      sum += pages;
    }
  }

  return true;
}

int minPages(vector<int> &books, int students) {
  if (students > books.size())
    return -1; // If students > books, impossible case

  int left = *max_element(books.begin(), books.end()); // Min possible max pages
  int right = accumulate(books.begin(), books.end(), 0); // Sum of all pages

  while (left < right) {
    // step 1: find the middle
    int mid = left + (right - left) / 2;
    // step 2: decrease the search range
    if (isPossible(books, students, mid))
      right = mid; // Try for a smaller max pages
    else
      left = mid + 1; // Increase max pages limit
  }

  // The answer is always at left (or right, since they are equal at the end)
  return left; // return right;
}

int main() {
  vector<int> books = {12, 34, 67, 90};
  int students = 2;

  int result = minPages(books, students);
  if (result != -1)
    cout << "Minimum max pages assigned to a student: " << result << endl;
  else
    cout << "Not possible to allocate books." << endl;

  return 0;
}

