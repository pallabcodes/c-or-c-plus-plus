# Socket Programming Standards

## Overview
Socket programming is the foundation of network communication. This document defines standards for implementing production grade socket operations including TCP/UDP sockets, address handling, and connection management.

## Socket Types

### TCP Sockets
* **Definition**: Stream oriented reliable communication
* **Use cases**: HTTP, WebSocket, database connections
* **Characteristics**: Reliable, ordered, connection oriented
* **Rationale**: TCP enables reliable communication

### UDP Sockets
* **Definition**: Datagram oriented communication
* **Use cases**: DNS, real time streaming, gaming
* **Characteristics**: Unreliable, unordered, connectionless
* **Rationale**: UDP enables low latency communication

## Socket Creation

### TCP Socket Creation
* **socket()**: Create socket file descriptor
* **bind()**: Bind to address and port
* **listen()**: Listen for connections
* **accept()**: Accept incoming connections
* **Rationale**: Socket creation enables server setup

### UDP Socket Creation
* **socket()**: Create socket file descriptor
* **bind()**: Bind to address and port
* **recvfrom()**: Receive datagrams
* **sendto()**: Send datagrams
* **Rationale**: UDP socket creation enables datagram communication

## Address Handling

### IP Address Management
* **IPv4**: 32 bit addresses
* **IPv6**: 128 bit addresses
* **Address conversion**: Use inet_pton/inet_ntop
* **Rationale**: Address handling enables network communication

### Port Management
* **Port range**: 0-65535
* **Well known ports**: 0-1023
* **Ephemeral ports**: 1024-65535
* **Rationale**: Port management enables service identification

### Example Address Handling
```cpp
class SocketAddress {
public:
    static Result<SocketAddress> from_ip_port(const std::string& ip, uint16_t port) {
        SocketAddress addr;
        if (inet_pton(AF_INET, ip.c_str(), &addr.addr_.sin_addr) != 1) {
            return std::unexpected("Invalid IP address");
        }
        addr.addr_.sin_family = AF_INET;
        addr.addr_.sin_port = htons(port);
        return addr;
    }
    
private:
    sockaddr_in addr_;
};
```

## Connection Management

### Client Connection
* **connect()**: Connect to server
* **Error handling**: Handle connection errors
* **Timeout**: Set connection timeout
* **Rationale**: Client connection enables client server communication

### Server Connection
* **accept()**: Accept client connections
* **Connection handling**: Handle multiple connections
* **Connection pooling**: Reuse connections
* **Rationale**: Server connection enables server functionality

## Non Blocking I/O

### Non Blocking Sockets
* **fcntl()**: Set socket to non blocking
* **EAGAIN/EWOULDBLOCK**: Handle would block errors
* **Event loops**: Use with event loops
* **Rationale**: Non blocking I/O enables scalability

### Example Non Blocking
```cpp
int set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
```

## Socket Options

### Common Options
* **SO_REUSEADDR**: Allow address reuse
* **SO_KEEPALIVE**: Enable keep alive
* **TCP_NODELAY**: Disable Nagle algorithm
* **SO_LINGER**: Control socket closure
* **Rationale**: Socket options enable optimization

### Example Socket Options
```cpp
void configure_socket(int fd) {
    int reuse = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    
    int keepalive = 1;
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
    
    int nodelay = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
}
```

## Error Handling

### Socket Errors
* **errno**: Check errno for errors
* **Error codes**: Handle specific error codes
* **Error messages**: Provide clear error messages
* **Rationale**: Error handling ensures reliability

### Common Errors
* **EAGAIN/EWOULDBLOCK**: Would block (non blocking)
* **ECONNREFUSED**: Connection refused
* **ETIMEDOUT**: Connection timeout
* **EINTR**: Interrupted system call
* **Rationale**: Understanding errors enables proper handling

## Implementation Standards

### Correctness
* **Error handling**: Proper error handling
* **Resource cleanup**: Proper resource cleanup
* **State management**: Proper state management
* **Rationale**: Correctness is critical

### Performance
* **Non blocking**: Use non blocking I/O
* **Socket options**: Optimize socket options
* **Connection reuse**: Reuse connections
* **Rationale**: Performance is critical

## Testing Requirements

### Unit Tests
* **Socket creation**: Test socket creation
* **Address handling**: Test address handling
* **Connection**: Test connection operations
* **Error handling**: Test error handling
* **Rationale**: Comprehensive testing ensures correctness

## Research Papers and References

### Socket Programming
* "Unix Network Programming" (Stevens) - Socket programming
* "Linux Socket Programming" guides
* Socket programming tutorials

## Implementation Checklist

- [ ] Understand TCP vs UDP
- [ ] Learn socket creation
- [ ] Understand address handling
- [ ] Learn connection management
- [ ] Practice socket programming
- [ ] Write comprehensive unit tests
- [ ] Document socket usage

