# Memory Management Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. This memory management implementation must meet enterprise production standards suitable for principal level engineering review and must be comparable to top tier implementations used in production systems at these companies.

## Purpose
This module covers the design and implementation of production grade memory management techniques in C and C plus plus. All code must follow production grade standards suitable for principal level code review and must be production ready for deployment in high performance systems requiring efficient memory allocation, proper resource management, and robust memory safety.

## Scope
* Applies to all C and C plus plus code in memory-management directory
* Extends repository root rules defined in the root `.cursor/rules/` files
* Covers all aspects of memory management from basic allocation to advanced techniques
* Code quality standards align with expectations from top tier companies like Google, Bloomberg, Uber, and Amazon

## Top Tier Product Comparisons

### Google Production Systems
* Abseil memory management utilities
* TCMalloc custom allocator
* Smart pointer implementations
* Memory pool patterns
* Production tested at massive scale

### Bloomberg Terminal Systems
* High performance memory management for financial data
* Custom allocators for low latency systems
* Memory pool implementations
* Zero copy techniques
* Production tested in critical financial systems

### Uber Production Systems
* Efficient memory management for real time systems
* Custom allocators for high throughput
* Memory pool patterns
* RAII patterns
* Production tested at scale

### Amazon Production Systems
* High performance memory management for cloud services
* Custom allocators for distributed systems
* Memory efficient data structures
* Production tested at massive scale

### Standard C++ Library
* Standard allocators
* Smart pointers (unique_ptr, shared_ptr, weak_ptr)
* RAII patterns
* Memory management best practices

## Memory Management Categories

### Stack vs Heap Allocation
* **Stack allocation**: Automatic, fast, limited size
* **Heap allocation**: Manual, flexible, larger size
* **Trade offs**: Performance vs flexibility
* **Rationale**: Understanding allocation strategies

### RAII (Resource Acquisition Is Initialization)
* **Smart pointers**: unique_ptr, shared_ptr, weak_ptr
* **Automatic cleanup**: Destructors handle cleanup
* **Exception safety**: Guaranteed cleanup on exceptions
* **Rationale**: Modern C++ memory management

### Custom Allocators
* **Memory pools**: Pre allocated memory pools
* **Arena allocators**: Region based allocation
* **Stack allocators**: Stack based allocation
* **Rationale**: Performance optimization

### Memory Safety
* **Bounds checking**: Array bounds validation
* **Null pointer checks**: Null pointer validation
* **Use after free prevention**: Lifetime management
* **Rationale**: Memory safety prevents crashes

## Production Standards

### Code Quality
* Functions limited to 50 lines
* Files limited to 200 lines
* Cyclomatic complexity ≤ 10
* Comprehensive error handling
* Input validation on all public APIs
* Memory safety and leak prevention

### Performance
* Efficient allocation strategies
* Minimal allocation overhead
* Cache friendly memory layout
* Zero copy techniques where applicable
* Benchmarking and profiling

### Correctness
* Correct memory management
* No memory leaks
* No use after free
* No double free
* Comprehensive test coverage

### Documentation
* API documentation for all public functions
* Memory ownership semantics
* Lifetime guarantees
* Thread safety guarantees
* Performance characteristics

## Research Papers and References

### Memory Management
* "The C++ Programming Language" (Stroustrup) - Memory management
* "Effective Modern C++" (Meyers) - Smart pointers and RAII
* "Memory Management" - Research papers on memory management

### Custom Allocators
* "Custom Memory Allocation" - Custom allocator design
* "Memory Pools" - Pool allocator patterns
* "Arena Allocation" - Arena allocator patterns

### Open Source References
* Google Abseil memory utilities
* TCMalloc allocator
* Standard C++ Library allocators
* Boost C++ Libraries memory management

## Implementation Goals

### Correctness
* Correct memory allocation and deallocation
* Proper handling of edge cases
* No memory leaks or corruption
* Thread safety where applicable

### Performance
* Efficient allocation strategies
* Minimal overhead
* Cache friendly design
* Zero copy where applicable

### Reliability
* Robust error handling
* Memory leak prevention
* Resource cleanup
* Graceful degradation

### Maintainability
* Clean, readable code
* Comprehensive documentation
* Extensive test coverage
* Clear error messages

