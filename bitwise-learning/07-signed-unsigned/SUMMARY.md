# Signed & Unsigned Integers: Complete Guide Summary

## Overview

This comprehensive guide covers **everything** about signed and unsigned integers at Bloomberg SDE-3 level. The content includes practical examples with TypeScript analogies to help developers coming from those languages understand C++ integer behavior.

## Files Created

### 📚 Core Documentation
- **`README.md`** - Complete theoretical foundation and concepts
- **`SUMMARY.md`** - This quick reference guide

### 💻 Practical Examples
- **`01_basic_types.cpp`** - Type declarations, ranges, and binary representation
- **`02_twos_complement.cpp`** - Two's complement arithmetic and properties
- **`03_overflow_underflow.cpp`** - Overflow, underflow, and wraparound behavior
- **`04_mixed_operations.cpp`** - Signed/unsigned mixing pitfalls and conversions
- **`05_bloomberg_patterns.cpp`** - Bloomberg-style safe integer handling
- **`06_performance.cpp`** - Performance considerations and type choice guidelines

## Key Concepts by Category

### 🔍 **Signed vs Unsigned**
- **Signed integers**: Negative, zero, positive (-2^(n-1) to +2^(n-1)-1)
- **Unsigned integers**: Zero and positive (0 to 2^n-1)
- **Two's complement**: Standard for negative number representation
- **Range asymmetry**: Signed ranges are not symmetric around zero

### 📝 **Binary Representation**
```cpp
// Same bit pattern, different interpretation:
uint8_t unsigned_val = 255;  // 11111111 = 255
int8_t signed_val = -1;      // 11111111 = -1 (two's complement)
```

### 🧮 **Arithmetic Behavior**
```cpp
// Signed overflow: Undefined behavior (dangerous!)
int32_t max = INT32_MAX;
int32_t overflow = max + 1;  // UNDEFINED!

// Unsigned overflow: Defined wraparound (safe)
uint32_t max_u = UINT32_MAX;
uint32_t wrap = max_u + 1;   // Wraps to 0
```

### 🔄 **Mixed Operations**
```cpp
// Silent conversions cause surprises:
uint32_t u = 1;
int32_t s = -1;
bool result = u > s;  // false! (-1 converts to UINT_MAX)
```

### 🏛️ **Bloomberg Patterns**
```cpp
// Type aliases with domain semantics
using Price = int64_t;      // Can be negative (discounts)
using Quantity = uint64_t;  // Always positive
using OrderId = uint64_t;   // Monotonically increasing

// Safe arithmetic
auto result = safe_add(price1, price2);
if (!result) {
    // Handle overflow
}
```

## Critical Rules to Remember

### ✅ **DOs**
- Use `uint64_t` for quantities, counters, and IDs
- Use `int64_t` for financial amounts and differences
- Always check for overflow in financial calculations
- Use fixed-width types (`int32_t`) over platform types (`int`)
- Choose smallest type that fits your range

### ❌ **DON'Ts**
- Never mix signed and unsigned in comparisons
- Don't use unsigned for loop variables (infinite loop risk)
- Don't ignore overflow (signed = undefined, unsigned = wraparound)
- Don't use platform-dependent types in cross-platform code
- Don't assume symmetric ranges around zero

## Performance Guidelines

### Type Choice by Use Case
| Use Case | Recommended Type | Reasoning |
|----------|------------------|-----------|
| Array indices | `size_t` (unsigned) | Always ≥ 0, large range |
| Loop counters | `size_t` or `int32_t` | Performance vs safety trade-off |
| Ages, small counts | `uint8_t` | 0-255 sufficient |
| Temperatures | `int8_t` | -128 to +127 covers extremes |
| Port numbers | `uint16_t` | 0-65535 standard range |
| Timestamps | `uint64_t` | Large range needed |
| Financial amounts | `int64_t` | Can be negative, large range |
| Error codes | `int32_t` | Negative = error, positive = success |

### Memory Considerations
- **Smaller types**: Better cache efficiency, less memory bandwidth
- **Alignment**: Struct members should be ordered for optimal alignment
- **Padding**: Compiler adds padding for alignment requirements

## TypeScript/JavaScript Equivalents

| C++ Concept | TypeScript Equivalent |
|-------------|----------------------|
| `int32_t` | `number` (64-bit float) |
| `uint8_t` | `Uint8Array` element |
| Two's complement | Hidden in number representation |
| Overflow | Precision loss, not overflow |
| Fixed ranges | No fixed ranges (except TypedArrays) |
| Safe arithmetic | BigInt for arbitrary precision |

## Interview Preparation

### Key Topics to Master
1. **Two's complement** representation and why it works
2. **Overflow behavior** differences (signed vs unsigned)
3. **Mixed operations** pitfalls and usual arithmetic conversions
4. **Range limitations** and appropriate type choice
5. **Bloomberg patterns** for safe integer arithmetic
6. **Performance implications** of type choice

### Common Interview Questions
- Why does `uint8_t(-1)` equal 255?
- What's wrong with `for (size_t i = 10; i >= 0; --i)`?
- How do you safely add two `int64_t` values?
- When should you use signed vs unsigned integers?
- What happens when you mix signed and unsigned in comparisons?

## Bloomberg-Specific Standards

### Coding Standards
- Use fixed-width integer types (`int32_t`, `uint64_t`)
- Avoid platform-dependent types (`int`, `long`)
- Use unsigned for non-negative values
- Use signed for differences and offsets
- Document type choices and range assumptions

### Safety First
- **Financial calculations**: Always check for overflow
- **Array bounds**: Use proper signed/unsigned comparisons
- **API design**: Clear contracts about value ranges
- **Error handling**: Result types for safe operations

## Quick Reference

### Range Reference
```cpp
// Signed ranges (two's complement)
int8_t:  -128 to +127
int16_t: -32,768 to +32,767
int32_t: -2,147,483,648 to +2,147,483,647
int64_t: -9,223,372,036,854,775,808 to +9,223,372,036,854,775,807

// Unsigned ranges
uint8_t:  0 to 255
uint16_t: 0 to 65,535
uint32_t: 0 to 4,294,967,295
uint64_t: 0 to 18,446,744,073,709,551,615
```

### Safe Arithmetic Patterns
```cpp
template<typename T>
std::optional<T> safe_add(T a, T b) {
    if constexpr (std::is_unsigned_v<T>) {
        T result = a + b;
        if (result < a) return std::nullopt; // Overflow
        return result;
    } else {
        if (b > 0 && a > std::numeric_limits<T>::max() - b) return std::nullopt;
        if (b < 0 && a < std::numeric_limits<T>::min() - b) return std::nullopt;
        return a + b;
    }
}
```

### Domain-Specific Types (Bloomberg Style)
```cpp
using Price = int64_t;      // Cents, can be negative
using Quantity = uint64_t;  // Always positive
using Amount = int64_t;     // Can be negative
using OrderId = uint64_t;   // Monotonically increasing
using Timestamp = uint64_t; // Unix milliseconds
```

This guide provides comprehensive coverage of signed/unsigned integers at Bloomberg SDE-3 level. Focus on understanding the binary representation, arithmetic behavior, and choosing the right types for your use cases.
