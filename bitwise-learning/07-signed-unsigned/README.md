# Signed & Unsigned Integers: Complete Guide for Bloomberg SDE-3

## Table of Contents
1. [Introduction: Signed vs Unsigned](#introduction-signed-vs-unsigned)
2. [Why Signed/Unsigned Matters](#why-signedunsigned-matters)
3. [Binary Representation](#binary-representation)
4. [Two's Complement](#twos-complement)
5. [Integer Types in C++](#integer-types-in-c)
6. [Range and Limits](#range-and-limits)
7. [Arithmetic Operations](#arithmetic-operations)
8. [Overflow and Underflow](#overflow-and-underflow)
9. [Mixed Signed/Unsigned Operations](#mixed-signedunsigned-operations)
10. [Bitwise Operations](#bitwise-operations)
11. [Type Conversions](#type-conversions)
12. [Performance Considerations](#performance-considerations)
13. [Bloomberg-Specific Patterns](#bloomberg-specific-patterns)
14. [Common Pitfalls](#common-pitfalls)
15. [Best Practices](#best-practices)

## Introduction: Signed vs Unsigned

### Definition
- **Signed integers**: Can represent negative numbers, zero, and positive numbers
- **Unsigned integers**: Can only represent zero and positive numbers

### Key Differences
```cpp
// Signed (int8_t)
int8_t signed_val = -1;  // OK: -128 to +127

// Unsigned (uint8_t)
uint8_t unsigned_val = -1;  // PROBLEM: wraps to 255
```

### TypeScript Analogy
In TypeScript, there's no direct equivalent to signed/unsigned integers:
```typescript
// TypeScript/JavaScript: All numbers are double-precision floating point
let signed: number = -42;      // Can be negative
let unsigned: number = 42;     // Can be positive

// No built-in unsigned types - closest is:
let unsignedLike: number = 42; // But can still be negative
```

## Why Signed/Unsigned Matters

### Bloomberg Context
1. **Financial Calculations**: Stock prices (always positive) vs. P&L (can be negative)
2. **Memory Efficiency**: Use smallest appropriate type
3. **Performance**: Unsigned operations can be faster on some architectures
4. **API Design**: Clear contracts about value ranges
5. **Security**: Prevent negative values where they don't make sense
6. **Cross-Platform**: Consistent behavior across different architectures

### Real-World Examples
```cpp
// Bloomberg trading system
uint64_t shares_quantity;    // Always positive - can't buy -100 shares
int64_t profit_loss;        // Can be negative (losses) or positive (gains)
uint32_t order_id;          // Always positive, monotonically increasing
int32_t price_offset;       // Can be negative (discount) or positive (premium)
```

## Binary Representation

### Unsigned Integers
- Pure binary representation
- All bits contribute to magnitude
- Range: 0 to 2^n - 1

```cpp
// uint8_t (8 bits): 0 to 255
// Binary: 00000000 = 0
// Binary: 11111111 = 255
```

### Signed Integers (Two's Complement)
- Most significant bit is sign bit (0 = positive, 1 = negative)
- Remaining bits represent magnitude
- Range: -2^(n-1) to 2^(n-1) - 1

```cpp
// int8_t (8 bits): -128 to +127
// Binary: 00000000 = 0
// Binary: 01111111 = +127
// Binary: 10000000 = -128
// Binary: 11111111 = -1
```

### TypeScript Analogy
```typescript
// JavaScript/TypeScript doesn't have raw binary representation
// But you can think of it as:
function toBinaryString(num: number): string {
    // Convert to 32-bit binary representation
    return (num >>> 0).toString(2).padStart(32, '0');
}

// Positive numbers: standard binary
console.log(toBinaryString(5));    // "00000000000000000000000000000101"

// Negative numbers: two's complement (conceptually)
console.log(toBinaryString(-5));   // "11111111111111111111111111111011"
```

## Two's Complement

### How It Works
1. **Positive numbers**: Same as unsigned binary
2. **Negative numbers**:
   - Take absolute value
   - Invert all bits (one's complement)
   - Add 1 (two's complement)

### Example: Converting -5 to 8-bit two's complement
```
Step 1: Absolute value of 5: 00000101
Step 2: One's complement:     11111010
Step 3: Add 1:               11111011 = -5 in two's complement
```

### Why Two's Complement?
1. **No separate +0 and -0**
2. **Same addition/subtraction circuits work**
3. **Overflow detection is simpler**
4. **Most significant bit indicates sign**

### TypeScript Analogy
```typescript
function toTwosComplement8bit(num: number): string {
    if (num >= 0) {
        // Positive: normal binary
        return num.toString(2).padStart(8, '0');
    } else {
        // Negative: two's complement
        const abs = Math.abs(num);
        const binary = abs.toString(2).padStart(8, '0');
        // Invert bits
        const inverted = binary.split('').map(bit => bit === '0' ? '1' : '0').join('');
        // Add 1
        const twosComplement = (parseInt(inverted, 2) + 1).toString(2).padStart(8, '0');
        return twosComplement;
    }
}

console.log(toTwosComplement8bit(5));   // "00000101"
console.log(toTwosComplement8bit(-5));  // "11111011"
```

## Integer Types in C++

### Fixed-Width Types (C++11)
```cpp
#include <cstdint>

// Signed
int8_t   // -128 to +127 (8 bits)
int16_t  // -32,768 to +32,767 (16 bits)
int32_t  // -2,147,483,648 to +2,147,483,647 (32 bits)
int64_t  // -9,223,372,036,854,775,808 to +9,223,372,036,854,775,807 (64 bits)

// Unsigned
uint8_t  // 0 to 255 (8 bits)
uint16_t // 0 to 65,535 (16 bits)
uint32_t // 0 to 4,294,967,295 (32 bits)
uint64_t // 0 to 18,446,744,073,709,551,615 (64 bits)
```

### Platform-Dependent Types
```cpp
// May vary by platform
short     // Usually 16 bits
int       // Usually 32 bits
long      // Usually 32 or 64 bits
long long // Usually 64 bits
```

### TypeScript Analogy
```typescript
// TypeScript doesn't have fixed-width integer types
// Everything is number (double-precision float)
// But you can simulate with type annotations:

// Closest equivalents (conceptual only):
type int8_t = number;    // -128 to 127
type uint8_t = number;   // 0 to 255
type int32_t = number;   // -2^31 to 2^31-1
type uint32_t = number;  // 0 to 2^32-1

// With BigInt (ES2020) for large integers:
type int64_t = bigint;
type uint64_t = bigint;

let smallInt: int8_t = 42;
let bigInt: int64_t = 9007199254740991n; // Beyond safe integer range
```

## Range and Limits

### Numeric Limits
```cpp
#include <limits>

std::numeric_limits<int8_t>::min();   // -128
std::numeric_limits<int8_t>::max();   // +127
std::numeric_limits<uint8_t>::min();  // 0
std::numeric_limits<uint8_t>::max();  // 255
```

### Common Ranges
| Type | Range | Use Case |
|------|-------|----------|
| `uint8_t` | 0 to 255 | ASCII chars, small counters |
| `int8_t` | -128 to +127 | Small offsets, temperatures |
| `uint16_t` | 0 to 65,535 | Port numbers, array indices |
| `int16_t` | -32K to +32K | Audio samples, coordinates |
| `uint32_t` | 0 to 4.2B | File sizes, IP addresses |
| `int32_t` | -2B to +2B | General-purpose integers |
| `uint64_t` | 0 to 18E | Large counters, timestamps |
| `int64_t` | -9E to +9E | Financial amounts, IDs |

### TypeScript Analogy
```typescript
// JavaScript/TypeScript ranges:
const INT8_MIN = -128;
const INT8_MAX = 127;
const UINT8_MAX = 255;

// But in practice, all numbers are doubles:
// - Safe integer range: -2^53 to +2^53
// - Beyond that, precision is lost

const SAFE_INTEGER_MIN = Number.MIN_SAFE_INTEGER;  // -2^53
const SAFE_INTEGER_MAX = Number.MAX_SAFE_INTEGER;  // +2^53

// For larger values, use BigInt:
const bigInt: bigint = 18446744073709551615n; // uint64_t max
```

## Arithmetic Operations

### Addition/Subtraction
```cpp
uint8_t a = 200;
uint8_t b = 100;
uint8_t result = a + b;  // 300 % 256 = 44 (wraparound)

int8_t c = 100;
int8_t d = 50;
int8_t result2 = c + d;  // 150 (fits in range)
```

### Multiplication/Division
```cpp
uint32_t x = 4000000000U;  // 4 billion
uint32_t y = 2;
uint32_t product = x * y;  // 8000000000U % 2^32 = 705032704 (wraparound)
```

### TypeScript Analogy
```typescript
// JavaScript has no integer overflow - uses floating point
let a: number = 200;
let b: number = 100;
let result = a + b;  // 300 (no overflow)

// For integer-like behavior with BigInt:
let bigA: bigint = 200n;
let bigB: bigint = 100n;
let bigResult = bigA + bigB;  // 300n

// To simulate uint8_t overflow:
function uint8Add(a: number, b: number): number {
    return (a + b) & 0xFF;  // Mask to 8 bits
}
console.log(uint8Add(200, 100));  // 44 (200 + 100 = 300, 300 & 255 = 44)
```

## Overflow and Underflow

### Signed Overflow
```cpp
int8_t a = 127;    // Maximum int8_t
int8_t b = 1;
int8_t result = a + b;  // Undefined behavior in C++!
```

### Unsigned Overflow (Defined Behavior)
```cpp
uint8_t a = 255;   // Maximum uint8_t
uint8_t b = 1;
uint8_t result = a + b;  // Wraps to 0 (defined behavior)
```

### Detection
```cpp
#include <limits>

// Check for overflow before operation
bool wouldOverflow(int32_t a, int32_t b) {
    if (b > 0 && a > std::numeric_limits<int32_t>::max() - b) return true;
    if (b < 0 && a < std::numeric_limits<int32_t>::min() - b) return true;
    return false;
}
```

### TypeScript Analogy
```typescript
// JavaScript/TypeScript: No overflow, but precision loss
let bigNumber: number = Number.MAX_SAFE_INTEGER + 1;
console.log(bigNumber);  // 9007199254740992 (precision lost)

// BigInt handles arbitrary precision:
let bigInt: bigint = 9223372036854775807n + 1n;  // int64_t max + 1
console.log(bigInt);  // 9223372036854775808n (exact)
```

## Mixed Signed/Unsigned Operations

### The "Usual Arithmetic Conversions"
When mixing signed and unsigned of same size:
- Signed is converted to unsigned
- Can lead to surprising results

```cpp
unsigned int u = 10;
int s = -5;

// What is u + s?
// s (-5) is converted to unsigned: 4294967291 (on 32-bit)
// Result: 10 + 4294967291 = 4294967301 % 2^32 = 4294967301 (wraparound)
```

### Comparison Issues
```cpp
unsigned int u = 1;
int s = -1;

// u > s is true! (-1 converted to unsigned becomes UINT_MAX)
```

### TypeScript Analogy
```typescript
// JavaScript/TypeScript: All numbers are signed floats
let u: number = 1;
let s: number = -1;
console.log(u > s);  // true (no conversion issues)

// But with BigInt:
let bigU: bigint = 1n;
let bigS: bigint = -1n;
console.log(bigU > bigS);  // true
```

## Bitwise Operations

### Same for Signed and Unsigned
```cpp
uint8_t a = 0b10101010;  // 170
uint8_t b = 0b11001100;  // 204

uint8_t and_result = a & b;  // 0b10001000 = 136
uint8_t or_result = a | b;   // 0b11101110 = 238
uint8_t xor_result = a ^ b;  // 0b01100110 = 102
```

### Shift Operations
```cpp
int8_t signed_val = -16;      // 11110000 in binary
uint8_t unsigned_val = 240;   // 11110000 in binary

// Right shift behavior differs!
int8_t signed_shift = signed_val >> 2;    // Arithmetic shift: 11111100 = -4
uint8_t unsigned_shift = unsigned_val >> 2;  // Logical shift: 00111100 = 60
```

### TypeScript Analogy
```typescript
// JavaScript/TypeScript: Bitwise ops convert to 32-bit signed integers
let a: number = 0b10101010;  // 170
let b: number = 0b11001100;  // 204

let andResult = a & b;  // 136
let orResult = a | b;   // 238
let xorResult = a ^ b;  // 102

// Right shift:
let signedShift = -16 >> 2;   // -4 (arithmetic shift)
let unsignedShift = 240 >>> 2; // 60 (logical shift - JS only)
```

## Type Conversions

### Implicit Conversions
```cpp
int16_t small = 1000;
int32_t large = small;  // OK: widening conversion

int32_t large_val = 100000;
int16_t small_val = large_val;  // PROBLEM: narrowing, potential data loss
```

### Explicit Conversions
```cpp
// C-style cast (avoid)
int32_t large = 100000;
int16_t small = (int16_t)large;

// static_cast (preferred)
int16_t small2 = static_cast<int16_t>(large);

// Safe conversion with range check
template<typename To, typename From>
std::optional<To> safe_cast(From value) {
    if (value > std::numeric_limits<To>::max() ||
        value < std::numeric_limits<To>::min()) {
        return std::nullopt;
    }
    return static_cast<To>(value);
}
```

### TypeScript Analogy
```typescript
// TypeScript: Type assertions (like casts)
let large: number = 100000;
let small: number = large as number;  // Type assertion, no runtime check

// Safe conversion:
function safeCastInt16(value: number): number | null {
    if (value > 32767 || value < -32768) {
        return null;
    }
    return Math.trunc(value);
}

console.log(safeCastInt16(100000));  // null (out of range)
console.log(safeCastInt16(1000));    // 1000
```

## Performance Considerations

### Choosing the Right Type
1. **Use smallest type that fits your range**
2. **Use unsigned for non-negative values**
3. **Consider alignment and padding**
4. **Think about cache efficiency**

### Architecture Considerations
```cpp
// On 64-bit systems:
// int is usually 32 bits (for compatibility)
// long is usually 64 bits
// Use fixed-width types for portability
```

### Bloomberg Performance Tips
- Use `uint32_t` for array indices (faster than signed)
- Use `int64_t` for financial amounts (128-bit arithmetic needed)
- Use `uint8_t` for flags and small enums
- Consider memory layout for struct padding

### TypeScript Analogy
```typescript
// JavaScript/TypeScript: All numbers are 64-bit floats
// Performance considerations are different:
// - V8 engine optimizes integer operations within safe range
// - Use TypedArrays for true integer performance:
//   let int32Array = new Int32Array(1000);  // True 32-bit integers
//   let uint8Array = new Uint8Array(1000);  // True 8-bit unsigned
```

## Bloomberg-Specific Patterns

### Financial Types
```cpp
// Bloomberg-style type aliases
using Price = int64_t;           // Prices in smallest currency unit (e.g., cents)
using Quantity = uint64_t;       // Share quantities (always positive)
using OrderId = uint64_t;        // Order IDs (monotonically increasing)
using Timestamp = uint64_t;      // Unix timestamps in milliseconds

// Safe arithmetic for financial calculations
class SafeInt64 {
    int64_t value_;
public:
    explicit SafeInt64(int64_t val) : value_(val) {}

    SafeInt64 operator+(SafeInt64 other) const {
        // Check for overflow
        if (other.value_ > 0 && value_ > std::numeric_limits<int64_t>::max() - other.value_) {
            throw std::overflow_error("Addition overflow");
        }
        return SafeInt64(value_ + other.value_);
    }
};
```

### Bloomberg Coding Standards
- Use fixed-width types (`int32_t`, `uint64_t`)
- Avoid platform-dependent types (`int`, `long`)
- Use unsigned for counters and sizes
- Use signed for differences and offsets
- Always check for overflow in financial calculations

## Common Pitfalls

### 1. Mixed Signed/Unsigned Comparisons
```cpp
std::vector<int> data = {-1, 0, 1};
size_t size = data.size();  // size_t is unsigned

if (size > -1) {  // PROBLEM: -1 converted to unsigned becomes UINT_MAX
    // This condition is always true!
}
```

### 2. Loop Variables
```cpp
for (int i = 10; i >= 0; --i) {  // OK
    // Loop body
}

for (size_t i = 10; i >= 0; --i) {  // INFINITE LOOP!
    // i wraps around to UINT_MAX when it reaches 0
}
```

### 3. Array Indexing
```cpp
int array[100];
int index = -5;
array[index] = 42;  // Undefined behavior! Negative index
```

### 4. Bit Shifting
```cpp
int32_t value = -16;
value >> 2;  // Arithmetic shift (sign-extended)
uint32_t uvalue = static_cast<uint32_t>(value);
uvalue >> 2;  // Logical shift (zero-filled)
```

### 5. Division by Zero
```cpp
int32_t dividend = 100;
uint32_t divisor = 0;
int32_t result = dividend / divisor;  // Undefined behavior
```

## Best Practices

### 1. Use Fixed-Width Types
```cpp
// Prefer
uint32_t counter;
int64_t balance;

// Avoid
unsigned int counter;  // Size varies by platform
long balance;          // Size varies by platform
```

### 2. Choose Appropriate Ranges
```cpp
// For financial amounts (can be negative)
int64_t account_balance;

// For quantities (always positive)
uint64_t shares_outstanding;

// For small counters
uint16_t retry_count;

// For flags
uint8_t status_flags;
```

### 3. Handle Overflow Safely
```cpp
#include <limits>

// Safe addition
template<typename T>
std::optional<T> safe_add(T a, T b) {
    if (b > 0 && a > std::numeric_limits<T>::max() - b) return std::nullopt;
    if (b < 0 && a < std::numeric_limits<T>::min() - b) return std::nullopt;
    return a + b;
}
```

### 4. Use Explicit Casting
```cpp
// Avoid implicit conversions
int32_t signed_val = -100;
uint32_t unsigned_val = static_cast<uint32_t>(signed_val);  // Explicit

// Use narrowing checks (C++20)
#include <utility>
auto [narrowed, overflowed] = std::narrow_cast<int8_t>(1000);
```

### 5. Document Type Choices
```cpp
// Good documentation
using UserId = uint64_t;  // User IDs are always positive, 64-bit for future growth
using Temperature = int16_t;  // -32768°C to +32767°C, covers all earthly temperatures
using HttpStatusCode = uint16_t;  // 0-65535, covers all possible status codes
```

## Summary

### Key Takeaways
1. **Signed integers**: Negative, zero, positive (-2^(n-1) to +2^(n-1)-1)
2. **Unsigned integers**: Zero and positive (0 to 2^n-1)
3. **Two's complement**: Standard for negative number representation
4. **Overflow**: Undefined for signed, defined wraparound for unsigned
5. **Mixed operations**: Signed converts to unsigned (dangerous)
6. **Choose types by range**: Use smallest type that fits your needs
7. **Performance**: Unsigned can be faster, smaller types save memory

### Bloomberg-Specific
- Use fixed-width types for portability
- Unsigned for quantities, signed for financial amounts
- Always check for overflow in critical calculations
- Follow Bloomberg naming conventions and patterns

### TypeScript Comparison
- JavaScript/TypeScript: All numbers are 64-bit floats
- No true integers, no overflow, but precision loss beyond 2^53
- BigInt for arbitrary precision integers
- TypedArrays for true integer performance

This guide provides comprehensive coverage of signed/unsigned integers at Bloomberg SDE-3 level. Focus on understanding the binary representation, arithmetic behavior, and choosing the right types for your use cases.
