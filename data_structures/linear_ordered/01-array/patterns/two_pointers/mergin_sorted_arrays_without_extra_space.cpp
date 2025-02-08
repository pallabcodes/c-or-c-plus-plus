#include <iostream>
#include <vector>
using namespace std;

void mergeSortedArrays(vector<int> &arr1, int m, vector<int> &arr2, int n) {
  int i = m - 1;     // Last valid element in arr1
  int j = n - 1;     // Last element in arr2
  int k = m + n - 1; // Last position in merged array

  while (j >= 0) {
    if (i >= 0 && arr1[i] > arr2[j]) {
      arr1[k--] = arr1[i--]; // Move larger element to end
    } else {
      arr1[k--] = arr2[j--]; // Move arr2 element
    }
  }
}

int main() {
  vector<int> arr1 = {1, 3, 5, 0, 0, 0}; // Extra space at the end
  vector<int> arr2 = {2, 4, 6};
  int m = 3, n = 3;

  mergeSortedArrays(arr1, m, arr2, n);

  for (int num : arr1)
    cout << num << " ";
}
