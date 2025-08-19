#include "network/socket.h"
#include <sstream>
#include <cstring>
#include <sys/time.h>
#include <netinet/tcp.h>

namespace networking {

// SocketAddress implementation
Result<SocketAddress> SocketAddress::from_ip_port(const std::string& ip, uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);  // Convert to network byte order
    
    // Convert IP string to binary form
    int result = inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
    if (result <= 0) {
        if (result == 0) {
            return std::unexpected("Invalid IP address format: " + ip);
        } else {
            return std::unexpected("inet_pton failed: " + std::string(strerror(errno)));
        }
    }
    
    return SocketAddress(addr);
}

SocketAddress SocketAddress::any_address(uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;  // Accept connections on any interface
    addr.sin_port = htons(port);
    return SocketAddress(addr);
}

SocketAddress SocketAddress::loopback_address(uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // 127.0.0.1
    addr.sin_port = htons(port);
    return SocketAddress(addr);
}

std::string SocketAddress::ip() const {
    char ip_str[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &addr_.sin_addr, ip_str, INET_ADDRSTRLEN) == nullptr) {
        return "invalid";
    }
    return std::string(ip_str);
}

uint16_t SocketAddress::port() const {
    return ntohs(addr_.sin_port);  // Convert from network byte order
}

const sockaddr* SocketAddress::sockaddr_ptr() const {
    return reinterpret_cast<const sockaddr*>(&addr_);
}

socklen_t SocketAddress::sockaddr_len() const {
    return sizeof(addr_);
}

std::string SocketAddress::to_string() const {
    return ip() + ":" + std::to_string(port());
}

// Socket implementation
Socket::Socket(Socket&& other) noexcept 
    : fd_(other.fd_), state_(other.state_), type_(other.type_), 
      peer_address_(other.peer_address_) {
    other.fd_ = -1;
    other.state_ = State::CLOSED;
}

Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        close();  // Close current socket if valid
        
        fd_ = other.fd_;
        state_ = other.state_;
        type_ = other.type_;
        peer_address_ = other.peer_address_;
        
        other.fd_ = -1;
        other.state_ = State::CLOSED;
    }
    return *this;
}

Socket::~Socket() {
    close();
}

Result<Socket> Socket::create(Type type, const SocketOptions& options) {
    // Create socket
    int socket_type = (type == Type::TCP) ? SOCK_STREAM : SOCK_DGRAM;
    int fd = socket(AF_INET, socket_type, 0);
    
    if (fd < 0) {
        return std::unexpected("Failed to create socket: " + std::string(strerror(errno)));
    }
    
    Socket sock(fd, type);
    
    // Apply socket options
    if (auto result = sock.apply_options(options); !result) {
        return std::unexpected(result.error());
    }
    
    return sock;
}

Socket Socket::from_fd(int fd, const SocketAddress& peer_addr) {
    Socket sock(fd, Type::TCP);  // Assume TCP for accepted connections
    sock.state_ = State::CONNECTED;
    sock.peer_address_ = peer_addr;
    return sock;
}

Result<void> Socket::bind(const SocketAddress& address) {
    if (!is_valid()) {
        return std::unexpected("Invalid socket");
    }
    
    int result = ::bind(fd_, address.sockaddr_ptr(), address.sockaddr_len());
    if (result < 0) {
        return std::unexpected("Bind failed: " + std::string(strerror(errno)));
    }
    
    return {};
}

Result<void> Socket::listen(int backlog) {
    if (!is_valid()) {
        return std::unexpected("Invalid socket");
    }
    
    int result = ::listen(fd_, backlog);
    if (result < 0) {
        return std::unexpected("Listen failed: " + std::string(strerror(errno)));
    }
    
    state_ = State::LISTENING;
    return {};
}

Result<Socket> Socket::accept() {
    if (!is_valid() || state_ != State::LISTENING) {
        return std::unexpected("Socket not in listening state");
    }
    
    sockaddr_in client_addr{};
    socklen_t addr_len = sizeof(client_addr);
    
    int client_fd = ::accept(fd_, reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
    if (client_fd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return std::unexpected("No pending connections (would block)");
        }
        return std::unexpected("Accept failed: " + std::string(strerror(errno)));
    }
    
    SocketAddress peer_addr(client_addr);
    return Socket::from_fd(client_fd, peer_addr);
}

