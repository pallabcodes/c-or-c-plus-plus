# Memory Layout Standards

## Overview
Memory layout is critical for struct performance and correctness. This document defines standards for implementing production grade struct memory layouts including alignment, padding, and optimization techniques.

## Struct Memory Layout

### Layout Rules
* **Member order**: Members stored in declaration order
* **Alignment**: Each member aligned to its type's alignment requirement
* **Padding**: Compiler inserts padding to satisfy alignment
* **Size**: Struct size is multiple of largest member alignment
* **Rationale**: Layout rules ensure correct memory access

### Example Layout
```cpp
struct Example {
    char c;      // 1 byte, offset 0
    // 3 bytes padding
    int i;       // 4 bytes, offset 4
    double d;    // 8 bytes, offset 8
    // Total: 16 bytes
};
```

## Alignment

### Natural Alignment
* **Definition**: Alignment equal to member size
* **char**: 1 byte alignment
* **int**: 4 byte alignment (typically)
* **double**: 8 byte alignment (typically)
* **Rationale**: Natural alignment enables efficient access

### Explicit Alignment
* **alignas**: Use alignas keyword for explicit alignment
* **Use cases**: Cache line alignment, SIMD alignment
* **Rationale**: Explicit alignment enables optimization

### Example Alignment
```cpp
struct alignas(64) CacheAligned {
    // Aligned to 64 bytes (cache line)
    uint64_t data[8];
};
```

## Padding

### Padding Rules
* **Between members**: Padding inserted to satisfy alignment
* **End padding**: Padding at end to satisfy struct alignment
* **Minimization**: Order members to minimize padding
* **Rationale**: Padding affects memory usage

### Padding Minimization
* **Order by size**: Order members by size (largest first or smallest first)
* **Group by alignment**: Group members with same alignment
* **Rationale**: Minimization reduces memory usage

### Example Padding Minimization
```cpp
// BAD: 24 bytes (8 + 4 + 4 padding + 8)
struct BadLayout {
    double d;   // 8 bytes
    int i;      // 4 bytes
    // 4 bytes padding
    double d2;  // 8 bytes
};

// GOOD: 16 bytes (8 + 8)
struct GoodLayout {
    double d;   // 8 bytes
    double d2;  // 8 bytes
    int i;      // 4 bytes
    // 4 bytes padding (for struct alignment)
};
```

## Bit Fields

### Definition
* **Bit fields**: Members occupying specific number of bits
* **Syntax**: `type member : bits`
* **Use cases**: Flags, compact data
* **Rationale**: Bit fields enable memory efficiency

### Bit Field Layout
* **Storage unit**: Bit fields packed into storage units
* **Order**: Implementation defined order
* **Padding**: Padding bits inserted as needed
* **Rationale**: Layout enables efficient packing

### Example Bit Fields
```cpp
struct Flags {
    unsigned int flag1 : 1;  // 1 bit
    unsigned int flag2 : 1;  // 1 bit
    unsigned int flag3 : 6;  // 6 bits
    // Total: 8 bits (1 byte)
};
```

## Nested Structs

### Layout
* **Nested layout**: Nested structs follow same layout rules
* **Alignment**: Nested struct alignment affects parent
* **Padding**: Padding inserted for nested struct alignment
* **Rationale**: Nested layout enables hierarchy

### Example Nested Structs
```cpp
struct Inner {
    int x;  // 4 bytes
    int y;  // 4 bytes
};

struct Outer {
    char c;        // 1 byte
    // 3 bytes padding
    Inner inner;   // 8 bytes (aligned to 4)
    double d;      // 8 bytes
    // Total: 24 bytes
};
```

## Implementation Standards

### Correctness
* **Layout correctness**: Ensure correct memory layout
* **Alignment**: Proper alignment for all members
* **Padding**: Understand and document padding
* **Rationale**: Correctness is critical

### Performance
* **Cache efficiency**: Design for cache efficiency
* **Padding minimization**: Minimize padding
* **Alignment optimization**: Optimize alignment
* **Rationale**: Performance is critical

## Testing Requirements

### Unit Tests
* **Layout tests**: Test memory layout correctness
* **Alignment tests**: Test alignment requirements
* **Padding tests**: Test padding behavior
* **Size tests**: Test struct size
* **Rationale**: Comprehensive testing ensures correctness

## Research Papers and References

### Memory Layout
* "Cache Conscious Data Structures" research papers
* "Memory Alignment" research
* "Data Structure Layout" research papers

## Implementation Checklist

- [ ] Understand memory layout rules
- [ ] Learn alignment requirements
- [ ] Understand padding behavior
- [ ] Practice layout optimization
- [ ] Write comprehensive unit tests
- [ ] Document memory layout

