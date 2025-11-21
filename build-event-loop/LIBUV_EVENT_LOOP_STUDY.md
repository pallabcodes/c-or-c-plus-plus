# libuv Event Loop Deep Dive Study Guide
## Code-by-Code Analysis of Node.js/libuv Event Loop

---

## 🎯 **Purpose**

This document provides a **code-by-code analysis** of the libuv event loop implementation, with specific file references and line numbers from the Node.js source code.

**Reference**: Node.js source code in `/Users/picon/Learning/c-or-c-plus-plus/node/`

---

## 📋 **Event Loop Overview**

### **Main Function: `uv_run()`**

**Location**: `node/deps/uv/src/unix/core.c`, lines 427-492

**Signature**:
```c
int uv_run(uv_loop_t* loop, uv_run_mode mode)
```

**Modes**:
- `UV_RUN_DEFAULT`: Run until no active handles/requests
- `UV_RUN_ONCE`: Run one iteration
- `UV_RUN_NOWAIT`: Run one iteration without blocking

---

## 🔄 **Event Loop Phases (Detailed)**

### **Phase 0: Initialization**

**Code**: `node/deps/uv/src/unix/core.c`, lines 432-443

```c
r = uv__loop_alive(loop);
if (!r)
  uv__update_time(loop);

/* Maintain backwards compatibility by processing timers before entering the
 * while loop for UV_RUN_DEFAULT. Otherwise timers only need to be executed
 * once, which should be done after polling in order to maintain proper
 * execution order of the conceptual event loop. */
if (mode == UV_RUN_DEFAULT && r != 0 && loop->stop_flag == 0) {
  uv__update_time(loop);
  uv__run_timers(loop);
}
```

**What Happens**:
1. Check if loop is alive (has active handles/requests)
2. Update time if loop is not alive
3. For `UV_RUN_DEFAULT`, run timers before entering main loop (backwards compatibility)

**Key Functions**:
- `uv__loop_alive()`: Checks if loop has active handles/requests/closing handles
- `uv__update_time()`: Updates loop's concept of "now"
- `uv__run_timers()`: Executes due timers

---

### **Phase 1: Main Loop - Pending Callbacks**

**Code**: `node/deps/uv/src/unix/core.c`, line 450

```c
uv__run_pending(loop);
```

**What Happens**:
- Executes callbacks that were deferred from previous iteration
- I/O callbacks are usually called immediately after polling, but some are deferred

**Implementation**: `node/deps/uv/src/unix/core.c`, `uv__run_pending()`

**Key Insight**: Prevents infinite loops by deferring some callbacks

---

### **Phase 2: Idle Handles**

**Code**: `node/deps/uv/src/unix/core.c`, line 451

```c
uv__run_idle(loop);
```

**What Happens**:
- Executes idle handle callbacks
- **Despite the name, idle handles run on EVERY loop iteration** (not just when idle)

**Use Cases**:
- Background tasks that need to run frequently
- Polling operations
- Cleanup tasks

**Implementation**: `node/deps/uv/src/unix/core.c`, `uv__run_idle()`

---

### **Phase 3: Prepare Handles**

**Code**: `node/deps/uv/src/unix/core.c`, line 452

```c
uv__run_prepare(loop);
```

**What Happens**:
- Executes prepare handle callbacks
- Runs **right before** the loop blocks for I/O

**Use Cases**:
- Setup work before blocking
- Pre-I/O operations

**Implementation**: `node/deps/uv/src/unix/core.c`, `uv__run_prepare()`

---

### **Phase 4: Calculate Poll Timeout**

**Code**: `node/deps/uv/src/unix/core.c`, lines 446-456

```c
can_sleep =
    uv__queue_empty(&loop->pending_queue) &&
    uv__queue_empty(&loop->idle_handles);

timeout = 0;
if ((mode == UV_RUN_ONCE && can_sleep) || mode == UV_RUN_DEFAULT)
  timeout = uv__backend_timeout(loop);
```

**What Happens**:
- Calculates how long to block for I/O
- Rules (from `uv__backend_timeout()`):
  - If `UV_RUN_NOWAIT`: timeout = 0
  - If loop stopped: timeout = 0
  - If no active handles: timeout = 0
  - If idle handles active: timeout = 0
  - If handles pending close: timeout = 0
  - Otherwise: timeout = next timer timeout, or infinity

**Implementation**: `node/deps/uv/src/unix/core.c`, `uv__backend_timeout()`

**Key Insight**: Prevents blocking indefinitely when there's work to do

---

### **Phase 5: Block for I/O (Poll)**

**Code**: `node/deps/uv/src/unix/core.c`, line 460

```c
uv__io_poll(loop, timeout);
```

**What Happens**:
- Blocks waiting for I/O events (epoll/kqueue)
- Blocks for `timeout` milliseconds
- When I/O events occur, callbacks are queued

