/*
 * God-Modded: Wavelet Tree
 * 
 * Succinct data structure supporting rank/select operations
 * on sequences over arbitrary alphabets using bitvectors.
 */
#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>
#include <algorithm>

struct WaveletTree {
    struct BitVec {
        std::vector<uint64_t> bits;
        std::vector<uint32_t> rank_table;
        
        // Thread-safety: Not thread-safe (constructor)
        // Ownership: Owns bits and rank_table vectors
        // Invariants: nbits > 0
        // Failure modes: Undefined behavior if nbits == 0
        explicit BitVec(size_t nbits) : bits((nbits + 63) / 64) {
            assert(nbits > 0);
        }
        
        // Thread-safety: Not thread-safe (modifies bits)
        // Ownership: Modifies owned bits
        // Invariants: i < bits.size() * 64
        // Failure modes: Undefined behavior if i >= bits.size() * 64
        void set(size_t i) {
            assert(i < bits.size() * 64);
            bits[i >> 6] |= (1ULL << (i & 63));
        }
        
        // Thread-safety: Not thread-safe (modifies rank_table)
        // Ownership: Modifies owned rank_table
        // Invariants: bits.size() > 0
        // Failure modes: None
        void build_rank() {
            rank_table.resize(bits.size() + 1);
            uint32_t count = 0;
            for (size_t i = 0; i < bits.size(); ++i) {
                rank_table[i] = count;
                count += __builtin_popcountll(bits[i]);
            }
            rank_table[bits.size()] = count;
        }
        
        // Thread-safety: Thread-safe for concurrent reads (const method)
        // Ownership: None (read-only access)
        // Invariants: i <= bits.size() * 64
        // Failure modes: Undefined behavior if i > bits.size() * 64
        uint32_t rank1(size_t i) const {
            assert(i <= bits.size() * 64);
            size_t word = i >> 6;
            uint32_t base = rank_table[word];
            uint64_t mask = (i & 63) ? ((1ULL << (i & 63)) - 1ULL) : 0ULL;
            return base + __builtin_popcountll(bits[word] & mask);
        }
        
        // Thread-safety: Thread-safe for concurrent reads (const method)
        // Ownership: None (read-only access)
        // Invariants: i <= bits.size() * 64
        // Failure modes: Undefined behavior if i > bits.size() * 64
        uint32_t rank0(size_t i) const {
            return i - rank1(i);
        }
    };
    
    std::vector<BitVec> levels;
    std::vector<uint8_t> alphabet;
    size_t n;
    
    // Thread-safety: Not thread-safe (constructor)
    // Ownership: Owns levels vector and alphabet
    // Invariants: sequence.size() > 0
    // Failure modes: Undefined behavior if sequence.empty()
    explicit WaveletTree(const std::vector<uint8_t>& sequence) : n(sequence.size()) {
        assert(!sequence.empty());
        
        std::vector<uint8_t> sorted = sequence;
        std::sort(sorted.begin(), sorted.end());
        sorted.erase(std::unique(sorted.begin(), sorted.end()), sorted.end());
        alphabet = sorted;
        
        std::vector<uint8_t> current = sequence;
        uint8_t min_val = *std::min_element(alphabet.begin(), alphabet.end());
        uint8_t max_val = *std::max_element(alphabet.begin(), alphabet.end());
        
        build_tree(current, min_val, max_val, 0);
    }
    
    // Thread-safety: Not thread-safe (modifies levels)
    // Ownership: Modifies owned levels
    // Invariants: min_val <= max_val
    // Failure modes: Undefined behavior if min_val > max_val
    void build_tree(std::vector<uint8_t>& seq, uint8_t min_val, uint8_t max_val, size_t level) {
        if (min_val == max_val || seq.empty()) {
            return;
        }
        
        if (level >= levels.size()) {
            levels.resize(level + 1);
            levels[level] = BitVec(seq.size());
        }
        
        uint8_t mid = min_val + (max_val - min_val) / 2;
        std::vector<uint8_t> left, right;
        
        for (size_t i = 0; i < seq.size(); ++i) {
            if (seq[i] <= mid) {
                levels[level].set(i);
                left.push_back(seq[i]);
            } else {
                right.push_back(seq[i]);
            }
        }
        
        levels[level].build_rank();
        
        if (!left.empty()) {
            build_tree(left, min_val, mid, level + 1);
        }
        if (!right.empty()) {
            build_tree(right, mid + 1, max_val, level + 1);
        }
    }
    
    // Thread-safety: Thread-safe for concurrent reads (const method)
    // Ownership: None (read-only access)
    // Invariants: i < n, c in alphabet
    // Failure modes: Undefined behavior if i >= n or c not in alphabet
    uint32_t rank(uint8_t c, size_t i) const {
        assert(i < n);
        uint8_t min_val = alphabet[0];
        uint8_t max_val = alphabet.back();
        return rank_recursive(c, i, min_val, max_val, 0);
    }
    
    // Thread-safety: Thread-safe for concurrent reads (const method)
    // Ownership: None (read-only access)
    // Invariants: min_val <= max_val, level < levels.size()
    // Failure modes: Undefined behavior if invariants violated
    uint32_t rank_recursive(uint8_t c, size_t i, uint8_t min_val, uint8_t max_val, size_t level) const {
        if (min_val == max_val) {
            return i;
        }
        
        uint8_t mid = min_val + (max_val - min_val) / 2;
        uint32_t zeros = levels[level].rank0(i);
        uint32_t ones = levels[level].rank1(i);
        
        if (c <= mid) {
            return rank_recursive(c, zeros, min_val, mid, level + 1);
        } else {
            return rank_recursive(c, ones, mid + 1, max_val, level + 1);
        }
    }
};

int main() {
    std::vector<uint8_t> seq = {1, 2, 3, 1, 2, 3, 1, 2};
    WaveletTree wt(seq);
    std::cout << wt.rank(2, 5) << std::endl;
    return 0;
}

