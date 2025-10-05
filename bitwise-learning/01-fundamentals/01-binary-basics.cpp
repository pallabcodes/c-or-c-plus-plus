/*
 * Bitwise Fundamentals: Binary Basics
 */
#include <iostream>
#include <bitset>
#include <cstdint>

static inline bool test_bit(uint32_t x, unsigned b) { return (x >> b) & 1u; }
static inline uint32_t set_bit(uint32_t x, unsigned b) { return x | (1u << b); }
static inline uint32_t clear_bit(uint32_t x, unsigned b) { return x & ~(1u << b); }
static inline uint32_t toggle_bit(uint32_t x, unsigned b) { return x ^ (1u << b); }
static inline uint32_t isolate_lsb(uint32_t x) { return x & -x; }
static inline uint32_t isolate_msb(uint32_t x) { if (!x) return 0; unsigned p = 31 - __builtin_clz(x); return 1u << p; }

int main() {
    uint32_t v = 0b10100100u;
    std::cout << std::bitset<8>(v) << std::endl;
    std::cout << test_bit(v, 2) << std::endl;
    v = set_bit(v, 0);
    v = clear_bit(v, 5);
    v = toggle_bit(v, 2);
    std::cout << std::bitset<8>(v) << std::endl;
    std::cout << "lsb=" << std::bitset<8>(isolate_lsb(v)) << " msb=" << std::bitset<8>(isolate_msb(v)) << std::endl;
    return 0;
}
