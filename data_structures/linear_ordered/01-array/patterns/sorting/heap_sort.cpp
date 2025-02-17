#include <iostream>
#include <vector>

using namespace std;

// Heapify function to maintain max heap property
void heapify(vector<int> &arr, int n, int i) {
    int largest = i;      // Assume root is largest
    int left = 2 * i + 1; // Left child
    int right = 2 * i + 2; // Right child

    // If left child exists and is greater than root
    if (left < n && arr[left] > arr[largest])
        largest = left;

    // If right child exists and is greater than the largest so far
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

    // Step 1: Build max heap (bottom-up approach)
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // Step 2: Extract elements one by one
    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);   // Move current root (max) to end
        heapify(arr, i, 0);     // Heapify the reduced heap
    }
}

// Driver function
int main() {
    vector<int> arr = {12, 11, 13, 5, 6, 7};

    heapSort(arr);

    cout << "Sorted Array: ";
    for (int num : arr) cout << num << " ";
    cout << endl;

    return 0;
}
