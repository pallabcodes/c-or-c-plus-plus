# Advanced Macro Techniques

## Scope
Applies to advanced macro features including variadic macros, stringification, token pasting, and complex macro patterns.

## Variadic Macros (C99)

### Definition
* Macros that accept variable number of arguments
* Syntax: `#define MACRO(...) expansion` or `#define MACRO(param, ...) expansion`
* Access arguments with `__VA_ARGS__`
* Requires C99 or later standard

### Basic Usage
```c
#define DEBUG(...) printf(__VA_ARGS__)
DEBUG("Value: %d\n", x);
```

### Named Variadic Parameters
* C23 feature: `#define MACRO(param, args...) expansion`
* Access with `args` instead of `__VA_ARGS__`
* More readable but limited compiler support

### Common Patterns
* Logging macros with format strings
* Assertion macros with custom messages
* Wrapper macros for function calls
* Debug macros with conditional compilation

## Stringification (#)

### Basic Stringification
* `#` operator converts macro parameter to string literal
* Example: `#define STR(x) #x` makes `STR(hello)` become `"hello"`
* Useful for debugging, logging, error messages

### Stringification Rules
* Whitespace is collapsed to single spaces
* Leading/trailing whitespace is removed
* String and character literals are preserved
* Escape sequences are handled correctly

### Advanced Usage
* Combine with variadic macros for formatted strings
* Create error messages with variable names
* Generate code with string literals
* Debug macros that print variable names and values

### Code Example
```c
#define PRINT_VAR(x) printf(#x " = %d\n", x)
PRINT_VAR(my_variable);  // Prints: my_variable = 42
```

## Token Pasting (##)

### Basic Token Pasting
* `##` operator concatenates tokens
* Example: `#define CONCAT(a, b) a##b` makes `CONCAT(var, 1)` become `var1`
* Useful for generating identifiers

### Common Patterns
* Generate function names: `FUNC_NAME_##suffix`
* Create type names: `TYPE_##name`
* Build enum values: `ENUM_##value`
* Generate variable names programmatically

### Token Pasting Rules
* Both sides must be valid preprocessing tokens
* Result must be valid token
* Cannot paste into string literals
* Useful for code generation

### Code Example
```c
#define DECLARE_VAR(type, name) type var_##name
DECLARE_VAR(int, counter);  // Expands to: int var_counter;
```

## Multi-Line Macros

### Statement Macros
* Macros that expand to statements
* Use `do { ... } while(0)` pattern
* Ensures macro can be used like function call
* Prevents issues with if-else statements

### do-while(0) Pattern
```c
#define MACRO_STMT(x) do { \
    statement1(x); \
    statement2(x); \
} while(0)
```

### Why do-while(0)?
* Forces semicolon after macro call
* Works correctly in if-else statements
* Prevents accidental statement issues
* Industry standard pattern

## Macro Arguments and Side Effects

### Multiple Evaluation Problem
* Macro parameters evaluated each time used in expansion
* `MAX(++a, ++b)` increments both variables twice
* Document this behavior clearly
* Consider inline functions for side-effect-sensitive cases

### Statement Expressions (GCC Extension)
* `({ ... })` creates expression from statements
* Returns value from last statement
* GCC/Clang extension, not standard C
* Useful for complex macros

### Code Example
```c
#define MAX(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a > _b ? _a : _b; \
})
```

## Advanced Patterns

### Macro Overloading
* Use variadic macros with different argument counts
* Pattern match on argument count
* Requires helper macros
* Complex but powerful

### Recursive Macros
* Macros that expand to themselves (with limits)
* Useful for iteration and code generation
* Limited recursion depth
* Can cause preprocessor errors if not careful

### Conditional Expansion
* Use `#if` and `#ifdef` within macros
* Generate different code based on conditions
* Useful for platform-specific code
* Compile-time configuration

## Code Quality Standards

### Documentation
* Explain variadic macro argument handling
* Document stringification behavior
* Note token pasting rules and limitations
* Explain side effect concerns
* Provide usage examples

### Error Handling
* Validate macro usage where possible
* Use compile-time assertions
* Provide clear error messages
* Document undefined behavior cases

### Performance
* Consider preprocessor performance
* Avoid deeply nested macro expansions
* Profile compilation time impact
* Document performance characteristics

## Testing Requirements
* Test with various argument counts
* Test stringification with special characters
* Test token pasting edge cases
* Verify side effect behavior
* Test multi-line macro expansion
* Check preprocessor output

## Related Topics
* Enterprise Patterns: Production variadic macro patterns
* Performance Optimization: Macro vs inline function trade-offs
* Metaprogramming: X-macros and advanced code generation
