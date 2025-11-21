/*
 * Enterprise: Hash Mixing Functions
 * 
 * Production-grade hash mixing functions used in Google's hash tables,
 * combining multiple bit manipulation tricks for excellent distribution.
 */
#include <iostream>
#include <cstdint>
#include <cassert>

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline uint64_t google_mix64(uint64_t x) {
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdULL;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ULL;
    x ^= x >> 33;
    return x;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline uint32_t murmur_hash3_32(uint32_t x) {
    x ^= x >> 16;
    x *= 0x85ebca6b;
    x ^= x >> 13;
    x *= 0xc2b2ae35;
    x ^= x >> 16;
    return x;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline uint64_t wyhash_mix(uint64_t x) {
    x ^= x >> 32;
    x *= 0xbf58476d1ce4e5b9ULL;
    x ^= x >> 32;
    x *= 0x94d049bb133111ebULL;
    x ^= x >> 32;
    return x;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline uint64_t cityhash_mix(uint64_t x) {
    x *= 0x9ddfea08eb382d69ULL;
    x ^= x >> 27;
    x *= 0x9e3779b97f4a7c15ULL;
    x ^= x >> 28;
    return x;
}

int main() {
    uint64_t key = 0x1234567890ABCDEFULL;
    std::cout << std::hex << google_mix64(key) << std::endl;
    std::cout << std::hex << murmur_hash3_32(static_cast<uint32_t>(key)) << std::endl;
    std::cout << std::hex << wyhash_mix(key) << std::endl;
    std::cout << std::hex << cityhash_mix(key) << std::endl;
    return 0;
}

