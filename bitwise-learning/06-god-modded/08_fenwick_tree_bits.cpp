/*
 * God-Modded: Fenwick Tree with Bit Manipulation
 * 
 * Fenwick tree (Binary Indexed Tree) using bit manipulation tricks
 * for efficient prefix sum queries and updates.
 */
#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>

struct FenwickTreeBits {
    std::vector<int32_t> tree;
    size_t n;
    
    // Thread-safety: Not thread-safe (constructor)
    // Ownership: Owns tree vector
    // Invariants: n > 0
    // Failure modes: Undefined behavior if n == 0
    explicit FenwickTreeBits(size_t n_size) : n(n_size), tree(n_size + 1, 0) {
        assert(n_size > 0);
    }
    
    // Thread-safety: Thread-safe (pure function)
    // Ownership: None (value semantics)
    // Invariants: i > 0
    // Failure modes: Undefined behavior if i == 0
    static inline size_t lsb(size_t i) {
        assert(i > 0);
        return i & -i;
    }
    
    // Thread-safety: Not thread-safe (modifies tree)
    // Ownership: Modifies owned tree
    // Invariants: i > 0 && i <= n
    // Failure modes: Undefined behavior if i == 0 or i > n
    void update(size_t i, int32_t delta) {
        assert(i > 0 && i <= n);
        while (i <= n) {
            tree[i] += delta;
            i += lsb(i);
        }
    }
    
    // Thread-safety: Thread-safe (pure function)
    // Ownership: None (read-only access)
    // Invariants: i > 0 && i <= n
    // Failure modes: Undefined behavior if i == 0 or i > n
    int32_t prefix_sum(size_t i) const {
        assert(i > 0 && i <= n);
        int32_t sum = 0;
        while (i > 0) {
            sum += tree[i];
            i -= lsb(i);
        }
        return sum;
    }
    
    // Thread-safety: Thread-safe (pure function)
    // Ownership: None (read-only access)
    // Invariants: l > 0 && r <= n && l <= r
    // Failure modes: Undefined behavior if invariants violated
    int32_t range_sum(size_t l, size_t r) const {
        assert(l > 0 && r <= n && l <= r);
        return prefix_sum(r) - prefix_sum(l - 1);
    }
};

int main() {
    FenwickTreeBits ft(10);
    ft.update(1, 5);
    ft.update(3, 3);
    ft.update(5, 7);
    std::cout << ft.prefix_sum(5) << std::endl;
    std::cout << ft.range_sum(2, 5) << std::endl;
    return 0;
}

