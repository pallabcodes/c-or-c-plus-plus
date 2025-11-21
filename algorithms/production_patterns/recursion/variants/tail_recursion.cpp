/*
 * Tail Recursion Optimization
 * 
 * Source: Compiler optimization techniques
 * Pattern: Tail call elimination, iterative conversion
 * 
 * What Makes It Ingenious:
 * - Tail recursion: Recursive call is last operation
 * - Tail call elimination: Compiler converts to iteration
 * - Stack optimization: O(1) space instead of O(n)
 * - Performance: Same as iteration, more readable
 * - Used in functional languages, compilers, interpreters
 * 
 * When to Use:
 * - Recursive algorithms where last operation is recursive call
 * - When stack space is limited
 * - Functional programming style
 * - When compiler supports tail call optimization
 * 
 * Real-World Usage:
 * - Functional language implementations (Scheme, Haskell)
 * - Compiler optimizations
 * - Iterative algorithms written recursively
 * - Stack-constrained environments
 * 
 * Time Complexity: Same as iterative version
 * Space Complexity: O(1) with optimization, O(n) without
 */

#include <vector>

class TailRecursion {
public:
    // Tail recursive factorial
    int factorial_tail(int n, int acc = 1) {
        // Base case
        if (n <= 1) {
            return acc;
        }
        
        // Tail recursive call: last operation is recursive call
        return factorial_tail(n - 1, acc * n);
    }
    
    // Non-tail recursive factorial (for comparison)
    int factorial_non_tail(int n) {
        if (n <= 1) {
            return 1;
        }
        
        // Not tail recursive: multiplication after recursive call
        return n * factorial_non_tail(n - 1);
    }
    
    // Tail recursive sum of array
    int sum_tail(const std::vector<int>& arr, int index = 0, int acc = 0) {
        if (index >= arr.size()) {
            return acc;  // Base case
        }
        
        // Tail recursive call
        return sum_tail(arr, index + 1, acc + arr[index]);
    }
    
    // Tail recursive reverse list
    std::vector<int> reverse_tail(const std::vector<int>& list, 
                                   std::vector<int> acc = {}) {
        if (list.empty()) {
            return acc;  // Base case
        }
        
        // Tail recursive: build accumulator
        std::vector<int> new_acc = {list[0]};
        new_acc.insert(new_acc.end(), acc.begin(), acc.end());
        
        return reverse_tail(
            std::vector<int>(list.begin() + 1, list.end()),
            new_acc
        );
    }
    
    // Tail recursive greatest common divisor
    int gcd_tail(int a, int b) {
        if (b == 0) {
            return a;  // Base case
        }
        
        // Tail recursive call
        return gcd_tail(b, a % b);
    }
    
    // Tail recursive binary search
    int binary_search_tail(const std::vector<int>& arr, int target,
                          int left = 0, int right = -1) {
        if (right == -1) {
            right = arr.size() - 1;
        }
        
        if (left > right) {
            return -1;  // Base case: not found
        }
        
        int mid = left + (right - left) / 2;
        
        if (arr[mid] == target) {
            return mid;  // Base case: found
        }
        
        // Tail recursive calls
        if (arr[mid] > target) {
            return binary_search_tail(arr, target, left, mid - 1);
        } else {
            return binary_search_tail(arr, target, mid + 1, right);
        }
    }
    
    // Convert tail recursive to iterative (manual optimization)
    int factorial_iterative(int n) {
        int acc = 1;
        while (n > 1) {
            acc = acc * n;
            n = n - 1;
        }
        return acc;
    }
    
    // Tail recursive with accumulator pattern
    int sum_range_tail(int start, int end, int acc = 0) {
        if (start > end) {
            return acc;  // Base case
        }
        
        // Tail recursive call with accumulator
        return sum_range_tail(start + 1, end, acc + start);
    }
    
    // Tail recursive list length
    template<typename T>
    int length_tail(const std::vector<T>& list, int acc = 0) {
        if (list.empty()) {
            return acc;  // Base case
        }
        
        // Tail recursive call
        return length_tail(
            std::vector<T>(list.begin() + 1, list.end()),
            acc + 1
        );
    }
};

// Example usage
#include <iostream>

int main() {
    TailRecursion rec;
    
    // Factorial
    std::cout << "Factorial(5) tail recursive: " 
              << rec.factorial_tail(5) << std::endl;
    std::cout << "Factorial(5) iterative: " 
              << rec.factorial_iterative(5) << std::endl;
    
    // Sum array
    std::vector<int> arr = {1, 2, 3, 4, 5};
    std::cout << "Sum of array: " << rec.sum_tail(arr) << std::endl;
    
    // GCD
    std::cout << "GCD(48, 18): " << rec.gcd_tail(48, 18) << std::endl;
    
    // Binary search
    std::vector<int> sorted = {1, 3, 5, 7, 9, 11, 13};
    int index = rec.binary_search_tail(sorted, 7);
    std::cout << "Binary search for 7: index " << index << std::endl;
    
    // Sum range
    std::cout << "Sum 1 to 10: " << rec.sum_range_tail(1, 10) << std::endl;
    
    return 0;
}

