#include <iostream>
#include <vector>

using namespace std;

// Function to perform selection sort on the vector
void selectionSort(vector<int> &arr) {
  int n = arr.size();

  // Edge case: if the array is empty or has only one element
  if (n <= 1) {
    return;
  }

  // One by one move the boundary of the unsorted subarray
  for (int i = 0; i < n - 1; i++) {
    // Find the minimum element in the unsorted part
    int minIndex = i;
    
    for (int j = i + 1; j < n; j++) {
      if (arr[j] < arr[minIndex]) {
        minIndex = j;
      }
    }

    // Swap the found minimum element with the first element
    if (minIndex != i) {
      swap(arr[i], arr[minIndex]);
    }
  }
}

// Function to print the array
void printArray(const vector<int> &arr) {
  if (arr.empty()) {
    cout << "Array is empty!" << endl;
  } else {
    for (int num : arr) {
      cout << num << " ";
    }
    cout << endl;
  }
}

int main() {
  // Example usage
  vector<int> arr1 = {64, 25, 12, 22, 11};
  vector<int> arr2 = {};  // Empty array
  vector<int> arr3 = {5}; // Single element array

  // Test case 1: Normal array
  cout << "Original Array: ";
  printArray(arr1);
  selectionSort(arr1);
  cout << "Sorted Array: ";
  printArray(arr1);

  // Test case 2: Empty array
  cout << "\nOriginal Array: ";
  printArray(arr2);
  selectionSort(arr2);
  cout << "Sorted Array: ";
  printArray(arr2);

  // Test case 3: Single element array
  cout << "\nOriginal Array: ";
  printArray(arr3);
  selectionSort(arr3);
  cout << "Sorted Array: ";
  printArray(arr3);

  return 0;
}
