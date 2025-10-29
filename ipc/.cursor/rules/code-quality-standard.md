# Code Quality Standards for IPC

## Scope
Applies to all IPC code in this directory. Extends repository root rules.

## Code Review Context
This code is expected to undergo line by line scrutiny by principal level reviewers from top tier companies like Google, Bloomberg, PayPal, Uber, Amazon, and Stripe. Every line must demonstrate production grade quality, clarity, and maintainability.

## Code Quality Dimensions

### Readability
* Code must be immediately understandable without extensive documentation
* Variable and function names must be self documenting and convey intent clearly
* Avoid abbreviations unless they are industry standard or clearly documented
* Use consistent naming conventions throughout all files
* Group related operations logically with clear separation between sections
* Use whitespace strategically to improve visual structure

### Maintainability
* Code must be easy to modify and extend without breaking existing functionality
* Functions must have single, well defined responsibilities
* Avoid deep nesting; prefer early returns and guard clauses
* Minimize coupling between components
* Use constants and configuration values instead of magic numbers
* Document assumptions and dependencies clearly

### Debuggability
* Error messages must provide actionable information for debugging
* Use structured logging with appropriate log levels
* Include context in error messages to trace execution paths
* Ensure all error conditions are testable and reproducible
* Provide clear failure modes that aid in root cause analysis
* Use assertion like checks for invariants that should never occur

## Size Constraints

### Function Length
* Maximum function length: 50 lines including comments and whitespace
* If a function exceeds 50 lines, it must be refactored into smaller, focused functions
* Each function should accomplish one clearly defined task
* Complex operations should be decomposed into helper functions with descriptive names

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

## Review Standards
* Assume line by line scrutiny by principal level reviewers
* Favor clarity over cleverness in all implementations
* Document tradeoffs and rationale for non trivial decisions
* Keep code minimal and focused on demonstrating a single IPC mechanism

## Style Guidelines
* Keep examples minimal and focused on one IPC mechanism per file
* Add concise comments only where intent or ordering is non obvious
* Use meaningful names for keys, queue identifiers, and shared objects
* Prefer early returns and shallow nesting to reduce complexity

## Documentation Requirements
* Explain IPC mechanism being used with brief context
* Document signal handling requirements where applicable
* Explain synchronization points and their purpose
* Keep functions focused on single IPC operations

## Integration with Other Rules
This file provides general quality standards. For specific guidance, refer to:
* `resource-management.md` for resource cleanup and validation
* `error-handling.md` for error reporting and validation
* `synchronization.md` for coordination and deadlock prevention
* `memory-safety.md` for buffer operations and shared memory
* `security.md` for permissions and input sanitization
* `signals-processes.md` for process and signal management
* `testing-validation.md` for test coverage requirements
