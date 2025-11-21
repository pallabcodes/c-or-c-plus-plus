/*
 * Bitwise Advanced: Fast Integer Square Root
 * 
 * Hacky bit manipulation tricks for computing integer square root
 * using bit shifts and Newton's method approximations.
 */
#include <iostream>
#include <cstdint>
#include <cassert>
#include <cmath>

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: x >= 0
// Failure modes: Undefined behavior if x < 0
static inline uint32_t fast_sqrt32(uint32_t x) {
    if (x == 0) return 0;
    uint32_t msb = 31 - __builtin_clz(x);
    uint32_t result = 1 << (msb >> 1);
    result = (result + x / result) >> 1;
    result = (result + x / result) >> 1;
    return result;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: x >= 0
// Failure modes: Undefined behavior if x < 0
static inline uint64_t fast_sqrt64(uint64_t x) {
    if (x == 0) return 0;
    uint64_t msb = 63 - __builtin_clzll(x);
    uint64_t result = 1ULL << (msb >> 1);
    result = (result + x / result) >> 1;
    result = (result + x / result) >> 1;
    return result;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: x >= 0
// Failure modes: Undefined behavior if x < 0
static inline uint32_t fast_sqrt_magic(uint32_t x) {
    if (x == 0) return 0;
    uint32_t msb = 31 - __builtin_clz(x);
    uint32_t shift = (msb - 1) >> 1;
    uint32_t result = 1 << shift;
    uint32_t x_shifted = x >> (msb - shift - shift);
    result = (result + x_shifted) >> 1;
    return result;
}

int main() {
    for (uint32_t i = 1; i < 100; i += 7) {
        uint32_t sqrt_val = fast_sqrt32(i);
        uint32_t expected = static_cast<uint32_t>(std::sqrt(i));
        std::cout << i << " -> " << sqrt_val << " (expected: " << expected << ")" << std::endl;
    }
    return 0;
}

