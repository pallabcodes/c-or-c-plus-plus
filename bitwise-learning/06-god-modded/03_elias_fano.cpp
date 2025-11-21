/*
 * God-Modded: Elias-Fano Encoding
 * 
 * Succinct data structure for storing monotone sequences
 * with O(1) access and minimal space overhead.
 */
#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>
#include <cmath>

struct EliasFano {
    std::vector<uint64_t> upper_bits;
    std::vector<uint64_t> lower_bits;
    uint32_t lower_bits_per_element;
    uint64_t max_value;
    
    // Thread-safety: Not thread-safe (constructor)
    // Ownership: Owns upper_bits and lower_bits vectors
    // Invariants: sequence is sorted, n > 0
    // Failure modes: Undefined behavior if sequence not sorted or n == 0
    explicit EliasFano(const std::vector<uint64_t>& sequence) {
        assert(!sequence.empty());
        max_value = sequence.back();
        uint64_t n = sequence.size();
        
        if (max_value == 0) {
            lower_bits_per_element = 0;
        } else {
            lower_bits_per_element = static_cast<uint32_t>(std::floor(std::log2(max_value / n)));
        }
        
        uint64_t lower_mask = (1ULL << lower_bits_per_element) - 1;
        size_t upper_size = (n + max_value / (1ULL << lower_bits_per_element) + 63) / 64;
        upper_bits.resize(upper_size, 0);
        lower_bits.resize((n * lower_bits_per_element + 63) / 64, 0);
        
        uint64_t prev_upper = 0;
        for (size_t i = 0; i < n; ++i) {
            uint64_t val = sequence[i];
            uint64_t lower = val & lower_mask;
            uint64_t upper = val >> lower_bits_per_element;
            
            size_t lower_start = i * lower_bits_per_element;
            for (uint32_t j = 0; j < lower_bits_per_element; ++j) {
                if (lower & (1ULL << j)) {
                    size_t idx = (lower_start + j) >> 6;
                    size_t bit = (lower_start + j) & 63;
                    lower_bits[idx] |= (1ULL << bit);
                }
            }
            
            for (uint64_t u = prev_upper + 1; u <= upper; ++u) {
                size_t idx = (i + u) >> 6;
                size_t bit = (i + u) & 63;
                upper_bits[idx] |= (1ULL << bit);
            }
            prev_upper = upper;
        }
    }
    
    // Thread-safety: Thread-safe for concurrent reads (const method)
    // Ownership: None (read-only access)
    // Invariants: i < sequence size
    // Failure modes: Undefined behavior if i >= sequence size
    uint64_t access(size_t i) const {
        uint64_t lower = 0;
        size_t lower_start = i * lower_bits_per_element;
        for (uint32_t j = 0; j < lower_bits_per_element; ++j) {
            size_t idx = (lower_start + j) >> 6;
            size_t bit = (lower_start + j) & 63;
            if (lower_bits[idx] & (1ULL << bit)) {
                lower |= (1ULL << j);
            }
        }
        
        uint64_t upper = 0;
        uint64_t pos = i;
        for (size_t j = 0; j < upper_bits.size(); ++j) {
            uint64_t word = upper_bits[j];
            uint64_t pop = __builtin_popcountll(word);
            if (pos < pop) {
                uint64_t mask = word;
                for (uint64_t k = 0; k < pos; ++k) {
                    mask &= mask - 1;
                }
                upper = (j << 6) + __builtin_ctzll(mask) - i;
                break;
            }
            pos -= pop;
        }
        
        return (upper << lower_bits_per_element) | lower;
    }
};

int main() {
    std::vector<uint64_t> seq = {1, 3, 5, 7, 9, 11, 13, 15};
    EliasFano ef(seq);
    for (size_t i = 0; i < seq.size(); ++i) {
        std::cout << ef.access(i) << " ";
    }
    std::cout << std::endl;
    return 0;
}

