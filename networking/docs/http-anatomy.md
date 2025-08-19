# The Complete Anatomy of an HTTP Request

## 🔬 Introduction

This document provides a **comprehensive breakdown** of what happens when you type `curl http://example.com` and press Enter. Every layer, every byte, every system call - explained with the depth that Google's backend engineers expect.

## 📡 The Complete Journey: From Application to Wire

### Phase 1: Application Layer Decision Making

```cpp
// What happens in your application
std::string url = "http://example.com:80/path?query=value";

// 1. URL Parsing
struct ParsedURL {
    std::string scheme;     // "http"
    std::string host;       // "example.com" 
    uint16_t port;          // 80 (default for HTTP)
    std::string path;       // "/path"
    std::string query;      // "query=value"
};
```

**Key Insights:**
- **Default ports**: HTTP (80), HTTPS (443)
- **URL encoding**: Special characters must be percent-encoded
- **Host resolution**: May involve DNS lookups (A/AAAA records)

### Phase 2: DNS Resolution (If Needed)

```cpp
// DNS Resolution Process
class DNSResolver {
    // 1. Check local cache (/etc/hosts, resolver cache)
    // 2. Query configured DNS servers (usually 8.8.8.8, 1.1.1.1)
    // 3. Recursive resolution: . -> .com -> example.com
    // 4. Return A record (IPv4) or AAAA record (IPv6)
    
    std::optional<sockaddr_in> resolve(const std::string& hostname) {
        struct addrinfo hints{}, *result;
        hints.ai_family = AF_INET;      // IPv4
        hints.ai_socktype = SOCK_STREAM; // TCP
        
        int status = getaddrinfo(hostname.c_str(), nullptr, &hints, &result);
        // ... error handling and result processing
    }
};
```

**DNS Timing Breakdown:**
- **Cache hit**: ~0.1ms
- **Cache miss (local network)**: ~10-50ms
- **Cache miss (internet)**: ~100-500ms

### Phase 3: TCP Connection Establishment

```cpp
// TCP Three-Way Handshake Implementation
class TCPConnection {
    int socket_fd_;
    
public:
    ConnectionResult connect(const sockaddr_in& server_addr) {
        // 1. Create socket
        socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd_ < 0) return ConnectionResult::SOCKET_CREATE_FAILED;
        
        // 2. Set socket options for performance
        int opt = 1;
        setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        setsockopt(socket_fd_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
        
        // 3. Initiate connection (triggers TCP handshake)
        int result = ::connect(socket_fd_, 
                             reinterpret_cast<const sockaddr*>(&server_addr),
                             sizeof(server_addr));
        
        return (result == 0) ? ConnectionResult::SUCCESS : 
                               ConnectionResult::CONNECTION_FAILED;
    }
};
```

**TCP Handshake Detailed:**
```
Client                                    Server
  |                                         |
  |----------- SYN (seq=x) --------------->|  (1)
  |                                         |
  |<------ SYN+ACK (seq=y, ack=x+1) -------|  (2)
  |                                         |
  |-------- ACK (seq=x+1, ack=y+1) ------->|  (3)
  |                                         |
  |========= CONNECTION ESTABLISHED ========|
```

**What each step does:**
1. **SYN**: Client announces its initial sequence number and desire to connect
2. **SYN+ACK**: Server acknowledges client's SYN and sends its own sequence number
3. **ACK**: Client acknowledges server's SYN, connection is now bidirectional

### Phase 4: HTTP Request Construction

```cpp
// Production-grade HTTP request builder
class HTTPRequestBuilder {
    std::string method_;
    std::string path_;
    std::string http_version_;
    std::unordered_map<std::string, std::string> headers_;
    std::string body_;
    
public:
    std::string build() const {
        std::ostringstream request;
        
        // 1. Request line: METHOD PATH HTTP/VERSION
        request << method_ << " " << path_ << " " << http_version_ << "\r\n";
        
        // 2. Headers: Key: Value
        for (const auto& [key, value] : headers_) {
            request << key << ": " << value << "\r\n";
        }
        
        // 3. Empty line to separate headers from body
        request << "\r\n";
        
        // 4. Body (if any)
        if (!body_.empty()) {
            request << body_;
        }
        
        return request.str();
    }
};
```

**Example HTTP Request Breakdown:**
```http
GET /path?query=value HTTP/1.1\r\n          ← Request Line
Host: example.com\r\n                       ← Mandatory in HTTP/1.1
User-Agent: MyClient/1.0\r\n                ← Client identification
Accept: text/html,application/json\r\n      ← Content negotiation
Connection: keep-alive\r\n                  ← Connection management
\r\n                                        ← Header-body separator
```

**Critical HTTP/1.1 Requirements:**
- **Host header**: Mandatory for virtual hosting
- **Content-Length**: Required for requests with body
- **Connection**: Controls connection lifecycle
- **CRLF line endings**: Must be `\r\n`, not just `\n`

