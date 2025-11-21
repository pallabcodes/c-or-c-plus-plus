/*
 * Cache-Oblivious Recursive Algorithms
 * 
 * Source: Research papers on cache-efficient algorithms
 * Pattern: Recursive algorithms optimized for memory hierarchy
 * 
 * What Makes It Ingenious:
 * - Cache-oblivious: No knowledge of cache parameters needed
 * - Memory hierarchy aware: Optimized for all cache levels
 * - Recursive blocking: Natural cache-friendly structure
 * - Used in high-performance computing, matrix operations
 * - Better cache locality than iterative blocked algorithms
 * 
 * When to Use:
 * - Large data structures that don't fit in cache
 * - Matrix operations (multiplication, transpose)
 * - Sorting large arrays
 * - Divide-and-conquer with cache optimization
 * - High-performance computing applications
 * 
 * Real-World Usage:
 * - BLAS/LAPACK libraries
 * - High-performance matrix libraries
 * - Database systems
 * - Scientific computing
 * 
 * Time Complexity: Same as standard algorithm
 * Space Complexity: O(n) but with better cache behavior
 */

#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>

class CacheObliviousRecursion {
public:
    // Cache-oblivious matrix multiplication
    // Recursively divides matrices until they fit in cache
    static void matrix_multiply_recursive(
        const std::vector<std::vector<double>>& A,
        const std::vector<std::vector<double>>& B,
        std::vector<std::vector<double>>& C,
        int a_row, int a_col,
        int b_row, int b_col,
        int c_row, int c_col,
        int size) {
        
        // Base case: small enough to fit in cache
        if (size <= 32) {  // Threshold for cache line
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    for (int k = 0; k < size; k++) {
                        C[c_row + i][c_col + j] += 
                            A[a_row + i][a_col + k] * B[b_row + k][b_col + j];
                    }
                }
            }
            return;
        }
        
        // Recursive case: divide into 4 quadrants
        int half = size / 2;
        
        // C11 = A11 * B11 + A12 * B21
        matrix_multiply_recursive(A, B, C, 
            a_row, a_col, b_row, b_col, c_row, c_col, half);
        matrix_multiply_recursive(A, B, C,
            a_row, a_col + half, b_row + half, b_col, c_row, c_col, half);
        
        // C12 = A11 * B12 + A12 * B22
        matrix_multiply_recursive(A, B, C,
            a_row, a_col, b_row, b_col + half, c_row, c_col + half, half);
        matrix_multiply_recursive(A, B, C,
            a_row, a_col + half, b_row + half, b_col + half, 
            c_row, c_col + half, half);
        
        // C21 = A21 * B11 + A22 * B21
        matrix_multiply_recursive(A, B, C,
            a_row + half, a_col, b_row, b_col, c_row + half, c_col, half);
        matrix_multiply_recursive(A, B, C,
            a_row + half, a_col + half, b_row + half, b_col, 
            c_row + half, c_col, half);
        
        // C22 = A21 * B12 + A22 * B22
        matrix_multiply_recursive(A, B, C,
            a_row + half, a_col, b_row, b_col + half, 
            c_row + half, c_col + half, half);
        matrix_multiply_recursive(A, B, C,
            a_row + half, a_col + half, b_row + half, b_col + half,
            c_row + half, c_col + half, half);
    }
    
    // Cache-oblivious matrix transpose
    static void transpose_recursive(
        const std::vector<std::vector<double>>& A,
        std::vector<std::vector<double>>& B,
        int a_row, int a_col,
        int b_row, int b_col,
        int size) {
        
        // Base case
        if (size <= 16) {
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    B[b_row + j][b_col + i] = A[a_row + i][a_col + j];
                }
            }
            return;
        }
        
        // Recursive case: divide into 4 quadrants
        int half = size / 2;
        
        transpose_recursive(A, B, a_row, a_col, b_row, b_col, half);
        transpose_recursive(A, B, a_row, a_col + half, b_row + half, b_col, half);
        transpose_recursive(A, B, a_row + half, a_col, b_row, b_col + half, half);
        transpose_recursive(A, B, a_row + half, a_col + half, 
                           b_row + half, b_col + half, half);
    }
    
    // Cache-oblivious merge sort
    static void merge_sort_cache_oblivious(
        std::vector<int>& arr, int left, int right) {
        
        if (left >= right) return;
        
        // Base case: small array fits in cache
        if (right - left < 32) {
            std::sort(arr.begin() + left, arr.begin() + right + 1);
            return;
        }
        
        int mid = left + (right - left) / 2;
        
        // Recursively sort both halves
        merge_sort_cache_oblivious(arr, left, mid);
        merge_sort_cache_oblivious(arr, mid + 1, right);
        
        // Merge with cache-friendly access pattern
        merge_cache_oblivious(arr, left, mid, right);
    }
    
