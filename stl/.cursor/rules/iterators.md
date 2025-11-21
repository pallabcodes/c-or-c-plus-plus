# Iterator Standards

## Overview
Iterators provide a uniform interface for traversing containers and sequences. This document defines standards for production grade iterator usage and implementation.

## Iterator Categories

### Input Iterator
* **Operations**: Read, increment (forward only)
* **Single pass**: Can only traverse once
* **Examples**: istream_iterator
* **Rationale**: Input iterators read from input streams

### Output Iterator
* **Operations**: Write, increment (forward only)
* **Single pass**: Can only traverse once
* **Examples**: ostream_iterator
* **Rationale**: Output iterators write to output streams

### Forward Iterator
* **Operations**: Read/write, increment (forward only)
* **Multi pass**: Can traverse multiple times
* **Examples**: forward_list iterator
* **Rationale**: Forward iterators support forward traversal

### Bidirectional Iterator
* **Operations**: Read/write, increment, decrement
* **Multi pass**: Can traverse forward and backward
* **Examples**: list iterator, set iterator
* **Rationale**: Bidirectional iterators support two way traversal

### Random Access Iterator
* **Operations**: Read/write, increment, decrement, arithmetic, comparison
* **Multi pass**: Can jump to any position
* **Examples**: vector iterator, deque iterator
* **Rationale**: Random access iterators support efficient random access

## Iterator Operations

### Basic Operations
* **Dereference**: *it (get value)
* **Increment**: ++it, it++ (move forward)
* **Decrement**: --it, it-- (move backward, bidirectional+)
* **Equality**: it1 == it2, it1 != it2
* **Rationale**: Basic operations enable traversal

### Random Access Operations
* **Arithmetic**: it + n, it - n, it += n, it -= n
* **Difference**: it1 - it2 (distance)
* **Comparison**: it1 < it2, it1 > it2, it1 <= it2, it1 >= it2
* **Indexing**: it[n] (equivalent to *(it + n))
* **Rationale**: Random access operations enable efficient positioning

## Iterator Traits

### iterator_traits
* **value_type**: Type of element
* **difference_type**: Type for distance
* **pointer**: Pointer to element
* **reference**: Reference to element
* **iterator_category**: Iterator category tag
* **Rationale**: Traits enable generic algorithms

### Iterator Category Tags
* **input_iterator_tag**: Input iterator category
* **output_iterator_tag**: Output iterator category
* **forward_iterator_tag**: Forward iterator category
* **bidirectional_iterator_tag**: Bidirectional iterator category
* **random_access_iterator_tag**: Random access iterator category
* **Rationale**: Tags enable algorithm selection

## Iterator Validity

### Valid Iterators
* **Container iterators**: Iterators from valid containers
* **Not end iterator**: Iterator is not end() (unless comparing)
* **Not invalidated**: Iterator hasn't been invalidated
* **Rationale**: Valid iterators can be safely dereferenced

### Invalid Iterators
* **End iterator**: end() iterator (cannot dereference)
* **Invalidated**: Iterator invalidated by container operation
* **Default constructed**: Default constructed iterator (unless specified)
* **Rationale**: Invalid iterators cause undefined behavior

## Iterator Ranges

### Valid Ranges
* **[first, last)**: Half open range (first inclusive, last exclusive)
* **first <= last**: first must be before or equal to last
* **Same container**: Both iterators from same container
* **Rationale**: Valid ranges enable safe iteration

### Range Validation
* **Check before use**: Validate range before using
* **Empty range**: Handle empty ranges (first == last)
* **Bounds checking**: Check bounds in debug builds
* **Rationale**: Range validation prevents undefined behavior

## Custom Iterators

### Iterator Requirements
* **Iterator traits**: Define iterator_traits specialization
* **Operations**: Implement required operations
* **Category tag**: Define iterator_category
* **Rationale**: Custom iterators enable custom container traversal

### Example Custom Iterator
```cpp
template<typename Container>
class ContainerIterator {
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = typename Container::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;
    
    // Required operations
    reference operator*() const;
    ContainerIterator& operator++();
    bool operator==(const ContainerIterator& other) const;
    // ... other operations
};
```

## Implementation Standards

### Correctness
* **Valid iterators**: Ensure iterators are valid before use
* **Range validation**: Validate iterator ranges
* **No invalidation**: Avoid using invalidated iterators
* **Rationale**: Correctness is critical

### Performance
* **Efficient traversal**: Use efficient traversal patterns
* **Cache efficiency**: Consider cache friendly access
* **Appropriate iterator**: Use appropriate iterator category
* **Rationale**: Performance is critical

## Testing Requirements

### Unit Tests
* **Operations**: Test all iterator operations
* **Validity**: Test iterator validity rules
* **Ranges**: Test iterator ranges
* **Edge cases**: Test boundary conditions
* **Rationale**: Comprehensive testing ensures correctness

## Research Papers and References

### Iterators
* "The C++ Standard Library" (Josuttis) - Iterator reference
* "Effective STL" (Meyers) - Iterator best practices
* STL iterator requirements

## Implementation Checklist

- [ ] Understand iterator categories
- [ ] Learn iterator operations
- [ ] Understand iterator traits
- [ ] Learn iterator validity rules
- [ ] Practice iterator usage
- [ ] Write comprehensive unit tests
- [ ] Document iterator guarantees