**Platform-Specific**:
- **Linux**: `epoll_wait()` - `node/deps/uv/src/unix/linux-core.c`
- **macOS/BSD**: `kevent()` - `node/deps/uv/src/unix/kqueue.c`
- **Windows**: `GetQueuedCompletionStatus()` - `node/deps/uv/src/win/core.c`

**Implementation**: `node/deps/uv/src/unix/poll.c`, `uv__io_poll()`

**Key Insight**: This is where the loop actually blocks and waits

---

### **Phase 6: Process Immediate Callbacks**

**Code**: `node/deps/uv/src/unix/core.c`, lines 462-465

```c
/* Process immediate callbacks (e.g. write_cb) a small fixed number of
 * times to avoid loop starvation.*/
for (r = 0; r < 8 && !uv__queue_empty(&loop->pending_queue); r++)
  uv__run_pending(loop);
```

**What Happens**:
- Processes pending callbacks up to 8 times
- **Prevents loop starvation**: If callbacks keep adding more callbacks, limit to 8 iterations

**Key Insight**: Prevents infinite callback chains from blocking the loop

---

### **Phase 7: Check Handles**

**Code**: `node/deps/uv/src/unix/core.c`, line 474

```c
uv__run_check(loop);
```

**What Happens**:
- Executes check handle callbacks
- Runs **right after** the loop has blocked for I/O
- Counterpart of prepare handles

**Use Cases**:
- Post-I/O operations
- Cleanup after blocking

**Implementation**: `node/deps/uv/src/unix/core.c`, `uv__run_check()`

---

### **Phase 8: Close Callbacks**

**Code**: `node/deps/uv/src/unix/core.c`, line 475

```c
uv__run_closing_handles(loop);
```

**What Happens**:
- Executes close callbacks for handles that were closed
- Handles are closed with `uv_close()`, but callbacks are deferred

**Implementation**: `node/deps/uv/src/unix/core.c`, `uv__run_closing_handles()`

**Key Insight**: Close callbacks are always deferred to next iteration

---

### **Phase 9: Update Time**

**Code**: `node/deps/uv/src/unix/core.c`, line 477

```c
uv__update_time(loop);
```

**What Happens**:
- Updates loop's concept of "now"
- Used for timer calculations
- Platform-specific (may use `clock_gettime()` or similar)

**Implementation**: `node/deps/uv/src/unix/core.c`, `uv__update_time()`

**Key Insight**: Time is updated once per iteration, after I/O polling

---

### **Phase 10: Run Due Timers**

**Code**: `node/deps/uv/src/unix/core.c`, line 478

```c
uv__run_timers(loop);
```

**What Happens**:
- Executes timers that are due
- Timers are stored in a min-heap
- Extracts timers with `expiry <= now` and executes callbacks

**Implementation**: `node/deps/uv/src/unix/timer.c`, `uv__run_timers()`

**Key Insight**: Timers are checked twice per iteration (beginning and end)

---

### **Phase 11: Check Loop State**

**Code**: `node/deps/uv/src/unix/core.c`, lines 480-482

```c
r = uv__loop_alive(loop);
if (mode == UV_RUN_ONCE || mode == UV_RUN_NOWAIT)
  break;
```

**What Happens**:
- Checks if loop is still alive
- If `UV_RUN_ONCE` or `UV_RUN_NOWAIT`, exit after one iteration
- Otherwise, continue if loop is alive

**Loop is Alive If**:
- Has active and ref'd handles
- Has active requests
- Has closing handles

---

## 📊 **Data Structures**

### **`uv_loop_t` Structure**

**Location**: `node/deps/uv/include/uv/loop.h`

**Key Fields**:
```c
typedef struct uv_loop_s {
  // Timer heap
  void* timer_heap[2];
  
  // Queues
  struct uv__queue wq;                    // Work queue
  struct uv__queue idle_handles;          // Idle handles
  struct uv__queue async_handles;         // Async handles
  struct uv__queue check_handles;         // Check handles
  struct uv__queue prepare_handles;       // Prepare handles
  struct uv__queue handle_queue;          // All handles
  struct uv__queue pending_queue;         // Pending callbacks
  struct uv__queue watcher_queue;         // I/O watchers
  
  // I/O polling
  uv__io_t** watchers;                    // File descriptor watchers
  unsigned int nwatchers;                 // Number of watchers
  unsigned int nfds;                      // Number of file descriptors
  
  // State
  int stop_flag;                          // Stop flag
  unsigned long timer_counter;           // Timer counter
  
  // Platform-specific
  void* backend_fd;                       // epoll/kqueue fd
  // ... more fields
} uv_loop_t;
```

---

### **Timer Heap**

**Location**: `node/deps/uv/src/heap-inl.h`

**Structure**: Min-heap (binary heap)

