# API Design Standards

## Overview
Well designed APIs are essential for library usability and maintainability. This document defines standards for designing production grade APIs for custom library functions with clarity, consistency, and correctness as primary concerns.

## API Design Principles

### Consistency
* **Naming**: Use consistent naming conventions
* **Parameter order**: Use consistent parameter ordering
* **Return values**: Use consistent return value conventions
* **Error handling**: Use consistent error handling patterns
* **Rationale**: Consistency improves usability and reduces errors

### Clarity
* **Function names**: Use clear, descriptive function names
* **Parameter names**: Use clear parameter names
* **Documentation**: Provide clear documentation
* **Examples**: Provide usage examples
* **Rationale**: Clarity reduces misunderstanding and errors

### Simplicity
* **Minimal parameters**: Minimize number of parameters
* **Simple interfaces**: Prefer simple interfaces over complex ones
* **Composability**: Design for composition
* **Rationale**: Simplicity improves usability and maintainability

### Correctness
* **Input validation**: Validate all inputs
* **Error handling**: Handle all error cases
* **Invariants**: Document and maintain invariants
* **Rationale**: Correctness prevents bugs and security issues

## Naming Conventions

### Function Names
* **Format**: `module_action` (e.g., `buffer_init`, `buffer_write`, `buffer_flush`)
* **Verbs**: Use verbs for actions (init, write, flush, parse)
* **Nouns**: Use nouns for getters (get_buffer_size)
* **Consistency**: Use consistent naming patterns
* **Rationale**: Clear naming improves readability

### Type Names
* **Format**: `type_name_t` (e.g., `buffer_t`, `format_spec_t`)
* **Suffix**: Use `_t` suffix for types
* **Consistency**: Use consistent naming patterns
* **Rationale**: Type suffix clarifies type vs variable

### Constant Names
* **Format**: `MODULE_CONSTANT` (e.g., `BUFFER_SIZE`, `MAX_FORMAT_SPECS`)
* **Uppercase**: Use uppercase for constants
* **Underscores**: Use underscores to separate words
* **Rationale**: Constant naming distinguishes constants from variables

## Parameter Design

### Parameter Ordering
* **Inputs first**: Put input parameters first
* **Outputs last**: Put output parameters last
* **Size parameters**: Put size parameters with corresponding buffers
* **Consistency**: Use consistent ordering
* **Rationale**: Consistent ordering improves usability

### Parameter Types
* **Inputs**: Use const pointers for input strings/buffers
* **Outputs**: Use non const pointers for outputs
* **Sizes**: Use size_t for sizes
* **File descriptors**: Use int for file descriptors
* **Rationale**: Type choices clarify intent

### Example API
```c
// Good API design
// Thread safety: Thread safe (uses thread local storage)
// Ownership: Caller owns fmt, caller owns output buffer
// Invariants: fmt must not be NULL, output must have sufficient capacity
// Failure modes: Returns -1 on error, returns written length on success
int my_printf(const char *fmt, ...);

// Good API design with explicit buffer
// Thread safety: Not thread safe (caller must synchronize)
// Ownership: Caller owns buf, caller owns data
// Invariants: buf must not be NULL, data must not be NULL, len > 0
// Failure modes: Returns -1 on error, 0 on success
int buffer_write(buffer_t *buf, const char *data, size_t len, int fd);
```

## Return Value Conventions

### Success/Failure
* **Success**: Return 0 or positive value
* **Failure**: Return -1 or negative value
* **Consistency**: Use consistent return value conventions
* **Rationale**: Consistent conventions improve usability

### Length Returns
* **Format**: Return length on success, -1 on error
* **Examples**: `write`, `read`, `snprintf`
* **Consistency**: Match standard library conventions
* **Rationale**: Familiar conventions improve usability

### Status Codes
* **Enum**: Use enum for status codes
* **Names**: Use descriptive names (SUCCESS, ERROR_INVALID_FORMAT)
* **Documentation**: Document all status codes
* **Rationale**: Status codes clarify error types

## Error Handling

### Error Codes
* **Consistent**: Use consistent error code conventions
* **Descriptive**: Use descriptive error code names
* **Documentation**: Document all error codes
* **Rationale**: Clear error codes aid debugging

### Error Reporting
* **Return codes**: Return error codes
* **Error messages**: Provide error messages where appropriate
* **Context**: Provide error context
* **Rationale**: Clear error reporting aids debugging

### Input Validation
* **Null checks**: Check for NULL pointers
* **Bounds checks**: Check bounds on numeric inputs
* **Format validation**: Validate format strings
* **Rationale**: Input validation prevents bugs and security issues

## Documentation Requirements

### Function Documentation
* **Purpose**: Describe function purpose
* **Parameters**: Document all parameters
* **Return values**: Document return values
* **Thread safety**: Document thread safety guarantees
* **Ownership**: Document memory ownership semantics
* **Invariants**: Document invariants
* **Failure modes**: Document failure modes
* **Examples**: Provide usage examples

### Example Documentation
```c
/**
 * Write data to buffer, flushing if necessary.
 *
 * @param buf Buffer to write to (must not be NULL)
 * @param data Data to write (must not be NULL)
 * @param len Length of data to write (must be > 0)
 * @param fd File descriptor for flushing (must be valid)
 *
 * @return 0 on success, -1 on error
 *
 * Thread safety: Not thread safe (caller must synchronize)
 * Ownership: Caller owns buf and data
 * Invariants: buf must be initialized, data must be valid
 * Failure modes: Returns -1 on write failure or invalid input
 *
 * Example:
 *   buffer_t buf;
 *   buffer_init(&buf);
 *   buffer_write(&buf, "Hello", 5, STDOUT_FILENO);
 */
int buffer_write(buffer_t *buf, const char *data, size_t len, int fd);
```

## Versioning and Compatibility

### API Stability
* **Stable APIs**: Maintain backward compatibility for stable APIs
* **Versioning**: Use versioning for API changes
* **Deprecation**: Deprecate APIs before removal
* **Rationale**: API stability enables adoption

### Extension Points
* **Extensible**: Design for extension
* **Reserved parameters**: Reserve parameters for future use
* **Flags**: Use flags for optional features
* **Rationale**: Extensibility enables evolution

## Testing Requirements

### API Tests
* **Valid inputs**: Test with valid inputs
* **Invalid inputs**: Test with invalid inputs
* **Edge cases**: Test edge cases
* **Error cases**: Test error handling
* **Rationale**: Comprehensive testing ensures correctness

### Usage Examples
* **Examples**: Provide usage examples in tests
* **Documentation**: Include examples in documentation
* **Rationale**: Examples clarify usage

## Research Papers and References

### API Design
* "API Design Guidelines" - Best practices
* "The Art of Unix Programming" - Unix API design principles
* "Designing Web APIs" - API design patterns

### Open Source References
* glibc API design
* musl libc API design
* Google Abseil API design

## Implementation Checklist

- [ ] Design API with consistent naming
- [ ] Design parameters with consistent ordering
- [ ] Define return value conventions
- [ ] Implement input validation
- [ ] Implement error handling
- [ ] Write comprehensive documentation
- [ ] Provide usage examples
- [ ] Write API tests
- [ ] Review API for usability
- [ ] Document versioning and compatibility

