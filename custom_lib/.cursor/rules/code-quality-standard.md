# Code Quality Standards for Custom Library Development

## Overview
This document defines production grade code quality standards for custom library implementations, specifically for printf and write functions. These standards ensure code is suitable for principal level review and production deployment.

## Function Size Limits
* **Maximum function length**: 50 lines (excluding comments and blank lines)
* **Rationale**: Functions exceeding 50 lines become difficult to understand, test, and maintain
* **Enforcement**: All functions must be reviewed for size compliance
* **Exception**: Complex parsing functions may extend to 60 lines with justification

## File Size Limits
* **Maximum file length**: 200 lines (excluding comments and blank lines)
* **Rationale**: Files exceeding 200 lines become difficult to navigate and understand
* **Enforcement**: Split large files into logical modules
* **Exception**: Header files with extensive type definitions may extend to 250 lines

## Cyclomatic Complexity
* **Maximum complexity**: 10 per function
* **Rationale**: High complexity indicates functions doing too much
* **Measurement**: Count decision points (if, while, for, switch, &&, ||, ?:)
* **Enforcement**: Refactor complex functions into smaller, focused functions
* **Exception**: State machines may have complexity up to 15 with justification

## Code Style

### Naming Conventions
* **Functions**: `snake_case` (e.g., `parse_format_string`, `buffer_flush`)
* **Types**: `snake_case` with `_t` suffix (e.g., `format_spec_t`, `buffer_t`)
* **Constants**: `UPPER_SNAKE_CASE` (e.g., `BUFFER_SIZE`, `MAX_FORMAT_SPECS`)
* **Macros**: `UPPER_SNAKE_CASE` (e.g., `BUFFER_SIZE`, `FORMAT_FLAG_MINUS`)
* **Rationale**: Consistent naming improves readability and maintainability

### Indentation
* **Indentation**: 4 spaces (no tabs)
* **Continuation**: Align continuation lines with opening delimiter
* **Rationale**: Consistent indentation improves readability

### Comments
* **Function comments**: Required for all public functions
* **Complex logic**: Required for non obvious algorithms
* **Format**: Use `/* */` for block comments, `//` for line comments
* **Content**: Explain why, not what (code should be self documenting)
* **Rationale**: Comments clarify intent and rationale

## Error Handling

### Input Validation
* **All public APIs**: Must validate inputs
* **Null pointers**: Check and return error for NULL inputs
* **Bounds checking**: Validate array bounds and buffer sizes
* **Format strings**: Validate format specifier syntax
* **Rationale**: Prevents undefined behavior and security vulnerabilities

### Error Reporting
* **Return codes**: Use consistent return code conventions
* **Error codes**: Define error codes in header files
* **Error messages**: Provide clear, actionable error messages
* **Status propagation**: Propagate errors correctly through call stack
* **Rationale**: Clear error reporting aids debugging and integration

### Error Recovery
* **Graceful degradation**: Handle errors without crashing
* **Resource cleanup**: Clean up resources on error paths
* **Partial success**: Handle partial operation success correctly
* **Rationale**: Robust error handling improves reliability

## Memory Safety

### Allocation
* **Stack allocation**: Prefer stack allocation for small, fixed size buffers
* **Heap allocation**: Use heap allocation only when necessary
* **Buffer sizes**: Use constants for buffer sizes, avoid magic numbers
* **Rationale**: Stack allocation is faster and safer

### Bounds Checking
* **Array access**: Always validate array indices
* **Buffer writes**: Check buffer capacity before writes
* **String operations**: Use bounded string functions (strnlen, strncpy)
* **Rationale**: Prevents buffer overflows and undefined behavior

### Leak Prevention
* **Allocation pairs**: Every allocation must have corresponding free
* **Error paths**: Free resources in all error paths
* **Early returns**: Clean up before early returns
* **Rationale**: Memory leaks cause resource exhaustion

## Thread Safety

### Reentrancy
* **Thread safe functions**: Functions must be reentrant where possible
* **Global state**: Minimize global state, use thread local storage when needed
* **Locks**: Use locks only when necessary, prefer lock free algorithms
* **Rationale**: Thread safety enables concurrent usage

