#include <bits/stdc++.h>
using namespace std;

// Heapify function to maintain heap property
void heapify(vector<int> &arr, int n, int i) {
  int largest = i;       // Root
  int left = 2 * i + 1;  // Left child
  int right = 2 * i + 2; // Right child

  // If left child is larger than root
  if (left < n && arr[left] > arr[largest])
    largest = left;

  // If right child is larger than current largest
  if (right < n && arr[right] > arr[largest])
    largest = right;

  // If largest is not root, swap and continue heapifying
  if (largest != i) {
    swap(arr[i], arr[largest]);
    heapify(arr, n, largest);
  }
}

// Heap Sort function
void heapSort(vector<int> &arr) {
  int n = arr.size();

  // Step 1: Build a max heap (O(n))
  for (int i = n / 2 - 1; i >= 0; i--)
    heapify(arr, n, i);

  // Step 2: Extract elements from the heap (O(n log n))
  for (int i = n - 1; i > 0; i--) {
    swap(arr[0], arr[i]); // Move max to end
    heapify(arr, i, 0);   // Heapify the reduced heap
  }
}

// Utility function to print array
void printArray(vector<int> &arr) {
  for (int val : arr)
    cout << val << " ";
  cout << endl;
}

// Driver code
int main() {
  vector<int> arr = {12, 11, 13, 5, 6, 7};
  cout << "Original array: ";
  printArray(arr);

  heapSort(arr);

  cout << "Sorted array: ";
  printArray(arr);
  return 0;
}
