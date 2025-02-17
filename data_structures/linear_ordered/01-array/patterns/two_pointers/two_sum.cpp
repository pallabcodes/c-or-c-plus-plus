#include <iostream>
using namespace std;

// Here, assumed the input to be always sorted
void pairSum(int arr[], int n, int target) {
  int left = 0, right = n - 1;

  while (left < right) {
    int sum = arr[left] + arr[right];

    if (sum == target) {
      cout << "Pair found: " << arr[left] << " + " << arr[right] << " = "
           << target << endl;
      return;
    }

    (sum < target) ? left++ : right--;
  }
  cout << "No pair found" << endl;
}

int main() {
  int arr[] = {1, 2, 3, 4, 6};
  int n = sizeof(arr) / sizeof(arr[0]);
  int target = 6;
  pairSum(arr, n, target);
  return 0;
}
