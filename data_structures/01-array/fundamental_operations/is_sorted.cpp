#include <iostream>

using namespace std;

bool isSortedAsc(int arr[], int n) {
  for (int i = 0; i < n - 1; i++) {
    if (arr[i] > arr[i + 1])
      return false;
  }
  return true;
}

bool isSortedDesc(int arr[], int n) {

  for (int i = n - 1; i > 0; i--) {
    if (arr[i] > arr[i - 1]) {
      return false;
    }
  }

  return true;
}

int main() {
  int arr[] = {1, 2, 3, 4, 5};
  cout << isSortedAsc(arr, 5) << endl; // T = O(n)

  int descArr[] = {5, 4, 3, 2, 1};

  cout << isSortedDesc(arr, 5) << endl; // T = O(n)

  // i can use an additional parameter then won't need to use 2 methods

  // or, could use a single method named `isSorted` with overloading too

  return 0;
}
