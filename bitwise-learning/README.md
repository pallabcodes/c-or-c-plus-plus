# Bitwise Learning - Bits, Bytes, and Bit Manipulation

Production-grade curriculum covering fundamentals to advanced techniques used at Google, Uber, Bloomberg, Amazon, PayPal, Stripe, and more.

## Overview

This module provides comprehensive coverage of bit manipulation techniques from basic operations to production-grade implementations. All code follows strict quality standards with proper documentation, error handling, and safety checks.

## Modules

### 01-fundamentals
Core bit manipulation operations:
- Binary representation, two's complement, masks, shifts, rotates
- Endianness, byte order, alignment
- Core bit tricks (set/clear/toggle/test, isolate lsb/msb)
- Bit rotation (left/right circular shifts)
- Advanced tricks (next power of 2, is power of 2, extract bits, manual clz/ctz)

### 02-advanced
Advanced bit manipulation techniques:
- SWAR techniques, parallel bit ops, branchless logic
- Fast popcount, clz/ctz, parity, bit reversal
- Bitboards, Morton/Z-order, Gray codes
- Branchless conditionals (max, min, abs, sign, conditional)
- Fast integer square root using bit manipulation
- Advanced SWAR (parallel comparisons, min/max, abs, multiply)

### 03-enterprise
Production patterns from top tech companies:
- Google: Bloom filters, hash mixing functions (multiple variants)
- Uber: Geo encoding, geohash, H3 hexagonal indexing
- Bloomberg: Market flags, protocol fields, compact encodings

### 04-performance
Performance-optimized implementations:
- SIMD bit ops, BMI1/BMI2 (PDEP/PEXT), AVX2/AVX-512
- Cache-aware bitsets (cache-line aligned)
- Roaring bitmaps (hybrid array/bitmap containers)
- Atomic bitfields (lock-free bit operations)

### 05-system
System-level bit manipulation:
- Protocols and checksums, CRCs, Fletcher
- Byte order conversions, unaligned access
- Memory mapped registers, masks, and barriers
- Lock-free bit manipulation (CAS operations)
- Memory barriers and ordering (acquire/release semantics)
- Advanced register manipulation (bit fields, volatile access)

### 06-god-modded
Advanced data structures and techniques:
- Succinct data structures, rank/select
- Bitslicing, boolean SIMD, advanced bitslicing (AES S-box)
- Compressed bitvectors (Elias-Fano, Golomb-Rice, run-length)
- Wavelet trees (rank/select on arbitrary alphabets)
- Fenwick tree with bit manipulation

## Code Quality Standards

All code follows production-grade standards:
- **API Documentation**: Every function includes thread-safety, ownership, invariants, and failure mode documentation
- **Error Handling**: Input validation and assertions where appropriate
- **Memory Safety**: Bounds checking and safe memory access patterns
- **Portability**: Platform-specific code properly guarded with feature detection
- **Function Limits**: Functions under 50 lines, files under 200 lines
- **Compiler Flags**: Strict warnings enabled (`-Wall -Wextra -Werror -Wpedantic`)

## Build Instructions

See BUILD.md for detailed build instructions.

Quick start:
```bash
cd bitwise-learning
make
```

## Platform Support

- Linux (x86_64, aarch64)
- macOS (x86_64, arm64)
- Some examples require specific CPU features (AVX2, BMI2)

## References

- Intel Intrinsics Guide for SIMD operations
- Hacker's Delight for bit manipulation tricks
- Production codebases: Google's bit manipulation utilities, Redis bitmaps, etc.
