/*
 * =============================================================================
 * Performance Engineering: Zero Copy Structs
 * Memory mapped read patterns and casting caveats
 * =============================================================================
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <vector>

// Example record stored in a binary file (little endian)
struct alignas(8) RecordDisk {
    uint64_t id;
    uint32_t price_cents;
    uint32_t qty;
};

// Memory mapped view (pretend we used mmap to map a file region)
struct MappedView {
    const uint8_t* data;
    size_t count;

    const RecordDisk* at(size_t i) const { return reinterpret_cast<const RecordDisk*>(data + i * sizeof(RecordDisk)); }
};

void demo_zero_copy() {
    std::cout << "\n=== ZERO COPY STRUCTS ===" << std::endl;
    // emulate file with a vector
    std::vector<uint8_t> file(sizeof(RecordDisk) * 3);
    for (size_t i = 0; i < 3; ++i) {
        RecordDisk r{ (uint64_t)(100 + i), (uint32_t)(1000 + 100*i), (uint32_t)(10 + i) };
        std::memcpy(file.data() + i * sizeof(RecordDisk), &r, sizeof(RecordDisk));
    }
    MappedView mv{ file.data(), 3 };
    for (size_t i = 0; i < mv.count; ++i) {
        const RecordDisk* r = mv.at(i);
        std::cout << r->id << " $" << (r->price_cents/100.0) << " qty=" << r->qty << std::endl;
    }
    std::cout << "\nNOTE: In production use mmap, validate endianness and alignment, and guard against TOCTOU." << std::endl;
}

int main() {
    try {
        demo_zero_copy();
        std::cout << "\n=== ZERO COPY COMPLETED SUCCESSFULLY ===" << std::endl;
    } catch (...) { std::cerr << "error" << std::endl; return 1; }
    return 0;
}
