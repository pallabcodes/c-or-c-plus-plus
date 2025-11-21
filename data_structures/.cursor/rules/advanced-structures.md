# Advanced Data Structures Standards

## Overview
Advanced data structures provide specialized functionality for specific use cases. This document defines standards for implementing production grade advanced data structures including segment trees, Fenwick trees, skip lists, and specialized structures.

## Segment Trees

### Range Query Structure
* **Purpose**: Efficient range queries (sum, min, max, etc.)
* **Structure**: Complete binary tree storing range information
* **Operations**: Build, query, update
* **Complexity**: O(n) build, O(log n) query/update
* **Applications**: Range queries, interval problems, RMQ
* **Rationale**: Efficient range operations

### Implementation Standards
* **Array representation**: Use array for efficient memory layout
* **Lazy propagation**: Implement for range updates
* **Query types**: Support sum, min, max, gcd, etc.
* **Rationale**: Efficient implementation and flexibility

### Lazy Propagation
* **Purpose**: Efficient range updates
* **Complexity**: O(log n) range update
* **Implementation**: Store pending updates in nodes
* **Rationale**: Efficient range updates

## Fenwick Trees (Binary Indexed Trees)

### Prefix Sum Structure
* **Purpose**: Efficient prefix sum queries
* **Structure**: Array based tree structure
* **Operations**: Update, query prefix sum
* **Complexity**: O(log n) update/query
* **Applications**: Prefix sums, inversions, range sum queries
* **Rationale**: Simpler than segment trees for prefix operations

### Implementation Standards
* **Array representation**: Use array for efficient memory layout
* **Bit manipulation**: Use bit operations for efficient navigation
* **Indexing**: 1 based indexing for simpler implementation
* **Rationale**: Efficient and simple implementation

## Skip Lists

### Probabilistic Structure
* **Purpose**: Alternative to balanced trees
* **Structure**: Multi level linked lists
* **Operations**: Insert, delete, search
* **Complexity**: O(log n) expected, O(n) worst case
* **Applications**: Database indexes, priority queues
* **Rationale**: Simpler than balanced trees, good average performance

### Implementation Standards
* **Level generation**: Use random level generation
* **Probability**: Use appropriate probability (typically 0.5)
* **Sentinel nodes**: Use sentinel nodes for simpler implementation
* **Rationale**: Correct probabilistic structure

## Circular Buffers (Ring Buffers)

### Fixed Size Buffer
* **Purpose**: Efficient fixed size FIFO buffer
* **Structure**: Circular array with head and tail pointers
* **Operations**: Enqueue, dequeue
* **Complexity**: O(1) enqueue/dequeue
* **Applications**: Streaming data, producer consumer patterns
* **Rationale**: Efficient fixed size buffer

### Implementation Standards
* **Array based**: Use array for cache efficiency
* **Full/empty detection**: Distinguish full from empty
* **Thread safety**: Consider thread safety for concurrent access
* **Rationale**: Efficient and correct implementation

## Specialized Structures

### Disjoint Set Union (Union Find)
* **Purpose**: Efficient union and find operations
* **Structure**: Forest of trees with path compression
* **Operations**: Union, find
* **Complexity**: O(α(n)) amortized (inverse Ackermann)
* **Applications**: Kruskal's algorithm, connected components
* **Rationale**: Extremely efficient union find operations

### Bloom Filters
* **Purpose**: Probabilistic membership testing
* **Structure**: Bit array with multiple hash functions
* **Operations**: Insert, query
* **Complexity**: O(k) where k is number of hash functions
* **Applications**: Caching, distributed systems
* **Rationale**: Space efficient probabilistic structure

### Roaring Bitmaps
* **Purpose**: Efficient bitmap compression
* **Structure**: Hybrid structure (arrays and bitmaps)
* **Operations**: Set, get, union, intersection
* **Complexity**: O(n) operations
* **Applications**: Database indexes, set operations
* **Rationale**: Memory efficient bitmap operations

## Implementation Standards

### Correctness
* **Invariants**: Document and maintain invariants
* **Edge cases**: Handle edge cases correctly
* **Error handling**: Robust error handling
* **Rationale**: Correctness is critical

### Performance
* **Complexity**: Achieve stated complexity bounds
* **Optimization**: Optimize hot paths
* **Benchmarking**: Benchmark against alternatives
* **Rationale**: Performance is critical

### Memory Efficiency
* **Memory layout**: Optimize memory layout
* **Compression**: Use compression where applicable
* **Overhead**: Minimize overhead
* **Rationale**: Memory efficiency affects scalability

## Testing Requirements

### Unit Tests
* **Operations**: Test all operations
* **Invariants**: Test that invariants are maintained
* **Edge cases**: Test boundary conditions
* **Rationale**: Comprehensive testing ensures correctness

### Property Based Tests
* **Invariants**: Test invariants hold for random operations
* **Fuzzing**: Fuzz operations with random inputs
* **Stress tests**: Test with large datasets
* **Rationale**: Property based tests find edge cases

### Performance Tests
* **Benchmarks**: Benchmark against alternatives
* **Scalability**: Test with different sizes
* **Comparison**: Compare with standard implementations
* **Rationale**: Performance tests ensure performance goals

## Research Papers and References

### Segment Trees
* "Introduction to Algorithms" (CLRS) - Segment trees
* Research papers on lazy propagation

### Fenwick Trees
* "A New Data Structure for Cumulative Frequency Tables" (Fenwick)
* Research papers on binary indexed trees

### Skip Lists
* "Skip Lists: A Probabilistic Alternative to Balanced Trees" (Pugh)
* Research papers on skip lists

### Disjoint Sets
* "Introduction to Algorithms" (CLRS) - Union find
* Research papers on path compression

## Implementation Checklist

- [ ] Implement segment tree with lazy propagation
- [ ] Implement Fenwick tree
- [ ] Implement skip list
- [ ] Implement circular buffer
- [ ] Implement union find
- [ ] Implement bloom filter
- [ ] Add error handling
- [ ] Write comprehensive unit tests
- [ ] Benchmark performance
- [ ] Document time and space complexity

