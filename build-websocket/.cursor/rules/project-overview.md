# WebSocket Server Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. This WebSocket server implementation must meet enterprise production standards suitable for principal level engineering review and must be comparable to top tier implementations used in production systems at these companies.

## Purpose
This module covers the design and implementation of a production grade WebSocket server in C and C++. All code must follow production grade standards suitable for principal level code review and must be production ready for deployment in high performance systems requiring millions of concurrent connections, low latency, safety, and observability.

## Scope
* Applies to all C and C++ code in build-websocket directory
* Extends repository root rules defined in the root `.cursor/rules/` files
* Covers all aspects of WebSocket server implementation from protocol basics to scale out distribution
* Code quality standards align with expectations from top tier companies like Google, Bloomberg, Uber, and Amazon

## Top Tier Product Comparisons

### uWebSockets
* High performance C++ WebSocket library
* Millions of concurrent connections
* Low latency, efficient memory usage
* Production tested at massive scale
* Efficient event driven architecture

### Boost.Beast
* C++ networking library
* RFC 6455 compliant WebSocket implementation
* Asio based architecture
* Production tested at scale
* Standard library integration

### websocketpp
* Header only C++ WebSocket library
* RFC 6455 compliant
* Flexible architecture
* Production tested
* Easy integration

### Cloudflare Workers
* Edge WebSocket support
* Global distribution
* Low latency
* Production tested at massive scale
* Efficient protocol handling

### Standard Libraries
* RFC 6455 WebSocket protocol standard
* Standard WebSocket implementations
* Production grade WebSocket practices

## WebSocket Protocol

### RFC 6455
* **Definition**: WebSocket protocol standard
* **Features**: Bidirectional, full duplex, frame based
* **Use cases**: Real time chat, gaming, live updates
* **Rationale**: RFC 6455 enables real time communication

### Handshake
* **HTTP Upgrade**: Upgrade HTTP connection to WebSocket
* **Key exchange**: Sec WebSocket Key/Accept exchange
* **Protocol negotiation**: Subprotocol negotiation
* **Rationale**: Handshake establishes WebSocket connection

### Frame Format
* **FIN**: Final frame flag
* **RSV**: Reserved bits
* **Opcode**: Frame type (text, binary, close, ping, pong)
* **Mask**: Masking flag
* **Payload length**: Payload length
* **Masking key**: Masking key (if masked)
* **Payload**: Frame payload
* **Rationale**: Frame format enables message framing

## Server Architecture

### Connection Lifecycle
* **Accept**: Accept incoming connections
* **Upgrade**: HTTP upgrade to WebSocket
* **Authenticate**: Authenticate connections
* **Steady state**: Handle messages
* **Drain**: Graceful shutdown
* **Close**: Close connections
* **Rationale**: Lifecycle management ensures reliability

### Event Driven I/O
* **epoll/kqueue**: Event notification mechanisms
* **Non blocking**: Non blocking socket operations
* **Backpressure**: Flow control and backpressure
* **Rationale**: Event driven I/O enables scalability

### Memory Management
* **Ring buffers**: Efficient buffer management
* **Slab allocators**: Fast memory allocation
* **Pool allocators**: Object pooling
* **Zero copy**: Zero copy operations
* **Rationale**: Memory management improves performance

## Scale Out and Distribution

### Load Balancing
* **Sticky routing**: Consistent connection routing
* **L4/L7**: Layer 4 and Layer 7 load balancing
* **Proxy timeouts**: Load balancer timeout tuning
* **Rationale**: Load balancing enables horizontal scaling

### Pub/Sub Backbone
* **Redis/NATS/Kafka**: Pub/Sub message brokers
* **Multi node broadcast**: Cross node message delivery
* **Presence service**: Connection presence tracking
* **Rationale**: Pub/Sub enables distributed systems

### Horizontal Sharding
* **Rooms/channels**: Shard by channels
* **Partitions**: Partition by connection hash
* **Subscription indices**: Efficient subscription lookup
* **Rationale**: Sharding enables scale

## Reliability and Client Experience

### Reconnect Strategy
* **Exponential backoff**: Jittered exponential backoff
* **Session resumption**: Session resumption tokens
* **Rationale**: Reconnect strategy improves reliability

### Delivery Semantics
* **At most once**: Fire and forget delivery
* **At least once**: Acknowledged delivery
* **Ack/nack**: Acknowledgment mechanisms
* **Rationale**: Delivery semantics ensure correctness

### Backpressure
* **Credit based flow**: Credit based flow control
* **Per conn quotas**: Per connection quotas
* **Drop policies**: Message drop policies
* **Rationale**: Backpressure prevents resource exhaustion

## Security and Compliance

### TLS/SSL
* **TLS termination**: TLS termination at proxy or server
* **ALPN**: Application Layer Protocol Negotiation
* **Session tickets**: TLS session tickets
* **mTLS**: Mutual TLS for authentication
* **Rationale**: TLS enables secure communication

### Authentication and Authorization
* **JWT**: JSON Web Token authentication
* **Origin checks**: Origin validation
* **Subprotocol allow list**: Subprotocol validation
* **ACLs**: Access control lists
* **Rationale**: Authentication ensures security

### Abuse Controls
* **Rate limiting**: Rate limiting per IP/connection
* **DDoS mitigation**: DDoS attack mitigation
* **Slowloris defense**: Slowloris attack prevention
* **Rationale**: Abuse controls prevent attacks

## Observability and Operations

### Metrics
* **Connection counts**: Active connection metrics
* **Handshake errors**: Handshake error metrics
* **Latency**: P50/P95/P99 latency metrics
* **Queue depth**: Message queue depth metrics
* **Rationale**: Metrics enable monitoring

### Logging
* **Structured logs**: Structured logging with connection ID, tenant ID, trace ID
* **Distributed tracing**: Distributed tracing support
* **Rationale**: Logging enables debugging

### Operations
* **Runbooks**: Operational runbooks
* **SLOs**: Service level objectives
* **Load tests**: Load testing procedures
* **Chaos drills**: Chaos engineering drills
* **Rationale**: Operations ensure reliability

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
* RFC 6455 compliance
* Proper error handling
* Correct protocol implementation
* Comprehensive test coverage
* Security best practices

### Documentation
* API documentation for all public functions
* Protocol implementation details
* Error handling guarantees
* Performance characteristics
* Thread safety guarantees

## Research Papers and References

### WebSocket Protocol
* RFC 6455 - WebSocket protocol specification
* RFC 7692 - permessage-deflate compression
* RFC 8441 - HTTP/2 WebSocket extension
* WebSocket implementation guides

### Performance
* "High Performance Browser Networking" - Network optimization
* "The C10K Problem" - Concurrency patterns
* "Event Driven Architecture" research papers

### Open Source References
* uWebSockets source code
* Boost.Beast implementation
* websocketpp library
* Standard WebSocket implementations

## Implementation Goals

### Correctness
* RFC 6455 compliance
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

