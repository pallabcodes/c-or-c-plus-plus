# Standard Template Library (STL) Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. This STL implementation and usage must meet enterprise production standards suitable for principal level engineering review and must be comparable to top tier implementations used in production systems at these companies.

## Purpose
This module covers the design, implementation, and production grade usage of the C++ Standard Template Library (STL). All code must follow production grade standards suitable for principal level code review and must demonstrate correct, efficient, and safe usage of STL containers, iterators, algorithms, and utilities.

## Scope
* Applies to all C++ code in stl directory
* Extends repository root rules defined in the root `.cursor/rules/` files
* Covers all aspects of STL from containers to algorithms
* Code quality standards align with expectations from top tier companies like Google, Bloomberg, Uber, and Amazon

## Top Tier Product Comparisons

### Google Production Systems
* Abseil containers (swisstable, btree)
* High performance STL usage patterns
* Production tested at massive scale
* Efficient container selection
* Performance optimized algorithms

### Bloomberg Terminal Systems
* High performance STL for financial data
* Low latency container operations
* Production tested in critical financial systems
* Efficient memory usage patterns
* Optimized algorithm selection

### Uber Production Systems
* Efficient STL usage for real time systems
* High throughput container operations
* Production tested at scale
* Performance optimized patterns
* Memory efficient implementations

### Amazon Production Systems
* High performance STL for cloud services
* Efficient container and algorithm usage
* Production tested at massive scale
* Scalable STL patterns
* Performance critical implementations

### Standard Library
* C++ Standard Library (std namespace)
* STL containers, iterators, algorithms
* Standard implementations (libstdc++, libc++)
* Production grade standard library usage

## STL Components

### Containers
* **Sequence Containers**: vector, deque, list, forward_list, array
* **Associative Containers**: set, multiset, map, multimap
* **Unordered Containers**: unordered_set, unordered_multiset, unordered_map, unordered_multimap
* **Container Adapters**: stack, queue, priority_queue

### Iterators
* **Iterator Categories**: Input, Output, Forward, Bidirectional, Random Access
* **Iterator Operations**: Dereference, increment, decrement, arithmetic
* **Iterator Traits**: iterator_traits, iterator_category
* **Custom Iterators**: Implementing custom iterators

### Algorithms
* **Non Modifying**: find, count, for_each, search, mismatch
* **Modifying**: transform, copy, move, fill, generate
* **Sorting**: sort, stable_sort, partial_sort, nth_element
* **Binary Search**: lower_bound, upper_bound, equal_range, binary_search
* **Set Operations**: set_union, set_intersection, set_difference
* **Heap Operations**: make_heap, push_heap, pop_heap, sort_heap
* **Min/Max**: min, max, minmax, min_element, max_element
* **Numeric**: accumulate, inner_product, partial_sum, adjacent_difference

### Function Objects
* **Functors**: Function objects and callable types
* **Lambda Expressions**: Lambda functions and closures
* **Bind**: std::bind and placeholders
* **Function**: std::function wrapper

### Allocators
* **Default Allocator**: std::allocator
* **Custom Allocators**: Implementing custom allocators
* **Allocator Traits**: allocator_traits
* **Memory Management**: Container memory management

## Production Standards

### Code Quality
* Functions limited to 50 lines
* Files limited to 200 lines
* Cyclomatic complexity ≤ 10
* Comprehensive error handling
* Input validation on all public APIs
* Memory safety and leak prevention

### Performance
* Efficient container selection
* Appropriate algorithm choice
* Minimize unnecessary copies
* Reserve capacity when known
* Use move semantics
* Benchmark critical paths

### Correctness
* Correct iterator usage
* No iterator invalidation
* Proper exception safety
* Correct comparison functions
* Comprehensive test coverage

### Documentation
* API documentation for all public functions
* Complexity guarantees
* Iterator validity guarantees
* Exception safety guarantees
* Performance characteristics

## Research Papers and References

### STL Design
* "The C++ Standard Library" (Josuttis) - Comprehensive STL reference
* "Effective STL" (Meyers) - Best practices and pitfalls
* "STL Source Code" - Standard library implementations

### Algorithms
* "Introduction to Algorithms" (CLRS) - Algorithm analysis
* "The Algorithm Design Manual" (Skiena) - Algorithm selection
* STL algorithm complexity guarantees

### Open Source References
* libstdc++ (GCC standard library)
* libc++ (LLVM standard library)
* Abseil containers (Google)
* Boost libraries

## Implementation Goals

### Correctness
* Correct container usage
* Proper iterator handling
* Exception safety
* No undefined behavior
* Comprehensive testing

### Performance
* Efficient container operations
* Appropriate algorithm selection
* Minimize allocations
* Cache friendly access patterns
* Benchmark and optimize

### Maintainability
* Clean, readable code
* Comprehensive documentation
* Extensive test coverage
* Clear usage patterns
* Well documented trade offs

