/*
 * =============================================================================
 * Advanced Struct Techniques: Anonymous Structs
 * Production grade examples without UB and with clear guidance
 * =============================================================================
 *
 * Goal
 *   Show how to use anonymous structs and nested aggregates in safe ways
 *   Clarify where compilers differ and how to keep layout predictable
 *
 * Guidance
 *   Prefer named types for public APIs
 *   Use anonymous structs for local scopes and tight data grouping
 *   Validate size and alignment when layout matters
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <type_traits>

// Local grouping with an anonymous struct inside a union like container
// This pattern is common in low latency paths where we alias views
struct Message {
    uint32_t id;
    
    struct {               // anonymous sub object
        uint16_t major;
        uint16_t minor;
    } version;             // instance name gives access: msg.version.major

    union {                // different views of the same payload
        struct {           // anonymous view for header fields
            uint8_t flags;
            uint8_t type;
        } hdr;
        uint16_t hdr_u16;  // packed view when sending on the wire
    } u;
};

static_assert(std::is_standard_layout<Message>::value, "Message should be standard layout");

// Anonymous temporary object for immediate use
void log_inline_version() {
    // Construct and use an unnamed aggregate directly
    std::cout << "inline version: "
              << (struct { int a; int b; }){1, 2}.a
              << '.'
              << (struct { int a; int b; }){1, 2}.b
              << std::endl;
}

// Aggregate initialization with nested anonymous members
void demo_message() {
    std::cout << "\n=== ANONYMOUS STRUCTS: MESSAGE ===" << std::endl;
    Message m{};
    m.id = 1001U;
    m.version.major = 3;
    m.version.minor = 14;
    m.u.hdr.flags = 0b001001;
    m.u.hdr.type = 7;

    std::cout << "id=" << m.id
              << " version=" << m.version.major << '.' << m.version.minor
              << " flags=0b" << std::bitset<8>(m.u.hdr.flags)
              << " type=" << (int)m.u.hdr.type
              << " hdr_u16=0x" << std::hex << m.u.hdr_u16 << std::dec
              << std::endl;
}

// ABI and layout notes
// When the exact layout matters prefer named structs in headers
// Keep anonymous usage local to translation units to avoid ABI drift
void layout_notes() {
    std::cout << "\nanonymous structs are best kept local and private" << std::endl;
}

int main() {
    try {
        log_inline_version();
        demo_message();
        layout_notes();
        std::cout << "\n=== ANONYMOUS STRUCTS COMPLETED SUCCESSFULLY ===" << std::endl;
    } catch (...) {
        std::cerr << "error" << std::endl;
        return 1;
    }
    return 0;
}
