# Macro Metaprogramming

## Scope
Applies to advanced metaprogramming techniques including X-macros, macro loops, type-generic macros, and compile-time code generation.

## X-Macros

### Concept
* Technique for code generation using macro expansion
* Define data once, generate multiple code patterns
* Reduces duplication and errors
* Used for enums, switch statements, serialization

### Basic Pattern
```c
#define FRUITS \
    X(APPLE) \
    X(ORANGE) \
    X(BANANA)

#define X(name) name,
enum fruit { FRUITS };
#undef X

#define X(name) #name,
const char *fruit_names[] = { FRUITS };
#undef X
```

### Common Uses
* Enum and string conversion
* Switch statement generation
* Structure definitions
* Serialization code
* Test case generation

### Advantages
* Single source of truth
* Consistent code generation
* Easy to maintain
* Reduces errors

## Macro Loops and Iteration

### Recursive Macros
* Macros that expand to themselves (with limits)
* Useful for iteration and code generation
* Limited recursion depth
* Can cause preprocessor errors

### Pattern Matching
* Match on argument count
* Generate different code based on pattern
* Complex but powerful
* Used in advanced metaprogramming

### Code Generation
* Generate repetitive code
* Maintain consistency
* Reduce manual errors
* Trade compilation time for correctness

## Type-Generic Macros (C11)

### _Generic Keyword
* C11 feature for type-based dispatch
* Compile-time type selection
* Type-safe generic operations
* Alternative to macro overloading

### Basic Usage
```c
#define type_max(x) _Generic((x), \
    int: max_int, \
    double: max_double, \
    default: max_generic \
)(x)
```

### Advantages
* Type safety
* Compile-time selection
* Better than macro overloading
* Standard C feature

### Limitations
* C11 required
* Limited to compile-time types
* Cannot handle runtime types
* More verbose than C++ templates

## Compile-Time Assertions

### Static Assert (C11)
* `_Static_assert` for compile-time checks
* Standard C11 feature
* Zero runtime cost
* Better error messages

### Macro-Based Assertions
* BUILD_BUG_ON pattern from Linux kernel
* Works in C99 and earlier
* Uses sizeof trick
* Less clear error messages

### Usage Patterns
* Type size validation
* Structure layout checks
* Constant validation
* Invariant checking

## Advanced Patterns

### Macro Metaprogramming
* Generate complex code structures
* Reduce boilerplate
* Maintain consistency
* Advanced technique, use carefully

### Code Generation
* Generate functions, structures, tests
* Maintain single source of truth
* Reduce manual coding
* Trade complexity for maintainability

### Reflection-Like Features
* Generate metadata from code
* String conversion from identifiers
* Type information generation
* Limited compared to true reflection

## Code Quality Standards

### Documentation
* Explain metaprogramming techniques clearly
* Document code generation patterns
* Note limitations and trade-offs
* Provide usage examples
* Reference advanced resources

### Complexity Management
* Keep metaprogramming readable
* Use helper macros for clarity
* Document expansion behavior
* Consider alternatives (code generators)

### Testing
* Test generated code thoroughly
* Verify all code paths
* Test edge cases
* Verify preprocessor output
* Compare against manual implementation

## Best Practices

### When to Use Advanced Techniques
* Significant code duplication
* Need for consistency
* Complex code generation
* Maintenance benefits outweigh complexity

### When to Avoid
* Simple cases don't need metaprogramming
* Consider external code generators
* Prefer standard language features when available
* Balance complexity vs benefit

## Related Topics
* Fundamentals: Basic macro concepts
* Enterprise Patterns: Production X-macro usage
* Performance Optimization: Compile-time code generation

