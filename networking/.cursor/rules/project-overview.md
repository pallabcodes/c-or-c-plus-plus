# Networking Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. This networking implementation must meet enterprise production standards suitable for principal level engineering review and must be comparable to top tier implementations used in production systems at these companies.

## Purpose
This module covers the design and implementation of production grade network programming in C and C++. All code must follow production grade standards suitable for principal level code review and must demonstrate correct, efficient, and maintainable network implementations including socket programming, HTTP/1.1, WebSocket, event driven I/O, and security.

## Scope
* Applies to all C and C++ code in networking directory
* Extends repository root rules defined in the root `.cursor/rules/` files
* Covers all aspects of network programming from socket basics to advanced protocols
* Code quality standards align with expectations from top tier companies like Google, Bloomberg, Uber, and Amazon

## Top Tier Product Comparisons

### Google Production Systems
* High performance HTTP/2 stack
* Event driven architecture
* Zero copy optimizations
* Production tested at massive scale
* Efficient connection management

### nginx Architecture
* Event driven I/O model
* High concurrency handling
* Memory efficient buffers
* Production tested at scale
* Efficient request processing

### Cloudflare Edge Systems
* Edge server architecture
* High throughput networking
* DDoS protection patterns
* Production tested at massive scale
* Efficient protocol handling

### Meta Network Infrastructure
* High performance networking
* Event driven patterns
* Connection pooling
* Production tested at scale
* Efficient resource management

### Standard Libraries
* C Standard Library socket patterns
* C++ Standard Library network patterns
* Standard protocol implementations
* Production grade networking practices

## Network Programming Fundamentals

### Socket Programming
* **TCP sockets**: Stream oriented reliable communication
* **UDP sockets**: Datagram oriented communication
* **Address handling**: IP address and port management
* **Rationale**: Sockets enable network communication

### HTTP Implementation
* **HTTP/1.1**: Request/response protocol
* **Parsing**: Header and body parsing
* **Keep alive**: Connection reuse
* **Rationale**: HTTP enables web communication

### WebSocket Implementation
* **RFC 6455**: WebSocket protocol standard
* **Frame parsing**: Binary frame handling
* **Handshake**: HTTP upgrade mechanism
* **Rationale**: WebSocket enables real time communication

## I/O Models

### Event Driven I/O
* **epoll**: Linux event notification
* **kqueue**: BSD/macOS event notification
* **IOCP**: Windows I/O completion ports
* **Rationale**: Event driven I/O enables scalability

### Reactor Pattern
* **Definition**: Event loop with event handlers
* **Use cases**: High concurrency servers
* **Benefits**: Efficient resource usage
* **Rationale**: Reactor pattern enables scalability

### Proactor Pattern
* **Definition**: Asynchronous I/O with completion handlers
* **Use cases**: High performance servers
* **Benefits**: Overlapped I/O operations
* **Rationale**: Proactor pattern enables performance

## Connection Management

### Connection Pooling
* **Definition**: Reuse connections for multiple requests
* **Benefits**: Reduces connection overhead
* **Use cases**: HTTP clients, database connections
* **Rationale**: Connection pooling improves performance

### Keep Alive
* **Definition**: Keep connections open for reuse
* **Benefits**: Reduces connection establishment overhead
* **Use cases**: HTTP/1.1, WebSocket
* **Rationale**: Keep alive improves performance

### Timeout Management
* **Definition**: Timeout handling for connections
* **Benefits**: Prevents resource leaks
* **Use cases**: All network connections
* **Rationale**: Timeout management ensures reliability

## Security

### TLS/SSL Integration
* **Definition**: Secure transport layer
* **Benefits**: Encrypted communication
* **Use cases**: HTTPS, WSS
* **Rationale**: TLS enables secure communication

### Authentication
* **Definition**: Client authentication mechanisms
* **Benefits**: Secure access control
* **Use cases**: API authentication, user authentication
* **Rationale**: Authentication ensures security

### Rate Limiting
* **Definition**: Limit request rate per client
* **Benefits**: Prevents abuse
* **Use cases**: API servers, web servers
* **Rationale**: Rate limiting prevents abuse

## Performance Optimization

### Zero Copy
* **Definition**: Avoid copying data between buffers
* **Benefits**: Reduces CPU usage
* **Use cases**: High throughput servers
* **Rationale**: Zero copy improves performance

### Memory Management
* **Definition**: Efficient buffer management
* **Benefits**: Reduces memory allocations
* **Use cases**: All network operations
* **Rationale**: Memory management improves performance

### CPU Optimization
* **Definition**: Branch prediction, cache locality
* **Benefits**: Improves CPU efficiency
* **Use cases**: Hot paths in network code
* **Rationale**: CPU optimization improves performance

## Production Standards

### Code Quality
* Functions limited to 50 lines
* Files limited to 200 lines
* Cyclomatic complexity ≤ 10
* Comprehensive error handling
* Input validation on all public APIs
* Memory safety and leak prevention

### Performance
* Efficient I/O operations
* Minimize memory allocations
* Optimize hot paths
* Benchmark critical paths
* Profile network operations

### Correctness
* Proper error handling
* Correct protocol implementation
* Proper connection management
* Comprehensive test coverage
* Security best practices

### Documentation
* API documentation for all public functions
* Protocol implementation details
* Error handling guarantees
* Performance characteristics
* Thread safety guarantees

## Research Papers and References

### Network Programming
* "Unix Network Programming" (Stevens) - Socket programming
* "HTTP: The Definitive Guide" - HTTP protocol
* "WebSocket Protocol" (RFC 6455) - WebSocket standard

### Performance
* "High Performance Browser Networking" - Network optimization
* "The C10K Problem" - Concurrency patterns
* "Event Driven Architecture" research papers

### Open Source References
* nginx source code
* libcurl implementation
* Boost.Asio library
* Standard library network patterns

## Implementation Goals

### Correctness
* Correct protocol implementation
* Proper error handling
* Secure communication
* Proper resource management
* Comprehensive testing

### Performance
* Efficient I/O operations
* Minimize allocations
* Optimize hot paths
* Benchmark and optimize
* Profile network operations

### Maintainability
* Clean, readable code
* Comprehensive documentation
* Extensive test coverage
* Clear protocol implementation
* Well documented trade offs

