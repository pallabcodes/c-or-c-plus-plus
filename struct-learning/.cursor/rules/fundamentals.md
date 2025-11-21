# Struct Fundamentals

## Scope
Applies to fundamental struct concepts including basic syntax, memory layout, alignment, padding, bit fields, and nested structures.

## Basic Struct Syntax

### Struct Declaration
* Basic struct declaration: `struct Name { members; };`
* C vs C++ differences (default access, constructors)
* POD (Plain Old Data) vs non POD types
* Struct vs class in C++ (default access differences)
* Forward declarations and incomplete types

### Member Access
* Direct member access: `obj.member`
* Pointer member access: `ptr->member`
* Member initialization in constructors
* Default member initialization (C++11)
* Aggregate initialization

### Initialization
* Zero initialization patterns
* Designated initializers (C99, C++20)
* Constructor initialization lists
* Copy and move constructors
* Default constructors

## Memory Layout

### Struct Size Calculation
* Sum of member sizes plus padding
* Alignment requirements affect size
* Platform specific size differences
* Use `sizeof` to verify actual size
* Static assertions for size validation

### Member Ordering
* Members stored in declaration order
* Order affects padding and size
* Hot path members should be grouped
* Cache line considerations
* ABI stability requirements

### Memory Representation
* Sequential memory layout
* Padding bytes between members
* End of struct padding
* Platform byte order considerations
* Memory mapped structs

## Alignment and Padding

### Alignment Rules
* Natural alignment: members aligned to their size
* Struct alignment: aligned to largest member
* Explicit alignment with `alignas`
* Platform specific alignment requirements
* SIMD alignment requirements (16, 32, 64 bytes)

### Padding Calculation
* Padding inserted to satisfy alignment
* Padding between members
* Padding at end of struct
* Minimize padding through member ordering
* Trade off: size vs access performance

### Packed Structs
* `__attribute__((packed))` or `#pragma pack`
* Eliminates padding (use with caution)
* May cause unaligned access penalties
* Useful for network protocols, file formats
* Document why packing is necessary

## Bit Fields

### Basic Bit Fields
* Syntax: `type member : bits;`
* Compact representation of flags
* Platform dependent bit ordering
* Unnamed bit fields for padding
* Bit field alignment considerations

### Bit Field Usage
* Flag sets and option masks
* Protocol field encoding
* Memory efficient representations
* Hardware register mapping
* Cross platform portability concerns

### Bit Field Limitations
* Cannot take address of bit field
* Type restrictions (int, unsigned int, bool)
* Platform specific behavior
* Performance considerations
* Use std::bitset for portable bit manipulation

## Nested Structs

### Struct Composition
* Structs as members of other structs
* Nested struct initialization
* Access patterns: `outer.inner.member`
* Memory layout of nested structs
* Alignment of nested structs

### Anonymous Structs
* C11 anonymous structs and unions
* Direct member access without nesting
* Useful for type punning (with care)
* Memory layout considerations
* Portability concerns

## Implementation Standards

### Documentation
* Document struct purpose and usage
* Explain memory layout when non obvious
* Note alignment requirements
* Document padding and size considerations
* Provide usage examples

### Validation
* Use static assertions for size validation
* Verify alignment with compile time checks
* Test with different compilers and platforms
* Validate memory layout with tools
* Check for padding issues

### Best Practices
* Group related members together
* Order members by size (largest first) to minimize padding
* Use appropriate types for size optimization
* Consider cache line boundaries
* Document non obvious layout decisions

## Code Examples

### Basic Struct
```cpp
// Thread-safety: Not thread-safe (mutable state)
// Ownership: Value semantics
// Invariants: name must be null terminated
// Failure modes: Buffer overflow if name exceeds size
struct Person {
    char name[64];
    int age;
    float height;
};
```

### Aligned Struct
```cpp
// Thread-safety: Thread-safe (immutable after construction)
// Ownership: Value semantics
// Invariants: None
// Failure modes: None
struct alignas(32) SIMDVector {
    float data[8];  // 8 floats = 32 bytes, aligned for AVX
};
```

## Testing Requirements
* Test struct initialization
* Verify memory layout with sizeof
* Test alignment with alignof
* Validate padding with memory inspection
* Test on multiple platforms
* Verify bit field behavior

## Related Topics
* Advanced Techniques: Unions, templates, RAII
* Performance Optimization: Cache alignment, SIMD
* System Programming: Kernel structures, protocols

