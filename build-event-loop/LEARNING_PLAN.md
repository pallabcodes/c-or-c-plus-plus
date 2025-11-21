# Event Loop Learning Plan
## Deep Dive into Node.js/libuv Event Loop Internals

---

## 🎯 **Learning Objective**

**Goal**: Understand event loop internals thoroughly by studying Node.js/libuv implementation, then build simplified versions to demonstrate understanding.

**Not a Product**: This is a learning project to understand how event loops work internally, not to build a production system.

**Outcome**: Be able to implement your own event loop (fully or minimally) with deep understanding of each component.

---

## 📚 **Reference Implementation: Node.js/libuv**

### **Why Node.js/libuv?**

- ✅ **Production-grade**: Used by millions of applications
- ✅ **Well-documented**: Extensive documentation and source code
- ✅ **Cross-platform**: Works on Linux, macOS, Windows
- ✅ **Complete**: Handles all event loop phases, timers, I/O, handles
- ✅ **Open source**: Full source code available for study

### **Key Files to Study**

**libuv Core:**
- `node/deps/uv/src/unix/core.c` - Main event loop (`uv_run()`)
- `node/deps/uv/src/unix/loop.c` - Loop initialization and management
- `node/deps/uv/src/unix/poll.c` - I/O polling (epoll/kqueue)
- `node/deps/uv/src/unix/timer.c` - Timer management
- `node/deps/uv/include/uv.h` - Public API

**Node.js Integration:**
- `node/src/api/embed_helpers.cc` - Node.js event loop wrapper
- `node/src/env.cc` - Environment setup

**Documentation:**
- `node/deps/uv/docs/src/design.rst` - Architecture overview
- `node/deps/uv/docs/src/loop.rst` - Loop API documentation

---

## 🗺️ **Learning Path**

### **Phase 1: Understanding the Event Loop Structure** (Week 1)

#### **1.1 Study libuv Event Loop Phases**

**Reference**: `node/deps/uv/docs/src/design.rst` (lines 55-112)

**What to Learn:**
1. **Loop Initialization** (`uv_loop_init`)
   - Data structures: queues, heaps, watchers
   - Platform-specific initialization (epoll/kqueue)
   - Timer heap initialization

2. **Loop Iteration Phases** (`uv_run`)
   - Phase 1: Update time
   - Phase 2: Run due timers
   - Phase 3: Run pending callbacks
   - Phase 4: Run idle handles
   - Phase 5: Run prepare handles
   - Phase 6: Calculate poll timeout
   - Phase 7: Block for I/O (epoll/kqueue)
   - Phase 8: Run check handles
   - Phase 9: Run close callbacks
   - Phase 10: Update time again
   - Phase 11: Run due timers again

**Study Files:**
- `node/deps/uv/src/unix/core.c` - `uv_run()` function (lines 427-492)
- `node/deps/uv/src/unix/loop.c` - `uv_loop_init()` function

**Deliverable**: Document each phase with code references and explanations

---

#### **1.2 Understand Data Structures**

**What to Learn:**
- `uv_loop_t` structure (loop state)
- `uv__queue` (intrusive doubly-linked list)
- Timer heap (min-heap for timers)
- Handle queues (idle, prepare, check, pending, closing)
- Watcher arrays (file descriptor tracking)

**Study Files:**
- `node/deps/uv/include/uv/loop.h` - Loop structure definition
- `node/deps/uv/src/queue.h` - Queue implementation

**Deliverable**: Create diagrams/documentation of data structures

---

### **Phase 2: Platform-Specific I/O Polling** (Week 1-2)

#### **2.1 Study epoll (Linux)**

**What to Learn:**
- How epoll works (edge vs level triggered)
- `epoll_create`, `epoll_ctl`, `epoll_wait`
- File descriptor management
- Event notification mechanism

**Study Files:**
- `node/deps/uv/src/unix/linux-core.c` - epoll implementation
- `node/deps/uv/src/unix/poll.c` - Poll abstraction

**Deliverable**: Implement minimal epoll-based event loop

---

#### **2.2 Study kqueue (macOS/BSD)**

