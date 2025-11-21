# Type Conversion and Formatting Standards

## Overview
Type conversion and formatting is the core of printf implementation. This document defines standards for converting values to formatted strings with correctness, precision, and performance as primary concerns.

## Integer Formatting

### Decimal Formatting (d, i, u)
* **Signed integers**: Handle sign, handle negative numbers correctly
* **Unsigned integers**: Handle unsigned numbers correctly
* **Width and precision**: Apply width and precision correctly
* **Padding**: Apply zero or space padding correctly
* **Sign handling**: Handle + and space flags correctly

### Octal Formatting (o)
* **Base conversion**: Convert to base 8
* **Alternate form**: Add 0 prefix when # flag is set
* **Width and precision**: Apply width and precision correctly
* **Padding**: Apply zero or space padding correctly

### Hexadecimal Formatting (x, X)
* **Base conversion**: Convert to base 16
* **Case handling**: Handle lowercase (x) and uppercase (X)
* **Alternate form**: Add 0x or 0X prefix when # flag is set
* **Width and precision**: Apply width and precision correctly
* **Padding**: Apply zero or space padding correctly

### Implementation Example
```c
// Thread safety: Thread safe (pure function, no shared state)
// Ownership: Caller owns output buffer
// Invariants: output must have capacity >= required, value is valid
// Failure modes: Returns -1 on buffer overflow, returns written length on success
int format_integer(long long value, int base, char *output, size_t output_size,
                   int width, int precision, int flags) {
    if (!output || output_size == 0) {
        return -1;
    }
    
    char buffer[64]; // Enough for any integer
    int index = 0;
    int is_negative = (value < 0 && base == 10);
    unsigned long long uvalue = is_negative ? -(unsigned long long)value : value;
    
    // Convert to string (reverse order)
    do {
        int digit = uvalue % base;
        buffer[index++] = (digit < 10) ? '0' + digit : 'a' + (digit - 10);
        uvalue /= base;
    } while (uvalue > 0 && index < 64);
    
    // Apply precision (minimum digits)
    while (index < precision && index < 64) {
        buffer[index++] = '0';
    }
    
    // Reverse string
    reverse_string(buffer, index);
    
    // Apply width, flags, and sign
    return format_with_width_flags(buffer, index, output, output_size,
                                   width, flags, is_negative);
}
```

## Floating Point Formatting

### Decimal Notation (f, F)
* **Precision**: Apply precision (decimal places)
* **Rounding**: Round correctly to specified precision
* **Width**: Apply width correctly
* **Padding**: Apply zero or space padding correctly
* **Sign**: Handle + and space flags correctly
* **Special values**: Handle NaN, Infinity correctly

### Scientific Notation (e, E)
* **Mantissa and exponent**: Format as mantissa and exponent
* **Precision**: Apply precision to mantissa
* **Exponent format**: Format exponent correctly (e.g., e+03)
* **Case**: Handle lowercase (e) and uppercase (E)
* **Special values**: Handle NaN, Infinity correctly

### Shortest Representation (g, G)
* **Algorithm**: Choose shortest representation (f or e)
* **Precision**: Apply precision correctly
* **Trailing zeros**: Remove trailing zeros
* **Decimal point**: Remove decimal point if not needed
* **Case**: Handle lowercase (g) and uppercase (G)

### Implementation Considerations
* **Precision limits**: Limit precision to reasonable values
* **Rounding**: Use correct rounding mode (round half to even)
* **Edge cases**: Handle very large and very small numbers
* **Performance**: Optimize common cases

## String Formatting

### String Output (s)
* **Null termination**: Handle null terminated strings
* **Precision**: Limit output to precision characters
* **Width**: Apply width correctly
* **Padding**: Apply space padding correctly
* **Null pointer**: Handle NULL pointer (print "(null)" or similar)

### Character Output (c)
* **Character output**: Output single character
* **Width**: Apply width correctly
* **Padding**: Apply space padding correctly
* **Encoding**: Handle multibyte characters correctly

