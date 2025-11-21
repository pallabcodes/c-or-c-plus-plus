/*
 * Performance: Roaring Bitmap
 * 
 * Production-grade roaring bitmap implementation for efficient
 * set operations on sparse and dense bitmaps using hybrid containers.
 */
#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>
#include <algorithm>

struct RoaringBitmap {
    struct ArrayContainer {
        std::vector<uint16_t> values;
        
        // Thread-safety: Not thread-safe (modifies values)
        // Ownership: Modifies owned values
        // Invariants: None
        // Failure modes: None
        void add(uint16_t x) {
            auto it = std::lower_bound(values.begin(), values.end(), x);
            if (it == values.end() || *it != x) {
                values.insert(it, x);
            }
        }
        
        // Thread-safety: Thread-safe for concurrent reads (const method)
        // Ownership: None (read-only access)
        // Invariants: None
        // Failure modes: None
        bool contains(uint16_t x) const {
            return std::binary_search(values.begin(), values.end(), x);
        }
    };
    
    struct BitmapContainer {
        uint64_t bits[1024];
        
        // Thread-safety: Not thread-safe (modifies bits)
        // Ownership: Modifies owned bits
        // Invariants: None
        // Failure modes: None
        BitmapContainer() {
            for (int i = 0; i < 1024; ++i) {
                bits[i] = 0;
            }
        }
        
        // Thread-safety: Not thread-safe (modifies bits)
        // Ownership: Modifies owned bits
        // Invariants: x < 65536
        // Failure modes: Undefined behavior if x >= 65536
        void add(uint16_t x) {
            bits[x >> 6] |= (1ULL << (x & 63));
        }
        
        // Thread-safety: Thread-safe for concurrent reads (const method)
        // Ownership: None (read-only access)
        // Invariants: x < 65536
        // Failure modes: Undefined behavior if x >= 65536
        bool contains(uint16_t x) const {
            return (bits[x >> 6] & (1ULL << (x & 63))) != 0;
        }
        
        // Thread-safety: Thread-safe (pure function)
        // Ownership: None (value semantics)
        // Invariants: None
        // Failure modes: None
        uint32_t cardinality() const {
            uint32_t count = 0;
            for (int i = 0; i < 1024; ++i) {
                count += __builtin_popcountll(bits[i]);
            }
            return count;
        }
    };
    
    std::vector<std::pair<uint16_t, void*>> containers;
    
    // Thread-safety: Not thread-safe (modifies containers)
    // Ownership: Modifies owned containers
    // Invariants: None
    // Failure modes: None
    void add(uint32_t x) {
        uint16_t high = static_cast<uint16_t>(x >> 16);
        uint16_t low = static_cast<uint16_t>(x & 0xFFFF);
        
        auto it = std::lower_bound(containers.begin(), containers.end(),
                                   high, [](const auto& p, uint16_t h) {
            return p.first < h;
        });
        
        if (it == containers.end() || it->first != high) {
            auto* arr = new ArrayContainer();
            arr->add(low);
            containers.insert(it, {high, arr});
        } else {
            auto* arr = static_cast<ArrayContainer*>(it->second);
            arr->add(low);
            if (arr->values.size() > 4096) {
                auto* bm = new BitmapContainer();
                for (uint16_t v : arr->values) {
                    bm->add(v);
                }
                delete arr;
                it->second = bm;
            }
        }
    }
    
    // Thread-safety: Thread-safe for concurrent reads (const method)
    // Ownership: None (read-only access)
    // Invariants: None
    // Failure modes: None
    bool contains(uint32_t x) const {
        uint16_t high = static_cast<uint16_t>(x >> 16);
        uint16_t low = static_cast<uint16_t>(x & 0xFFFF);
        
        auto it = std::lower_bound(containers.begin(), containers.end(),
                                   high, [](const auto& p, uint16_t h) {
            return p.first < h;
        });
        
        if (it == containers.end() || it->first != high) {
            return false;
        }
        
        auto* arr = static_cast<ArrayContainer*>(it->second);
        return arr->contains(low);
    }
};

int main() {
    RoaringBitmap rb;
    for (uint32_t i = 0; i < 10000; i += 3) {
        rb.add(i);
    }
    std::cout << rb.contains(3000) << " " << rb.contains(3001) << std::endl;
    return 0;
}

