# The Complete Anatomy of WebSocket Connections

## 🚀 Introduction

WebSockets provide **full-duplex communication** over a single TCP connection, enabling real-time applications like chat systems, live trading platforms, and multiplayer games. This document breaks down every byte of the WebSocket protocol - from handshake to frame parsing.

## 📡 WebSocket vs HTTP: The Fundamental Difference

```
HTTP Request-Response (Half-Duplex):
Client ──request──> Server
Client <─response── Server
[Connection closed or kept alive for next request]

WebSocket (Full-Duplex):
Client ←──messages──→ Server
Client ←──messages──→ Server
[Persistent bidirectional connection]
```

## 🔄 Phase 1: WebSocket Handshake (HTTP Upgrade)

### Client Initiates Handshake

```cpp
// WebSocket handshake request builder
class WebSocketHandshakeBuilder {
public:
    std::string build_request(const std::string& host, 
                             const std::string& path,
                             const std::string& origin = "") {
        // 1. Generate random WebSocket key (16 bytes, base64 encoded)
        std::string ws_key = generate_websocket_key();
        
        std::ostringstream request;
        request << "GET " << path << " HTTP/1.1\r\n"
                << "Host: " << host << "\r\n"
                << "Upgrade: websocket\r\n"              // Protocol upgrade
                << "Connection: Upgrade\r\n"            // Connection upgrade
                << "Sec-WebSocket-Key: " << ws_key << "\r\n"
                << "Sec-WebSocket-Version: 13\r\n";     // RFC 6455
        
        if (!origin.empty()) {
            request << "Origin: " << origin << "\r\n";
        }
        
        request << "\r\n";  // End of headers
        
        // Store key for validation
        expected_accept_key_ = compute_accept_key(ws_key);
        
        return request.str();
    }

private:
    std::string generate_websocket_key() {
        // Generate 16 random bytes
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint8_t> dis(0, 255);
        
        std::vector<uint8_t> key_bytes(16);
        for (auto& byte : key_bytes) {
            byte = dis(gen);
        }
        
        return base64_encode(key_bytes);
    }
    
    std::string compute_accept_key(const std::string& ws_key) {
        // RFC 6455: Concatenate key with magic string and SHA1 hash
        std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        std::string combined = ws_key + magic;
        
        // SHA1 hash
        unsigned char hash[SHA_DIGEST_LENGTH];
        SHA1(reinterpret_cast<const unsigned char*>(combined.c_str()),
             combined.length(), hash);
        
        // Base64 encode the hash
        return base64_encode(std::vector<uint8_t>(hash, hash + SHA_DIGEST_LENGTH));
    }
};
```

**Example WebSocket Handshake Request:**
```http
GET /chat HTTP/1.1
Host: example.com
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
Sec-WebSocket-Version: 13
Origin: https://example.com

```

### Server Handshake Validation and Response

```cpp
// Server-side handshake handler
class WebSocketHandshakeHandler {
public:
    HandshakeResult validate_and_respond(const HTTPRequest& request) {
        // 1. Validate required headers
        if (request.header("Upgrade") != "websocket") {
            return HandshakeResult::invalid("Missing Upgrade: websocket");
        }
        
        if (request.header("Connection").find("Upgrade") == std::string::npos) {
            return HandshakeResult::invalid("Missing Connection: Upgrade");
        }
        
        auto ws_version = request.header("Sec-WebSocket-Version");
        if (ws_version != "13") {
            return HandshakeResult::invalid("Unsupported WebSocket version");
        }
        
        // 2. Extract and validate WebSocket key
        auto ws_key = request.header("Sec-WebSocket-Key");
        if (ws_key.empty() || !is_valid_base64(ws_key)) {
            return HandshakeResult::invalid("Invalid Sec-WebSocket-Key");
        }
        
        // 3. Compute accept key
        std::string accept_key = compute_accept_key(ws_key);
        
        // 4. Build handshake response
        std::ostringstream response;
        response << "HTTP/1.1 101 Switching Protocols\r\n"
                << "Upgrade: websocket\r\n"
                << "Connection: Upgrade\r\n"
                << "Sec-WebSocket-Accept: " << accept_key << "\r\n"
                << "\r\n";
        
        return HandshakeResult::success(response.str());
    }
};
```

