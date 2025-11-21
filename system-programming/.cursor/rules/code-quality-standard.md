# Code Quality Standards for System Programming

## Scope
Applies to all system programming code in this directory. Extends repository root rules and IPC standards.

## Code Review Context
This code is expected to undergo line by line scrutiny by principal level reviewers from top tier companies like Google, Linux kernel maintainers, and system library developers. Every line must demonstrate production grade quality, clarity, and maintainability comparable to internal implementations at these organizations.

## Code Quality Dimensions

### Readability
* Code must be immediately understandable without extensive documentation
* Variable and function names must be self documenting and convey intent clearly
* Avoid abbreviations unless they are industry standard or clearly documented (e.g., mmap, epoll, futex)
* Use consistent naming conventions throughout all files
* Group related operations logically with clear separation between sections
* Use whitespace strategically to improve visual structure
* Follow system programming naming conventions (e.g., fd for file descriptor, pid for process ID)

### Maintainability
* Code must be easy to modify and extend without breaking existing functionality
* Functions must have single, well defined responsibilities
* Avoid deep nesting; prefer early returns and guard clauses
* Minimize coupling between system programming components
* Use constants and configuration values instead of magic numbers
* Document assumptions and dependencies clearly
* Design for extensibility with clear interfaces

### Debuggability
* Error messages must provide actionable information for debugging
* Use structured logging with appropriate log levels
* Include context in error messages to trace execution paths
* Ensure all error conditions are testable and reproducible
* Provide clear failure modes that aid in root cause analysis
* Use assertion like checks for invariants that should never occur
* Include system call context in error messages (errno, syscall name)

## Size Constraints

### Function Length
* Maximum function length: 50 lines including comments and whitespace
* If a function exceeds 50 lines, it must be refactored into smaller, focused functions
* Each function should accomplish one clearly defined task
* Complex operations like process management should be decomposed into helper functions with descriptive names

### File Length
* Maximum file length: 200 lines including comments and whitespace
* Files exceeding 200 lines should be split into logical modules
* Each file should have a single, well defined purpose
* Related functions and data structures should be grouped within appropriate header and implementation files

### Cyclomatic Complexity
* Maximum cyclomatic complexity per function: 10
* Functions with complexity over 10 must be refactored to reduce branching
* Use early returns and guard clauses to reduce nesting
* Extract complex conditions into well named boolean functions
* System programming code should favor clear control flow over clever optimizations

## Review Standards
* Assume line by line scrutiny by principal level reviewers
* Favor clarity over cleverness in all implementations
* Document tradeoffs and rationale for non trivial decisions
* Keep code minimal and focused on demonstrating single concepts
* Reference research papers and Linux kernel patterns when applicable

## Style Guidelines
* Keep examples minimal and focused on one system programming concept per file
* Add concise comments only where intent or system call usage is non obvious
* Use meaningful names for file descriptors, process IDs, and system resources
* Prefer early returns and shallow nesting to reduce complexity
* Follow system programming terminology consistently (e.g., process, thread, mutex, semaphore)

## Documentation Requirements
* Explain system calls and their parameters with brief context
* Document performance characteristics and complexity analysis
* Explain synchronization points and their purpose
* Keep functions focused on single system operations
* Reference Linux manual pages (man pages) when implementing system calls
* Document invariants and constraints for system resources
* Include thread safety, ownership, invariants, and failure mode documentation for all functions

## Resource Management

### System Resources
* Always check return values of system calls
* Use perror or strerror after system call failure with clear context
* Provide deterministic cleanup paths for all resources
* Prefer RAII wrappers or cleanup labels to avoid leaks
* Document resource ownership and lifecycle

### File Descriptors
* Check file descriptor return values
* Close file descriptors in all code paths
* Use close on exec flag where appropriate
* Document file descriptor ownership

### Memory Mappings
* Check mmap against MAP_FAILED
* Verify lengths and protections
* Unmap memory mappings before exit
* Document mapping ownership and lifecycle

## Error Handling

### System Call Errors
* Always check return values
* Check errno on failure paths
* Map errno to clear, actionable error messages
* Return appropriate error codes
* Propagate errors correctly from helpers

### Resource Exhaustion
* Handle ENOMEM, EMFILE, ENFILE errors gracefully
* Implement resource limits and quotas
* Provide fallback strategies
* Document resource requirements

## Security Considerations

### Input Validation
* Validate all inputs before system calls
* Check buffer sizes and bounds
* Sanitize file paths and names
* Validate process IDs and file descriptors

### Privilege Management
* Use least privilege permissions
* Avoid predictable resource names
* Secure shared memory and IPC objects
* Document security implications

## Performance Requirements
* Profile critical paths and record findings
* Optimize for modern hardware (cache awareness, NUMA)
* Minimize system call overhead
* Use appropriate I/O models (blocking, non blocking, async)
* Document performance characteristics
* Benchmark against alternatives

## Integration with Other Rules
This file provides general quality standards. For specific guidance, refer to:
* `process-management.md` for process creation and management
* `thread-management.md` for threading and concurrency
* `synchronization.md` for mutexes, condition variables, semaphores
* `file-operations.md` for file I/O and memory mapped I/O
* `network-programming.md` for socket programming and event-driven I/O
* `signal-handling.md` for signal management
* `system-calls.md` for kernel interfaces
* `platform-specific.md` for Linux, macOS, Windows differences
* `performance-optimization.md` for profiling and optimization
* `memory-management.md` for memory allocation and management
* `testing-validation.md` for test coverage requirements
* Root level `ipc_standards.mdc` for inter process communication

