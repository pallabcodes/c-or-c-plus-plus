/*
 * Sqrt Decomposition for Dynamic Programming Range Queries
 *
 * Source: Competitive programming community, research on block algorithms
 * Algorithm: Divide array into blocks of size √n for balanced query/update time
 * Paper: Various block-based algorithms research
 *
 * What Makes It Ingenious:
 * - Divides array into √n blocks for O(√n) query/update time
 * - Precomputes block aggregates for fast range queries
 * - Simple implementation with good performance
 * - Used when updates are less frequent than queries
 * - Memory efficient compared to full segment trees
 *
 * When to Use:
 * - Static or semi-static arrays with range queries
 * - When updates are rare but queries are frequent
 * - Memory-constrained environments
 * - Simple range sum/min/max queries
 * - Competitive programming problems
 * - When full segment tree is overkill
 *
 * Real-World Usage:
 * - Range sum queries in arrays
 * - Range minimum/maximum queries
 * - Static database queries
 * - Log analysis and processing
 * - Time series data analysis
 * - Competitive programming contests
 *
 * Time Complexity:
 * - Preprocessing: O(n)
 * - Range query: O(√n)
 * - Point update: O(√n) worst case, O(1) amortized
 *
 * Space Complexity: O(n) for blocks + O(√n) for block aggregates
 */

#include <vector>
#include <cmath>
#include <iostream>
#include <functional>
#include <algorithm>

template<typename T>
class SqrtDecomposition {
private:
    std::vector<T> arr;
    int n;
    int block_size;
    int num_blocks;
    std::vector<T> blocks; // Block aggregates
    std::function<T(T, T)> combine_func;
    T identity;

    // Recompute block aggregate
    void update_block(int block_idx) {
        int start = block_idx * block_size;
        int end = std::min(start + block_size, n);

        blocks[block_idx] = identity;
        for (int i = start; i < end; ++i) {
            blocks[block_idx] = combine_func(blocks[block_idx], arr[i]);
        }
    }

public:
    SqrtDecomposition(const std::vector<T>& input,
                     std::function<T(T, T)> combine,
                     T identity_elem = T{})
        : arr(input), n(input.size()), combine_func(combine), identity(identity_elem) {

        block_size = std::sqrt(n);
        if (block_size == 0) block_size = 1;
        num_blocks = (n + block_size - 1) / block_size;

        blocks.resize(num_blocks, identity);

        // Precompute block aggregates
        for (int i = 0; i < num_blocks; ++i) {
            update_block(i);
        }
    }

    // Point update: update arr[idx] to new_val
    void update(int idx, T new_val) {
        if (idx < 0 || idx >= n) return;

        arr[idx] = new_val;
        int block_idx = idx / block_size;
        update_block(block_idx);
    }

    // Range query: query from left to right inclusive
    T query(int left, int right) {
        if (left > right || left < 0 || right >= n) return identity;

        T result = identity;

        // Query complete blocks
        int start_block = left / block_size;
        int end_block = right / block_size;

        if (start_block == end_block) {
            // Same block: iterate through elements
            for (int i = left; i <= right; ++i) {
                result = combine_func(result, arr[i]);
            }
            return result;
        }

        // Partial start block
        int start_end = (start_block + 1) * block_size - 1;
        for (int i = left; i <= start_end && i <= right; ++i) {
            result = combine_func(result, arr[i]);
        }

        // Complete blocks in middle
        for (int b = start_block + 1; b < end_block; ++b) {
            result = combine_func(result, blocks[b]);
        }

        // Partial end block
        int end_start = end_block * block_size;
        for (int i = end_start; i <= right; ++i) {
            result = combine_func(result, arr[i]);
        }

        return result;
    }

    // Get element at index
    T get(int idx) const {
        return (idx >= 0 && idx < n) ? arr[idx] : identity;
    }

    // Get block size and number of blocks
    int get_block_size() const { return block_size; }
    int get_num_blocks() const { return num_blocks; }

    // Print debug information
    void debug_print() const {
        std::cout << "Array: ";
        for (T val : arr) std::cout << val << " ";
        std::cout << std::endl;

        std::cout << "Blocks: ";
        for (T val : blocks) std::cout << val << " ";
        std::cout << std::endl;

        std::cout << "Block size: " << block_size << ", Num blocks: " << num_blocks << std::endl;
    }
};

