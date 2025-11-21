# Format String Parsing Standards

## Overview
Format string parsing is the foundation of printf implementation. This document defines standards for parsing ISO C Standard format specifiers with correctness, performance, and security as primary concerns.

## ISO C Standard Format Specifier Structure

### Format Specifier Components
```
%[flags][width][.precision][length]specifier
```

### Flags
* `-`: Left justify (default: right justify)
* `+`: Force sign for signed types (default: only negative)
* ` `: Space before positive numbers (default: no space)
* `0`: Zero padding (default: space padding)
* `#`: Alternate form (0x prefix for hex, etc.)

### Width
* Integer specifying minimum field width
* `*`: Width from argument
* Default: No minimum width

### Precision
* `.` followed by integer or `*`
* For integers: Minimum number of digits
* For floats: Number of decimal places
* For strings: Maximum characters to print
* Default: Implementation defined

### Length Modifiers
* `h`: short (e.g., `%hd`, `%hu`)
* `hh`: char (e.g., `%hhd`, `%hhu`)
* `l`: long (e.g., `%ld`, `%lu`)
* `ll`: long long (e.g., `%lld`, `%llu`)
* `L`: long double (e.g., `%Lf`)
* `z`: size_t (e.g., `%zu`)
* `t`: ptrdiff_t (e.g., `%td`)

### Specifiers
* `d`, `i`: Signed decimal integer
* `u`: Unsigned decimal integer
* `o`: Unsigned octal
* `x`, `X`: Unsigned hexadecimal (lowercase/uppercase)
* `f`, `F`: Floating point (decimal notation)
* `e`, `E`: Floating point (scientific notation)
* `g`, `G`: Floating point (shortest representation)
* `c`: Character
* `s`: String
* `p`: Pointer address
* `n`: Number of characters written (not implemented in safe implementations)
* `%`: Literal percent sign

## Implementation Standards

### Data Structure
```c
typedef struct {
    char flag_minus;      // Left justify flag
    char flag_plus;       // Force sign flag
    char flag_space;      // Space before positive
    char flag_zero;       // Zero padding
    char flag_hash;       // Alternate form
    int width;            // Field width (-1 if not specified, -2 if *)
    int precision;        // Precision (-1 if not specified, -2 if *)
    char length_modifier[3]; // Length modifier (null terminated)
    char specifier;       // Format specifier character
} format_spec_t;
```

### Parsing Algorithm

#### State Machine Approach
* **State 0**: Start of format specifier (after `%`)
* **State 1**: Parsing flags
* **State 2**: Parsing width
* **State 3**: Parsing precision (after `.`)
* **State 4**: Parsing length modifier
* **State 5**: Parsing specifier
* **State 6**: Complete

#### Parsing Function Requirements
* **Input validation**: Check for NULL format string
* **Bounds checking**: Prevent buffer overflows
* **Error handling**: Return error on invalid format
* **Performance**: Optimize for common cases
* **Security**: Validate all numeric values

### Common Case Optimization

#### Fast Path for Simple Specifiers
* `%d`, `%s`, `%c`: Most common specifiers
* Optimize parsing for these cases
* Avoid full state machine for simple cases

#### Example Fast Path
```c
// Fast path for %d
if (fmt[0] == '%' && fmt[1] == 'd' && fmt[2] == '\0') {
    // Direct integer formatting
    return format_integer(va_arg(args, int), 10, 0, 0, 0);
}
```

## Security Considerations

### Format String Vulnerabilities
* **Never use user provided format strings**: Always validate format strings
* **Bounds checking**: Check all numeric values (width, precision)
* **Integer overflow**: Prevent integer overflow in width/precision
* **Buffer overflow**: Prevent buffer overflow in parsing

### Input Validation
* **Format string validation**: Validate format string syntax
* **Numeric bounds**: Limit width and precision to reasonable values
* **Length limits**: Limit format string length
* **Character validation**: Validate specifier characters

### Example Validation
```c
// Validate width and precision
if (spec->width > MAX_WIDTH || spec->width < -2) {
    return ERROR_INVALID_WIDTH;
}
if (spec->precision > MAX_PRECISION || spec->precision < -2) {
    return ERROR_INVALID_PRECISION;
}
```

## Error Handling

### Error Codes
* `PARSE_SUCCESS`: Successful parsing
* `PARSE_ERROR_INVALID_FORMAT`: Invalid format string
* `PARSE_ERROR_TOO_MANY_SPECS`: Too many format specifiers
* `PARSE_ERROR_INVALID_SPEC`: Invalid specifier
* `PARSE_ERROR_OVERFLOW`: Integer overflow in width/precision

### Error Reporting
* **Return codes**: Use consistent return code convention
* **Error position**: Report position of error in format string
* **Error context**: Provide context about what failed
* **Rationale**: Clear error reporting aids debugging

## Performance Optimization

### Parsing Performance
* **Fast path**: Optimize common format specifiers
* **State machine**: Efficient state machine implementation
* **Lookup tables**: Use lookup tables for flag parsing
* **Branch prediction**: Structure code for good branch prediction

### Benchmarking
* **Common cases**: Benchmark common format strings
* **Complex cases**: Benchmark complex format strings
* **Throughput**: Measure format strings per second
* **Latency**: Measure parsing latency

## Testing Requirements

### Unit Tests
* **All specifiers**: Test all format specifier types
* **All flags**: Test all flag combinations
* **Width/precision**: Test width and precision variations
* **Length modifiers**: Test all length modifiers
* **Edge cases**: Test boundary conditions

### Invalid Input Tests
* **Invalid format**: Test invalid format strings
* **Overflow**: Test integer overflow cases
* **Bounds**: Test boundary value cases
* **Null inputs**: Test NULL pointer handling

### Fuzzing
* **Format strings**: Fuzz format string parsing
* **Security**: Fuzz for security vulnerabilities
* **Edge cases**: Fuzz for edge case discovery

## Research Papers and References

### Format String Parsing
* ISO C Standard (C11/C17) - Format specifier specification
* "Secure Coding in C and C++" - Format string vulnerabilities
* "Efficient String Formatting" - Performance optimization

### Open Source References
* glibc printf format parsing implementation
* musl libc printf format parsing implementation
* Google Abseil string formatting

## Implementation Checklist

- [ ] Implement format specifier data structure
- [ ] Implement state machine parser
- [ ] Add fast path for common specifiers
- [ ] Implement input validation
- [ ] Add bounds checking
- [ ] Implement error handling
- [ ] Add security validation
- [ ] Write comprehensive unit tests
- [ ] Add fuzzing tests
- [ ] Benchmark performance
- [ ] Document API and behavior

