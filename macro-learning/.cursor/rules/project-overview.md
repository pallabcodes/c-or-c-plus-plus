# C Macros Learning Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. This macro implementation must meet enterprise production standards suitable for principal level engineering review and must be comparable to top tier implementations used in production systems at these companies, particularly the Linux kernel and system libraries.

## Purpose
This module covers the design and implementation of production grade C preprocessor macros and macro programming techniques. All code must follow production grade standards suitable for principal level code review and must be production ready for deployment in high performance systems requiring compile-time optimizations, code generation, and low level abstractions.

## Scope
* Applies to all C code in macro learning directory
* Extends repository root rules defined in the root `.cursor/rules/` files
* Covers all aspects of macro programming from fundamentals to advanced metaprogramming
* Code quality standards align with expectations from top tier companies like Google, Linux kernel, glibc, and systemd

## Top Tier Product Comparisons

### Linux Kernel
* Production tested macros used in billions of devices
* container_of macro for type-safe pointer arithmetic
* ARRAY_SIZE for compile-time array size calculation
* BUILD_BUG_ON for compile-time assertions
* min/max macros with type safety
* Feature detection and conditional compilation

### glibc (GNU C Library)
* Standard library macro implementations
* Error handling macros (errno, perror patterns)
* Type-generic math macros
* Portability macros for cross-platform code
* Performance critical macro optimizations

### systemd
* Modern system service manager macros
* Logging and diagnostic macros
* Configuration and feature flags
* Cross-platform compatibility macros
* Production tested in enterprise environments

### Google Abseil
* Modern C++ macro patterns (though C++ focused)
* Configuration macros for feature detection
* Compile-time validation macros
* Performance optimization macros

### Redis
* Embedded system macro patterns
* Memory efficient macro implementations
* Configuration macros
* Platform abstraction macros

## Macro Architecture Components

### Core Components
1. Fundamentals: Basic macro definitions, expansion, object-like vs function-like
2. Advanced Techniques: Variadic macros, stringification, token pasting
3. Enterprise Patterns: Linux kernel patterns, system library macros, production patterns
4. Performance Optimization: Inline functions vs macros, compile-time evaluation
5. System Programming: Conditional compilation, feature detection, platform macros
6. Advanced Techniques: X-macros, macro loops, type-generic macros, compile-time assertions

## Code Quality Standards
All macro code must demonstrate:
* Comprehensive documentation with usage examples
* Proper use of parentheses to prevent operator precedence issues
* do-while(0) patterns for statement-like macros
* Side-effect prevention and multiple evaluation awareness
* Portability through feature detection and platform guards
* Testing of edge cases and boundary conditions
* Performance considerations vs inline functions
* Research backed implementations with proper citations

## Reference Material
* See existing examples in system programming directories for low level patterns
* Reference C standard (ISO/IEC 9899:2011) preprocessor specification
* Study Linux kernel source code for production macro patterns
* Review glibc and systemd implementations
* Benchmark against alternatives (inline functions, templates in C++)

## Related Rules
Refer to the other rule files in this directory for specific guidance on fundamentals, advanced techniques, enterprise patterns, performance optimization, system programming, advanced techniques, safety considerations, and testing.