// Range Minimum Query using Sqrt Decomposition
class RangeMinimumQuery {
private:
    SqrtDecomposition<int> sqrt_decomp;

public:
    RangeMinimumQuery(const std::vector<int>& arr)
        : sqrt_decomp(arr, [](int a, int b) { return std::min(a, b); }, INT_MAX) {}

    int query_min(int left, int right) {
        return sqrt_decomp.query(left, right);
    }

    void update(int idx, int new_val) {
        sqrt_decomp.update(idx, new_val);
    }
};

// Range Sum Query using Sqrt Decomposition
class RangeSumQuery {
private:
    SqrtDecomposition<long long> sqrt_decomp;

public:
    RangeSumQuery(const std::vector<long long>& arr)
        : sqrt_decomp(arr, std::plus<long long>(), 0LL) {}

    long long query_sum(int left, int right) {
        return sqrt_decomp.query(left, right);
    }

    void update(int idx, long long new_val) {
        sqrt_decomp.update(idx, new_val);
    }
};

// DP with Sqrt Decomposition: Range sum queries with updates
class DPSqrtDecomposition {
public:
    // Example: Maximum subarray sum with range updates
    static long long max_subarray_sum_with_updates(
        std::vector<long long>& arr,
        const std::vector<std::tuple<int, int, long long>>& updates,
        const std::vector<std::pair<int, int>>& queries) {

        RangeSumQuery rsq(arr);

        // Apply updates
        for (const auto& [idx, new_val] : updates) {
            rsq.update(idx, new_val);
        }

        // Answer queries
        long long max_sum = LLONG_MIN;
        for (const auto& [left, right] : queries) {
            long long sum = rsq.query_sum(left, right);
            max_sum = std::max(max_sum, sum);
        }

        return max_sum;
    }

    // Example: Range XOR queries (useful for some DP problems)
    static int range_xor_with_updates(
        std::vector<int>& arr,
        const std::vector<std::tuple<int, int, int>>& updates,
        const std::vector<std::pair<int, int>>& queries) {

        SqrtDecomposition<int> xor_decomp(arr,
            [](int a, int b) { return a ^ b; }, 0);

        // Apply updates
        for (const auto& [idx, new_val] : updates) {
            xor_decomp.update(idx, new_val);
        }

        // Answer queries
        int result = 0;
        for (const auto& [left, right] : queries) {
            result ^= xor_decomp.query(left, right);
        }

        return result;
    }

    // Demonstrate sqrt decomposition
    static void demonstrate() {
        std::cout << "Sqrt Decomposition Demonstration:" << std::endl;

        // Range Sum Query
        std::vector<long long> arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        RangeSumQuery rsq(arr);

        std::cout << "\nRange Sum Query:" << std::endl;
        std::cout << "Sum [0,4]: " << rsq.query_sum(0, 4) << std::endl;
        std::cout << "Sum [2,7]: " << rsq.query_sum(2, 7) << std::endl;

        rsq.update(3, 10);
        std::cout << "After updating index 3 to 10:" << std::endl;
        std::cout << "Sum [0,4]: " << rsq.query_sum(0, 4) << std::endl;

        // Range Minimum Query
        std::vector<int> arr2 = {5, 3, 8, 1, 9, 2, 7, 4, 6, 0};
        RangeMinimumQuery rmq(arr2);

        std::cout << "\nRange Minimum Query:" << std::endl;
        std::cout << "Min [1,5]: " << rmq.query_min(1, 5) << std::endl;
        std::cout << "Min [3,8]: " << rmq.query_min(3, 8) << std::endl;

        rmq.update(6, 0);
        std::cout << "After updating index 6 to 0:" << std::endl;
        std::cout << "Min [3,8]: " << rmq.query_min(3, 8) << std::endl;

        // DP Example: Range XOR
        std::vector<int> arr3 = {1, 3, 5, 7, 9, 11};
        std::vector<std::tuple<int, int, int>> updates = {{2, 10}};
        std::vector<std::pair<int, int>> queries = {{0, 2}, {1, 4}};

        int xor_result = range_xor_with_updates(arr3, updates, queries);
        std::cout << "\nRange XOR result: " << xor_result << std::endl;
    }
};

// Example usage
int main() {
    DPSqrtDecomposition::demonstrate();
    return 0;
}

