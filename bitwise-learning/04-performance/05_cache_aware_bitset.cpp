/*
 * Performance: Cache-Aware Bitset
 * 
 * Cache-line aligned bitset for optimal cache performance,
 * using padding and alignment to prevent false sharing.
 */
#include <iostream>
#include <cstdint>
#include <cassert>
#include <cstring>

struct alignas(64) CacheLineBitset {
    uint64_t bits[8];
    char padding[64 - sizeof(uint64_t) * 8];
    
    // Thread-safety: Not thread-safe (constructor)
    // Ownership: Owns bits array
    // Invariants: None
    // Failure modes: None
    CacheLineBitset() {
        std::memset(bits, 0, sizeof(bits));
        std::memset(padding, 0, sizeof(padding));
    }
    
    // Thread-safety: Not thread-safe (modifies bits)
    // Ownership: Modifies owned bits
    // Invariants: i < 512
    // Failure modes: Undefined behavior if i >= 512
    void set(size_t i) {
        assert(i < 512);
        bits[i >> 6] |= (1ULL << (i & 63));
    }
    
    // Thread-safety: Thread-safe (pure function)
    // Ownership: None (read-only access)
    // Invariants: i < 512
    // Failure modes: Undefined behavior if i >= 512
    bool test(size_t i) const {
        assert(i < 512);
        return (bits[i >> 6] & (1ULL << (i & 63))) != 0;
    }
    
    // Thread-safety: Thread-safe (pure function)
    // Ownership: None (value semantics)
    // Invariants: None
    // Failure modes: None
    uint32_t popcount() const {
        uint32_t count = 0;
        for (int i = 0; i < 8; ++i) {
            count += __builtin_popcountll(bits[i]);
        }
        return count;
    }
};

struct CacheAwareBitset {
    std::vector<CacheLineBitset> lines;
    size_t total_bits;
    
    // Thread-safety: Not thread-safe (constructor)
    // Ownership: Owns lines vector
    // Invariants: nbits > 0
    // Failure modes: Undefined behavior if nbits == 0
    explicit CacheAwareBitset(size_t nbits) : total_bits(nbits) {
        assert(nbits > 0);
        size_t num_lines = (nbits + 511) / 512;
        lines.resize(num_lines);
    }
    
    // Thread-safety: Not thread-safe (modifies lines)
    // Ownership: Modifies owned lines
    // Invariants: i < total_bits
    // Failure modes: Undefined behavior if i >= total_bits
    void set(size_t i) {
        assert(i < total_bits);
        size_t line_idx = i / 512;
        size_t bit_idx = i % 512;
        lines[line_idx].set(bit_idx);
    }
    
    // Thread-safety: Thread-safe (pure function)
    // Ownership: None (read-only access)
    // Invariants: i < total_bits
    // Failure modes: Undefined behavior if i >= total_bits
    bool test(size_t i) const {
        assert(i < total_bits);
        size_t line_idx = i / 512;
        size_t bit_idx = i % 512;
        return lines[line_idx].test(bit_idx);
    }
};

int main() {
    CacheAwareBitset cab(2048);
    cab.set(100);
    cab.set(600);
    std::cout << cab.test(100) << " " << cab.test(600) << std::endl;
    return 0;
}

