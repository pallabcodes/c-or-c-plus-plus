# Complete C++ Macros Guide - Bloomberg SDE-3 Level

## Overview

This comprehensive guide covers **everything** about C++ macros at the level expected of Bloomberg SDE-3 candidates. The content includes practical examples with JavaScript/TypeScript analogies to help developers coming from those languages.

## Files Created

### 📚 Core Documentation
- **`README.md`** - Complete theoretical foundation and concepts
- **`SUMMARY.md`** - This quick reference guide

### 💡 Practical Examples
- **`basic_examples.cpp`** - Fundamental macro syntax and usage
- **`advanced_examples.cpp`** - Variadic macros, stringification, concatenation
- **`preprocessor_directives.cpp`** - #ifdef, #ifndef, #pragma, conditional compilation
- **`modern_alternatives.cpp`** - constexpr, templates, inline functions

### 🏢 Bloomberg-Style Patterns
- **`bloomberg_patterns.cpp`** - Real Bloomberg macro patterns and conventions
- **`macro_pitfalls.cpp`** - Common mistakes and how to avoid them

## Key Concepts by Category

### 🔍 **What Are Macros?**
- **Text substitution** before compilation (preprocessor)
- **No type checking** - pure text replacement
- **No scope** - macros are global
- **Compile-time only** - don't exist at runtime

### 📝 **Basic Macros**
```cpp
// Object-like macros (constants)
#define PI 3.14159
#define MAX_SIZE 1024

// Function-like macros
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SQUARE(x) ((x) * (x))
```

### 🏗️ **Advanced Features**
```cpp
// Stringification
#define STRINGIFY(x) #x
PRINT_VAR(value);  // Outputs: value = 42

// Token concatenation
#define CONCAT(a, b) a##b
MAKE_VAR(counter, int);  // Creates: int counter_var;

// Variadic macros
#define LOG(...) printf(__VA_ARGS__)
LOG("Value: %d\n", 42);
```

### 🔎 **Preprocessor Directives**
```cpp
// Include guards
#ifndef HEADER_H
#define HEADER_H
#endif

// Conditional compilation
#ifdef DEBUG
    #define LOG(x) std::cout << x << std::endl
#else
    #define LOG(x)
#endif

// Platform detection
#ifdef _WIN32
    // Windows code
#elif defined(__linux__)
    // Linux code
#endif
```

### 🏛️ **Bloomberg-Style Patterns**
```cpp
// Bloomberg naming conventions
#define BB_MAX_ORDERS 10000
#define BSL_ASSERT(condition) assert(condition)
#define BSLS_ASSERT(condition) Bloomberg::bsls::Assert::invoke(...)
#define BALL_LOG_INFO(stream) Bloomberg::ball::Logger::log(...)

// Platform abstraction
#ifdef BSL_PLATFORM_OS_WINDOWS
    #define BSL_FORCE_INLINE __forceinline
#else
    #define BSL_FORCE_INLINE __attribute__((always_inline)) inline
#endif
```

## Critical Best Practices

### ✅ **DOs**
- **Always parenthesize** macro parameters and results
- **Use uppercase names** for macros (MAX, not max)
- **Use do-while(0)** for multi-line macros
- **Document macros** thoroughly
- **Use header guards** in all headers
- **Prefer modern C++** alternatives when possible

### ❌ **DON'Ts**
- **Don't forget parentheses** - causes operator precedence issues
- **Don't use macros for functions** - use inline functions instead
- **Don't use macros for constants** - use constexpr instead
- **Don't create side effects** - macros evaluate multiple times
- **Don't use lowercase names** - conflicts with functions

## Common Pitfalls to Avoid

1. **Missing Parentheses**: `#define SQUARE(x) x * x` → `SQUARE(3+2)` = 11 (wrong!)
2. **Multiple Evaluation**: `MAX(++i, 10)` increments i multiple times
3. **Side Effects**: Macros can have unexpected side effects
4. **Operator Precedence**: Without parentheses, order matters
5. **Scope Issues**: Macros are global, can't be scoped
6. **Type Safety**: Macros don't check types
7. **Name Collisions**: Macros can conflict with functions
8. **Complex Logic**: Hard to debug macro code
9. **Undefined Behavior**: Using undefined macros causes errors

## Modern C++ Alternatives