**What to Learn:**
- How kqueue works
- `kqueue()`, `kevent()` system calls
- Event filtering and notification
- Differences from epoll

**Study Files:**
- `node/deps/uv/src/unix/kqueue.c` - kqueue implementation

**Deliverable**: Implement minimal kqueue-based event loop

---

### **Phase 3: Timer Management** (Week 2)

#### **3.1 Study Timer Heap**

**What to Learn:**
- Min-heap data structure for timers
- Timer insertion and extraction
- Finding next timer timeout
- Timer callback execution

**Study Files:**
- `node/deps/uv/src/timer.c` - Timer implementation
- `node/deps/uv/src/heap-inl.h` - Heap implementation

**Deliverable**: Implement timer heap and integrate with event loop

---

#### **3.2 Study Timer Wheel (Advanced)**

**What to Learn:**
- Hierarchical timer wheel for O(1) operations
- Timer wheel vs heap trade-offs
- When to use each approach

**Reference**: Research papers on timer wheels

**Deliverable**: Implement timer wheel variant

---

### **Phase 4: Handle Management** (Week 2-3)

#### **4.1 Study Handle Types**

**What to Learn:**
- **Idle handles**: Run on every loop iteration
- **Prepare handles**: Run before blocking for I/O
- **Check handles**: Run after blocking for I/O
- **Timer handles**: Scheduled callbacks
- **I/O handles**: File descriptors (TCP, UDP, pipes)

**Study Files:**
- `node/deps/uv/src/unix/core.c` - Handle execution functions
  - `uv__run_idle()`
  - `uv__run_prepare()`
  - `uv__run_check()`
  - `uv__run_pending()`
  - `uv__run_closing()`

**Deliverable**: Implement handle registration and execution

---

#### **4.2 Study Handle Lifecycle**

**What to Learn:**
- Handle initialization
- Handle activation/deactivation
- Handle closing (deferred callbacks)
- Handle reference counting

**Study Files:**
- `node/deps/uv/src/unix/core.c` - Handle management
- `node/deps/uv/include/uv.h` - Handle API

**Deliverable**: Implement handle lifecycle management

---

### **Phase 5: I/O Operations** (Week 3)

#### **5.1 Study Non-Blocking I/O**

**What to Learn:**
- Non-blocking sockets
- `EAGAIN`/`EWOULDBLOCK` handling
- Read/write operations
- Connection management

**Study Files:**
- `node/deps/uv/src/unix/stream.c` - Stream I/O
- `node/deps/uv/src/unix/tcp.c` - TCP implementation

**Deliverable**: Implement TCP server with event loop

---

#### **5.2 Study File I/O (Thread Pool)**

**What to Learn:**
- Why file I/O uses thread pool (blocking operations)
- Thread pool architecture
- Work queue management
- Completion callbacks

**Study Files:**
- `node/deps/uv/src/threadpool.c` - Thread pool
- `node/deps/uv/src/unix/fs.c` - File I/O

**Deliverable**: Document thread pool integration (no implementation needed)

---

### **Phase 6: Build Simplified Event Loop** (Week 3-4)

#### **6.1 Minimal Event Loop**

**Features:**
- Single-threaded event loop
- epoll/kqueue I/O polling
- Timer heap
- Basic handle types (idle, prepare, check)
- Pending callback queue

**Implementation Steps:**
1. Loop structure (`event_loop_t`)
2. Loop initialization
3. Handle registration
4. Timer management
5. I/O polling
6. Handle execution
7. Loop iteration

**Deliverable**: Working minimal event loop

---

#### **6.2 Add Features Incrementally**

**Add in Order:**
1. TCP server support
2. UDP support
3. Signal handling
4. Process management
5. File I/O (thread pool)

**Deliverable**: Feature-complete event loop

---

## 📖 **Study Methodology**

### **For Each Component:**

1. **Read the Code**
   - Read libuv source code
   - Understand data structures
   - Trace execution flow

2. **Read Documentation**
   - Read libuv design docs
   - Read Node.js event loop docs
   - Read research papers (if applicable)

3. **Write Documentation**
   - Document how it works
   - Document data structures
   - Document algorithms
   - Document trade-offs

