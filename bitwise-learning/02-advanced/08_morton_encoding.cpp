/*
 * Bitwise Advanced: Morton Encoding (Z-Order Curve)
 * 
 * Space-filling curve encoding for efficient spatial queries,
 * interleaving bits of coordinates for locality preservation.
 */
#include <iostream>
#include <cstdint>
#include <cassert>

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: x, y < (1 << 16)
// Failure modes: Undefined behavior if coordinates exceed 16 bits
static inline uint32_t morton_encode_2d(uint16_t x, uint16_t y) {
    uint32_t code = 0;
    for (int i = 0; i < 16; ++i) {
        code |= ((x >> i) & 1) << (2 * i);
        code |= ((y >> i) & 1) << (2 * i + 1);
    }
    return code;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: code is valid Morton code
// Failure modes: Undefined behavior if code is invalid
static inline void morton_decode_2d(uint32_t code, uint16_t* x, uint16_t* y) {
    assert(x != nullptr && y != nullptr);
    *x = 0;
    *y = 0;
    for (int i = 0; i < 16; ++i) {
        *x |= ((code >> (2 * i)) & 1) << i;
        *y |= ((code >> (2 * i + 1)) & 1) << i;
    }
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: x, y, z < (1 << 10)
// Failure modes: Undefined behavior if coordinates exceed 10 bits
static inline uint32_t morton_encode_3d(uint16_t x, uint16_t y, uint16_t z) {
    assert(x < (1 << 10) && y < (1 << 10) && z < (1 << 10));
    uint32_t code = 0;
    for (int i = 0; i < 10; ++i) {
        code |= ((x >> i) & 1) << (3 * i);
        code |= ((y >> i) & 1) << (3 * i + 1);
        code |= ((z >> i) & 1) << (3 * i + 2);
    }
    return code;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: code is valid 3D Morton code
// Failure modes: Undefined behavior if code is invalid
static inline void morton_decode_3d(uint32_t code, uint16_t* x, uint16_t* y, uint16_t* z) {
    assert(x != nullptr && y != nullptr && z != nullptr);
    *x = 0;
    *y = 0;
    *z = 0;
    for (int i = 0; i < 10; ++i) {
        *x |= ((code >> (3 * i)) & 1) << i;
        *y |= ((code >> (3 * i + 1)) & 1) << i;
        *z |= ((code >> (3 * i + 2)) & 1) << i;
    }
}

int main() {
    uint16_t x = 5, y = 3;
    uint32_t code = morton_encode_2d(x, y);
    std::cout << "(" << x << "," << y << ") -> " << code << std::endl;
    
    uint16_t decoded_x, decoded_y;
    morton_decode_2d(code, &decoded_x, &decoded_y);
    std::cout << code << " -> (" << decoded_x << "," << decoded_y << ")" << std::endl;
    return 0;
}

