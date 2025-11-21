/*
 * God-Modded: Compressed Bitvector
 * 
 * Space-efficient bitvector using run-length encoding and
 * compressed representations for sparse bitvectors.
 */
#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>

struct CompressedBitvector {
    struct Run {
        uint32_t length;
        bool value;
    };
    
    std::vector<Run> runs;
    size_t total_bits;
    
    // Thread-safety: Not thread-safe (constructor)
    // Ownership: Owns runs vector
    // Invariants: bits.size() > 0
    // Failure modes: Undefined behavior if bits.empty()
    explicit CompressedBitvector(const std::vector<bool>& bits) : total_bits(bits.size()) {
        assert(!bits.empty());
        
        if (bits.empty()) {
            return;
        }
        
        bool current = bits[0];
        uint32_t count = 1;
        
        for (size_t i = 1; i < bits.size(); ++i) {
            if (bits[i] == current) {
                ++count;
            } else {
                runs.push_back({count, current});
                current = bits[i];
                count = 1;
            }
        }
        runs.push_back({count, current});
    }
    
    // Thread-safety: Thread-safe for concurrent reads (const method)
    // Ownership: None (read-only access)
    // Invariants: i < total_bits
    // Failure modes: Undefined behavior if i >= total_bits
    bool get(size_t i) const {
        assert(i < total_bits);
        size_t pos = 0;
        for (const auto& run : runs) {
            if (i < pos + run.length) {
                return run.value;
            }
            pos += run.length;
        }
        return false;
    }
    
    // Thread-safety: Thread-safe (pure function)
    // Ownership: None (value semantics)
    // Invariants: None
    // Failure modes: None
    size_t compressed_size() const {
        return runs.size() * sizeof(Run);
    }
    
    // Thread-safety: Thread-safe (pure function)
    // Ownership: None (value semantics)
    // Invariants: None
    // Failure modes: None
    size_t uncompressed_size() const {
        return (total_bits + 7) / 8;
    }
};

int main() {
    std::vector<bool> bits = {0,0,0,1,1,1,0,0,1,1,1,1};
    CompressedBitvector cbv(bits);
    std::cout << cbv.get(3) << " " << cbv.get(5) << std::endl;
    std::cout << "Compressed: " << cbv.compressed_size() 
              << " Uncompressed: " << cbv.uncompressed_size() << std::endl;
    return 0;
}

