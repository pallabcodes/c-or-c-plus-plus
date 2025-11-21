# Advanced Macro Techniques

## Examples

### 01_variadic_macros.c
Demonstrates variadic macros (C99):
- Basic variadic macro usage
- Logging macros with prefixes
- Debug macros with conditional compilation
- Assertion macros with custom messages
- Function call wrappers

### 02_stringification.c
Demonstrates stringification (# operator):
- Converting macro parameters to strings
- Variable name and value printing
- Error message generation
- Function name logging
- Type name stringification

### 03_token_pasting.c
Demonstrates token pasting (## operator):
- Basic token concatenation
- Variable name generation
- Function name generation
- Type name generation
- Getter/setter generation

### 04_multiline_macros.c
Demonstrates multi-line macros:
- do-while(0) pattern for statement macros
- Why do-while(0) is necessary
- Resource management macros
- Error handling macros
- Statement expressions (GCC extension)

## Key Concepts
- Variadic macros with __VA_ARGS__
- Stringification for debugging
- Token pasting for code generation
- Safe multi-line macro patterns

