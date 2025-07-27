#include "websocket/frame_parser.h"
#include <random>
#include <algorithm>
#include <cstring>
#include <openssl/sha.h>
#include <openssl/evp.h>

namespace networking::websocket {

// ================================================================================================
// Frame Implementation
// ================================================================================================

std::pair<CloseCode, std::string> Frame::get_close_info() const {
    if (opcode != Opcode::CLOSE || payload.size() < 2) {
        return {CloseCode::NO_STATUS_RECEIVED, ""};
    }
    
    // Extract close code (big-endian uint16)
    uint16_t code = (static_cast<uint16_t>(payload[0]) << 8) | payload[1];
    CloseCode close_code = static_cast<CloseCode>(code);
    
    // Extract reason (remaining bytes as UTF-8)
    std::string reason;
    if (payload.size() > 2) {
        reason = std::string(payload.begin() + 2, payload.end());
    }
    
    return {close_code, reason};
}

void Frame::set_close_info(CloseCode code, std::string_view reason) {
    opcode = Opcode::CLOSE;
    payload.clear();
    
    // Add close code (big-endian uint16)
    uint16_t code_value = static_cast<uint16_t>(code);
    payload.push_back(static_cast<uint8_t>(code_value >> 8));
    payload.push_back(static_cast<uint8_t>(code_value & 0xFF));
    
    // Add reason
    payload.insert(payload.end(), reason.begin(), reason.end());
}

// ================================================================================================
// FrameParser Implementation
// ================================================================================================

ParseResult FrameParser::parse_frame(std::span<const uint8_t> data) {
    if (data.size() < 2) {
        return std::unexpected(ParseError::INCOMPLETE);
    }
    
    // Parse header and get frame info
    auto header_result = parse_header(data);
    if (!header_result) {
        return std::unexpected(header_result.error());
    }
    
    auto [frame, header_length, payload_length] = *header_result;
    
    // Check if we have complete frame
    if (data.size() < header_length + payload_length) {
        return std::unexpected(ParseError::INCOMPLETE);
    }
    
    // Extract payload
    if (payload_length > 0) {
        frame.payload.resize(payload_length);
        std::copy(data.begin() + header_length, 
                 data.begin() + header_length + payload_length,
                 frame.payload.begin());
        
        // Apply unmasking if needed
        if (frame.masked) {
            apply_mask(frame.payload, frame.mask_key);
        }
    }
    
    // Validate frame according to RFC 6455
    if (frame.is_control_frame()) {
        // Control frames must not be fragmented
        if (!frame.fin) {
            return std::unexpected(ParseError::FRAGMENTED_CONTROL_FRAME);
        }
        
        // Control frames must have payload <= 125 bytes
        if (payload_length > 125) {
            return std::unexpected(ParseError::CONTROL_FRAME_TOO_LARGE);
        }
    }
    
    // Validate UTF-8 for text frames
    if (frame.opcode == Opcode::TEXT || 
        (frame.opcode == Opcode::CONTINUATION && frame.fin)) {
        if (!is_valid_utf8(frame.payload)) {
            return std::unexpected(ParseError::INVALID_UTF8);
        }
    }
    
    // Validate close frame payload
    if (frame.opcode == Opcode::CLOSE && payload_length > 0) {
        if (payload_length < 2) {
            return std::unexpected(ParseError::PROTOCOL_VIOLATION);
        }
        
        // Validate close reason is valid UTF-8
        if (payload_length > 2) {
            std::span<const uint8_t> reason_bytes(frame.payload.begin() + 2, frame.payload.end());
            if (!is_valid_utf8(reason_bytes)) {
                return std::unexpected(ParseError::INVALID_UTF8);
            }
        }
    }
    
    return std::make_pair(std::move(frame), header_length + payload_length);
}

std::expected<std::tuple<Frame, size_t, size_t>, ParseError> 
FrameParser::parse_header(std::span<const uint8_t> data) {
    Frame frame;
    size_t pos = 0;
    
    // First byte: FIN + RSV + Opcode
    uint8_t first_byte = data[pos++];
    frame.fin = (first_byte & 0x80) != 0;
    frame.rsv1 = (first_byte & 0x40) != 0;
    frame.rsv2 = (first_byte & 0x20) != 0;
    frame.rsv3 = (first_byte & 0x10) != 0;
    
    uint8_t opcode_value = first_byte & 0x0F;
    
    // Validate opcode
    switch (opcode_value) {
        case 0x0: frame.opcode = Opcode::CONTINUATION; break;
        case 0x1: frame.opcode = Opcode::TEXT; break;
        case 0x2: frame.opcode = Opcode::BINARY; break;
        case 0x8: frame.opcode = Opcode::CLOSE; break;
        case 0x9: frame.opcode = Opcode::PING; break;
        case 0xA: frame.opcode = Opcode::PONG; break;
        default:
            return std::unexpected(ParseError::INVALID_OPCODE);
    }
    
    // Reserved bits must be 0 unless extensions are negotiated
    if (frame.rsv1 || frame.rsv2 || frame.rsv3) {
        return std::unexpected(ParseError::RESERVED_BITS_SET);
    }
    
    // Second byte: MASK + Payload length
    uint8_t second_byte = data[pos++];
    frame.masked = (second_byte & 0x80) != 0;
    uint8_t payload_length_indicator = second_byte & 0x7F;
    
    // Determine actual payload length
    uint64_t payload_length;
    
    if (payload_length_indicator < 126) {
        payload_length = payload_length_indicator;
    } else if (payload_length_indicator == 126) {
        // 16-bit length
        if (data.size() < pos + 2) {
            return std::unexpected(ParseError::INCOMPLETE);
        }
        
        payload_length = (static_cast<uint64_t>(data[pos]) << 8) | data[pos + 1];
        pos += 2;
        
        // Must not use extended length for small payloads
        if (payload_length < 126) {
            return std::unexpected(ParseError::INVALID_LENGTH);
        }
    } else { // payload_length_indicator == 127
        // 64-bit length
        if (data.size() < pos + 8) {
            return std::unexpected(ParseError::INCOMPLETE);
        }
        
        payload_length = 0;
        for (int i = 0; i < 8; ++i) {
            payload_length = (payload_length << 8) | data[pos + i];
        }
        pos += 8;
        
        // Must not use 64-bit length for smaller payloads
        if (payload_length < 65536) {
            return std::unexpected(ParseError::INVALID_LENGTH);
        }
        
        // Most significant bit must be 0
        if (payload_length & 0x8000000000000000ULL) {
            return std::unexpected(ParseError::INVALID_LENGTH);
        }
    }
    
    // Extract masking key if present
    if (frame.masked) {
        if (data.size() < pos + 4) {
            return std::unexpected(ParseError::INCOMPLETE);
        }
        
        frame.mask_key = *reinterpret_cast<const uint32_t*>(data.data() + pos);
        pos += 4;
    }
    
    return std::make_tuple(std::move(frame), pos, static_cast<size_t>(payload_length));
}

std::vector<uint8_t> FrameParser::serialize_frame(const Frame& frame, bool mask_client_frames) {
    std::vector<uint8_t> result;
    
    // First byte: FIN + RSV + Opcode
    uint8_t first_byte = 0;
    if (frame.fin) first_byte |= 0x80;
    if (frame.rsv1) first_byte |= 0x40;
    if (frame.rsv2) first_byte |= 0x20;
    if (frame.rsv3) first_byte |= 0x10;
    first_byte |= static_cast<uint8_t>(frame.opcode) & 0x0F;
    result.push_back(first_byte);
    
    // Determine if we should mask this frame
    bool should_mask = frame.masked || mask_client_frames;
    uint32_t mask_key = should_mask ? 
        (frame.masked ? frame.mask_key : generate_mask_key()) : 0;
    
    // Second byte: MASK + Payload length
    size_t payload_size = frame.payload.size();
    uint8_t second_byte = should_mask ? 0x80 : 0x00;
    
    if (payload_size < 126) {
        second_byte |= static_cast<uint8_t>(payload_size);
        result.push_back(second_byte);
    } else if (payload_size < 65536) {
        second_byte |= 126;
        result.push_back(second_byte);
        
        // 16-bit length (big-endian)
        result.push_back(static_cast<uint8_t>(payload_size >> 8));
        result.push_back(static_cast<uint8_t>(payload_size & 0xFF));
    } else {
        second_byte |= 127;
        result.push_back(second_byte);
        
        // 64-bit length (big-endian)
        uint64_t length = payload_size;
        for (int i = 7; i >= 0; --i) {
            result.push_back(static_cast<uint8_t>((length >> (i * 8)) & 0xFF));
        }
    }
    
    // Masking key
    if (should_mask) {
        result.push_back(static_cast<uint8_t>(mask_key & 0xFF));
        result.push_back(static_cast<uint8_t>((mask_key >> 8) & 0xFF));
        result.push_back(static_cast<uint8_t>((mask_key >> 16) & 0xFF));
        result.push_back(static_cast<uint8_t>((mask_key >> 24) & 0xFF));
    }
    
    // Payload
    if (!frame.payload.empty()) {
        size_t payload_start = result.size();
        result.insert(result.end(), frame.payload.begin(), frame.payload.end());
        
        // Apply masking if needed
        if (should_mask) {
            std::span<uint8_t> payload_span(result.data() + payload_start, payload_size);
            apply_mask(payload_span, mask_key);
        }
    }
    
    return result;
}

Frame FrameParser::create_text_frame(std::string_view text, bool fin) {
    Frame frame;
    frame.fin = fin;
    frame.opcode = Opcode::TEXT;
    frame.set_text(text);
    return frame;
}

Frame FrameParser::create_binary_frame(std::span<const uint8_t> data, bool fin) {
    Frame frame;
    frame.fin = fin;
    frame.opcode = Opcode::BINARY;
    frame.payload.assign(data.begin(), data.end());
    return frame;
}

Frame FrameParser::create_close_frame(CloseCode code, std::string_view reason) {
    Frame frame;
    frame.set_close_info(code, reason);
    return frame;
}

Frame FrameParser::create_ping_frame(std::span<const uint8_t> payload) {
    Frame frame;
    frame.opcode = Opcode::PING;
    frame.payload.assign(payload.begin(), payload.end());
    return frame;
}

Frame FrameParser::create_pong_frame(std::span<const uint8_t> payload) {
    Frame frame;
    frame.opcode = Opcode::PONG;
    frame.payload.assign(payload.begin(), payload.end());
    return frame;
}

void FrameParser::apply_mask(std::span<uint8_t> payload, uint32_t mask_key) {
    uint8_t mask_bytes[4] = {
        static_cast<uint8_t>(mask_key & 0xFF),
        static_cast<uint8_t>((mask_key >> 8) & 0xFF),
        static_cast<uint8_t>((mask_key >> 16) & 0xFF),
        static_cast<uint8_t>((mask_key >> 24) & 0xFF)
    };
    
    for (size_t i = 0; i < payload.size(); ++i) {
        payload[i] ^= mask_bytes[i % 4];
    }
}

uint32_t FrameParser::generate_mask_key() {
    static thread_local std::random_device rd;
    static thread_local std::mt19937 gen(rd());
    static thread_local std::uniform_int_distribution<uint32_t> dis;
    
    return dis(gen);
}

bool FrameParser::is_valid_utf8(std::span<const uint8_t> data) {
    size_t i = 0;
    
    while (i < data.size()) {
        uint8_t byte = data[i];
        
        if (byte < 0x80) {
            // ASCII character
            i++;
        } else if ((byte & 0xE0) == 0xC0) {
            // 2-byte sequence
            if (i + 1 >= data.size() || (data[i + 1] & 0xC0) != 0x80) {
                return false;
            }
            
            // Check for overlong encoding
            uint32_t codepoint = ((byte & 0x1F) << 6) | (data[i + 1] & 0x3F);
            if (codepoint < 0x80) {
                return false;
            }
            
            i += 2;
        } else if ((byte & 0xF0) == 0xE0) {
            // 3-byte sequence
            if (i + 2 >= data.size() || 
                (data[i + 1] & 0xC0) != 0x80 || 
                (data[i + 2] & 0xC0) != 0x80) {
                return false;
            }
            
            // Check for overlong encoding and surrogates
            uint32_t codepoint = ((byte & 0x0F) << 12) | 
                               ((data[i + 1] & 0x3F) << 6) | 
                               (data[i + 2] & 0x3F);
            if (codepoint < 0x800 || (codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
                return false;
            }
            
            i += 3;
        } else if ((byte & 0xF8) == 0xF0) {
            // 4-byte sequence
            if (i + 3 >= data.size() || 
                (data[i + 1] & 0xC0) != 0x80 || 
                (data[i + 2] & 0xC0) != 0x80 ||
                (data[i + 3] & 0xC0) != 0x80) {
                return false;
            }
            
            // Check for overlong encoding and valid range
            uint32_t codepoint = ((byte & 0x07) << 18) | 
                               ((data[i + 1] & 0x3F) << 12) |
                               ((data[i + 2] & 0x3F) << 6) | 
                               (data[i + 3] & 0x3F);
            if (codepoint < 0x10000 || codepoint > 0x10FFFF) {
                return false;
            }
            
            i += 4;
        } else {
            // Invalid start byte
            return false;
        }
    }
    
    return true;
}

// ================================================================================================
// MessageReassembler Implementation
// ================================================================================================

std::optional<Frame> MessageReassembler::add_frame(Frame frame) {
    // Control frames are never fragmented
    if (frame.is_control_frame()) {
        return frame;
    }
    
    if (!assembling_) {
        // Starting new message
        if (frame.opcode == Opcode::CONTINUATION) {
            // Error: continuation frame without initial frame
            reset();
            return std::nullopt;
        }
        
        if (frame.fin) {
            // Complete single-frame message
            return frame;
        }
        
        // Start of fragmented message
        assembling_ = true;
        message_opcode_ = frame.opcode;
        assembled_payload_ = std::move(frame.payload);
        return std::nullopt;
    } else {
        // Continuing fragmented message
        if (frame.opcode != Opcode::CONTINUATION) {
            // Error: expected continuation frame
            reset();
            return std::nullopt;
        }
        
        // Add payload to assembled message
        assembled_payload_.insert(assembled_payload_.end(), 
                                frame.payload.begin(), frame.payload.end());
        
        if (frame.fin) {
            // Message complete
            Frame complete_frame;
            complete_frame.fin = true;
            complete_frame.opcode = message_opcode_;
            complete_frame.payload = std::move(assembled_payload_);
            
            reset();
            return complete_frame;
        }
        
        // More fragments to come
        return std::nullopt;
    }
}

void MessageReassembler::reset() {
    assembling_ = false;
    message_opcode_ = Opcode::TEXT;
    assembled_payload_.clear();
}

// ================================================================================================
// Utility Functions
// ================================================================================================

std::string_view opcode_to_string(Opcode opcode) {
    switch (opcode) {
        case Opcode::CONTINUATION: return "CONTINUATION";
        case Opcode::TEXT: return "TEXT";
        case Opcode::BINARY: return "BINARY";
        case Opcode::CLOSE: return "CLOSE";
        case Opcode::PING: return "PING";
        case Opcode::PONG: return "PONG";
        default: return "UNKNOWN";
    }
}

std::string_view close_code_to_string(CloseCode code) {
    switch (code) {
        case CloseCode::NORMAL_CLOSURE: return "NORMAL_CLOSURE";
        case CloseCode::GOING_AWAY: return "GOING_AWAY";
        case CloseCode::PROTOCOL_ERROR: return "PROTOCOL_ERROR";
        case CloseCode::UNSUPPORTED_DATA: return "UNSUPPORTED_DATA";
        case CloseCode::NO_STATUS_RECEIVED: return "NO_STATUS_RECEIVED";
        case CloseCode::ABNORMAL_CLOSURE: return "ABNORMAL_CLOSURE";
        case CloseCode::INVALID_FRAME_PAYLOAD_DATA: return "INVALID_FRAME_PAYLOAD_DATA";
        case CloseCode::POLICY_VIOLATION: return "POLICY_VIOLATION";
        case CloseCode::MESSAGE_TOO_BIG: return "MESSAGE_TOO_BIG";
        case CloseCode::MANDATORY_EXTENSION: return "MANDATORY_EXTENSION";
        case CloseCode::INTERNAL_SERVER_ERROR: return "INTERNAL_SERVER_ERROR";
        case CloseCode::SERVICE_RESTART: return "SERVICE_RESTART";
        case CloseCode::TRY_AGAIN_LATER: return "TRY_AGAIN_LATER";
        case CloseCode::BAD_GATEWAY: return "BAD_GATEWAY";
        case CloseCode::TLS_HANDSHAKE: return "TLS_HANDSHAKE";
        default: return "UNKNOWN";
    }
}

std::string generate_accept_key(std::string_view client_key) {
    // WebSocket magic string from RFC 6455
    constexpr const char* WEBSOCKET_MAGIC = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    
    // Concatenate client key with magic string
    std::string combined = std::string(client_key) + WEBSOCKET_MAGIC;
    
    // Compute SHA-1 hash
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(combined.c_str()), combined.length(), hash);
    
    // Base64 encode the hash
    const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    result.reserve(28); // SHA-1 is 20 bytes, base64 is ~28 chars
    
    for (int i = 0; i < SHA_DIGEST_LENGTH; i += 3) {
        uint32_t value = (hash[i] << 16);
        if (i + 1 < SHA_DIGEST_LENGTH) value |= (hash[i + 1] << 8);
        if (i + 2 < SHA_DIGEST_LENGTH) value |= hash[i + 2];
        
        result += base64_chars[(value >> 18) & 0x3F];
        result += base64_chars[(value >> 12) & 0x3F];
        result += (i + 1 < SHA_DIGEST_LENGTH) ? base64_chars[(value >> 6) & 0x3F] : '=';
        result += (i + 2 < SHA_DIGEST_LENGTH) ? base64_chars[value & 0x3F] : '=';
    }
    
    return result;
}

bool is_valid_websocket_key(std::string_view key) {
    // WebSocket key should be 16 bytes base64-encoded = 24 characters with padding
    if (key.length() != 24) {
        return false;
    }
    
    // Check for valid base64 characters
    for (size_t i = 0; i < 22; ++i) {
        char c = key[i];
        if (!((c >= 'A' && c <= 'Z') || 
              (c >= 'a' && c <= 'z') || 
              (c >= '0' && c <= '9') || 
              c == '+' || c == '/')) {
            return false;
        }
    }
    
    // Last two characters should be '=='
    return key.substr(22) == "==";
}

} // namespace networking::websocket
