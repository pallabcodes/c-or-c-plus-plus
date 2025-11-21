# Topic 1: Intrusive Doubly-Linked List Queue

## What

An intrusive doubly-linked list queue data structure. This is a fundamental building block used throughout libuv for managing handles, callbacks, and watchers.

## Why

- **O(1) Operations**: Insertion and removal are constant time
- **Memory Efficient**: No separate node allocations needed - the queue node is embedded directly in the containing structure
- **Cache Friendly**: Better cache locality since data and links are together
- **Zero Overhead**: No extra memory allocations beyond the containing structure

## Where Used in libuv/Node.js

- Handle queues (idle, prepare, check, pending, closing)
- Watcher queues
- Pending callback queues
- Process handle queues
- Thread pool work queues

**Reference**: `node/deps/uv/src/queue.h` (entire file, ~90 lines)

## Universal Use

This data structure pattern is used extensively in:

- **OS Kernels**: Linux kernel uses intrusive lists for process management, file descriptors, etc.
- **Database Systems**: PostgreSQL, MySQL use intrusive lists for connection pools, query queues
- **Networking Stacks**: nginx, Apache use intrusive lists for connection management
- **Game Engines**: Entity component systems use intrusive lists for component storage
- **Embedded Systems**: Memory-constrained systems benefit from zero-allocation queues

## Data Structures

```c
struct queue {
  struct queue* next;  // Pointer to next element
  struct queue* prev;  // Pointer to previous element
};
```

**Key Characteristics**:
- Circular structure: Empty queue points to itself
- Intrusive: Queue node is embedded in containing structure
- No separate allocation: No malloc/free for queue nodes

**Example Usage**:
```c
struct my_item {
  int data;
  struct queue q;  // Queue node embedded here
};

struct queue head;
struct my_item item;

queue_init(&head);
queue_insert_tail(&head, &item.q);
```

## Algorithms

### Insertion: O(1)

**Insert at Head**:
```
1. Set q->next = h->next
2. Set q->prev = h
3. Update h->next->prev = q
4. Update h->next = q
```

**Insert at Tail**:
```
1. Set q->next = h
2. Set q->prev = h->prev
3. Update h->prev->next = q
4. Update h->prev = q
```

### Removal: O(1)

```
1. Set q->prev->next = q->next
2. Set q->next->prev = q->prev
```

### Traversal: O(n)

```
For each element:
  1. Start at head->next
  2. Process element
  3. Move to next
  4. Stop when back at head
```

### Queue Operations

- **queue_init**: O(1) - Initialize empty queue
- **queue_empty**: O(1) - Check if empty
- **queue_insert_head**: O(1) - Insert at head
- **queue_insert_tail**: O(1) - Insert at tail
- **queue_remove**: O(1) - Remove element
- **queue_add**: O(1) - Merge two queues
- **queue_split**: O(1) - Split queue at element
- **queue_move**: O(1) - Move all elements

## Implementation Details

### Intrusive Pattern

The intrusive pattern means the queue node is part of the containing structure:

```c
struct handle {
  int handle_type;
  void* data;
  struct queue q;  // Queue node embedded here
};
```

This eliminates the need for separate node allocations and improves cache locality.

### Container-of Macro

The `queue_data` macro uses `offsetof` to get the containing structure:

```c
#define queue_data(pointer, type, field) \
  ((type*) ((char*) (pointer) - offsetof(type, field)))
```

This allows converting from a queue pointer to the containing structure pointer.

### Circular Structure

An empty queue is circular - it points to itself:

```c
queue->next = queue;
queue->prev = queue;
```

This simplifies empty checks and eliminates special cases.

## Using libuv's Queue Directly

Instead of our implementation, you can use libuv's production-grade queue directly:

```c
#include "queue_libuv.h"  // Wrapper around libuv's queue.h

struct uv__queue head;
uv__queue_init(&head);
// Use libuv's queue functions directly
```

**Benefits**:
- Production-grade code (battle-tested)
- Learn from actual Node.js implementation
- No bugs from reimplementation
- Can study the source code directly

See `examples/example_libuv.c` for a complete example using libuv's queue.

## Study Notes

### Key Insights from libuv Implementation

1. **All functions are inline**: For performance, all queue operations are inline functions
2. **Circular structure**: Empty queue points to itself, simplifying checks
3. **No error handling**: Queue operations assume valid pointers (libuv style)
4. **Macro-based iteration**: `queue_foreach` macro for safe iteration

### libuv Code Reference

- **File**: `node/deps/uv/src/queue.h`
- **Lines**: Entire file (small, ~90 lines)
- **Key Functions**:
  - `uv__queue_init()` - Initialize queue
  - `uv__queue_insert_head()` - Insert at head
  - `uv__queue_insert_tail()` - Insert at tail
  - `uv__queue_remove()` - Remove element
  - `uv__queue_empty()` - Check if empty

### Direct Import Option

You can include libuv's queue.h directly:
```c
#include "../../../../node/deps/uv/src/queue.h"
// Now use uv__queue instead of queue
```

## Testing

Run tests:
```bash
cd build-event-loop/learning/01-intrusive-queue
mkdir build && cd build
cmake ..
make
./test_queue
```

## Example

See `examples/example.c` for a complete example of using the queue to manage a task list.

## Next Steps

This queue implementation will be used in:
- Topic 4: Loop State Structure (handle queues)
- Topic 11-15: Handle Management (idle, prepare, check, pending, closing queues)
- Topic 20: File Descriptor Management (watcher queues)

## References

- libuv source: `node/deps/uv/src/queue.h`
- Linux kernel: Uses similar intrusive list pattern
- nginx: Uses intrusive lists for connection management

