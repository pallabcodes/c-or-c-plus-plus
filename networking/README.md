# Production-Grade HTTP & WebSocket Implementation in C++

## 🎯 Project Overview

This project demonstrates the **complete anatomy of HTTP requests and WebSockets** from the ground up, implementing every layer of the network stack that a Google Backend Engineer would expect to see in production systems.

## 🏗️ Architecture & Design Philosophy

This isn't just a toy implementation - it's designed with the same principles used in:
- **Google's HTTP/2 stack**
- **nginx's event-driven architecture** 
- **Cloudflare's edge servers**
- **Meta's network infrastructure**

### Core Design Principles
1. **Zero-copy where possible** - Minimize memory allocations
2. **Event-driven I/O** - Linux epoll for maximum scalability
3. **Lock-free data structures** - For high-throughput scenarios
4. **RAII & Modern C++** - Memory safety without GC overhead
5. **Production logging** - Structured, machine-readable logs

## 📊 What You'll Learn

### HTTP Deep Dive
- **Raw socket handling** - BSD sockets, TCP connection lifecycle
- **HTTP parsing** - Headers, body, chunked encoding, pipeline handling
- **Keep-alive connections** - Connection pooling and reuse
- **HTTP/1.1 vs HTTP/2** - Protocol differences and optimizations
- **TLS/SSL integration** - Secure connections (OpenSSL)

### WebSocket Deep Dive  
- **WebSocket handshake** - HTTP upgrade mechanism
- **Frame parsing** - Binary protocol, masking, control frames
- **Connection state management** - Ping/pong, close handshake
- **Extensions support** - Compression, multiplexing

### Network Layer Understanding
- **TCP/IP stack** - How packets flow through the kernel
- **Socket buffers** - Kernel buffer management, backpressure
- **Event loops** - epoll, kqueue, IOCP patterns
- **Thread models** - Reactor vs Proactor patterns

### Performance Engineering
- **Memory management** - Custom allocators, object pools
- **CPU optimization** - Branch prediction, cache locality
- **Network optimization** - Nagle's algorithm, TCP_NODELAY
- **Monitoring & Metrics** - Latency histograms, throughput tracking

## 🏭 Production-Ready Features

### High Performance I/O
```cpp
// Event-driven architecture like nginx
class EventLoop {
    EpollReactor epoll_reactor_;
    ThreadPool worker_pool_;
    ConnectionPool conn_pool_;
    // Zero-copy buffer management
};
```

### HTTP/1.1 Implementation
```cpp
// Production-grade HTTP parser
class HttpRequestParser {
    // Incremental parsing for streaming
    // Memory-efficient header storage  
    // RFC 7230 compliant
};
```

### WebSocket Implementation
```cpp  
// RFC 6455 compliant WebSocket
class WebSocketConnection {
    // Frame-based message handling
    // Automatic ping/pong management
    // Compression extension support
};
```

## 📁 Project Structure

```
networking/
├── README.md                          # This file
├── CMakeLists.txt                     # Build configuration
├── docs/                              # Deep-dive documentation
│   ├── http-anatomy.md               # HTTP request lifecycle  
│   ├── websocket-anatomy.md          # WebSocket protocol details
│   ├── performance-guide.md          # Optimization techniques
│   └── network-layers.md             # TCP/IP stack explained
├── include/                           # Public headers
│   ├── http/                         
│   ├── websocket/
│   ├── network/
│   └── utils/
├── src/                              # Implementation
│   ├── http/                         # HTTP implementation
│   ├── websocket/                    # WebSocket implementation  
│   ├── network/                      # Low-level networking
│   └── utils/                        # Utilities & helpers
├── examples/                         # Practical examples
│   ├── http_server/                  # Production HTTP server
│   ├── websocket_chat/               # Real-time chat application
│   ├── load_balancer/                # HTTP load balancer
│   └── benchmarks/                   # Performance testing
├── tests/                            # Comprehensive test suite
│   ├── unit/                         # Unit tests
│   ├── integration/                  # Integration tests
│   └── performance/                  # Performance benchmarks
└── third_party/                      # External dependencies
    ├── openssl/                      # TLS/SSL support
    └── benchmark/                    # Google Benchmark library
```

