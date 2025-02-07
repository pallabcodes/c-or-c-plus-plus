#include <iostream>
using namespace std;

void deleteElement(int arr[], int &n, int index) {
  if (index < 0 || index >= n) {
    cout << "Invalid index\n";
    return;
  }

  for (int i = index; i < n - 1; i++) {
    arr[i] = arr[i + 1]; // Shift left
  }

  n--; // Reduce size
}

int main() {
  int arr[10] = {1, 2, 3, 4, 5};
  int n = 5; // total no. of elements within arr

  deleteElement(arr, n, 2);

  for (int i = 0; i < n; i++) {
    cout << arr[i] << " ";
  }

  return 0;
}
