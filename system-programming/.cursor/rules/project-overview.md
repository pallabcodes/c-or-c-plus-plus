# System Programming Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. This system programming implementation must meet enterprise production standards suitable for principal level engineering review and must be comparable to top tier implementations used in production systems at these companies, particularly Linux kernel, glibc, and system libraries.

## Purpose
This module covers the design and implementation of production grade system programming techniques in C and C plus plus. All code must follow production grade standards suitable for principal level code review and must be production ready for deployment in high performance systems requiring low level system interfaces, process management, concurrent programming, and high performance I/O.

## Scope
* Applies to all C and C plus plus code in system programming directory
* Extends repository root rules defined in the root `.cursor/rules/` files
* Extends IPC standards from root `.cursor/rules/ipc_standards.mdc`
* Covers all aspects of system programming from process management to high performance I/O
* Code quality standards align with expectations from top tier companies like Google, Linux kernel, glibc, and systemd

## Top Tier Product Comparisons

### Linux Kernel
* Production tested system programming patterns used in billions of devices
* Process management and scheduling
* Memory management and virtual memory
* File system and I/O operations
* Synchronization primitives and concurrency
* Network stack and protocols

### glibc (GNU C Library)
* Standard library system call wrappers
* Threading implementation (NPTL)
* Memory allocation (malloc, mmap)
* File I/O and stream operations
* Process and signal handling
* Production tested in enterprise environments

### systemd
* Modern system service manager
* Process lifecycle management
* Service dependencies and ordering
* Resource limits and security
* Logging and diagnostics
* Production tested in enterprise Linux distributions

### Google Production Systems
* High performance system programming patterns
* Process pools and connection management
* Thread pools and work queues
* Memory efficient data structures
* Zero copy I/O techniques
* Production tested at massive scale

### Redis
* High performance system programming
* Event driven I/O (epoll, kqueue)
* Memory efficient data structures
* Lock free programming patterns
* Production tested in high throughput environments

## System Programming Architecture Components

### Core Components
1. Process Management: Process creation, lifecycle, memory mapping, virtual memory
2. Thread Management: Thread creation, synchronization, thread pools, concurrent programming
3. Synchronization: Mutexes, condition variables, semaphores, barriers, lock free programming
4. File Operations: File I/O, memory mapped I/O, asynchronous I/O, high performance patterns
5. Network Programming: Socket programming, TCP/UDP, event-driven I/O (epoll, kqueue), connection management
6. Inter Process Communication: Pipes, shared memory, message queues, sockets (covered in IPC standards)
7. Signal Handling: Signal management, async signal safety, signal masking
8. System Calls: Kernel interfaces, syscall optimization, system call overhead
9. Platform-Specific: Linux, macOS/BSD, Windows differences and portability
10. Performance Optimization: Profiling, benchmarking, optimization techniques

## Code Quality Standards
All system programming code must demonstrate:
* Comprehensive error handling with clear messages
* Proper resource management with deterministic cleanup
* Correct synchronization to prevent race conditions and deadlocks
* Memory safety through bounds checking and proper alignment
* Security through input validation, privilege management, and secure coding practices
* Testing of both success and failure scenarios including edge cases
* Performance optimization through profiling and benchmarking
* Research backed implementations with proper citations

## Reference Material
* See existing examples in processes, threads, synchronization, and file_ops directories
* Reference Linux kernel source code for production patterns
* Study glibc and systemd implementations
* Review Google production system programming patterns
* Benchmark against industry standard implementations

## Related Rules
Refer to the other rule files in this directory for specific guidance on process management, thread management, synchronization, file operations, network programming, signal handling, system calls, platform-specific considerations, performance optimization, memory management, and testing. Also refer to root level IPC standards for inter process communication patterns.

