# C++ Macros: Complete Guide for Bloomberg SDE-3

## Table of Contents
1. [Introduction: What Are Macros?](#introduction-what-are-macros)
2. [Why Macros Exist](#why-macros-exist)
3. [The Preprocessor](#the-preprocessor)
4. [Object-Like Macros](#object-like-macros)
5. [Function-Like Macros](#function-like-macros)
6. [Advanced Macro Features](#advanced-macro-features)
7. [Preprocessor Directives](#preprocessor-directives)
8. [Common Macro Patterns](#common-macro-patterns)
9. [Bloomberg-Style Macros](#bloomberg-style-macros)
10. [When to Use Macros](#when-to-use-macros)
11. [When NOT to Use Macros](#when-not-to-use-macros)
12. [Modern C++ Alternatives](#modern-c-alternatives)
13. [Common Pitfalls](#common-pitfalls)
14. [Best Practices](#best-practices)

## Introduction: What Are Macros?

### Definition
**Macros** are preprocessor directives that perform **text substitution** before compilation. They are processed by the C++ preprocessor, which runs before the actual compiler.

### Key Characteristics
- **Text replacement**: Macros are replaced literally in source code
- **No type checking**: The preprocessor doesn't understand C++ types
- **No scope**: Macros are global unless undefined
- **Compile-time only**: Macros don't exist at runtime

### JavaScript/TypeScript Analogy
In JS/TS, macros don't exist natively, but you can think of them as:
- **Build-time code generation** (like Babel transforms)
- **Template literals** that get replaced before execution
- **Build scripts** that modify code before compilation

## Why Macros Exist

### Historical Reasons
1. **C Compatibility**: Macros predate modern C++ features
2. **Performance**: Zero runtime overhead (pure text substitution)
3. **Conditional Compilation**: Platform-specific code
4. **Code Generation**: Reduce boilerplate

### Modern Use Cases
1. **Header Guards**: Prevent multiple inclusion
2. **Debugging**: Conditional debug output
3. **Platform Abstraction**: Different code for different platforms
4. **Feature Flags**: Enable/disable features at compile time
5. **Assertions**: Custom assertion mechanisms

### Why Bloomberg Engineers Need Macros
- **Cross-platform code**: Windows, Linux, Solaris support
- **Performance-critical systems**: Zero-overhead abstractions
- **Legacy codebases**: Maintaining C compatibility
- **Build configuration**: Feature flags and optimizations

## The Preprocessor

### Preprocessing Stages
```
Source Code → Preprocessor → Compiler → Object Files → Linker → Executable
              (Macros)      (C++ code)
```

### Preprocessor Operations
1. **Macro expansion**: Replace macros with their definitions
2. **File inclusion**: `#include` directives
3. **Conditional compilation**: `#ifdef`, `#ifndef`, `#if`
4. **Line control**: `#line` directives
5. **Pragma directives**: `#pragma` for compiler-specific features

## Object-Like Macros

### Basic Syntax
```cpp
#define MACRO_NAME replacement_text
```

### Simple Constants
```cpp
#define PI 3.141592653589793
#define MAX_SIZE 1024
#define COMPANY_NAME "Bloomberg"
```

### JS/TS Equivalent
```javascript
// In JS/TS, you'd use constants:
const PI = 3.141592653589793;
const MAX_SIZE = 1024;
const COMPANY_NAME = "Bloomberg";

// Or in TypeScript:
const PI: number = 3.141592653589793;
```

### Key Differences
- **C++ macros**: Text replacement, no type checking
- **JS/TS constants**: Type-checked, scoped, can be optimized

### When to Use
- **Compile-time constants** that need to be available everywhere
- **Configuration values** that change per build
- **Magic numbers** that should be named

## Function-Like Macros

### Basic Syntax
```cpp
#define MACRO_NAME(param1, param2) replacement_text
```

### Simple Examples
```cpp
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SQUARE(x) ((x) * (x))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
```

### JS/TS Equivalent
```javascript
// In JS/TS, you'd use functions:
function max(a, b) { return a > b ? a : b; }
function square(x) { return x * x; }
function min(a, b) { return a < b ? a : b; }

// Or arrow functions:
const max = (a, b) => a > b ? a : b;
const square = x => x * x;
```

### Critical Differences
- **Macros**: Text substitution, evaluated multiple times
- **Functions**: Type-checked, evaluated once, can be inlined

### Common Pitfall: Multiple Evaluation
```cpp
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int i = 5;
int result = MAX(++i, 10);  // i incremented TWICE!
// Expands to: ((++i) > (10) ? (++i) : (10))
```

## Advanced Macro Features

### Stringification (#)
Converts macro arguments to string literals.

```cpp
#define STRINGIFY(x) #x
#define PRINT_VAR(x) std::cout << #x << " = " << x << std::endl

int value = 42;
PRINT_VAR(value);  // Outputs: value = 42
```

### Token Concatenation (##)
Combines tokens into a single token.

```cpp
#define CONCAT(a, b) a##b
#define MAKE_VAR(name, type) type CONCAT(name, _var)

MAKE_VAR(counter, int);  // Expands to: int counter_var;
```

### Variadic Macros (...)
Macros that accept variable number of arguments.

```cpp
#define LOG(...) printf(__VA_ARGS__)
#define DEBUG_PRINT(format, ...) \
    printf("[DEBUG] " format "\n", __VA_ARGS__)

LOG("Value: %d\n", 42);
DEBUG_PRINT("Error %d: %s", errno, strerror(errno));
```

### JS/TS Equivalent
```javascript
// In JS/TS, you'd use rest parameters:
function log(...args) {
    console.log(...args);
}

function debugPrint(format, ...args) {
    console.log(`[DEBUG] ${format}`, ...args);
}
```

## Preprocessor Directives

### Include Guards
```cpp
#ifndef HEADER_NAME_H
#define HEADER_NAME_H
// Header content
#endif
```

### Conditional Compilation
```cpp
#ifdef DEBUG
    #define LOG(x) std::cout << x << std::endl
#else
    #define LOG(x)
#endif

#if defined(WIN32) || defined(_WIN32)
    // Windows-specific code
#elif defined(__linux__)
    // Linux-specific code
#elif defined(__APPLE__)
    // macOS-specific code
#endif
```

### Pragma Directives
```cpp
#pragma once  // Non-standard but widely supported
#pragma pack(push, 1)  // Set structure packing
#pragma pack(pop)      // Restore previous packing
```

## Common Macro Patterns

### Assertions
```cpp
#ifdef NDEBUG
    #define ASSERT(condition) ((void)0)
#else
    #define ASSERT(condition) \
        ((condition) ? (void)0 : \
         (std::cerr << "Assertion failed: " << #condition \
                   << " in " << __FILE__ << ":" << __LINE__ << std::endl, \
          std::abort()))
#endif
```

### Debug Macros
```cpp
#ifdef DEBUG
    #define DBG(x) std::cout << "[DEBUG] " << x << std::endl
    #define DBG_VAR(x) std::cout << "[DEBUG] " << #x << " = " << x << std::endl
#else
    #define DBG(x)
    #define DBG_VAR(x)
#endif
```

### Loop Macros
```cpp
#define FOR_EACH(item, container) \
    for (auto it = (container).begin(); it != (container).end(); ++it) \
        for (bool _flag = true; _flag; _flag = false) \
            for (auto& item = *it; _flag; _flag = false)
```

## Bloomberg-Style Macros

### Naming Conventions
Bloomberg uses specific prefixes:
- `BB_` - Bloomberg-specific macros
- `BSL_` - Bloomberg Standard Library macros
- `BDEM_` - Bloomberg Data Environment macros

### Common Bloomberg Patterns
```cpp
// Bloomberg-style assertions
#define BSLS_ASSERT(condition) \
    Bloomberg::bsls::Assert::invoke(condition, #condition, __FILE__, __LINE__)

// Bloomberg-style logging
#define BALL_LOG_TRACE(stream) \
    Bloomberg::ball::Logger::log(Bloomberg::ball::Severity::e_TRACE, stream)

// Bloomberg-style memory management
#define BSLMA_ALLOCATOR_PTR(type) \
    Bloomberg::bslma::ManagedPtr<type>
```

### Platform Abstraction
```cpp
#ifdef BSL_PLATFORM_OS_WINDOWS
    #define BSL_FORCE_INLINE __forceinline
#elif defined(BSL_PLATFORM_OS_LINUX) || defined(BSL_PLATFORM_OS_DARWIN)
    #define BSL_FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define BSL_FORCE_INLINE inline
#endif
```

## When to Use Macros

### Appropriate Uses
1. **Header guards**: `#ifndef` / `#define` / `#endif`
2. **Conditional compilation**: Platform-specific code
3. **Feature flags**: Enable/disable features at compile time
4. **Debugging aids**: Conditional debug output
5. **Code generation**: Reduce boilerplate (with caution)
6. **Performance-critical**: Zero-overhead abstractions

### Real-World Examples
```cpp
// Header guard (essential)
#ifndef BLOOMBERG_TRADING_ORDER_H
#define BLOOMBERG_TRADING_ORDER_H
// ...
#endif

// Platform abstraction
#ifdef _WIN32
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT
#endif

// Feature flag
#ifdef ENABLE_OPTIMIZATION
    #define INLINE inline
#else
    #define INLINE
#endif
```

## When NOT to Use Macros

### Avoid Macros For
1. **Type-safe constants**: Use `constexpr` instead
2. **Functions**: Use `inline` functions or templates
3. **Type definitions**: Use `using` or `typedef`
4. **Complex logic**: Use proper functions
5. **Scoped constants**: Use `const` or `constexpr`

### Modern Alternatives
```cpp
// BAD: Macro for constant
#define MAX_SIZE 1024

// GOOD: constexpr
constexpr int MAX_SIZE = 1024;

// BAD: Macro function
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// GOOD: Template function
template<typename T>
constexpr T max(const T& a, const T& b) {
    return a > b ? a : b;
}
```

## Modern C++ Alternatives

### constexpr (C++11)
```cpp
// Instead of: #define PI 3.14159
constexpr double PI = 3.141592653589793;

// Compile-time evaluation
constexpr int square(int x) {
    return x * x;
}
constexpr int result = square(5);  // Evaluated at compile time
```

### Templates (C++98+)
```cpp
// Instead of: #define MAX(a, b) ((a) > (b) ? (a) : (b))
template<typename T>
constexpr T max(const T& a, const T& b) {
    return a > b ? a : b;
}
```

### Inline Functions (C++98+)
```cpp
// Instead of: #define SQUARE(x) ((x) * (x))
inline int square(int x) {
    return x * x;
}
```

### if constexpr (C++17)
```cpp
// Instead of: #ifdef DEBUG ... #endif
template<bool Debug = false>
void log(const std::string& message) {
    if constexpr (Debug) {
        std::cout << "[DEBUG] " << message << std::endl;
    }
}
```

## Common Pitfalls

### 1. Missing Parentheses
```cpp
// BAD
#define SQUARE(x) x * x
int result = SQUARE(3 + 2);  // Expands to: 3 + 2 * 3 + 2 = 11 (wrong!)

// GOOD
#define SQUARE(x) ((x) * (x))
int result = SQUARE(3 + 2);  // Expands to: ((3 + 2) * (3 + 2)) = 25 (correct)
```

### 2. Multiple Evaluation
```cpp
// BAD
#define MAX(a, b) ((a) > (b) ? (a) : (b))
int i = 5;
int result = MAX(++i, 10);  // i incremented twice!

// GOOD: Use inline function
inline int max(int a, int b) {
    return a > b ? a : b;
}
```

### 3. Side Effects
```cpp
// BAD
#define PRINT_AND_INCREMENT(x) (std::cout << x++, x)
int i = 5;
int j = PRINT_AND_INCREMENT(i);  // Unpredictable behavior

// GOOD: Use function
int printAndIncrement(int& x) {
    std::cout << x;
    return ++x;
}
```

### 4. Scope Issues
```cpp
// BAD: Macros are global
#define DEBUG 1
void function() {
    // DEBUG is visible everywhere, can't be shadowed
}

// GOOD: Use constexpr
namespace {
    constexpr bool DEBUG = true;
}
```

## Best Practices

### 1. Always Parenthesize
```cpp
#define SAFE_MACRO(x) ((x) + 1)
```

### 2. Use UPPERCASE Names
```cpp
#define MAX_SIZE 1024  // Good
#define max_size 1024  // Bad (looks like variable)
```

### 3. Document Macros
```cpp
/// Maximum buffer size for network operations
/// @note This value is platform-dependent
#define MAX_BUFFER_SIZE 65536
```

### 4. Undefine When Done
```cpp
#define TEMP_MACRO(x) ((x) * 2)
// ... use it ...
#undef TEMP_MACRO
```

### 5. Prefer Modern Alternatives
```cpp
// Prefer constexpr over #define for constants
// Prefer inline functions over function-like macros
// Prefer templates over macros for type-generic code
```

### 6. Use Header Guards
```cpp
#ifndef MY_HEADER_H
#define MY_HEADER_H
// Always use include guards
#endif
```

### 7. Test Macros Thoroughly
```cpp
// Test edge cases
#define TEST_MACRO(x) ((x) * 2)
static_assert(TEST_MACRO(0) == 0);
static_assert(TEST_MACRO(1) == 2);
static_assert(TEST_MACRO(-1) == -2);
```

## Summary

### Key Takeaways
1. **Macros are preprocessor text substitution** - no type checking
2. **Use macros sparingly** - prefer modern C++ alternatives
3. **Always parenthesize** macro parameters and results
4. **Header guards are essential** - use `#ifndef` / `#define` / `#endif`
5. **Conditional compilation** is a valid use case
6. **Modern C++** provides better alternatives: `constexpr`, templates, `inline`

### Bloomberg-Specific
- Use Bloomberg naming conventions (`BB_`, `BSL_`, etc.)
- Follow Bloomberg coding standards for macros
- Understand platform abstraction macros
- Know when to use macros vs. modern alternatives

This guide provides comprehensive coverage of macros at Bloomberg SDE-3 level. Focus on understanding when macros are appropriate and when modern C++ features should be used instead.
