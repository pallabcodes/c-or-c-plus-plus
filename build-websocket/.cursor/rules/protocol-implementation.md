# WebSocket Protocol Implementation Standards

## Overview
RFC 6455 WebSocket protocol implementation is critical for production grade WebSocket servers. This document defines standards for implementing production grade WebSocket protocol including handshake, frame parsing, and control frames.

## RFC 6455 Compliance

### Protocol Standard
* **RFC 6455**: WebSocket protocol standard
* **Compliance**: Strict RFC 6455 compliance required
* **Test suite**: Autobahn TestSuite compliance
* **Rationale**: Compliance ensures interoperability

### Handshake Implementation
* **HTTP Upgrade**: HTTP/1.1 upgrade request
* **Status 101**: Switching Protocols response
* **Key exchange**: Sec WebSocket Key/Accept exchange
* **Origin validation**: Origin header validation
* **Rationale**: Handshake establishes WebSocket connection

### Example Handshake
```cpp
class WebSocketHandshake {
public:
    Result<HandshakeResponse> handle_request(const HttpRequest& request) {
        // Validate upgrade header
        if (request.get_header("Upgrade") != "websocket") {
            return std::unexpected("Invalid upgrade header");
        }
        
        // Validate connection header
        auto connection = request.get_header("Connection");
        if (connection.find("Upgrade") == std::string::npos) {
            return std::unexpected("Invalid connection header");
        }
        
        // Validate WebSocket version
        if (request.get_header("Sec-WebSocket-Version") != "13") {
            return std::unexpected("Unsupported WebSocket version");
        }
        
        // Generate accept key
        auto client_key = request.get_header("Sec-WebSocket-Key");
        auto accept_key = generate_accept_key(client_key);
        
        // Build response
        HandshakeResponse response;
        response.status = 101;
        response.set_header("Upgrade", "websocket");
        response.set_header("Connection", "Upgrade");
        response.set_header("Sec-WebSocket-Accept", accept_key);
        
        return response;
    }
    
private:
    std::string generate_accept_key(const std::string& client_key) {
        const std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        std::string combined = client_key + magic;
        
        unsigned char hash[SHA_DIGEST_LENGTH];
        SHA1(reinterpret_cast<const unsigned char*>(combined.c_str()),
             combined.length(), hash);
        
        return base64_encode(hash, SHA_DIGEST_LENGTH);
    }
};
```

## Frame Format

### Frame Structure
* **FIN**: Final frame flag (1 bit)
* **RSV1-3**: Reserved bits (3 bits)
* **Opcode**: Frame type (4 bits)
* **Mask**: Masking flag (1 bit)
* **Payload length**: Payload length (7, 7+16, or 7+64 bits)
* **Masking key**: Masking key (32 bits, if masked)
* **Payload**: Frame payload (variable length)
* **Rationale**: Frame structure enables message framing

### Opcode Types
* **0x0**: Continuation frame
* **0x1**: Text frame
* **0x2**: Binary frame
* **0x8**: Close frame
* **0x9**: Ping frame
* **0xA**: Pong frame
* **Rationale**: Opcodes enable frame type identification

### Frame Parsing
* **Header parsing**: Parse frame header
* **Length parsing**: Parse payload length
* **Masking**: Unmask payload if masked
* **Validation**: Validate frame structure
* **Rationale**: Frame parsing enables message processing

