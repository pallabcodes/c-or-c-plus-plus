/*
 * Enterprise: Google Style Bloom Filter
 * 
 * Probabilistic data structure for membership testing using
 * multiple hash functions and a bit array.
 */
#include <vector>
#include <cstdint>
#include <iostream>
#include <cassert>

struct Bloom {
    std::vector<uint64_t> bits;
    uint32_t k;

    // Thread-safety: Not thread-safe (mutable state)
    // Ownership: Owns bits vector
    // Invariants: k > 0, bits.size() > 0
    // Failure modes: Undefined behavior if k == 0 or m_bits == 0
    explicit Bloom(size_t m_bits, uint32_t k_hashes) 
        : bits((m_bits + 63) / 64), k(k_hashes) {
        assert(k_hashes > 0);
        assert(m_bits > 0);
    }

    // Thread-safety: Thread-safe (pure function)
    // Ownership: None (value semantics)
    // Invariants: None
    // Failure modes: None
    static inline uint64_t mix64(uint64_t x) {
        x ^= x >> 33;
        x *= 0xff51afd7ed558ccdULL;
        x ^= x >> 33;
        x *= 0xc4ceb9fe1a85ec53ULL;
        x ^= x >> 33;
        return x;
    }

    // Thread-safety: Thread-safe (pure function)
    // Ownership: None (value semantics)
    // Invariants: None
    // Failure modes: None
    static inline uint64_t mix64_2(uint64_t x) {
        x ^= x >> 30;
        x *= 0xbf58476d1ce4e5b9ULL;
        x ^= x >> 27;
        x *= 0x94d049bb133111ebULL;
        x ^= x >> 31;
        return x;
    }

    // Thread-safety: Not thread-safe (modifies bits)
    // Ownership: Modifies owned bits
    // Invariants: k > 0, bits.size() > 0
    // Failure modes: None (bounds checked via modulo)
    void add(uint64_t key) {
        uint64_t h1 = mix64(key);
        uint64_t h2 = mix64_2(key);
        size_t m = bits.size() * 64;
        for (uint32_t i = 0; i < k; ++i) {
            uint64_t h = h1 + i * h2;
            size_t idx = (h % m);
            bits[idx >> 6] |= (1ull << (idx & 63));
        }
    }

    // Thread-safety: Thread-safe for concurrent reads (const method)
    // Ownership: None (read-only access)
    // Invariants: k > 0, bits.size() > 0
    // Failure modes: None (bounds checked via modulo)
    bool possibly_contains(uint64_t key) const {
        uint64_t h1 = mix64(key);
        uint64_t h2 = mix64_2(key);
        size_t m = bits.size() * 64;
        for (uint32_t i = 0; i < k; ++i) {
            uint64_t h = h1 + i * h2;
            size_t idx = (h % m);
            if ((bits[idx >> 6] & (1ull << (idx & 63))) == 0) {
                return false;
            }
        }
        return true;
    }
};

int main() {
    Bloom bf(1 << 20, 7);
    for (uint64_t i = 0; i < 1000; ++i) {
        bf.add(i);
    }
    std::cout << bf.possibly_contains(10) << " " 
              << bf.possibly_contains(1000000) << std::endl;
    return 0;
}
