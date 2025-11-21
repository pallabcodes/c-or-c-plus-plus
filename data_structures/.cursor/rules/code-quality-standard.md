# Code Quality Standards for Data Structures

## Overview
This document defines production grade code quality standards for data structure implementations. These standards ensure code is suitable for principal level review and production deployment.

## Function Size Limits
* **Maximum function length**: 50 lines (excluding comments and blank lines)
* **Rationale**: Functions exceeding 50 lines become difficult to understand, test, and maintain
* **Enforcement**: All functions must be reviewed for size compliance
* **Exception**: Complex algorithms may extend to 60 lines with justification

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
* **Exception**: Tree traversal algorithms may have complexity up to 15 with justification

## Code Style

### Naming Conventions
* **Classes**: `PascalCase` (e.g., `DynamicArray`, `BinarySearchTree`)
* **Functions**: `snake_case` (e.g., `insert_element`, `delete_node`)
* **Types**: `snake_case` with `_t` suffix (e.g., `node_t`, `tree_t`)
* **Constants**: `UPPER_SNAKE_CASE` (e.g., `MAX_SIZE`, `DEFAULT_CAPACITY`)
* **Rationale**: Consistent naming improves readability and maintainability

### Indentation
* **Indentation**: 4 spaces (no tabs)
* **Continuation**: Align continuation lines with opening delimiter
* **Rationale**: Consistent indentation improves readability

### Comments
* **Function comments**: Required for all public functions
* **Complex algorithms**: Required for non obvious algorithms
* **Invariants**: Document data structure invariants
* **Complexity**: Document time and space complexity
* **Rationale**: Comments clarify intent and complexity

## Error Handling

### Input Validation
* **All public APIs**: Must validate inputs
* **Null pointers**: Check and return error for NULL inputs
* **Bounds checking**: Validate array bounds and indices
* **Preconditions**: Validate preconditions (e.g., non empty for pop operations)
* **Rationale**: Prevents undefined behavior and crashes

### Error Reporting
* **Return codes**: Use consistent return code conventions
* **Error codes**: Define error codes in header files
* **Exceptions**: Use exceptions for C++ (optional, prefer return codes)
* **Status propagation**: Propagate errors correctly through call stack
* **Rationale**: Clear error reporting aids debugging and integration

### Error Recovery
* **Graceful degradation**: Handle errors without crashing
* **Resource cleanup**: Clean up resources on error paths
* **Partial success**: Handle partial operation success correctly
* **Invariant maintenance**: Maintain data structure invariants on errors
* **Rationale**: Robust error handling improves reliability

## Memory Safety

### Allocation
* **Stack allocation**: Prefer stack allocation for small, fixed size structures
* **Heap allocation**: Use heap allocation for dynamic structures
* **RAII**: Use RAII for C++ (smart pointers, containers)
* **Buffer sizes**: Use constants for buffer sizes, avoid magic numbers
* **Rationale**: Proper allocation prevents leaks and improves performance

### Bounds Checking
* **Array access**: Always validate array indices
* **Buffer writes**: Check buffer capacity before writes
* **Iterator bounds**: Validate iterator bounds
* **Rationale**: Bounds checks prevent buffer overflows and undefined behavior

### Leak Prevention
* **Allocation pairs**: Every allocation must have corresponding deallocation
* **Error paths**: Free resources in all error paths
* **Destructors**: Implement proper destructors for C++
* **Smart pointers**: Use smart pointers in C++ to prevent leaks
* **Rationale**: Memory leaks cause resource exhaustion

## Data Structure Invariants

### Invariant Documentation
* **Document invariants**: Document all data structure invariants
* **Maintain invariants**: Ensure invariants are maintained in all operations
* **Validate invariants**: Provide debug mode invariant validation
* **Rationale**: Invariants ensure correctness

### Example Invariants
* **Binary Search Tree**: Left child < parent < right child
* **Heap**: Parent >= children (max heap) or parent <= children (min heap)
* **Linked List**: Next pointers form valid chain, prev pointers match (doubly linked)
* **Rationale**: Invariants define correctness

## Performance

### Complexity Analysis
* **Time complexity**: Document time complexity for all operations
* **Space complexity**: Document space complexity
* **Amortized complexity**: Document amortized complexity where applicable
* **Rationale**: Complexity analysis enables performance evaluation

### Optimization
* **Hot paths**: Optimize frequently executed code paths
* **Common cases**: Fast path for common operations
* **Cache efficiency**: Design for cache friendly memory layout
* **SIMD**: Use SIMD optimizations where applicable
* **Rationale**: Performance is critical for data structures

### Benchmarking
* **Benchmarks**: Include benchmarks for performance critical operations
* **Profiling**: Profile code to identify bottlenecks
* **Metrics**: Measure time, memory, cache misses
* **Rationale**: Data driven optimization decisions

## Testing

### Unit Tests
* **Coverage**: Aim for 90%+ code coverage
* **Operations**: Test all data structure operations
* **Edge cases**: Test boundary conditions and error cases
* **Invariants**: Test that invariants are maintained
* **Rationale**: Comprehensive testing ensures correctness

### Property Based Tests
* **Invariants**: Test invariants hold for random operations
* **Fuzzing**: Fuzz operations with random inputs
* **Stress tests**: Test with large datasets
* **Rationale**: Property based tests find edge cases

### Performance Tests
* **Benchmarks**: Benchmark performance critical operations
* **Scalability**: Test performance with different sizes
* **Comparison**: Compare with standard library implementations
* **Rationale**: Performance tests ensure performance goals

## Documentation

### API Documentation
* **Public functions**: Document all public functions
* **Parameters**: Document all parameters and return values
* **Complexity**: Document time and space complexity
* **Thread safety**: Document thread safety guarantees
* **Ownership**: Document memory ownership semantics
* **Invariants**: Document data structure invariants
* **Rationale**: Clear documentation enables correct usage

### Implementation Documentation
* **Algorithms**: Document complex algorithms
* **Design decisions**: Document non obvious design choices
* **Trade offs**: Document design trade offs
* **Rationale**: Implementation docs aid maintenance

## Examples

### Good Function (Within Limits)
```cpp
// Thread safety: Not thread safe (caller must synchronize)
// Ownership: Caller owns array, returns ownership of new array
// Invariants: array must not be NULL, size > 0, capacity >= size
// Complexity: O(n) time, O(n) space
// Failure modes: Returns NULL on allocation failure
int* resize_array(int* array, size_t size, size_t new_capacity) {
    if (!array || size == 0 || new_capacity < size) {
        return nullptr;
    }
    
    int* new_array = new int[new_capacity];
    if (!new_array) {
        return nullptr;
    }
    
    std::copy(array, array + size, new_array);
    delete[] array;
    
    return new_array;
}
```

### Bad Function (Exceeds Limits)
```cpp
// BAD: Function exceeds 50 lines and has high complexity
int* resize_array(int* array, size_t size, size_t new_capacity) {
    // 60+ lines of complex logic with nested conditionals
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

