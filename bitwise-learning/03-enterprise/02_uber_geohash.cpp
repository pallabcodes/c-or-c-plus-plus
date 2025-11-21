/*
 * Enterprise: Uber Style Geohash (Morton encoding)
 * 
 * Z-order curve encoding for spatial indexing, interleaving
 * latitude and longitude bits for efficient range queries.
 */
#include <iostream>
#include <cstdint>

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline uint64_t part1by1(uint32_t x) {
    uint64_t v = x;
    v = (v | (v << 16)) & 0x0000FFFF0000FFFFULL;
    v = (v | (v << 8)) & 0x00FF00FF00FF00FFULL;
    v = (v | (v << 4)) & 0x0F0F0F0F0F0F0F0FULL;
    v = (v | (v << 2)) & 0x3333333333333333ULL;
    v = (v | (v << 1)) & 0x5555555555555555ULL;
    return v;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline uint64_t morton_encode(uint32_t lat_q, uint32_t lon_q) {
    return (part1by1(lon_q) << 1) | part1by1(lat_q);
}

int main() {
    uint32_t lat_q = 2147483648u;
    uint32_t lon_q = 2147483648u;
    std::cout << std::hex << morton_encode(lat_q, lon_q) << std::endl;
    return 0;
}
