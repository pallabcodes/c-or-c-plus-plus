// Counting Sort: Non-comparison based sorting for integers in a range
// Extremely efficient when range is small compared to array size
// Time: O(n + k) where k is the range
// Space: O(k)

#include <vector>
#include <algorithm>
#include <iostream>
#include <climits>

void countingSort(std::vector<int>& arr) {
    if (arr.empty()) return;
    
    int n = arr.size();
    int maxVal = *std::max_element(arr.begin(), arr.end());
    int minVal = *std::min_element(arr.begin(), arr.end());
    int range = maxVal - minVal + 1;
    
    // Create count array
    std::vector<int> count(range, 0);
    std::vector<int> output(n);
    
    // Count occurrences
    for (int i = 0; i < n; i++) {
        count[arr[i] - minVal]++;
    }
    
    // Modify count to store positions
    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }
    
    // Build output array (stable sort)
    for (int i = n - 1; i >= 0; i--) {
        output[count[arr[i] - minVal] - 1] = arr[i];
        count[arr[i] - minVal]--;
    }
    
    // Copy output to original array
    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
    }
}

// Optimized version when range is known
void countingSortOptimized(std::vector<int>& arr, int minVal, int maxVal) {
    int n = arr.size();
    int range = maxVal - minVal + 1;
    
    std::vector<int> count(range, 0);
    
    // Count occurrences
    for (int i = 0; i < n; i++) {
        count[arr[i] - minVal]++;
    }
    
    // Rebuild array in sorted order
    int idx = 0;
    for (int i = 0; i < range; i++) {
        while (count[i] > 0) {
            arr[idx++] = i + minVal;
            count[i]--;
        }
    }
}

// Example usage
int main() {
    std::vector<int> arr = {4, 2, 2, 8, 3, 3, 1, 7, 5, 6};
    
    std::cout << "Original array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    countingSort(arr);
    
    std::cout << "Sorted array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    // Test optimized version
    std::vector<int> arr2 = {9, 1, 6, 7, 6, 2, 1, 5, 3, 4};
    std::cout << "\nOriginal array: ";
    for (int x : arr2) std::cout << x << " ";
    std::cout << std::endl;
    
    countingSortOptimized(arr2, 1, 9);
    
    std::cout << "Sorted array (optimized): ";
    for (int x : arr2) std::cout << x << " ";
    std::cout << std::endl;
    
    return 0;
}

