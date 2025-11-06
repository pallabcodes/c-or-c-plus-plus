# Code Quality Standards for Event Loop

## Readability
* Self describing names; explicit units for time and sizes
* Separate concerns: I O readiness, scheduling, timers, and application callbacks

## Maintainability
* Small focused functions; early returns
* Configurable thresholds and watermarks
* Document invariants for queues, timers, and reactor state

## Debuggability
* Structured logs with correlation ids per reactor thread
* Clear error messages with remediation guidance

## Size Constraints
* Max function length: 50 lines including comments and whitespace
* Max file length: 200 lines including comments and whitespace
* Max cyclomatic complexity: 10

## Performance
* Avoid blocking in reactor threads
* Batch ready events; minimize syscalls
* Use cache friendly data structures and zero copy buffers

## Testing
* Deterministic tests for timers and scheduling
* Soak tests for stability; fuzz inputs to parsers if present

## Security and Safety
* Validate inputs to callbacks and timers
* Bound memory and time spent per tick
