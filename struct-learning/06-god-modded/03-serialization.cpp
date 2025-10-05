/*
 * =============================================================================
 * God Modded: Serialization
 * Binary record write and simple JSON-like emit
 * =============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <string>

struct TxRecord {
    uint64_t id;
    uint32_t amount_cents;
    uint16_t currency;
};

static void write_binary(std::vector<uint8_t>& out, const TxRecord& r) {
    size_t off = out.size(); out.resize(off + sizeof(TxRecord));
    std::memcpy(out.data() + off, &r, sizeof(TxRecord));
}

static std::string to_json(const TxRecord& r) {
    return std::string("{\"id\":") + std::to_string(r.id) +
           ",\"amount_cents\":" + std::to_string(r.amount_cents) +
           ",\"currency\":" + std::to_string(r.currency) + "}";
}

void demo_serialization() {
    std::cout << "\n=== GOD MODDED: SERIALIZATION ===" << std::endl;
    TxRecord a{111, 12345, 840}, b{112, 99999, 978};
    std::vector<uint8_t> bin;
    write_binary(bin, a); write_binary(bin, b);
    std::cout << "bin_size=" << bin.size() << std::endl;
    std::cout << to_json(a) << std::endl;
}

int main() {
    try { demo_serialization(); std::cout << "\n=== SERIALIZATION COMPLETED SUCCESSFULLY ===" << std::endl; }
    catch (...) { std::cerr << "error" << std::endl; return 1; }
    return 0;
}
