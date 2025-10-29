# Synchronization for IPC

## Scope
Applies to all IPC code involving shared memory or concurrent access. Extends repository root rules.

## Primitive Selection
* Use appropriate synchronization primitives: semaphores, mutexes, condition variables
* Choose POSIX semaphores for cross process synchronization
* Use pthread mutexes with PTHREAD_PROCESS_SHARED for shared memory mutexes
* Document why a particular primitive was chosen for the use case

## Ordering and Ownership
* Document ordering requirements and ownership semantics for shared data
* Clearly specify which process is responsible for initialization
* Document acquire and release semantics for synchronization points
* Ensure consistent lock ordering across all processes to prevent deadlocks

## Timeout Handling
* Use timed wait variants where blocking could indefinitely stall the system
* Handle timeout paths explicitly with appropriate error reporting
* Avoid busy waiting; prefer blocking operations with timeouts
* Document timeout values and their rationale

## Deadlock Prevention
* Follow a single, consistent lock order across all code paths
* Minimize critical sections to reduce contention
* Avoid nested locking where possible
* Document locking protocols in code comments

