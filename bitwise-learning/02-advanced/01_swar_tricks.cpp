/*
 * Bitwise Advanced: SWAR Tricks
 * 
 * SWAR (SIMD Within A Register) techniques for parallel byte operations
 * without explicit SIMD instructions.
 */
#include <iostream>
#include <cstdint>

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: Operates on 4 bytes packed in uint32_t
// Failure modes: Overflow in byte addition wraps (expected behavior)
static inline uint32_t swar_add_bytes(uint32_t x, uint32_t y) {
    return (x & 0x00FF00FFu) + (y & 0x00FF00FFu) + (((x ^ y) & 0xFF00FF00u) >> 8);
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: Operates on 4 bytes packed in uint32_t
// Failure modes: None
static inline uint32_t swar_min_bytes(uint32_t x, uint32_t y) {
    uint32_t diff = x ^ y;
    uint32_t mask = ((x - y) ^ x ^ diff) & 0x80808080u;
    return (x & mask) | (y & ~mask);
}

int main() {
    uint32_t a = 0x10203040u;
    uint32_t b = 0x01010101u;
    std::cout << std::hex << swar_add_bytes(a, b) << std::endl;
    std::cout << std::hex << swar_min_bytes(a, b) << std::endl;
    return 0;
}
