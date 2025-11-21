/*
 * =============================================================================
 * Struct Fundamentals: Bit Fields - Low-Level Bit Manipulation Techniques
 * Production-Grade Bit Field Patterns for Top-Tier Companies
 * =============================================================================
 *
 * This file demonstrates bit field techniques used for compact data packing,
 * flags, protocol headers, and low-latency paths. Covers endianness risks,
 * portability cautions, and validated access patterns.
 *
 * Author: System Engineering Team
 * Version: 1.0
 * Last Modified: 2024-01-15
 *
 * =============================================================================
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <bitset>
#include <climits>

// =============================================================================
// PORTABILITY NOTE
// - Bit-field layout, signedness, and packing are implementation-defined.
// - Prefer explicit masks and shifts on portable wire formats.
// - Use bit-fields for in-process packing when the producer and consumer are the same binary.
// =============================================================================

// Compact feature flags example
struct FeatureFlags {
    // Total 32 bits
    unsigned enable_logging      : 1;  // 1
    unsigned enable_metrics      : 1;  // 2
    unsigned enable_tracing      : 1;  // 3
    unsigned enable_audit        : 1;  // 4
    unsigned reserved_low        : 4;  // 8
    unsigned retry_count         : 4;  // 12
    unsigned rate_limit_bucket   : 8;  // 20
    unsigned priority            : 4;  // 24
    unsigned reserved_high       : 8;  // 32
};

static_assert(sizeof(FeatureFlags) == 4, "FeatureFlags expected to be 4 bytes on this platform");

// Network header style fields (for in-process parsing only)
struct PacketHeader {
    unsigned version     : 3;  // 0..7
    unsigned type        : 5;  // 0..31
    unsigned flags       : 6;  // bit flags
    unsigned length      : 10; // 0..1023
    unsigned checksum    : 8;  // naive example
};

static_assert(sizeof(PacketHeader) == 4, "PacketHeader expected to be 4 bytes on this platform");

// Explicit portable packing for wire formats
inline uint32_t pack_header_portable(uint8_t version, uint8_t type, uint8_t flags, uint16_t length, uint8_t checksum) {
    // Bounds are intentionally narrow
    version  &= 0x07;  // 3 bits
    type     &= 0x1F;  // 5 bits
    flags    &= 0x3F;  // 6 bits
    length   &= 0x03FF; // 10 bits
    // Layout: [checksum:8][length:10][flags:6][type:5][version:3]
    uint32_t v = 0;
    v |= static_cast<uint32_t>(version);
    v |= static_cast<uint32_t>(type)    << 3;
    v |= static_cast<uint32_t>(flags)   << 8;
    v |= static_cast<uint32_t>(length)  << 14;
    v |= static_cast<uint32_t>(checksum)<< 24;
    return v;
}

inline void unpack_header_portable(uint32_t v, uint8_t& version, uint8_t& type, uint8_t& flags, uint16_t& length, uint8_t& checksum) {
    version  = static_cast<uint8_t>( v        & 0x07);
    type     = static_cast<uint8_t>((v >> 3)  & 0x1F);
    flags    = static_cast<uint8_t>((v >> 8)  & 0x3F);
    length   = static_cast<uint16_t>((v >> 14) & 0x03FF);
    checksum = static_cast<uint8_t>((v >> 24) & 0xFF);
}

// Flags helpers
constexpr bool has_flag(uint8_t flags, uint8_t mask) { return (flags & mask) != 0; }

// =============================================================================
// ADVANCED BIT MANIPULATION TRICKS (GOD-MODDED)
// =============================================================================

// Morton encoding for spatial indexing (Google Maps style)
inline uint64_t morton_encode_2d(uint32_t x, uint32_t y) {
    uint64_t result = 0;
    for (int i = 0; i < 32; ++i) {
        result |= (static_cast<uint64_t>(x) & (1ULL << i)) << i;
        result |= (static_cast<uint64_t>(y) & (1ULL << i)) << (i + 1);
    }
    return result;
}

// Bit reversal (useful for FFT, crypto)
inline uint32_t reverse_bits(uint32_t n) {
    n = ((n >> 1) & 0x55555555) | ((n & 0x55555555) << 1);
    n = ((n >> 2) & 0x33333333) | ((n & 0x33333333) << 2);
    n = ((n >> 4) & 0x0F0F0F0F) | ((n & 0x0F0F0F0F) << 4);
    n = ((n >> 8) & 0x00FF00FF) | ((n & 0x00FF00FF) << 8);
    n = (n >> 16) | (n << 16);
    return n;
}

// Population count (count set bits) - optimized
inline int popcount(uint32_t x) {
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x = x + (x >> 8);
    x = x + (x >> 16);
    return x & 0x3F;
}

// Find next power of 2 (rounding up)
inline uint32_t next_power_of_2(uint32_t n) {
    if (n == 0) return 1;
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}

// Extract bit field (hacky but fast)
template<unsigned Start, unsigned Length>
inline uint32_t extract_bitfield(uint32_t value) {
    static_assert(Start + Length <= 32, "Bit field out of range");
    return (value >> Start) & ((1U << Length) - 1);
}

// Set bit field
template<unsigned Start, unsigned Length>
inline uint32_t set_bitfield(uint32_t value, uint32_t field_value) {
    static_assert(Start + Length <= 32, "Bit field out of range");
    uint32_t mask = ((1U << Length) - 1) << Start;
    return (value & ~mask) | ((field_value << Start) & mask);
}

// Demo helpers
void demonstrate_feature_flags() {
    std::cout << "\n=== BIT FIELDS: FEATURE FLAGS ===" << std::endl;
    FeatureFlags f{};
    f.enable_logging = 1;
    f.enable_metrics = 1;
    f.enable_tracing = 0;
    f.enable_audit = 1;
    f.retry_count = 5;  // up to 15
    f.rate_limit_bucket = 200; // up to 255
    f.priority = 7; // up to 15

    std::cout << "Size: " << sizeof(FeatureFlags) << " bytes" << std::endl;
    std::cout << "logging=" << f.enable_logging
              << " metrics=" << f.enable_metrics
              << " tracing=" << f.enable_tracing
              << " audit=" << f.enable_audit
              << " retry=" << f.retry_count
              << " bucket=" << f.rate_limit_bucket
              << " priority=" << f.priority
              << std::endl;
}

void demonstrate_packet_header_inprocess() {
    std::cout << "\n=== BIT FIELDS: IN-PROCESS HEADER ===" << std::endl;
    PacketHeader h{};
    h.version = 3;
    h.type = 12;
    h.flags = 0b100101; // sample bits
    h.length = 512;
    h.checksum = 0xAB;

    std::cout << "Size: " << sizeof(PacketHeader) << " bytes" << std::endl;
    std::cout << "v=" << h.version << " type=" << h.type
              << " flags=" << h.flags << " len=" << h.length
              << " csum=0x" << std::hex << (int)h.checksum << std::dec
              << std::endl;
}

void demonstrate_packet_header_portable() {
    std::cout << "\n=== PORTABLE PACKING: WIRE FORMAT ===" << std::endl;
    uint8_t v = 3, t = 12, fl = 0b100101; uint16_t len = 512; uint8_t cs = 0xAB;
    uint32_t packed = pack_header_portable(v, t, fl, len, cs);

    uint8_t v2, t2, fl2; uint16_t len2; uint8_t cs2;
    unpack_header_portable(packed, v2, t2, fl2, len2, cs2);

    std::cout << "packed=0x" << std::hex << packed << std::dec << std::endl;
    std::cout << "v=" << (int)v2 << " type=" << (int)t2
              << " flags=0b" << std::bitset<6>(fl2)
              << " len=" << len2 << " csum=0x" << std::hex << (int)cs2 << std::dec
              << std::endl;
}

void demonstrate_advanced_bit_tricks() {
    std::cout << "\n=== ADVANCED BIT MANIPULATION TRICKS ===" << std::endl;
    
    // Morton encoding
    uint32_t x = 5, y = 3;
    uint64_t morton = morton_encode_2d(x, y);
    std::cout << "Morton encoding: (" << x << "," << y << ") -> " << morton << std::endl;
    
    // Bit reversal
    uint32_t num = 0b10110110;
    uint32_t reversed = reverse_bits(num);
    std::cout << "Bit reversal: " << std::bitset<8>(num) << " -> " << std::bitset<8>(reversed) << std::endl;
    
    // Population count
    uint32_t test = 0b10110110;
    std::cout << "Popcount of " << std::bitset<8>(test) << " = " << popcount(test) << std::endl;
    
    // Next power of 2
    uint32_t n = 100;
    std::cout << "Next power of 2 after " << n << " = " << next_power_of_2(n) << std::endl;
    
    // Extract bit field
    uint32_t value = 0b1111000011110000;
    uint32_t extracted = extract_bitfield<4, 8>(value);
    std::cout << "Extract bits [4:12] from " << std::bitset<16>(value) 
              << " = " << std::bitset<8>(extracted) << std::endl;
    
    // Set bit field
    uint32_t modified = set_bitfield<4, 8>(value, 0b10101010);
    std::cout << "Set bits [4:12] to 0b10101010 = " << std::bitset<16>(modified) << std::endl;
}

int main() {
    try {
        demonstrate_feature_flags();
        demonstrate_packet_header_inprocess();
        demonstrate_packet_header_portable();
        demonstrate_advanced_bit_tricks();
        std::cout << "\n=== BIT FIELDS DEMONSTRATION COMPLETED SUCCESSFULLY ===" << std::endl;
    } catch (...) {
        std::cerr << "error" << std::endl;
        return 1;
    }
    return 0;
}