### Phase 5: Sending Data Through the Network Stack

```cpp
// Zero-copy sending implementation
class NetworkSender {
    int socket_fd_;
    
public:
    SendResult send_request(const std::string& request) {
        // 1. Prepare data for sending
        const char* data = request.c_str();
        size_t total_bytes = request.length();
        size_t bytes_sent = 0;
        
        // 2. Send with partial send handling
        while (bytes_sent < total_bytes) {
            ssize_t result = send(socket_fd_, 
                                data + bytes_sent, 
                                total_bytes - bytes_sent, 
                                MSG_NOSIGNAL);
            
            if (result < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // Socket buffer is full, would block
                    // In production: use epoll to wait for EPOLLOUT
                    continue;
                }
                return SendResult::ERROR;
            }
            
            bytes_sent += result;
        }
        
        return SendResult::SUCCESS;
    }
};
```

**What happens in the kernel:**
1. **User space to kernel**: `send()` copies data to kernel socket buffer
2. **TCP segmentation**: Large messages split into TCP segments (MSS size)
3. **IP packaging**: TCP segments wrapped in IP packets
4. **Network interface**: Packets queued for transmission
5. **Physical wire**: Electrical/optical signals represent bits

### Phase 6: Server-Side Request Processing

```cpp
// Production HTTP server request handling
class HTTPServer {
    class RequestProcessor {
    public:
        HTTPResponse process(const HTTPRequest& request) {
            // 1. Request validation
            if (!validate_request(request)) {
                return HTTPResponse::bad_request();
            }
            
            // 2. Route resolution
            auto handler = route_table_.find_handler(request.path());
            if (!handler) {
                return HTTPResponse::not_found();
            }
            
            // 3. Authentication/Authorization
            if (!auth_manager_.authorize(request)) {
                return HTTPResponse::unauthorized();
            }
            
            // 4. Business logic execution
            try {
                return handler->handle(request);
            } catch (const std::exception& e) {
                logger_.error("Request processing failed: {}", e.what());
                return HTTPResponse::internal_server_error();
            }
        }
    };
};
```

**Server Request Processing Pipeline:**
```
[Raw TCP Data] 
    ↓
[HTTP Parser] → Parses headers, validates format
    ↓  
[Request Object] → Structured representation
    ↓
[Middleware Chain] → Auth, logging, rate limiting
    ↓
[Route Handler] → Business logic
    ↓
[Response Builder] → Creates HTTP response
    ↓
[TCP Send] → Sends back to client
```

### Phase 7: HTTP Response Construction and Sending

```cpp
// HTTP response with proper status codes
class HTTPResponse {
    uint16_t status_code_;
    std::string reason_phrase_;
    HeaderMap headers_;
    std::string body_;
    
public:
    std::string serialize() const {
        std::ostringstream response;
        
        // 1. Status line
        response << "HTTP/1.1 " << status_code_ << " " 
                << reason_phrase_ << "\r\n";
        
        // 2. Headers
        response << "Server: ProductionServer/1.0\r\n";
        response << "Date: " << get_http_date() << "\r\n";
        response << "Content-Length: " << body_.length() << "\r\n";
        
        for (const auto& [key, value] : headers_) {
            response << key << ": " << value << "\r\n";
        }
        
        // 3. Header-body separator
        response << "\r\n";
        
        // 4. Body
        response << body_;
        
        return response.str();
    }
};
```

**Response Status Codes by Category:**
- **1xx Informational**: 100 Continue, 101 Switching Protocols
- **2xx Success**: 200 OK, 201 Created, 204 No Content
- **3xx Redirection**: 301 Moved Permanently, 304 Not Modified
- **4xx Client Error**: 400 Bad Request, 401 Unauthorized, 404 Not Found
- **5xx Server Error**: 500 Internal Server Error, 503 Service Unavailable

### Phase 8: Connection Management

```cpp
// Connection lifecycle management
class ConnectionManager {
public:
    enum class ConnectionAction {
        KEEP_ALIVE,     // Reuse for next request
        CLOSE,          // Close after response
        UPGRADE         // Upgrade to WebSocket/HTTP2
    };
    
    ConnectionAction determine_action(const HTTPRequest& request, 
                                    const HTTPResponse& response) {
        // 1. Check Connection header
        auto conn_header = request.header("Connection");
        if (conn_header == "close") {
            return ConnectionAction::CLOSE;
        }
        
        // 2. Check for protocol upgrade
        auto upgrade_header = request.header("Upgrade");
        if (upgrade_header == "websocket") {
            return ConnectionAction::UPGRADE;
        }
        
        // 3. Default to keep-alive in HTTP/1.1
        return ConnectionAction::KEEP_ALIVE;
    }
};
```

## 🔧 Advanced HTTP Features

### Chunked Transfer Encoding

