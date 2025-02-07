#include <iostream>

using namespace std;

// Insert an element at a specific index

void insertElement(int arr[], int &n, int index, int value) {

  // n no. of elements in the arr
  if (index < 0 || index > n) {
    cout << "Invalid index\n";
    return;
  }

  // need to use n (0, 1, 2, 3, 4, 5 ) thus 6 elements but using n - 1 won't
  // allow to shift

  for (int i = n; i > index; i--) {
    arr[i] = arr[i - 1];
  }

  arr[index] = value;
  n++; // increased the array size
}

int main() {
  int arr[10] = {1, 2, 3, 4, 5};
  int n = 5; // total no. of elements present in arr is 5

  insertElement(arr, n, 2, 99);

  // expected output = { 1, 2, 99, 3, 4, 5 }

  // so, no. of elements prsent now 6

  for (int i = 0; i < n; i++) {
    cout << arr[i] << " ";
  }

  return 0;
}