### Example Frame Parser
```cpp
class WebSocketFrameParser {
public:
    Result<WebSocketFrame> parse(std::string_view data) {
        if (data.size() < 2) {
            return std::unexpected("Frame too short");
        }
        
        WebSocketFrame frame;
        uint8_t byte1 = data[0];
        uint8_t byte2 = data[1];
        
        frame.fin = (byte1 & 0x80) != 0;
        frame.rsv1 = (byte1 & 0x40) != 0;
        frame.rsv2 = (byte1 & 0x20) != 0;
        frame.rsv3 = (byte1 & 0x10) != 0;
        frame.opcode = static_cast<Opcode>(byte1 & 0x0F);
        frame.masked = (byte2 & 0x80) != 0;
        
        uint64_t payload_len = byte2 & 0x7F;
        size_t header_len = 2;
        
        if (payload_len == 126) {
            if (data.size() < 4) {
                return std::unexpected("Frame too short");
            }
            payload_len = (static_cast<uint16_t>(data[2]) << 8) | data[3];
            header_len = 4;
        } else if (payload_len == 127) {
            if (data.size() < 10) {
                return std::unexpected("Frame too short");
            }
            payload_len = 0;
            for (int i = 0; i < 8; ++i) {
                payload_len = (payload_len << 8) | static_cast<uint8_t>(data[2 + i]);
            }
            header_len = 10;
        }
        
        // Parse masking key if present
        if (frame.masked) {
            if (data.size() < header_len + 4) {
                return std::unexpected("Frame too short");
            }
            std::memcpy(frame.masking_key, &data[header_len], 4);
            header_len += 4;
        }
        
        // Parse payload
        if (data.size() < header_len + payload_len) {
            return std::unexpected("Frame incomplete");
        }
        
        frame.payload = data.substr(header_len, payload_len);
        
        // Unmask payload if masked
        if (frame.masked) {
            unmask_payload(frame.payload, frame.masking_key);
        }
        
        return frame;
    }
    
private:
    void unmask_payload(std::string_view& payload, const uint8_t* key) {
        for (size_t i = 0; i < payload.size(); ++i) {
            payload[i] ^= key[i % 4];
        }
    }
};
```

## Control Frames

### Ping Frame
* **Purpose**: Keep alive, connection health check
* **Response**: Must respond with Pong frame
* **Implementation**: Send ping periodically, respond to pings
* **Rationale**: Ping frames enable connection health checking

### Pong Frame
* **Purpose**: Response to Ping frame
* **Implementation**: Respond to ping with pong
* **Rationale**: Pong frames acknowledge ping frames

### Close Frame
* **Purpose**: Close connection gracefully
* **Close code**: Status code indicating reason
* **Close reason**: Human readable reason
* **Implementation**: Send close frame, wait for peer close, close socket
* **Rationale**: Close frames enable graceful connection closure

## Fragmentation

### Fragmented Messages
* **FIN flag**: Final frame in message
* **Continuation frames**: Frames with opcode 0x0
* **Message assembly**: Assemble fragmented messages
* **Rationale**: Fragmentation enables large messages

### Implementation
* **Message buffer**: Buffer fragmented frames
* **FIN detection**: Detect final frame
* **Message assembly**: Assemble complete message
* **Rationale**: Implementation enables fragmentation handling

## Implementation Standards

### Correctness
* **RFC compliance**: Strict RFC 6455 compliance
* **Frame validation**: Validate all frames
* **Error handling**: Proper error handling
* **Rationale**: Correctness is critical

### Performance
* **Efficient parsing**: Optimize frame parsing
* **Memory efficiency**: Minimize memory allocations
* **Zero copy**: Use zero copy when possible
* **Rationale**: Performance is critical

## Testing Requirements

### Unit Tests
* **Handshake tests**: Test handshake implementation
* **Frame parsing tests**: Test frame parsing
* **Control frame tests**: Test ping/pong/close
* **Fragmentation tests**: Test message fragmentation
* **Autobahn tests**: Run Autobahn TestSuite
* **Rationale**: Comprehensive testing ensures correctness

## Research Papers and References

### WebSocket Protocol
* RFC 6455 - WebSocket protocol specification
* RFC 7692 - permessage-deflate compression
* RFC 8441 - HTTP/2 WebSocket extension
* WebSocket implementation guides

## Implementation Checklist

- [ ] Understand RFC 6455
- [ ] Implement handshake
- [ ] Implement frame parsing
- [ ] Implement control frames
- [ ] Implement fragmentation
- [ ] Write comprehensive unit tests
- [ ] Run Autobahn TestSuite
- [ ] Document protocol implementation