**Operations**:
- `heap_insert()`: O(log n)
- `heap_extract_min()`: O(log n)
- `heap_min()`: O(1)

**Key Insight**: Timers are stored in a min-heap, sorted by expiry time

---

### **Handle Queues**

**Structure**: Intrusive doubly-linked list (`uv__queue`)

**Location**: `node/deps/uv/src/queue.h`

**Operations**:
- `uv__queue_init()`: Initialize queue
- `uv__queue_insert_tail()`: Add to tail
- `uv__queue_remove()`: Remove from queue
- `uv__queue_empty()`: Check if empty

**Key Insight**: Handles are stored in queues, allowing O(1) insertion/removal

---

## 🔍 **Key Functions to Study**

### **Loop Management**

1. **`uv_loop_init()`** - `node/deps/uv/src/unix/loop.c`, line 30
   - Initializes loop structure
   - Sets up queues, heaps, watchers
   - Platform-specific initialization

2. **`uv_loop_close()`** - `node/deps/uv/src/unix/loop.c`
   - Closes loop and frees resources
   - Ensures all handles are closed

3. **`uv_loop_alive()`** - `node/deps/uv/src/unix/core.c`, line 422
   - Checks if loop has active work

---

### **Handle Execution**

1. **`uv__run_pending()`** - `node/deps/uv/src/unix/core.c`
   - Executes pending callbacks

2. **`uv__run_idle()`** - `node/deps/uv/src/unix/core.c`
   - Executes idle handles

3. **`uv__run_prepare()`** - `node/deps/uv/src/unix/core.c`
   - Executes prepare handles

4. **`uv__run_check()`** - `node/deps/uv/src/unix/core.c`
   - Executes check handles

5. **`uv__run_closing_handles()`** - `node/deps/uv/src/unix/core.c`
   - Executes close callbacks

---

### **Timer Management**

1. **`uv_timer_start()`** - `node/deps/uv/src/timer.c`
   - Starts a timer

2. **`uv__run_timers()`** - `node/deps/uv/src/timer.c`
   - Executes due timers

3. **`uv__update_time()`** - `node/deps/uv/src/unix/core.c`
   - Updates loop time

---

### **I/O Polling**

1. **`uv__io_poll()`** - `node/deps/uv/src/unix/poll.c`
   - Blocks for I/O events
   - Platform-specific (epoll/kqueue)

2. **`uv__backend_timeout()`** - `node/deps/uv/src/unix/core.c`
   - Calculates poll timeout

---

## 📝 **Study Exercises**

### **Exercise 1: Trace Execution**

1. Create a simple program with:
   - One timer
   - One TCP server
   - One idle handle

2. Trace through `uv_run()`:
   - Which phases execute?
   - In what order?
   - What callbacks are called?

---

### **Exercise 2: Understand Timer Execution**

1. Create multiple timers with different delays
2. Trace timer heap operations:
   - When are timers inserted?
   - When are timers extracted?
   - How is the next timeout calculated?

---

### **Exercise 3: Understand I/O Polling**

1. Create a TCP server
2. Trace I/O polling:
   - When does `epoll_wait()`/`kevent()` block?
   - How long does it block?
   - What happens when events occur?

---

### **Exercise 4: Understand Handle Lifecycle**

1. Create handles of different types
2. Trace lifecycle:
   - Initialization
   - Activation
   - Execution
   - Deactivation
   - Closing

---

## 🎯 **Implementation Checklist**

When building your own event loop:

- [ ] Loop structure (`event_loop_t`)
- [ ] Loop initialization
- [ ] Timer heap
- [ ] Handle queues (idle, prepare, check, pending, closing)
- [ ] I/O watchers (epoll/kqueue)
- [ ] Poll timeout calculation
- [ ] Handle execution (all phases)
- [ ] Timer execution
- [ ] I/O event handling
- [ ] Close callback handling

---

## 📚 **Next Steps**

1. **Read the code**: Start with `uv_run()` in `core.c`
2. **Trace execution**: Follow one iteration through all phases
3. **Understand data structures**: Study `uv_loop_t` and queues
4. **Implement minimal version**: Build simplified event loop
5. **Add features**: Incrementally add timers, I/O, handles

**Remember**: Understanding comes from reading code, not just documentation!

---

## 🔗 **Related Files**

- **Loop initialization**: `node/deps/uv/src/unix/loop.c`
- **Core loop**: `node/deps/uv/src/unix/core.c`
- **Timer management**: `node/deps/uv/src/timer.c`
- **I/O polling**: `node/deps/uv/src/unix/poll.c`
- **epoll**: `node/deps/uv/src/unix/linux-core.c`
- **kqueue**: `node/deps/uv/src/unix/kqueue.c`
- **Queue implementation**: `node/deps/uv/src/queue.h`
- **Heap implementation**: `node/deps/uv/src/heap-inl.h`

---

**Happy Learning! 🎓**

