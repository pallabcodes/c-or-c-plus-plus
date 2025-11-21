/*
 * Bitwise Advanced: Popcount and Parity
 * 
 * Demonstrates builtin bit counting operations: population count,
 * count leading/trailing zeros, and parity.
 */
#include <iostream>
#include <cstdint>

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None (builtin handles all cases)
static inline int popcnt32(uint32_t x) {
    return __builtin_popcount(x);
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: x must not be 0 (undefined behavior if x == 0)
// Failure modes: Undefined behavior if x == 0
static inline int clz32(uint32_t x) {
    return __builtin_clz(x);
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: x must not be 0 (undefined behavior if x == 0)
// Failure modes: Undefined behavior if x == 0
static inline int ctz32(uint32_t x) {
    return __builtin_ctz(x);
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline int parity32(uint32_t x) {
    return __builtin_parity(x);
}

int main() {
    uint32_t v = 0xF0F0F0F1u;
    std::cout << popcnt32(v) << ' ' << clz32(v) << ' ' 
              << ctz32(v) << ' ' << parity32(v) << std::endl;
    return 0;
}
