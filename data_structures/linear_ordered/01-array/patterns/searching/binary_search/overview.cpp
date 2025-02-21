#include <bits/stdc++.h> // includes all standard c++ libraries
using namespace std;

int binarySearch(vector<int> &arr, int target) {
  int left = 0, right = arr.size() - 1;

  while (left <= right) {
    int mid = left + (right - left) / 2; // Prevents overflow

    if (arr[mid] == target)
      return mid; // Target found
    else if (arr[mid] < target)
      left = mid + 1; // Search right half
    else
      right = mid - 1; // Search left half
  }

  return -1; // Target not found
}

int main() {
  vector<int> arr = {1, 3, 5, 7, 9, 11};
  int target = 7;

  int index = binarySearch(arr, target);
  if (index != -1)
    cout << "Element found at index: " << index << endl;
  else
    cout << "Element not found." << endl;

  return 0;
}
