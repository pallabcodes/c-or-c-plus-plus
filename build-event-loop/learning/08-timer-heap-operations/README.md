# Topic 8: Timer Heap Operations

## What

Helper functions for working with timers in the heap. Provides high-level operations for querying the timer heap without directly manipulating the heap structure.

## Why

Provides a clean API for:
- Finding the next timer timeout (for I/O poll timeout calculation)
- Peeking at the minimum timer (without removing it)
- Checking heap state (empty, count)

## Where Used

- Timer timeout calculation (for I/O poll)
- Timer execution (finding due timers)
- Event loop timeout management

## Universal Use

- **Schedulers**: Finding next scheduled task
- **Priority Queues**: Peeking at minimum priority item
- **Event Systems**: Finding next event timeout
- **Game Engines**: Finding next game event
- **Network Stacks**: Finding next timeout for select/poll

## Data Structures

Uses the timer heap from the event loop structure.

## Algorithms

### Next Timeout Calculation
- **Time Complexity**: O(1)
- **Space Complexity**: O(1)
- Peek at minimum timer, calculate time until expiry

### Peek Minimum
- **Time Complexity**: O(1)
- **Space Complexity**: O(1)
- Return minimum timer without removing it

### Heap State Queries
- **Time Complexity**: O(1)
- **Space Complexity**: O(1)
- Check if empty, get count

## libuv Implementation

### Source Files

- **Next Timeout**: `node/deps/uv/src/timer.c` (lines 144-162)
- **Timer Execution**: `node/deps/uv/src/timer.c` (lines 165-195)

### Key Functions

- `uv__next_timeout()`: Calculate next timer timeout
- `uv__run_timers()`: Execute due timers (Topic 9)

### libuv's Implementation

```c
int uv__next_timeout(const uv_loop_t* loop) {
  const struct heap_node* heap_node;
  const uv_timer_t* handle;
  uint64_t diff;

  heap_node = heap_min(timer_heap(loop));
  if (heap_node == NULL)
    return -1; /* block indefinitely */

  handle = container_of(heap_node, uv_timer_t, node.heap);
  if (handle->timeout <= loop->time)
    return 0;

  diff = handle->timeout - loop->time;
  if (diff > INT_MAX)
    diff = INT_MAX;

  return (int) diff;
}
```

**Our Implementation**:
- Similar logic but adapted for our array-based heap
- Uses heap property that minimum is at index 0

## Implementation Details

### Next Timeout

```c
int timer_heap_next_timeout(const struct event_loop* loop);
```

Returns:
- `-1`: No timers (block indefinitely)
- `0`: Timer has expired (execute immediately)
- `>0`: Milliseconds until next timer expires

### Peek Minimum

```c
struct timer* timer_heap_min(const struct event_loop* loop);
```

Returns pointer to minimum timer without removing it from heap.

### Heap State

```c
int timer_heap_empty(const struct event_loop* loop);
size_t timer_heap_count(const struct event_loop* loop);
```

Query heap state without modifying it.

## Testing

Run tests:
```bash
cd 08-timer-heap-operations
gcc -std=c11 -I. -I../01-intrusive-queue/src -I../02-min-heap/src \
    -I../03-time-management/src -I../04-loop-structure/src \
    -I../05-handle-structure/src -I../07-timer-structure/src \
    -o test_timer_heap tests/test_timer_heap.c src/timer_heap.c \
    ../07-timer-structure/src/timer.c ../05-handle-structure/src/handle.c \
    ../04-loop-structure/src/event_loop.c ../02-min-heap/src/heap.c \
    ../03-time-management/src/time.c && ./test_timer_heap
```

## Example Usage

See `examples/example.c` for a complete example showing:
- Querying heap state
- Finding minimum timer
- Calculating next timeout

## Universal Applications

### Where This Pattern is Used

1. **Schedulers**: Finding next scheduled task
2. **Priority Queues**: Peeking at minimum priority item
3. **Event Systems**: Finding next event timeout
4. **Game Engines**: Finding next game event
5. **Network Stacks**: Finding next timeout for select/poll/epoll
6. **OS Kernels**: Finding next timer interrupt
7. **Database Systems**: Finding next query timeout

### Why It's Important

- **Efficient Queries**: O(1) peek at minimum
- **Non-Destructive**: Peek without modifying heap
- **Timeout Calculation**: Essential for I/O poll timeout
- **State Inspection**: Query heap without side effects

## Integration with Event Loop

These functions are used by:
- **I/O Poll**: Calculate timeout for `epoll_wait`/`kevent`
- **Timer Execution**: Find due timers (Topic 9)
- **Loop Control**: Determine if loop should continue

## Next Steps

- Topic 9: Timer Execution (execute all due timers)
- Topic 10: Timer Timeout Calculation (use in I/O poll)