4. **Implement Simplified Version**
   - Implement minimal version
   - Test with simple examples
   - Compare with libuv behavior

5. **Integrate**
   - Integrate with event loop
   - Test end-to-end
   - Debug and fix issues

---

## 📝 **Documentation Template**

For each component, create:

```markdown
# Component Name (e.g., Timer Heap)

## Overview
What this component does

## libuv Implementation
- File: `node/deps/uv/src/timer.c`
- Key functions: `uv_timer_start()`, `uv__run_timers()`
- Data structure: Min-heap

## How It Works
1. Step 1
2. Step 2
3. Step 3

## Data Structures
```c
struct timer_heap {
    // ...
};
```

## Algorithms
- Insertion: O(log n)
- Extraction: O(log n)
- Finding next: O(1)

## Trade-offs
- Pros: Simple, predictable
- Cons: O(log n) operations

## Implementation
[Your simplified implementation]

## Testing
[Test cases]
```

---

## 🧪 **Testing Strategy**

### **Unit Tests**
- Test each component in isolation
- Test data structures (heap, queues)
- Test handle lifecycle

### **Integration Tests**
- Test event loop phases
- Test timer execution
- Test I/O operations
- Test handle callbacks

### **Example Programs**
- Echo server
- HTTP server
- Timer-based scheduler
- File watcher

---

## 📚 **Resources**

### **libuv Source Code**
- Repository: `node/deps/uv/`
- Key files listed above

### **Documentation**
- libuv Design: `node/deps/uv/docs/src/design.rst`
- libuv API: `node/deps/uv/docs/src/loop.rst`
- Node.js Event Loop: Node.js official docs

### **Research Papers**
- "The Design and Implementation of libuv" (if available)
- Timer wheel papers
- Event-driven I/O papers

### **Books**
- "Node.js Design Patterns" - Event loop chapter
- "Linux System Programming" - epoll chapter

---

## ✅ **Learning Checklist**

### **Phase 1: Understanding**
- [ ] Understand loop initialization
- [ ] Understand all 11 phases of loop iteration
- [ ] Understand data structures
- [ ] Document each phase

### **Phase 2: I/O Polling**
- [ ] Understand epoll
- [ ] Understand kqueue
- [ ] Implement minimal epoll loop
- [ ] Implement minimal kqueue loop

### **Phase 3: Timers**
- [ ] Understand timer heap
- [ ] Understand timer wheel
- [ ] Implement timer heap
- [ ] Integrate with event loop

### **Phase 4: Handles**
- [ ] Understand handle types
- [ ] Understand handle lifecycle
- [ ] Implement handle management
- [ ] Implement handle execution

### **Phase 5: I/O**
- [ ] Understand non-blocking I/O
- [ ] Understand thread pool (file I/O)
- [ ] Implement TCP server
- [ ] Document file I/O approach

### **Phase 6: Implementation**
- [ ] Implement minimal event loop
- [ ] Add TCP support
- [ ] Add UDP support
- [ ] Add signal handling
- [ ] Test end-to-end

---

## 🎯 **Success Criteria**

**You've successfully learned event loops when:**

1. ✅ **You can explain** each phase of the event loop
2. ✅ **You understand** how epoll/kqueue works
3. ✅ **You understand** timer management (heap/wheel)
4. ✅ **You understand** handle lifecycle
5. ✅ **You can implement** a minimal event loop
6. ✅ **You can extend** it with new features
7. ✅ **You can debug** event loop issues
8. ✅ **You can optimize** event loop performance

---

## 🚀 **Next Steps**

1. **Start with Phase 1**: Study libuv event loop phases
2. **Read the code**: `node/deps/uv/src/unix/core.c`
3. **Document understanding**: Create notes for each phase
4. **Implement minimal version**: Build simplified event loop
5. **Iterate**: Add features incrementally

**Remember**: This is a learning project. Take your time to understand each component deeply before moving to the next.

---

## 📝 **Notes**

- **Don't rush**: Understanding is more important than speed
- **Read code carefully**: libuv code is well-written and educational
- **Experiment**: Try different approaches, break things, learn
- **Document**: Write down what you learn
- **Ask questions**: If something is unclear, dig deeper

**Good luck! 🎓**