### constexpr (C++11+)
```cpp
// Instead of: #define PI 3.14159
constexpr double PI = 3.141592653589793;

// Instead of: #define SQUARE(x) ((x) * (x))
constexpr int square(int x) {
    return x * x;
}
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
// Instead of: #define MIN(a, b) ((a) < (b) ? (a) : (b))
inline int min(int a, int b) {
    return a < b ? a : b;
}
```

### if constexpr (C++17+)
```cpp
// Instead of: #ifdef DEBUG ... #endif
template<bool Debug = false>
void log(const std::string& message) {
    if constexpr (Debug) {
        std::cout << message << std::endl;
    }
}
```

## When to Use Macros

### Appropriate Uses
1. **Header guards**: `#ifndef` / `#define` / `#endif`
2. **Conditional compilation**: Platform-specific code
3. **Feature flags**: Enable/disable features at compile time
4. **Debugging aids**: Conditional debug output
5. **Code generation**: Reduce boilerplate (with caution)

### When NOT to Use Macros
1. **Constants**: Use `constexpr` instead
2. **Functions**: Use `inline` functions or templates
3. **Type definitions**: Use `using` or `typedef`
4. **Complex logic**: Use proper functions
5. **Scoped constants**: Use `const` or `constexpr`

## Bloomberg-Specific Patterns

### Naming Conventions
- `BB_` - Bloomberg-specific macros
- `BSL_` - Bloomberg Standard Library macros
- `BSLS_` - Bloomberg Standard Library Support macros
- `BDEM_` - Bloomberg Data Environment macros
- `BALL_` - Bloomberg Application Logging Library macros

### Common Bloomberg Macros
```cpp
BSLS_ASSERT(condition)        // Bloomberg assertions
BALL_LOG_INFO(stream)         // Bloomberg logging
BSL_FORCE_INLINE              // Platform-specific inline
BSL_PLATFORM_OS_WINDOWS       // Platform detection
BSL_ENABLE_FEATURE_X          // Feature flags
```

## JavaScript/TypeScript Equivalents

| C++ Macro | JavaScript/TypeScript Equivalent |
|-----------|-----------------------------------|
| `#define PI 3.14` | `const PI = 3.14;` |
| `#define MAX(a,b) ...` | `const max = (a, b) => a > b ? a : b;` |
| `#ifdef DEBUG` | `if (process.env.NODE_ENV !== 'production')` |
| `#include "file.h"` | `import './file.js'` |
| `__FILE__`, `__LINE__` | `__filename`, `__dirname` |
| Variadic macros | Rest parameters `(...args)` |
| Stringification `#x` | Template literals `` `${variable}` `` |

## Interview Preparation Tips

### Key Topics to Master
1. **Macro expansion** mechanics and order
2. **Preprocessor directives** (#ifdef, #ifndef, #pragma)
3. **Common pitfalls** (parentheses, multiple evaluation)
4. **Modern alternatives** (constexpr, templates, inline)
5. **Bloomberg coding standards** for macros

### Common Interview Questions
- Why do macros exist?
- What's wrong with `#define SQUARE(x) x * x`?
- When should you use macros vs. constexpr?
- How do you prevent multiple evaluation?
- What are header guards and why are they needed?

## Quick Reference

### Creating Macros
```cpp
// Object-like
#define CONSTANT value

// Function-like
#define MACRO(param) ((param) * 2)

// Multi-line
#define MACRO(x) \
    do { \
        /* code */ \
    } while(0)
```

### Preprocessor Directives
```cpp
#include "file.h"        // Include file
#define MACRO value      // Define macro
#undef MACRO            // Undefine macro
#ifdef MACRO            // If defined
#ifndef MACRO           // If not defined
#if condition           // Conditional
#pragma directive       // Compiler-specific
#error message          // Generate error
#warning message        // Generate warning
```

### Modern Alternatives
```cpp
constexpr double PI = 3.14;              // Instead of #define
constexpr int square(int x) { ... }     // Instead of macro function
template<typename T> T max(...) { ... } // Instead of type-unsafe macro
inline int min(...) { ... }             // Instead of macro
if constexpr (condition) { ... }        // Instead of #ifdef
```

This guide provides comprehensive coverage of macros at Bloomberg SDE-3 level. Study each example file thoroughly and practice applying these patterns in your code.
