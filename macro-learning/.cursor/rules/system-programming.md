# System Programming Macros

## Scope
Applies to system-level macros including conditional compilation, feature detection, platform macros, and build system integration.

## Conditional Compilation

### #if, #ifdef, #ifndef
* Conditional code inclusion
* Platform-specific implementations
* Feature flags
* Debug vs release builds

### #ifdef vs #if defined()
* `#ifdef MACRO` checks if macro is defined
* `#if defined(MACRO)` more flexible, can combine conditions
* `#if defined(MACRO) && !defined(OTHER)` for complex conditions
* Prefer `#if defined()` for complex conditions

### Common Patterns
* Platform detection
* Feature flags
* Version checks
* Debug builds

### Code Example
```c
#if defined(__linux__)
    // Linux-specific code
#elif defined(__APPLE__)
    // macOS-specific code
#elif defined(_WIN32)
    // Windows-specific code
#endif
```

## Feature Detection

### Compiler Detection
* `__GNUC__` for GCC
* `__clang__` for Clang
* `_MSC_VER` for MSVC
* Version number macros

### Standard Detection
* `__STDC_VERSION__` for C standard version
* `__cplusplus` for C++ (if compiling C++)
* Feature test macros
* Standard library feature detection

### Platform Detection
* `__linux__` for Linux
* `__APPLE__` for macOS/iOS
* `_WIN32` for Windows
* Architecture detection (`__x86_64__`, `__aarch64__`)

### Code Example
```c
#if __STDC_VERSION__ >= 201112L
    // C11 features available
#endif

#if defined(__GNUC__) && __GNUC__ >= 7
    // GCC 7+ features
#endif
```

## Build System Integration

### Configuration Macros
* Passed via `-D` compiler flag
* Build-time configuration
* Feature enable/disable
* Version information

### Makefile Integration
```makefile
CFLAGS += -DDEBUG -DVERSION=\"1.0.0\"
```

### CMake Integration
```cmake
add_definitions(-DFEATURE_X=1)
```

### Autotools Integration
* config.h generation
* Feature detection
* Platform configuration

## Debugging Macros

### Debug Builds
* Conditional debug code
* Assertions
* Logging
* Diagnostics

### Pattern Example
```c
#ifdef DEBUG
    #define DBG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define DBG_PRINT(fmt, ...) ((void)0)
#endif
```

### Assert Macros
* Custom assertion with messages
* Conditional compilation
* Better error reporting
* Production vs debug

## Platform Abstraction

### Portable Macros
* Abstract platform differences
* Provide consistent interface
* Handle platform-specific code
* Cross-platform compatibility

### Example Patterns
* File path separators
* Endianness handling
* System call wrappers
* Memory alignment

## Code Quality Standards

### Documentation
* Document platform requirements
* Explain conditional compilation logic
* Note feature dependencies
* Reference platform documentation

### Portability
* Test on multiple platforms
* Handle missing features gracefully
* Provide fallback implementations
* Document platform limitations

### Testing
* Test all conditional branches
* Verify on target platforms
* Test feature detection
* Verify build system integration

## Related Topics
* Fundamentals: Basic conditional compilation
* Enterprise Patterns: Production platform macros
* Performance Optimization: Compile-time optimizations

