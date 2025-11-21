# Code Quality Standards for Pointers

## Overview
This document defines production grade code quality standards for pointer and reference implementations. These standards ensure code is suitable for principal level review and production deployment in high performance systems.

## Function Size Limits
* **Maximum function length**: 50 lines (excluding comments and blank lines)
* **Rationale**: Functions exceeding 50 lines become difficult to understand, test, and maintain
* **Enforcement**: All functions must be reviewed for size compliance
* **Exception**: Complex pointer manipulation functions may extend to 60 lines with justification

## File Size Limits
* **Maximum file length**: 200 lines (excluding comments and blank lines)
* **Rationale**: Files exceeding 200 lines become difficult to navigate and understand
* **Enforcement**: Split large files into logical modules
* **Exception**: Header files with extensive pointer type definitions may extend to 250 lines

## Cyclomatic Complexity
* **Maximum complexity**: 10 per function
* **Rationale**: High complexity indicates functions doing too much
* **Measurement**: Count decision points (if, while, for, switch, &&, ||, ?:)
* **Enforcement**: Refactor complex functions into smaller, focused functions
* **Exception**: Complex pointer manipulation functions may have complexity up to 15 with justification

## Code Style

### Naming Conventions
* **Pointers**: `ptr` suffix or descriptive name (e.g., `data_ptr`, `node_ptr`)
* **References**: `ref` suffix or descriptive name (e.g., `data_ref`, `node_ref`)
* **Function pointers**: `func_ptr` or descriptive name (e.g., `callback_func`, `handler_func`)
* **Void pointers**: `vptr` or descriptive name (e.g., `generic_ptr`, `data_vptr`)
* **Rationale**: Consistent naming improves readability and maintainability

### Indentation
* **Indentation**: 4 spaces (no tabs)
* **Continuation**: Align continuation lines with opening delimiter
* **Rationale**: Consistent indentation improves readability

### Comments
* **Pointer comments**: Required for all pointer parameters
* **Ownership**: Document pointer ownership semantics
* **Null guarantees**: Document null pointer guarantees
* **Rationale**: Comments clarify pointer usage and safety

## Error Handling

### Input Validation
* **All public APIs**: Must validate inputs
* **Null pointers**: Check and return error for NULL/nullptr inputs
* **Invalid pointers**: Validate pointer validity when possible
* **Preconditions**: Document and validate preconditions
* **Rationale**: Prevents crashes and undefined behavior

### Error Reporting
* **Return codes**: Use consistent return code conventions
* **Exceptions**: Use exceptions for C++ (prefer exceptions over return codes)
* **Error codes**: Define error codes in header files
* **Rationale**: Clear error reporting aids debugging

### Memory Safety
* **Null checks**: Always check for null before dereferencing
* **Bounds checking**: Check array bounds for pointer arithmetic
* **Use after free**: Avoid using freed memory
* **Double free**: Avoid double freeing memory
* **Rationale**: Memory safety prevents crashes and security vulnerabilities

## Pointer Safety

### Null Pointer Checks
* **Before dereference**: Always check for null
* **Initialization**: Initialize pointers to nullptr
* **Return values**: Check return values from functions returning pointers
* **Rationale**: Null checks prevent crashes

### Dangling Pointers
* **Lifetime management**: Ensure pointer validity
* **Scope awareness**: Be aware of pointer scope
* **Use after free**: Never use freed memory
* **Rationale**: Dangling pointers cause undefined behavior

### Pointer Arithmetic
* **Bounds checking**: Check bounds before pointer arithmetic
* **Array bounds**: Validate array bounds
* **Type safety**: Use correct types for pointer arithmetic
* **Rationale**: Pointer arithmetic safety prevents buffer overflows

## Memory Management

### Ownership
* **Clear ownership**: Document ownership semantics
* **Raw pointers**: Use raw pointers only for non owning references
* **Smart pointers**: Use smart pointers for ownership (C++)
* **Rationale**: Clear ownership prevents memory leaks

### Deallocation
* **Match allocation**: Match deallocation with allocation
* **Single deallocation**: Deallocate once per allocation
* **Null before delete**: Set pointer to nullptr after delete
* **Rationale**: Proper deallocation prevents leaks and double free

## Performance

### Pointer Indirection
* **Minimize indirection**: Avoid unnecessary pointer indirection
* **Cache efficiency**: Consider cache efficiency
* **Pointer chasing**: Minimize pointer chasing
* **Rationale**: Pointer indirection has overhead

### Memory Access
* **Cache friendly**: Design for cache friendly access
* **Memory layout**: Consider memory layout
* **Rationale**: Memory access patterns affect performance

## Testing

### Unit Tests
* **Coverage**: Aim for 90%+ code coverage
* **Null pointer tests**: Test null pointer handling
* **Dangling pointer tests**: Test pointer lifetime
* **Memory leak tests**: Test for memory leaks
* **Rationale**: Comprehensive testing ensures correctness

## Documentation

### API Documentation
* **Public functions**: Document all public functions
* **Pointer parameters**: Document pointer ownership and null guarantees
* **Return values**: Document return value ownership and null guarantees
* **Examples**: Provide usage examples
* **Rationale**: Clear documentation enables correct usage

## Examples

### Good Pointer Usage (Within Limits)
```cpp
// Thread safety: Not thread safe (mutable state)
// Ownership: Borrows data pointer (does not own)
// Invariants: data must not be null, size > 0
// Failure modes: Returns -1 if data is null or size is invalid
int process_data(int* data, size_t size) {
    if (!data || size == 0) {
        return -1;
    }
    
    int sum = 0;
    for (size_t i = 0; i < size; ++i) {
        sum += data[i];
    }
    return sum;
}
```

### Bad Pointer Usage (Exceeds Limits)
```cpp
// BAD: No null check, exceeds 50 lines, high complexity
int process_data(int* data, size_t size) {
    // 60+ lines of complex logic
    // No null pointer checks
    // High cyclomatic complexity (> 10)
    // Difficult to test and maintain
}
```

## Enforcement

### Code Review
* **Mandatory**: All code must be reviewed for compliance
* **Checklist**: Use checklist to verify standards
* **Pointer safety review**: Special attention to pointer safety
* **Automation**: Use tools to check function/file sizes and complexity

### CI/CD
* **Static analysis**: Run static analysis tools in CI
* **Linting**: Run linters to check style compliance
* **Testing**: Run tests to verify correctness
* **Memory sanitizers**: Run memory sanitizers to detect issues
* **Metrics**: Track code quality metrics over time

