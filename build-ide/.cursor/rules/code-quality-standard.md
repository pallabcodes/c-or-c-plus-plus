# Code Quality Standards for IDE Systems

## Scope
Applies to all IDE code in this directory. Extends repository root rules.

## Code Review Context
This code is expected to undergo line by line scrutiny by principal level reviewers from top tier IDE companies like Microsoft (VSCode), JetBrains (IntelliJ), and open source leaders. Every line must demonstrate production grade quality, clarity, and maintainability comparable to internal implementations at these companies.

## Code Quality Dimensions

### Readability
* Code must be immediately understandable without extensive documentation
* Variable and function names must be self documenting and convey intent clearly
* Avoid abbreviations unless they are industry standard or clearly documented (e.g., LSP, AST, DAP)
* Use consistent naming conventions throughout all files
* Group related operations logically with clear separation between sections
* Use whitespace strategically to improve visual structure
* Follow IDE specific naming conventions for components (e.g., buffer, viewport, symbol)

### Maintainability
* Code must be easy to modify and extend without breaking existing functionality
* Functions must have single, well defined responsibilities
* Avoid deep nesting; prefer early returns and guard clauses
* Minimize coupling between IDE components
* Use constants and configuration values instead of magic numbers
* Document assumptions and dependencies clearly
* Design for extensibility with plugin architectures where appropriate

### Debuggability
* Error messages must provide actionable information for debugging
* Use structured logging with appropriate log levels
* Include context in error messages to trace execution paths
* Ensure all error conditions are testable [[] reproducible
* Provide clear failure modes that aid in root cause analysis
* Use assertion like checks for invariants that should never occur
* Include UI context in error messages for user facing features

## Size Constraints

### Function Length
* Maximum function length: 50 lines including comments and whitespace
* If a function exceeds 50 lines, it must be refactored into smaller, focused functions
* Each function should accomplish one clearly defined task
* Complex operations like code analysis should be decomposed into helper functions with descriptive names

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
* Parsing and code analysis logic should be modularized

## Review Standards
* Assume line by line scrutiny by principal level reviewers
* Favor clarity over cleverness in all implementations
* Document tradeoffs and rationale for non trivial decisions
* Keep code minimal and focused on demonstrating single concepts
* Reference research papers when implementing algorithms from academic literature
* Reference open source implementations when appropriate

## Style Guidelines
* Keep examples minimal and focused on one IDE concept per file
* Add concise comments only where intent or ordering is non obvious
* Use meaningful names for IDE objects, buffers, and symbols
* Prefer early returns and shallow nesting to reduce complexity
* Follow IDE terminology consistently (e.g., buffer, viewport, caret, selection)

## Documentation Requirements
* Explain IDE algorithms being used with brief context
* Document performance characteristics and complexity analysis
* Explain UI update mechanisms and their purpose
* Keep functions focused on single IDE operations
* Reference research papers when implementing academic algorithms
* Reference open source projects when borrowing implementation patterns
* Document invariants and constraints for data structures

## Performance Requirements
* Profile critical paths and record findings
* Optimize for responsive UI (target < 16ms per frame for 60 FPS)
* Use appropriate data structures for access patterns
* Minimize allocations in hot paths
* Consider incremental algorithms for real time operations
* Virtual scrolling for large files and views

## UI Responsiveness
* Keep UI thread operations fast
* Defer heavy computations to background threads
* Use incremental updates for UI rendering
* Minimize blocking operations
* Progressive rendering for large documents

## Integration with Other Rules
This file provides general quality standards. For specific guidance, refer to:
* `text-editor-core.md` for text buffer and editing implementation
* `language-server-protocol.md` for LSP implementation
* `syntax-highlighting.md` for highlighting and tokenization
* `code-completion.md` for IntelliSense and autocomplete
* `code-navigation.md` for symbol resolution and navigation
* `refactoring.md` for code transformation operations
* `debugging-support.md` for debugger integration
* `build-system-integration.md` for compiler and build tool integration
* `source-control.md` for version control integration
* `extensibility.md` for plugin architecture
* `performance-optimization.md` for performance tuning
* `ui-rendering.md` for text and graphics rendering
* `testing-validation.md` for test coverage requirements

