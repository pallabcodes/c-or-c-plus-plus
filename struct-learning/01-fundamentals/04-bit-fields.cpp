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

int main() {
    try {
        demonstrate_feature_flags();
        demonstrate_packet_header_inprocess();
        demonstrate_packet_header_portable();
        std::cout << "\n=== BIT FIELDS DEMONSTRATION COMPLETED SUCCESSFULLY ===" << std::endl;
    } catch (...) {
        std::cerr << "error" << std::endl;
        return 1;
    }
    return 0;
}
