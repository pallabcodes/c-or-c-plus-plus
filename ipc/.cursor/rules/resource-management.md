# Resource Management for IPC

## Scope
Applies to all IPC code in this directory. Extends repository root rules.

## System Call Validation
* Always check return values from system calls: pipe, fork, dup, shm_open, mmap, sem_open, msgget, msgsnd, msgrcv, socketpair
* Use perror after system call failure with clear context about which call failed
* Never ignore return values even if success seems guaranteed

## Cleanup Requirements
* Provide deterministic cleanup paths for all resources: file descriptors, memory mappings, semaphores, shared memory objects, message queues
* Use single exit point with cleanup labels or small RAII wrappers to prevent leaks
* Clean up resources in reverse order of creation
* Ensure cleanup code executes even in error paths

## Example Pattern
```cpp
int shm_fd = -1;
sem_t* sem = SEM_FAILED;
void* mapping = nullptr;

// ... create resources ...

cleanup:
if (mapping) munmap(mapping, size);
if (shm_fd != -1) close(shm_fd);
if (sem != SEM_FAILED) sem_close(sem);
sem_unlink(sem_name);
shm_unlink(shm_name);
```

