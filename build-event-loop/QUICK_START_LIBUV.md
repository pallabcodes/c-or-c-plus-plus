# Quick Start: Using libuv Files from Node.js

## Setup Complete ✅

The CMake configuration now automatically includes libuv paths. You can use files from `node/deps/uv/` directly!

## Three Ways to Use libuv Files

### Method 1: Direct Include (Simplest)

Just include the libuv header directly:

```c
#include "uv.h"      // For struct definitions
#include "queue.h"   // libuv's queue.h from node/deps/uv/src/

struct my_struct {
    int data;
    struct uv__queue q;  // Use uv__queue
};

struct uv__queue head;
uv__queue_init(&head);
uv__queue_insert_tail(&head, &item.q);
```

### Method 2: Compatibility Wrapper (Recommended)

Use the wrapper to keep your existing code style:

```c
#include "queue_libuv.h"  // Wrapper that maps uv__queue to queue

struct my_struct {
    int data;
    struct queue q;  // Now you can use "queue" instead of "uv__queue"
};

struct queue head;
queue_init(&head);  // Maps to uv__queue_init
queue_insert_tail(&head, &item.q);
```

### Method 3: Conditional Compilation

Choose at compile time:

```c
#ifdef USE_LIBUV_QUEUE
  #include "queue_libuv.h"
#else
  #include "queue.h"  // Your local implementation
#endif
```

Then compile with: `gcc -DUSE_LIBUV_QUEUE ...`

## Available Files

### Headers You Can Include Directly

- `queue.h` - Intrusive doubly-linked list
- `heap-inl.h` - Min-heap for timers
- `uv.h` - Main libuv header (defines struct uv__queue)

### Example: Using libuv Queue

See: `learning/01-intrusive-queue/examples/example_libuv.c`

## CMake Integration

All subdirectories automatically have access to:

- `${LIBUV_INCLUDE_DIR}` → `node/deps/uv/include`
- `${LIBUV_SRC_DIR}` → `node/deps/uv/src`
- `${LIBUV_SRC_DIR}/unix` → `node/deps/uv/src/unix`

Just include the headers you need!

## Migration Example

### Before (Local Implementation)

```c
#include "../src/queue.h"  // Your local queue.h

struct test_item {
    int value;
    struct queue q;
};

queue_init(&head);
queue_insert_tail(&head, &item.q);
```

### After (Using libuv)

```c
#include "queue_libuv.h"  // Wrapper

struct test_item {
    int value;
    struct queue q;  // Same code, uses libuv underneath!
};

queue_init(&head);  // Same API
queue_insert_tail(&head, &item.q);
```

## Benefits

1. ✅ **No reimplementation** - Use production-tested code
2. ✅ **Same API** - Wrapper maintains your naming
3. ✅ **Learning** - Study actual libuv implementation
4. ✅ **Consistency** - Same code Node.js uses

## Next Steps

1. Try the example: `learning/01-intrusive-queue/examples/example_libuv.c`
2. Read: `USING_LIBUV_FILES.md` for detailed documentation
3. Gradually migrate components as needed

