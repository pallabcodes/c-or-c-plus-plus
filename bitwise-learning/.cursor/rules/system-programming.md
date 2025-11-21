# System Programming Bitwise Operations

## Scope
Applies to system level bit manipulation including CRCs, checksums, register manipulation, memory barriers, and hardware interface operations.

## Checksums and CRCs

### CRC32 Implementation
* Polynomial: 0xEDB88320 (IEEE 802.3)
* Used in network protocols, file systems, storage
* Software implementation vs hardware acceleration
* Table driven vs bitwise implementation
* Consider performance vs code size trade offs

### Implementation Guidelines
* Use standard polynomials for compatibility
* Provide both software and hardware accelerated versions
* Optimize for common use cases
* Handle byte order appropriately
* Document polynomial and initialization values

### Other Checksums
* Fletcher checksum: Simple additive checksum
* Adler32: Used in zlib
* Consider application requirements
* Balance speed vs error detection capability

## Register Manipulation

### Hardware Registers
* Device control registers
* Status registers
* Configuration registers
* Memory mapped I/O registers

### Bit Field Operations
* Set bits: `reg |= mask`
* Clear bits: `reg &= ~mask`
* Toggle bits: `reg ^= mask`
* Test bits: `(reg & mask) == mask` or `(reg & mask) != 0`
* Extract field: `(reg >> shift) & mask`

### Implementation Standards
* Use volatile for hardware registers
* Document register layout and bit meanings
* Provide helper functions for common operations
* Consider endianness for multi byte registers
* Use appropriate memory barriers

### Code Example
```cpp
// Thread-safety: Not thread-safe (modifies hardware register)
// Ownership: Modifies hardware register
// Invariants: None
// Failure modes: None
static inline void set_bits(volatile Reg& r, uint32_t mask) {
    r.v |= mask;
}
```

## Memory Barriers and Ordering

### Memory Ordering
* Compiler barriers: `asm volatile("" ::: "memory")`
* CPU barriers: `__sync_synchronize()` or `std::atomic_thread_fence`
* Acquire/release semantics
* Consider ordering requirements for bit operations

### Volatile Usage
* Use for memory mapped I/O
* Prevents compiler optimizations
* Does not provide memory ordering guarantees
* Use with appropriate barriers

## Byte Order Conversions

### Network Byte Order
* Big endian for network protocols
* Convert between host and network byte order
* Use `htonl`, `ntohl`, `htons`, `ntohs` or builtins
* Handle both little and big endian hosts

### Unaligned Access
* May be slow or cause faults on some architectures
* Use memcpy for portable unaligned access
* Consider alignment requirements
* Document alignment assumptions

## Protocol Field Packing

### Bit Field Packing
* Pack multiple fields into single word
* Extract fields using masks and shifts
* Document bit layout clearly
* Consider endianness effects
* Validate field ranges

### Implementation
* Define bit positions and masks as constants
* Provide pack/unpack functions
* Validate input ranges
* Handle signed vs unsigned fields appropriately

## Memory Mapped I/O

### Access Patterns
* Use volatile pointers
* Appropriate alignment requirements
* Memory barriers for ordering
* Consider cache coherency
* Document access patterns

### Implementation Guidelines
* Use appropriate pointer types
* Ensure proper alignment
* Use memory barriers when needed
* Document hardware requirements
* Handle platform differences

## Code Quality Standards

### Documentation
* Document register layouts and bit meanings
* Explain memory ordering requirements
* Note platform specific behavior
* Reference hardware documentation

### Error Handling
* Validate register addresses
* Check for hardware availability
* Handle access errors appropriately
* Provide meaningful error messages

### Portability
* Guard platform specific code
* Provide portable alternatives when possible
* Document platform requirements
* Test on multiple platforms

### Safety
* Validate all inputs
* Check bounds and ranges
* Use appropriate types (volatile, atomic)
* Consider race conditions in multi threaded code

## Testing Requirements
* Test on target hardware when possible
* Verify correct bit manipulation
* Test edge cases (all zeros, all ones)
* Test with different byte orders
* Verify memory ordering behavior
* Stress test with concurrent access

## Related Topics
* Performance Optimization: Hardware accelerated operations
* Memory Safety: Proper use of volatile and barriers
* Fundamentals: Basic bit operations used in system code

