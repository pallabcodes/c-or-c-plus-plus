# Language Server Protocol Standards

## Overview
The Language Server Protocol (LSP) enables IDE features like code completion, navigation, and diagnostics. This document defines standards for implementing production grade LSP client and server components that match the quality of top tier IDEs like VSCode, IntelliJ IDEA, and others.

## Scope
* Applies to all LSP implementation code including client, server, and protocol handling
* Extends repository root rules defined in the root `.cursor/rules/` files
* Covers all aspects of LSP from basic protocol implementation to advanced features
* Code quality standards align with expectations from top tier IDE companies like Microsoft, JetBrains, and others

## Top Tier IDE LSP Implementations

### Visual Studio Code
* Comprehensive LSP client implementation
* Support for 100+ language servers
* Fast response times (< 100ms)
* Robust error handling
* Extension marketplace integration
* Used by millions of developers

### IntelliJ IDEA
* LSP client for external language servers
* Custom language intelligence for Java/Kotlin
* Hybrid approach combining LSP and custom analysis
* Fast and reliable
* Production tested at scale

### Vim/Neovim
* LSP client via plugins (coc.nvim, nvim-lspconfig)
* Support for many language servers
* Efficient integration with modal editing
* Lightweight implementation

## LSP Architecture

### Client Server Model
* **Client**: IDE frontend requesting language features
* **Server**: Language server providing language intelligence
* **Protocol**: JSON-RPC 2.0 based communication
* **Transport**: stdio, pipes, sockets, HTTP
* **Rationale**: Separates language intelligence from IDE implementation

### Request Response Pattern
* **Requests**: Client requests (completion, definition, etc.)
* **Responses**: Server responses with results
* **Notifications**: One-way messages (didChange, didOpen)
* **Error handling**: Proper error responses
* **Rationale**: Standardized communication pattern

## Core LSP Features

### Text Document Synchronization
* **didOpen**: Document opened notification
* **didChange**: Document changed notification (full or incremental)
* **didClose**: Document closed notification
* **didSave**: Document saved notification
* **Rationale**: Keep server in sync with editor state

### Code Completion
* **Completion request**: Request completions at position
* **Completion items**: List of completion suggestions
* **Ranking**: Rank completions by relevance
* **Context awareness**: Context-aware completions
* **Rationale**: Essential IDE feature

### Go to Definition
* **Definition request**: Request definition location
* **Location response**: File and position of definition
* **Multiple definitions**: Handle multiple definitions
* **Rationale**: Essential navigation feature

### Find References
* **References request**: Find all references to symbol
* **Locations response**: List of reference locations
* **Include declaration**: Option to include declaration
* **Rationale**: Essential navigation feature

### Diagnostics
* **Publish diagnostics**: Server publishes errors/warnings
* **Diagnostic information**: Error message, range, severity
* **Real-time updates**: Update diagnostics on changes
* **Rationale**: Real-time error reporting

### Hover Information
* **Hover request**: Request hover information at position
* **Hover response**: Documentation and type information
* **Markdown support**: Support Markdown formatting
* **Rationale**: Quick information display

### Signature Help
* **Signature help request**: Request signature help
* **Signature information**: Function signatures and parameters
* **Active parameter**: Highlight active parameter
* **Rationale**: Function call assistance

### Code Actions
* **Code action request**: Request code actions
* **Code actions**: List of available actions (fixes, refactorings)
* **Command execution**: Execute code actions
* **Rationale**: Quick fixes and refactorings

### Document Symbols
* **Document symbols request**: Request document symbols
* **Symbol information**: List of symbols in document
* **Hierarchical symbols**: Support hierarchical symbols
* **Rationale**: Outline view and navigation

### Workspace Symbols
* **Workspace symbols request**: Request workspace symbols
* **Symbol search**: Search symbols across workspace
* **Fuzzy matching**: Support fuzzy matching
* **Rationale**: Global symbol search

## Implementation Standards

### Protocol Compliance
* **JSON-RPC 2.0**: Implement JSON-RPC 2.0 correctly
* **LSP specification**: Follow LSP specification strictly
* **Error handling**: Handle protocol errors correctly
* **Version compatibility**: Handle protocol version differences
* **Rationale**: Compliance ensures interoperability

