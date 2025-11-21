# Code Quality Standards for Recursion

## Overview
This document defines production grade code quality standards for recursive algorithm implementations. These standards ensure code is suitable for principal level review and production deployment in high performance systems.

## Function Size Limits
* **Maximum function length**: 50 lines (excluding comments and blank lines)
* **Rationale**: Functions exceeding 50 lines become difficult to understand, test, and maintain
* **Enforcement**: All functions must be reviewed for size compliance
* **Exception**: Complex recursive functions may extend to 60 lines with justification

## File Size Limits
* **Maximum file length**: 200 lines (excluding comments and blank lines)
* **Rationale**: Files exceeding 200 lines become difficult to navigate and understand
* **Enforcement**: Split large files into logical modules
* **Exception**: Header files with extensive recursive type definitions may extend to 250 lines

## Cyclomatic Complexity
* **Maximum complexity**: 10 per function
* **Rationale**: High complexity indicates functions doing too much
* **Measurement**: Count decision points (if, while, for, switch, &&, ||, ?:)
* **Enforcement**: Refactor complex functions into smaller, focused functions
* **Exception**: Complex recursive functions may have complexity up to 15 with justification

## Code Style

### Naming Conventions
* **Recursive functions**: Descriptive names indicating recursion (e.g., `fibonacci_recursive`, `tree_traverse`)
* **Helper functions**: `_helper` suffix for recursive helpers (e.g., `merge_sort_helper`)
* **Memoization**: `_memo` suffix for memoized versions (e.g., `fibonacci_memo`)
* **Rationale**: Consistent naming improves readability and maintainability

### Indentation
* **Indentation**: 4 spaces (no tabs)
* **Continuation**: Align continuation lines with opening delimiter
* **Rationale**: Consistent indentation improves readability

### Comments
* **Recursive comments**: Required for all recursive functions
* **Base case**: Document base case clearly
* **Recursive case**: Document recursive case clearly
* **Termination**: Document termination guarantee
* **Rationale**: Comments clarify recursive logic

## Error Handling

### Input Validation
* **All public APIs**: Must validate inputs
* **Negative values**: Check for negative values where applicable
* **Invalid ranges**: Validate parameter ranges
* **Preconditions**: Document and validate preconditions
* **Rationale**: Prevents crashes and undefined behavior

### Error Reporting
* **Return codes**: Use consistent return code conventions
* **Exceptions**: Use exceptions for C++ (prefer exceptions over return codes)
* **Error codes**: Define error codes in header files
* **Rationale**: Clear error reporting aids debugging

### Stack Overflow Prevention
* **Depth limits**: Set maximum recursion depth
* **Depth checking**: Check recursion depth
* **Iteration conversion**: Convert to iteration when depth exceeds limit
* **Rationale**: Stack overflow prevention ensures reliability

## Recursion Safety

### Base Cases
* **Requirement**: Every recursive function must have base case
* **Clarity**: Base case must be clearly defined
* **Termination**: Base case must guarantee termination
* **Rationale**: Base cases ensure termination

### Termination Guarantees
* **Documentation**: Document termination guarantee
* **Proof**: Provide informal proof of termination
* **Testing**: Test termination with various inputs
* **Rationale**: Termination guarantees ensure correctness

### Stack Management
* **Depth limits**: Set maximum recursion depth
* **Stack frame size**: Minimize stack frame size
* **Memory usage**: Understand stack memory usage
* **Rationale**: Stack management prevents overflow

## Performance

### Tail Recursion
* **Identification**: Identify tail recursive functions
* **Optimization**: Use tail recursion when applicable
* **Conversion**: Convert to iteration if needed
* **Rationale**: Tail recursion enables optimization

### Memoization
* **Identification**: Identify overlapping subproblems
* **Implementation**: Implement memoization when beneficial
* **Trade offs**: Consider memory vs time trade offs
* **Rationale**: Memoization improves performance

### Iteration Conversion
* **When to convert**: Convert when stack overflow is concern
* **How to convert**: Use explicit stack or iteration
* **Trade offs**: Consider readability vs performance
* **Rationale**: Iteration conversion prevents stack overflow

## Testing

### Unit Tests
* **Coverage**: Aim for 90%+ code coverage
* **Base case tests**: Test base cases
* **Recursive case tests**: Test recursive cases
* **Edge case tests**: Test boundary conditions
* **Stack overflow tests**: Test with large inputs
* **Rationale**: Comprehensive testing ensures correctness

## Documentation

### API Documentation
* **Public functions**: Document all public functions
* **Base case**: Document base case
* **Recursive case**: Document recursive case
* **Termination**: Document termination guarantee
* **Stack usage**: Document stack usage
* **Examples**: Provide usage examples
* **Rationale**: Clear documentation enables correct usage

## Examples

### Good Recursive Function (Within Limits)
```c
// Thread safety: Thread safe (pure function, no shared state)
// Ownership: None (value semantics)
// Invariants: n >= 0
// Failure modes: Stack overflow if n is very large
// Termination: Guaranteed (n decreases to 0)
int factorial(int n) {
    if (n < 0) {
        return -1;  // Error: negative input
    }
    if (n == 0 || n == 1) {
        return 1;  // Base case
    }
    return n * factorial(n - 1);  // Recursive case
}
```

### Bad Recursive Function (Exceeds Limits)
```c
// BAD: No base case check, exceeds 50 lines, high complexity
int complex_function(int n) {
    // 60+ lines of complex logic
    // No proper base case
    // High cyclomatic complexity (> 10)
    // Difficult to test and maintain
}
```

## Enforcement

### Code Review
* **Mandatory**: All code must be reviewed for compliance
* **Checklist**: Use checklist to verify standards
* **Recursion review**: Special attention to termination and stack safety
* **Automation**: Use tools to check function/file sizes and complexity

### CI/CD
* **Static analysis**: Run static analysis tools in CI
* **Linting**: Run linters to check style compliance
* **Testing**: Run tests to verify correctness
* **Stack overflow tests**: Run stack overflow tests
* **Metrics**: Track code quality metrics over time

