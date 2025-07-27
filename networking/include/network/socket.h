#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <string>
#include <chrono>
#include <memory>
#include <optional>
#include <expected>

namespace networking {

/**
 * @brief Result type for socket operations
 * 
 * Uses C++23 std::expected for elegant error handling without exceptions.
 * This pattern is used extensively in systems programming for performance.
 */
template<typename T>
using Result = std::expected<T, std::string>;

/**
 * @brief Socket address wrapper for type safety
 * 
 * Encapsulates sockaddr_in to prevent common network programming errors
 * like endianness issues and address family mismatches.
 */
class SocketAddress {
public:
    SocketAddress() = default;
    
    /**
     * @brief Create address from IP string and port
     * @param ip IPv4 address in dotted decimal notation (e.g., "192.168.1.1")
     * @param port Port number in host byte order
     */
    static Result<SocketAddress> from_ip_port(const std::string& ip, uint16_t port);
    
    /**
     * @brief Create address for any interface on given port
     * @param port Port number in host byte order
     */
    static SocketAddress any_address(uint16_t port);
    
    /**
     * @brief Create loopback address (127.0.0.1) on given port
     */
    static SocketAddress loopback_address(uint16_t port);
    
    // Accessors
    std::string ip() const;
    uint16_t port() const;
    const sockaddr* sockaddr_ptr() const;
    socklen_t sockaddr_len() const;
    
    // String representation for logging
    std::string to_string() const;

private:
    sockaddr_in addr_{};
    explicit SocketAddress(const sockaddr_in& addr) : addr_(addr) {}
};

/**
 * @brief Socket options for fine-tuning network behavior
 * 
 * Encapsulates common socket options that affect performance:
 * - TCP_NODELAY: Disable Nagle's algorithm for low latency
 * - SO_REUSEADDR: Allow address reuse for quick restart
 * - SO_KEEPALIVE: Enable TCP keep-alive mechanism
 * - Receive/Send buffer sizes: Tune for throughput vs memory
 */
struct SocketOptions {
    bool reuse_address = true;      // SO_REUSEADDR
    bool nodelay = true;            // TCP_NODELAY (disable Nagle)
    bool keepalive = false;         // SO_KEEPALIVE
    std::optional<int> recv_buffer_size; // SO_RCVBUF
    std::optional<int> send_buffer_size; // SO_SNDBUF
    std::optional<std::chrono::milliseconds> recv_timeout; // SO_RCVTIMEO
    std::optional<std::chrono::milliseconds> send_timeout; // SO_SNDTIMEO
    
    // Non-blocking I/O flag
    bool non_blocking = false;
};

/**
 * @brief RAII wrapper for BSD sockets
 * 
 * Provides type-safe, exception-safe socket management with automatic
 * cleanup. Designed for high-performance network programming with
 * zero-cost abstractions.
 */
class Socket {
public:
    /**
     * @brief Socket creation result
     */
    enum class Type {
        TCP,  // SOCK_STREAM
        UDP   // SOCK_DGRAM  
    };
    
    /**
     * @brief Connection state for state machine management
     */
    enum class State {
        CLOSED,
        CONNECTING,
        CONNECTED,
        LISTENING,
        ERROR
    };

    // Move-only type (sockets cannot be copied)
    Socket() = default;
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;
    ~Socket();

    /**
     * @brief Create a new socket
     * @param type Socket type (TCP or UDP)
     * @param options Socket configuration options
     */
    static Result<Socket> create(Type type, const SocketOptions& options = {});
    
    /**
     * @brief Create socket from existing file descriptor
     * 
     * Used for accepted connections from listening sockets.
     * Takes ownership of the file descriptor.
     */
    static Socket from_fd(int fd, const SocketAddress& peer_addr);

    // Connection operations
    Result<void> bind(const SocketAddress& address);
    Result<void> listen(int backlog = SOMAXCONN);
    Result<Socket> accept();
    Result<void> connect(const SocketAddress& address);
    
    // I/O operations with proper error handling
    Result<size_t> send(const void* data, size_t length, int flags = 0);
    Result<size_t> recv(void* buffer, size_t length, int flags = 0);
    
    // Vectored I/O for zero-copy operations
    Result<size_t> sendv(const struct iovec* iov, int iovcnt);
    Result<size_t> recvv(const struct iovec* iov, int iovcnt);
    
    // Socket information
    int fd() const { return fd_; }
    State state() const { return state_; }
    bool is_valid() const { return fd_ >= 0; }
    
    // Address information
    Result<SocketAddress> local_address() const;
    Result<SocketAddress> peer_address() const;
    
    // Socket options management
    Result<void> set_option(int level, int optname, const void* optval, socklen_t optlen);
    Result<void> get_option(int level, int optname, void* optval, socklen_t* optlen);
    
