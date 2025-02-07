#include <iostream>
using namespace std;

void findMinMax(int arr[], int n, int &min, int &max) {
  // Iniitally, min and max could be either first element
  min = max = arr[0];

  for (int i = 1; i < n; i++) {

    if (arr[i] > max) {
      max = arr[i];
    }

    if (arr[i] < min) {
      min = arr[i];
    }
  }
}

int main() {
  int arr[] = {7, 2, 10, 4, 9};
  int n = 5, min, max;

  // ✔ Time Complexity: O(n) (single pass, space complexity 0(1)
  findMinMax(arr, n, min, max);
  cout << "Min: " << min << ", Max: " << max;
  return 0;
}