**Server Handshake Response:**
```http
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=

```

**Critical Security Note:** The `Sec-WebSocket-Accept` key prevents cross-protocol attacks by ensuring the server understands WebSocket protocol.

## 📦 Phase 2: WebSocket Frame Format

### Frame Structure (RFC 6455)

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-------+-+-------------+-------------------------------+
|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
|N|V|V|V|       |S|             |   (if payload len==126/127)   |
| |1|2|3|       |K|             |                               |
+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
|     Extended payload length continued, if payload len == 127  |
+ - - - - - - - - - - - - - - - +-------------------------------+
|                               |Masking-key, if MASK set to 1  |
+-------------------------------+-------------------------------+
| Masking-key (continued)       |          Payload Data         |
+-------------------------------- - - - - - - - - - - - - - - - +
:                     Payload Data continued ...                :
+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
|                     Payload Data continued ...                |
+---------------------------------------------------------------+
```

### Frame Implementation

```cpp
// WebSocket frame structure
struct WebSocketFrame {
    // Control bits
    bool fin;           // Final fragment
    bool rsv1, rsv2, rsv3; // Reserved bits (must be 0)
    
    // Opcode (4 bits)
    enum class Opcode : uint8_t {
        CONTINUATION = 0x0,
        TEXT = 0x1,
        BINARY = 0x2,
        // 0x3-0x7 reserved for future non-control frames
        CLOSE = 0x8,
        PING = 0x9,
        PONG = 0xA
        // 0xB-0xF reserved for future control frames
    } opcode;
    
    // Masking
    bool masked;
    std::array<uint8_t, 4> masking_key;
    
    // Payload
    std::vector<uint8_t> payload;
};

// Frame parser with proper error handling
class WebSocketFrameParser {
public:
    ParseResult parse(const uint8_t* data, size_t length) {
        if (length < 2) {
            return ParseResult::need_more_data();
        }
        
        WebSocketFrame frame;
        size_t offset = 0;
        
        // 1. Parse first byte (FIN, RSV, opcode)
        uint8_t first_byte = data[offset++];
        frame.fin = (first_byte & 0x80) != 0;
        frame.rsv1 = (first_byte & 0x40) != 0;
        frame.rsv2 = (first_byte & 0x20) != 0;
        frame.rsv3 = (first_byte & 0x10) != 0;
        frame.opcode = static_cast<WebSocketFrame::Opcode>(first_byte & 0x0F);
        
        // 2. Validate reserved bits (must be 0 unless extensions are negotiated)
        if (frame.rsv1 || frame.rsv2 || frame.rsv3) {
            return ParseResult::protocol_error("Reserved bits must be 0");
        }
        
        // 3. Parse second byte (MASK, payload length)
        uint8_t second_byte = data[offset++];
        frame.masked = (second_byte & 0x80) != 0;
        uint8_t payload_len = second_byte & 0x7F;
        
        // 4. Parse extended payload length
        uint64_t actual_payload_length = payload_len;
        
        if (payload_len == 126) {
            // 16-bit extended length
            if (length < offset + 2) return ParseResult::need_more_data();
            actual_payload_length = (data[offset] << 8) | data[offset + 1];
            offset += 2;
        } else if (payload_len == 127) {
            // 64-bit extended length
            if (length < offset + 8) return ParseResult::need_more_data();
            actual_payload_length = 0;
            for (int i = 0; i < 8; i++) {
                actual_payload_length = (actual_payload_length << 8) | data[offset + i];
            }
            offset += 8;
            
            // Check for valid payload length (must not have most significant bit set)
            if (actual_payload_length & 0x8000000000000000ULL) {
                return ParseResult::protocol_error("Invalid payload length");
            }
        }
        
        // 5. Parse masking key (if present)
        if (frame.masked) {
            if (length < offset + 4) return ParseResult::need_more_data();
            std::copy(data + offset, data + offset + 4, frame.masking_key.begin());
            offset += 4;
        }
        
        // 6. Parse payload
        if (length < offset + actual_payload_length) {
            return ParseResult::need_more_data();
        }
        
        frame.payload.resize(actual_payload_length);
        std::copy(data + offset, data + offset + actual_payload_length, 
                 frame.payload.begin());
        
        // 7. Unmask payload if necessary
        if (frame.masked) {
            unmask_payload(frame.payload, frame.masking_key);
        }
        
        return ParseResult::success(std::move(frame), offset + actual_payload_length);
    }

private:
    void unmask_payload(std::vector<uint8_t>& payload, 
                       const std::array<uint8_t, 4>& masking_key) {
        for (size_t i = 0; i < payload.size(); i++) {
            payload[i] ^= masking_key[i % 4];
        }
    }
};
```

## 🔐 Phase 3: Masking (Client-to-Server Security)

### Why Masking Exists

Masking prevents **cache poisoning attacks** where malicious JavaScript could construct frames that look like valid HTTP requests to proxy servers.

```cpp
// Frame masking implementation
class FrameMasker {
public:
    static std::array<uint8_t, 4> generate_masking_key() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint8_t> dis(0, 255);
        