Result<void> Socket::connect(const SocketAddress& address) {
    if (!is_valid()) {
        return std::unexpected("Invalid socket");
    }
    
    state_ = State::CONNECTING;
    
    int result = ::connect(fd_, address.sockaddr_ptr(), address.sockaddr_len());
    if (result < 0) {
        if (errno == EINPROGRESS) {
            // Non-blocking connect in progress
            return {};
        }
        state_ = State::ERROR;
        return std::unexpected("Connect failed: " + std::string(strerror(errno)));
    }
    
    state_ = State::CONNECTED;
    peer_address_ = address;
    return {};
}

Result<size_t> Socket::send(const void* data, size_t length, int flags) {
    if (!is_valid()) {
        return std::unexpected("Invalid socket");
    }
    
    ssize_t result = ::send(fd_, data, length, flags | MSG_NOSIGNAL);
    if (result < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;  // Would block, return 0 bytes sent
        }
        return std::unexpected("Send failed: " + std::string(strerror(errno)));
    }
    
    return static_cast<size_t>(result);
}

Result<size_t> Socket::recv(void* buffer, size_t length, int flags) {
    if (!is_valid()) {
        return std::unexpected("Invalid socket");
    }
    
    ssize_t result = ::recv(fd_, buffer, length, flags);
    if (result < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;  // Would block, return 0 bytes received
        }
        return std::unexpected("Receive failed: " + std::string(strerror(errno)));
    }
    
    if (result == 0) {
        // Connection closed by peer
        state_ = State::CLOSED;
    }
    
    return static_cast<size_t>(result);
}

Result<size_t> Socket::sendv(const struct iovec* iov, int iovcnt) {
    if (!is_valid()) {
        return std::unexpected("Invalid socket");
    }
    
    struct msghdr msg{};
    msg.msg_iov = const_cast<struct iovec*>(iov);
    msg.msg_iovlen = iovcnt;
    
    ssize_t result = sendmsg(fd_, &msg, MSG_NOSIGNAL);
    if (result < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        return std::unexpected("Sendv failed: " + std::string(strerror(errno)));
    }
    
    return static_cast<size_t>(result);
}

Result<size_t> Socket::recvv(const struct iovec* iov, int iovcnt) {
    if (!is_valid()) {
        return std::unexpected("Invalid socket");
    }
    
    struct msghdr msg{};
    msg.msg_iov = const_cast<struct iovec*>(iov);
    msg.msg_iovlen = iovcnt;
    
    ssize_t result = recvmsg(fd_, &msg, 0);
    if (result < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        return std::unexpected("Recvv failed: " + std::string(strerror(errno)));
    }
    
    if (result == 0) {
        state_ = State::CLOSED;
    }
    
    return static_cast<size_t>(result);
}

Result<SocketAddress> Socket::local_address() const {
    if (!is_valid()) {
        return std::unexpected("Invalid socket");
    }
    
    sockaddr_in addr{};
    socklen_t addr_len = sizeof(addr);
    
    if (getsockname(fd_, reinterpret_cast<sockaddr*>(&addr), &addr_len) < 0) {
        return std::unexpected("getsockname failed: " + std::string(strerror(errno)));
    }
    
    return SocketAddress(addr);
}

Result<SocketAddress> Socket::peer_address() const {
    if (!is_valid()) {
        return std::unexpected("Invalid socket");
    }
    
    sockaddr_in addr{};
    socklen_t addr_len = sizeof(addr);
    
    if (getpeername(fd_, reinterpret_cast<sockaddr*>(&addr), &addr_len) < 0) {
        return std::unexpected("getpeername failed: " + std::string(strerror(errno)));
    }
    
    return SocketAddress(addr);
}

Result<void> Socket::set_option(int level, int optname, const void* optval, socklen_t optlen) {
    if (!is_valid()) {
        return std::unexpected("Invalid socket");
    }
    
    if (setsockopt(fd_, level, optname, optval, optlen) < 0) {
        return std::unexpected("setsockopt failed: " + std::string(strerror(errno)));
    }
    
    return {};
}

Result<void> Socket::get_option(int level, int optname, void* optval, socklen_t* optlen) {
    if (!is_valid()) {
        return std::unexpected("Invalid socket");
    }
    
    if (getsockopt(fd_, level, optname, optval, optlen) < 0) {
        return std::unexpected("getsockopt failed: " + std::string(strerror(errno)));
    }
    
    return {};
}