private:
    static void merge_cache_oblivious(
        std::vector<int>& arr, int left, int mid, int right) {
        
        int n1 = mid - left + 1;
        int n2 = right - mid;
        
        std::vector<int> left_arr(arr.begin() + left, arr.begin() + mid + 1);
        std::vector<int> right_arr(arr.begin() + mid + 1, arr.begin() + right + 1);
        
        int i = 0, j = 0, k = left;
        
        while (i < n1 && j < n2) {
            if (left_arr[i] <= right_arr[j]) {
                arr[k++] = left_arr[i++];
            } else {
                arr[k++] = right_arr[j++];
            }
        }
        
        while (i < n1) {
            arr[k++] = left_arr[i++];
        }
        
        while (j < n2) {
            arr[k++] = right_arr[j++];
        }
    }
    
public:
    // Cache-oblivious binary search (with cache line awareness)
    static int binary_search_cache_oblivious(
        const std::vector<int>& arr, int target, int left, int right) {
        
        // Base case: small range fits in cache line
        if (right - left < 64) {
            // Linear search for small ranges (better cache locality)
            for (int i = left; i <= right; i++) {
                if (arr[i] == target) {
                    return i;
                }
            }
            return -1;
        }
        
        int mid = left + (right - left) / 2;
        
        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] > target) {
            return binary_search_cache_oblivious(arr, target, left, mid - 1);
        } else {
            return binary_search_cache_oblivious(arr, target, mid + 1, right);
        }
    }
    
    // Cache-oblivious tree traversal (for cache-line sized nodes)
    template<typename T>
    struct CacheLineNode {
        T data[8];  // Pack multiple elements per cache line
        int children[8];
        int count;
        
        CacheLineNode() : count(0) {
            for (int i = 0; i < 8; i++) {
                children[i] = -1;
            }
        }
    };
    
    // Recursive traversal with cache awareness
    template<typename T>
    static void traverse_cache_oblivious(
        const std::vector<CacheLineNode<T>>& tree,
        int node_idx,
        std::function<void(T)> visit) {
        
        if (node_idx == -1) return;
        
        const auto& node = tree[node_idx];
        
        // Process all data in cache line together
        for (int i = 0; i < node.count; i++) {
            visit(node.data[i]);
        }
        
        // Recursively traverse children
        for (int i = 0; i < 8; i++) {
            if (node.children[i] != -1) {
                traverse_cache_oblivious(tree, node.children[i], visit);
            }
        }
    }
};

// Example usage
int main() {
    // Cache-oblivious merge sort
    std::vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 5, 77, 3};
    std::cout << "Original array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    CacheObliviousRecursion::merge_sort_cache_oblivious(arr, 0, arr.size() - 1);
    
    std::cout << "Sorted array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    // Cache-oblivious binary search
    std::vector<int> sorted = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    int index = CacheObliviousRecursion::binary_search_cache_oblivious(
        sorted, 11, 0, sorted.size() - 1);
    std::cout << "Found 11 at index: " << index << std::endl;
    
    return 0;
}

