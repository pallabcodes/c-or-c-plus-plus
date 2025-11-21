/*
 * Bitwise Fundamentals: Bit Rotation
 * 
 * Efficient bit rotation operations using bit manipulation
 * tricks for circular shifts without explicit rotate instructions.
 */
#include <iostream>
#include <cstdint>
#include <cassert>
#include <climits>

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: n < 32
// Failure modes: Undefined behavior if n >= 32
static inline uint32_t rotate_left_32(uint32_t x, unsigned n) {
    assert(n < 32);
    return (x << n) | (x >> (32 - n));
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: n < 32
// Failure modes: Undefined behavior if n >= 32
static inline uint32_t rotate_right_32(uint32_t x, unsigned n) {
    assert(n < 32);
    return (x >> n) | (x << (32 - n));
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: n < 64
// Failure modes: Undefined behavior if n >= 64
static inline uint64_t rotate_left_64(uint64_t x, unsigned n) {
    assert(n < 64);
    return (x << n) | (x >> (64 - n));
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: n < 64
// Failure modes: Undefined behavior if n >= 64
static inline uint64_t rotate_right_64(uint64_t x, unsigned n) {
    assert(n < 64);
    return (x >> n) | (x << (64 - n));
}

int main() {
    uint32_t v = 0x12345678u;
    std::cout << std::hex << v << " -> " << rotate_left_32(v, 8) << std::endl;
    std::cout << std::hex << v << " -> " << rotate_right_32(v, 8) << std::endl;
    return 0;
}

