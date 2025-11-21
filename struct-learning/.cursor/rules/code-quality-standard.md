# Code Quality Standards for Structs

## Overview
This document defines production grade code quality standards for struct and memory layout implementations. These standards ensure code is suitable for principal level review and production deployment in high performance systems.

## Function Size Limits
* **Maximum function length**: 50 lines (excluding comments and blank lines)
* **Rationale**: Functions exceeding 50 lines become difficult to understand, test, and maintain
* **Enforcement**: All functions must be reviewed for size compliance
* **Exception**: Complex struct manipulation functions may extend to 60 lines with justification

## File Size Limits
* **Maximum file length**: 200 lines (excluding comments and blank lines)
* **Rationale**: Files exceeding 200 lines become difficult to navigate and understand
* **Enforcement**: Split large files into logical modules
* **Exception**: Header files with extensive struct definitions may extend to 250 lines

## Cyclomatic Complexity
* **Maximum complexity**: 10 per function
* **Rationale**: High complexity indicates functions doing too much
* **Measurement**: Count decision points (if, while, for, switch, &&, ||, ?:)
* **Enforcement**: Refactor complex functions into smaller, focused functions
* **Exception**: Complex struct manipulation functions may have complexity up to 15 with justification

## Code Style

### Naming Conventions
* **Structs**: `PascalCase` (e.g., `Person`, `Order`, `NetworkPacket`)
* **Members**: `snake_case` (e.g., `user_id`, `order_amount`, `packet_size`)
* **Constants**: `UPPER_SNAKE_CASE` (e.g., `MAX_SIZE`, `DEFAULT_CAPACITY`)
* **Rationale**: Consistent naming improves readability and maintainability

### Indentation
* **Indentation**: 4 spaces (no tabs)
* **Continuation**: Align continuation lines with opening delimiter
* **Rationale**: Consistent indentation improves readability

### Comments
* **Struct comments**: Required for all public structs
* **Member comments**: Document non obvious members
* **Memory layout**: Document memory layout when important
* **Alignment**: Document alignment requirements
* **Rationale**: Comments clarify struct design and usage

## Error Handling

### Input Validation
* **All public APIs**: Must validate inputs
* **Null pointers**: Check and return error for NULL/nullptr inputs
* **Invalid ranges**: Validate parameter ranges
* **Preconditions**: Document and validate preconditions
* **Rationale**: Prevents crashes and undefined behavior

### Error Reporting
* **Return codes**: Use consistent return code conventions
* **Exceptions**: Use exceptions for C++ (prefer exceptions over return codes)
* **Error codes**: Define error codes in header files
* **Rationale**: Clear error reporting aids debugging

### Memory Safety
* **Bounds checking**: Check bounds for array members
* **Null checks**: Check for null before dereferencing
* **Use after free**: Avoid using freed memory
* **Double free**: Avoid double freeing memory
* **Rationale**: Memory safety prevents crashes and security vulnerabilities

## Struct Design

### Memory Layout
* **Member ordering**: Order members by size (largest first or smallest first for padding)
* **Padding minimization**: Minimize padding through ordering
* **Alignment**: Consider alignment requirements
* **Rationale**: Layout affects memory usage and performance

### Alignment
* **Natural alignment**: Use natural alignment when possible
* **Explicit alignment**: Use alignas when needed
* **Cache line alignment**: Align to cache lines for hot data
* **Rationale**: Alignment affects performance

### Padding
* **Minimization**: Minimize padding through member ordering
* **Explicit control**: Use packed structs when needed (with caution)
* **Documentation**: Document padding when important
* **Rationale**: Padding affects memory usage

## Performance

### Cache Efficiency
* **Hot cold splitting**: Separate hot and cold data
* **AoS vs SoA**: Choose appropriate layout (Array of Structs vs Struct of Arrays)
* **Cache line alignment**: Align hot data to cache lines
* **Rationale**: Cache efficiency improves performance

### Memory Access
* **Sequential access**: Design for sequential access when possible
* **Memory layout**: Consider memory access patterns
* **Rationale**: Memory access patterns affect performance

## Testing

### Unit Tests
* **Coverage**: Aim for 90%+ code coverage
* **Memory layout tests**: Test memory layout correctness
* **Alignment tests**: Test alignment requirements
* **Padding tests**: Test padding behavior
* **Rationale**: Comprehensive testing ensures correctness

## Documentation

### API Documentation
* **Public structs**: Document all public structs
* **Members**: Document non obvious members
* **Memory layout**: Document memory layout when important
* **Alignment**: Document alignment requirements
* **Examples**: Provide usage examples
* **Rationale**: Clear documentation enables correct usage

## Examples

### Good Struct Design (Within Limits)
```cpp
// Thread safety: Not thread safe (mutable state)
// Ownership: Owns no resources
// Invariants: id > 0, amount >= 0
// Failure modes: Undefined behavior if invariants violated
// Memory layout: 16 bytes (8 + 4 + 4 padding)
struct alignas(8) Order {
    uint64_t id;      // 8 bytes
    uint32_t user_id; // 4 bytes
    // 4 bytes padding
    
    Order() : id(0), user_id(0) {}
    Order(uint64_t i, uint32_t u) : id(i), user_id(u) {}
};
```

### Bad Struct Design (Exceeds Limits)
```cpp
// BAD: Poor layout, no alignment, exceeds 200 lines
struct ComplexStruct {
    // 250+ lines of complex logic
    // Poor memory layout
    // No alignment considerations
    // Difficult to test and maintain
};
```

## Enforcement

### Code Review
* **Mandatory**: All code must be reviewed for compliance
* **Checklist**: Use checklist to verify standards
* **Struct review**: Special attention to memory layout and alignment
* **Automation**: Use tools to check function/file sizes and complexity

### CI/CD
* **Static analysis**: Run static analysis tools in CI
* **Linting**: Run linters to check style compliance
* **Testing**: Run tests to verify correctness
* **Memory sanitizers**: Run memory sanitizers to detect issues
* **Metrics**: Track code quality metrics over time
