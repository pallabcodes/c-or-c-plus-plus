/*
 * V8 Hash-Based Binary Search
 * 
 * Source: node/deps/v8/src/objects/descriptor-array-inl.h (lines 95-129)
 * 
 * What Makes It Ingenious:
 * - Uses hash values for comparison (faster than string comparison)
 * - Binary search on hash, then linear scan for collisions
 * - Optimized for JavaScript object property lookup
 * 
 * When to Use:
 * - Searching by computed key (hash, checksum, etc.)
 * - Key comparison is expensive
 * - Hash collisions possible
 * 
 * Real-World Usage:
 * - V8 JavaScript engine property descriptor lookup
 * - Object property access optimization
 */

#include <cstdint>
#include <vector>
#include <algorithm>
#include <functional>

template<typename Key, typename Value>
class HashBasedBinarySearch {
public:
    struct Entry {
        uint32_t hash;
        Key key;
        Value value;
    };

private:
    std::vector<Entry> entries_;

    // Binary search by hash value
    int BinarySearchByHash(uint32_t target_hash) {
        int left = 0;
        int right = entries_.size() - 1;
        
        while (left <= right) {
            int mid = left + (right - left) / 2;
            
            if (entries_[mid].hash == target_hash) {
                // Found hash match, return first occurrence
                // Need to find first occurrence of this hash
                while (mid > 0 && entries_[mid - 1].hash == target_hash) {
                    mid--;
                }
                return mid;
            } else if (entries_[mid].hash < target_hash) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        
        return -1; // Not found
    }

public:
    // Add entry (assumes entries are sorted by hash)
    void AddEntry(uint32_t hash, const Key& key, const Value& value) {
        Entry entry{hash, key, value};
        auto it = std::lower_bound(entries_.begin(), entries_.end(), entry,
            [](const Entry& a, const Entry& b) {
                return a.hash < b.hash;
            });
        entries_.insert(it, entry);
    }

    // Search: Binary search on hash, then linear scan for exact match
    Value* Search(const Key& target_key, std::function<uint32_t(const Key&)> hash_fn) {
        uint32_t target_hash = hash_fn(target_key);
        
        // Binary search to find hash match
        int pos = BinarySearchByHash(target_hash);
        if (pos == -1) {
            return nullptr; // Hash not found
        }
        
        // Linear scan for exact match (handle hash collisions)
        for (int i = pos; i < static_cast<int>(entries_.size()); i++) {
            if (entries_[i].hash != target_hash) {
                break; // No more hash matches
            }
            if (entries_[i].key == target_key) {
                return &entries_[i].value; // Exact match found
            }
        }
        
        return nullptr; // Key not found
    }
};

// Example usage
#include <iostream>
#include <string>

int main() {
    HashBasedBinarySearch<std::string, int> search;
    
    // Hash function
    auto hash_fn = [](const std::string& s) -> uint32_t {
        uint32_t hash = 0;
        for (char c : s) {
            hash = hash * 31 + c;
        }
        return hash;
    };
    
    // Add entries
    search.AddEntry(hash_fn("name"), "name", 1);
    search.AddEntry(hash_fn("age"), "age", 2);
    search.AddEntry(hash_fn("city"), "city", 3);
    
    // Search
    auto* result = search.Search("age", hash_fn);
    if (result) {
        std::cout << "Found: " << *result << std::endl;
    }
    
    return 0;
}

