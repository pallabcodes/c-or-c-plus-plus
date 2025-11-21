/*
 * System: Memory Barriers and Ordering
 * 
 * Memory barrier patterns for bit manipulation in multi-threaded
 * systems, ensuring correct ordering and visibility.
 */
#include <iostream>
#include <atomic>
#include <cstdint>
#include <cassert>
#include <thread>

// Thread-safety: Thread-safe (atomic with memory ordering)
// Ownership: None (value semantics)
// Invariants: bit < 64
// Failure modes: Undefined behavior if bit >= 64
static inline void set_bit_with_barrier(std::atomic<uint64_t>& bits, unsigned bit) {
    assert(bit < 64);
    uint64_t mask = 1ULL << bit;
    bits.fetch_or(mask, std::memory_order_release);
}

// Thread-safety: Thread-safe (atomic with memory ordering)
// Ownership: None (value semantics)
// Invariants: bit < 64
// Failure modes: Undefined behavior if bit >= 64
static inline bool test_bit_with_barrier(const std::atomic<uint64_t>& bits, unsigned bit) {
    assert(bit < 64);
    uint64_t mask = 1ULL << bit;
    std::atomic_thread_fence(std::memory_order_acquire);
    return (bits.load(std::memory_order_acquire) & mask) != 0;
}

// Thread-safety: Thread-safe (atomic with memory ordering)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline void full_memory_barrier() {
    std::atomic_thread_fence(std::memory_order_seq_cst);
}

// Thread-safety: Thread-safe (atomic with memory ordering)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline void acquire_barrier() {
    std::atomic_thread_fence(std::memory_order_acquire);
}

// Thread-safety: Thread-safe (atomic with memory ordering)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline void release_barrier() {
    std::atomic_thread_fence(std::memory_order_release);
}

struct BarrierBitset {
    std::atomic<uint64_t> bits;
    
    // Thread-safety: Not thread-safe (constructor)
    // Ownership: Owns bits atomic
    // Invariants: None
    // Failure modes: None
    BarrierBitset() : bits(0) {}
    
    // Thread-safety: Thread-safe (atomic with release)
    // Ownership: Modifies owned bits
    // Invariants: i < 64
    // Failure modes: Undefined behavior if i >= 64
    void set_release(size_t i) {
        assert(i < 64);
        set_bit_with_barrier(bits, i);
    }
    
    // Thread-safety: Thread-safe (atomic with acquire)
    // Ownership: None (read-only access)
    // Invariants: i < 64
    // Failure modes: Undefined behavior if i >= 64
    bool test_acquire(size_t i) const {
        assert(i < 64);
        return test_bit_with_barrier(bits, i);
    }
};

int main() {
    BarrierBitset bbs;
    bbs.set_release(10);
    std::cout << bbs.test_acquire(10) << std::endl;
    return 0;
}