### Pointer Output (p)
* **Address format**: Format pointer address as hexadecimal
* **Width**: Apply width correctly
* **Padding**: Apply zero padding correctly
* **Case**: Use lowercase hexadecimal

## Formatting Flags

### Left Justify (-)
* **Alignment**: Left align output in field
* **Padding**: Add padding on right
* **Rationale**: Left justify for readability

### Force Sign (+)
* **Sign display**: Always show sign for signed types
* **Positive numbers**: Show + for positive numbers
* **Rationale**: Explicit sign indication

### Space Flag ( )
* **Space before positive**: Add space before positive numbers
* **Negative numbers**: No space before negative numbers
* **Rationale**: Align positive and negative numbers

### Zero Padding (0)
* **Zero padding**: Pad with zeros instead of spaces
* **Left justify**: Ignored when left justify is set
* **Rationale**: Zero padding for numeric alignment

### Alternate Form (#)
* **Octal**: Add 0 prefix for octal
* **Hexadecimal**: Add 0x or 0X prefix for hexadecimal
* **Floating point**: Always show decimal point for f, F
* **Rationale**: Explicit base indication

## Width and Precision

### Width Handling
* **Minimum width**: Ensure output is at least width characters
* **Padding**: Add padding to reach width
* **Overflow**: Output exceeds width if content is larger
* **Asterisk**: Get width from argument

### Precision Handling
* **Integers**: Minimum number of digits
* **Floats**: Number of decimal places
* **Strings**: Maximum characters to output
* **Asterisk**: Get precision from argument

## Error Handling

### Conversion Errors
* **Overflow**: Handle integer overflow
* **Underflow**: Handle floating point underflow
* **Invalid input**: Handle invalid input values
* **Error reporting**: Return error codes

### Buffer Overflow
* **Bounds checking**: Check buffer capacity
* **Truncation**: Handle truncation gracefully
* **Error reporting**: Return error on overflow
* **Rationale**: Prevent buffer overflows

## Performance Optimization

### Fast Paths
* **Common cases**: Optimize common format specifiers
* **Small integers**: Fast path for small integers
* **Simple floats**: Fast path for simple floats
* **Rationale**: Optimize hot paths

### Lookup Tables
* **Digit conversion**: Use lookup tables for digit conversion
* **Flag combinations**: Optimize flag combinations
* **Rationale**: Reduce computation overhead

### Benchmarking
* **Throughput**: Measure conversions per second
* **Latency**: Measure conversion latency
* **Memory**: Measure memory usage
* **Rationale**: Data driven optimization

## Testing Requirements

### Unit Tests
* **All types**: Test all type conversions
* **All flags**: Test all flag combinations
* **Width/precision**: Test width and precision variations
* **Edge cases**: Test boundary conditions
* **Special values**: Test NaN, Infinity, NULL

### Correctness Tests
* **Round trip**: Test round trip conversion
* **Precision**: Test precision correctness
* **Rounding**: Test rounding correctness
* **Rationale**: Ensure correctness

## Research Papers and References

### Floating Point Formatting
* IEEE 754 Standard - Floating point representation
* "What Every Computer Scientist Should Know About Floating Point Arithmetic"
* "Printing Floating Point Numbers Quickly and Accurately"

### Integer Formatting
* "Efficient Integer to String Conversion"
* "Fast Integer Formatting"

### Open Source References
* glibc printf type conversion implementation
* musl libc printf type conversion implementation
* Google Abseil string formatting

## Implementation Checklist

- [ ] Implement integer formatting (decimal, octal, hexadecimal)
- [ ] Implement floating point formatting (f, e, g)
- [ ] Implement string and character formatting
- [ ] Implement pointer formatting
- [ ] Implement all formatting flags
- [ ] Implement width and precision handling
- [ ] Add error handling
- [ ] Add performance optimizations
- [ ] Write comprehensive unit tests
- [ ] Add correctness tests
- [ ] Benchmark performance
- [ ] Document API and behavior

