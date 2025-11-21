// TimSort: Production-grade hybrid sorting algorithm used in Python and Java
// Combines merge sort and insertion sort for optimal performance
// Time: O(n log n) worst case, O(n) best case (nearly sorted)
// Space: O(n)

#include <vector>
#include <algorithm>
#include <iostream>

const int RUN = 32; // Minimum run size

// Insertion sort for small runs
void insertionSort(std::vector<int>& arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int temp = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > temp) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
    }
}

// Merge two sorted runs
void merge(std::vector<int>& arr, int l, int m, int r) {
    int len1 = m - l + 1, len2 = r - m;
    std::vector<int> left(len1), right(len2);
    
    for (int i = 0; i < len1; i++) left[i] = arr[l + i];
    for (int i = 0; i < len2; i++) right[i] = arr[m + 1 + i];
    
    int i = 0, j = 0, k = l;
    while (i < len1 && j < len2) {
        if (left[i] <= right[j]) {
            arr[k++] = left[i++];
        } else {
            arr[k++] = right[j++];
        }
    }
    
    while (i < len1) arr[k++] = left[i++];
    while (j < len2) arr[k++] = right[j++];
}

// TimSort main function
void timSort(std::vector<int>& arr) {
    int n = arr.size();
    
    // Sort individual runs of size RUN
    for (int i = 0; i < n; i += RUN) {
        insertionSort(arr, i, std::min(i + RUN - 1, n - 1));
    }
    
    // Merge runs starting from size RUN
    for (int size = RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = std::min(left + 2 * size - 1, n - 1);
            
            if (mid < right) {
                merge(arr, left, mid, right);
            }
        }
    }
}

// Example usage
int main() {
    std::vector<int> arr = {5, 21, 7, 23, 19, 2, 8, 1, 15, 12};
    
    std::cout << "Original array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    timSort(arr);
    
    std::cout << "Sorted array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    return 0;
}

