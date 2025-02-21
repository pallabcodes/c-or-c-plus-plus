#include <algorithm> // For reverse() function
#include <cmath>     // For abs() and pow() functions
#include <iostream>
#include <vector>

using namespace std;

// Counting Sort for a specific digit (modifies the array in place)
void countingSort(vector<int> &arr, int exp) {
  int n = arr.size();
  vector<int> output(n);    // Output array
  vector<int> count(10, 0); // Count array for digits 0 to 9

  // 802 / 100 = 8.02  so 8.02 % 10 = 8

  // Store count of occurrences in count[]
  for (int i = 0; i < n; i++) {
    count[(abs(arr[i]) / exp) % 10]++;
  }

  // [2, 0, 1, 0, 1, 0, 1, 2, 0, 1]

  // [ 6, 1, 0, 0, 0, 0, 0, 0, 1, 0 ]

      // Change count[i] so that it now contains the actual position of this
      // digit in output[]
      for (int i = 1; i < 10; i++) {
    count[i] += count[i - 1];
  }

  // Before starting the below loop

  // arr =   [170, 45, 75, 90, 802, 24, 2, 66]
  // count = [ 2, 2, 4, 4, 5, 7, 8, 8, 8, 8 ]
  // output = [ _, _, _, 2, 24, _, _, 66 ](empty)

  // Build the output array
  for (int i = n - 1; i >= 0; i--) {
    int digit = (abs(arr[i]) / exp) % 10; // 4
    output[count[digit] - 1] = arr[i];
    count[digit]--;
  }

  // 1st pass when exp = 1 [170, 90, 802, 2, 24, 45, 75, 66]
  // 2nd pass when exp = 10 [802, 2, 24, 45, 66, 170, 75, 90]

  // Copy the output array to arr[], so that arr[] contains sorted numbers
  for (int i = 0; i < n; i++) {
    arr[i] = output[i];
  }
}

// Radix Sort function (handles negative numbers)
void radixSort(vector<int> &arr) {
  // Check for edge case where the array is empty or has only one element
  if (arr.empty() || arr.size() == 1) {
    return;
  }

  // Find the maximum element to get the number of digits
  int maxVal = *max_element(arr.begin(), arr.end());

  // Perform counting sort for every digit (starting from the least significant
  // digit)
  for (int exp = 1; maxVal / exp > 0; exp *= 10) {
    countingSort(arr, exp);
  }

  // Handle the negative numbers
  vector<int> negative, positive;
  for (int num : arr) {
    if (num < 0) {
      negative.push_back(num);
    } else {
      positive.push_back(num);
    }
  }

  // Reverse negative numbers (they were sorted in increasing order of absolute
  // value)
  reverse(negative.begin(), negative.end());

  // Merge the two sorted parts back into the original array
  negative.insert(negative.end(), positive.begin(), positive.end());

  arr = negative; // Update the original array
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
  // Test case 1: Normal array with positive numbers
  vector<int> arr1 = {170, 45, 75, 90, 802, 24, 2, 66};
  cout << "Original Array: ";
  printArray(arr1);
  radixSort(arr1);
  cout << "Sorted Array: ";
  printArray(arr1);

  // Test case 2: Empty array
  vector<int> arr2 = {};
  cout << "\nOriginal Array: ";
  printArray(arr2);
  radixSort(arr2);
  cout << "Sorted Array: ";
  printArray(arr2);

  // Test case 3: Single element array
  vector<int> arr3 = {5};
  cout << "\nOriginal Array: ";
  printArray(arr3);
  radixSort(arr3);
  cout << "Sorted Array: ";
  printArray(arr3);

  // Test case 4: Array with negative numbers
  vector<int> arr4 = {-5, -2, -9, 1, 3, 8};
  cout << "\nOriginal Array: ";
  printArray(arr4);
  radixSort(arr4);
  cout << "Sorted Array: ";
  printArray(arr4);

  return 0;
}
