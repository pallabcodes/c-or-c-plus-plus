/*
 * Bitwise Advanced: Bitboards
 * 
 * Demonstrates bitboard representation for chess-like board games,
 * where each bit represents a square on an 8x8 board.
 */
#include <iostream>
#include <cstdint>
#include <bitset>

using U64 = uint64_t;

// Thread-safety: Thread-safe (pure function, no shared state)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline void print_bb(U64 bb) {
    for (int r = 7; r >= 0; --r) {
        for (int f = 0; f < 8; ++f) {
            int sq = r * 8 + f;
            std::cout << (((bb >> sq) & 1ull) ? '1' : '.');
        }
        std::cout << '\n';
    }
}

int main() {
    U64 knights = (1ull << 1) | (1ull << 6) | (1ull << 57) | (1ull << 62);
    print_bb(knights);
    return 0;
}
