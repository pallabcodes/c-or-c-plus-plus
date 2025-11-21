// Advanced Bit Manipulation Hacks: God-tier bit tricks
// Collection of ingenious bit manipulation techniques

#include <iostream>
#include <cstdint>
#include <bitset>

using namespace std;

// Swap two numbers without temporary variable
void swapXOR(int& a, int& b) {
    a ^= b;
    b ^= a;
    a ^= b;
}

// Find absolute value without branching
int absNoBranch(int x) {
    int mask = x >> (sizeof(int) * 8 - 1);
    return (x + mask) ^ mask;
}

// Find minimum of two numbers
int minNoBranch(int x, int y) {
    return y ^ ((x ^ y) & -(x < y));
}

// Find maximum of two numbers
int maxNoBranch(int x, int y) {
    return x ^ ((x ^ y) & -(x < y));
}

// Check if two numbers have opposite signs
bool oppositeSigns(int x, int y) {
    return (x ^ y) < 0;
}

// Compute modulus division by power of 2
int modPowerOf2(int x, int mod) {
    return x & (mod - 1);
}

// Check if number is power of 4
bool isPowerOf4(uint32_t x) {
    return x != 0 && (x & (x - 1)) == 0 && (x & 0xAAAAAAAA) == 0;
}

// Multiply by 7
int multiplyBy7(int x) {
    return (x << 3) - x;
}

// Multiply by 3.5 (rounded)
int multiplyBy3_5(int x) {
    return (x << 1) + x + (x >> 1);
}

// Round up to multiple of 8
int roundUpTo8(int x) {
    return (x + 7) & ~7;
}

// Count number of bits to flip to convert A to B
int bitsToFlip(int a, int b) {
    int diff = a ^ b;
    int count = 0;
    while (diff) {
        count++;
        diff &= diff - 1; // Clear least significant bit
    }
    return count;
}

// Find position of rightmost set bit
int rightmostSetBitPos(int x) {
    return __builtin_ffs(x); // GCC builtin
    // Alternative: return (x & -x) ? __builtin_ctz(x) + 1 : 0;
}

// Toggle nth bit
int toggleBit(int x, int n) {
    return x ^ (1 << n);
}

// Set nth bit
int setBit(int x, int n) {
    return x | (1 << n);
}

// Clear nth bit
int clearBit(int x, int n) {
    return x & ~(1 << n);
}

// Check if nth bit is set
bool isBitSet(int x, int n) {
    return (x >> n) & 1;
}

// Extract lowest set bit
int lowestSetBit(int x) {
    return x & -x;
}

// Clear lowest set bit
int clearLowestSetBit(int x) {
    return x & (x - 1);
}

// Get all subsets of a set
void printSubsets(int set) {
    int subset = set;
    do {
        cout << bitset<8>(subset) << " ";
        subset = (subset - 1) & set;
    } while (subset != set);
}

// Example usage
int main() {
    int a = 5, b = 7;
    cout << "Before swap: a=" << a << ", b=" << b << endl;
    swapXOR(a, b);
    cout << "After swap: a=" << a << ", b=" << b << endl;
    
    cout << "\nAbsolute value of -42: " << absNoBranch(-42) << endl;
    cout << "Min(10, 5): " << minNoBranch(10, 5) << endl;
    cout << "Max(10, 5): " << maxNoBranch(10, 5) << endl;
    
    cout << "\nOpposite signs(5, -3): " << oppositeSigns(5, -3) << endl;
    cout << "Mod 8 of 23: " << modPowerOf2(23, 8) << endl;
    cout << "Is 16 power of 4: " << isPowerOf4(16) << endl;
    
    cout << "\nMultiply 5 by 7: " << multiplyBy7(5) << endl;
    cout << "Round up 13 to multiple of 8: " << roundUpTo8(13) << endl;
    cout << "Bits to flip 5->7: " << bitsToFlip(5, 7) << endl;
    
    cout << "\nSubsets of 0b1011: ";
    printSubsets(0b1011);
    cout << endl;
    
    return 0;
}

