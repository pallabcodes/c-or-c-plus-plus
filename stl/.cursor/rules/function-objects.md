# Function Objects Standards

## Overview
Function objects (functors) and callable types enable generic algorithms with custom behavior. This document defines standards for production grade function object usage and implementation.

## Function Object Types

### Function Objects (Functors)
* **Class with operator()**: Class implementing call operator
* **State**: Can maintain state
* **Efficiency**: Can be inlined by compiler
* **Rationale**: Functors provide efficient, stateful callables

### Lambda Expressions
* **Anonymous functions**: Inline function definitions
* **Captures**: Capture variables from enclosing scope
* **Closures**: Create closures
* **Rationale**: Lambdas provide convenient inline functions

### Function Pointers
* **Function addresses**: Addresses of functions
* **No state**: Cannot maintain state
* **Compatibility**: Compatible with C
* **Rationale**: Function pointers provide simple callables

### std::function
* **Type erasure**: Wrapper for any callable
* **Polymorphism**: Can hold different callable types
* **Overhead**: Has overhead compared to direct calls
* **Rationale**: std::function provides runtime polymorphism

## Lambda Expressions

### Lambda Syntax
```cpp
[capture](parameters) -> return_type { body }
```

### Capture Modes
* **By value**: [=] or [var] (copy)
* **By reference**: [&] or [&var] (reference)
* **Mixed**: [=, &var] or [&, var] (mixed)
* **Rationale**: Captures enable access to enclosing scope

### Lambda Examples
```cpp
// Simple lambda
auto add = [](int a, int b) { return a + b; };

// Lambda with capture
int multiplier = 2;
auto multiply = [multiplier](int x) { return x * multiplier; };

// Lambda with reference capture
auto increment = [&counter]() { ++counter; };
```

## Function Objects

### Custom Functors
```cpp
struct GreaterThan {
    int threshold;
    GreaterThan(int t) : threshold(t) {}
    bool operator()(int value) const {
        return value > threshold;
    }
};

// Usage
std::vector<int> result;
std::copy_if(input.begin(), input.end(), std::back_inserter(result),
             GreaterThan(10));
```

### Standard Functors
* **Arithmetic**: plus, minus, multiplies, divides, modulus, negate
* **Comparison**: equal_to, not_equal_to, greater, less, greater_equal, less_equal
* **Logical**: logical_and, logical_or, logical_not
* **Rationale**: Standard functors provide common operations

## std::bind

### Bind Syntax
```cpp
std::bind(function, args...)
```

### Placeholders
* **std::placeholders::_1**: First argument
* **std::placeholders::_2**: Second argument
* **Rationale**: Placeholders enable argument reordering

### Bind Examples
```cpp
// Bind arguments
auto bound = std::bind(add, 10, std::placeholders::_1);
int result = bound(5); // Calls add(10, 5)

// Reorder arguments
auto reordered = std::bind(subtract, std::placeholders::_2, std::placeholders::_1);
int result = reordered(5, 10); // Calls subtract(10, 5)
```

## std::function

### Function Wrapper
```cpp
std::function<int(int, int)> func = add;
int result = func(3, 4);

// Can hold different callables
func = [](int a, int b) { return a * b; };
result = func(3, 4);
```

### Use Cases
* **Callbacks**: Store callbacks
* **Polymorphism**: Runtime polymorphism
* **Rationale**: std::function enables flexible callable storage

## Implementation Standards

### Correctness
* **Type matching**: Ensure callable types match
* **Capture safety**: Ensure captures remain valid
* **Exception safety**: Maintain exception safety
* **Rationale**: Correctness is critical

### Performance
* **Prefer lambdas**: Lambdas are often more efficient
* **Avoid std::function**: Avoid std::function when possible (overhead)
* **Inline optimization**: Enable compiler inlining
* **Rationale**: Performance is critical

## Testing Requirements

### Unit Tests
* **Callables**: Test all callable types
* **Captures**: Test lambda captures
* **Bind**: Test std::bind
* **std::function**: Test std::function
* **Rationale**: Comprehensive testing ensures correctness

## Research Papers and References

### Function Objects
* "The C++ Standard Library" (Josuttis) - Function object reference
* "Effective Modern C++" (Meyers) - Lambda best practices
* C++ lambda specification

## Implementation Checklist

- [ ] Understand function object types
- [ ] Learn lambda expressions
- [ ] Understand captures
- [ ] Learn std::bind
- [ ] Learn std::function
- [ ] Practice function object usage
- [ ] Write comprehensive unit tests
- [ ] Document function object usage

