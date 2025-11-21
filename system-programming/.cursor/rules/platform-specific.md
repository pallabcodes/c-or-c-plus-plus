# Platform-Specific Considerations

## Scope
Applies to platform-specific system programming differences, portability concerns, and platform-specific optimizations for Linux, macOS, and Windows.

## Platform Overview

### Linux
* Primary target platform for system programming
* Rich system call interface
* Advanced I/O interfaces (epoll, io_uring)
* Extensive kernel features
* Production tested in enterprise environments

### macOS/BSD
* Unix-like system with BSD heritage
* kqueue for event-driven I/O
* Different system call names and behaviors
* Mach kernel features
* Production tested in Apple ecosystem

### Windows
* Different system call interface (Win32 API)
* IOCP (I/O Completion Ports) for async I/O
* Different process and thread models
* Different memory management
* Production tested in Windows environments

## System Call Differences

### Process Management

#### Linux
* fork(), clone() for process creation
* waitpid() for process waiting
* prctl() for process control
* clone() with CLONE_* flags for advanced control

#### macOS/BSD
* fork() for process creation
* waitpid() for process waiting
* Different process control mechanisms
* Some clone() flags not available

#### Windows
* CreateProcess() instead of fork()
* WaitForSingleObject() instead of waitpid()
* Different process model (no fork())
* Use CreateThread() for threads

### I/O and Event Loops

#### Linux
* epoll for event-driven I/O
* io_uring for high performance I/O
* signalfd, timerfd, eventfd for event sources
* SO_REUSEPORT for load balancing

#### macOS/BSD
* kqueue for event-driven I/O
* Different event source mechanisms
* SO_REUSEPORT available
* Different socket option names

#### Windows
* IOCP (I/O Completion Ports) for async I/O
* WSAEventSelect for event-driven I/O
* Different socket API (Winsock)
* No epoll or kqueue equivalent

### Memory Management

#### Linux
* mmap() with MAP_ANONYMOUS
* madvise() for memory hints
* mlock() for locking memory
* Transparent huge pages support

#### macOS/BSD
* mmap() with MAP_ANONYMOUS
* madvise() available (different flags)
* mlock() for locking memory
* Different huge page support

#### Windows
* VirtualAlloc() instead of mmap()
* Different memory protection model
* Different memory locking mechanisms

### Synchronization

#### Linux
* futex() for fast userspace mutexes
* pthread primitives (NPTL)
* Robust mutexes (PTHREAD_MUTEX_ROBUST)
* Process-shared synchronization

#### macOS/BSD
* pthread primitives
* Different mutex implementation
* Process-shared synchronization available
* No futex() equivalent

#### Windows
* CriticalSection for mutexes
* Event, Semaphore, Mutex objects
* Different synchronization model
* No pthread equivalent (use third-party)

## Portability Strategies

### Abstraction Layers
* Create platform abstraction layer
* Hide platform-specific system calls
* Provide consistent interface
* Document platform differences

### Conditional Compilation
* Use #ifdef for platform-specific code
* Define platform macros (__linux__, __APPLE__, _WIN32)
* Keep platform-specific code isolated
* Test on all target platforms

### Feature Detection
* Use autoconf or CMake for feature detection
* Check for system call availability
* Provide fallback implementations
* Document feature availability

## Code Examples

### Platform-Agnostic Event Loop
```cpp
// Thread-safety: Not thread-safe (single event loop)
// Ownership: Owns platform-specific resources
// Invariants: Valid socket
// Failure modes: Returns -1 on error
#ifdef __linux__
    int epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    // Use epoll
#elif defined(__APPLE__) || defined(__FreeBSD__)
    int kq = kqueue();
    // Use kqueue
#elif defined(_WIN32)
    HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    // Use IOCP
#endif
```

### Platform-Specific Socket Options
```cpp
// Thread-safety: Thread-safe (socket operation)
// Ownership: Borrows socket fd
// Invariants: fd valid
// Failure modes: Returns -1 on error
int set_reuseport(int fd) {
#ifdef __linux__
    int opt = 1;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
#elif defined(__APPLE__) || defined(__FreeBSD__)
    int opt = 1;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
#else
    // Not available on this platform
    return 0;
#endif
}
```

## Testing Requirements
* Test on all target platforms
* Test platform-specific features
* Test fallback implementations
* Verify portability
* Document platform differences

## Related Topics
* Network Programming: Platform-specific I/O models
* File Operations: Platform-specific file I/O
* Process Management: Platform-specific process APIs
* Thread Management: Platform-specific threading

