# Struct and Memory Layout Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. This struct implementation must meet enterprise production standards suitable for principal level engineering review and must be comparable to top tier implementations used in production systems at these companies.

## Purpose
This module covers the design and implementation of production grade struct and memory layout techniques in C and C++. All code must follow production grade standards suitable for principal level code review and must demonstrate correct, efficient, and maintainable struct design including memory layout optimization, alignment, padding, bit fields, unions, and advanced patterns.

## Scope
* Applies to all C and C++ code in struct-learning directory
* Extends repository root rules defined in the root `.cursor/rules/` files
* Covers all aspects of struct design from fundamentals to advanced optimization techniques
* Code quality standards align with expectations from top tier companies like Google, Bloomberg, Uber, and Amazon

## Top Tier Product Comparisons

### Google Production Systems
* Cache optimized struct layouts
* Hot cold splitting patterns
* SIMD optimized structures
* Production tested at massive scale
* Efficient memory layouts

### Bloomberg Terminal Systems
* High performance struct layouts for financial systems
* Cache friendly data structures
* Production tested in financial trading systems
* Efficient memory access patterns
* Lock free struct patterns

### Uber Production Systems
* Efficient struct layouts for real time systems
* Cache optimization for microservices
* Production tested at scale
* Performance optimized memory layouts
* Zero copy struct patterns

### Amazon Production Systems
* High performance struct layouts for cloud services
* Memory efficient data structures
* Production tested at massive scale
* Scalable struct patterns
* Performance critical implementations

### Standard Libraries
* C Standard Library struct patterns
* C++ Standard Library struct usage
* Standard memory layout patterns
* Production grade struct practices

## Struct Fundamentals

### Basic Structs
* **Declaration**: Struct syntax and declaration
* **Members**: Member variables and types
* **Initialization**: Struct initialization patterns
* **Rationale**: Basic structs enable data organization

### Memory Layout
* **Layout**: Understanding struct memory layout
* **Size**: Calculating struct size
* **Ordering**: Member ordering affects layout
* **Rationale**: Memory layout affects performance

### Alignment and Padding
* **Alignment**: Data alignment requirements
* **Padding**: Compiler inserted padding
* **Optimization**: Minimizing padding
* **Rationale**: Alignment and padding affect memory usage

### Bit Fields
* **Definition**: Compact bit level representation
* **Use cases**: Flags, compact data
* **Rationale**: Bit fields enable memory efficiency

### Nested Structs
* **Composition**: Structs containing other structs
* **Use cases**: Complex data organization
* **Rationale**: Nested structs enable hierarchy

## Advanced Techniques

### Unions
* **Definition**: Shared memory for different types
* **Use cases**: Type punning, memory efficiency
* **Safety**: Type safety considerations
* **Rationale**: Unions enable memory efficiency

### Anonymous Structs
* **Definition**: Unnamed struct members
* **Use cases**: C++11 features, flexible design
* **Rationale**: Anonymous structs enable flexibility

### Struct Templates
* **Definition**: Generic struct definitions
* **Use cases**: Type generic structures
* **Rationale**: Templates enable code reuse

### RAII with Structs
* **Definition**: Resource management in structs
* **Use cases**: Automatic resource cleanup
* **Rationale**: RAII prevents resource leaks

### Move Semantics
* **Definition**: Efficient struct movement
* **Use cases**: Performance optimization
* **Rationale**: Move semantics improve performance

## Enterprise Patterns

### Google Style
* **Patterns**: Google specific struct patterns
* **Use cases**: Search engine structures
* **Rationale**: Google patterns enable scale

### Uber Style
* **Patterns**: Uber specific struct patterns
* **Use cases**: Real time system structures
* **Rationale**: Uber patterns enable real time performance

### Bloomberg Style
* **Patterns**: Bloomberg specific struct patterns
* **Use cases**: Financial data structures
* **Rationale**: Bloomberg patterns enable financial systems

### Amazon Style
* **Patterns**: Amazon specific struct patterns
* **Use cases**: Cloud service structures
* **Rationale**: Amazon patterns enable cloud scale

