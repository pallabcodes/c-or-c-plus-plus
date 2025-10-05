/*
 * Enterprise: Google Style Bloom Filter
 */
#include <vector>
#include <cstdint>
#include <iostream>

struct Bloom {
    std::vector<uint64_t> bits;
    uint32_t k;
    explicit Bloom(size_t m_bits, uint32_t k_hashes) : bits((m_bits+63)/64), k(k_hashes) {}

    static inline uint64_t mix64(uint64_t x) {
        x ^= x >> 33; x *= 0xff51afd7ed558ccdULL;
        x ^= x >> 33; x *= 0xc4ceb9fe1a85ec53ULL;
        x ^= x >> 33; return x;
    }
    static inline uint64_t mix64_2(uint64_t x) {
        x ^= x >> 30; x *= 0xbf58476d1ce4e5b9ULL;
        x ^= x >> 27; x *= 0x94d049bb133111ebULL;
        x ^= x >> 31; return x;
    }

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

    bool possibly_contains(uint64_t key) const {
        uint64_t h1 = mix64(key);
        uint64_t h2 = mix64_2(key);
        size_t m = bits.size() * 64;
        for (uint32_t i = 0; i < k; ++i) {
            uint64_t h = h1 + i * h2;
            size_t idx = (h % m);
            if ((bits[idx >> 6] & (1ull << (idx & 63))) == 0) return false;
        }
        return true;
    }
};

int main() {
    Bloom bf(1<<20, 7);
    for (uint64_t i = 0; i < 1000; ++i) bf.add(i);
    std::cout << bf.possibly_contains(10) << " " << bf.possibly_contains(1000000) << std::endl;
    return 0;
}
