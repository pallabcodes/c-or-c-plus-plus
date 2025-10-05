/*
 * =============================================================================
 * Performance Engineering: Lock Free Structs
 * Single producer single consumer ring buffer with atomics
 * =============================================================================
 */

#include <iostream>
#include <atomic>
#include <vector>
#include <cstddef>

template<typename T>
struct SpscRing {
    std::vector<T> buffer;
    const size_t mask;
    std::atomic<size_t> head;
    std::atomic<size_t> tail;

    explicit SpscRing(size_t capacity_pow2)
        : buffer(capacity_pow2), mask(capacity_pow2 - 1), head(0), tail(0) {}

    bool push(const T& v) {
        size_t h = head.load(std::memory_order_relaxed);
        size_t t = tail.load(std::memory_order_acquire);
        if (((h + 1) & mask) == (t & mask)) return false; // full
        buffer[h & mask] = v;
        head.store(h + 1, std::memory_order_release);
        return true;
    }

    bool pop(T& out) {
        size_t t = tail.load(std::memory_order_relaxed);
        size_t h = head.load(std::memory_order_acquire);
        if ((t & mask) == (h & mask)) return false; // empty
        out = buffer[t & mask];
        tail.store(t + 1, std::memory_order_release);
        return true;
    }
};

int main() {
    try {
        std::cout << "\n=== LOCK FREE STRUCTS ===" << std::endl;
        SpscRing<int> q(1024);
        for (int i = 0; i < 10; ++i) q.push(i);
        int x;
        int count = 0;
        while (q.pop(x)) { std::cout << x << ' '; ++count; }
        std::cout << "\ncount=" << count << std::endl;
        std::cout << "\n=== LOCK FREE COMPLETED SUCCESSFULLY ===" << std::endl;
    } catch (...) { std::cerr << "error" << std::endl; return 1; }
    return 0;
}