## Performance Engineering

### Cache Optimization
* **Hot cold splitting**: Separate hot and cold data
* **AoS vs SoA**: Array of structs vs struct of arrays
* **Cache line alignment**: Align to cache lines
* **Rationale**: Cache optimization improves performance

### SIMD Structs
* **Vectorization**: SIMD optimized structures
* **Alignment**: SIMD alignment requirements
* **Use cases**: Parallel processing
* **Rationale**: SIMD enables parallel performance

### Lock Free Structs
* **Definition**: Concurrent struct patterns
* **Use cases**: High performance concurrency
* **Rationale**: Lock free patterns enable scalability

### Memory Pool Structs
* **Definition**: Custom allocator patterns
* **Use cases**: Efficient memory management
* **Rationale**: Memory pools improve allocation performance

### Zero Copy Structs
* **Definition**: High performance data transfer
* **Use cases**: Network, I/O operations
* **Rationale**: Zero copy improves I/O performance

## System Programming

### Kernel Structs
* **Patterns**: Operating system data structures
* **Use cases**: Kernel development
* **Rationale**: Kernel structs enable OS development

### Network Structs
* **Patterns**: Protocol and packet structures
* **Use cases**: Network programming
* **Rationale**: Network structs enable protocol implementation

### File System Structs
* **Patterns**: Storage and I/O structures
* **Use cases**: File system development
* **Rationale**: File system structs enable storage systems

### Hardware Structs
* **Patterns**: Device driver patterns
* **Use cases**: Hardware interfacing
* **Rationale**: Hardware structs enable device drivers

### Embedded Structs
* **Patterns**: Microcontroller optimization
* **Use cases**: Embedded systems
* **Rationale**: Embedded structs enable resource constrained systems

## Advanced Techniques

### Metaprogramming
* **Definition**: Compile time struct generation
* **Use cases**: Code generation, optimization
* **Rationale**: Metaprogramming enables compile time optimization

### Reflection
* **Definition**: Runtime struct introspection
* **Use cases**: Serialization, debugging
* **Rationale**: Reflection enables runtime flexibility

### Serialization
* **Definition**: Binary and text serialization
* **Use cases**: Data persistence, network transfer
* **Rationale**: Serialization enables data exchange

### Validation
* **Definition**: Type safe struct validation
* **Use cases**: Input validation, data integrity
* **Rationale**: Validation ensures correctness

### Code Generation
* **Definition**: Automatic struct code generation
* **Use cases**: Boilerplate reduction, optimization
* **Rationale**: Code generation reduces errors

## Production Standards

### Code Quality
* Functions limited to 50 lines
* Files limited to 200 lines
* Cyclomatic complexity ≤ 10
* Comprehensive error handling
* Input validation on all public APIs
* Memory safety and leak prevention

### Performance
* Efficient memory layouts
* Cache friendly design
* Minimize padding
* Optimize alignment
* Benchmark critical paths

### Correctness
* Proper alignment
* Correct padding
* Safe type punning
* Proper memory management
* Comprehensive test coverage

### Documentation
* API documentation for all public structs
* Memory layout documentation
* Alignment requirements
* Performance characteristics
* Thread safety guarantees

## Research Papers and References

### Memory Layout
* "Cache Conscious Data Structures" research papers
* "Memory Alignment" research
* "Data Structure Layout" research papers

### Performance Optimization
* "Hot Cold Splitting" research papers
* "SIMD Optimization" research
* "Cache Optimization" research papers

### Open Source References
* Linux kernel struct patterns
* Google Abseil struct patterns
* Standard library struct usage

## Implementation Goals

### Correctness
* Correct memory layouts
* Proper alignment
* Safe type operations
* Proper memory management
* Comprehensive testing

### Performance
* Efficient memory layouts
* Cache friendly design
* Minimize padding
* Optimize alignment
* Benchmark and optimize

### Maintainability
* Clean, readable code
* Comprehensive documentation
* Extensive test coverage
* Clear memory layout
* Well documented trade offs
