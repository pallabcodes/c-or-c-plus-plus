/*
 * System: Lock-Free Bit Manipulation
 * 
 * Lock-free bit operations using compare-and-swap (CAS)
 * for high-performance concurrent systems.
 */
#include <iostream>
#include <atomic>
#include <cstdint>
#include <cassert>

// Thread-safety: Thread-safe (lock-free atomic operations)
// Ownership: None (value semantics)
// Invariants: bit < 64
// Failure modes: Undefined behavior if bit >= 64
static inline bool lock_free_set_bit(std::atomic<uint64_t>& bits, unsigned bit) {
    assert(bit < 64);
    uint64_t mask = 1ULL << bit;
    uint64_t old = bits.load(std::memory_order_acquire);
    uint64_t desired;
    do {
        desired = old | mask;
        if (desired == old) {
            return false;
        }
    } while (!bits.compare_exchange_weak(old, desired,
                                         std::memory_order_acq_rel,
                                         std::memory_order_acquire));
    return true;
}

// Thread-safety: Thread-safe (lock-free atomic operations)
// Ownership: None (value semantics)
// Invariants: bit < 64
// Failure modes: Undefined behavior if bit >= 64
static inline bool lock_free_clear_bit(std::atomic<uint64_t>& bits, unsigned bit) {
    assert(bit < 64);
    uint64_t mask = 1ULL << bit;
    uint64_t old = bits.load(std::memory_order_acquire);
    uint64_t desired;
    do {
        desired = old & ~mask;
        if (desired == old) {
            return false;
        }
    } while (!bits.compare_exchange_weak(old, desired,
                                         std::memory_order_acq_rel,
                                         std::memory_order_acquire));
    return true;
}

// Thread-safety: Thread-safe (lock-free atomic operations)
// Ownership: None (value semantics)
// Invariants: bit < 64
// Failure modes: Undefined behavior if bit >= 64
static inline bool lock_free_toggle_bit(std::atomic<uint64_t>& bits, unsigned bit) {
    assert(bit < 64);
    uint64_t mask = 1ULL << bit;
    uint64_t old = bits.load(std::memory_order_acquire);
    uint64_t desired;
    do {
        desired = old ^ mask;
    } while (!bits.compare_exchange_weak(old, desired,
                                         std::memory_order_acq_rel,
                                         std::memory_order_acquire));
    return (old & mask) != 0;
}

// Thread-safety: Thread-safe (lock-free atomic operations)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline uint32_t lock_free_popcount(const std::atomic<uint64_t>& bits) {
    uint64_t val = bits.load(std::memory_order_acquire);
    return __builtin_popcountll(val);
}

int main() {
    std::atomic<uint64_t> bits(0);
    lock_free_set_bit(bits, 5);
    lock_free_set_bit(bits, 10);
    std::cout << lock_free_popcount(bits) << std::endl;
    lock_free_toggle_bit(bits, 5);
    std::cout << lock_free_popcount(bits) << std::endl;
    return 0;
}

