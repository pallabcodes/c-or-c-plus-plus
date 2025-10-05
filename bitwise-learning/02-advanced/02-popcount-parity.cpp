/*
 * Bitwise Advanced: Popcount and Parity
 */
#include <iostream>
#include <cstdint>

static inline int popcnt32(uint32_t x) { return __builtin_popcount(x); }
static inline int clz32(uint32_t x) { return __builtin_clz(x); }
static inline int ctz32(uint32_t x) { return __builtin_ctz(x); }
static inline int parity32(uint32_t x) { return __builtin_parity(x); }

int main() {
    uint32_t v = 0xF0F0F0F1u;
    std::cout << popcnt32(v) << ' ' << clz32(v) << ' ' << ctz32(v) << ' ' << parity32(v) << std::endl;
    return 0;
}