Result<void> Socket::set_non_blocking(bool non_blocking) {
    if (!is_valid()) {
        return std::unexpected("Invalid socket");
    }
    
    int flags = fcntl(fd_, F_GETFL, 0);
    if (flags < 0) {
        return std::unexpected("fcntl F_GETFL failed: " + std::string(strerror(errno)));
    }
    
    if (non_blocking) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    
    if (fcntl(fd_, F_SETFL, flags) < 0) {
        return std::unexpected("fcntl F_SETFL failed: " + std::string(strerror(errno)));
    }
    
    return {};
}

Result<void> Socket::set_nodelay(bool nodelay) {
    int opt = nodelay ? 1 : 0;
    return set_option(IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
}

Result<void> Socket::set_reuseaddr(bool reuse) {
    int opt = reuse ? 1 : 0;
    return set_option(SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

Result<void> Socket::set_keepalive(bool keepalive) {
    int opt = keepalive ? 1 : 0;
    return set_option(SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
}

void Socket::close() {
    if (is_valid()) {
        ::close(fd_);
        fd_ = -1;
        state_ = State::CLOSED;
    }
}

Result<void> Socket::shutdown(int how) {
    if (!is_valid()) {
        return std::unexpected("Invalid socket");
    }
    
    if (::shutdown(fd_, how) < 0) {
        return std::unexpected("Shutdown failed: " + std::string(strerror(errno)));
    }
    
    return {};
}

int Socket::last_error() const {
    return errno;
}

std::string Socket::error_string() const {
    return std::string(strerror(errno));
}

Result<void> Socket::apply_options(const SocketOptions& options) {
    // Set reuse address
    if (options.reuse_address) {
        if (auto result = set_reuseaddr(true); !result) {
            return std::unexpected("Failed to set SO_REUSEADDR: " + result.error());
        }
    }
    
    // Set TCP_NODELAY for TCP sockets
    if (type_ == Type::TCP && options.nodelay) {
        if (auto result = set_nodelay(true); !result) {
            return std::unexpected("Failed to set TCP_NODELAY: " + result.error());
        }
    }
    
    // Set keepalive
    if (options.keepalive) {
        if (auto result = set_keepalive(true); !result) {
            return std::unexpected("Failed to set SO_KEEPALIVE: " + result.error());
        }
    }
    
    // Set receive buffer size
    if (options.recv_buffer_size) {
        int size = *options.recv_buffer_size;
        if (auto result = set_option(SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)); !result) {
            return std::unexpected("Failed to set SO_RCVBUF: " + result.error());
        }
    }
    
    // Set send buffer size
    if (options.send_buffer_size) {
        int size = *options.send_buffer_size;
        if (auto result = set_option(SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)); !result) {
            return std::unexpected("Failed to set SO_SNDBUF: " + result.error());
        }
    }
    
    // Set receive timeout
    if (options.recv_timeout) {
        struct timeval tv;
        auto timeout_ms = options.recv_timeout->count();
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        if (auto result = set_option(SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)); !result) {
            return std::unexpected("Failed to set SO_RCVTIMEO: " + result.error());
        }
    }
    
    // Set send timeout
    if (options.send_timeout) {
        struct timeval tv;
        auto timeout_ms = options.send_timeout->count();
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        if (auto result = set_option(SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)); !result) {
            return std::unexpected("Failed to set SO_SNDTIMEO: " + result.error());
        }
    }
    
    // Set non-blocking mode
    if (options.non_blocking) {
        if (auto result = set_non_blocking(true); !result) {
            return std::unexpected("Failed to set non-blocking mode: " + result.error());
        }
    }
    
    return {};
}

// AsyncSocket implementation
Result<bool> AsyncSocket::connect_async(const SocketAddress& address, 
                                      std::chrono::milliseconds timeout) {
    // Ensure socket is non-blocking
    if (auto result = socket_.set_non_blocking(true); !result) {
        return std::unexpected(result.error());
    }
    
    auto connect_result = socket_.connect(address);
    if (connect_result) {
        // Connected immediately
        return true;
    }
    
    // Check if connection is in progress
    if (connect_result.error().find("in progress") != std::string::npos) {
        // Would need to implement select/poll/epoll here for timeout
        // For now, return false indicating would block
        return false;
    }
    
    return std::unexpected(connect_result.error());
}

Result<size_t> AsyncSocket::send_async(const void* data, size_t length) {
    return socket_.send(data, length);
}

Result<size_t> AsyncSocket::recv_async(void* buffer, size_t length) {
    return socket_.recv(buffer, length);
}

