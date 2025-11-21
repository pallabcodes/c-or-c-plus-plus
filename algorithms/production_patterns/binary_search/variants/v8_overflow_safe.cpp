/*
 * V8 Overflow-Safe Mid Calculation
 * 
 * Source: node/deps/v8/src/codegen/code-stub-assembler.cc (lines 11464-11472)
 * 
 * What Makes It Ingenious:
 * - Conditional mid calculation based on array size
 * - (low + high) / 2 for small arrays (faster)
 * - low + (high - low) / 2 for large arrays (overflow-safe)
 * - Compiler-level optimization
 * 
 * When to Use:
 * - Code generation / compiler backends
 * - Need overflow safety
 * - Performance-critical (uses faster path when safe)
 * 
 * Real-World Usage:
 * - V8 TurboFan compiler code generation
 * - Assembly-level optimizations
 */

#include <vector>
#include <climits>
#include <cstdint>

template<typename T>
class OverflowSafeBinarySearch {
private:
    static constexpr int MAX_INT31 = INT_MAX / 2;
    
    // Calculate mid point with overflow protection
    // Uses faster path when safe, safe path when needed
    static int CalculateMid(int low, int high, int max_size) {
        if (max_size < MAX_INT31) {
            // Fast path: (low + high) / 2
            // Safe because max_size < INT_MAX/2, so low + high < INT_MAX
            return (low + high) >> 1; // Right shift is faster than division
        } else {
            // Safe path: low + (high - low) / 2
            // Prevents overflow when low + high > INT_MAX
            return low + ((high - low) >> 1);
        }
    }
    
public:
    // Binary search with overflow-safe mid calculation
    int Search(const std::vector<T>& array, const T& target) {
        if (array.empty()) {
            return -1;
        }
        
        int low = 0;
        int high = static_cast<int>(array.size()) - 1;
        int max_size = static_cast<int>(array.size());
        
        while (low <= high) {
            // Use overflow-safe mid calculation
            int mid = CalculateMid(low, high, max_size);
            
            if (array[mid] == target) {
                return mid; // Found
            } else if (array[mid] < target) {
                low = mid + 1; // Search right
            } else {
                high = mid - 1; // Search left
            }
        }
        
        return -1; // Not found
    }
    
    // Variant: Always use safe calculation (simpler, slightly slower)
    int SearchAlwaysSafe(const std::vector<T>& array, const T& target) {
        if (array.empty()) {
            return -1;
        }
        
        int low = 0;
        int high = static_cast<int>(array.size()) - 1;
        
        while (low <= high) {
            // Always use safe calculation
            int mid = low + ((high - low) >> 1);
            
            if (array[mid] == target) {
                return mid;
            } else if (array[mid] < target) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        
        return -1;
    }
};

// Example usage
#include <iostream>

int main() {
    OverflowSafeBinarySearch<int> search;
    
    std::vector<int> arr = {1, 3, 5, 7, 9, 11, 13, 15};
    
    // Test with normal array
    int result = search.Search(arr, 7);
    std::cout << "Found 7 at index: " << result << std::endl;
    
    // Test with large array (would benefit from overflow-safe calculation)
    std::vector<int> large_arr;
    for (int i = 0; i < 1000000; i++) {
        large_arr.push_back(i * 2);
    }
    
    result = search.Search(large_arr, 500000);
    std::cout << "Found 500000 at index: " << result << std::endl;
    
    return 0;
}

