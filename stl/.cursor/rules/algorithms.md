# Algorithm Standards

## Overview
STL algorithms provide generic operations on sequences. This document defines standards for production grade algorithm usage and selection.

## Algorithm Categories

### Non Modifying Algorithms
* **find**: Find first occurrence of value
* **count**: Count occurrences of value
* **for_each**: Apply function to each element
* **search**: Search for subsequence
* **mismatch**: Find first mismatch
* **Rationale**: Non modifying algorithms don't change sequences

### Modifying Algorithms
* **transform**: Transform elements
* **copy**: Copy elements
* **move**: Move elements (C++11)
* **fill**: Fill with value
* **generate**: Generate values
* **Rationale**: Modifying algorithms change sequences

### Sorting Algorithms
* **sort**: Sort range (unstable, O(n log n))
* **stable_sort**: Stable sort (O(n log n) or O(n log² n))
* **partial_sort**: Sort first n elements
* **nth_element**: Partition around nth element
* **Rationale**: Sorting algorithms order sequences

### Binary Search Algorithms
* **lower_bound**: First position where value could be inserted
* **upper_bound**: Last position where value could be inserted
* **equal_range**: Range of equal elements
* **binary_search**: Check if value exists
* **Rationale**: Binary search requires sorted range

### Set Operations
* **set_union**: Union of two sorted sets
* **set_intersection**: Intersection of two sorted sets
* **set_difference**: Difference of two sorted sets
* **set_symmetric_difference**: Symmetric difference
* **Rationale**: Set operations work on sorted ranges

### Heap Operations
* **make_heap**: Create heap from range
* **push_heap**: Add element to heap
* **pop_heap**: Remove top element from heap
* **sort_heap**: Sort heap
* **Rationale**: Heap operations maintain heap property

### Min/Max Operations
* **min/max**: Minimum/maximum of two values
* **minmax**: Both min and max (C++11)
* **min_element/max_element**: Iterator to min/max element
* **Rationale**: Min/max operations find extremes

### Numeric Algorithms
* **accumulate**: Sum of range
* **inner_product**: Inner product of two ranges
* **partial_sum**: Partial sums
* **adjacent_difference**: Adjacent differences
* **Rationale**: Numeric algorithms perform mathematical operations

## Algorithm Selection

### Complexity Considerations
* **Time complexity**: Consider time complexity
* **Space complexity**: Consider space complexity
* **Stability**: Consider stability requirements
* **Rationale**: Complexity affects performance

### Preconditions
* **Sorted range**: Some algorithms require sorted range
* **Valid iterators**: All algorithms require valid iterators
* **Valid ranges**: All algorithms require valid ranges
* **Rationale**: Preconditions ensure correctness

## Implementation Standards

### Correctness
* **Preconditions**: Verify preconditions
* **Postconditions**: Verify postconditions
* **Exception safety**: Maintain exception safety
* **Rationale**: Correctness is critical

### Performance
* **Appropriate algorithm**: Choose algorithm with good complexity
* **Avoid unnecessary copies**: Minimize copies
* **Cache efficiency**: Consider cache friendly access
* **Rationale**: Performance is critical

## Common Patterns

### Find Pattern
```cpp
auto it = std::find(container.begin(), container.end(), value);
if (it != container.end()) {
    // Found
}
```

### Transform Pattern
```cpp
std::vector<int> result;
std::transform(input.begin(), input.end(), std::back_inserter(result),
               [](int x) { return x * 2; });
```

### Sort Pattern
```cpp
std::sort(container.begin(), container.end());
std::sort(container.begin(), container.end(), std::greater<int>());
```

## Testing Requirements

### Unit Tests
* **Operations**: Test all algorithm operations
* **Edge cases**: Test boundary conditions
* **Preconditions**: Test precondition violations
* **Exception safety**: Test exception safety
* **Rationale**: Comprehensive testing ensures correctness

## Research Papers and References

### Algorithms
* "Introduction to Algorithms" (CLRS) - Algorithm analysis
* "The Algorithm Design Manual" (Skiena) - Algorithm selection
* STL algorithm complexity guarantees

## Implementation Checklist

- [ ] Understand algorithm categories
- [ ] Learn algorithm complexity
- [ ] Understand preconditions
- [ ] Practice algorithm usage
- [ ] Write comprehensive unit tests
- [ ] Benchmark algorithm performance
- [ ] Document algorithm selection rationale

