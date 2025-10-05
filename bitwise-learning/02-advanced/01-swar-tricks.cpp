/*
 * Bitwise Advanced: SWAR Tricks
 */
#include <iostream>
#include <cstdint>

static inline uint32_t swar_add_bytes(uint32_t x, uint32_t y) {
    return (x & 0x00FF00FFu) + (y & 0x00FF00FFu) + (((x ^ y) & 0xFF00FF00u) >> 8);
}

static inline uint32_t swar_min_bytes(uint32_t x, uint32_t y) {
    uint32_t diff = x ^ y;
    uint32_t mask = ((x - y) ^ x ^ diff) & 0x80808080u; // select per byte
    return (x & mask) | (y & ~mask);
}

int main() {
    uint32_t a = 0x10203040u;
    uint32_t b = 0x01010101u;
    std::cout << std::hex << swar_add_bytes(a,b) << std::endl;
    std::cout << std::hex << swar_min_bytes(a,b) << std::endl;
    return 0;
}
