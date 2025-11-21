# Code Quality Standards for OOP

## Overview
This document defines production grade code quality standards for object oriented programming implementations. These standards ensure code is suitable for principal level review and production deployment in high performance systems.

## Function Size Limits
* **Maximum function length**: 50 lines (excluding comments and blank lines)
* **Rationale**: Functions exceeding 50 lines become difficult to understand, test, and maintain
* **Enforcement**: All functions must be reviewed for size compliance
* **Exception**: Complex virtual function implementations may extend to 60 lines with justification

## File Size Limits
* **Maximum file length**: 200 lines (excluding comments and blank lines)
* **Rationale**: Files exceeding 200 lines become difficult to navigate and understand
* **Enforcement**: Split large files into logical modules
* **Exception**: Header files with extensive class definitions may extend to 250 lines

## Cyclomatic Complexity
* **Maximum complexity**: 10 per function
* **Rationale**: High complexity indicates functions doing too much
* **Measurement**: Count decision points (if, while, for, switch, &&, ||, ?:)
* **Enforcement**: Refactor complex functions into smaller, focused functions
* **Exception**: Complex virtual function implementations may have complexity up to 15 with justification

## Code Style

### Naming Conventions
* **Classes**: `PascalCase` (e.g., `Vehicle`, `Car`, `RentalAgency`)
* **Functions**: `snake_case` (e.g., `calculate_rental_cost`, `display_info`)
* **Member variables**: `snake_case` with trailing underscore (e.g., `make_`, `model_`, `year_`)
* **Constants**: `UPPER_SNAKE_CASE` (e.g., `MAX_SIZE`, `DEFAULT_CAPACITY`)
* **Rationale**: Consistent naming improves readability and maintainability

### Indentation
* **Indentation**: 4 spaces (no tabs)
* **Continuation**: Align continuation lines with opening delimiter
* **Rationale**: Consistent indentation improves readability

### Comments
* **Function comments**: Required for all public functions
* **Virtual functions**: Document virtual function contracts
* **Inheritance**: Document inheritance relationships
* **Design patterns**: Document pattern usage
* **Rationale**: Comments clarify OOP design and usage

## Error Handling

### Input Validation
* **All public APIs**: Must validate inputs
* **Null pointers**: Check and return error for NULL inputs
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

## OOP Principles

### Encapsulation
* **Access control**: Use private, protected, public appropriately
* **Data hiding**: Hide implementation details
* **Interfaces**: Define clear public interfaces
* **Rationale**: Encapsulation enables maintainability

### Inheritance
* **Single inheritance**: Prefer single inheritance
* **Virtual inheritance**: Use virtual inheritance for diamond problem
* **Abstract classes**: Use abstract classes for interfaces
* **Rationale**: Inheritance enables code reuse

### Polymorphism
* **Virtual functions**: Use virtual functions for runtime polymorphism
* **Function overloading**: Use overloading for compile time polymorphism
* **Templates**: Use templates for generic programming
* **Rationale**: Polymorphism enables flexibility

## Memory Safety

### RAII
* **Resource management**: Use RAII for resource management
* **Smart pointers**: Use smart pointers (unique_ptr, shared_ptr)
* **Destructors**: Implement proper destructors
* **Rationale**: RAII prevents resource leaks

### Ownership
* **Clear ownership**: Document ownership semantics
* **Smart pointers**: Use smart pointers for ownership
* **Raw pointers**: Use raw pointers only for non owning references
* **Rationale**: Clear ownership prevents memory issues

## Performance

### Virtual Function Overhead
* **Minimize virtual calls**: Avoid unnecessary virtual calls
* **Use final**: Use final when inheritance is not needed
* **Inline non virtual**: Prefer inline non virtual functions
* **Rationale**: Virtual calls have overhead

### Object Layout
* **Cache efficiency**: Design for cache friendly object layout
* **Virtual table**: Understand vtable overhead
* **Rationale**: Object layout affects performance

## Testing

### Unit Tests
* **Coverage**: Aim for 90%+ code coverage
* **Inheritance**: Test inheritance hierarchies
* **Polymorphism**: Test polymorphic behavior
* **Design patterns**: Test pattern implementations
* **Rationale**: Comprehensive testing ensures correctness

## Documentation

### API Documentation
* **Public functions**: Document all public functions
* **Virtual functions**: Document virtual function contracts
* **Inheritance**: Document inheritance relationships
* **Design patterns**: Document pattern usage
* **Parameters**: Document all parameters and return values
* **Examples**: Provide usage examples
* **Rationale**: Clear documentation enables correct usage

## Examples

### Good Class (Within Limits)
```cpp
// Thread safety: Not thread safe (mutable state)
// Ownership: Owns no resources
// Invariants: make and model must be non empty, year > 0
// Failure modes: Throws if year <= 0
class Vehicle {
protected:
    std::string make_;
    std::string model_;
    int year_;

public:
    Vehicle(const std::string& make, const std::string& model, int year)
        : make_(make), model_(model), year_(year) {
        if (year <= 0) {
            throw std::invalid_argument("Year must be positive");
        }
    }

    virtual void displayInfo() const = 0;
    virtual double calculateRentalCost() const = 0;
    virtual ~Vehicle() = default;
};
```

### Bad Class (Exceeds Limits)
```cpp
// BAD: Class exceeds 200 lines and has high complexity
class ComplexClass {
    // 250+ lines of complex logic
    // High cyclomatic complexity (> 10 per function)
    // Difficult to test and maintain
};
```

## Enforcement

### Code Review
* **Mandatory**: All code must be reviewed for compliance
* **Checklist**: Use checklist to verify standards
* **OOP review**: Special attention to OOP design
* **Automation**: Use tools to check function/file sizes and complexity

### CI/CD
* **Static analysis**: Run static analysis tools in CI
* **Linting**: Run linters to check style compliance
* **Testing**: Run tests to verify correctness
* **Metrics**: Track code quality metrics over time
