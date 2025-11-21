# Synchronization

## Scope
Applies to mutexes, condition variables, semaphores, barriers, spinlocks, atomics, and lock free programming.

## Mutexes

### pthread_mutex_t
* Initialize mutexes with pthread_mutex_init() or PTHREAD_MUTEX_INITIALIZER
* Always lock and unlock mutexes in pairs
* Use RAII wrappers to ensure unlock on all paths
* Destroy mutexes with pthread_mutex_destroy()

### Mutex Types
* PTHREAD_MUTEX_NORMAL: Standard mutex
* PTHREAD_MUTEX_ERRORCHECK: Error checking mutex
* PTHREAD_MUTEX_RECURSIVE: Recursive mutex (use sparingly)
* PTHREAD_MUTEX_DEFAULT: Platform default
* Document mutex type choice

### Lock Ordering
* Establish consistent lock ordering to prevent deadlocks
* Document lock ordering rules
* Use timeout locks (pthread_mutex_timedlock()) where appropriate
* Minimize critical sections

## Condition Variables

### pthread_cond_t
* Initialize condition variables with pthread_cond_init()
* Always use condition variables with a mutex
* Use while loops for condition checks (spurious wakeups)
* Signal or broadcast appropriately (signal for one waiter, broadcast for all)

### Condition Variable Patterns
* Wait pattern: lock mutex, check condition, wait, recheck condition
* Signal pattern: lock mutex, change condition, signal/broadcast, unlock
* Document condition variable usage patterns
* Handle timeout with pthread_cond_timedwait()

## Semaphores

### sem_t (POSIX Semaphores)
* Initialize semaphores with sem_init()
* Use sem_wait() and sem_post() for synchronization
* Handle semaphore errors appropriately
* Destroy semaphores with sem_destroy()

### Named Semaphores
* Use sem_open() for named semaphores
* Use sem_close() and sem_unlink() for cleanup
* Handle semaphore creation errors
* Document semaphore naming conventions

### Semaphore Patterns
* Binary semaphores for mutual exclusion
* Counting semaphores for resource counting
* Use semaphores for producer consumer patterns
* Document semaphore usage and counts

## Barriers

### pthread_barrier_t
* Initialize barriers with pthread_barrier_init()
* Use pthread_barrier_wait() for synchronization
* All threads must reach barrier before proceeding
* Destroy barriers with pthread_barrier_destroy()

### Barrier Usage
* Use barriers for phase synchronization
* Document barrier synchronization points
* Handle barrier errors appropriately
* Consider timeout mechanisms for barriers

## Spinlocks

### pthread_spinlock_t
* Use spinlocks for very short critical sections
* Initialize with pthread_spin_init()
* Understand when spinlocks are appropriate (low contention, short sections)
* Avoid spinlocks on single core systems

### Spinlock Trade-offs
* Lower overhead than mutexes for short sections
* Waste CPU cycles while spinning
* Use only when contention is low and sections are very short
* Consider adaptive mutexes as alternative

## Atomic Operations

### std::atomic (C++11)
* Use atomic operations for lock free programming
* Specify memory ordering explicitly (memory_order_relaxed, acquire, release, seq_cst)
* Document memory ordering choices
* Understand atomic operation costs

### Atomic Patterns
* Compare and swap (CAS) for lock free updates
* Atomic counters and flags
* Lock free data structures
* Document atomic operation semantics

## Lock Free Programming

### Lock Free Data Structures
* Use lock free algorithms for high contention scenarios
* Understand ABA problem and solutions
* Use memory barriers appropriately
* Document lock free algorithm correctness

### Futex (Fast Userspace Mutex)
* Linux specific fast mutex implementation
* Use futex() system call for efficient synchronization
* Understand futex operations (FUTEX_WAIT, FUTEX_WAKE)
* Document platform specific usage

## Deadlock Prevention

### Strategies
* Lock ordering: Always acquire locks in same order
* Timeout locks: Use timed lock operations
* Lock free alternatives: Use atomic operations when possible
* Deadlock detection: Monitor for potential deadlocks

### Detection
* Resource allocation graph algorithms
* Timeout based detection
* Lock ordering validation
* Document deadlock prevention strategies

## Implementation Standards

### Error Handling
* Check all synchronization primitive return values
* Handle EDEADLK, ETIMEDOUT errors appropriately
* Map errors to clear messages
* Document error handling strategies

### Resource Management
* Initialize all synchronization primitives
* Destroy primitives before exit
* Use RAII wrappers where possible
* Document resource ownership

### Documentation
* Document lock ordering rules
* Explain synchronization requirements
* Note thread safety assumptions
* Document deadlock prevention strategies

## Code Examples

### Mutex with RAII
```cpp
// Thread-safety: Thread-safe (internal mutex)
// Ownership: Owns mutex, unlocks on destruction
// Invariants: Mutex must be initialized
// Failure modes: Lock failures throw or return error
class MutexLock {
    pthread_mutex_t* mutex;
public:
    explicit MutexLock(pthread_mutex_t* m) : mutex(m) {
        pthread_mutex_lock(mutex);
    }
    ~MutexLock() {
        pthread_mutex_unlock(mutex);
    }
};
```

### Condition Variable Pattern
```cpp
// Thread-safety: Thread-safe (condition variable + mutex)
// Ownership: Shared state protected by mutex
// Invariants: Mutex must be held when waiting
// Failure modes: Spurious wakeups, timeout
void wait_for_condition(pthread_cond_t* cond, pthread_mutex_t* mutex, 
                       bool& condition) {
    pthread_mutex_lock(mutex);
    while (!condition) {
        pthread_cond_wait(cond, mutex);
    }
    pthread_mutex_unlock(mutex);
}
```

## Testing Requirements
* Test mutex locking and unlocking
* Test condition variable signaling
* Test semaphore counting
* Test barrier synchronization
* Test deadlock scenarios
* Test lock free algorithms with concurrent access
* Verify no resource leaks

## Related Topics
* Thread Management: Thread synchronization
* Process Management: Process synchronization primitives
* Network Programming: Socket synchronization
* Platform-Specific: Platform-specific synchronization primitives
* Performance Optimization: Lock performance profiling

