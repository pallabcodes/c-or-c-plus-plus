/*
 * Bitwise System: Register Masks
 * 
 * Demonstrates hardware register manipulation patterns using
 * bit masks for device control and status checking.
 */
#include <iostream>
#include <cstdint>

struct Reg {
    uint32_t v;
};

// Thread-safety: Not thread-safe (modifies r)
// Ownership: Modifies r
// Invariants: None
// Failure modes: None
static inline void set_bits(Reg& r, uint32_t mask) {
    r.v |= mask;
}

// Thread-safety: Not thread-safe (modifies r)
// Ownership: Modifies r
// Invariants: None
// Failure modes: None
static inline void clear_bits(Reg& r, uint32_t mask) {
    r.v &= ~mask;
}

// Thread-safety: Thread-safe (const method, read-only)
// Ownership: None (read-only access)
// Invariants: None
// Failure modes: None
static inline bool has_bits(const Reg& r, uint32_t mask) {
    return (r.v & mask) == mask;
}

int main() {
    constexpr uint32_t ENABLE = 1u << 0;
    constexpr uint32_t RESET = 1u << 1;
    Reg r{0};
    set_bits(r, ENABLE);
    std::cout << has_bits(r, ENABLE) << std::endl;
    clear_bits(r, ENABLE);
    set_bits(r, RESET);
    std::cout << std::hex << r.v << std::endl;
    return 0;
}
