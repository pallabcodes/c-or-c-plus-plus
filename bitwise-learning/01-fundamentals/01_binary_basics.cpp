/*
 * Bitwise Fundamentals: Binary Basics
 * 
 * Demonstrates core bit manipulation operations: test, set, clear, toggle,
 * and isolation of least/most significant bits.
 */
#include <iostream>
#include <bitset>
#include <cstdint>
#include <cassert>
#include <limits>

// Thread-safety: Thread-safe (pure functions, no shared state)
// Ownership: None (value semantics)
// Invariants: Bit position must be < 32
// Failure modes: Undefined behavior if b >= 32
static inline bool test_bit(uint32_t x, unsigned b) {
    assert(b < 32);
    return (x >> b) & 1u;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: Bit position must be < 32
// Failure modes: Undefined behavior if b >= 32
static inline uint32_t set_bit(uint32_t x, unsigned b) {
    assert(b < 32);
    return x | (1u << b);
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: Bit position must be < 32
// Failure modes: Undefined behavior if b >= 32
static inline uint32_t clear_bit(uint32_t x, unsigned b) {
    assert(b < 32);
    return x & ~(1u << b);
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: Bit position must be < 32
// Failure modes: Undefined behavior if b >= 32
static inline uint32_t toggle_bit(uint32_t x, unsigned b) {
    assert(b < 32);
    return x ^ (1u << b);
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: Returns 0 if x == 0
static inline uint32_t isolate_lsb(uint32_t x) {
    return x & -x;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: Returns 0 if x == 0
static inline uint32_t isolate_msb(uint32_t x) {
    if (!x) return 0;
    unsigned p = 31 - __builtin_clz(x);
    return 1u << p;
}

int main() {
    uint32_t v = 0b10100100u;
    std::cout << std::bitset<8>(v) << std::endl;
    std::cout << test_bit(v, 2) << std::endl;
    v = set_bit(v, 0);
    v = clear_bit(v, 5);
    v = toggle_bit(v, 2);
    std::cout << std::bitset<8>(v) << std::endl;
    std::cout << "lsb=" << std::bitset<8>(isolate_lsb(v)) 
              << " msb=" << std::bitset<8>(isolate_msb(v)) << std::endl;
    return 0;
}
