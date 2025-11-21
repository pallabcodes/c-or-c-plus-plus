# Process Management

## Scope
Applies to process creation, lifecycle management, memory mapping, virtual memory, and process related system calls.

## Process Creation

### fork() and clone()
* Use fork() for simple process duplication
* Use clone() for more control over shared resources
* Understand Copy on Write (CoW) semantics
* Handle fork() return value correctly (negative = error, 0 = child, positive = parent)
* Always wait for child processes to avoid zombies

### exec Family
* Use exec family with explicit argv arrays
* Validate paths before exec
* Use execvp or execvpe for PATH search when appropriate
* Never return after successful exec (exec replaces process image)

### Process Groups and Sessions
* Understand process groups and sessions
* Use setpgid() and setsid() appropriately
* Handle orphaned process groups
* Document process group relationships

## Virtual Memory and Memory Mapping

### mmap() Patterns
* Use MAP_PRIVATE for copy on write semantics
* Use MAP_SHARED for inter process communication
* Check return value against MAP_FAILED
* Verify mapping length and protection flags
* Unmap mappings with munmap() before exit

### Memory Protection
* Use mprotect() to change memory protections
* Understand PROT_READ, PROT_WRITE, PROT_EXEC flags
* Document memory protection changes
* Handle mprotect() errors appropriately

### File Backed Mappings
* Use file backed mappings for large files
* Understand MAP_SHARED vs MAP_PRIVATE for files
* Handle file truncation and extension
* Document file mapping ownership

## Process Lifecycle

### Process Termination
* Use exit() or _exit() appropriately
* Clean up resources before exit
* Set exit codes meaningfully
* Handle atexit() handlers

### Process Waiting
* Use wait() or waitpid() to collect child processes
* Handle SIGCHLD signal or use waitpid() with WNOHANG
* Avoid zombie processes
* Use waitpid() with WUNTRACED for stopped processes

### Process Signals
* Handle SIGCHLD to avoid zombies
* Use waitpid() with correct options
* Document signal handling strategies
* Ensure async signal safety in signal handlers

## Implementation Standards

### Error Handling
* Check all system call return values
* Use perror() or strerror() with context
* Map errno to clear error messages
* Return appropriate exit codes

### Resource Management
* Clean up all resources before exit
* Use RAII patterns where possible
* Document resource ownership
* Provide cleanup labels for error paths

### Documentation
* Document process relationships (parent child)
* Explain memory mapping strategies
* Note Copy on Write behavior
* Document process group and session relationships

## Code Examples

### Process Creation with Error Handling
```cpp
// Thread-safety: Not thread-safe (process creation)
// Ownership: Creates new process, parent owns child
// Invariants: Valid executable path
// Failure modes: Returns -1 on error, sets errno
pid_t create_process(const char* path, char* const argv[]) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return -1;
    }
    if (pid == 0) {
        execvp(path, argv);
        perror("execvp failed");
        _exit(1);
    }
    return pid;
}
```

### Memory Mapping with Cleanup
```cpp
// Thread-safety: Thread-safe (read-only after mapping)
// Ownership: Caller owns mapping, must munmap
// Invariants: size > 0, fd valid
// Failure modes: Returns MAP_FAILED on error
void* map_file(int fd, size_t size) {
    void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE, 
                      MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap failed");
        return NULL;
    }
    return addr;
}
```

## Testing Requirements
* Test process creation success and failure
* Test memory mapping with various sizes
* Test Copy on Write behavior
* Test process cleanup and zombie prevention
* Test error handling for all system calls
* Verify resource cleanup in all code paths

## Related Topics
* Thread Management: Process vs thread trade-offs
* Synchronization: Process synchronization primitives
* File Operations: File backed memory mappings
* Signal Handling: Process signal management
* Network Programming: Process-based connection handling
* Platform-Specific: Platform-specific process APIs

