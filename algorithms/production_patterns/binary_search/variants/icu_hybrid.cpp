/*
 * ICU Hybrid Binary + Linear Search
 * 
 * Source: node/deps/icu-small/source/common/uarrsort.cpp (lines 74-116)
 * 
 * What Makes It Ingenious:
 * - Binary search until sub-array is small (MIN_QSORT threshold)
 * - Then switches to linear search
 * - Optimized for stable sorting insertion points
 * - Handles duplicates intelligently
 * 
 * When to Use:
 * - Finding insertion point for stable sort
 * - Small arrays after binary search phase
 * - Need to handle duplicates
 * 
 * Real-World Usage:
 * - ICU library stable sorting
 * - Insertion sort optimization
 */

#include <vector>
#include <functional>

template<typename T>
class HybridBinarySearch {
private:
    static constexpr int MIN_QSORT = 7; // Threshold for switching to linear
    
public:
    /*
     * Stable binary search - finds insertion point for stable sort
     * Returns: index where item should be inserted, or ~index if found
     */
    int StableBinarySearch(
        const std::vector<T>& array,
        const T& item,
        std::function<int(const T&, const T&)> comparator
    ) {
        int start = 0;
        int limit = array.size();
        bool found = false;
        
        // Binary search until we get down to a tiny sub-array
        while ((limit - start) >= MIN_QSORT) {
            int i = (start + limit) / 2;
            int diff = comparator(item, array[i]);
            
            if (diff == 0) {
                /*
                 * Found the item. We look for the *last* occurrence of such
                 * an item, for stable sorting.
                 * If we knew that there will be only few equal items,
                 * we could break now and enter the linear search.
                 * However, if there are many equal items, then it should be
                 * faster to continue with the binary search.
                 */
                found = true;
                start = i + 1; // Continue searching right for last occurrence
            } else if (diff < 0) {
                limit = i; // Search left
            } else {
                start = i + 1; // Search right
            }
        }
        
        // Linear search over the remaining tiny sub-array
        while (start < limit) {
            int diff = comparator(item, array[start]);
            if (diff == 0) {
                found = true;
            } else if (diff < 0) {
                break; // Found insertion point
            }
            ++start;
        }
        
        // Return insertion point, or ~index if found (for stable sort)
        return found ? ~(start - 1) : start;
    }
    
    /*
     * Find insertion point for item in sorted array
     * Returns: index where item should be inserted
     */
    int FindInsertionPoint(
        const std::vector<T>& array,
        const T& item,
        std::function<int(const T&, const T&)> comparator
    ) {
        int result = StableBinarySearch(array, item, comparator);
        if (result < 0) {
            // Item found, return insertion point (last occurrence + 1)
            return ~result + 1;
        }
        return result;
    }
};

// Example usage
#include <iostream>

int main() {
    HybridBinarySearch<int> search;
    
    std::vector<int> arr = {1, 3, 3, 3, 5, 7, 9};
    
    auto comparator = [](int a, int b) -> int {
        if (a < b) return -1;
        if (a > b) return 1;
        return 0;
    };
    
    // Find insertion point for 3 (should be after all 3s)
    int pos = search.FindInsertionPoint(arr, 3, comparator);
    std::cout << "Insertion point for 3: " << pos << std::endl;
    
    // Find insertion point for 4 (should be between 3 and 5)
    pos = search.FindInsertionPoint(arr, 4, comparator);
    std::cout << "Insertion point for 4: " << pos << std::endl;
    
    return 0;
}

