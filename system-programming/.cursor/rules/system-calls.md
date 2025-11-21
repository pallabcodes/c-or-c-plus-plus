# System Calls

## Scope
Applies to kernel interfaces, system call usage, syscall optimization, system call overhead, and low level system interfaces.

## System Call Basics

### System Call Interface
* System calls are kernel interfaces
* Use wrapper functions from libc (recommended)
* Direct syscall() for advanced usage
* Understand system call numbers and conventions

### Error Handling
* System calls return -1 on error
* Check errno for error details
* Use perror() or strerror() for error messages
* Handle EINTR for interrupted calls

### System Call Categories
* Process management: fork, exec, wait, exit
* File operations: open, read, write, close
* Memory management: mmap, munmap, mprotect
* Network: socket, bind, connect, accept
* Synchronization: futex, clone

## System Call Optimization

### Minimizing Overhead
* Batch operations when possible
* Reduce system call frequency
* Use appropriate I/O models
* Profile system call overhead

### System Call Alternatives
* Use library functions that buffer I/O
* Use memory mapped I/O for large operations
* Use event driven I/O (epoll, kqueue)
* Consider userspace alternatives

### Context Switching
* System calls involve kernel mode switch
* Minimize context switch overhead
* Use batching and coalescing
* Profile context switch costs

## Advanced System Calls

### clone() System Call
* More control than fork()
* Specify shared resources (CLONE_VM, CLONE_FILES, etc.)
* Use for thread creation (with CLONE_THREAD)
* Document clone() flags and usage

### futex() System Call
* Fast userspace mutex
* Efficient synchronization primitive
* Use FUTEX_WAIT and FUTEX_WAKE operations
* Document futex usage patterns

### io_uring System Calls
* Modern high performance I/O
* io_uring_setup(), io_uring_enter()
* Zero copy and batch operations
* Very high throughput

## System Call Tracing

### strace and ptrace
* Use strace for system call tracing
* Use ptrace() for process tracing
* Understand system call sequences
* Profile system call patterns

### Performance Analysis
* Count system calls per operation
* Measure system call latency
* Identify system call bottlenecks
* Optimize hot system call paths

## Implementation Standards

### Error Handling
* Always check system call return values
* Handle EINTR appropriately
* Map errno to clear messages
* Document error handling strategies

### Performance
* Profile system call usage
* Minimize system call overhead
* Use appropriate alternatives
* Document performance characteristics

### Documentation
* Document system call usage
* Explain system call parameters
* Note platform specific behavior
* Reference Linux manual pages

## Code Examples

### System Call with Error Handling
```cpp
// Thread-safety: Depends on system call
// Ownership: Creates new process
// Invariants: Valid executable path
// Failure modes: Returns -1, sets errno
pid_t safe_fork() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return -1;
    }
    return pid;
}
```

### System Call Tracing Pattern
```cpp
// Thread-safety: Not thread-safe (process specific)
// Ownership: Traces target process
// Invariants: Valid process ID
// Failure modes: Returns -1 on error
int trace_process(pid_t pid) {
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
        perror("ptrace attach failed");
        return -1;
    }
    // Trace system calls
    return 0;
}
```

## Testing Requirements
* Test system call success and failure
* Test error handling for all calls
* Test system call performance
* Verify system call correctness
* Profile system call overhead

## Related Topics
* Process Management: Process system calls
* File Operations: File system calls
* Thread Management: Thread system calls
* Network Programming: Network system calls
* Platform-Specific: Platform-specific system call interfaces
* Performance Optimization: System call profiling

