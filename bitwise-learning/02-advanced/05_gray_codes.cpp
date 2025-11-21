/*
 * Bitwise Advanced: Gray Codes
 * 
 * Gray code encoding/decoding for error correction and
 * sequential encoding where adjacent values differ by one bit.
 */
#include <iostream>
#include <cstdint>
#include <cassert>

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline uint32_t binary_to_gray(uint32_t n) {
    return n ^ (n >> 1);
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline uint32_t gray_to_binary(uint32_t gray) {
    uint32_t mask = gray;
    while (mask) {
        mask >>= 1;
        gray ^= mask;
    }
    return gray;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline uint32_t next_gray(uint32_t gray) {
    return binary_to_gray(gray_to_binary(gray) + 1);
}

int main() {
    for (uint32_t i = 0; i < 16; ++i) {
        uint32_t gray = binary_to_gray(i);
        uint32_t back = gray_to_binary(gray);
        std::cout << i << " -> " << gray << " -> " << back << std::endl;
    }
    return 0;
}

