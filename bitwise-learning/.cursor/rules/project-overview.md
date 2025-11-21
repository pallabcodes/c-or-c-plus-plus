# Bitwise Learning Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. This bitwise manipulation implementation must meet enterprise production standards suitable for principal level engineering review and must be comparable to top tier implementations used in production systems at these companies.

## Purpose
This module covers the design and implementation of production grade bit manipulation techniques in C and C plus plus. All code must follow production grade standards suitable for principal level code review and must be production ready for deployment in high performance systems requiring efficient bit operations, memory optimization, and low level optimizations.

## Scope
* Applies to all C and C plus plus code in bitwise learning directory
* Extends repository root rules defined in the root `.cursor/rules/` files
* Covers all aspects of bit manipulation from fundamentals to advanced data structures
* Code quality standards align with expectations from top tier companies like Google, Bloomberg, Uber, and Amazon

## Top Tier Product Comparisons

### Google Production Systems
* Bit manipulation utilities in Abseil library
* Bloom filters for distributed systems
* Hash mixing functions for hash tables
* Compact data structures for memory efficiency
* SIMD optimizations in performance critical paths

### Bloomberg Terminal Systems
* Market data flag encoding using bit fields
* Protocol field packing and unpacking
* Compact encodings for high frequency trading
* Memory efficient data structures for real time processing
* Register level optimizations for latency critical paths

### Uber Geospatial Systems
* Geohash encoding for location indexing
* Morton encoding (Z order curves) for spatial queries
* H3 hexagonal hierarchical spatial indexing
* Efficient geographic coordinate encoding
* Spatial data compression techniques

### Amazon Production Systems
* Bitmap indexes for database systems
* Roaring bitmaps for set operations
* Memory efficient data structures
* SIMD accelerated operations
* Cache aware bit manipulation

### Redis Bitmap Operations
* Efficient bitmap storage and operations
* Population count optimizations
* Bit range operations
* Memory efficient set representations
* Production tested implementations

## Bitwise Architecture Components

### Core Components
1. Fundamentals: Basic bit operations, masks, shifts, endianness
2. Advanced Techniques: SWAR, popcount, clz/ctz, parity, bit reversal
3. Enterprise Patterns: Bloom filters, hash mixing, geohash, market flags
4. Performance Optimization: SIMD operations, BMI1/BMI2, AVX2/AVX-512
5. System Programming: CRCs, checksums, register manipulation, memory barriers
6. Advanced Data Structures: Succinct bitvectors, rank/select, bitslicing, compressed encodings

## Code Quality Standards
All bitwise code must demonstrate:
* Comprehensive error handling with clear messages
* Proper input validation and bounds checking
* Correct handling of undefined behavior cases
* Memory safety through proper alignment and bounds checking
* Portability through feature detection and platform guards
* Testing of edge cases and boundary conditions
* Performance optimization through compiler intrinsics and SIMD
* Research backed implementations with proper citations

## Reference Material
* See existing examples in system programming directories for low level patterns
* Reference research papers cited in individual rule files
* Study open source implementations from Google Abseil, Redis, and other production systems
* Benchmark against industry standard implementations

## Related Rules
Refer to the other rule files in this directory for specific guidance on fundamentals, advanced techniques, enterprise patterns, performance optimization, system programming, advanced data structures, memory safety, portability, and testing.

