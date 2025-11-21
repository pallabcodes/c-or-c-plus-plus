/*
 * Bitwise Advanced: Branchless Conditionals
 * 
 * Hacky branchless implementations using bit manipulation
 * to avoid branch misprediction penalties.
 */
#include <iostream>
#include <cstdint>
#include <cassert>
#include <climits>

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline int32_t branchless_max(int32_t a, int32_t b) {
    int32_t diff = a - b;
    int32_t sign = diff >> 31;
    return a + (diff & sign);
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline int32_t branchless_abs(int32_t x) {
    int32_t mask = x >> 31;
    return (x ^ mask) - mask;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline int32_t branchless_sign(int32_t x) {
    return (x >> 31) | (-x >> 31);
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline uint32_t branchless_conditional(uint32_t condition, uint32_t a, uint32_t b) {
    uint32_t mask = -(condition != 0);
    return (a & mask) | (b & ~mask);
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: divisor must be power of 2
// Failure modes: Undefined behavior if divisor is not power of 2
static inline uint32_t fast_mod_power2(uint32_t x, uint32_t divisor) {
    assert((divisor & (divisor - 1)) == 0);
    return x & (divisor - 1);
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: divisor must be power of 2
// Failure modes: Undefined behavior if divisor is not power of 2
static inline uint32_t fast_div_power2(uint32_t x, uint32_t divisor) {
    assert((divisor & (divisor - 1)) == 0);
    unsigned shift = __builtin_ctz(divisor);
    return x >> shift;
}

int main() {
    std::cout << branchless_max(10, 5) << std::endl;
    std::cout << branchless_abs(-42) << std::endl;
    std::cout << branchless_conditional(1, 100, 200) << std::endl;
    std::cout << fast_mod_power2(17, 8) << std::endl;
    return 0;
}

