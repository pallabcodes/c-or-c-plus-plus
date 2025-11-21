# Code Quality Standards for Compilers

## Overview
This document defines production grade code quality standards for compiler implementations. These standards ensure code is suitable for principal level review and production deployment in high performance compiler systems.

## Function Size Limits
* **Maximum function length**: 50 lines (excluding comments and blank lines)
* **Rationale**: Functions exceeding 50 lines become difficult to understand, test, and maintain
* **Enforcement**: All functions must be reviewed for size compliance
* **Exception**: Complex compiler passes may extend to 60 lines with justification

## File Size Limits
* **Maximum file length**: 200 lines (excluding comments and blank lines)
* **Rationale**: Files exceeding 200 lines become difficult to navigate and understand
* **Enforcement**: Split large files into logical modules
* **Exception**: Header files with extensive compiler data structures may extend to 250 lines

## Cyclomatic Complexity
* **Maximum complexity**: 10 per function
* **Rationale**: High complexity indicates functions doing too much
* **Measurement**: Count decision points (if, while, for, switch, &&, ||, ?:)
* **Enforcement**: Refactor complex functions into smaller, focused functions
* **Exception**: Complex compiler algorithms may have complexity up to 15 with justification

## Code Style

### Naming Conventions
* **Classes**: `PascalCase` (e.g., `Lexer`, `Parser`, `CodeGenerator`)
* **Functions**: `snake_case` (e.g., `tokenize`, `parse_expression`, `generate_code`)
* **Constants**: `UPPER_SNAKE_CASE` (e.g., `MAX_TOKEN_SIZE`, `DEFAULT_OPTIMIZATION_LEVEL`)
* **Rationale**: Consistent naming improves readability and maintainability

### Indentation
* **Indentation**: 4 spaces (no tabs)
* **Continuation**: Align continuation lines with opening delimiter
* **Rationale**: Consistent indentation improves readability

### Comments
* **Compiler comments**: Required for all public compiler functions
* **Algorithm comments**: Document complex algorithms
* **Research citations**: Cite research papers for algorithms
* **Rationale**: Comments clarify compiler implementation and research basis

## Error Handling

### Input Validation
* **All public APIs**: Must validate inputs
* **Source code**: Validate source code input
* **Invalid syntax**: Handle syntax errors gracefully
* **Preconditions**: Document and validate preconditions
* **Rationale**: Prevents crashes and undefined behavior

### Error Reporting
* **Diagnostics**: Use structured diagnostics
* **Error messages**: Clear, actionable error messages
* **Source locations**: Include source locations in errors
* **Suggestions**: Provide fix suggestions when possible
* **Rationale**: Clear error reporting aids debugging

### Compiler Errors
* **Syntax errors**: Handle syntax errors with recovery
* **Semantic errors**: Handle semantic errors clearly
* **Type errors**: Provide clear type error messages
* **Rationale**: Compiler errors must be helpful

## Compiler Safety

### Memory Safety
* **Buffer bounds**: Always check buffer bounds
* **AST safety**: Validate AST structure
* **Symbol table safety**: Validate symbol table operations
* **Rationale**: Memory safety prevents crashes

### Correctness
* **Language semantics**: Implement correct language semantics
* **Optimization correctness**: Ensure optimizations preserve semantics
* **Code generation correctness**: Generate correct code
* **Rationale**: Correctness is critical for compilers

## Performance

### Compile Time
* **Efficient algorithms**: Use efficient algorithms
* **Incremental compilation**: Support incremental compilation
* **Parallel compilation**: Support parallel compilation
* **Rationale**: Compile time affects developer productivity

### Memory Usage
* **Efficient data structures**: Use efficient data structures
* **Memory pools**: Use memory pools for frequent allocations
* **Rationale**: Memory usage affects scalability

## Testing

### Unit Tests
* **Coverage**: Aim for 90%+ code coverage
* **Lexer tests**: Test lexical analysis
* **Parser tests**: Test parsing
* **Optimization tests**: Test optimizations
* **Code generation tests**: Test code generation
* **Rationale**: Comprehensive testing ensures correctness

## Documentation

### API Documentation
* **Public functions**: Document all public functions
* **Compiler phases**: Document compiler phases
* **Algorithms**: Document algorithms with citations
* **Examples**: Provide usage examples
* **Rationale**: Clear documentation enables correct usage

## Examples

### Good Compiler Function (Within Limits)
```cpp
// Thread safety: Not thread safe (mutable state)
// Ownership: Borrows source (does not own)
// Invariants: source must not be null
// Failure modes: Returns error token on invalid input
// Research: Based on "Compilers: Principles, Techniques, and Tools" (Aho et al.)
Token lexer_next_token(Lexer* lexer, const char* source) {
    if (!lexer || !source) {
        return create_error_token("Invalid input");
    }
    
    skip_whitespace(lexer, source);
    
    if (is_at_end(lexer, source)) {
        return create_eof_token();
    }
    
    char c = source[lexer->current];
    if (is_digit(c)) {
        return lex_number(lexer, source);
    }
    if (is_alpha(c)) {
        return lex_identifier(lexer, source);
    }
    
    return lex_single_char(lexer, source, c);
}
```

### Bad Compiler Function (Exceeds Limits)
```cpp
// BAD: No error handling, exceeds 50 lines, high complexity
Token lexer_next_token(Lexer* lexer, const char* source) {
    // 60+ lines of complex logic
    // No error handling
    // High cyclomatic complexity (> 10)
    // Difficult to test and maintain
}
```

## Enforcement

### Code Review
* **Mandatory**: All code must be reviewed for compliance
* **Checklist**: Use checklist to verify standards
* **Compiler review**: Special attention to correctness and performance
* **Automation**: Use tools to check function/file sizes and complexity

### CI/CD
* **Static analysis**: Run static analysis tools in CI
* **Linting**: Run linters to check style compliance
* **Testing**: Run tests to verify correctness
* **Compiler tests**: Run compiler test suites
* **Metrics**: Track code quality metrics over time
