# Code Quality Standards for Multithreading

## Overview
This document defines production grade code quality standards for multithreaded programming implementations. These standards ensure code is suitable for principal level review and production deployment in high performance concurrent systems.

## Function Size Limits
* **Maximum function length**: 50 lines (excluding comments and blank lines)
* **Rationale**: Functions exceeding 50 lines become difficult to understand, test, and maintain
* **Enforcement**: All functions must be reviewed for size compliance
* **Exception**: Complex synchronization functions may extend to 60 lines with justification

## File Size Limits
* **Maximum file length**: 200 lines (excluding comments and blank lines)
* **Rationale**: Files exceeding 200 lines become difficult to navigate and understand
* **Enforcement**: Split large files into logical modules
* **Exception**: Header files with extensive type definitions may extend to 250 lines

## Cyclomatic Complexity
* **Maximum complexity**: 10 per function
* **Rationale**: High complexity indicates functions doing too much
* **Measurement**: Count decision points (if, while, for, switch, &&, ||, ?:)
* **Enforcement**: Refactor complex functions into smaller, focused functions
* **Exception**: Complex synchronization logic may have complexity up to 15 with justification

## Code Style

### Naming Conventions
* **Functions**: `snake_case` (e.g., `create_thread`, `acquire_lock`, `thread_join`)
* **Classes**: `PascalCase` (e.g., `ThreadPool`, `Mutex`, `ConditionVariable`)
* **Types**: `snake_case` with `_t` suffix (e.g., `thread_t`, `mutex_t`)
* **Constants**: `UPPER_SNAKE_CASE` (e.g., `MAX_THREADS`, `DEFAULT_STACK_SIZE`)
* **Rationale**: Consistent naming improves readability and maintainability

### Indentation
* **Indentation**: 4 spaces (no tabs)
* **Continuation**: Align continuation lines with opening delimiter
* **Rationale**: Consistent indentation improves readability

### Comments
* **Function comments**: Required for all public functions
* **Thread safety**: Document thread safety guarantees
* **Synchronization**: Document synchronization mechanisms
* **Invariants**: Document concurrency invariants
* **Rationale**: Comments clarify thread safety and synchronization

## Error Handling

### Input Validation
* **All public APIs**: Must validate inputs
* **Null pointers**: Check and return error for NULL inputs
* **Thread validation**: Validate thread handles
* **Resource validation**: Validate synchronization primitives
* **Rationale**: Prevents crashes and undefined behavior

### Error Reporting
* **Return codes**: Use consistent return code conventions
* **Error codes**: Define error codes in header files
* **errno**: Use errno for POSIX thread errors
* **Exceptions**: Use exceptions for C++ (optional, prefer return codes)
* **Rationale**: Clear error reporting aids debugging

### Error Recovery
* **Graceful degradation**: Handle errors without crashing
* **Resource cleanup**: Clean up resources on error paths
* **State consistency**: Maintain consistent state on errors
* **Rationale**: Robust error handling improves reliability

## Thread Safety

### Thread Safety Levels
* **Not thread safe**: Must be documented clearly
* **Thread safe**: Document synchronization mechanism
* **Lock free**: Document lock free algorithm
* **Rationale**: Clear thread safety documentation enables correct usage

### Synchronization
* **Locks**: Use locks appropriately
* **Lock ordering**: Establish consistent lock ordering
* **Lock duration**: Minimize lock duration
* **Lock free**: Use lock free algorithms where possible
* **Rationale**: Proper synchronization prevents data races

## Memory Safety

### Thread Local Storage
* **TLS**: Use thread local storage for per thread data
* **Initialization**: Initialize TLS properly
* **Cleanup**: Clean up TLS on thread exit
* **Rationale**: TLS prevents data races

### Shared Memory
* **Synchronization**: Synchronize access to shared memory
* **Atomic operations**: Use atomics for simple shared state
* **Memory ordering**: Use appropriate memory ordering
* **Rationale**: Proper synchronization prevents data races

## Performance

### Lock Contention
* **Minimize locks**: Minimize number of locks
* **Lock duration**: Minimize lock duration
* **Lock granularity**: Use appropriate lock granularity
* **Lock free**: Use lock free algorithms where possible
* **Rationale**: Lock contention affects performance

### Scalability
* **Concurrent performance**: Measure concurrent performance
* **Scalability**: Test scalability with multiple threads
* **Bottlenecks**: Identify scalability bottlenecks
* **Rationale**: Scalability is critical for concurrent systems

## Testing

### Unit Tests
* **Coverage**: Aim for 90%+ code coverage
* **Operations**: Test all thread operations
* **Synchronization**: Test synchronization primitives
* **Edge cases**: Test boundary conditions
* **Rationale**: Comprehensive testing ensures correctness

### Concurrency Tests
* **Race conditions**: Test for race conditions
* **Deadlocks**: Test for deadlocks
* **Stress tests**: High concurrency stress tests
* **Thread sanitizer**: Use thread sanitizer
* **Rationale**: Concurrency tests find bugs

## Documentation

### API Documentation
* **Public functions**: Document all public functions
* **Thread safety**: Document thread safety guarantees
* **Synchronization**: Document synchronization mechanisms
* **Parameters**: Document all parameters and return values
* **Examples**: Provide usage examples
* **Rationale**: Clear documentation enables correct usage

## Examples

### Good Function (Within Limits)
```c
// Thread safety: Thread safe (uses mutex)
// Ownership: Caller owns mutex, must be initialized
// Failure modes: Returns -1 on lock failure, 0 on success
int safe_increment(int* counter, pthread_mutex_t* mutex) {
    if (!counter || !mutex) {
        return -1;
    }
    
    if (pthread_mutex_lock(mutex) != 0) {
        return -1;
    }
    
    (*counter)++;
    
    pthread_mutex_unlock(mutex);
    return 0;
}
```

### Bad Function (Exceeds Limits)
```c
// BAD: Function exceeds 50 lines and has high complexity
int complex_thread_operation(...) {
    // 60+ lines of complex logic with nested conditionals
    // High cyclomatic complexity (> 10)
    // Difficult to test and maintain
}
```

## Enforcement

### Code Review
* **Mandatory**: All code must be reviewed for compliance
* **Checklist**: Use checklist to verify standards
* **Thread safety review**: Special attention to thread safety
* **Automation**: Use tools to check function/file sizes and complexity

### CI/CD
* **Static analysis**: Run static analysis tools in CI
* **Thread sanitizer**: Run thread sanitizer in CI
* **Linting**: Run linters to check style compliance
* **Testing**: Run tests to verify correctness
* **Metrics**: Track code quality metrics over time

