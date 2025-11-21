# Bitwise Fundamentals

## Scope
Applies to fundamental bit manipulation operations including basic bit operations, masks, shifts, endianness, and core bit tricks.

## Core Operations

### Bit Testing and Manipulation
* Test bit: `(x >> b) & 1u` or `x & (1u << b)`
* Set bit: `x | (1u << b)`
* Clear bit: `x & ~(1u << b)`
* Toggle bit: `x ^ (1u << b)`
* Always validate bit position is within valid range (0 to 31 for uint32_t)

### Bit Isolation
* Isolate LSB: `x & -x` (two's complement trick)
* Isolate MSB: Requires count leading zeros or bit scan reverse
* Extract bit range: Use masks and shifts appropriately
* Clear lower bits: `x & ~((1u << n) - 1u)`
* Clear higher bits: `x & ((1u << n) - 1u)`

### Shifts and Rotates
* Logical shift: `x << n` or `x >> n` (zeros fill)
* Arithmetic shift: Implementation dependent, use with caution
* Rotate left: `(x << n) | (x >> (32 - n))` for uint32_t
* Rotate right: `(x >> n) | (x << (32 - n))` for uint32_t
* Always handle edge cases (n == 0, n >= 32)

### Endianness and Byte Order
* Detect endianness: Compare byte representation of known value
* Byte swap: Use `__builtin_bswap32` or `__builtin_bswap64` when available
* Convert between host and network byte order
* Handle unaligned access with care
* Document endianness assumptions in code

## Implementation Standards

### Error Handling
* Validate bit positions are within valid range
* Use assertions for debug builds to catch invalid inputs
* Return appropriate error codes or use exceptions for invalid operations
* Document undefined behavior cases clearly

### Performance Considerations
* Prefer compiler intrinsics when available (`__builtin_clz`, `__builtin_ctz`, `__builtin_popcount`)
* Use constants for masks to enable compiler optimizations
* Avoid unnecessary branches in hot paths
* Consider lookup tables for complex operations if memory allows

### Portability
* Use standard integer types (`uint32_t`, `uint64_t`) instead of `int` or `long`
* Guard platform specific code with feature detection macros
* Document platform specific behavior
* Provide fallback implementations for missing intrinsics

## Code Examples

### Basic Bit Operations
```cpp
// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: Bit position must be < 32
// Failure modes: Undefined behavior if b >= 32
static inline bool test_bit(uint32_t x, unsigned b) {
    assert(b < 32);
    return (x >> b) & 1u;
}
```

### Endianness Detection
```cpp
// Thread-safety: Thread-safe (pure function, no shared state)
// Ownership: None (value semantics)
// Invariants: None
// Failure modes: None
static inline bool is_little_endian() {
    uint16_t x = 1;
    return *reinterpret_cast<uint8_t*>(&x) == 1;
}
```

## Testing Requirements
* Test all bit positions (0 to 31 for uint32_t)
* Test edge cases (all zeros, all ones, single bit set)
* Test endianness on both little and big endian platforms
* Test shift operations with n == 0 and n >= bit width
* Verify correct behavior with undefined input values

## Related Topics
* Advanced Techniques: SWAR, popcount optimizations
* Performance Optimization: SIMD for bulk operations
* System Programming: Register manipulation, memory barriers