### Performance
* **Async operations**: Use async operations for non-blocking
* **Caching**: Cache results when appropriate
* **Incremental updates**: Use incremental document updates
* **Request batching**: Batch requests when possible
* **Rationale**: Performance is critical for responsiveness

### Error Handling
* **Protocol errors**: Handle protocol errors gracefully
* **Server errors**: Handle server errors gracefully
* **Timeout handling**: Handle timeouts appropriately
* **Retry logic**: Implement retry logic for transient errors
* **Rationale**: Robust error handling improves reliability

### Connection Management
* **Connection lifecycle**: Manage connection lifecycle
* **Reconnection**: Handle reconnection on failure
* **Health checks**: Monitor server health
* **Graceful shutdown**: Shutdown gracefully
* **Rationale**: Reliable connection management

## JSON-RPC 2.0 Implementation

### Request Format
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "textDocument/completion",
  "params": {
    "textDocument": { "uri": "file:///path/to/file.cpp" },
    "position": { "line": 10, "character": 5 }
  }
}
```

### Response Format
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": [
    {
      "label": "function_name",
      "kind": 3,
      "detail": "int function_name(int x)",
      "documentation": "Function description"
    }
  ]
}
```

### Error Format
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "error": {
    "code": -32600,
    "message": "Invalid Request"
  }
}
```

## LSP Client Implementation

### Client Responsibilities
* **Request management**: Manage request IDs and responses
* **Document synchronization**: Keep documents synchronized
* **Error handling**: Handle errors gracefully
* **UI integration**: Integrate with IDE UI
* **Rationale**: Client manages communication with server

### Document Management
* **Document cache**: Cache document contents
* **Change tracking**: Track document changes
* **Incremental updates**: Send incremental updates
* **Version management**: Manage document versions
* **Rationale**: Efficient document management

### Request Handling
* **Request queuing**: Queue requests appropriately
* **Response handling**: Handle responses correctly
* **Timeout management**: Handle timeouts
* **Error recovery**: Recover from errors
* **Rationale**: Reliable request handling

## LSP Server Implementation

### Server Responsibilities
* **Language analysis**: Analyze code for language features
* **Request processing**: Process client requests
* **Diagnostic publishing**: Publish diagnostics
* **State management**: Manage server state
* **Rationale**: Server provides language intelligence

### Language Analysis
* **Parsing**: Parse source code
* **AST construction**: Build abstract syntax tree
* **Symbol resolution**: Resolve symbols
* **Type checking**: Perform type checking
* **Rationale**: Language analysis enables features

### State Management
* **Document state**: Track document state
* **Workspace state**: Track workspace state
* **Index management**: Manage symbol index
* **Cache management**: Manage caches
* **Rationale**: Efficient state management

## Testing Requirements

### Unit Tests
* **Protocol**: Test protocol implementation
* **Requests**: Test request handling
* **Responses**: Test response handling
* **Edge cases**: Test error cases
* **Rationale**: Comprehensive testing ensures correctness

### Integration Tests
* **End to end**: Test complete LSP workflows
* **Language servers**: Test with real language servers
* **Multiple languages**: Test with multiple languages
* **Rationale**: Integration tests verify system behavior

### Performance Tests
* **Response time**: Test response times
* **Throughput**: Test request throughput
* **Memory usage**: Test memory usage
* **Scalability**: Test with large codebases
* **Rationale**: Performance tests ensure performance goals

## Research Papers and References

### Language Server Protocol
* "Language Server Protocol Specification" (Microsoft, 2016)
* LSP specification documentation
* JSON-RPC 2.0 specification
* Protocol extension mechanisms

### Open Source References
* VSCode LSP implementation
* clangd language server (C++)
* rust-analyzer language server (Rust)
* jdtls language server (Java)
* pylsp language server (Python)

## Implementation Checklist

- [ ] Implement JSON-RPC 2.0 protocol
- [ ] Implement text document synchronization
- [ ] Implement code completion
- [ ] Implement go to definition
- [ ] Implement find references
- [ ] Implement diagnostics
- [ ] Implement hover information
- [ ] Implement signature help
- [ ] Implement code actions
- [ ] Implement document symbols
- [ ] Implement workspace symbols
- [ ] Add comprehensive error handling
- [ ] Add connection management
- [ ] Write comprehensive unit tests
- [ ] Write integration tests
- [ ] Test with real language servers
- [ ] Document protocol compliance
- [ ] Optimize for performance
