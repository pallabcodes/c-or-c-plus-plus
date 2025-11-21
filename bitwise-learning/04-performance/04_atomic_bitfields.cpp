/*
 * Performance: Atomic Bitfields
 * 
 * Lock-free bit manipulation using atomic operations for
 * high-performance concurrent bit operations.
 */
#include <iostream>
#include <atomic>
#include <cstdint>
#include <cassert>
#include <thread>
#include <vector>

// Thread-safety: Thread-safe (atomic operations)
// Ownership: None (value semantics)
// Invariants: bit < 64
// Failure modes: Undefined behavior if bit >= 64
static inline void atomic_set_bit(std::atomic<uint64_t>& bits, unsigned bit) {
    assert(bit < 64);
    uint64_t mask = 1ULL << bit;
    bits.fetch_or(mask, std::memory_order_acq_rel);
}

// Thread-safety: Thread-safe (atomic operations)
// Ownership: None (value semantics)
// Invariants: bit < 64
// Failure modes: Undefined behavior if bit >= 64
static inline void atomic_clear_bit(std::atomic<uint64_t>& bits, unsigned bit) {
    assert(bit < 64);
    uint64_t mask = 1ULL << bit;
    bits.fetch_and(~mask, std::memory_order_acq_rel);
}

// Thread-safety: Thread-safe (atomic operations)
// Ownership: None (value semantics)
// Invariants: bit < 64
// Failure modes: Undefined behavior if bit >= 64
static inline bool atomic_test_bit(const std::atomic<uint64_t>& bits, unsigned bit) {
    assert(bit < 64);
    uint64_t mask = 1ULL << bit;
    return (bits.load(std::memory_order_acquire) & mask) != 0;
}

// Thread-safety: Thread-safe (atomic operations)
// Ownership: None (value semantics)
// Invariants: bit < 64
// Failure modes: Returns false if bit was already set
static inline bool atomic_test_and_set(std::atomic<uint64_t>& bits, unsigned bit) {
    assert(bit < 64);
    uint64_t mask = 1ULL << bit;
    uint64_t old = bits.fetch_or(mask, std::memory_order_acq_rel);
    return (old & mask) == 0;
}

struct AtomicBitSet {
    std::vector<std::atomic<uint64_t>> bits;
    
    // Thread-safety: Not thread-safe (constructor)
    // Ownership: Owns bits vector
    // Invariants: nbits > 0
    // Failure modes: Undefined behavior if nbits == 0
    explicit AtomicBitSet(size_t nbits) : bits((nbits + 63) / 64) {
        assert(nbits > 0);
        for (auto& b : bits) {
            b.store(0, std::memory_order_relaxed);
        }
    }
    
    // Thread-safety: Thread-safe (atomic operations)
    // Ownership: Modifies owned bits
    // Invariants: i < bits.size() * 64
    // Failure modes: Undefined behavior if i >= bits.size() * 64
    void set(size_t i) {
        assert(i < bits.size() * 64);
        atomic_set_bit(bits[i >> 6], i & 63);
    }
    
    // Thread-safety: Thread-safe (atomic operations)
    // Ownership: Modifies owned bits
    // Invariants: i < bits.size() * 64
    // Failure modes: Undefined behavior if i >= bits.size() * 64
    bool test_and_set(size_t i) {
        assert(i < bits.size() * 64);
        return atomic_test_and_set(bits[i >> 6], i & 63);
    }
    
    // Thread-safety: Thread-safe (atomic operations)
    // Ownership: None (read-only access)
    // Invariants: i < bits.size() * 64
    // Failure modes: Undefined behavior if i >= bits.size() * 64
    bool test(size_t i) const {
        assert(i < bits.size() * 64);
        return atomic_test_bit(bits[i >> 6], i & 63);
    }
};

int main() {
    AtomicBitSet abs(1024);
    abs.set(100);
    std::cout << abs.test(100) << " " << abs.test(101) << std::endl;
    std::cout << abs.test_and_set(100) << " " << abs.test_and_set(200) << std::endl;
    return 0;
}

