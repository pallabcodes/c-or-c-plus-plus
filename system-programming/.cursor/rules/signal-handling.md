# Signal Handling

## Scope
Applies to signal management, async signal safety, signal masking, signal delivery, and signal based IPC.

## Signal Basics

### Signal Types
* Termination signals: SIGTERM, SIGKILL, SIGINT
* Error signals: SIGSEGV, SIGBUS, SIGFPE, SIGILL
* Job control: SIGSTOP, SIGCONT, SIGTSTP
* User defined: SIGUSR1, SIGUSR2
* Document signal usage and meaning

### Signal Delivery
* Signals are delivered asynchronously
* Signal handlers interrupt normal execution
* Only async signal safe functions in handlers
* Signals can be lost if not handled

## Signal Handlers

### signal() and sigaction()
* Prefer sigaction() over signal() (more control)
* Set up signal handlers with sigaction()
* Provide handler function (void (*)(int))
* Handle siginfo_t for extended signal information

### Async Signal Safety
* Only use async signal safe functions in handlers
* Async signal safe: write(), read(), _exit(), etc.
* Not safe: malloc(), printf(), most library functions
* Set flags in handlers, process in main code

### Handler Patterns
* Set volatile sig_atomic_t flags in handlers
* Use self pipe trick for complex handling
* Avoid complex logic in signal handlers
* Document signal handler safety

## Signal Masking

### sigprocmask()
* Block signals with sigprocmask()
* Use sigset_t for signal sets
* Block signals during critical sections
* Unblock signals after critical sections

### Signal Sets
* Use sigemptyset(), sigfillset(), sigaddset(), sigdelset()
* Use sigismember() to test signal membership
* Create signal sets for blocking
* Document signal masking strategies

## Signal Delivery and Blocking

### Pending Signals
* Blocked signals become pending
* Signals are delivered when unblocked
* Multiple instances may be merged (standard signals)
* Real time signals queue multiple instances

### Signal Queues
* Standard signals: Only one instance queued
* Real time signals (SIGRTMIN to SIGRTMAX): Queue multiple
* Use real time signals for reliable delivery
* Document signal queuing behavior

## Common Signal Patterns

### SIGCHLD Handling
* Handle SIGCHLD to avoid zombie processes
* Use waitpid() with WNOHANG in handler
* Reap all terminated children
* Document SIGCHLD handling strategy

### Graceful Shutdown
* Handle SIGTERM for graceful shutdown
* Set shutdown flag in handler
* Clean up resources in main code
* Use timeout for forced shutdown

### Error Signal Handling
* Handle SIGSEGV, SIGBUS for error recovery
* Log error information
* Clean up and exit gracefully
* Consider core dumps for debugging

## Implementation Standards

### Error Handling
* Check sigaction() return value
* Handle signal delivery errors
* Document signal handling errors
* Provide fallback strategies

### Safety
* Ensure async signal safety in handlers
* Avoid race conditions with signal flags
* Use volatile for signal flags
* Document signal safety requirements

### Documentation
* Document signal usage and meaning
* Explain signal handler responsibilities
* Note async signal safety requirements
* Document signal masking strategies

## Code Examples

### Safe Signal Handler
```cpp
// Thread-safety: Signal handlers (async signal safe)
// Ownership: Sets global flag
// Invariants: Handler must be async signal safe
// Failure modes: Signal may be lost if handler not set
volatile sig_atomic_t shutdown_flag = 0;

void signal_handler(int sig) {
    // Only async signal safe operations
    shutdown_flag = 1;
}

void setup_signal_handler() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction failed");
    }
}
```

### SIGCHLD Handler
```cpp
// Thread-safety: Signal handler (async signal safe)
// Ownership: Reaps child processes
// Invariants: Handler must call waitpid
// Failure modes: Zombie processes if not handled
void sigchld_handler(int sig) {
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        // Reap all terminated children
    }
    errno = saved_errno;
}
```

## Testing Requirements
* Test signal delivery and handling
* Test signal masking and unblocking
* Test SIGCHLD handling
* Test graceful shutdown with signals
* Test error signal handling
* Verify no signal loss
* Test async signal safety

## Related Topics
* Process Management: Process signal handling
* Thread Management: Thread signal handling
* Synchronization: Signal based synchronization
* Platform-Specific: Platform-specific signal APIs

