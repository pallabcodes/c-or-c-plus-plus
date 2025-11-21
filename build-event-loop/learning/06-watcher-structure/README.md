# Topic 6: Watcher Structure

## What

File descriptor watcher for I/O events. Tracks which file descriptors to monitor for I/O events (read, write, error, hang up) and manages their registration with the platform-specific I/O polling mechanism (epoll/kqueue).

## Why

The event loop needs to know which file descriptors to monitor for I/O events. The watcher structure provides a way to:
- Track file descriptors and their event interests
- Register/unregister with the platform-specific poller
- Queue watchers for processing
- Handle pending callbacks

## Where Used

- I/O polling system (epoll/kqueue integration)
- File descriptor management
- Event loop I/O phase
- All I/O-based handles (TCP, UDP, pipes, etc.)

## Universal Use

- **I/O Multiplexing Systems**: epoll, kqueue, select, poll
- **Networking Stacks**: Socket monitoring, connection management
- **File Systems**: File descriptor tracking, async I/O
- **OS Kernels**: File descriptor management, I/O event notification
- **Database Systems**: Connection monitoring, async I/O
- **Game Engines**: Network I/O, file I/O

## Data Structures

### I/O Watcher Structure

```c
struct io_watcher {
  /* Queue nodes */
  struct queue pending_queue;   /* Queue node for pending callbacks */
  struct queue watcher_queue;   /* Queue node for watcher queue */
  
  /* Callback function */
  io_watcher_cb cb;             /* Callback to call when events occur */
  
  /* File descriptor */
  int fd;                       /* File descriptor to watch */
  
  /* Event masks */
  unsigned int events;          /* Current events (registered with poller) */
  unsigned int pevents;         /* Pending events (to be registered) */
};
```

### I/O Event Flags

- `IO_EVENT_READ`: Data available for reading (POLLIN)
- `IO_EVENT_WRITE`: Ready for writing (POLLOUT)
- `IO_EVENT_ERROR`: Error condition (POLLERR)
- `IO_EVENT_HUP`: Hang up (POLLHUP)

### I/O Callback Type

```c
typedef void (*io_watcher_cb)(struct event_loop* loop,
                               struct io_watcher* watcher,
                               unsigned int events);
```

## Algorithms

### Watcher Initialization
- **Time Complexity**: O(1)
- **Space Complexity**: O(1)
- Initialize queues, set callback and file descriptor

### Start Watching
- **Time Complexity**: O(1) (amortized)
- **Space Complexity**: O(1)
- Add events to pending events, add to watcher queue

### Stop Watching
- **Time Complexity**: O(1)
- **Space Complexity**: O(1)
- Remove events from pending events, remove from queue if no events

### Close Watcher
- **Time Complexity**: O(1)
- **Space Complexity**: O(1)
- Stop all events, remove from queues, reset file descriptor

## libuv Implementation

### Source Files

- **Structure Definition**: `node/deps/uv/src/unix/core.c` (lines 906-914)
- **Watcher Functions**: `node/deps/uv/src/unix/core.c` (lines 917-1003)
- **I/O Polling**: `node/deps/uv/src/unix/linux.c` (epoll), `node/deps/uv/src/unix/kqueue.c` (kqueue)

### Key Functions

- `uv__io_init()`: Initialize watcher
- `uv__io_start()`: Start watching for events
- `uv__io_stop()`: Stop watching for events
- `uv__io_close()`: Close watcher
- `uv__io_poll()`: Poll for I/O events (platform-specific)

### libuv's Structure

```c
struct uv__io_t {
  struct uv__queue pending_queue;
  struct uv__queue watcher_queue;
  uv__io_cb cb;
  int fd;
  unsigned int events;
  unsigned int pevents;
};
```

**Our Simplified Version**:
- Same structure as libuv
- Same functionality
- Platform-specific polling will be added in Topics 17-21

## Implementation Details

### Watcher Initialization

```c
void io_watcher_init(struct io_watcher* watcher,
                     io_watcher_cb cb,
                     int fd);
```

Initializes a watcher with:
- Callback function
- File descriptor
- Zeroed event masks
- Initialized queue nodes

### Start Watching

```c
int io_watcher_start(struct event_loop* loop,
                     struct io_watcher* watcher,
                     unsigned int events);
```

Starts watching for specified events:
- Adds events to pending events mask
- Adds watcher to watcher queue if not already there
- Will be registered with platform poller in Topics 17-21

### Stop Watching

```c
void io_watcher_stop(struct event_loop* loop,
                     struct io_watcher* watcher,
                     unsigned int events);
```

Stops watching for specified events:
- Removes events from pending events mask
- Removes from watcher queue if no pending events
- Will be unregistered from platform poller in Topics 17-21

### Close Watcher

```c
void io_watcher_close(struct event_loop* loop,
                      struct io_watcher* watcher);
```

Closes watcher completely:
- Stops all events
- Removes from all queues
- Resets file descriptor to -1

## Testing

Run tests:
```bash
cd 06-watcher-structure
gcc -std=c11 -I. -I../01-intrusive-queue/src -I../04-loop-structure/src \
    -o test_io_watcher tests/test_io_watcher.c src/io_watcher.c \
    ../04-loop-structure/src/event_loop.c \
    ../02-min-heap/src/heap.c ../03-time-management/src/time.c && ./test_io_watcher
```

## Example Usage

See `examples/example.c` for a complete example showing:
- Watcher initialization
- Starting/stopping events
- Closing watchers

## Universal Applications

### Where This Pattern is Used

1. **I/O Multiplexing**: epoll, kqueue, select, poll
2. **Networking**: Socket monitoring, connection management
3. **File Systems**: File descriptor tracking, async I/O
4. **OS Kernels**: File descriptor management, I/O event notification
5. **Database Systems**: Connection monitoring, async I/O
6. **Game Engines**: Network I/O, file I/O
7. **Web Servers**: Connection handling, request processing

### Why It's Important

- **Efficient I/O**: Only monitor file descriptors that need events
- **Event-Driven**: Callbacks fired when events occur
- **Queue Management**: Watchers queued for registration/unregistration
- **Platform Abstraction**: Works with different polling mechanisms
- **Resource Management**: Proper cleanup and lifecycle management

## Integration with Event Loop

The watcher structure integrates with the event loop through:
- `watcher_queue`: Queue of watchers to be registered with poller
- `watchers` array: Indexed by file descriptor for O(1) lookup
- `nfds`: Count of active file descriptors

## Next Steps

- Topic 7: Timer Structure (Timer handle with expiry time)
- Topic 17-18: Platform-specific I/O polling (epoll/kqueue)
- Topic 19: Platform abstraction layer
- Topic 20-21: File descriptor management and I/O polling

