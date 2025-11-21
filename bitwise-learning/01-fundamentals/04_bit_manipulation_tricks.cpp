/*
 * Bitwise Fundamentals: Advanced Bit Manipulation Tricks
 * 
 * Collection of hacky bit manipulation tricks: next power of 2,
 * is power of 2, round up to power of 2, extract bit ranges.
 */
#include <iostream>
#include <cstdint>
#include <cassert>
#include <climits>

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: x > 0
// Failure modes: Returns 0 if x == 0
static inline uint32_t next_power_of_2(uint32_t x) {
    if (x == 0) return 1;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline bool is_power_of_2(uint32_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: x > 0
// Failure modes: Returns 0 if x == 0
static inline uint32_t round_up_power_of_2(uint32_t x) {
    return next_power_of_2(x);
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: start < end, end <= 32
// Failure modes: Undefined behavior if start >= end or end > 32
static inline uint32_t extract_bits(uint32_t x, unsigned start, unsigned end) {
    assert(start < end && end <= 32);
    uint32_t mask = ((1U << (end - start)) - 1U) << start;
    return (x & mask) >> start;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline uint32_t count_trailing_zeros_manual(uint32_t x) {
    if (x == 0) return 32;
    uint32_t count = 0;
    if ((x & 0x0000FFFFu) == 0) { count += 16; x >>= 16; }
    if ((x & 0x000000FFu) == 0) { count += 8; x >>= 8; }
    if ((x & 0x0000000Fu) == 0) { count += 4; x >>= 4; }
    if ((x & 0x00000003u) == 0) { count += 2; x >>= 2; }
    if ((x & 0x00000001u) == 0) { count += 1; }
    return count;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline uint32_t count_leading_zeros_manual(uint32_t x) {
    if (x == 0) return 32;
    uint32_t count = 0;
    if ((x & 0xFFFF0000u) == 0) { count += 16; x <<= 16; }
    if ((x & 0xFF000000u) == 0) { count += 8; x <<= 8; }
    if ((x & 0xF0000000u) == 0) { count += 4; x <<= 4; }
    if ((x & 0xC0000000u) == 0) { count += 2; x <<= 2; }
    if ((x & 0x80000000u) == 0) { count += 1; }
    return count;
}

int main() {
    std::cout << next_power_of_2(17) << std::endl;
    std::cout << is_power_of_2(16) << " " << is_power_of_2(15) << std::endl;
    std::cout << extract_bits(0x12345678u, 8, 16) << std::endl;
    return 0;
}

