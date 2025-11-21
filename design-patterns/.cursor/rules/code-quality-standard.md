# Code Quality Standards for Design Patterns

## Overview
This document defines production grade code quality standards for design pattern implementations. These standards ensure code is suitable for principal level review and production deployment in high performance systems.

## Function Size Limits
* **Maximum function length**: 50 lines (excluding comments and blank lines)
* **Rationale**: Functions exceeding 50 lines become difficult to understand, test, and maintain
* **Enforcement**: All functions must be reviewed for size compliance
* **Exception**: Complex pattern implementations may extend to 60 lines with justification

## File Size Limits
* **Maximum file length**: 200 lines (excluding comments and blank lines)
* **Rationale**: Files exceeding 200 lines become difficult to navigate and understand
* **Enforcement**: Split large files into logical modules
* **Exception**: Header files with extensive pattern definitions may extend to 250 lines

## Cyclomatic Complexity
* **Maximum complexity**: 10 per function
* **Rationale**: High complexity indicates functions doing too much
* **Measurement**: Count decision points (if, while, for, switch, &&, ||, ?:)
* **Enforcement**: Refactor complex functions into smaller, focused functions
* **Exception**: Complex pattern implementations may have complexity up to 15 with justification

## Code Style

### Naming Conventions
* **Classes**: `PascalCase` (e.g., `Singleton`, `Factory`, `Observer`)
* **Functions**: `snake_case` (e.g., `create_instance`, `notify_observers`)
* **Pattern suffixes**: Use pattern names in class names (e.g., `ConcreteFactory`, `ObserverPattern`)
* **Rationale**: Consistent naming improves readability and maintainability

### Indentation
* **Indentation**: 4 spaces (no tabs)
* **Continuation**: Align continuation lines with opening delimiter
* **Rationale**: Consistent indentation improves readability

### Comments
* **Pattern comments**: Required for all pattern implementations
* **SOLID principles**: Document SOLID principles application
* **Trade offs**: Document design trade offs
* **Rationale**: Comments clarify pattern usage and design decisions

## Error Handling

### Input Validation
* **All public APIs**: Must validate inputs
* **Null pointers**: Check and return error for NULL/nullptr inputs
* **Invalid parameters**: Validate parameter ranges
* **Preconditions**: Document and validate preconditions
* **Rationale**: Prevents crashes and undefined behavior

### Error Reporting
* **Return codes**: Use consistent return code conventions
* **Exceptions**: Use exceptions for C++ (prefer exceptions over return codes)
* **Error codes**: Define error codes in header files
* **Rationale**: Clear error reporting aids debugging

### Exception Safety
* **Basic guarantee**: Maintain valid state on exceptions
* **Strong guarantee**: All or nothing operations
* **No throw guarantee**: Operations that never throw
* **Rationale**: Exception safety ensures robustness

## Pattern Implementation

### Correctness
* **Pattern compliance**: Follow pattern structure correctly
* **SOLID principles**: Apply SOLID principles
* **Rationale**: Correctness ensures pattern benefits

### Performance
* **Pattern overhead**: Minimize pattern overhead
* **Virtual calls**: Understand virtual call overhead
* **Memory usage**: Optimize memory usage
* **Rationale**: Performance is critical

## Testing

### Unit Tests
* **Coverage**: Aim for 90%+ code coverage
* **Pattern tests**: Test pattern behavior
* **SOLID tests**: Test SOLID principles compliance
* **Rationale**: Comprehensive testing ensures correctness

## Documentation

### API Documentation
* **Public functions**: Document all public functions
* **Pattern usage**: Document pattern usage
* **SOLID principles**: Document SOLID principles application
* **Examples**: Provide usage examples
* **Rationale**: Clear documentation enables correct usage

## Examples

### Good Pattern Implementation (Within Limits)
```cpp
// Thread safety: Thread safe (mutex protected)
// Ownership: Owns instance (singleton pattern)
// Invariants: Only one instance exists
// Failure modes: Throws if initialization fails
class Singleton {
private:
    static std::unique_ptr<Singleton> instance_;
    static std::mutex mutex_;
    
    Singleton() = default;
    
public:
    static Singleton& getInstance() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
            instance_ = std::unique_ptr<Singleton>(new Singleton());
        }
        return *instance_;
    }
    
    // Delete copy constructor and assignment
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};
```

### Bad Pattern Implementation (Exceeds Limits)
```cpp
// BAD: No thread safety, exceeds 50 lines, high complexity
class Singleton {
    // 60+ lines of complex logic
    // No thread safety
    // High cyclomatic complexity (> 10)
    // Difficult to test and maintain
};
```

## Enforcement

### Code Review
* **Mandatory**: All code must be reviewed for compliance
* **Checklist**: Use checklist to verify standards
* **Pattern review**: Special attention to pattern correctness and SOLID principles
* **Automation**: Use tools to check function/file sizes and complexity

### CI/CD
* **Static analysis**: Run static analysis tools in CI
* **Linting**: Run linters to check style compliance
* **Testing**: Run tests to verify correctness
* **Metrics**: Track code quality metrics over time

