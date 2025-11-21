# Macro Fundamentals

## Scope
Applies to fundamental macro concepts including object-like macros, function-like macros, macro expansion, and basic preprocessor directives.

## Object-Like Macros

### Definition
* Simple text replacement: `#define NAME value`
* No parameters, direct substitution
* Used for constants, configuration values, feature flags
* Example: `#define MAX_SIZE 1024`

### Best Practices
* Use UPPER_CASE naming convention
* Define values that don't change during program execution
* Consider const variables for runtime values
* Document the purpose and units of constants
* Group related constants together

### Common Patterns
* Configuration values
* Magic numbers given meaningful names
* Feature flags for conditional compilation
* Version numbers and build information

## Function-Like Macros

### Definition
* Macros that take parameters: `#define MACRO(param) expansion`
* Parameters are substituted into expansion
* More powerful but more dangerous than object-like macros
* Example: `#define MAX(a, b) ((a) > (b) ? (a) : (b))`

### Parameter Parenthesization
* Always parenthesize parameters in expansion
* Prevents operator precedence issues
* Example: `#define SQUARE(x) ((x) * (x))`
* Without parentheses: `SQUARE(1 + 2)` expands incorrectly

### Expression Parenthesization
* Parenthesize entire macro body for expression macros
* Ensures correct evaluation in larger expressions
* Example: `#define MAX(a, b) ((a) > (b) ? (a) : (b))`
* Without outer parentheses: `MAX(a, b) + 1` may evaluate incorrectly

## Macro Expansion

### Expansion Rules
* Macros are expanded during preprocessing phase
* Expansion happens before compilation
* Can be recursive (with limits)
* Expansion stops when macro name is encountered again
* Use `-E` flag to see preprocessor output

### Multiple Evaluation
* Parameters may be evaluated multiple times
* Can cause side effects: `MAX(++a, ++b)` evaluates increments twice
* Document this behavior
* Consider inline functions for side-effect-sensitive cases

### Stringification
* `#` operator converts parameter to string literal
* Example: `#define STR(x) #x` makes `STR(hello)` become `"hello"`
* Useful for debugging and logging macros

## Include Guards

### Purpose
* Prevent multiple inclusion of header files
* Avoid redefinition errors
* Standard pattern for header organization

### Implementation
```c
#ifndef HEADER_NAME_H
#define HEADER_NAME_H
// header content
#endif
```

### Modern Alternative
* `#pragma once` (non-standard but widely supported)
* Simpler but less portable
* Consider compiler support before using

## Common Pitfalls

### Operator Precedence
* Always parenthesize macro parameters
* Parenthesize macro body for expressions
* Test with various operator combinations

### Side Effects
* Macros evaluate parameters multiple times
* Avoid macros with side-effect parameters
* Document when side effects occur
* Provide inline function alternatives

### Scope Issues
* Macros don't respect C scope rules
* Can shadow variables unexpectedly
* Use descriptive macro names
* Consider namespace prefixes

## Implementation Standards

### Documentation
* Document macro purpose and usage
* Explain parameters and return behavior
* Note side effects and multiple evaluation
* Provide usage examples
* Reference related macros

### Testing
* Test with various argument types
* Test edge cases (zero, negative, maximum values)
* Test with side-effect expressions
* Verify expansion with preprocessor output
* Test in different contexts

## Code Examples

### Safe Min Macro
```c
// Thread-safety: N/A (compile-time)
// Ownership: N/A (compile-time)
// Invariants: a and b must be comparable
// Failure modes: Multiple evaluation of parameters
// Side effects: Parameters evaluated twice
#define MIN(a, b) ((a) < (b) ? (a) : (b))
```

### Array Size Macro
```c
// Thread-safety: N/A (compile-time)
// Ownership: N/A (compile-time)
// Invariants: arr must be array (not pointer)
// Failure modes: Undefined behavior if arr is pointer
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
```

## Related Topics
* Advanced Techniques: Variadic macros, token pasting
* Enterprise Patterns: Production macro patterns
* Performance Optimization: Inline functions vs macros