### Synchronization
* **Critical sections**: Minimize critical section size
* **Lock ordering**: Establish consistent lock ordering to prevent deadlocks
* **Lock free**: Use atomic operations where possible
* **Rationale**: Proper synchronization prevents race conditions

## Performance

### Optimization
* **Hot paths**: Optimize frequently executed code paths
* **Common cases**: Fast path for common format specifiers
* **System calls**: Minimize system call overhead through buffering
* **Rationale**: Performance is critical for I/O operations

### Benchmarking
* **Benchmarks**: Include benchmarks for performance critical functions
* **Profiling**: Profile code to identify bottlenecks
* **Metrics**: Measure throughput, latency, and memory usage
* **Rationale**: Data driven optimization decisions

## Testing

### Unit Tests
* **Coverage**: Aim for 90%+ code coverage
* **Edge cases**: Test boundary conditions and error cases
* **Format specifiers**: Test all format specifier combinations
* **Rationale**: Comprehensive testing ensures correctness

### Integration Tests
* **End to end**: Test complete printf/write workflows
* **Error scenarios**: Test error handling and recovery
* **Concurrency**: Test thread safety with concurrent access
* **Rationale**: Integration tests verify system behavior

### Fuzzing
* **Format strings**: Fuzz format string parsing
* **Input validation**: Fuzz input validation logic
* **Security**: Fuzz for security vulnerabilities
* **Rationale**: Fuzzing finds edge cases and security issues

## Documentation

### API Documentation
* **Public functions**: Document all public functions
* **Parameters**: Document all parameters and return values
* **Thread safety**: Document thread safety guarantees
* **Ownership**: Document memory ownership semantics
* **Rationale**: Clear documentation enables correct usage

### Implementation Documentation
* **Algorithms**: Document complex algorithms
* **Design decisions**: Document non obvious design choices
* **Performance**: Document performance characteristics
* **Rationale**: Implementation docs aid maintenance

## Security

### Format String Security
* **Validation**: Validate format strings to prevent injection
* **Bounds**: Check bounds on all format specifier values
* **Sanitization**: Sanitize user provided format strings
* **Rationale**: Format string vulnerabilities are security risks

### Input Validation
* **All inputs**: Validate all user provided inputs
* **Bounds**: Check bounds on all numeric inputs
* **Sanitization**: Sanitize inputs before processing
* **Rationale**: Input validation prevents security vulnerabilities

## Examples

### Good Function (Within Limits)
```c
// Thread safety: Thread safe (pure function, no shared state)
// Ownership: Caller owns fmt, caller owns specs array
// Invariants: fmt must not be NULL, specs must not be NULL, max_specs > 0
// Failure modes: Returns 0 on invalid format string
size_t parse_format_string(const char *fmt, format_spec_t *specs, size_t max_specs) {
    if (!fmt || !specs || max_specs == 0) {
        return 0;
    }
    
    size_t count = 0;
    const char *p = fmt;
    
    while (*p && count < max_specs) {
        if (*p == '%') {
            // Parse format specifier
            if (parse_specifier(&p, &specs[count]) == 0) {
                break;
            }
            count++;
        } else {
            p++;
        }
    }
    
    return count;
}
```

### Bad Function (Exceeds Limits)
```c
// BAD: Function exceeds 50 lines and has high complexity
size_t parse_format_string(const char *fmt, format_spec_t *specs, size_t max_specs) {
    // 60+ lines of complex parsing logic with nested conditionals
    // High cyclomatic complexity (> 10)
    // Difficult to test and maintain
}
```

## Enforcement

### Code Review
* **Mandatory**: All code must be reviewed for compliance
* **Checklist**: Use checklist to verify standards
* **Automation**: Use tools to check function/file sizes and complexity

### CI/CD
* **Static analysis**: Run static analysis tools in CI
* **Linting**: Run linters to check style compliance
* **Testing**: Run tests to verify correctness
* **Metrics**: Track code quality metrics over time

