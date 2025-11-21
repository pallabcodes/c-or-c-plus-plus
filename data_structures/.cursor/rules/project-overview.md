# Data Structures Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. This data structure implementation must meet enterprise production standards suitable for principal level engineering review and must be comparable to top tier implementations used in production systems at these companies.

## Purpose
This module covers the design and implementation of production grade data structures in C and C plus plus. All code must follow production grade standards suitable for principal level code review and must be production ready for deployment in high performance systems requiring efficient data organization, memory management, and algorithmic operations.

## Scope
* Applies to all C and C plus plus code in data_structures directory
* Extends repository root rules defined in the root `.cursor/rules/` files
* Covers all aspects of data structures from linear to non linear structures
* Code quality standards align with expectations from top tier companies like Google, Bloomberg, Uber, and Amazon

## Top Tier Product Comparisons

### Google Production Systems
* Abseil library data structures (flat_hash_map, btree_map)
* High performance hash tables and B trees
* Memory efficient containers for distributed systems
* Cache aware data structure design
* Production tested implementations

### Bloomberg Terminal Systems
* High performance data structures for market data
* Real time data structures for financial data streams
* Memory efficient structures for high frequency trading
* Lock free data structures for concurrent access
* Production tested in critical financial systems

### Uber Geospatial Systems
* Spatial data structures (R trees, quadtrees)
* Efficient geographic data organization
* High performance location indexing
* Memory efficient spatial queries
* Production tested at scale

### Amazon Production Systems
* High performance hash tables for distributed systems
* B trees for database systems
* Memory efficient data structures for cloud services
* Production tested at massive scale
* Cache optimized structures

### Redis Data Structures
* High performance hash tables
* Efficient set and sorted set implementations
* Memory efficient data structures
* Production tested implementations
* Lock free concurrent structures

### Standard Template Library (STL)
* C++ standard library containers
* Industry standard implementations
* Performance characteristics and guarantees
* Memory and time complexity specifications

## Data Structure Categories

### Linear Ordered Structures
* **Arrays**: Dynamic arrays, 2D arrays, matrices
* **Linked Lists**: Singly linked, doubly linked, circular linked lists
* **Stacks**: LIFO structures, monotonic stacks
* **Queues**: FIFO structures, priority queues, deques

### Non Linear Unordered Structures
* **Trees**: Binary trees, BST, AVL, red black trees, segment trees, Fenwick trees
* **Heaps**: Binary heaps, priority queues, heap algorithms
* **Tries**: Prefix trees, radix trees, compressed tries
* **Graphs**: Adjacency lists, adjacency matrices, graph algorithms
* **Skip Lists**: Probabilistic balanced structures
* **Circular Buffers**: Ring buffers for streaming data

## Key Components

### Core Operations
* **Insertion**: Efficient insertion algorithms
* **Deletion**: Efficient deletion algorithms
* **Search**: Fast search and lookup operations
* **Traversal**: Efficient traversal algorithms
* **Sorting**: In place and external sorting

### Advanced Features
* **Iterators**: Iterator design and implementation
* **Memory Management**: Efficient memory allocation and deallocation
* **Concurrency**: Thread safe data structures
* **Performance**: Cache aware design, SIMD optimizations
* **Persistence**: Serialization and deserialization

## Production Standards

### Code Quality
* Functions limited to 50 lines
* Files limited to 200 lines
* Cyclomatic complexity ≤ 10
* Comprehensive error handling
* Input validation on all public APIs
* Memory safety and leak prevention

### Performance
* Optimal time complexity for operations
* Efficient memory usage
* Cache friendly memory layout
* SIMD optimizations where applicable
* Benchmarking and profiling

### Correctness
* Correct implementation of data structure invariants
* Comprehensive test coverage
* Edge case handling
* Conformance testing
* Fuzzing for robustness

### Documentation
* API documentation for all public functions
* Time and space complexity analysis
* Thread safety guarantees
* Ownership semantics
* Performance characteristics

## Research Papers and References

### Data Structures
* "Introduction to Algorithms" (CLRS) - Comprehensive algorithms and data structures
* "The Art of Computer Programming" (Knuth) - Fundamental algorithms
* "Data Structures and Algorithms" (Aho, Hopcroft, Ullman)

### Hash Tables
* "Hash Tables" - Hash table design and analysis
* "Cuckoo Hashing" - Cuckoo hashing algorithm
* "Robin Hood Hashing" - Robin hood hashing

### Trees
* "Red Black Trees" - Red black tree algorithms
* "B Trees" - B tree design for databases
* "Segment Trees" - Segment tree algorithms

### Open Source References
* Google Abseil data structures
* Boost C++ Libraries containers
* LLVM data structures
* Redis data structure implementations
* Standard Template Library (STL)

## Implementation Goals

### Correctness
* Correct implementation of data structure operations
* Proper handling of edge cases
* Correct memory management
* Thread safety where applicable

### Performance
* Optimal algorithmic complexity
* Efficient memory usage
* Cache friendly design
* SIMD optimizations where applicable

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

