# Performance Optimization for Bitwise Operations

## Scope
Applies to performance critical bitwise operations including SIMD, CPU intrinsics, BMI1/BMI2, AVX2/AVX-512, and cache aware optimizations.

## SIMD (Single Instruction Multiple Data)

### AVX2 Operations
* 256 bit registers for parallel operations
* Process 8 x 32 bit integers or 32 x 8 bit bytes simultaneously
* Bitwise operations: AND, OR, XOR, ANDNOT
* Shifts and rotates for packed data
* Permutation and shuffle operations

### AVX-512 Operations
* 512 bit registers for even more parallelism
* Masked operations for conditional execution
* More specialized instructions
* Consider power consumption and frequency scaling

### Implementation Guidelines
* Align data to register size (32 bytes for AVX2, 64 bytes for AVX-512)
* Use `alignas` for stack allocated arrays
* Check CPU feature availability before use
* Provide fallback implementations
* Document alignment requirements

### Code Example
```cpp
#ifndef __AVX2__
#warning "AVX2 not available, code may not compile or run correctly"
#endif

alignas(32) int out[8];
_mm256_storeu_si256(reinterpret_cast<__m256i*>(out), result);
```

## BMI1 and BMI2 Instructions

### BMI1 Operations
* TZCNT: Trailing zero count (improved BSF)
* LZCNT: Leading zero count (improved BSR)
* ANDN: And not operation
* BLSR: Clear lowest set bit
* BLSMSK: Get mask up to lowest set bit

### BMI2 Operations
* PDEP: Parallel deposit (scatter bits)
* PEXT: Parallel extract (gather bits)
* Very useful for bit manipulation
* Hardware accelerated bit permutation

### Implementation
* Use `__BMI2__` macro to detect availability
* Provide software fallback implementation
* Document performance characteristics
* Consider using for bit manipulation heavy code

### Code Example
```cpp
#ifdef __BMI2__
    return _pdep_u64(src, mask);
#else
    // Software fallback implementation
#endif
```

## Compiler Intrinsics

### Builtin Functions
* `__builtin_popcount`: Population count
* `__builtin_clz`: Count leading zeros
* `__builtin_ctz`: Count trailing zeros
* `__builtin_parity`: Parity calculation
* `__builtin_bswap32/64`: Byte swap

### Benefits
* Compiler can optimize better than manual implementation
* May use hardware instructions when available
* Portable across platforms
* Fallback to software implementation automatically

### Usage Guidelines
* Prefer builtins over manual implementations
* Document undefined behavior cases
* Provide fallbacks for missing intrinsics
* Profile to verify optimization

## Cache Aware Optimizations

### Data Layout
* Structure of Arrays (SoA) vs Array of Structures (AoS)
* Pack related data together
* Minimize cache line crossings
* Consider prefetching for predictable access patterns

### Bitmap Optimization
* Use cache line sized blocks
* Minimize random access patterns
* Batch operations when possible
* Consider cache effects in algorithm design

### Memory Access Patterns
* Sequential access preferred over random
* Align data structures to cache lines
* Minimize false sharing in multi threaded code
* Use memory barriers appropriately

## Bitslicing

### Boolean SIMD
* Represent multiple boolean values in single register
* Each bit position represents different value
* Enable parallel boolean operations
* Used in cryptography and signal processing

### Implementation
* Pack boolean values into bit positions
* Use SIMD for parallel operations
* Extract results appropriately
* Consider endianness effects

## Performance Measurement

### Benchmarking
* Use consistent measurement methodology
* Warm up before timing
* Measure multiple times and take median
* Report p50, p95, p99 latencies
* Document test environment

### Profiling
* Use `perf` or similar tools
* Identify hot paths
* Measure cache misses
* Analyze instruction level parallelism
* Profile on target hardware

### Optimization Strategy
* Measure before optimizing
* Optimize hot paths first
* Verify optimizations improve performance
* Document performance improvements
* Consider trade offs (code complexity, portability)

## Platform Specific Considerations

### x86_64
* AVX2 widely available
* BMI2 available on Haswell and later
* Consider frequency scaling with AVX-512
* Use appropriate instruction sets

### ARM (aarch64)
* NEON for SIMD operations
* Different instruction set than x86
* Provide platform specific implementations
* Test on both architectures

### Portability
* Feature detection at compile time
* Runtime feature detection when needed
* Provide fallback implementations
* Document platform requirements
* Test on multiple platforms

## Code Quality Standards

### Documentation
* Document performance characteristics
* Note platform specific optimizations
* Explain algorithm choices
* Reference benchmarks and measurements

### Error Handling
* Validate CPU feature availability
* Handle unsupported platforms gracefully
* Provide clear error messages
* Fallback to slower but correct implementation

### Testing
* Test on multiple platforms
* Verify correctness of optimized paths
* Benchmark performance improvements
* Test fallback implementations
* Stress test with various inputs

## Related Topics
* Advanced Techniques: SWAR, parallel operations
* Enterprise Patterns: Production optimizations
* System Programming: Low level optimizations

