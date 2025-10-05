/*
 * Bitwise System: Register Masks
 */
#include <iostream>
#include <cstdint>

struct Reg { uint32_t v; };

static inline void set_bits(Reg& r, uint32_t mask) { r.v |= mask; }
static inline void clear_bits(Reg& r, uint32_t mask) { r.v &= ~mask; }
static inline bool has_bits(const Reg& r, uint32_t mask) { return (r.v & mask) == mask; }

int main() {
    constexpr uint32_t ENABLE=1u<<0, RESET=1u<<1;
    Reg r{0};
    set_bits(r, ENABLE);
    std::cout << has_bits(r, ENABLE) << std::endl;
    clear_bits(r, ENABLE);
    set_bits(r, RESET);
    std::cout << std::hex << r.v << std::endl;
    return 0;
}