        std::array<uint8_t, 4> key;
        for (auto& byte : key) {
            byte = dis(gen);
        }
        return key;
    }
    
    static void mask_payload(std::vector<uint8_t>& payload,
                           const std::array<uint8_t, 4>& masking_key) {
        for (size_t i = 0; i < payload.size(); i++) {
            payload[i] ^= masking_key[i % 4];
        }
    }
};

// Client must mask all frames sent to server
WebSocketFrame create_text_frame(const std::string& text) {
    WebSocketFrame frame;
    frame.fin = true;
    frame.opcode = WebSocketFrame::Opcode::TEXT;
    frame.masked = true;  // Client frames MUST be masked
    frame.masking_key = FrameMasker::generate_masking_key();
    
    frame.payload.assign(text.begin(), text.end());
    FrameMasker::mask_payload(frame.payload, frame.masking_key);
    
    return frame;
}
```

**Security Rule:** 
- **Client frames → Server**: MUST be masked
- **Server frames → Client**: MUST NOT be masked

## 📤 Phase 4: Frame Serialization

```cpp
// Frame serializer for sending
class WebSocketFrameSerializer {
public:
    std::vector<uint8_t> serialize(const WebSocketFrame& frame) {
        std::vector<uint8_t> buffer;
        
        // 1. First byte: FIN + RSV + Opcode
        uint8_t first_byte = 0;
        if (frame.fin) first_byte |= 0x80;
        if (frame.rsv1) first_byte |= 0x40;
        if (frame.rsv2) first_byte |= 0x20;
        if (frame.rsv3) first_byte |= 0x10;
        first_byte |= static_cast<uint8_t>(frame.opcode) & 0x0F;
        buffer.push_back(first_byte);
        
        // 2. Second byte: MASK + Payload length
        uint8_t second_byte = 0;
        if (frame.masked) second_byte |= 0x80;
        
        uint64_t payload_length = frame.payload.size();
        
        if (payload_length < 126) {
            // 7-bit length
            second_byte |= static_cast<uint8_t>(payload_length);
            buffer.push_back(second_byte);
        } else if (payload_length <= 0xFFFF) {
            // 16-bit extended length
            second_byte |= 126;
            buffer.push_back(second_byte);
            buffer.push_back((payload_length >> 8) & 0xFF);
            buffer.push_back(payload_length & 0xFF);
        } else {
            // 64-bit extended length
            second_byte |= 127;
            buffer.push_back(second_byte);
            for (int i = 7; i >= 0; i--) {
                buffer.push_back((payload_length >> (i * 8)) & 0xFF);
            }
        }
        
        // 3. Masking key (if present)
        if (frame.masked) {
            buffer.insert(buffer.end(), 
                         frame.masking_key.begin(), 
                         frame.masking_key.end());
        }
        
        // 4. Payload data
        buffer.insert(buffer.end(), 
                     frame.payload.begin(), 
                     frame.payload.end());
        
        return buffer;
    }
};
```

## 🎮 Phase 5: Control Frames

### Ping/Pong Mechanism

```cpp
// Keep-alive and latency measurement
class WebSocketPingPong {
    std::chrono::steady_clock::time_point last_ping_;
    std::chrono::milliseconds ping_interval_{30000}; // 30 seconds
    
public:
    bool should_send_ping() const {
        auto now = std::chrono::steady_clock::now();
        return (now - last_ping_) >= ping_interval_;
    }
    
