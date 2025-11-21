/*
 * God-Modded: Golomb-Rice Encoding
 * 
 * Compressed encoding for integers using Golomb-Rice codes,
 * optimal for geometric distributions.
 */
#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>

struct GolombRiceEncoder {
    uint32_t m;
    uint32_t k;
    
    // Thread-safety: Not thread-safe (constructor)
    // Ownership: None
    // Invariants: m > 0
    // Failure modes: Undefined behavior if m == 0
    explicit GolombRiceEncoder(uint32_t m_param) : m(m_param) {
        assert(m > 0);
        k = 0;
        uint32_t temp = m;
        while (temp > 1) {
            temp >>= 1;
            ++k;
        }
    }
    
    // Thread-safety: Thread-safe (pure function)
    // Ownership: None (value semantics)
    // Invariants: x >= 0
    // Failure modes: Undefined behavior if x < 0
    std::vector<bool> encode(uint32_t x) const {
        assert(static_cast<int32_t>(x) >= 0);
        std::vector<bool> code;
        
        uint32_t q = x / m;
        uint32_t r = x % m;
        
        for (uint32_t i = 0; i < q; ++i) {
            code.push_back(true);
        }
        code.push_back(false);
        
        if (m != (1U << k)) {
            if (r < (1U << (k + 1)) - m) {
                for (int i = k - 1; i >= 0; --i) {
                    code.push_back((r >> i) & 1);
                }
            } else {
                r += (1U << (k + 1)) - m;
                for (int i = k; i >= 0; --i) {
                    code.push_back((r >> i) & 1);
                }
            }
        } else {
            for (int i = k - 1; i >= 0; --i) {
                code.push_back((r >> i) & 1);
            }
        }
        
        return code;
    }
    
    // Thread-safety: Thread-safe (pure function)
    // Ownership: None (value semantics)
    // Invariants: code is valid Golomb-Rice encoding
    // Failure modes: Undefined behavior if code is invalid
    uint32_t decode(const std::vector<bool>& code, size_t* pos) const {
        assert(pos != nullptr);
        size_t p = *pos;
        
        uint32_t q = 0;
        while (p < code.size() && code[p]) {
            ++q;
            ++p;
        }
        if (p >= code.size()) {
            return 0;
        }
        ++p;
        
        uint32_t r = 0;
        if (m != (1U << k)) {
            for (uint32_t i = 0; i < k; ++i) {
                if (p < code.size()) {
                    r = (r << 1) | (code[p] ? 1 : 0);
                    ++p;
                }
            }
            if (r < (1U << (k + 1)) - m) {
            } else {
                if (p < code.size()) {
                    r = (r << 1) | (code[p] ? 1 : 0);
                    ++p;
                }
                r -= (1U << (k + 1)) - m;
            }
        } else {
            for (uint32_t i = 0; i < k; ++i) {
                if (p < code.size()) {
                    r = (r << 1) | (code[p] ? 1 : 0);
                    ++p;
                }
            }
        }
        
        *pos = p;
        return q * m + r;
    }
};

int main() {
    GolombRiceEncoder encoder(4);
    std::vector<bool> code = encoder.encode(10);
    std::cout << "Encoded: ";
    for (bool b : code) {
        std::cout << (b ? '1' : '0');
    }
    std::cout << std::endl;
    
    size_t pos = 0;
    uint32_t decoded = encoder.decode(code, &pos);
    std::cout << "Decoded: " << decoded << std::endl;
    return 0;
}

