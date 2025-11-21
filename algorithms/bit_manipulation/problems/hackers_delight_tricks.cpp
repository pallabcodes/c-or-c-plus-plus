// Bit Tricks from Hacker's Delight
// Collection of ingenious bit manipulation techniques
// Based on the book "Hacker's Delight" by Henry S. Warren
// God modded implementations of advanced bit hacks

#include <iostream>
#include <cstdint>
#include <bitset>
#include <climits>

// Count trailing zeros (CTZ)
int countTrailingZeros(uint32_t x) {
    if (x == 0) return 32;
    
    int n = 1;
    if ((x & 0x0000FFFF) == 0) { n += 16; x >>= 16; }
    if ((x & 0x000000FF) == 0) { n += 8; x >>= 8; }
    if ((x & 0x0000000F) == 0) { n += 4; x >>= 4; }
    if ((x & 0x00000003) == 0) { n += 2; x >>= 2; }
    return n - (x & 1);
}

// Count leading zeros (CLZ)
int countLeadingZeros(uint32_t x) {
    if (x == 0) return 32;
    
    int n = 0;
    if (x <= 0x0000FFFF) { n += 16; x <<= 16; }
    if (x <= 0x00FFFFFF) { n += 8; x <<= 8; }
    if (x <= 0x0FFFFFFF) { n += 4; x <<= 4; }
    if (x <= 0x3FFFFFFF) { n += 2; x <<= 2; }
    if (x <= 0x7FFFFFFF) { n += 1; }
    return n;
}

// Reverse bits
uint32_t reverseBits(uint32_t x) {
    x = ((x >> 1) & 0x55555555) | ((x & 0x55555555) << 1);
    x = ((x >> 2) & 0x33333333) | ((x & 0x33333333) << 2);
    x = ((x >> 4) & 0x0F0F0F0F) | ((x & 0x0F0F0F0F) << 4);
    x = ((x >> 8) & 0x00FF00FF) | ((x & 0x00FF00FF) << 8);
    x = (x >> 16) | (x << 16);
    return x;
}

// Count set bits (popcount) - parallel method
int popcount(uint32_t x) {
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x = x + (x >> 8);
    x = x + (x >> 16);
    return x & 0x3F;
}

// Round up to next power of 2
uint32_t roundUpPowerOf2(uint32_t x) {
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
}

// Round down to previous power of 2
uint32_t roundDownPowerOf2(uint32_t x) {
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x - (x >> 1);
}

// Check if power of 2
bool isPowerOf2(uint32_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}

// Next higher number with same number of set bits
uint32_t nextHigherSameBits(uint32_t x) {
    uint32_t smallest = x & -x;
    uint32_t ripple = x + smallest;
    uint32_t ones = ((x ^ ripple) >> 2) / smallest;
    return ripple | ones;
}

// Previous lower number with same number of set bits
uint32_t prevLowerSameBits(uint32_t x) {
    return ~nextHigherSameBits(~x);
}

// Sign extension
int32_t signExtend(int32_t x, int bits) {
    int shift = 32 - bits;
    return (x << shift) >> shift;
}

// Absolute value without branching
int32_t absNoBranch(int32_t x) {
    int32_t mask = x >> 31;
    return (x + mask) ^ mask;
}

// Minimum without branching
int32_t minNoBranch(int32_t x, int32_t y) {
    return y + ((x - y) & ((x - y) >> 31));
}

// Maximum without branching
int32_t maxNoBranch(int32_t x, int32_t y) {
    return x - ((x - y) & ((x - y) >> 31));
}

// Swap without temporary variable
void swapNoTemp(uint32_t& x, uint32_t& y) {
    x ^= y;
    y ^= x;
    x ^= y;
}

// Check if two integers have opposite signs
bool oppositeSigns(int32_t x, int32_t y) {
    return (x ^ y) < 0;
}

// Compute parity (even/odd number of set bits)
bool parity(uint32_t x) {
    x ^= x >> 16;
    x ^= x >> 8;
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;
    return x & 1;
}

// Example usage
int main() {
    uint32_t test = 0b1011010110101101;
    
    std::cout << "Number: " << test << " (binary: " << std::bitset<32>(test) << ")" << std::endl;
    std::cout << "Popcount: " << popcount(test) << std::endl;
    std::cout << "Trailing zeros: " << countTrailingZeros(test) << std::endl;
    std::cout << "Leading zeros: " << countLeadingZeros(test) << std::endl;
    std::cout << "Reversed: " << reverseBits(test) << std::endl;
    std::cout << "Next power of 2: " << roundUpPowerOf2(test) << std::endl;
    std::cout << "Is power of 2: " << (isPowerOf2(test) ? "Yes" : "No") << std::endl;
    std::cout << "Parity: " << (parity(test) ? "Odd" : "Even") << std::endl;
    
    uint32_t a = 5, b = 10;
    std::cout << "\nBefore swap: a=" << a << ", b=" << b << std::endl;
    swapNoTemp(a, b);
    std::cout << "After swap: a=" << a << ", b=" << b << std::endl;
    
    std::cout << "\nMin(15, 8): " << minNoBranch(15, 8) << std::endl;
    std::cout << "Max(15, 8): " << maxNoBranch(15, 8) << std::endl;
    std::cout << "Abs(-42): " << absNoBranch(-42) << std::endl;
    
    return 0;
}