    WebSocketFrame create_ping_frame(const std::string& payload = "") {
        WebSocketFrame frame;
        frame.fin = true;
        frame.opcode = WebSocketFrame::Opcode::PING;
        frame.payload.assign(payload.begin(), payload.end());
        
        last_ping_ = std::chrono::steady_clock::now();
        return frame;
    }
    
    WebSocketFrame create_pong_frame(const WebSocketFrame& ping_frame) {
        WebSocketFrame pong_frame;
        pong_frame.fin = true;
        pong_frame.opcode = WebSocketFrame::Opcode::PONG;
        pong_frame.payload = ping_frame.payload; // Echo ping payload
        
        return pong_frame;
    }
};
```

### Connection Close Handshake

```cpp
// Graceful connection termination
class WebSocketCloseHandler {
public:
    enum class CloseCode : uint16_t {
        NORMAL_CLOSURE = 1000,
        GOING_AWAY = 1001,
        PROTOCOL_ERROR = 1002,
        UNSUPPORTED_DATA = 1003,
        NO_STATUS_RECEIVED = 1005,
        ABNORMAL_CLOSURE = 1006,
        INVALID_FRAME_PAYLOAD_DATA = 1007,
        POLICY_VIOLATION = 1008,
        MESSAGE_TOO_BIG = 1009,
        MISSING_EXTENSION = 1010,
        INTERNAL_ERROR = 1011,
        SERVICE_RESTART = 1012,
        TRY_AGAIN_LATER = 1013,
        TLS_HANDSHAKE = 1015
    };
    
    WebSocketFrame create_close_frame(CloseCode code, 
                                     const std::string& reason = "") {
        WebSocketFrame frame;
        frame.fin = true;
        frame.opcode = WebSocketFrame::Opcode::CLOSE;
        
        // Payload: 2-byte code + optional reason
        frame.payload.resize(2 + reason.length());
        frame.payload[0] = (static_cast<uint16_t>(code) >> 8) & 0xFF;
        frame.payload[1] = static_cast<uint16_t>(code) & 0xFF;
        
        if (!reason.empty()) {
            std::copy(reason.begin(), reason.end(), 
                     frame.payload.begin() + 2);
        }
        
        return frame;
    }
    
