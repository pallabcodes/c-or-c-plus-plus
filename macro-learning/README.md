# C Macros Learning - Preprocessor and Macro Programming

Production-grade curriculum covering fundamentals to advanced macro techniques used in Linux kernel, system libraries, and production C codebases.

## Overview

This module provides comprehensive coverage of C preprocessor macros from basic definitions to advanced metaprogramming techniques. All code follows strict quality standards with proper documentation, error handling, and safety considerations.

## Modules

### 01-fundamentals
Core macro concepts:
- Object-like macros vs function-like macros
- Macro expansion and evaluation
- Common pitfalls and best practices
- Include guards and header organization

### 02-advanced
Advanced macro techniques:
- Variadic macros (C99)
- Stringification (#) and token pasting (##)
- Macro arguments and side effects
- Multi-line macros and do-while patterns

### 03-enterprise
Production patterns from real codebases:
- Linux kernel macros (container_of, ARRAY_SIZE, etc.)
- System library patterns (assert, logging, error handling)
- Configuration macros and feature flags
- Cross-platform compatibility macros

### 04-performance
Performance considerations:
- Inline functions vs macros
- Compile-time evaluation
- Constant folding and optimization
- Zero-cost abstractions

### 05-system
System programming macros:
- Conditional compilation (#if, #ifdef, #ifndef)
- Feature detection and platform macros
- Build system integration
- Debugging and diagnostic macros

### 06-advanced-techniques
Advanced metaprogramming:
- X-macros for code generation
- Macro loops and iteration
- Type-generic macros (C11 _Generic)
- Compile-time assertions and validation

## Code Quality Standards

All code follows production-grade standards:
- **API Documentation**: Every macro includes purpose, parameters, and usage notes
- **Safety**: Proper use of parentheses, do-while(0) patterns, and side-effect prevention
- **Portability**: Platform-specific code properly guarded
- **Function Limits**: Macros under 50 lines, files under 200 lines
- **Compiler Flags**: Strict warnings enabled (`-Wall -Wextra -Werror -Wpedantic`)

## Build Instructions

See BUILD.md for detailed build instructions.

Quick start:
```bash
cd macro-learning
make
```

## Platform Support

- Linux (x86_64, aarch64)
- macOS (x86_64, arm64)
- Standard C99/C11 compliant compilers

## References

- C Standard (ISO/IEC 9899:2011) - Preprocessor specification
- Linux kernel coding style and macros
- GCC/Clang preprocessor documentation
- Production codebases: Linux kernel, glibc, systemd, etc.

