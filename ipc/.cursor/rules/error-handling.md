# Error Handling for IPC

## Scope
Applies to all IPC code in this directory. Extends repository root rules.

## Input Validation
* Validate all inputs before calling system interfaces
* Check file paths, permission values, buffer sizes, and other parameters
* Fail fast with clear messages that do not leak internal implementation details

## Error Reporting
* Check errno immediately after system call failures while it is still valid
* Map errno values to clear, actionable error messages
* Use perror or strerror appropriately with context
* Log errors with sufficient context for debugging but without exposing sensitive data

## Exit Codes
* Return appropriate non zero exit codes from main on failure
* Propagate errors from helper functions to callers
* Use consistent exit codes across the codebase for similar failure conditions

## Example Pattern
```cpp
if (shm_fd == -1) {
    std::fprintf(stderr, "shm_open failed: %s\n", std::strerror(errno));
    return 1;
}
```

