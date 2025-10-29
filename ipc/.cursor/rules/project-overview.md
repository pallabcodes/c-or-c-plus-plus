# IPC Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. All code must meet enterprise production standards suitable for principal level engineering review.

## Purpose
This module covers inter process communication mechanisms for Linux systems using C and C plus plus. All code must follow production grade standards suitable for principal level code review and must be production ready for deployment in high performance, high availability systems.

## Scope
* Applies to all C and C plus plus code in ipc directory
* Extends repository root rules defined in the root `.cursorrules` file
* Covers pipes, shared memory, semaphores, message queues, sockets, and signal handling
* Code quality standards align with expectations from top tier tech companies

## IPC Mechanisms Covered
* Pipes: anonymous and named pipes for unidirectional communication
* Shared memory: POSIX and System V shared memory with synchronization
* Semaphores: POSIX named and unnamed semaphores for coordination
* Message queues: POSIX and System V message queues
* Sockets: Unix domain sockets for bidirectional communication
* Signals: Signal handling for process coordination

## Code Quality Standards
All IPC code must demonstrate:
* Comprehensive error handling with clear messages
* Proper resource management with deterministic cleanup
* Correct synchronization to prevent race conditions and deadlocks
* Memory safety through bounds checking and proper alignment
* Security through least privilege and input validation
* Testing of both success and failure scenarios

## Reference Material
See existing examples in `system programming/processes/` for concrete implementation patterns.

## Related Rules
Refer to the other rule files in this directory for specific guidance on resource management, error handling, synchronization, memory safety, security, signals, and testing.

