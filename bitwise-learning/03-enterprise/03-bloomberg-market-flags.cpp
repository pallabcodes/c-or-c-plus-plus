/*
 * Enterprise: Bloomberg Style Market Flags
 */
#include <iostream>
#include <cstdint>

enum : uint32_t {
    MF_HALTED      = 1u<<0,
    MF_AUCTION     = 1u<<1,
    MF_LULD        = 1u<<2,
    MF_SHORT_SALE  = 1u<<3,
    MF_ODD_LOT     = 1u<<4,
};

static inline bool has(uint32_t f, uint32_t m) { return (f & m) != 0; }

int main() {
    uint32_t f = MF_AUCTION | MF_LULD;
    std::cout << has(f, MF_AUCTION) << ' ' << has(f, MF_HALTED) << std::endl;
    return 0;
}
