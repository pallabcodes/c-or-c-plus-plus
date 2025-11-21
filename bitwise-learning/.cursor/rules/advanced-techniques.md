# Advanced Bitwise Techniques

## Scope
Applies to advanced bit manipulation techniques including SWAR, parallel bit operations, popcount, clz/ctz, parity, bit reversal, bitboards, and Gray codes.

## SWAR (SIMD Within A Register)

### Principles
* Perform parallel operations on multiple data elements packed in a single register
* Use masks to isolate and operate on individual elements
* Leverage carry propagation for arithmetic operations
* Minimize branches through bitwise operations

### Common SWAR Operations
* Parallel byte addition: Mask, add, handle carry propagation
* Parallel byte comparison: Use subtraction and sign bit extraction
* Parallel byte min/max: Compare and select using masks
* Parallel byte counting: Mask and accumulate

### Implementation Guidelines
* Document the packing format clearly
* Handle overflow and underflow appropriately
* Use appropriate masks for element isolation
* Consider endianness when packing/unpacking

## Population Count (Popcount)

### Algorithms
* Naive: Count bits in a loop
* Lookup table: Precomputed counts for byte values
* Parallel: SWAR style bit counting
* Hardware: `__builtin_popcount` or `_mm_popcnt_u32` intrinsic

### Performance Considerations
* Use hardware popcount when available (BMI1, SSE4.2)
* Fallback to optimized software implementation
* Consider lookup tables for frequently used sizes
* Profile to determine best approach for specific use case

## Count Leading/Trailing Zeros

### Operations
* Count leading zeros (clz): `__builtin_clz` or bit scan reverse
* Count trailing zeros (ctz): `__builtin_ctz` or bit scan forward
* Handle zero input appropriately (undefined behavior for builtins)
* Use for log2 calculations and power of two detection

### Implementation
* Prefer compiler intrinsics when available
* Provide fallback using bit manipulation tricks
* Document undefined behavior for zero input
* Use for efficient bit position calculations

## Parity

### Calculation
* XOR all bits together
* Use `__builtin_parity` when available
* Can be computed via popcount mod 2
* Useful for error detection codes

## Bit Reversal

### Algorithms
* Byte wise reversal with lookup table
* Divide and conquer bit swapping
* Hardware: `_mm256_shuffle_epi8` for SIMD
* Use for FFT and other signal processing applications

## Bitboards

### Chess and Game Applications
* Each bit represents a square on an 8x8 board
* Efficient set operations (union, intersection, difference)
* Fast move generation using bit manipulation
* Parallel evaluation of multiple positions

### Implementation Guidelines
* Use uint64_t for 8x8 boards
* Document bit position encoding clearly
* Provide helper functions for common operations
* Optimize for common patterns (ranks, files, diagonals)

## Morton Encoding (Z Order Curves)

### Spatial Indexing
* Interleave bits from multiple coordinates
* Enables efficient spatial queries
* Used in geospatial applications (Uber H3, geohash)
* Supports range queries and nearest neighbor search

### Implementation
* Part1by1: Interleave bits for 2D coordinates
* Part1by2: Interleave bits for 3D coordinates
* Reverse operations for decoding
* Handle coordinate ranges appropriately

## Gray Codes

### Properties
* Adjacent values differ by exactly one bit
* Useful for minimizing switching in digital circuits
* Used in error correction and encoding schemes
* Can be generated via XOR with shifted value

## Code Quality Standards

### Documentation
* Explain the algorithm being used
* Document performance characteristics
* Note platform specific optimizations
* Reference research papers when applicable

### Error Handling
* Validate input ranges
* Handle edge cases (zero, all ones, overflow)
* Document undefined behavior cases
* Provide clear error messages

### Performance
* Use compiler intrinsics when available
* Profile different implementations
* Consider cache effects for lookup tables
* Optimize for common use cases

## Testing Requirements
* Test with all zeros and all ones
* Test edge cases for each operation
* Verify correctness against reference implementation
* Benchmark performance against alternatives
* Test on different platforms and architectures

## Related Topics
* Enterprise Patterns: Geohash, spatial indexing
* Performance Optimization: SIMD for parallel operations
* Advanced Data Structures: Succinct structures using these techniques

