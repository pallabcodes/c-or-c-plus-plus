#include <iostream>

using namespace std;

// leftRotate i.e. anti-clockwise
void leftRotateByOne(int arr[], int n) {
  int temp = arr[0];

  for (int i = 0; i < n - 1; i++) {
    arr[i] = arr[i + 1];
  }

  arr[n - 1] = temp;
}

int main() {
  int arr[] = {1, 2, 3, 4, 5};
  int n = 5;

  leftRotateByOne(arr, n); // T = O(n)

  for (int i = 0; i < n; i++) {
    cout << arr[i] << " ";
  }

  return 0;
}