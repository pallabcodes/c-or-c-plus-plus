# Memory Safety for IPC

## Scope
Applies to all IPC code using shared memory or buffer operations. Extends repository root rules.

## Memory Mapping Validation
* Always check mmap return value against MAP_FAILED
* Verify requested lengths and protection flags are appropriate
* Validate alignment requirements for shared data structures
* Ensure shared memory is properly sized before mapping

## Shared Structure Safety
* Ensure proper alignment and padding for shared structs
* Avoid undefined behavior from type punning across process boundaries
* Use explicit padding or compiler attributes to control layout
* Document memory layout assumptions and alignment requirements

## Buffer Operations
* Bound all buffer operations and never use unbounded string functions like strcpy
* Use strncpy with explicit bounds or safer alternatives like snprintf
* Check buffer sizes before copy operations
* Validate lengths received from other processes before using them

## Initialization
* Initialize shared memory with known values before exposing to other processes
* Use memory barriers or synchronization primitives when needed to ensure visibility
* Clear sensitive data before releasing shared memory regions
* Document initialization requirements and ordering

