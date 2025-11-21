# Safety Considerations for Macros

## Scope
Applies to safety concerns, common pitfalls, undefined behavior, and best practices for safe macro usage.

## Common Pitfalls

### Operator Precedence Issues
* Always parenthesize macro parameters
* Parenthesize entire macro body for expressions
* Test with various operator combinations
* Example: `#define SQUARE(x) ((x) * (x))`

### Multiple Evaluation
* Macro parameters evaluated each time used
* Can cause side effects: `MAX(++a, ++b)`
* Document this behavior clearly
* Consider inline functions for side-effect-sensitive cases

### Scope and Name Collision
* Macros don't respect C scope rules
* Can shadow variables unexpectedly
* Use descriptive, namespaced macro names
* Consider prefixing macros (e.g., `MYLIB_MAX`)

### Type Safety
* Macros don't provide type checking
* Can accept incompatible types
* Use type-generic macros (C11 _Generic) when possible
* Consider inline functions for better type safety

## Undefined Behavior

### Invalid Expansions
* Macros can expand to invalid code
* Preprocessor doesn't validate C syntax
* Test macro expansions thoroughly
* Use compiler warnings

### Recursive Expansion
* Macros expanding to themselves cause errors
* Limited recursion depth
* Be careful with recursive patterns
* Test expansion limits

### Token Pasting Issues
* Invalid token pasting causes errors
* Result must be valid preprocessing token
* Test edge cases
* Validate generated identifiers

## Best Practices

### Parenthesization Rules
1. Parenthesize all macro parameters
2. Parenthesize entire macro body for expressions
3. Test with various operator combinations
4. Verify with preprocessor output

### do-while(0) Pattern
* Use for statement-like macros
* Ensures correct semicolon usage
* Works in if-else statements
* Industry standard pattern

### Documentation Requirements
* Document side effects clearly
* Note multiple evaluation behavior
* Explain type requirements
* Provide usage examples
* Warn about common pitfalls

## Error Prevention

### Compile-Time Validation
* Use static assertions
* Validate macro arguments where possible
* Use BUILD_BUG_ON patterns
* Catch errors early

### Testing Strategies
* Test with various argument types
* Test edge cases
* Verify preprocessor output
* Test in different contexts
* Compare against alternatives

### Code Review
* Review macro expansions
* Check parenthesization
* Verify side effect handling
* Validate type usage
* Check documentation

## Portability Concerns

### Standard Compliance
* Use standard C features when possible
* Document compiler-specific extensions
* Provide fallbacks for non-standard features
* Test with multiple compilers

### Platform Differences
* Preprocessor behavior can vary
* Test on multiple platforms
* Use feature detection
* Document platform requirements

## Related Topics
* Fundamentals: Basic safety practices
* Advanced Techniques: Safety in metaprogramming
* Testing: Validation strategies

