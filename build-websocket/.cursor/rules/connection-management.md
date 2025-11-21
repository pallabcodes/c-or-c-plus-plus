# Connection Management Standards

## Overview
Connection management is critical for production grade WebSocket servers handling millions of concurrent connections. This document defines standards for managing WebSocket connections including lifecycle, timeouts, and graceful shutdown.

## Connection Lifecycle

### Lifecycle States
* **Connecting**: Initial connection state
* **Handshaking**: HTTP upgrade in progress
* **Authenticating**: Authentication in progress
* **Open**: Connection established and ready
* **Closing**: Connection closing
* **Closed**: Connection closed
* **Rationale**: Lifecycle states enable state management

### State Transitions
* **Connecting -> Handshaking**: HTTP upgrade initiated
* **Handshaking -> Authenticating**: Handshake successful
* **Authenticating -> Open**: Authentication successful
* **Open -> Closing**: Close initiated
* **Closing -> Closed**: Close complete
* **Rationale**: Transitions enable lifecycle management

### Example Connection Manager
```cpp
class WebSocketConnection {
public:
    enum class State {
        CONNECTING,
        HANDSHAKING,
        AUTHENTICATING,
        OPEN,
        CLOSING,
        CLOSED
    };
    
    State get_state() const { return state_; }
    
    void set_state(State new_state) {
        std::lock_guard<std::mutex> lock(mutex_);
        state_ = new_state;
    }
    
    bool is_open() const {
        return state_ == State::OPEN;
    }
    
private:
    State state_ = State::CONNECTING;
    mutable std::mutex mutex_;
};
```

## Timeout Management

### Handshake Timeout
* **Definition**: Timeout for handshake completion
* **Default**: 10 seconds
* **Implementation**: Timer for handshake timeout
* **Rationale**: Handshake timeout prevents hanging connections

### Idle Timeout
* **Definition**: Timeout for idle connections
* **Default**: 60 seconds
* **Implementation**: Timer reset on activity
* **Rationale**: Idle timeout frees unused resources

### Ping Timeout
* **Definition**: Timeout for ping response
* **Default**: 30 seconds
* **Implementation**: Timer for ping response
* **Rationale**: Ping timeout detects dead connections

### Example Timeout Management
```cpp
class ConnectionTimeoutManager {
public:
    void start_handshake_timeout(WebSocketConnection& conn) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto timer = std::make_shared<Timer>(HANDSHAKE_TIMEOUT);
        timer->set_callback([&conn]() {
            conn.close(CloseCode::PROTOCOL_ERROR, "Handshake timeout");
        });
        timers_[conn.id()] = timer;
        timer->start();
    }
    
    void reset_idle_timer(WebSocketConnection& conn) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = timers_.find(conn.id());
        if (it != timers_.end()) {
            it->second->reset(IDLE_TIMEOUT);
        }
    }
    
private:
    std::unordered_map<ConnectionId, std::shared_ptr<Timer>> timers_;
    std::mutex mutex_;
};
```

## Heartbeat and Keepalive

### Application Level Ping
* **Definition**: Send ping frames periodically
* **Interval**: Configurable ping interval
* **Purpose**: Keep connection alive, detect dead connections
* **Rationale**: Application level ping enables connection health checking

### TCP Keepalive
* **Definition**: TCP keepalive mechanism
* **Configuration**: SO_KEEPALIVE socket option
* **Purpose**: Detect dead TCP connections
* **Rationale**: TCP keepalive complements application ping

### Example Heartbeat
```cpp
class HeartbeatManager {
public:
    void start_heartbeat(WebSocketConnection& conn) {
        auto timer = std::make_shared<Timer>(PING_INTERVAL);
        timer->set_callback([&conn, this]() {
            send_ping(conn);
            start_pong_timeout(conn);
        });
        timer->start();
        heartbeats_[conn.id()] = timer;
    }
    
    void handle_pong(WebSocketConnection& conn) {
        cancel_pong_timeout(conn);
    }
    
private:
    void send_ping(WebSocketConnection& conn) {
        WebSocketFrame ping;
        ping.opcode = Opcode::PING;
        ping.payload = generate_ping_payload();
        conn.send_frame(ping);
    }
    
    void start_pong_timeout(WebSocketConnection& conn) {
        auto timer = std::make_shared<Timer>(PONG_TIMEOUT);
        timer->set_callback([&conn]() {
            conn.close(CloseCode::ABNORMAL_CLOSURE, "Pong timeout");
        });
        timer->start();
        pong_timeouts_[conn.id()] = timer;
    }
    
    std::unordered_map<ConnectionId, std::shared_ptr<Timer>> heartbeats_;
    std::unordered_map<ConnectionId, std::shared_ptr<Timer>> pong_timeouts_;
};
```

## Graceful Shutdown

### Drain Phase
* **Definition**: Stop accepting new connections
* **Implementation**: Set drain flag, reject new upgrades
* **Rationale**: Drain phase prevents new connections during shutdown

### Close Phase
* **Definition**: Close existing connections gracefully
* **Implementation**: Send close frame, wait for peer close
* **Timeout**: Force close after timeout
* **Rationale**: Close phase enables graceful connection closure

### Example Graceful Shutdown
```cpp
class WebSocketServer {
public:
    void shutdown() {
        // Enter drain phase
        drain_ = true;
        
        // Stop accepting new connections
        acceptor_.close();
        
        // Close existing connections
        std::lock_guard<std::mutex> lock(connections_mutex_);
        for (auto& [id, conn] : connections_) {
            conn->close(CloseCode::GOING_AWAY, "Server shutdown");
        }
        
        // Wait for connections to close
        auto deadline = std::chrono::steady_clock::now() + SHUTDOWN_TIMEOUT;
        while (!connections_.empty() && 
               std::chrono::steady_clock::now() < deadline) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Force close remaining connections
        for (auto& [id, conn] : connections_) {
            conn->force_close();
        }
    }
    
private:
    bool drain_ = false;
    std::unordered_map<ConnectionId, std::shared_ptr<WebSocketConnection>> connections_;
    std::mutex connections_mutex_;
};
```

## Connection Limits

### Per IP Limits
* **Definition**: Limit connections per IP address
* **Purpose**: Prevent abuse
* **Implementation**: Track connections per IP
* **Rationale**: Per IP limits prevent abuse

### Global Limits
* **Definition**: Limit total connections
* **Purpose**: Prevent resource exhaustion
* **Implementation**: Track total connection count
* **Rationale**: Global limits prevent resource exhaustion

## Implementation Standards

### Correctness
* **State management**: Proper state management
* **Timeout handling**: Proper timeout handling
* **Graceful shutdown**: Proper graceful shutdown
* **Rationale**: Correctness is critical

### Performance
* **Efficient state tracking**: Optimize state tracking
* **Timer efficiency**: Efficient timer management
* **Rationale**: Performance is critical

## Testing Requirements

### Unit Tests
* **Lifecycle tests**: Test connection lifecycle
* **Timeout tests**: Test timeout handling
* **Heartbeat tests**: Test heartbeat mechanism
* **Shutdown tests**: Test graceful shutdown
* **Rationale**: Comprehensive testing ensures correctness

## Research Papers and References

### Connection Management
* "High Performance Browser Networking" - Connection management
* Connection management research papers
* Network optimization guides

## Implementation Checklist

- [ ] Understand connection lifecycle
- [ ] Implement timeout management
- [ ] Implement heartbeat mechanism
- [ ] Implement graceful shutdown
- [ ] Implement connection limits
- [ ] Write comprehensive unit tests
- [ ] Document connection management

