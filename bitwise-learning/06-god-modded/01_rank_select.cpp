/*
 * God-Modded: Succinct Bitvector (rank/select)
 * 
 * Succinct data structure supporting O(1) rank queries and
 * O(log n) select queries with minimal space overhead.
 */
#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>
#include <limits>

struct BitVec {
    std::vector<uint64_t> bits;
    std::vector<uint32_t> rankL1;
    std::vector<uint16_t> rankL2;

    // Thread-safety: Not thread-safe (mutable state)
    // Ownership: Owns bits, rankL1, rankL2 vectors
    // Invariants: nbits > 0
    // Failure modes: Undefined behavior if nbits == 0
    explicit BitVec(size_t nbits) : bits((nbits + 63) / 64) {
        assert(nbits > 0);
    }

    // Thread-safety: Not thread-safe (modifies bits)
    // Ownership: Modifies owned bits
    // Invariants: i < bits.size() * 64
    // Failure modes: Undefined behavior if i >= bits.size() * 64
    void set(size_t i) {
        assert(i < bits.size() * 64);
        bits[i >> 6] |= (1ull << (i & 63));
    }

    // Thread-safety: Thread-safe (pure function)
    // Ownership: None (value semantics)
    // Invariants: None
    // Failure modes: None
    static inline uint32_t popcnt64(uint64_t x) {
        return static_cast<uint32_t>(__builtin_popcountll(x));
    }

    // Thread-safety: Not thread-safe (modifies rankL1, rankL2)
    // Ownership: Modifies owned rankL1, rankL2
    // Invariants: bits.size() > 0
    // Failure modes: None
    void build() {
        size_t n64 = bits.size();
        rankL1.resize((n64 + 7) / 8 + 1);
        rankL2.resize(n64 + 1);
        uint32_t total = 0;
        for (size_t i = 0; i < n64; ++i) {
            if ((i & 7) == 0) {
                rankL1[i >> 3] = total;
            }
            rankL2[i] = total - rankL1[i >> 3];
            total += popcnt64(bits[i]);
        }
        rankL2[n64] = total - rankL1[n64 >> 3];
        rankL1[(n64 + 7) >> 3] = total;
    }

    // Thread-safety: Thread-safe for concurrent reads (const method)
    // Ownership: None (read-only access)
    // Invariants: i <= bits.size() * 64
    // Failure modes: Undefined behavior if i > bits.size() * 64
    uint32_t rank1(size_t i) const {
        assert(i <= bits.size() * 64);
        size_t word = i >> 6;
        uint32_t base = rankL1[word >> 3] + rankL2[word];
        uint64_t mask = (i & 63) ? ((1ull << (i & 63)) - 1ull) : 0ull;
        return base + popcnt64(bits[word] & mask);
    }

    // Thread-safety: Thread-safe for concurrent reads (const method)
    // Ownership: None (read-only access)
    // Invariants: k > 0
    // Failure modes: Returns (size_t)-1 if k exceeds number of set bits
    size_t select1(uint32_t k) const {
        assert(k > 0);
        uint32_t acc = 0;
        for (size_t i = 0; i < bits.size(); ++i) {
            uint32_t c = popcnt64(bits[i]);
            if (acc + c >= k) {
                uint64_t w = bits[i];
                for (int b = 0; b < 64; ++b) {
                    if (w & (1ull << b)) {
                        ++acc;
                        if (acc == k) {
                            return (i << 6) + b;
                        }
                    }
                }
            }
            acc += c;
        }
        return std::numeric_limits<size_t>::max();
    }
};

int main() {
    BitVec bv(256);
    for (int i = 0; i < 256; i += 3) {
        bv.set(i);
    }
    bv.build();
    std::cout << bv.rank1(100) << ' ' << bv.select1(10) << std::endl;
    return 0;
}

