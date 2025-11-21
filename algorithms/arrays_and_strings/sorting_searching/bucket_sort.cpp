// Bucket Sort: Distribution sort for uniformly distributed data
// Divides array into buckets, sorts each bucket, then concatenates
// Time: O(n + k) average case, O(n^2) worst case
// Space: O(n + k)

#include <vector>
#include <algorithm>
#include <iostream>

void bucketSort(std::vector<float>& arr) {
    int n = arr.size();
    if (n <= 0) return;
    
    // Create n empty buckets
    std::vector<std::vector<float>> buckets(n);
    
    // Put array elements in different buckets
    for (int i = 0; i < n; i++) {
        int bucketIdx = n * arr[i]; // For floats in range [0, 1)
        buckets[bucketIdx].push_back(arr[i]);
    }
    
    // Sort individual buckets
    for (int i = 0; i < n; i++) {
        std::sort(buckets[i].begin(), buckets[i].end());
    }
    
    // Concatenate all buckets into arr
    int index = 0;
    for (int i = 0; i < n; i++) {
        for (float val : buckets[i]) {
            arr[index++] = val;
        }
    }
}

// Bucket sort for integers
void bucketSortIntegers(std::vector<int>& arr) {
    int n = arr.size();
    if (n <= 0) return;
    
    // Find min and max
    int maxVal = *std::max_element(arr.begin(), arr.end());
    int minVal = *std::min_element(arr.begin(), arr.end());
    
    // Number of buckets (can be adjusted)
    int bucketCount = n;
    int bucketRange = (maxVal - minVal) / bucketCount + 1;
    
    std::vector<std::vector<int>> buckets(bucketCount);
    
    // Distribute array elements into buckets
    for (int i = 0; i < n; i++) {
        int bucketIdx = (arr[i] - minVal) / bucketRange;
        buckets[bucketIdx].push_back(arr[i]);
    }
    
    // Sort buckets and concatenate
    int index = 0;
    for (int i = 0; i < bucketCount; i++) {
        std::sort(buckets[i].begin(), buckets[i].end());
        for (int val : buckets[i]) {
            arr[index++] = val;
        }
    }
}

// Example usage
int main() {
    // Test with floats
    std::vector<float> arr = {0.897, 0.565, 0.656, 0.1234, 0.665, 0.3434};
    
    std::cout << "Original array (floats): ";
    for (float x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    bucketSort(arr);
    
    std::cout << "Sorted array: ";
    for (float x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    // Test with integers
    std::vector<int> arr2 = {42, 32, 33, 52, 37, 47, 51};
    
    std::cout << "\nOriginal array (integers): ";
    for (int x : arr2) std::cout << x << " ";
    std::cout << std::endl;
    
    bucketSortIntegers(arr2);
    
    std::cout << "Sorted array: ";
    for (int x : arr2) std::cout << x << " ";
    std::cout << std::endl;
    
    return 0;
}

