# Topic 5: Handle Structure

## What

Base structure for all handle types in the event loop. All handle types (idle, prepare, check, timer, I/O) inherit from this base structure.

## Why

Provides polymorphic handle management - a single structure that can represent different handle types while maintaining common functionality (lifecycle, callbacks, queue linkage).

## Where Used

- All handle types inherit from this base structure
- Handle lifecycle management
- Handle queue management
- Callback management

## Universal Use

- **Object-Oriented Systems**: Base class pattern for polymorphism
- **Plugin Architectures**: Common interface for different plugin types
- **Resource Management**: RAII patterns, resource tracking
- **Event Systems**: Common structure for different event sources
- **OS Kernels**: Process/file descriptor management
- **Database Systems**: Connection/transaction handle management

## Data Structures

### Handle Structure

```c
struct handle {
  /* Public fields */
  void* data;                    /* User data pointer */
  
  /* Read-only fields */
  struct event_loop* loop;       /* Pointer to event loop */
  handle_type_t type;            /* Handle type */
  
  /* Private fields */
  handle_close_cb close_cb;      /* Close callback */
  struct queue handle_queue;     /* Queue node for linking handles */
  
  /* File descriptor or reserved space */
  union {
    int fd;                      /* File descriptor (for I/O handles) */
    void* reserved[4];           /* Reserved space (for non-I/O handles) */
  } u;
  
  /* Flags */
  unsigned int flags;            /* Internal flags */
};
```

### Handle Type Enumeration

```c
typedef enum {
  HANDLE_TYPE_UNKNOWN = 0,
  HANDLE_TYPE_IDLE,
  HANDLE_TYPE_PREPARE,
  HANDLE_TYPE_CHECK,
  HANDLE_TYPE_TIMER,
  HANDLE_TYPE_IO,
  HANDLE_TYPE_MAX
} handle_type_t;
```

### Handle Flags

- `HANDLE_FLAG_ACTIVE`: Handle is active (in a queue)
- `HANDLE_FLAG_CLOSING`: Handle is being closed
- `HANDLE_FLAG_CLOSED`: Handle is closed

## Algorithms

### Handle Initialization
- **Time Complexity**: O(1)
- **Space Complexity**: O(1)
- Initialize all fields, set up queue node

### Handle Activation/Deactivation
- **Time Complexity**: O(1)
- **Space Complexity**: O(1)
- Set/unset active flag

### Handle Lifecycle
- **Time Complexity**: O(1) per operation
- **Space Complexity**: O(1)
- Track handle state through flags

## libuv Implementation

### Source Files

- **Structure Definition**: `node/deps/uv/include/uv.h` (lines 465-483)
- **Handle Fields Macro**: `node/deps/uv/include/uv.h` (lines 465-478)
- **Handle API**: `node/deps/uv/include/uv.h` (lines 485-506)

### Key Differences from libuv

**libuv's Structure**:
```c
#define UV_HANDLE_FIELDS                                                      \
  void* data;                                                                 \
  uv_loop_t* loop;                                                            \
  uv_handle_type type;                                                        \
  uv_close_cb close_cb;                                                       \
  struct uv__queue handle_queue;                                              \
  union {                                                                     \
    int fd;                                                                   \
    void* reserved[4];                                                        \
  } u;                                                                        \
  UV_HANDLE_PRIVATE_FIELDS                                                    \

struct uv_handle_s {
  UV_HANDLE_FIELDS
};
```

**Our Simplified Version**:
- Simplified flags (single `flags` field instead of platform-specific private fields)
- Direct structure instead of macro-based
- Same core fields and functionality

### libuv Handle Types

libuv supports many handle types:
- `UV_IDLE` - Idle handle
- `UV_PREPARE` - Prepare handle
- `UV_CHECK` - Check handle
- `UV_TIMER` - Timer handle
- `UV_TCP` - TCP handle
- `UV_UDP` - UDP handle
- `UV_PIPE` - Pipe handle
- `UV_POLL` - Poll handle
- `UV_SIGNAL` - Signal handle
- `UV_PROCESS` - Process handle
- `UV_FS_EVENT` - File system event handle
- `UV_FS_POLL` - File system poll handle
- `UV_ASYNC` - Async handle

We implement a subset for learning purposes.

## Implementation Details

### Handle Initialization

```c
void handle_init(struct handle* handle,
                 struct event_loop* loop,
                 handle_type_t type);
```

Initializes a handle with:
- Loop pointer
- Handle type
- Zeroed fields
- Initialized queue node
- File descriptor set to -1

### Handle State Management

```c
int handle_is_active(const struct handle* handle);
void handle_set_active(struct handle* handle);
void handle_set_inactive(struct handle* handle);
```

Manages the active state of handles. Active handles are in queues and will be processed by the event loop.

### Handle Closing

```c
void handle_start_closing(struct handle* handle, handle_close_cb close_cb);
```

Starts the closing process for a handle. The close callback will be called when the handle is fully closed.

### Handle Data Access

```c
void* handle_get_data(const struct handle* handle);
void handle_set_data(struct handle* handle, void* data);
```

Provides access to user data stored in the handle.

## Testing

Run tests:
```bash
cd 05-handle-structure
gcc -std=c11 -I. -I../01-intrusive-queue/src -I../04-loop-structure/src \
    -o test_handle tests/test_handle.c src/handle.c \
    ../04-loop-structure/src/event_loop.c \
    ../02-min-heap/src/heap.c ../03-time-management/src/time.c && ./test_handle
```

## Example Usage

See `examples/example.c` for a complete example showing:
- Handle initialization
- Setting user data
- Activating handles
- Closing handles

## Universal Applications

### Where This Pattern is Used

1. **Object-Oriented Programming**: Base class for polymorphism
2. **Plugin Systems**: Common interface for plugins
3. **Resource Management**: RAII patterns, smart pointers
4. **Event Systems**: Common structure for event sources
5. **OS Kernels**: Process/file descriptor handles
6. **Database Systems**: Connection handles
7. **Graphics Systems**: Window/context handles
8. **Networking Stacks**: Socket/connection handles

### Why It's Important

- **Polymorphism**: Single interface for different types
- **Lifecycle Management**: Consistent resource management
- **Queue Integration**: Handles can be linked in queues
- **Callback Support**: Unified callback mechanism
- **Type Safety**: Type information for debugging and validation

## Next Steps

- Topic 6: Watcher Structure (I/O watcher for file descriptors)
- Topic 7: Timer Structure (Timer handle with expiry time)
- Topic 11-15: Specific handle types (idle, prepare, check, etc.)

