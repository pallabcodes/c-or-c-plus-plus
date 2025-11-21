# Topic 2: Min-Heap / Binary Heap

## What

A min-heap (minimum heap) is a complete binary tree data structure where each parent node is less than or equal to its children. This ensures the minimum element is always at the root.

## Why

- **O(log n) Operations**: Insert and extract operations are logarithmic time
- **O(1) Peek**: Accessing the minimum element is constant time
- **Efficient for Timers**: Perfect for managing timers sorted by expiry time
- **Cache Friendly**: Array-based implementation provides better cache locality than pointer-based trees

## Where Used in libuv/Node.js

- Timer management: Timers are stored in a min-heap sorted by expiry time
- Finding next timer: O(1) peek at minimum timer
- Timer execution: Extract due timers in order

**Reference**: `node/deps/uv/src/heap-inl.h` (libuv uses tree-based heap, we use array-based for simplicity)

**Note**: libuv uses a pointer-based tree heap, but for learning we implement an array-based heap which is simpler and more cache-friendly.

## Universal Use

Min-heaps are used extensively in:

- **Schedulers**: OS schedulers use heaps for priority queues
- **Graph Algorithms**: Dijkstra's algorithm uses min-heap for finding shortest paths
- **Game Engines**: A* pathfinding uses heaps for open/closed sets
- **Networking**: Network stacks use heaps for timeout management
- **Databases**: Query optimizers use heaps for sorting and merging
- **Event Systems**: Event loops use heaps for timer management

## Data Structures

```c
struct heap_node {
  uint64_t key;  // Priority/key value (e.g., timer expiry time)
  void* data;     // User data pointer
};

struct heap {
  struct heap_node* nodes;  // Array of heap nodes
  size_t capacity;          // Maximum capacity
  size_t size;              // Current size
};
```

**Key Characteristics**:
- Complete binary tree: All levels filled except possibly the last
- Array representation: Parent at index i, children at 2*i+1 and 2*i+2
- Min-heap property: Parent <= children (for all nodes)

**Array Index Relationships**:
- Parent of node i: `(i - 1) / 2`
- Left child of node i: `2 * i + 1`
- Right child of node i: `2 * i + 2`

## Algorithms

### Insert: O(log n)

**Algorithm**:
```
1. Insert new element at the end of the array (last position)
2. Bubble up (heapify up):
   - Compare with parent
   - If parent > child, swap
   - Repeat until heap property is restored
```

**Example**:
```
Insert 1 into heap [2, 5, 3]:
1. Add to end: [2, 5, 3, 1]
2. Compare 1 with parent 5: swap -> [2, 1, 3, 5]
3. Compare 1 with parent 2: swap -> [1, 2, 3, 5]
```

### Extract Min: O(log n)

**Algorithm**:
```
1. Extract root (minimum element)
2. Move last element to root
3. Bubble down (heapify down):
   - Compare with children
   - Swap with smallest child if parent > child
   - Repeat until heap property is restored
```

**Example**:
```
Extract min from [1, 2, 3, 5]:
1. Extract 1, move 5 to root: [5, 2, 3]
2. Compare 5 with children (2, 3): swap with 2 -> [2, 5, 3]
3. Compare 5 with child 3: swap -> [2, 3, 5]
```

### Peek Min: O(1)

Simply return the root element (index 0).

### Remove: O(log n)

**Algorithm**:
```
1. Move last element to position of element to remove
2. Restore heap property (bubble up or down as needed)
```

## Complexity Analysis

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| Insert | O(log n) | O(1) amortized |
| Extract Min | O(log n) | O(1) |
| Peek Min | O(1) | O(1) |
| Remove | O(log n) | O(1) |
| Build Heap | O(n) | O(1) |

## Implementation Details

### Array-Based vs Tree-Based

**Array-Based (Our Implementation)**:
- Better cache locality
- Simpler implementation
- Fixed overhead (no pointer storage)
- Used in most production systems

**Tree-Based (libuv's Implementation)**:
- More flexible (can remove arbitrary nodes easily)
- More complex implementation
- Pointer overhead
- Used when arbitrary removal is needed

### Heap Property Maintenance

**Heapify Up (Bubble Up)**:
- Used after insertion
- Move element up until parent <= child
- At most log(n) comparisons

**Heapify Down (Bubble Down)**:
- Used after extraction or removal
- Move element down until parent <= children
- At most log(n) comparisons

### Growth Strategy

When capacity is exceeded:
- Double the capacity
- Reallocate array
- Copy existing elements
- Amortized O(1) insertion cost

## Study Notes

### Key Insights from libuv Implementation

1. **Tree-based heap**: libuv uses pointer-based tree heap for flexibility
2. **Path calculation**: Uses bit manipulation to find insertion/removal points
3. **Heap property**: Maintains min-heap property throughout operations
4. **Comparison function**: Uses function pointer for flexible comparison

### libuv Code Reference

- **File**: `node/deps/uv/src/heap-inl.h`
- **Lines**: Entire file (~245 lines)
- **Key Functions**:
  - `heap_init()` - Initialize heap
  - `heap_insert()` - Insert element
  - `heap_remove()` - Remove element
  - `heap_dequeue()` - Extract minimum
  - `heap_min()` - Peek at minimum

### Our Implementation Differences

- **Array-based**: Simpler, better cache performance
- **Fixed key type**: Uses uint64_t for timer expiry times
- **Automatic growth**: Handles capacity expansion automatically
- **Simpler API**: Easier to understand and use

## Testing

Run tests:
```bash
cd build-event-loop/learning/02-min-heap
mkdir build && cd build
cmake ..
make
./test_heap
```

Or compile directly:
```bash
gcc -std=c11 -I. -o test_heap tests/test_heap.c src/heap.c
./test_heap
```

## Example

See `examples/example.c` for a complete example of using the heap as a priority queue.

## Next Steps

This heap implementation will be used in:
- Topic 7: Timer Structure (storing timers)
- Topic 8: Timer Heap Operations (insert/extract timers)
- Topic 9: Timer Execution (finding due timers)
- Topic 10: Timer Timeout Calculation (finding next timeout)

## References

- libuv source: `node/deps/uv/src/heap-inl.h`
- Introduction to Algorithms (CLRS): Chapter 6 - Heapsort
- Linux kernel: Uses heaps for timer management
- Redis: Uses heaps for sorted sets

