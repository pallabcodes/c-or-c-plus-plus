// SWAR (SIMD Within A Register) Tricks: Advanced bit manipulation
// Parallel bit operations using word-level parallelism
// Extremely fast for counting bits, finding patterns, etc.

#include <iostream>
#include <cstdint>
#include <bitset>

using namespace std;

// Count set bits using SWAR
int popcountSWAR(uint32_t x) {
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x = x + (x >> 8);
    x = x + (x >> 16);
    return x & 0x3F;
}

// Count set bits in 64-bit using SWAR
int popcount64SWAR(uint64_t x) {
    x = x - ((x >> 1) & 0x5555555555555555ULL);
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    x = x + (x >> 8);
    x = x + (x >> 16);
    x = x + (x >> 32);
    return x & 0x7F;
}

// Reverse bits using SWAR
uint32_t reverseBitsSWAR(uint32_t x) {
    x = ((x >> 1) & 0x55555555) | ((x & 0x55555555) << 1);
    x = ((x >> 2) & 0x33333333) | ((x & 0x33333333) << 2);
    x = ((x >> 4) & 0x0F0F0F0F) | ((x & 0x0F0F0F0F) << 4);
    x = ((x >> 8) & 0x00FF00FF) | ((x & 0x00FF00FF) << 8);
    x = (x >> 16) | (x << 16);
    return x;
}

// Find next power of 2
uint32_t nextPowerOf2(uint32_t x) {
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
}

// Count trailing zeros
int countTrailingZeros(uint32_t x) {
    if (x == 0) return 32;
    return __builtin_ctz(x); // GCC builtin, or implement:
    // return popcountSWAR((x & -x) - 1);
}

// Count leading zeros
int countLeadingZeros(uint32_t x) {
    if (x == 0) return 32;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return 32 - popcountSWAR(x);
}

// Round up to next power of 2
uint32_t roundUpPowerOf2(uint32_t x) {
    return x == 0 ? 1 : (1 << (32 - countLeadingZeros(x - 1)));
}

// Is power of 2
bool isPowerOf2(uint32_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}

// Example usage
int main() {
    uint32_t test = 0b1011010110101101;
    
    cout << "Number: " << test << " (binary: " << bitset<32>(test) << ")" << endl;
    cout << "Popcount: " << popcountSWAR(test) << endl;
    cout << "Reversed: " << reverseBitsSWAR(test) << endl;
    cout << "Next power of 2: " << nextPowerOf2(test) << endl;
    cout << "Trailing zeros: " << countTrailingZeros(test) << endl;
    cout << "Leading zeros: " << countLeadingZeros(test) << endl;
    cout << "Is power of 2: " << (isPowerOf2(test) ? "Yes" : "No") << endl;
    
    return 0;
}

