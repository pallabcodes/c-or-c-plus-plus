#pragma once

#include <cstdint>
#include <vector>
#include <span>
#include <string>
#include <string_view>
#include <expected>
#include <optional>

namespace networking::websocket {

// ================================================================================================
// WebSocket Frame Structure (RFC 6455)
// ================================================================================================

/**
 * WebSocket frame opcodes as defined in RFC 6455
 */
enum class Opcode : uint8_t {
    CONTINUATION = 0x0,  // Continuation frame
    TEXT = 0x1,          // Text frame (UTF-8)
    BINARY = 0x2,        // Binary frame
    // 0x3-0x7 reserved for future non-control frames
    CLOSE = 0x8,         // Close frame
    PING = 0x9,          // Ping frame
    PONG = 0xA,          // Pong frame
    // 0xB-0xF reserved for future control frames
};

/**
 * WebSocket close status codes (RFC 6455 Section 7.4)
 */
enum class CloseCode : uint16_t {
    NORMAL_CLOSURE = 1000,           // Normal closure
    GOING_AWAY = 1001,               // Endpoint going away
    PROTOCOL_ERROR = 1002,           // Protocol error
    UNSUPPORTED_DATA = 1003,         // Unsupported data type
    NO_STATUS_RECEIVED = 1005,       // No status code received
    ABNORMAL_CLOSURE = 1006,         // Abnormal closure
    INVALID_FRAME_PAYLOAD_DATA = 1007, // Invalid UTF-8 data
    POLICY_VIOLATION = 1008,         // Policy violation
    MESSAGE_TOO_BIG = 1009,          // Message too big
    MANDATORY_EXTENSION = 1010,      // Missing extension
    INTERNAL_SERVER_ERROR = 1011,    // Internal server error
    SERVICE_RESTART = 1012,          // Service restart
    TRY_AGAIN_LATER = 1013,          // Try again later
    BAD_GATEWAY = 1014,              // Bad gateway
    TLS_HANDSHAKE = 1015,            // TLS handshake failure
};

/**
 * Represents a complete WebSocket frame
 */
struct Frame {
    bool fin = true;                      // Final fragment flag
    bool rsv1 = false;                    // Reserved bit 1 (extensions)
    bool rsv2 = false;                    // Reserved bit 2 (extensions)
    bool rsv3 = false;                    // Reserved bit 3 (extensions)
    Opcode opcode = Opcode::TEXT;         // Frame opcode
    bool masked = false;                  // Mask flag
    uint32_t mask_key = 0;                // Masking key (if masked)
    std::vector<uint8_t> payload;         // Frame payload
    
    /**
     * Check if this is a control frame
     */
    bool is_control_frame() const {
        return static_cast<uint8_t>(opcode) >= 0x8;
    }
    
    /**
     * Get payload as string (for text frames)
     */
    std::string get_text() const {
        return std::string(payload.begin(), payload.end());
    }
    
    /**
     * Set payload from string (for text frames)
     */
    void set_text(std::string_view text) {
        payload.assign(text.begin(), text.end());
    }
    
    /**
     * Get close code and reason (for close frames)
     */
    std::pair<CloseCode, std::string> get_close_info() const;
    
    /**
     * Set close code and reason (for close frames)
     */
    void set_close_info(CloseCode code, std::string_view reason = "");
};

// ================================================================================================
// Frame Parser
// ================================================================================================

/**
 * Parse errors that can occur during frame parsing
 */
enum class ParseError {
    INCOMPLETE,          // Need more data
    INVALID_OPCODE,      // Unknown opcode
    INVALID_LENGTH,      // Invalid payload length
    CONTROL_FRAME_TOO_LARGE, // Control frame > 125 bytes
    FRAGMENTED_CONTROL_FRAME, // Control frame with FIN=0
    RESERVED_BITS_SET,   // Reserved bits are set
    INVALID_UTF8,        // Invalid UTF-8 in text frame
    PROTOCOL_VIOLATION,  // General protocol violation
};

/**
 * Result of frame parsing - either a frame and bytes consumed, or an error
 */
using ParseResult = std::expected<std::pair<Frame, size_t>, ParseError>;

/**
 * WebSocket frame parser implementing RFC 6455
 */
class FrameParser {
public:
    /**
     * Parse a WebSocket frame from binary data
     * 
     * @param data Raw binary data containing frame
     * @return ParseResult containing frame and bytes consumed, or error
     */
    static ParseResult parse_frame(std::span<const uint8_t> data);
    
    /**
     * Serialize a WebSocket frame to binary data
     * 
     * @param frame Frame to serialize
     * @param mask_client_frames Whether to mask the frame (client-side)
     * @return Serialized frame data
     */
    static std::vector<uint8_t> serialize_frame(const Frame& frame, bool mask_client_frames = false);
    
    /**
     * Create a text frame
     */
    static Frame create_text_frame(std::string_view text, bool fin = true);
    
    /**
     * Create a binary frame
     */
    static Frame create_binary_frame(std::span<const uint8_t> data, bool fin = true);
    
    /**
     * Create a close frame
     */
    static Frame create_close_frame(CloseCode code = CloseCode::NORMAL_CLOSURE, 
                                   std::string_view reason = "");
    
    /**
     * Create a ping frame
     */
    static Frame create_ping_frame(std::span<const uint8_t> payload = {});
    
    /**
     * Create a pong frame
     */
    static Frame create_pong_frame(std::span<const uint8_t> payload = {});

private:
    /**
     * Apply or remove masking to payload
     */
    static void apply_mask(std::span<uint8_t> payload, uint32_t mask_key);
    
    /**
     * Generate random masking key
     */
    static uint32_t generate_mask_key();
    
    /**
     * Validate UTF-8 encoding
     */
    static bool is_valid_utf8(std::span<const uint8_t> data);
    
    /**
     * Parse frame header and determine payload length
     */
    static std::expected<std::tuple<Frame, size_t, size_t>, ParseError> 
    parse_header(std::span<const uint8_t> data);
};

// ================================================================================================
// Message Reassembler
// ================================================================================================

/**
 * Reassembles fragmented WebSocket messages
 */
class MessageReassembler {
public:
    /**
     * Add a frame to the reassembler
     * 
     * @param frame Frame to add
     * @return Complete message if available, nullopt if more fragments needed
     */
    std::optional<Frame> add_frame(Frame frame);
    
    /**
     * Check if currently reassembling a message
     */
    bool is_assembling() const { return assembling_; }
    
    /**
     * Reset the reassembler state
     */
    void reset();

private:
    bool assembling_ = false;
    Opcode message_opcode_ = Opcode::TEXT;
    std::vector<uint8_t> assembled_payload_;
};

// ================================================================================================
// Utility Functions
// ================================================================================================

/**
 * Convert opcode to string for debugging
 */
std::string_view opcode_to_string(Opcode opcode);

/**
 * Convert close code to string for debugging
 */
std::string_view close_code_to_string(CloseCode code);

/**
 * Generate WebSocket accept key from client key (RFC 6455)
 */
std::string generate_accept_key(std::string_view client_key);

/**
 * Validate WebSocket key format
 */
bool is_valid_websocket_key(std::string_view key);

} // namespace networking::websocket
