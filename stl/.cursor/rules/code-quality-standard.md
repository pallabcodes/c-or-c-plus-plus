# Code Quality Standards for STL

## Overview
This document defines production grade code quality standards for STL implementations and usage. These standards ensure code is suitable for principal level review and production deployment in high performance systems.

## Function Size Limits
* **Maximum function length**: 50 lines (excluding comments and blank lines)
* **Rationale**: Functions exceeding 50 lines become difficult to understand, test, and maintain
* **Enforcement**: All functions must be reviewed for size compliance
* **Exception**: Complex STL algorithm wrappers may extend to 60 lines with justification

## File Size Limits
* **Maximum file length**: 200 lines (excluding comments and blank lines)
* **Rationale**: Files exceeding 200 lines become difficult to navigate and understand
* **Enforcement**: Split large files into logical modules
* **Exception**: Header files with extensive template definitions may extend to 250 lines

## Cyclomatic Complexity
* **Maximum complexity**: 10 per function
* **Rationale**: High complexity indicates functions doing too much
* **Measurement**: Count decision points (if, while, for, switch, &&, ||, ?:)
* **Enforcement**: Refactor complex functions into smaller, focused functions
* **Exception**: Complex algorithm implementations may have complexity up to 15 with justification

## Code Style

### Naming Conventions
* **Functions**: `snake_case` (e.g., `process_vector`, `find_element`, `sort_container`)
* **Classes**: `PascalCase` (e.g., `CustomContainer`, `IteratorWrapper`)
* **Types**: `snake_case` with `_t` suffix or `PascalCase` (e.g., `container_t`, `IteratorType`)
* **Constants**: `UPPER_SNAKE_CASE` (e.g., `MAX_SIZE`, `DEFAULT_CAPACITY`)
* **Template parameters**: `PascalCase` single letters (e.g., `T`, `Container`, `Iterator`)
* **Rationale**: Consistent naming improves readability and maintainability

### Indentation
* **Indentation**: 4 spaces (no tabs)
* **Continuation**: Align continuation lines with opening delimiter
* **Rationale**: Consistent indentation improves readability

### Comments
* **Function comments**: Required for all public functions
* **Complexity**: Document time and space complexity
* **Iterator validity**: Document iterator validity guarantees
* **Exception safety**: Document exception safety guarantees
* **Rationale**: Comments clarify STL usage and guarantees

## Error Handling

### Input Validation
* **All public APIs**: Must validate inputs
* **Null pointers**: Check and return error for NULL inputs
* **Iterator validation**: Validate iterator ranges
* **Container validation**: Validate container state
* **Rationale**: Prevents crashes and undefined behavior

### Error Reporting
* **Return codes**: Use consistent return code conventions
* **Error codes**: Define error codes in header files
* **Exceptions**: Use exceptions for C++ (prefer exceptions over return codes)
* **Rationale**: Clear error reporting aids debugging

### Exception Safety
* **Basic guarantee**: Maintain valid state on exceptions
* **Strong guarantee**: All or nothing operations
* **No throw guarantee**: Operations that never throw
* **Rationale**: Exception safety ensures robustness

## STL Usage

### Container Selection
* **Appropriate container**: Choose container based on usage patterns
* **Performance**: Consider time complexity of operations
* **Memory**: Consider memory overhead
* **Rationale**: Correct container selection optimizes performance

### Iterator Usage
* **Valid iterators**: Ensure iterators are valid before use
* **Invalidation**: Understand iterator invalidation rules
* **Range validation**: Validate iterator ranges
* **Rationale**: Correct iterator usage prevents undefined behavior

### Algorithm Selection
* **Appropriate algorithm**: Choose algorithm based on requirements
* **Complexity**: Consider time and space complexity
* **Stability**: Consider stability requirements
* **Rationale**: Correct algorithm selection optimizes performance

## Memory Safety

### Iterator Safety
* **Valid ranges**: Ensure iterator ranges are valid
* **No invalidation**: Avoid using invalidated iterators
* **Bounds checking**: Use bounds checking in debug builds
* **Rationale**: Iterator safety prevents undefined behavior

### Container Safety
* **Reserve capacity**: Reserve capacity when size is known
* **Move semantics**: Use move semantics to avoid copies
* **RAII**: Use RAII for resource management
* **Rationale**: Container safety prevents memory issues

## Performance

### Container Operations
* **Efficient operations**: Use efficient container operations
* **Reserve capacity**: Reserve capacity to avoid reallocations
* **Move semantics**: Use move semantics to avoid copies
* **Rationale**: Efficient operations improve performance

### Algorithm Performance
* **Appropriate algorithms**: Choose algorithms with good complexity
* **Avoid unnecessary copies**: Minimize copies
* **Cache efficiency**: Consider cache friendly access patterns
* **Rationale**: Algorithm performance affects overall performance

## Testing

### Unit Tests
* **Coverage**: Aim for 90%+ code coverage
* **Operations**: Test all container and algorithm operations
* **Edge cases**: Test boundary conditions
* **Iterator tests**: Test iterator operations
* **Rationale**: Comprehensive testing ensures correctness

### Performance Tests
* **Benchmarks**: Benchmark critical operations
* **Complexity verification**: Verify complexity guarantees
* **Memory profiling**: Profile memory usage
* **Rationale**: Performance tests ensure performance goals

## Documentation

### API Documentation
* **Public functions**: Document all public functions
* **Complexity**: Document time and space complexity
* **Iterator validity**: Document iterator validity guarantees
* **Exception safety**: Document exception safety guarantees
* **Parameters**: Document all parameters and return values
* **Examples**: Provide usage examples
* **Rationale**: Clear documentation enables correct usage

## Examples

### Good Function (Within Limits)
```cpp
// Complexity: O(n) time, O(1) space
// Iterator validity: Input iterators remain valid
// Exception safety: Strong guarantee
template<typename InputIt, typename OutputIt>
OutputIt copy_even(InputIt first, InputIt last, OutputIt d_first) {
    if (first == last) {
        return d_first;
    }
    
    return std::copy_if(first, last, d_first,
                       [](const auto& value) {
                           return value % 2 == 0;
                       });
}
```

### Bad Function (Exceeds Limits)
```cpp
// BAD: Function exceeds 50 lines and has high complexity
template<typename Container>
void complex_operation(Container& c) {
    // 60+ lines of complex logic with nested conditionals
    // High cyclomatic complexity (> 10)
    // Difficult to test and maintain
}
```

## Enforcement

### Code Review
* **Mandatory**: All code must be reviewed for compliance
* **Checklist**: Use checklist to verify standards
* **STL usage review**: Special attention to STL usage
* **Automation**: Use tools to check function/file sizes and complexity

### CI/CD
* **Static analysis**: Run static analysis tools in CI
* **Linting**: Run linters to check style compliance
* **Testing**: Run tests to verify correctness
* **Benchmarking**: Run benchmarks to verify performance
* **Metrics**: Track code quality metrics over time

