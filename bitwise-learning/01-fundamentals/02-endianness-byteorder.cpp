/*
 * Bitwise Fundamentals: Endianness and Byte Order
 */
#include <iostream>
#include <cstdint>
#include <cstring>

static inline bool is_little_endian() {
    uint16_t x = 1; return *reinterpret_cast<uint8_t*>(&x) == 1;
}

static inline uint32_t bswap32(uint32_t x) {
    return __builtin_bswap32(x);
}

int main() {
    std::cout << (is_little_endian() ? "little" : "big") << std::endl;
    uint32_t v = 0x01020304u;
    uint32_t r = bswap32(v);
    std::cout << std::hex << v << " -> " << r << std::endl;
    return 0;
}