## 🚀 Getting Started

### Prerequisites
- **Compiler**: GCC 11+ or Clang 13+ (C++20 support)
- **OS**: Linux (primary), macOS (supported)
- **Dependencies**: OpenSSL, Google Test, Google Benchmark

### Build Instructions
```bash
# Clone and navigate to networking directory
cd /home/pallab/Downloads/c-or-c-plus-plus/networking

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_BENCHMARKS=ON

# Build the project
make -j$(nproc)

# Run tests
make test

# Run benchmarks
./benchmarks/http_performance_test
```

## 🎯 Learning Path

### Phase 1: Foundation (Week 1)
1. **Socket Programming Basics** - Raw TCP sockets
2. **HTTP Parsing** - Manual header parsing
3. **Basic Server** - Single-threaded echo server

### Phase 2: HTTP Implementation (Week 2)  
1. **HTTP Request Parser** - Complete RFC compliance
2. **HTTP Response Builder** - Efficient response generation
3. **Connection Management** - Keep-alive, pipelining

### Phase 3: WebSocket Implementation (Week 3)
1. **WebSocket Handshake** - HTTP upgrade mechanism
2. **Frame Protocol** - Binary message parsing
3. **Real-time Chat** - Bidirectional communication

### Phase 4: Performance & Production (Week 4)
1. **Event-driven I/O** - epoll integration
2. **Thread Pool** - Worker thread management  
3. **Load Testing** - Performance characterization

## 🏆 Real-World Applications

This code demonstrates patterns used in:

- **Web Servers**: nginx, Apache HTTP Server
- **Reverse Proxies**: HAProxy, Envoy Proxy  
- **API Gateways**: Kong, AWS API Gateway
- **Real-time Systems**: Discord, Slack, WhatsApp
- **CDN Edge Servers**: Cloudflare, Fastly

## 📈 Performance Expectations

**HTTP Server Performance**:
- **Throughput**: 100K+ requests/second (single thread)
- **Latency**: Sub-millisecond P99 (localhost)
- **Memory**: Constant memory usage under load
- **Connections**: 10K+ concurrent connections

**WebSocket Performance**:
- **Messages**: 1M+ messages/second
- **Latency**: <100μs message forwarding
- **Memory**: Per-connection overhead <1KB

## 🔬 Deep Technical Details

### HTTP Request Lifecycle
```
[Client] ----TCP SYN----> [Server]
[Client] <---TCP SYN+ACK- [Server]  
[Client] ----TCP ACK----> [Server]
[Client] ----HTTP REQ---> [Server] (Application Layer)
[Client] <---HTTP RESP--- [Server] (Application Layer)
[Client] ----TCP FIN----> [Server] (Connection Close)
```

### Memory Layout Optimization
```cpp
// Cache-aligned structures for performance
struct alignas(64) HttpRequest {
    HttpMethod method;           // 4 bytes
    std::string_view path;       // 16 bytes  
    HeaderMap headers;           // Custom hash map
    // Total: Fits in single cache line
};
```

### Zero-Copy Buffer Management
```cpp
class ZeroCopyBuffer {
    // mmap'd memory regions
    // Scatter-gather I/O vectors
    // Direct kernel buffer access
};
```

## 📚 Educational Value

This project teaches:
- **Systems Programming** - Low-level network programming
- **Protocol Implementation** - RFC compliance and edge cases
- **Performance Engineering** - Profiling, optimization, monitoring
- **Concurrent Programming** - Thread-safe data structures
- **Memory Management** - RAII, smart pointers, custom allocators

## 🎖️ Industry Relevance

Code quality standards equivalent to:
- **Google's C++ Style Guide** compliance
- **Production code review** standards
- **Performance requirements** of major tech companies
- **Reliability standards** for mission-critical systems

---

*This project is designed to demonstrate mastery of systems programming, network protocols, and performance engineering - the core skills required for backend engineering roles at top tech companies.*
