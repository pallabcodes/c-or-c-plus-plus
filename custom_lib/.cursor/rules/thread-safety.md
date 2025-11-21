# Thread Safety Standards

## Overview
Thread safety is critical for library functions that may be called from multiple threads concurrently. This document defines standards for implementing thread safe custom library functions with proper synchronization and performance considerations.

## Thread Safety Levels

### Thread Safe Functions
* **Definition**: Functions that can be called concurrently from multiple threads safely
* **Requirements**: No data races, correct synchronization, proper memory ordering
* **Implementation**: Use locks, atomics, or thread local storage
* **Rationale**: Enable concurrent usage without data corruption

### Reentrant Functions
* **Definition**: Functions that can be called concurrently from same or different threads
* **Requirements**: No shared mutable state, thread local storage, or proper synchronization
* **Implementation**: Pure functions, thread local storage, or synchronized access
* **Rationale**: Enable safe recursive or concurrent calls

### Not Thread Safe Functions
* **Definition**: Functions that are not safe for concurrent calls
* **Requirements**: Must be documented clearly
* **Usage**: Single threaded contexts or caller provided synchronization
* **Rationale**: Simpler implementation, better performance

## Synchronization Primitives

### Mutexes
* **Usage**: Protect shared mutable state
* **Lock ordering**: Establish consistent lock ordering to prevent deadlocks
* **Lock duration**: Minimize lock duration
* **Rationale**: Prevent data races and ensure correctness

### Atomic Operations
* **Usage**: Lock free algorithms, simple shared state
* **Memory ordering**: Use appropriate memory ordering (acquire, release, seq_cst)
* **Performance**: Better performance than mutexes for simple operations
* **Rationale**: Avoid lock overhead for simple operations

### Thread Local Storage
* **Usage**: Per thread state, avoid shared state
* **Performance**: No synchronization overhead
* **Memory**: Each thread has its own copy
* **Rationale**: Eliminate need for synchronization

## Implementation Patterns

### Thread Safe Buffer
```c
typedef struct {
    char data[BUFFER_SIZE];
    size_t pos;
    pthread_mutex_t mutex;
} thread_safe_buffer_t;

// Thread safety: Thread safe (uses mutex)
// Ownership: Caller owns buf
// Invariants: buf must not be NULL
int buffer_init_thread_safe(thread_safe_buffer_t *buf) {
    if (!buf) {
        return -1;
    }
    
    buf->pos = 0;
    return pthread_mutex_init(&buf->mutex, NULL);
}
```

### Thread Local Buffer
```c
// Thread local storage for per thread buffer
static __thread buffer_t thread_buffer;

// Thread safety: Thread safe (thread local storage)
// Ownership: Thread owns buffer
// Invariants: None (initialized on first use)
buffer_t *get_thread_buffer(void) {
    if (thread_buffer.pos == 0 && thread_buffer.data[0] == 0) {
        buffer_init(&thread_buffer);
    }
    return &thread_buffer;
}
```

### Lock Free Counter
```c
// Thread safety: Thread safe (atomic operations)
// Ownership: Shared counter
// Invariants: None
static atomic_size_t write_count = ATOMIC_VAR_INIT(0);

void increment_write_count(void) {
    atomic_fetch_add(&write_count, 1);
}
```

## Error Handling in Thread Safe Code

### Lock Acquisition Errors
* **EINVAL**: Invalid mutex
* **EDEADLK**: Deadlock detected
* **Error handling**: Return error, don't proceed
* **Rationale**: Prevent undefined behavior

### Lock Release Errors
* **EINVAL**: Invalid mutex or not owned
* **Error handling**: Log error, attempt recovery
* **Rationale**: Prevent resource leaks

## Performance Considerations

### Lock Contention
* **Minimize critical sections**: Keep locked code minimal
* **Lock free algorithms**: Use lock free algorithms where possible
* **Lock granularity**: Use fine grained locks
* **Rationale**: Reduce lock contention and improve performance

### Lock Free Alternatives
* **Atomics**: Use atomic operations for simple shared state
* **Thread local**: Use thread local storage to avoid shared state
* **Read copy update**: Use RCU for read mostly data
* **Rationale**: Better performance than locks

### Benchmarking
* **Throughput**: Measure operations per second
* **Latency**: Measure operation latency
* **Contention**: Measure lock contention
* **Scalability**: Measure performance with multiple threads

## Common Pitfalls

### Data Races
* **Problem**: Concurrent access to shared mutable state without synchronization
* **Solution**: Use locks, atomics, or thread local storage
* **Detection**: Use thread sanitizer (TSAN)
* **Rationale**: Data races cause undefined behavior

### Deadlocks
* **Problem**: Circular lock dependencies
* **Solution**: Establish consistent lock ordering
* **Detection**: Use deadlock detection tools
* **Rationale**: Deadlocks cause hangs

### Race Conditions
* **Problem**: Timing dependent bugs
* **Solution**: Proper synchronization and memory ordering
* **Detection**: Stress testing, fuzzing
* **Rationale**: Race conditions cause incorrect behavior

## Testing Requirements

### Unit Tests
* **Single thread**: Test single threaded correctness
* **Multiple threads**: Test concurrent correctness
* **Error cases**: Test error handling
* **Rationale**: Ensure correctness

### Concurrency Tests
* **Race conditions**: Test for race conditions
* **Deadlocks**: Test for deadlocks
* **Stress tests**: High concurrency stress tests
* **Rationale**: Find concurrency bugs

### Thread Sanitizer
* **Enable TSAN**: Use thread sanitizer in tests
* **Fix issues**: Fix all TSAN reported issues
* **CI integration**: Run TSAN in CI
* **Rationale**: Detect data races and deadlocks

## Documentation Requirements

### Thread Safety Guarantees
* **Document level**: Document thread safety level for each function
* **Synchronization**: Document synchronization mechanisms
* **Shared state**: Document shared state and protection
* **Rationale**: Enable correct usage

### Usage Examples
* **Single threaded**: Show single threaded usage
* **Multi threaded**: Show multi threaded usage
* **Error handling**: Show error handling
* **Rationale**: Clarify usage patterns

## Research Papers and References

### Thread Safety
* "Thread Safety in C Libraries" (USENIX) - Reentrant library design
* "Lock Free Programming" - Concurrent data structures
* "Memory Ordering in Modern Microprocessors" - Memory consistency

### Open Source References
* glibc thread safety implementation
* musl libc thread safety implementation
* Google Abseil thread safety patterns

## Implementation Checklist

- [ ] Identify shared mutable state
- [ ] Choose synchronization mechanism (mutex, atomic, thread local)
- [ ] Implement thread safe operations
- [ ] Add error handling for synchronization
- [ ] Write unit tests
- [ ] Write concurrency tests
- [ ] Run thread sanitizer
- [ ] Benchmark performance
- [ ] Document thread safety guarantees
- [ ] Document usage examples

