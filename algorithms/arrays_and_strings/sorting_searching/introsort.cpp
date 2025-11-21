// IntroSort: Hybrid sorting algorithm used in C++ STL
// Combines quicksort, heapsort, and insertion sort
// Guarantees O(n log n) worst case while being fast in practice
// Time: O(n log n) worst case, O(n log n) average
// Space: O(log n)

#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>

const int INSERTION_SORT_THRESHOLD = 16;
const int MAX_DEPTH = 2 * (int)log2(1000000); // Adjust based on max array size

// Insertion sort for small arrays
void insertionSort(std::vector<int>& arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Heap sort for worst case handling
void heapify(std::vector<int>& arr, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < n && arr[left] > arr[largest]) largest = left;
    if (right < n && arr[right] > arr[largest]) largest = right;
    
    if (largest != i) {
        std::swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(std::vector<int>& arr, int left, int right) {
    int n = right - left + 1;
    std::vector<int> heap(arr.begin() + left, arr.begin() + right + 1);
    
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(heap, n, i);
    }
    
    for (int i = n - 1; i > 0; i--) {
        std::swap(heap[0], heap[i]);
        heapify(heap, i, 0);
    }
    
    for (int i = 0; i < n; i++) {
        arr[left + i] = heap[i];
    }
}

// Partition for quicksort
int partition(std::vector<int>& arr, int left, int right) {
    int pivot = arr[right];
    int i = left - 1;
    
    for (int j = left; j < right; j++) {
        if (arr[j] <= pivot) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[right]);
    return i + 1;
}

// IntroSort main function
void introSort(std::vector<int>& arr, int left, int right, int depthLimit) {
    int size = right - left + 1;
    
    // Use insertion sort for small arrays
    if (size < INSERTION_SORT_THRESHOLD) {
        insertionSort(arr, left, right);
        return;
    }
    
    // Use heap sort if depth limit exceeded
    if (depthLimit == 0) {
        heapSort(arr, left, right);
        return;
    }
    
    // Otherwise use quicksort
    int pivot = partition(arr, left, right);
    introSort(arr, left, pivot - 1, depthLimit - 1);
    introSort(arr, pivot + 1, right, depthLimit - 1);
}

void introSort(std::vector<int>& arr) {
    int n = arr.size();
    int depthLimit = MAX_DEPTH;
    introSort(arr, 0, n - 1, depthLimit);
}

// Example usage
int main() {
    std::vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 5, 77, 1};
    
    std::cout << "Original array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    introSort(arr);
    
    std::cout << "Sorted array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    return 0;
}

