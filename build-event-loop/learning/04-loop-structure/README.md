# Topic 4: Loop State Structure (`event_loop_t`)

## What

The main event loop structure that contains all queues, heaps, watchers, and state needed to run the event loop. This is the central data structure that ties everything together.

## Why

- **Central State**: Single structure containing all event loop state
- **Organization**: Groups related data together for better cache locality
- **Encapsulation**: Hides implementation details behind a clean interface
- **Foundation**: Everything else builds on top of this structure

## Where Used in libuv/Node.js

- Everywhere in the event loop - this is the core structure
- Loop initialization, execution, and cleanup
- All handle operations reference the loop
- All I/O operations reference the loop

**Reference**: 
- `node/deps/uv/include/uv.h` (lines 1934-1949) - `struct uv_loop_s`
- `node/deps/uv/include/uv/unix.h` (lines 220-251) - `UV_LOOP_PRIVATE_FIELDS`
- `node/deps/uv/src/unix/loop.c` (lines 30-113) - `uv_loop_init()`

## Universal Use

Event loop structures are used in:

- **All Event-Driven Systems**: Node.js, nginx, Apache, Redis
- **Game Engines**: Unity, Unreal Engine event systems
- **GUI Frameworks**: GTK, Qt, Windows message loops
- **Networking Stacks**: All async I/O frameworks
- **Embedded Systems**: Real-time event processing

## Data Structures

```c
struct event_loop {
  void* data;                    // User data
  
  struct heap* timer_heap;       // Timer heap (min-heap)
  
  struct queue idle_handles;     // Idle handle queue
  struct queue prepare_handles;  // Prepare handle queue
  struct queue check_handles;    // Check handle queue
  struct queue pending_queue;    // Pending callback queue
  struct queue closing_handles;  // Closing handle queue
  struct queue handle_queue;     // All handles queue
  
  struct io_watcher** watchers;  // I/O watcher array
  size_t nwatchers;              // Number of watchers
  size_t nfds;                   // Number of file descriptors
  
  int backend_fd;                // Platform I/O fd (epoll/kqueue)
  
  uint64_t time;                 // Current time (ms)
  unsigned int active_handles;   // Active handle count
  unsigned int stop_flag;        // Stop flag
  uint64_t timer_counter;        // Timer ID counter
};
```

**Key Components**:
- **Timer Heap**: Stores timers sorted by expiry time
- **Handle Queues**: Different queues for different handle types
- **I/O Watchers**: Array of file descriptor watchers
- **Platform FD**: epoll/kqueue file descriptor
- **State**: Time, active handles, stop flag

## Algorithms

### Initialization: O(1)

```
1. Zero-initialize structure
2. Initialize timer heap
3. Initialize all queues
4. Initialize watcher array (NULL)
5. Set initial time
6. Initialize state variables
```

### Alive Check: O(1)

```
1. Check active_handles > 0
2. Check closing_handles queue not empty
3. Check timer heap not empty
4. Return true if any condition is true
```

### Time Update: O(1)

```
1. Get current monotonic time
2. Store in loop->time
```

## Complexity Analysis

| Operation | Time Complexity | Notes |
|-----------|----------------|-------|
| Init | O(1) | Constant time initialization |
| Free | O(n) | Free watchers array |
| Alive Check | O(1) | Simple checks |
| Time Update | O(1) | System call overhead |

## Implementation Details

### Queue Initialization

All queues are initialized as empty circular lists:
```c
queue_init(&loop->idle_handles);
queue_init(&loop->prepare_handles);
// ... etc
```

### Timer Heap Initialization

Timer heap is allocated and initialized:
```c
loop->timer_heap = malloc(sizeof(struct heap));
heap_init(loop->timer_heap, 16);
```

### Watcher Array

Watcher array starts as NULL and grows as needed:
```c
loop->watchers = NULL;
loop->nwatchers = 0;
```

### Platform Initialization

Platform-specific I/O polling is initialized separately (Topic 19):
```c
loop->backend_fd = -1;  // Will be set by platform init
```

## Study Notes

### Key Insights from libuv Implementation

1. **Modular Design**: Public and private fields separated
2. **Platform Abstraction**: Platform-specific fields via macros
3. **Reference Counting**: Tracks active handles and requests
4. **Internal Fields**: Extensible via internal_fields pointer

### libuv Code Reference

- **Public Structure**: `node/deps/uv/include/uv.h` (lines 1934-1949)
- **Private Fields**: `node/deps/uv/include/uv/unix.h` (lines 220-251)
- **Initialization**: `node/deps/uv/src/unix/loop.c` (lines 30-113)

### Our Implementation Differences

- **Simplified**: Only essential fields for learning
- **No Platform Macros**: Direct structure definition
- **No Internal Fields**: Simpler without extensibility hooks
- **Clear Separation**: Each component is clear

## Testing

Run tests:
```bash
cd build-event-loop/learning/04-loop-structure
gcc -std=c11 -I. -I../01-intrusive-queue/src -I../02-min-heap/src -I../03-time-management/src \
    -o test_event_loop tests/test_event_loop.c src/event_loop.c \
    ../02-min-heap/src/heap.c ../03-time-management/src/time.c
./test_event_loop
```

## Example

See `examples/example.c` for a complete example of initializing and using the event loop structure.

## Next Steps

This loop structure will be used in:
- Topic 5: Handle Structure (handles reference the loop)
- Topic 6: Watcher Structure (watchers are stored in loop)
- Topic 22: Loop Initialization (uses this structure)
- Topic 24: Loop Iteration (executes phases using this structure)

## References

- libuv source: `node/deps/uv/include/uv.h`, `node/deps/uv/include/uv/unix.h`
- libuv init: `node/deps/uv/src/unix/loop.c`

