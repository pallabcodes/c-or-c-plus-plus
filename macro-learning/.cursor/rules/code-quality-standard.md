# Code Quality Standards for C Macros

## Scope
Applies to all macro code in this directory. Extends repository root rules.

## Code Review Context
This code is expected to undergo line by line scrutiny by principal level reviewers from top tier companies like Google, Linux kernel maintainers, and system library developers. Every macro must demonstrate production grade quality, clarity, and maintainability comparable to internal implementations at these organizations.

## Code Quality Dimensions

### Readability
* Macros must be immediately understandable without extensive documentation
* Macro names must be self documenting and convey intent clearly
* Avoid abbreviations unless they are industry standard or clearly documented (e.g., MAX, MIN, ARRAY_SIZE)
* Use consistent naming conventions throughout all files
* Group related macros logically with clear separation between sections
* Use whitespace strategically to improve visual structure
* Follow macro naming conventions (UPPER_CASE for macros)

### Maintainability
* Macros must be easy to modify and extend without breaking existing functionality
* Each macro should have single, well defined purpose
* Avoid deep nesting in macro definitions
* Minimize coupling between macros
* Use constants and configuration values instead of magic numbers
* Document assumptions and dependencies clearly
* Design for extensibility with parameterized macros where appropriate

### Debuggability
* Macro expansion should be traceable with preprocessor output
* Use meaningful error messages in macro-generated code
* Include context in diagnostic macros
* Ensure all error conditions are testable and reproducible
* Provide clear failure modes that aid in root cause analysis
* Use compile-time assertions for invariants
* Document macro expansion behavior

## Size Constraints

### Macro Length
* Maximum macro definition length: 50 lines including comments and whitespace
* If a macro exceeds 50 lines, consider refactoring into multiple macros or inline functions
* Each macro should accomplish one clearly defined task
* Complex operations should be decomposed into helper macros with descriptive names

### File Length
* Maximum file length: 200 lines including comments and whitespace
* Files exceeding 200 lines should be split into logical modules
* Each file should have a single, well defined purpose
* Related macros should be grouped within appropriate header files

### Complexity
* Maximum complexity per macro: 10 (consider nesting, conditionals)
* Macros with complexity over 10 must be refactored to reduce branching
* Use helper macros to reduce nesting
* Extract complex conditions into well named macros
* Prefer simple, linear macro definitions

## Review Standards
* Assume line by line scrutiny by principal level reviewers
* Favor clarity over cleverness in all macro definitions
* Document tradeoffs and rationale for non trivial decisions
* Keep macros minimal and focused on demonstrating single concepts
* Reference C standard and production implementations when applicable

## Style Guidelines
* Keep examples minimal and focused on one macro concept per file
* Add concise comments only where intent or macro expansion is non obvious
* Use meaningful names for macros and parameters
* Prefer simple macro definitions to reduce complexity
* Follow C macro naming conventions (UPPER_CASE)
* Always parenthesize macro parameters and expressions

## Documentation Requirements
* Explain macro purpose and usage with brief context
* Document parameters and return values (for function-like macros)
* Explain expansion behavior when non obvious
* Note side effects and multiple evaluation concerns
* Reference C standard sections when applicable
* Document invariants and constraints for macro usage
* Include usage examples in comments

## Safety Requirements

### Parenthesization
* Always parenthesize macro parameters in expansion
* Parenthesize entire macro body for expression macros
* Prevent operator precedence issues
* Example: `#define MAX(a, b) ((a) > (b) ? (a) : (b))`

### Side Effects
* Document when macros evaluate parameters multiple times
* Consider using statement expressions or inline functions for complex cases
* Warn users about side effect issues in documentation
* Provide side-effect-safe alternatives when possible

### Type Safety
* Consider type safety in macro design
* Use type-generic macros (C11 _Generic) when appropriate
* Document type requirements clearly
* Consider using inline functions for better type checking

## Performance Requirements
* Profile macro expansion impact on compilation time
* Consider inline functions vs macros trade-offs
* Optimize for compile-time evaluation when possible
* Document performance characteristics
* Benchmark macro vs function alternatives

## Integration with Other Rules
This file provides general quality standards. For specific guidance, refer to:
* `fundamentals.md` for basic macro operations
* `advanced-techniques.md` for variadic macros and token operations
* `enterprise-patterns.md` for production patterns from Linux kernel and system libraries
* `performance-optimization.md` for performance considerations
* `system-programming.md` for system level macros
* `advanced-techniques.md` for metaprogramming techniques
* `safety-considerations.md` for safety and portability concerns
* `testing-validation.md` for test coverage requirements

