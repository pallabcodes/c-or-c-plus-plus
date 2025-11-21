# Container Standards

## Overview
STL containers are fundamental data structures for storing and managing collections of elements. This document defines standards for production grade container usage and selection.

## Container Categories

### Sequence Containers
* **vector**: Dynamic array, random access, amortized O(1) push_back
* **deque**: Double ended queue, random access, O(1) push_front/push_back
* **list**: Doubly linked list, bidirectional iterator, O(1) insert/erase
* **forward_list**: Singly linked list, forward iterator, O(1) insert_after/erase_after
* **array**: Fixed size array, random access, stack allocated
* **Rationale**: Sequence containers store elements in linear order

### Associative Containers
* **set**: Sorted unique keys, O(log n) operations, balanced tree
* **multiset**: Sorted keys (duplicates allowed), O(log n) operations
* **map**: Sorted key value pairs, O(log n) operations
* **multimap**: Sorted key value pairs (duplicates allowed), O(log n) operations
* **Rationale**: Associative containers provide sorted access

### Unordered Containers
* **unordered_set**: Hash table, average O(1) operations, no ordering
* **unordered_multiset**: Hash table with duplicates, average O(1) operations
* **unordered_map**: Hash table key value pairs, average O(1) operations
* **unordered_multimap**: Hash table with duplicate keys, average O(1) operations
* **Rationale**: Unordered containers provide fast average case access

### Container Adapters
* **stack**: LIFO adapter (default deque)
* **queue**: FIFO adapter (default deque)
* **priority_queue**: Heap adapter (default vector)
* **Rationale**: Container adapters provide specific access patterns

## Container Selection Guidelines

### When to Use vector
* **Random access**: Need random access by index
* **Append operations**: Primarily append to end
* **Cache efficiency**: Need cache friendly access
* **Rationale**: vector is most efficient for most use cases

### When to Use deque
* **Both ends**: Need efficient insertion at both ends
* **Random access**: Need random access
* **Large size**: Very large containers (better memory fragmentation)
* **Rationale**: deque provides efficient double ended operations

### When to Use list
* **Middle insertion**: Frequent insertion in middle
* **Stability**: Need iterator stability (no invalidation)
* **Splicing**: Need efficient splicing operations
* **Rationale**: list provides stable iterators and efficient middle operations

### When to Use set/map
* **Sorted access**: Need sorted order
* **Range queries**: Need range based queries
* **Ordered iteration**: Need ordered iteration
* **Rationale**: Associative containers provide sorted access

### When to Use unordered_set/unordered_map
* **Fast lookup**: Need fast average case lookup
* **No ordering**: Don't need sorted order
* **Hashable keys**: Keys are hashable
* **Rationale**: Unordered containers provide fastest average case access

## Container Operations

### Capacity Management
* **reserve**: Reserve capacity for vector (avoid reallocations)
* **shrink_to_fit**: Reduce capacity to size (C++11)
* **Rationale**: Capacity management optimizes memory usage

### Element Access
* **at**: Bounds checked access (throws on out of bounds)
* **operator[]**: Unchecked access (undefined behavior on out of bounds)
* **front/back**: Access first/last element
* **Rationale**: Different access methods for different safety needs

### Modifiers
* **insert**: Insert elements (may invalidate iterators)
* **emplace**: Construct in place (C++11, more efficient)
* **erase**: Remove elements (may invalidate iterators)
* **clear**: Remove all elements
* **Rationale**: Modifiers change container contents

## Iterator Invalidation Rules

### vector
* **Insert/erase**: All iterators invalidated if reallocation occurs
* **push_back**: All iterators invalidated if reallocation occurs
* **Rationale**: Reallocation moves elements to new memory

### deque
* **Insert/erase**: Iterators invalidated, but references remain valid
* **push_front/push_back**: Iterators invalidated, but references remain valid
* **Rationale**: deque uses multiple blocks, iterators track block and position

### list/forward_list
* **Insert/erase**: Iterators remain valid (except erased iterators)
* **Splicing**: Iterators remain valid
* **Rationale**: Linked list structure preserves iterator validity

### Associative Containers
* **Insert/erase**: Iterators remain valid (except erased iterators)
* **Rationale**: Tree structure preserves iterator validity

### Unordered Containers
* **Rehash**: All iterators invalidated on rehash
* **Insert**: Iterators remain valid (unless rehash occurs)
* **Rationale**: Rehashing reorganizes hash table

## Implementation Standards

### Correctness
* **Iterator validity**: Ensure iterators remain valid
* **Bounds checking**: Use bounds checking in debug builds
* **Exception safety**: Maintain exception safety guarantees
* **Rationale**: Correctness is critical

### Performance
* **Reserve capacity**: Reserve capacity when size is known
* **Move semantics**: Use move semantics to avoid copies
* **Appropriate container**: Choose container based on usage patterns
* **Rationale**: Performance is critical

## Testing Requirements

### Unit Tests
* **Operations**: Test all container operations
* **Iterator validity**: Test iterator validity rules
* **Edge cases**: Test boundary conditions
* **Exception safety**: Test exception safety guarantees
* **Rationale**: Comprehensive testing ensures correctness

## Research Papers and References

### Containers
* "The C++ Standard Library" (Josuttis) - Comprehensive container reference
* "Effective STL" (Meyers) - Container best practices
* STL container complexity guarantees

## Implementation Checklist

- [ ] Understand container categories and characteristics
- [ ] Learn container selection guidelines
- [ ] Understand iterator invalidation rules
- [ ] Practice container operations
- [ ] Write comprehensive unit tests
- [ ] Benchmark container performance
- [ ] Document container selection rationale

