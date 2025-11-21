# Learning Resources and References

## Scope
Defines learning resources, foundational concepts, C standard references, and implementation examples for macro programming.

## Foundational Knowledge

### Required Understanding
* C preprocessor phases and operation
* Tokenization and macro expansion rules
* Preprocessing directives
* Compilation process (preprocessing, compilation, linking)

### C Standard References
* ISO/IEC 9899:2011 (C11) - Preprocessor specification
* ISO/IEC 9899:1999 (C99) - Variadic macros
* GCC Preprocessor Manual
* Clang Preprocessor Documentation

## Key Concepts

### Preprocessor Phases
1. Trigraph replacement
2. Line splicing
3. Tokenization
4. Macro expansion
5. Directive handling
6. Stringization and token pasting

### Macro Expansion Rules
* Argument substitution
* Stringification and token pasting
* Rescanning and further replacement
* Recursion limits
* Standard defined behavior

## Production References

### Linux Kernel
* kernel.org source code
* Macros in include/linux/ directory
* container_of, ARRAY_SIZE, BUILD_BUG_ON
* Production tested in billions of devices

### glibc (GNU C Library)
* gnu.org/software/libc
* Standard library macro implementations
* Portability macros
* System library patterns

### systemd
* freedesktop.org/systemd
* Modern system service macros
* Configuration macros
* Production system patterns

## Learning Path

### Fundamentals (01-fundamentals)
* Start with object-like macros
* Learn function-like macros
* Understand expansion rules
* Practice with simple examples
* Study include guards

### Advanced Techniques (02-advanced)
* Learn variadic macros
* Study stringification and token pasting
* Understand multi-line macros
* Practice do-while(0) pattern
* Learn about side effects

### Enterprise Patterns (03-enterprise)
* Study Linux kernel macros
* Learn system library patterns
* Understand production practices
* Review real-world implementations
* Practice with production patterns

### Performance (04-performance)
* Compare macros vs inline functions
* Learn compile-time evaluation
* Understand optimization trade-offs
* Profile macro usage
* Study performance patterns

### System Programming (05-system)
* Learn conditional compilation
* Study feature detection
* Understand platform macros
* Practice build system integration
* Review debugging macros

### Advanced Techniques (06-advanced-techniques)
* Study X-macros
* Learn macro metaprogramming
* Understand type-generic macros
* Practice code generation
* Review advanced patterns

## Tools and Resources

### Compiler Tools
* `gcc -E` for preprocessor output
* `clang -E` for preprocessor output
* Compiler explorer (godbolt.org)
* Preprocessor debugging

### Documentation
* C Standard documents
* Compiler documentation (GCC, Clang)
* Linux kernel coding style
* System library documentation

### Online Resources
* Stack Overflow (macro questions)
* GitHub (open source implementations)
* C reference websites
* Preprocessor tutorials

## Best Practices from Production

### Linux Kernel Style
* Clear macro naming
* Comprehensive documentation
* Type safety considerations
* Performance optimization
* Extensive testing

### System Library Patterns
* Portability focus
* Standard compliance
* Feature detection
* Graceful degradation
* Cross-platform support

## Related Topics
* All other rule files reference learning resources
* Code examples should reference standards and implementations
* Documentation should cite sources

