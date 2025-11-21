/*
 * Bitwise Performance: BMI2 PDEP/PEXT
 * 
 * Parallel deposit/extract operations using BMI2 instructions
 * for efficient bit manipulation. Falls back to software
 * implementation on non-BMI2 platforms.
 */
#include <iostream>
#include <cstdint>
#ifdef __x86_64__
#include <immintrin.h>
#endif

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline uint64_t pdep64(uint64_t src, uint64_t mask) {
#ifdef __BMI2__
    return _pdep_u64(src, mask);
#else
    uint64_t res = 0;
    uint64_t bb = 1;
    for (uint64_t m = mask; m; m &= (m - 1)) {
        uint64_t bit = m & -m;
        if (src & bb) {
            res |= bit;
        }
        bb <<= 1;
    }
    return res;
#endif
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline uint64_t pext64(uint64_t src, uint64_t mask) {
#ifdef __BMI2__
    return _pext_u64(src, mask);
#else
    uint64_t res = 0;
    unsigned s = 0;
    for (uint64_t m = mask; m; m &= (m - 1)) {
        uint64_t bit = m & -m;
        if (src & bit) {
            res |= (1ull << s);
        }
        ++s;
    }
    return res;
#endif
}

int main() {
    uint64_t src = 0b1111000011110000ull;
    uint64_t mask = 0b0000000011111111ull;
    std::cout << std::hex << pdep64(0xABCDull, mask) << ' ' 
              << pext64(src, mask) << std::endl;
    return 0;
}
