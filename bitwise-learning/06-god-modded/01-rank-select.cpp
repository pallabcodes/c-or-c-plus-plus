/*
 * God-Modded: Succinct Bitvector (rank/select)
 */
#include <iostream>
#include <vector>
#include <cstdint>

struct BitVec {
    std::vector<uint64_t> bits;
    std::vector<uint32_t> rankL1; // per 512 bits
    std::vector<uint16_t> rankL2; // per 64 bits within block

    explicit BitVec(size_t nbits) : bits((nbits+63)/64) {}

    void set(size_t i) { bits[i>>6] |= (1ull << (i & 63)); }

    static inline uint32_t popcnt64(uint64_t x) { return (uint32_t)__builtin_popcountll(x); }

    void build() {
        size_t n64 = bits.size();
        rankL1.resize((n64+7)/8 + 1);
        rankL2.resize(n64 + 1);
        uint32_t total = 0;
        for (size_t i = 0; i < n64; ++i) {
            if ((i & 7) == 0) rankL1[i>>3] = total;
            rankL2[i] = total - rankL1[i>>3];
            total += popcnt64(bits[i]);
        }
        rankL2[n64] = total - rankL1[n64>>3];
        rankL1[(n64+7)>>3] = total;
    }

    uint32_t rank1(size_t i) const { // count 1s in [0, i)
        size_t word = i >> 6;
        uint32_t base = rankL1[word>>3] + rankL2[word];
        uint64_t mask = (i & 63) ? ((1ull << (i & 63)) - 1ull) : 0ull;
        return base + popcnt64(bits[word] & mask);
    }

    size_t select1(uint32_t k) const { // naive scan for demo
        uint32_t acc = 0;
        for (size_t i = 0; i < bits.size(); ++i) {
            uint32_t c = popcnt64(bits[i]);
            if (acc + c >= k) {
                uint64_t w = bits[i];
                for (int b = 0; b < 64; ++b) {
                    if (w & (1ull<<b)) { ++acc; if (acc == k) return (i<<6) + b; }
                }
            }
            acc += c;
        }
        return (size_t)-1;
    }
};

int main() {
    BitVec bv(256);
    for (int i = 0; i < 256; i += 3) bv.set(i);
    bv.build();
    std::cout << bv.rank1(100) << ' ' << bv.select1(10) << std::endl;
    return 0;
}

