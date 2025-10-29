# Signals and Process Management for IPC

## Scope
Applies to all IPC code involving processes and signal handling. Extends repository root rules.

## Zombie Prevention
* Handle SIGCHLD or use waitpid with correct options to avoid zombie processes
* Use WNOHANG option when polling for child process status
* Do not ignore SIGCHLD unless you have an alternative mechanism for reaping children
* Document signal handling strategy

## Process Execution
* Use exec family functions with explicit argv arrays, never with shell commands
* Validate executable paths before calling exec
* Use absolute paths or search PATH carefully for executables
* Avoid passing user controlled data directly to exec without validation

## Signal Safety
* Reset or block signals where necessary during critical sections
* Document which signals are handled and why
* Use sigaction instead of signal for better portability and control
* Avoid signal handlers that perform complex operations; prefer flag setting

## Process Communication
* Document which process is the producer and which is the consumer
* Establish clear protocols for process coordination
* Handle process termination gracefully with proper cleanup
* Use process groups appropriately for coordinated shutdown

