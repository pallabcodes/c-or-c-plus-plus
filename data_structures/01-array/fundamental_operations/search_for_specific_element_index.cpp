#include <iostream>

using namespace std;

int searchElement(int arr[], int n, int x) {
  if (n == 0) {
    cout << "Empty Array\n";
    return -1;
  }

  for (int i = 0; i < n; i++) {
    if (arr[i] == x)
      return i;
  }

  return -1;
}

int main() {
  int arr[] = {1, 2, 3, 4, 5};
  int n = 5;

  cout << "Index of 3: " << searchElement(arr, n, 3);
  return 0;
}