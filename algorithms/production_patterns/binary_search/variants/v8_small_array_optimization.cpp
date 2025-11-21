/*
 * V8 Small Array Optimization
 * 
 * Source: node/deps/v8/src/objects/descriptor-array-inl.h (lines 85-92)
 * 
 * What Makes It Ingenious:
 * - Linear search for small arrays (≤8 elements)
 * - Binary search overhead not worth it for tiny arrays
 * - Cache-friendly linear search
 * - Adaptive based on array size
 * 
 * When to Use:
 * - Array size may be small
 * - Need optimal performance for both small and large arrays
 * - Cache locality matters
 * 
 * Real-World Usage:
 * - V8 DescriptorArray property lookup
 * - Small object property access
 */

#include <vector>
#include <algorithm>

template<typename T>
class AdaptiveBinarySearch {
private:
    static constexpr int MAX_ELEMENTS_FOR_LINEAR_SEARCH = 8;
    
    // Linear search for small arrays
    int LinearSearch(const std::vector<T>& array, const T& target) {
        for (size_t i = 0; i < array.size(); i++) {
            if (array[i] == target) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }
    
    // Binary search for larger arrays
    int BinarySearch(const std::vector<T>& array, const T& target) {
        int left = 0;
        int right = static_cast<int>(array.size()) - 1;
        
        while (left <= right) {
            int mid = left + (right - left) / 2;
            
            if (array[mid] == target) {
                return mid;
            } else if (array[mid] < target) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        
        return -1;
    }
    
public:
    // Adaptive search: linear for small, binary for large
    int Search(const std::vector<T>& array, const T& target) {
        if (array.empty()) {
            return -1;
        }
        
        // Use linear search for small arrays
        if (array.size() <= MAX_ELEMENTS_FOR_LINEAR_SEARCH) {
            return LinearSearch(array, target);
        }
        
        // Use binary search for larger arrays
        return BinarySearch(array, target);
    }
    
    // Variant: Also consider concurrent search (use linear for thread safety)
    int SearchConcurrent(const std::vector<T>& array, const T& target, bool concurrent_search) {
        if (array.empty()) {
            return -1;
        }
        
        // Always use linear search for concurrent access (simpler, safer)
        if (concurrent_search || array.size() <= MAX_ELEMENTS_FOR_LINEAR_SEARCH) {
            return LinearSearch(array, target);
        }
        
        return BinarySearch(array, target);
    }
};

// Example usage
#include <iostream>

int main() {
    AdaptiveBinarySearch<int> search;
    
    // Small array - will use linear search
    std::vector<int> small_arr = {1, 3, 5, 7};
    int result = search.Search(small_arr, 5);
    std::cout << "Small array - Found 5 at index: " << result << std::endl;
    
    // Large array - will use binary search
    std::vector<int> large_arr;
    for (int i = 0; i < 100; i++) {
        large_arr.push_back(i * 2);
    }
    result = search.Search(large_arr, 50);
    std::cout << "Large array - Found 50 at index: " << result << std::endl;
    
    return 0;
}