// SocketFactory implementation
SocketOptions SocketFactory::merge_options(const SocketOptions& override_options) {
    SocketOptions merged = default_options_;
    
    // Override with specific options
    if (override_options.reuse_address != default_options_.reuse_address) {
        merged.reuse_address = override_options.reuse_address;
    }
    if (override_options.nodelay != default_options_.nodelay) {
        merged.nodelay = override_options.nodelay;
    }
    if (override_options.keepalive != default_options_.keepalive) {
        merged.keepalive = override_options.keepalive;
    }
    if (override_options.recv_buffer_size) {
        merged.recv_buffer_size = override_options.recv_buffer_size;
    }
    if (override_options.send_buffer_size) {
        merged.send_buffer_size = override_options.send_buffer_size;
    }
    if (override_options.recv_timeout) {
        merged.recv_timeout = override_options.recv_timeout;
    }
    if (override_options.send_timeout) {
        merged.send_timeout = override_options.send_timeout;
    }
    if (override_options.non_blocking != default_options_.non_blocking) {
        merged.non_blocking = override_options.non_blocking;
    }
    
    return merged;
}

Result<Socket> SocketFactory::create_tcp_socket(const SocketOptions& options) {
    auto merged_options = merge_options(options);
    return Socket::create(Socket::Type::TCP, merged_options);
}

Result<Socket> SocketFactory::create_udp_socket(const SocketOptions& options) {
    auto merged_options = merge_options(options);
    return Socket::create(Socket::Type::UDP, merged_options);
}

Result<Socket> SocketFactory::create_listener(const SocketAddress& bind_addr,
                                            int backlog,
                                            const SocketOptions& options) {
    auto socket_result = create_tcp_socket(options);
    if (!socket_result) {
        return std::unexpected(socket_result.error());
    }
    
    auto socket = std::move(*socket_result);
    
    if (auto result = socket.bind(bind_addr); !result) {
        return std::unexpected("Bind failed: " + result.error());
    }
    
    if (auto result = socket.listen(backlog); !result) {
        return std::unexpected("Listen failed: " + result.error());
    }
    
    return socket;
}

Result<Socket> SocketFactory::create_connection(const SocketAddress& connect_addr,
                                              const SocketOptions& options) {
    auto socket_result = create_tcp_socket(options);
    if (!socket_result) {
        return std::unexpected(socket_result.error());
    }
    
    auto socket = std::move(*socket_result);
    
    if (auto result = socket.connect(connect_addr); !result) {
        return std::unexpected("Connect failed: " + result.error());
    }
    
    return socket;
}

// SocketStats implementation
double SocketStats::avg_send_latency() const {
    if (send_calls == 0) return 0.0;
    return static_cast<double>(total_send_time.count()) / send_calls;
}

double SocketStats::avg_recv_latency() const {
    if (recv_calls == 0) return 0.0;
    return static_cast<double>(total_recv_time.count()) / recv_calls;
}

double SocketStats::send_throughput_mbps() const {
    auto duration = std::chrono::steady_clock::now() - created_at;
    if (duration.count() == 0) return 0.0;
    
    double seconds = std::chrono::duration<double>(duration).count();
    double megabits = static_cast<double>(bytes_sent * 8) / (1024 * 1024);
    return megabits / seconds;
}

double SocketStats::recv_throughput_mbps() const {
    auto duration = std::chrono::steady_clock::now() - created_at;
    if (duration.count() == 0) return 0.0;
    
    double seconds = std::chrono::duration<double>(duration).count();
    double megabits = static_cast<double>(bytes_received * 8) / (1024 * 1024);
    return megabits / seconds;
}

// InstrumentedSocket implementation
Result<size_t> InstrumentedSocket::send(const void* data, size_t length, int flags) {
    auto start = std::chrono::steady_clock::now();
    auto result = socket_.send(data, length, flags);
    auto end = std::chrono::steady_clock::now();
    
    stats_.send_calls++;
    stats_.total_send_time += (end - start);
    
    if (result) {
        stats_.bytes_sent += *result;
    } else {
        stats_.errors++;
    }
    
    return result;
}

Result<size_t> InstrumentedSocket::recv(void* buffer, size_t length, int flags) {
    auto start = std::chrono::steady_clock::now();
    auto result = socket_.recv(buffer, length, flags);
    auto end = std::chrono::steady_clock::now();
    
    stats_.recv_calls++;
    stats_.total_recv_time += (end - start);
    
    if (result) {
        stats_.bytes_received += *result;
    } else {
        stats_.errors++;
    }
    
    return result;
}

} // namespace networking
