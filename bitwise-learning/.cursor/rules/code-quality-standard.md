# Code Quality Standards for Bitwise Operations

## Scope
Applies to all bitwise manipulation code in this directory. Extends repository root rules.

## Code Review Context
This code is expected to undergo line by line scrutiny by principal level reviewers from top tier companies like Google, Bloomberg, Uber, and Amazon. Every line must demonstrate production grade quality, clarity, and maintainability comparable to internal implementations at these companies.

## Code Quality Dimensions

### Readability
* Code must be immediately understandable without extensive documentation
* Variable and function names must be self documenting and convey intent clearly
* Avoid abbreviations unless they are industry standard or clearly documented (e.g., SWAR, LSB, MSB, CRC, SIMD)
* Use consistent naming conventions throughout all files
* Group related operations logically with clear separation between sections
* Use whitespace strategically to improve visual structure
* Follow bitwise specific naming conventions (e.g., mask, shift, rotate, isolate)

### Maintainability
* Code must be easy to modify and extend without breaking existing functionality
* Functions must have single, well defined responsibilities
* Avoid deep nesting; prefer early returns and guard clauses
* Minimize coupling between bitwise operation components
* Use constants and configuration values instead of magic numbers
* Document assumptions and dependencies clearly
* Design for extensibility with template or generic patterns where appropriate

### Debuggability
* Error messages must provide actionable information for debugging
* Use structured logging with appropriate log levels
* Include context in error messages to trace execution paths
* Ensure all error conditions are testable and reproducible
* Provide clear failure modes that aid in root cause analysis
* Use assertion like checks for invariants that should never occur
* Include bit position and value context in error messages

## Size Constraints

### Function Length
* Maximum function length: 50 lines including comments and whitespace
* If a function exceeds 50 lines, it must be refactored into smaller, focused functions
* Each function should accomplish one clearly defined task
* Complex operations like hash mixing should be decomposed into helper functions with descriptive names

### File Length
* Maximum file length: 200 lines including comments and whitespace
* Files exceeding 200 lines should be split into logical modules
* Each file should have a single, well defined purpose
* Related functions and data structures should be grouped within appropriate header and implementation files

### Cyclomatic Complexity
* Maximum cyclomatic complexity per function: 10
* Functions with complexity over 10 must be refactored to reduce branching
* Use early returns and guard clauses to reduce nesting
* Extract complex conditions into well named boolean functions
* Bit manipulation code should favor bitwise operations over branching

## Review Standards
* Assume line by line scrutiny by principal level reviewers
* Favor clarity over cleverness in all implementations
* Document tradeoffs and rationale for non trivial decisions
* Keep code minimal and focused on demonstrating single concepts
* Reference research papers when implementing algorithms from academic literature

## Style Guidelines
* Keep examples minimal and focused on one bitwise concept per file
* Add concise comments only where intent or bit manipulation logic is non obvious
* Use meaningful names for bit positions, masks, and operations
* Prefer early returns and shallow nesting to reduce complexity
* Follow bitwise terminology consistently (e.g., mask, shift, rotate, isolate, extract)

## Documentation Requirements
* Explain bitwise algorithms being used with brief context
* Document performance characteristics and complexity analysis
* Explain platform specific optimizations and their purpose
* Keep functions focused on single bitwise operations
* Reference research papers when implementing academic algorithms
* Document invariants and constraints for bit manipulation operations
* Include thread safety, ownership, invariants, and failure mode documentation for all functions

## Performance Requirements
* Profile critical paths and record findings
* Optimize for modern hardware (SIMD, cache awareness, CPU intrinsics)
* Use appropriate compiler intrinsics for platform specific optimizations
* Minimize allocations in hot paths
* Consider vectorization opportunities for bulk bit operations
* Document performance characteristics of different implementations

## Integration with Other Rules
This file provides general quality standards. For specific guidance, refer to:
* `fundamentals.md` for basic bit operations
* `advanced-techniques.md` for SWAR and advanced algorithms
* `enterprise-patterns.md` for production patterns from top companies
* `performance-optimization.md` for SIMD and CPU intrinsics
* `system-programming.md` for system level bit manipulation
* `advanced-data-structures.md` for succinct structures and compression
* `memory-safety-portability.md` for safety and portability concerns
* `testing-validation.md` for test coverage requirements