    std::pair<CloseCode, std::string> parse_close_frame(const WebSocketFrame& frame) {
        if (frame.payload.size() < 2) {
            return {CloseCode::NO_STATUS_RECEIVED, ""};
        }
        
        uint16_t code = (frame.payload[0] << 8) | frame.payload[1];
        std::string reason;
        
        if (frame.payload.size() > 2) {
            reason.assign(frame.payload.begin() + 2, frame.payload.end());
        }
        
        return {static_cast<CloseCode>(code), reason};
    }
};
```

## 🌊 Phase 6: Message Fragmentation

Large messages can be split across multiple frames:

```cpp
// Message fragmentation for large payloads
class WebSocketFragmentHandler {
    static constexpr size_t MAX_FRAME_SIZE = 65536; // 64KB
    
public:
    std::vector<WebSocketFrame> fragment_message(const std::string& message,
                                               WebSocketFrame::Opcode opcode) {
        std::vector<WebSocketFrame> frames;
        size_t offset = 0;
        
        while (offset < message.length()) {
            WebSocketFrame frame;
            
            // First frame: use provided opcode
            // Subsequent frames: use CONTINUATION opcode
            frame.opcode = (offset == 0) ? opcode : WebSocketFrame::Opcode::CONTINUATION;
            
            // Determine payload size for this frame
            size_t remaining = message.length() - offset;
            size_t frame_size = std::min(remaining, MAX_FRAME_SIZE);
            
            // Set FIN bit only on last frame
            frame.fin = (offset + frame_size == message.length());
            
            // Copy payload data
            frame.payload.assign(message.begin() + offset, 
                               message.begin() + offset + frame_size);
            
            frames.push_back(std::move(frame));
            offset += frame_size;
        }
        
        return frames;
    }
    
    // Reassemble fragmented message
    class MessageReassembler {
        std::vector<uint8_t> buffer_;
        WebSocketFrame::Opcode original_opcode_;
        bool expecting_continuation_ = false;
        
    public:
        std::optional<std::vector<uint8_t>> add_frame(const WebSocketFrame& frame) {
            if (!expecting_continuation_) {
                // First frame of message
                if (frame.opcode == WebSocketFrame::Opcode::CONTINUATION) {
                    throw std::runtime_error("Unexpected continuation frame");
                }
                
                original_opcode_ = frame.opcode;
                buffer_.clear();
            } else {
                // Continuation frame
                if (frame.opcode != WebSocketFrame::Opcode::CONTINUATION) {
                    throw std::runtime_error("Expected continuation frame");
                }
            }
            
            // Append payload to buffer
            buffer_.insert(buffer_.end(), 
                          frame.payload.begin(), 
                          frame.payload.end());
            
            if (frame.fin) {
                // Message complete
                expecting_continuation_ = false;
                return std::vector<uint8_t>(buffer_);
            } else {
                // More fragments coming
                expecting_continuation_ = true;
                return std::nullopt;
            }
        }
    };
};
```

## ⚡ Phase 7: High-Performance WebSocket Server

```cpp
// Production-grade WebSocket server
class WebSocketServer {
    EventLoop event_loop_;
    ThreadPool worker_pool_;
    ConnectionManager connection_manager_;
    
public:
    class WebSocketConnection {
        int socket_fd_;
        ConnectionState state_;
        MessageReassembler reassembler_;
        std::queue<WebSocketFrame> send_queue_;
        
    public:
        void handle_frame(const WebSocketFrame& frame) {
            switch (frame.opcode) {
                case WebSocketFrame::Opcode::TEXT:
                case WebSocketFrame::Opcode::BINARY:
                case WebSocketFrame::Opcode::CONTINUATION: {
                    auto message = reassembler_.add_frame(frame);
                    if (message) {
                        // Complete message received
                        on_message_received(*message);
                    }
                    break;
                }
                
                case WebSocketFrame::Opcode::PING: {
                    // Respond with PONG
                    auto pong = create_pong_frame(frame);
                    send_frame(pong);
                    break;
                }
                
                case WebSocketFrame::Opcode::PONG: {
                    // Update connection liveness
                    last_pong_received_ = std::chrono::steady_clock::now();
                    break;
                }
                
                case WebSocketFrame::Opcode::CLOSE: {
                    // Initiate graceful close
                    handle_close_frame(frame);
                    break;
                }
            }
        }
        
