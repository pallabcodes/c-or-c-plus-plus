# Advanced Recursion Standards

## Overview
Advanced recursion techniques include indirect recursion, nested recursion, and complex recursive patterns. This document defines standards for implementing production grade advanced recursive algorithms.

## Indirect Recursion

### Definition
* **Indirect recursion**: Functions calling each other recursively
* **Pattern**: Function A calls function B, which calls function A
* **Use cases**: Complex control flow
* **Rationale**: Indirect recursion enables flexible patterns

### Example Indirect Recursion
```c
void funB(int n);  // Forward declaration

void funA(int n) {
    if (n > 0) {
        printf("%d ", n);
        funB(n - 1);  // Calls funB
    }
}

void funB(int n) {
    if (n > 1) {
        printf("%d ", n);
        funA(n / 2);  // Calls funA (indirect recursion)
    }
}
```

### Implementation Considerations
* **Forward declarations**: May need forward declarations
* **Mutual dependency**: Functions depend on each other
* **Documentation**: Document mutual recursion clearly
* **Rationale**: Considerations ensure correctness

## Nested Recursion

### Definition
* **Nested recursion**: Recursive call with recursive parameter
* **Pattern**: Function calls itself with recursive result
* **Use cases**: Complex mathematical functions
* **Rationale**: Nested recursion enables advanced algorithms

### Example Nested Recursion
```c
// Ackermann function (nested recursion)
int ackermann(int m, int n) {
    if (m == 0) {
        return n + 1;  // Base case
    }
    if (n == 0) {
        return ackermann(m - 1, 1);  // Recursive call
    }
    return ackermann(m - 1, ackermann(m, n - 1));  // Nested recursion
}
```

### Implementation Considerations
* **Complexity**: Can be very complex
* **Stack depth**: Deep stack depth possible
* **Documentation**: Document nested recursion clearly
* **Rationale**: Considerations ensure correctness

## Static and Global Variables

### Static Variables
* **Definition**: Variables that persist across calls
* **Use cases**: Accumulating values, counting calls
* **Thread safety**: Not thread safe
* **Rationale**: Static variables enable state persistence

### Example Static Variables
```c
int count_calls(int n) {
    static int count = 0;  // Static variable
    count++;
    if (n > 0) {
        return count_calls(n - 1);
    }
    return count;
}
```

### Global Variables
* **Definition**: Variables accessible from all functions
* **Use cases**: Shared state, configuration
* **Thread safety**: Not thread safe
* **Rationale**: Global variables enable shared state

## Head Recursion

### Definition
* **Head recursion**: Recursive call before other operations
* **Pattern**: Process after recursion
* **Use cases**: Reverse order processing
* **Rationale**: Head recursion enables post processing

### Example Head Recursion
```c
// Head recursive: process after recursion
void print_reverse(int n) {
    if (n > 0) {
        print_reverse(n - 1);  // Recursive call first
        printf("%d ", n);       // Process after
    }
}
```

## Implementation Standards

### Correctness
* **Base cases**: Proper base cases
* **Recursive cases**: Correct recursive cases
* **Termination**: Termination guarantee
* **Rationale**: Correctness is critical

### Performance
* **Efficiency**: Consider efficiency implications
* **Stack depth**: Monitor stack depth
* **Optimization**: Optimize when possible
* **Rationale**: Performance is critical

## Testing Requirements

### Unit Tests
* **Indirect recursion tests**: Test indirect recursion
* **Nested recursion tests**: Test nested recursion
* **Static variable tests**: Test static variables
* **Edge cases**: Test boundary conditions
* **Rationale**: Comprehensive testing ensures correctness

## Research Papers and References

### Advanced Recursion
* "Introduction to Algorithms" (CLRS) - Advanced recursion
* "The Art of Computer Programming" (Knuth) - Advanced techniques
* Advanced recursion guides

## Implementation Checklist

- [ ] Understand indirect recursion
- [ ] Learn nested recursion
- [ ] Understand static/global variables
- [ ] Practice advanced recursion
- [ ] Write comprehensive unit tests
- [ ] Document advanced recursion usage

