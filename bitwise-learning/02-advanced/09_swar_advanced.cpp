/*
 * Bitwise Advanced: Advanced SWAR Techniques
 * 
 * More sophisticated SWAR tricks: parallel comparisons, min/max,
 * absolute value, and arithmetic operations on packed bytes/words.
 */
#include <iostream>
#include <cstdint>
#include <cassert>
#include <climits>

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: Operates on 4 bytes packed in uint32_t
// Failure modes: Overflow in byte operations wraps (expected)
static inline uint32_t swar_max_bytes(uint32_t x, uint32_t y) {
    uint32_t diff = x - y;
    uint32_t sign = diff >> 31;
    uint32_t mask = sign | (sign << 8) | (sign << 16) | (sign << 24);
    return (x & ~mask) | (y & mask);
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: Operates on 4 bytes packed in uint32_t
// Failure modes: None
static inline uint32_t swar_abs_bytes(uint32_t x) {
    uint32_t sign = x >> 31;
    uint32_t mask = sign | (sign << 8) | (sign << 16) | (sign << 24);
    return (x ^ mask) - mask;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: Operates on 4 bytes packed in uint32_t
// Failure modes: Overflow wraps (expected)
static inline uint32_t swar_multiply_bytes(uint32_t x, uint32_t y) {
    uint32_t low = (x & 0x00FF00FFu) * (y & 0x00FF00FFu);
    uint32_t high = ((x >> 8) & 0x00FF00FFu) * ((y >> 8) & 0x00FF00FFu);
    return (low & 0x00FF00FFu) | ((high & 0x00FF00FFu) << 8);
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: Operates on 4 bytes packed in uint32_t
// Failure modes: None
static inline uint32_t swar_compare_bytes(uint32_t x, uint32_t y) {
    uint32_t diff = x - y;
    uint32_t sign = diff >> 31;
    uint32_t mask = sign | (sign << 8) | (sign << 16) | (sign << 24);
    return mask;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: Operates on 2 16-bit words packed in uint32_t
// Failure modes: Overflow wraps (expected)
static inline uint32_t swar_add_words(uint32_t x, uint32_t y) {
    return (x & 0xFFFFu) + (y & 0xFFFFu) + 
           ((((x >> 16) + (y >> 16)) & 0xFFFFu) << 16);
}

int main() {
    uint32_t a = 0x10203040u;
    uint32_t b = 0x01010101u;
    std::cout << std::hex << swar_max_bytes(a, b) << std::endl;
    std::cout << std::hex << swar_abs_bytes(0x80000000u) << std::endl;
    return 0;
}

