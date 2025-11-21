# Using libuv Files Directly from Node.js Source

This document explains how to use files from `node/deps/uv/` directly in `build-event-loop` instead of reimplementing them.

## Setup

The CMake configuration has been updated to include libuv paths. The following directories are available:

- `node/deps/uv/include/` - Public libuv headers
- `node/deps/uv/src/` - libuv source files
- `node/deps/uv/src/unix/` - Unix-specific implementations

## Usage Options

### Option 1: Direct Include (Recommended)

Simply include the libuv header directly:

```c
#include "queue.h"  // Uses libuv's queue.h from node/deps/uv/src/
```

**Note**: libuv uses `uv__queue` naming, so you'll use `uv__queue_*` functions.

### Option 2: Compatibility Wrapper

Use the compatibility wrapper to maintain your existing naming:

```c
#include "queue_libuv.h"  // Wrapper that maps uv__queue to queue
```

This allows you to use `queue_*` functions while using libuv's implementation.

### Option 3: Conditional Compilation

You can choose at compile time:

```c
#ifdef USE_LIBUV_QUEUE
  #include "queue_libuv.h"
#else
  #include "queue.h"  // Your local implementation
#endif
```

## Available libuv Files

### Core Data Structures

- **queue.h** - Intrusive doubly-linked list
  - Location: `node/deps/uv/src/queue.h`
  - Functions: `uv__queue_init`, `uv__queue_insert_head`, `uv__queue_remove`, etc.

- **heap-inl.h** - Min-heap implementation
  - Location: `node/deps/uv/src/heap-inl.h`
  - Used for timer management

### Timer Management

- **timer.c** - Timer implementation
  - Location: `node/deps/uv/src/timer.c`
  - Note: This is a `.c` file, so you'd need to link it or extract header parts

### Platform-Specific

- **unix/poll.c** - Poll abstraction (epoll/kqueue)
- **unix/core.c** - Core event loop
- **unix/loop.c** - Loop initialization

## Example: Using libuv Queue

### Before (Local Implementation)

```c
#include "queue.h"  // Your local queue.h

struct my_struct {
    int data;
    struct queue q;
};

struct queue head;
queue_init(&head);

struct my_struct item;
queue_insert_tail(&head, &item.q);
```

### After (Using libuv)

```c
#include "queue_libuv.h"  // Wrapper, or use queue.h directly

struct my_struct {
    int data;
    struct queue q;  // Now maps to uv__queue
};

struct queue head;
queue_init(&head);  // Maps to uv__queue_init

struct my_struct item;
queue_insert_tail(&head, &item.q);  // Maps to uv__queue_insert_tail
```

## CMake Configuration

The main `CMakeLists.txt` automatically sets up include paths:

```cmake
include_directories(
  ${LIBUV_INCLUDE_DIR}      # node/deps/uv/include
  ${LIBUV_SRC_DIR}          # node/deps/uv/src
  ${LIBUV_SRC_DIR}/unix     # node/deps/uv/src/unix
)
```

Subdirectories automatically inherit these paths.

## Benefits

1. **No Reimplementation**: Use battle-tested libuv code
2. **Consistency**: Same implementation Node.js uses
3. **Learning**: Study the actual production code
4. **Maintenance**: Less code to maintain

## Considerations

1. **Naming**: libuv uses `uv__` prefix for internal APIs
2. **Dependencies**: Some libuv files depend on others (check includes)
3. **Platform**: Unix-specific files are in `unix/` subdirectory
4. **License**: libuv uses ISC license (compatible with most projects)

## Migration Path

1. Start by using `queue.h` (simplest)
2. Gradually replace other components as needed
3. Keep your implementations as reference/learning material
4. Document which files come from libuv vs. your own

## Files You Can Use

### Headers (Safe to Include)
- `queue.h` - Queue implementation
- `heap-inl.h` - Heap implementation
- `uv-common.h` - Common utilities
- `unix/internal.h` - Internal definitions (if needed)

### Source Files (Need Linking)
- `timer.c` - Timer implementation
- `unix/core.c` - Event loop core
- `unix/poll.c` - Poll abstraction

For source files, you can either:
1. Link them as separate compilation units
2. Extract needed parts into headers
3. Study them but implement your own version

## Example CMakeLists.txt for Subdirectory

```cmake
cmake_minimum_required(VERSION 3.15)
project(my_project C)

add_executable(my_project main.c)

# Include paths are already set by parent CMakeLists.txt
# Just include the headers you need:
# #include "queue.h"  // libuv's queue.h
```

## Troubleshooting

### Issue: "queue.h not found"
- Ensure CMake paths are set correctly
- Check that `node/deps/uv/src/queue.h` exists
- Verify include paths in CMake configuration

### Issue: "uv__queue undefined"
- Use `queue_libuv.h` wrapper, or
- Use `uv__queue` naming directly

### Issue: Platform-specific code
- Unix code is in `unix/` subdirectory
- May need platform detection in CMake
- Consider using libuv's platform abstraction

## References

- libuv source: `node/deps/uv/src/`
- libuv headers: `node/deps/uv/include/`
- Your local implementations: `build-event-loop/learning/` (for reference)

