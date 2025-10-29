# Code Quality Standards for Database Systems

## Scope
Applies to all database code in this directory. Extends repository root rules.

## Code Review Context
This code is expected to undergo line by line scrutiny by principal level reviewers from top tier database companies like Turso, PlanetScale, PingCAP, and ClickHouse. Every line must demonstrate production grade quality, clarity, and maintainability comparable to internal implementations at these companies.

## Code Quality Dimensions

### Readability
* Code must be immediately understandable without extensive documentation
* Variable and function names must be self documenting and convey intent clearly
* Avoid abbreviations unless they are industry standard or clearly documented (e.g., MVCC, WAL, LSM)
* Use consistent naming conventions throughout all files
* Group related operations logically with clear separation between sections
* Use whitespace strategically to improve visual structure
* Follow database specific naming conventions for components (e.g., buffer pool, page manager, query executor)

### Maintainability
* Code must be easy to modify and extend without breaking existing functionality
* Functions must have single, well defined responsibilities
* Avoid deep nesting; prefer early returns and guard clauses
* Minimize coupling between database components
* Use constants and configuration values instead of magic numbers
* Document assumptions and dependencies clearly
* Design for extensibility with plugin architectures where appropriate

### Debuggability
* Error messages must provide actionable information for debugging
* Use structured logging with appropriate log levels
* Include context in error messages to trace execution paths
* Ensure all error conditions are testable and reproducible
* Provide clear failure modes that aid in root cause analysis
* Use assertion like checks for invariants that should never occur
* Include query context in error messages for query processing components

## Size Constraints

### Function Length
* Maximum function length: 50 lines including comments and whitespace
* If a function exceeds 50 lines, it must be refactored into smaller, focused functions
* Each function should accomplish one clearly defined task
* Complex operations like query optimization should be decomposed into helper functions with descriptive names

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
* Query planning and optimization code should be modularized

## Review Standards
* Assume line by line scrutiny by principal level reviewers
* Favor clarity over cleverness in all implementations
* Document tradeoffs and rationale for non trivial decisions
* Keep code minimal and focused on demonstrating single concepts
* Reference research papers when implementing algorithms from academic literature

## Style Guidelines
* Keep examples minimal and focused on one database concept per file
* Add concise comments only where intent or ordering is non obvious
* Use meaningful names for database objects, indexes, and structures
* Prefer early returns and shallow nesting to reduce complexity
* Follow database terminology consistently (e.g., tuple, relation, page, buffer)

## Documentation Requirements
* Explain database algorithms being used with brief context
* Document performance characteristics and complexity analysis
* Explain synchronization points and their purpose
* Keep functions focused on single database operations
* Reference research papers when implementing academic algorithms
* Document invariants and constraints for data structures

## Performance Requirements
* Profile critical paths and record findings
* Optimize for modern hardware (SIMD, cache awareness, NUMA)
* Use appropriate data structures for access patterns
* Minimize allocations in hot paths
* Consider vectorization opportunities for query processing

## Integration with Other Rules
This file provides general quality standards. For specific guidance, refer to:
* `storage-engine.md` for storage layer implementation
* `query-processing.md` for SQL processing and optimization
* `transaction-management.md` for transaction and concurrency control
* `recovery-durability.md` for logging and recovery
* `indexing-structures.md` for index implementation
* `concurrency-control.md` for locking and synchronization
* `distributed-systems.md` for distributed database features
* `vector-search.md` for vector and embedding operations
* `performance-optimization.md` for performance tuning
* `memory-management.md` for buffer pool and memory handling
* `networking-protocols.md` for network layer
* `security-compliance.md` for security and compliance
* `testing-validation.md` for test coverage requirements

