#include <iostream>

using namespace std;

void reverseArray(int arr[], int n) {
  int left = 0, right = n - 1;

  // while condition could left != right or left < right
  while (left < right) {
    swap(arr[left], arr[right]);
    left += 1;
    right -= 1;
  }
}

int main() {
  int arr[] = {1, 2, 3, 4, 5};
  int n = 5;

  reverseArray(arr, 5); // T : O(n), S = O(1)

  for (int i = 0; i < n; i++) {
    cout << "Element " << arr[i] << " ";
  }

  return 0;
}