        void send_text(const std::string& text) {
            auto frames = fragment_message(text, WebSocketFrame::Opcode::TEXT);
            for (auto& frame : frames) {
                send_frame(frame);
            }
        }
        
        void send_binary(const std::vector<uint8_t>& data) {
            auto frames = fragment_message(data, WebSocketFrame::Opcode::BINARY);
            for (auto& frame : frames) {
                send_frame(frame);
            }
        }
    };
};
```

## 🔒 Security Considerations

### Origin Validation
```cpp
// Prevent cross-site WebSocket hijacking
class OriginValidator {
    std::set<std::string> allowed_origins_;
    
public:
    bool is_valid_origin(const std::string& origin) const {
        if (allowed_origins_.empty()) {
            return true; // Allow all if none specified
        }
        
        return allowed_origins_.count(origin) > 0;
    }
};
```

### Rate Limiting
```cpp
// Prevent WebSocket abuse
class WebSocketRateLimiter {
    struct ConnectionLimits {
        size_t max_message_size = 1024 * 1024; // 1MB
        size_t max_messages_per_second = 100;
        size_t max_connections_per_ip = 10;
    };
    
    std::unordered_map<std::string, size_t> connections_per_ip_;
    std::unordered_map<int, MessageRateTracker> message_rates_;
};
```

## 🚀 Real-World Applications

### Chat Server Implementation
```cpp
// Broadcasting to multiple connections
class ChatServer : public WebSocketServer {
    std::unordered_set<WebSocketConnection*> connections_;
    std::mutex connections_mutex_;
    
public:
    void broadcast_message(const std::string& message) {
        std::lock_guard<std::mutex> lock(connections_mutex_);
        
        for (auto* conn : connections_) {
            conn->send_text(message);
        }
    }
    
    void on_connection_established(WebSocketConnection* conn) override {
        std::lock_guard<std::mutex> lock(connections_mutex_);
        connections_.insert(conn);
    }
    
    void on_connection_closed(WebSocketConnection* conn) override {
        std::lock_guard<std::mutex> lock(connections_mutex_);
        connections_.erase(conn);
    }
};
```

## 📊 Performance Metrics

```cpp
// WebSocket performance monitoring
class WebSocketMetrics {
    std::atomic<uint64_t> total_connections_{0};
    std::atomic<uint64_t> active_connections_{0};
    std::atomic<uint64_t> messages_sent_{0};
    std::atomic<uint64_t> messages_received_{0};
    std::atomic<uint64_t> bytes_sent_{0};
    std::atomic<uint64_t> bytes_received_{0};
    
    LatencyHistogram message_latency_;
    
public:
    void record_message_sent(size_t bytes, std::chrono::nanoseconds latency) {
        messages_sent_++;
        bytes_sent_ += bytes;
        message_latency_.record(latency);
    }
};
```

## 🎯 Key Differences: WebSocket vs HTTP

| Aspect | HTTP | WebSocket |
|--------|------|-----------|
| **Connection** | Request-Response | Persistent |
| **Communication** | Half-duplex | Full-duplex |
| **Overhead** | Headers per request | Minimal frame overhead |
| **Real-time** | Polling required | Native push support |
| **Complexity** | Simple | Complex state management |
| **Caching** | Extensive | Not applicable |

## 🔄 Connection Lifecycle Summary

```
1. [HTTP Handshake]    Client → Server (HTTP upgrade request)
2. [HTTP Response]     Server → Client (101 Switching Protocols)
3. [Frame Exchange]    Client ↔ Server (Binary frames)
4. [Keep-alive]        Ping/Pong frames maintain connection
5. [Close Handshake]   Either side initiates graceful close
6. [TCP Close]         Underlying TCP connection terminated
```

This deep understanding of WebSocket internals enables you to build production-grade real-time applications that can handle thousands of concurrent connections with minimal latency.

---

*Next: [Network Performance Optimization Guide](performance-guide.md)*