When you don't know response size upfront:

```cpp
class ChunkedResponseSender {
public:
    void send_chunk(int socket_fd, const std::string& data) {
        // 1. Send chunk size in hexadecimal
        std::string chunk_size = to_hex(data.length()) + "\r\n";
        send(socket_fd, chunk_size.c_str(), chunk_size.length(), 0);
        
        // 2. Send chunk data
        send(socket_fd, data.c_str(), data.length(), 0);
        
        // 3. Send trailing CRLF
        send(socket_fd, "\r\n", 2, 0);
    }
    
    void send_final_chunk(int socket_fd) {
        // Send zero-sized chunk to indicate end
        send(socket_fd, "0\r\n\r\n", 5, 0);
    }
};
```

### HTTP Pipelining

Multiple requests on same connection:

```cpp
// Client sends multiple requests without waiting
send_request("GET /page1 HTTP/1.1\r\n...");
send_request("GET /page2 HTTP/1.1\r\n...");
send_request("GET /page3 HTTP/1.1\r\n...");

// Server must respond in same order (FIFO)
```

## ⚡ Performance Considerations

### Memory Management
```cpp
// Custom allocator for HTTP parsing
class HTTPMemoryPool {
    static constexpr size_t BLOCK_SIZE = 4096;
    std::vector<std::unique_ptr<uint8_t[]>> memory_blocks_;
    size_t current_offset_ = 0;
    
public:
    // Fast allocation for short-lived HTTP objects
    void* allocate(size_t size) {
        if (current_offset_ + size > BLOCK_SIZE) {
            allocate_new_block();
        }
        
        void* ptr = &memory_blocks_.back()[current_offset_];
        current_offset_ += size;
        return ptr;
    }
};
```

### Zero-Copy Techniques
```cpp
// Avoid copying large response bodies
class ZeroCopyFileResponse {
public:
    void send_file(int socket_fd, const std::string& file_path) {
        int file_fd = open(file_path.c_str(), O_RDONLY);
        
        // Use sendfile() to copy directly from file to socket
        // Bypasses user space entirely
        off_t offset = 0;
        struct stat file_stat;
        fstat(file_fd, &file_stat);
        
        sendfile(socket_fd, file_fd, &offset, file_stat.st_size);
        close(file_fd);
    }
};
```

## 🚀 Real-World Optimizations

### Connection Pooling
```cpp
// Reuse connections to same server
class ConnectionPool {
    std::unordered_map<std::string, std::queue<TCPConnection>> pools_;
    std::mutex mutex_;
    
public:
    TCPConnection get_connection(const std::string& host, uint16_t port) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::string key = host + ":" + std::to_string(port);
        auto& pool = pools_[key];
        
        if (!pool.empty()) {
            auto conn = std::move(pool.front());
            pool.pop();
            return conn;
        }
        
        // Create new connection if pool is empty
        return TCPConnection::connect(host, port);
    }
};
```

### HTTP/2 Preview
```cpp
// HTTP/2 introduces binary framing
struct HTTP2Frame {
    uint32_t length : 24;
    uint8_t type;
    uint8_t flags;
    uint32_t stream_id : 31;
    uint8_t reserved : 1;
    std::vector<uint8_t> payload;
};

// Multiplexing: Multiple streams on single connection
class HTTP2Connection {
    std::unordered_map<uint32_t, HTTP2Stream> streams_;
    // No head-of-line blocking like HTTP/1.1
};
```

## 📊 Monitoring & Observability

```cpp
// Production metrics collection
class HTTPMetrics {
    std::atomic<uint64_t> total_requests_{0};
    std::atomic<uint64_t> total_bytes_sent_{0};
    LatencyHistogram response_time_histogram_;
    
public:
    void record_request(const HTTPRequest& req, 
                       const HTTPResponse& resp,
                       std::chrono::nanoseconds latency) {
        total_requests_++;
        total_bytes_sent_ += resp.body().size();
        response_time_histogram_.record(latency);
        
        // Log in structured format for analysis
        logger_.info("http_request", {
            {"method", req.method()},
            {"path", req.path()},
            {"status", resp.status_code()},
            {"latency_ns", latency.count()},
            {"bytes_sent", resp.body().size()}
        });
    }
};
```

## 🎯 Key Takeaways

1. **HTTP is stateless** - Each request is independent
2. **TCP provides reliability** - Retransmissions, ordering, flow control  
3. **Connection reuse is critical** - TCP handshake cost is significant
4. **Buffer management matters** - Avoid copies, use scatter-gather I/O
5. **Error handling is essential** - Network failures are common
6. **Monitoring enables optimization** - Measure what matters

This deep understanding of HTTP internals is what separates senior engineers from junior ones. Every optimization, every architectural decision in web systems stems from understanding these fundamentals.

---

*Next: [WebSocket Protocol Deep Dive](websocket-anatomy.md)*
