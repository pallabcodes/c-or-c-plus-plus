# Enterprise Macro Patterns

## Scope
Applies to production macro patterns from Linux kernel, system libraries, and real-world codebases.

## Linux Kernel Patterns

### container_of Macro
* Type-safe pointer arithmetic
* Get containing structure from member pointer
* Used extensively in kernel data structures
* Production tested in billions of devices

### Implementation Pattern
```c
#define container_of(ptr, type, member) ({ \
    const typeof(((type *)0)->member) *__mptr = (ptr); \
    (type *)((char *)__mptr - offsetof(type, member)); \
})
```

### Usage
* Linked lists embedded in structures
* Type-safe generic containers
* Avoids void pointer casting
* Compile-time type checking

### ARRAY_SIZE Macro
* Compile-time array size calculation
* Prevents pointer decay issues
* Used throughout kernel code
* Safe alternative to sizeof/sizeof pattern

### Implementation
```c
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
```

### BUILD_BUG_ON Macro
* Compile-time assertion
* Fails compilation if condition is true
* Used for invariant checking
* Zero runtime cost

### Implementation Pattern
```c
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
```

### min/max Macros
* Type-safe min/max operations
* Handle different types correctly
* Used in performance-critical paths
* Kernel optimized versions

### Implementation Considerations
* Type checking with typeof
* Handle signed/unsigned correctly
* Prevent multiple evaluation
* Consider inline functions for C++

## System Library Patterns

### Error Handling Macros
* Standardized error reporting
* Combine errno with messages
* Consistent error handling patterns
* Used in glibc and system libraries

### Logging Macros
* Structured logging macros
* Different log levels
* Conditional compilation
* Performance optimized

### Pattern Example
```c
#define LOG_ERROR(fmt, ...) \
    fprintf(stderr, "ERROR: " fmt "\n", ##__VA_ARGS__)
```

### Assert Macros
* Custom assertion macros
* Better error messages
* Conditional compilation
* Production vs debug builds

### Configuration Macros
* Feature detection macros
* Build configuration
* Platform-specific code
* Cross-platform compatibility

## systemd Patterns

### Modern Service Macros
* Service definition macros
* Configuration generation
* Type-safe configurations
* Compile-time validation

### Diagnostic Macros
* System diagnostic helpers
* Error reporting
* Logging integration
* Production debugging

## Production Best Practices

### Type Safety
* Use typeof for type checking
* Avoid void pointer casting
* Leverage offsetof for structure access
* Compile-time type validation

### Performance
* Zero-cost abstractions
* Compile-time evaluation
* Avoid runtime overhead
* Optimize for common cases

### Portability
* Feature detection macros
* Platform abstraction
* Standard compliance
* Graceful degradation

### Maintainability
* Clear macro names
* Comprehensive documentation
* Usage examples
* Migration paths

## Code Quality Standards

### Documentation
* Explain macro purpose and usage
* Document type requirements
* Note platform dependencies
* Provide usage examples
* Reference kernel/system library sources

### Error Handling
* Compile-time validation
* Clear error messages
* Graceful failure modes
* Document undefined behavior

### Testing
* Test with various types
* Test edge cases
* Verify on multiple platforms
* Test with different compilers
* Compare against reference implementations

## Research References
* Linux kernel source code (kernel.org)
* glibc source code (gnu.org)
* systemd source code (freedesktop.org)
* C Standard (ISO/IEC 9899:2011)

## Related Topics
* Advanced Techniques: X-macros, metaprogramming
* Performance Optimization: Compile-time vs runtime
* System Programming: Platform-specific macros

