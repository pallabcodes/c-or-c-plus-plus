# Network Programming

## Scope
Applies to socket programming, TCP/UDP protocols, event-driven I/O (epoll, kqueue, select, poll), connection management, and high performance networking.

## Socket Programming Basics

### Socket Creation
* Use socket() to create sockets
* Specify domain (AF_INET, AF_INET6, AF_UNIX)
* Specify type (SOCK_STREAM for TCP, SOCK_DGRAM for UDP)
* Specify protocol (0 for default)
* Always check socket() return value

### Socket Options
* Use setsockopt() for socket configuration
* Set SO_REUSEADDR for server sockets
* Set SO_REUSEPORT for load balancing (Linux 3.9+)
* Set TCP_NODELAY to disable Nagle's algorithm
* Set SO_LINGER for graceful shutdown
* Document socket option choices

### Address Structures
* Use struct sockaddr_in for IPv4
* Use struct sockaddr_in6 for IPv6
* Use struct sockaddr_un for Unix domain sockets
* Convert between host and network byte order (htonl, ntohl)
* Use getaddrinfo() for address resolution

## TCP Programming

### Server Pattern
* Create socket with socket()
* Bind to address with bind()
* Listen for connections with listen()
* Accept connections with accept()
* Handle each connection (fork, thread, or event loop)
* Close sockets appropriately

### Client Pattern
* Create socket with socket()
* Connect to server with connect()
* Send and receive data
* Close socket when done
* Handle connection errors

### Connection Management
* Handle EINTR errors on accept() and connect()
* Use non-blocking sockets with event loops
* Implement connection timeouts
* Handle connection resets gracefully
* Document connection lifecycle

## UDP Programming

### Datagram Sockets
* Use sendto() and recvfrom() for UDP
* Handle partial datagrams
* Check return values (may send/receive less than requested)
* Handle ICMP errors (connection refused, etc.)
* Document datagram boundaries

### UDP Considerations
* No connection state
* No guarantee of delivery
* No ordering guarantees
* Maximum datagram size limits
* Use for low latency, loss-tolerant applications

## Event-Driven I/O

### select()
* Monitor multiple file descriptors
* Portable but limited scalability
* Use FD_SETSIZE limit (typically 1024)
* Handle EINTR errors
* Prefer epoll/kqueue for better performance

### poll()
* Monitor multiple file descriptors
* More scalable than select()
* No FD_SETSIZE limit
* Handle EINTR errors
* Prefer epoll/kqueue for better performance

### epoll() (Linux)
* High performance event notification
* Edge-triggered (EPOLLET) or level-triggered mode
* Use epoll_create1() with EPOLL_CLOEXEC
* Use epoll_ctl() to manage file descriptors
* Use epoll_wait() to wait for events
* Very efficient for large numbers of file descriptors

### kqueue() (macOS/BSD)
* High performance event notification
* Monitors various event types (file, socket, signal, timer)
* Use kqueue() to create kqueue
* Use kevent() to register and wait for events
* Very efficient for large numbers of file descriptors

### Event Loop Patterns
* Single-threaded event loop
* Multi-threaded event loop with work queues
* Per-core event loops (one loop per CPU core)
* Accept sharding with SO_REUSEPORT
* Document event loop architecture

## High Performance Networking

### Zero-Copy Techniques
* Use sendfile() for file to socket transfers
* Use splice() and vmsplice() for zero-copy
* Use MSG_ZEROCOPY flag (Linux 4.14+) for send()
* Minimize data copying
* Document zero-copy usage

### Connection Pooling
* Reuse connections when possible
* Implement connection pools
* Handle connection failures
* Monitor connection health
* Document pooling strategies

### Load Balancing
* Use SO_REUSEPORT for kernel-level load balancing
* Implement application-level load balancing
* Use consistent hashing for session affinity
* Monitor load distribution
* Document load balancing approach

## Implementation Standards

### Error Handling
* Check all socket function return values
* Handle EINTR, EAGAIN, EWOULDBLOCK errors
* Use perror() or strerror() with context
* Map errno to clear error messages
* Document error handling strategies

### Resource Management
* Close sockets in all code paths
* Use close on exec flag (SOCK_CLOEXEC)
* Clean up listening sockets
* Handle socket leaks
* Document socket ownership

### Security
* Validate all network inputs
* Use TLS/SSL for secure connections
* Implement rate limiting
* Protect against DoS attacks
* Document security considerations

### Performance
* Profile network I/O operations
* Optimize buffer sizes
* Minimize system calls
* Use appropriate I/O model
* Document performance characteristics

## Code Examples

### TCP Server with epoll
```cpp
// Thread-safety: Not thread-safe (single event loop)
// Ownership: Owns listening socket and epoll fd
// Invariants: port > 0, port < 65536
// Failure modes: Returns -1 on error, sets errno
int create_tcp_server_epoll(uint16_t port) {
    int listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (listen_fd < 0) {
        perror("socket failed");
        return -1;
    }
    
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        close(listen_fd);
        return -1;
    }
    
    if (listen(listen_fd, SOMAXCONN) < 0) {
        perror("listen failed");
        close(listen_fd);
        return -1;
    }
    
    int epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd < 0) {
        perror("epoll_create1 failed");
        close(listen_fd);
        return -1;
    }
    
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev) < 0) {
        perror("epoll_ctl failed");
        close(epoll_fd);
        close(listen_fd);
        return -1;
    }
    
    return epoll_fd;
}
```

### Non-blocking Socket Read
```cpp
// Thread-safety: Thread-safe (per socket)
// Ownership: Borrows fd, caller owns socket
// Invariants: fd valid, buf not null, size > 0
// Failure modes: Returns -1 on error, 0 on EOF, bytes read on success
ssize_t nonblocking_read(int fd, void* buf, size_t size) {
    ssize_t n = read(fd, buf, size);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;  // Would block
        }
        return -1;  // Error
    }
    return n;  // Bytes read or EOF
}
```

## Testing Requirements
* Test socket creation and configuration
* Test TCP connection establishment
* Test UDP datagram sending and receiving
* Test event-driven I/O (epoll, kqueue)
* Test error handling for all operations
* Test connection cleanup
* Test high load scenarios
* Verify no socket leaks

## Platform-Specific Considerations

### Linux
* Use epoll for event-driven I/O
* Use io_uring for high performance I/O
* SO_REUSEPORT available (Linux 3.9+)
* MSG_ZEROCOPY available (Linux 4.14+)

### macOS/BSD
* Use kqueue for event-driven I/O
* SO_REUSEPORT available
* Different socket option names

### Cross-Platform
* Use abstraction layer for epoll/kqueue
* Handle platform-specific socket options
* Test on all target platforms
* Document platform differences

## Related Topics
* Process Management: Process-based connection handling
* Thread Management: Thread-based connection handling
* File Operations: File descriptor management
* Performance Optimization: Network performance profiling

