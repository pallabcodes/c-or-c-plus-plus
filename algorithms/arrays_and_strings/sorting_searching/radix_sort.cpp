// Radix Sort: Non-comparison based sorting for integers
// Uses counting sort as a subroutine
// Time: O(d * (n + k)) where d is number of digits, k is base (usually 10)
// Space: O(n + k)

#include <vector>
#include <algorithm>
#include <iostream>

// Counting sort as subroutine for radix sort
void countingSort(std::vector<int>& arr, int exp) {
    int n = arr.size();
    std::vector<int> output(n);
    std::vector<int> count(10, 0);
    
    // Count occurrences of each digit
    for (int i = 0; i < n; i++) {
        count[(arr[i] / exp) % 10]++;
    }
    
    // Change count to position
    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }
    
    // Build output array
    for (int i = n - 1; i >= 0; i--) {
        output[count[(arr[i] / exp) % 10] - 1] = arr[i];
        count[(arr[i] / exp) % 10]--;
    }
    
    // Copy output to original array
    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
    }
}

// Radix Sort main function
void radixSort(std::vector<int>& arr) {
    if (arr.empty()) return;
    
    // Find maximum to know number of digits
    int maxVal = *std::max_element(arr.begin(), arr.end());
    
    // Do counting sort for every digit
    for (int exp = 1; maxVal / exp > 0; exp *= 10) {
        countingSort(arr, exp);
    }
}

// Radix sort for negative numbers (using offset)
void radixSortWithNegatives(std::vector<int>& arr) {
    if (arr.empty()) return;
    
    // Find min and max
    int minVal = *std::min_element(arr.begin(), arr.end());
    int maxVal = *std::max_element(arr.begin(), arr.end());
    
    // Offset to make all numbers non-negative
    int offset = (minVal < 0) ? -minVal : 0;
    
    // Add offset to all elements
    for (int& x : arr) x += offset;
    
    // Perform radix sort
    radixSort(arr);
    
    // Remove offset
    for (int& x : arr) x -= offset;
}

// Example usage
int main() {
    std::vector<int> arr = {170, 45, 75, 90, 802, 24, 2, 66};
    
    std::cout << "Original array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    radixSort(arr);
    
    std::cout << "Sorted array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    // Test with negatives
    std::vector<int> arr2 = {-170, 45, -75, 90, -802, 24, 2, -66};
    std::cout << "\nOriginal array (with negatives): ";
    for (int x : arr2) std::cout << x << " ";
    std::cout << std::endl;
    
    radixSortWithNegatives(arr2);
    
    std::cout << "Sorted array: ";
    for (int x : arr2) std::cout << x << " ";
    std::cout << std::endl;
    
    return 0;
}

