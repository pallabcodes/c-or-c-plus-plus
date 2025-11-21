# Iterator Design Standards

## Overview
Iterators provide a uniform interface for traversing data structures. This document defines standards for implementing production grade iterators that are efficient, safe, and consistent with standard library conventions.

## Iterator Categories

### Input Iterator
* **Operations**: Read, increment, equality comparison
* **Single pass**: Can only traverse once
* **Applications**: Reading from streams
* **Rationale**: Basic read only iterator

### Forward Iterator
* **Operations**: All input iterator operations, multi pass
* **Multi pass**: Can traverse multiple times
* **Applications**: Singly linked lists
* **Rationale**: Forward only traversal

### Bidirectional Iterator
* **Operations**: All forward iterator operations, decrement
* **Bidirectional**: Can traverse forward and backward
* **Applications**: Doubly linked lists, trees
* **Rationale**: Bidirectional traversal

### Random Access Iterator
* **Operations**: All bidirectional iterator operations, arithmetic
* **Random access**: Can jump to arbitrary positions
* **Applications**: Arrays, vectors
* **Rationale**: Efficient random access

## Iterator Implementation

### Iterator Traits
* **value_type**: Type of elements
* **difference_type**: Type for distance
* **pointer**: Pointer type
* **reference**: Reference type
* **iterator_category**: Iterator category
* **Rationale**: Standard iterator interface

### Iterator Operations
* **Dereference**: `*it` returns reference to element
* **Increment**: `++it` advances iterator
* **Decrement**: `--it` (bidirectional/random access)
* **Arithmetic**: `it + n`, `it - n` (random access)
* **Comparison**: `it1 == it2`, `it1 < it2`
* **Rationale**: Standard iterator operations

### Example Implementation
```cpp
template<typename T>
class ArrayIterator {
private:
    T* ptr_;
    
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    
    // Dereference
    reference operator*() const { return *ptr_; }
    
    // Increment
    ArrayIterator& operator++() { ++ptr_; return *this; }
    
    // Decrement
    ArrayIterator& operator--() { --ptr_; return *this; }
    
    // Arithmetic
    ArrayIterator operator+(difference_type n) const {
        return ArrayIterator(ptr_ + n);
    }
    
    // Comparison
    bool operator==(const ArrayIterator& other) const {
        return ptr_ == other.ptr_;
    }
};
```

## Range Based Operations

### Range Support
* **begin()**: Return iterator to beginning
* **end()**: Return iterator to end (one past last)
* **C++11**: Support range based for loops
* **Rationale**: Modern C++ convenience

### Example Range Support
```cpp
template<typename T>
class DynamicArray {
public:
    ArrayIterator<T> begin() { return ArrayIterator(data_); }
    ArrayIterator<T> end() { return ArrayIterator(data_ + size_); }
    
    // Const iterators
    ArrayIterator<const T> begin() const { return ArrayIterator(data_); }
    ArrayIterator<const T> end() const { return ArrayIterator(data_ + size_); }
};
```

## Iterator Safety

### Invalidation
* **Document invalidation**: Document when iterators are invalidated
* **Operations**: Document which operations invalidate iterators
* **Examples**: Insert/delete operations may invalidate iterators
* **Rationale**: Iterator safety prevents undefined behavior

### Bounds Checking
* **Debug mode**: Check bounds in debug mode
* **Release mode**: Optimize away checks in release mode
* **Rationale**: Safety in debug, performance in release

## Performance Considerations

### Zero Overhead
* **No overhead**: Iterators should have zero overhead
* **Inline**: Inline iterator operations
* **Optimization**: Allow compiler optimization
* **Rationale**: Performance is critical

### Cache Efficiency
* **Sequential access**: Iterators encourage sequential access
* **Cache friendly**: Sequential access is cache friendly
* **Rationale**: Cache efficiency affects performance

## Testing Requirements

### Iterator Tests
* **Traversal**: Test iterator traversal
* **Operations**: Test all iterator operations
* **Bounds**: Test iterator bounds
* **Invalidation**: Test iterator invalidation
* **Rationale**: Comprehensive testing ensures correctness

### Range Tests
* **Range based for**: Test range based for loops
* **STL algorithms**: Test with STL algorithms
* **Rationale**: Compatibility with standard library

## Research Papers and References

### Iterators
* "The C++ Programming Language" (Stroustrup) - Iterator design
* Standard Template Library (STL) iterator implementation
* "Effective STL" (Meyers) - Iterator best practices

## Implementation Checklist

- [ ] Define iterator traits
- [ ] Implement iterator operations
- [ ] Implement range support (begin/end)
- [ ] Document iterator invalidation
- [ ] Add bounds checking (debug mode)
- [ ] Write iterator tests
- [ ] Test with STL algorithms
- [ ] Benchmark iterator performance

