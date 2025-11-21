# Recursion Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. This recursion implementation must meet enterprise production standards suitable for principal level engineering review and must be comparable to top tier implementations used in production systems at these companies.

## Purpose
This module covers the design and implementation of production grade recursive algorithms in C and C++. All code must follow production grade standards suitable for principal level code review and must demonstrate correct, efficient, and maintainable recursive solutions including tail recursion, head recursion, tree recursion, indirect recursion, and memoization.

## Scope
* Applies to all C and C++ code in recursion directory
* Extends repository root rules defined in the root `.cursor/rules/` files
* Covers all aspects of recursion from fundamentals to advanced optimization techniques
* Code quality standards align with expectations from top tier companies like Google, Bloomberg, Uber, and Amazon

## Top Tier Product Comparisons

### Google Production Systems
* Efficient recursive algorithms
* Tail call optimization
* Memoization patterns
* Production tested at massive scale
* Performance optimized recursion

### Bloomberg Terminal Systems
* High performance recursive algorithms for financial systems
* Efficient tree traversal algorithms
* Production tested in financial trading systems
* Optimized recursive patterns
* Stack overflow prevention

### Uber Production Systems
* Efficient recursive algorithms for real time systems
* Graph traversal algorithms
* Production tested at scale
* Performance optimized recursion
* Memory efficient patterns

### Amazon Production Systems
* High performance recursive algorithms for cloud services
* Tree and graph algorithms
* Production tested at massive scale
* Scalable recursive patterns
* Performance critical implementations

### Standard Libraries
* C Standard Library recursive patterns
* C++ Standard Library recursive algorithms
* Standard recursive implementations
* Production grade recursive practices

## Recursion Types

### Tail Recursion
* **Definition**: Recursive call is last operation
* **Optimization**: Can be optimized to iteration
* **Use cases**: Iterative algorithms, accumulators
* **Rationale**: Tail recursion enables optimization

### Head Recursion
* **Definition**: Recursive call before other operations
* **Use cases**: Processing after recursion, reverse order
* **Rationale**: Head recursion enables post processing

### Tree Recursion
* **Definition**: Multiple recursive calls per function
* **Use cases**: Tree traversal, divide and conquer
* **Optimization**: Memoization often needed
* **Rationale**: Tree recursion enables complex algorithms

### Indirect Recursion
* **Definition**: Functions calling each other recursively
* **Use cases**: Complex control flow
* **Rationale**: Indirect recursion enables flexible patterns

### Nested Recursion
* **Definition**: Recursive call with recursive parameter
* **Use cases**: Complex mathematical functions
* **Rationale**: Nested recursion enables advanced algorithms

## Optimization Techniques

### Tail Call Optimization
* **Definition**: Compiler optimization for tail recursion
* **Benefits**: Eliminates stack growth
* **Limitations**: Not always applicable
* **Rationale**: Tail call optimization improves performance

### Memoization
* **Definition**: Caching recursive results
* **Benefits**: Reduces redundant computations
* **Use cases**: Overlapping subproblems
* **Rationale**: Memoization improves performance

### Iteration Conversion
* **Definition**: Converting recursion to iteration
* **Benefits**: Eliminates stack overhead
* **Use cases**: When stack overflow is concern
* **Rationale**: Iteration conversion prevents stack overflow

## Stack Management

### Stack Overflow Prevention
* **Depth limits**: Set maximum recursion depth
* **Iteration conversion**: Convert to iteration when needed
* **Rationale**: Stack overflow prevention ensures reliability

### Stack Frame Analysis
* **Memory usage**: Understand stack frame size
* **Optimization**: Minimize stack frame size
* **Rationale**: Stack frame analysis enables optimization

## Production Standards

### Code Quality
* Functions limited to 50 lines
* Files limited to 200 lines
* Cyclomatic complexity ≤ 10
* Comprehensive error handling
* Input validation on all public APIs
* Stack overflow prevention

### Performance
* Efficient recursive algorithms
* Tail call optimization when applicable
* Memoization for overlapping subproblems
* Iteration conversion when needed
* Benchmark critical paths

### Correctness
* Proper base cases
* Correct recursive cases
* Termination guarantees
* Stack overflow prevention
* Comprehensive test coverage

### Documentation
* API documentation for all public functions
* Recursion depth limits
* Stack usage guarantees
* Performance characteristics
* Termination guarantees

## Research Papers and References

### Recursion Theory
* "Introduction to Algorithms" (CLRS) - Recursive algorithms
* "The Art of Computer Programming" (Knuth) - Recursive techniques
* Recursion research papers

### Optimization
* "Tail Call Optimization" research papers
* "Memoization Techniques" research
* "Stack Management" research papers

### Open Source References
* Google C++ Style Guide
* Standard C++ Library recursive algorithms
* Algorithm libraries (Boost, Abseil)

## Implementation Goals

### Correctness
* Correct recursive algorithms
* Proper base cases
* Termination guarantees
* Stack overflow prevention
* Comprehensive testing

### Performance
* Efficient recursive algorithms
* Tail call optimization
* Memoization when applicable
* Benchmark and optimize
* Profile critical paths

### Maintainability
* Clean, readable code
* Comprehensive documentation
* Extensive test coverage
* Clear termination conditions
* Well documented trade offs

