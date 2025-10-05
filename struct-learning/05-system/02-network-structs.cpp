/*
 * =============================================================================
 * System Programming: Network Structs
 * Protocol header layouts and parsing notes
 * =============================================================================
 */

#include <iostream>
#include <cstdint>
#include <cstring>

// Note: Use ntohs and ntohl in real parsing; fields here are illustrative
struct __attribute__((packed)) IPv4Header {
    uint8_t ver_ihl;      // version(4) + header len(4)
    uint8_t tos;
    uint16_t total_len;
    uint16_t id;
    uint16_t flags_frag;  // flags(3) + frag offset(13)
    uint8_t ttl;
    uint8_t proto;
    uint16_t checksum;
    uint32_t src;
    uint32_t dst;
};

struct __attribute__((packed)) UdpHeader {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t len;
    uint16_t checksum;
};

void demo_network_headers() {
    std::cout << "\n=== SYSTEM: NETWORK STRUCTS ===" << std::endl;
    uint8_t pkt[sizeof(IPv4Header) + sizeof(UdpHeader)]{};

    IPv4Header* ip = reinterpret_cast<IPv4Header*>(pkt);
    ip->ver_ihl = (4u << 4) | 5u; ip->ttl = 64; ip->proto = 17; // UDP
    ip->total_len = sizeof(pkt);

    UdpHeader* udp = reinterpret_cast<UdpHeader*>(pkt + sizeof(IPv4Header));
    udp->src_port = 9000; udp->dst_port = 9001; udp->len = sizeof(UdpHeader);

    std::cout << "IPv4 ihl=" << (int)(ip->ver_ihl & 0x0F) << " ttl=" << (int)ip->ttl
              << " proto=" << (int)ip->proto << std::endl;
    std::cout << "UDP sport=" << udp->src_port << " dport=" << udp->dst_port << std::endl;
    std::cout << "\nNOTE: In production handle endianness, checksum, and bounds." << std::endl;
}

int main() {
    try { demo_network_headers(); std::cout << "\n=== NETWORK STRUCTS COMPLETED SUCCESSFULLY ===" << std::endl; }
    catch (...) { std::cerr << "error" << std::endl; return 1; }
    return 0;
}
