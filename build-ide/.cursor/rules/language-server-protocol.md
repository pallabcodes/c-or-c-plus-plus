# Language Server Protocol Standards

## Scope
Applies to all Language Server Protocol (LSP) implementation code. Extends repository root rules.

## Language Server Protocol Overview
* Protocol for providing language features via JSON RPC
* Client server architecture
* Language agnostic interface
* Reference: "Language Server Protocol Specification" (Microsoft, 2016)
* Standard for IDE language support

## LSP Implementation

### Protocol Transport
* JSON RPC 2.0 over stdio, pipes, or sockets
* Request response pattern
* Notification pattern for unsolicited messages
* Error handling and recovery

### Server Lifecycle
* Server initialization and startup
* Server shutdown and cleanup
* Dynamic server registration
* Multiple language server support

### Request Handling
* Initialize request for capabilities negotiation
* Document synchronization (didOpen, didChange, didClose)
* Text document requests (completion, hover, definition)
* Workspace requests (symbols, references)

### Capabilities
* Text document capabilities
* Workspace capabilities
* Experimental capabilities
* Dynamic capability registration

## Core LSP Features

### Document Synchronization
* Track open documents
* Incremental text updates
* Full document sync fallback
* Handle large documents efficiently

### Code Completion
* Completion request handling
* Completion item ranking
* Context aware suggestions
* Lazy loading of completion details

### Hover Information
* Hover request processing
* Markdown formatted documentation
* Signature information
* Quick info display

### Go to Definition
* Definition location resolution
* Multiple definition support
* Symbol navigation
* Cross file references

### Find References
* Reference location search
* Include declarations option
* Workspace wide search
* Result ranking

### Filtering/Highlighting
* Document symbol requests
* Document highlighting
* Semantic tokens for semantic highlighting
* Incremental updates

### Code Actions
* Code action requests
* Quick fixes
* Refactorings
* Source actions

## Error Handling

### Diagnostics
* Publish diagnostics notifications
* Parse errors, type errors, warnings
* Diagnostic severity levels
* Diagnostic tags and codes

### Error Recovery
* Graceful degradation on errors
* Retry mechanisms
* Fallback behavior
* User notification

## Implementation Requirements
* Efficient JSON parsing and serialization
* Async request handling
* Timeout management
* Resource cleanup
* Proper error propagation
* Logging for debugging

## Performance Considerations
* Minimize request latency
* Batch requests when possible
* Cache responses appropriately
* Background processing for expensive operations
* Incremental parsing where supported

## Integration Points
* IDE client integration
* Extension system integration
* Editor component integration
* Configuration management

