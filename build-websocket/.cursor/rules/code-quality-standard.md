# Code Quality Standards for WebSocket Server

## Overview
This document defines production grade code quality standards for WebSocket server implementations. These standards ensure code is suitable for principal level review and production deployment in high performance systems handling millions of concurrent connections.

## Function Size Limits
* **Maximum function length**: 50 lines (excluding comments and blank lines)
* **Rationale**: Functions exceeding 50 lines become difficult to understand, test, and maintain
* **Enforcement**: All functions must be reviewed for size compliance
* **Exception**: Complex protocol handlers may extend to 60 lines with justification

## File Size Limits
* **Maximum file length**: 200 lines (excluding comments and blank lines)
* **Rationale**: Files exceeding 200 lines become difficult to navigate and understand
* **Enforcement**: Split large files into logical modules
* **Exception**: Header files with extensive protocol definitions may extend to 250 lines

## Cyclomatic Complexity
* **Maximum complexity**: 10 per function
* **Rationale**: High complexity indicates functions doing too much
* **Measurement**: Count decision points (if, while, for, switch, &&, ||, ?:)
* **Enforcement**: Refactor complex functions into smaller, focused functions
* **Exception**: Complex protocol handlers may have complexity up to 15 with justification

## Code Style

### Naming Conventions
* **Classes**: `PascalCase` (e.g., `WebSocketServer`, `ConnectionManager`, `FrameParser`)
* **Functions**: `snake_case` (e.g., `handle_handshake`, `parse_frame`, `send_message`)
* **Constants**: `UPPER_SNAKE_CASE` (e.g., `MAX_FRAME_SIZE`, `DEFAULT_TIMEOUT`)
* **Rationale**: Consistent naming improves readability and maintainability

### Indentation
* **Indentation**: 4 spaces (no tabs)
* **Continuation**: Align continuation lines with opening delimiter
* **Rationale**: Consistent indentation improves readability

### Comments
* **Protocol comments**: Required for all protocol related functions
* **Frame handling**: Document frame parsing and generation
* **Error handling**: Document error handling behavior
* **Rationale**: Comments clarify protocol implementation and usage

## Error Handling

### Input Validation
* **All public APIs**: Must validate inputs
* **Null pointers**: Check and return error for NULL/nullptr inputs
* **Frame validation**: Validate all WebSocket frames
* **Preconditions**: Document and validate preconditions
* **Rationale**: Prevents crashes and undefined behavior

### Error Reporting
* **Return codes**: Use consistent return code conventions
* **Exceptions**: Use exceptions for C++ (prefer exceptions over return codes)
* **Error codes**: Define error codes in header files
* **std::expected**: Use std::expected for error handling (C++23)
* **Rationale**: Clear error reporting aids debugging

### Protocol Errors
* **Handshake errors**: Handle handshake failures properly
* **Frame errors**: Handle malformed frames
* **Connection errors**: Handle connection failures
* **Timeout errors**: Handle timeout conditions
* **Rationale**: Protocol errors must be handled gracefully

## Network Safety

### Buffer Bounds
* **Bounds checking**: Always check buffer bounds
* **Frame size limits**: Enforce maximum frame size
* **Message size limits**: Enforce maximum message size
* **Rationale**: Buffer safety prevents security vulnerabilities

### Connection State
* **State management**: Proper connection state management
* **State validation**: Validate state before operations
* **Rationale**: State management prevents errors

### Resource Management
* **Connection cleanup**: Proper connection cleanup
* **Memory cleanup**: Proper memory cleanup
* **File descriptor leaks**: Prevent file descriptor leaks
* **Rationale**: Resource management prevents leaks

## Performance

### I/O Operations
* **Non blocking**: Use non blocking I/O
* **Batch operations**: Batch I/O operations when possible
* **Zero copy**: Use zero copy techniques when applicable
* **Rationale**: I/O optimization improves performance

### Memory Management
* **Buffer pooling**: Use buffer pools for frequent allocations
* **Memory reuse**: Reuse memory when possible
* **Slab allocators**: Use slab allocators for hot paths
* **Rationale**: Memory management improves performance

## Testing

### Unit Tests
* **Coverage**: Aim for 90%+ code coverage
* **Protocol tests**: Test protocol implementation
* **Frame tests**: Test frame parsing and generation
* **Error tests**: Test error handling
* **Rationale**: Comprehensive testing ensures correctness

## Documentation

### API Documentation
* **Public functions**: Document all public functions
* **Protocol parameters**: Document protocol parameters
* **Error conditions**: Document error conditions
* **Examples**: Provide usage examples
* **Rationale**: Clear documentation enables correct usage

## Examples

### Good WebSocket Function (Within Limits)
```cpp
// Thread safety: Thread safe (mutex protected)
// Ownership: Borrows connection (does not own)
// Invariants: connection must be valid, frame must be valid
// Failure modes: Returns error on protocol violation or invalid input
Result<void> send_text_frame(WebSocketConnection& connection, 
                             const std::string& message) {
    if (message.size() > MAX_MESSAGE_SIZE) {
        return std::unexpected("Message too large");
    }
    
    WebSocketFrame frame;
    frame.fin = true;
    frame.opcode = Opcode::TEXT;
    frame.payload = message;
    
    return connection.send_frame(frame);
}
```

### Bad WebSocket Function (Exceeds Limits)
```cpp
// BAD: No validation, exceeds 50 lines, high complexity
void send_frame(Connection& conn, Frame& frame) {
    // 60+ lines of complex logic
    // No validation
    // High cyclomatic complexity (> 10)
    // Difficult to test and maintain
}
```

## Enforcement

### Code Review
* **Mandatory**: All code must be reviewed for compliance
* **Checklist**: Use checklist to verify standards
* **Protocol review**: Special attention to RFC 6455 compliance
* **Automation**: Use tools to check function/file sizes and complexity

### CI/CD
* **Static analysis**: Run static analysis tools in CI
* **Linting**: Run linters to check style compliance
* **Testing**: Run tests to verify correctness
* **Protocol tests**: Run Autobahn TestSuite compliance tests
* **Metrics**: Track code quality metrics over time