    // Convenience methods for common options
    Result<void> set_non_blocking(bool non_blocking);
    Result<void> set_nodelay(bool nodelay);
    Result<void> set_reuseaddr(bool reuse);
    Result<void> set_keepalive(bool keepalive);
    
    // Connection management
    void close();
    Result<void> shutdown(int how = SHUT_RDWR);
    
    // Error handling
    int last_error() const;
    std::string error_string() const;

private:
    int fd_ = -1;
    State state_ = State::CLOSED;
    Type type_ = Type::TCP;
    SocketAddress peer_address_{};
    
    explicit Socket(int fd, Type type) : fd_(fd), type_(type) {}
    
    Result<void> apply_options(const SocketOptions& options);
    void update_state();
};

/**
 * @brief Non-blocking socket operations with timeout support
 * 
 * Provides async-like operations that can be used with event loops
 * like epoll/kqueue for high-performance I/O multiplexing.
 */
class AsyncSocket {
public:
    explicit AsyncSocket(Socket socket) : socket_(std::move(socket)) {}
    
    /**
     * @brief Non-blocking connect with timeout
     * @param address Target address to connect to
     * @param timeout Maximum time to wait for connection
     * @return true if connected, false if would block, error on failure
     */
    Result<bool> connect_async(const SocketAddress& address, 
                              std::chrono::milliseconds timeout = std::chrono::milliseconds::zero());
    
    /**
     * @brief Non-blocking send with partial send handling
     * @param data Data to send
     * @param length Length of data
     * @return Number of bytes actually sent, or error
     */
    Result<size_t> send_async(const void* data, size_t length);
    
    /**
     * @brief Non-blocking receive
     * @param buffer Buffer to receive into
     * @param length Buffer size
     * @return Number of bytes received, 0 for would block, error on failure
     */
    Result<size_t> recv_async(void* buffer, size_t length);
    
    Socket& socket() { return socket_; }
    const Socket& socket() const { return socket_; }

private:
    Socket socket_;
};

/**
 * @brief Socket factory for creating configured sockets
 * 
 * Centralizes socket creation with consistent options for production use.
 * Implements factory pattern for consistent socket configuration across
 * the application.
 */
class SocketFactory {
public:
    explicit SocketFactory(SocketOptions default_options = {})
        : default_options_(default_options) {}
    
    /**
     * @brief Create TCP socket with default options
     */
    Result<Socket> create_tcp_socket(const SocketOptions& options = {});
    
    /**
     * @brief Create UDP socket with default options
     */
    Result<Socket> create_udp_socket(const SocketOptions& options = {});
    
    /**
     * @brief Create listening TCP socket bound to address
     */
    Result<Socket> create_listener(const SocketAddress& bind_addr,
                                  int backlog = SOMAXCONN,
                                  const SocketOptions& options = {});
    
    /**
     * @brief Create connected TCP socket
     */
    Result<Socket> create_connection(const SocketAddress& connect_addr,
                                    const SocketOptions& options = {});

private:
    SocketOptions default_options_;
    
    SocketOptions merge_options(const SocketOptions& override_options);
};

/**
 * @brief Socket statistics for monitoring and debugging
 * 
 * Provides detailed socket metrics for production monitoring.
 * Essential for diagnosing network performance issues.
 */
struct SocketStats {
    uint64_t bytes_sent = 0;
    uint64_t bytes_received = 0;
    uint64_t send_calls = 0;
    uint64_t recv_calls = 0;
    uint64_t errors = 0;
    std::chrono::steady_clock::time_point created_at;
    std::chrono::steady_clock::time_point connected_at;
    std::chrono::nanoseconds total_send_time{0};
    std::chrono::nanoseconds total_recv_time{0};
    
    // Calculate derived metrics
    double avg_send_latency() const;
    double avg_recv_latency() const;
    double send_throughput_mbps() const;
    double recv_throughput_mbps() const;
};

/**
 * @brief Instrumented socket wrapper for performance monitoring
 * 
 * Wraps Socket with detailed metrics collection for production
 * monitoring and performance analysis.
 */
class InstrumentedSocket {
public:
    explicit InstrumentedSocket(Socket socket) 
        : socket_(std::move(socket)) {
        stats_.created_at = std::chrono::steady_clock::now();
    }
    
    // Delegate to underlying socket with metrics collection
    Result<size_t> send(const void* data, size_t length, int flags = 0);
    Result<size_t> recv(void* buffer, size_t length, int flags = 0);
    
    const SocketStats& stats() const { return stats_; }
    Socket& socket() { return socket_; }
    const Socket& socket() const { return socket_; }

private:
    Socket socket_;
    SocketStats stats_;
};

} // namespace networking
