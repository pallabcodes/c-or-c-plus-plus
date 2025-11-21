# Thread Management

## Scope
Applies to thread creation, lifecycle management, thread pools, thread local storage, and concurrent programming patterns.

## Thread Creation

### pthread_create()
* Always check pthread_create() return value
* Pass correct function signatures (void* (*)(void*))
* Handle thread creation errors appropriately
* Document thread function responsibilities

### Thread Attributes
* Use pthread_attr_t for custom thread attributes
* Set stack size appropriately (default may be too large)
* Configure detach state (joinable vs detached)
* Set scheduling policy and priority when needed
* Clean up thread attributes with pthread_attr_destroy()

## Thread Lifecycle

### Thread Joining
* Use pthread_join() for joinable threads
* Always join threads or detach them
* Handle join errors appropriately
* Pass return values via pthread_exit() and pthread_join()

### Thread Detachment
* Use pthread_detach() for fire and forget threads
* Detach threads that don't need to be joined
* Document thread lifetime and ownership
* Avoid accessing detached thread resources

### Thread Cancellation
* Use pthread_cancel() carefully
* Set cancellation points appropriately
* Use pthread_setcancelstate() and pthread_setcanceltype()
* Clean up resources in cancellation handlers
* Prefer cooperative cancellation over asynchronous

## Thread Local Storage

### Thread Specific Data
* Use pthread_key_create() for thread specific keys
* Use pthread_setspecific() and pthread_getspecific()
* Provide destructor functions for cleanup
* Document thread local data usage

### C++ thread_local
* Prefer C++11 thread_local for C++ code
* Understand initialization semantics
* Document thread local variable usage
* Consider performance implications

## Thread Pools

### Design Patterns
* Fixed size thread pool with work queue
* Dynamic thread pool with load balancing
* Work stealing for load distribution
* Graceful shutdown with drain and stop

### Implementation Considerations
* Use condition variables for work notification
* Implement bounded work queues
* Handle thread pool exhaustion
* Monitor thread pool health and metrics

### Code Example
```cpp
// Thread-safety: Thread-safe (internal synchronization)
// Ownership: Owns worker threads and work queue
// Invariants: num_threads > 0
// Failure modes: Thread creation failures, queue full
class ThreadPool {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
public:
    explicit ThreadPool(size_t num_threads);
    ~ThreadPool();
    void enqueue(std::function<void()> task);
};
```

## Thread Affinity and NUMA

### CPU Affinity
* Use pthread_setaffinity_np() for CPU affinity
* Understand NUMA topology
* Set affinity for performance critical threads
* Document affinity requirements

### NUMA Awareness
* Use numa_available() to check NUMA support
* Allocate memory on appropriate NUMA nodes
* Set thread affinity to NUMA nodes
* Profile NUMA effects on performance

## Performance Considerations

### False Sharing
* Understand cache line alignment
* Use padding to avoid false sharing
* Profile cache performance
* Document false sharing prevention

### Thread Overhead
* Minimize thread creation overhead
* Use thread pools instead of creating threads on demand
* Consider async/await patterns (C++20)
* Profile thread creation and context switching costs

## Implementation Standards

### Error Handling
* Check all pthread function return values
* Map pthread errors to clear messages
* Handle thread creation failures gracefully
* Document error handling strategies

### Resource Management
* Join or detach all threads
* Clean up thread specific data
* Shutdown thread pools gracefully
* Document thread ownership and lifecycle

### Documentation
* Document thread responsibilities
* Explain synchronization requirements
* Note thread safety assumptions
* Document thread lifetime and ownership

## Testing Requirements
* Test thread creation and joining
* Test thread pool with various workloads
* Test thread local storage
* Test thread cancellation and cleanup
* Test error handling for all operations
* Verify no thread leaks

## Related Topics
* Synchronization: Mutexes, condition variables for thread coordination
* Process Management: Process vs thread trade-offs
* Network Programming: Thread-based connection handling
* Platform-Specific: Platform-specific threading APIs
* Performance Optimization: Thread performance profiling

