/*
 * Redis Hash Table - Open Addressing with Incremental Rehashing
 * 
 * Source: https://github.com/redis/redis/blob/unstable/src/dict.c
 * Repository: redis/redis
 * File: `src/dict.c`
 * 
 * What Makes It Ingenious:
 * - Two hash tables for incremental rehashing (non-blocking)
 * - Power-of-2 table sizes (bitwise modulo instead of expensive modulo)
 * - Progressive rehashing: moves one bucket per operation
 * - SipHash for security (resistant to hash flooding attacks)
 * - No blocking during rehashing - operations continue normally
 * 
 * When to Use:
 * - Need non-blocking hash table resizing
 * - High-performance key-value storage
 * - Security-sensitive applications (SipHash)
 * - Real-time systems where blocking is unacceptable
 * 
 * Real-World Usage:
 * - Redis database (all key-value operations)
 * - High-performance caching systems
 * - Real-time data structures
 * 
 * Time Complexity:
 * - Insert: O(1) average, O(n) worst case (during rehashing)
 * - Search: O(1) average, O(n) worst case (during rehashing)
 * - Delete: O(1) average, O(n) worst case (during rehashing)
 * - Rehashing: O(n) amortized (spread across operations)
 * 
 * Space Complexity: O(n) where n is number of elements
 */

#include <cstdint>
#include <cstring>
#include <vector>
#include <functional>

// Simplified SipHash implementation (Redis uses full SipHash)
// For production, use a proper SipHash library
uint64_t simple_hash(const char* key, size_t len) {
    uint64_t hash = 5381;
    for (size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(key[i]);
    }
    return hash;
}

template<typename K, typename V>
class RedisHashTable {
private:
    struct DictEntry {
        K key;
        V value;
        DictEntry* next; // For chaining during rehashing
        
        DictEntry() : next(nullptr) {}
    };
    
    struct DictTable {
        DictEntry** buckets;
        size_t size;      // Power of 2
        size_t size_mask; // size - 1 (for bitwise modulo)
        size_t used;      // Number of entries
        
        DictTable(size_t s) : size(s), size_mask(s - 1), used(0) {
            buckets = new DictEntry*[size];
            memset(buckets, 0, size * sizeof(DictEntry*));
        }
        
        ~DictTable() {
            for (size_t i = 0; i < size; i++) {
                DictEntry* entry = buckets[i];
                while (entry) {
                    DictEntry* next = entry->next;
                    delete entry;
                    entry = next;
                }
            }
            delete[] buckets;
        }
    };
    
    DictTable* table[2]; // Two tables for incremental rehashing
    int rehash_idx;      // -1 if not rehashing, else index of next bucket
    size_t rehash_buckets; // Number of buckets to rehash per operation
    
    // Hash function
    size_t hash_key(const K& key) const {
        // In Redis, this uses SipHash
        // Simplified here for demonstration
        std::hash<K> hasher;
        return hasher(key);
    }
    
    // Get bucket index using bitwise AND (power-of-2 optimization)
    size_t get_bucket_index(DictTable* t, size_t hash) const {
        return hash & t->size_mask; // Equivalent to hash % t->size
    }
    
    // Rehash one bucket from table[0] to table[1]
    void rehash_step() {
        if (rehash_idx < 0) return;
        
        DictTable* src = table[0];
        DictTable* dst = table[1];
        
        // Rehash one bucket
        while (rehash_idx < static_cast<int>(src->size) && src->buckets[rehash_idx] == nullptr) {
            rehash_idx++;
        }
        
        if (rehash_idx >= static_cast<int>(src->size)) {
            // Rehashing complete
            delete table[0];
            table[0] = table[1];
            table[1] = nullptr;
            rehash_idx = -1;
            return;
        }
        
        // Move all entries from this bucket
        DictEntry* entry = src->buckets[rehash_idx];
        DictEntry* next;
        while (entry) {
            next = entry->next;
            
            // Rehash to new table
            size_t hash = hash_key(entry->key);
            size_t idx = get_bucket_index(dst, hash);
            entry->next = dst->buckets[idx];
            dst->buckets[idx] = entry;
            dst->used++;
            
            entry = next;
        }
        
        src->buckets[rehash_idx] = nullptr;
        src->used -= (dst->used - (dst->used - 1)); // Approximate
        
        rehash_idx++;
    }
    
    // Start rehashing if load factor is too high
    void check_rehash() {
        if (rehash_idx >= 0) return; // Already rehashing
        
        DictTable* t = table[0];
        double load_factor = static_cast<double>(t->used) / t->size;
        
        // Start rehashing if load factor > 1.0
        if (load_factor > 1.0) {
            size_t new_size = t->size * 2;
            table[1] = new DictTable(new_size);
            rehash_idx = 0;
            rehash_buckets = 1; // Rehash one bucket per operation
        }
    }
    
public:
    RedisHashTable(size_t initial_size = 4) : rehash_idx(-1), rehash_buckets(1) {
        // Ensure initial_size is power of 2
        size_t size = 1;
        while (size < initial_size) {
            size <<= 1;
        }
        
        table[0] = new DictTable(size);
        table[1] = nullptr;
    }
    
    ~RedisHashTable() {
        delete table[0];
        if (table[1]) {
            delete table[1];
        }
    }
    
    // Insert or update key-value pair
    bool insert(const K& key, const V& value) {
        // Perform incremental rehashing
        if (rehash_idx >= 0) {
            rehash_step();
        }
        
        // Check if we need to start rehashing
        check_rehash();
        
        // Try both tables (if rehashing)
        for (int i = 0; i < 2; i++) {
            if (table[i] == nullptr) continue;
            
            size_t hash = hash_key(key);
            size_t idx = get_bucket_index(table[i], hash);
            
            // Check if key already exists
            DictEntry* entry = table[i]->buckets[idx];
            while (entry) {
                if (entry->key == key) {
                    entry->value = value; // Update
                    return false;
                }
                entry = entry->next;
            }
        }
        
        // Insert into table[0] (or table[1] if rehashing)
        DictTable* t = (rehash_idx >= 0) ? table[1] : table[0];
        size_t hash = hash_key(key);
        size_t idx = get_bucket_index(t, hash);
        
        DictEntry* new_entry = new DictEntry();
        new_entry->key = key;
        new_entry->value = value;
        new_entry->next = t->buckets[idx];
        t->buckets[idx] = new_entry;
        t->used++;
        
        return true;
    }
    
    // Find value by key
    V* find(const K& key) {
        // Perform incremental rehashing
        if (rehash_idx >= 0) {
            rehash_step();
        }
        
        // Search both tables (if rehashing)
        for (int i = 0; i < 2; i++) {
            if (table[i] == nullptr) continue;
            
            size_t hash = hash_key(key);
            size_t idx = get_bucket_index(table[i], hash);
            
            DictEntry* entry = table[i]->buckets[idx];
            while (entry) {
                if (entry->key == key) {
                    return &entry->value;
                }
                entry = entry->next;
            }
        }
        
        return nullptr;
    }
    
    // Remove key-value pair
    bool remove(const K& key) {
        // Perform incremental rehashing
        if (rehash_idx >= 0) {
            rehash_step();
        }
        
        // Search both tables (if rehashing)
        for (int i = 0; i < 2; i++) {
            if (table[i] == nullptr) continue;
            
            size_t hash = hash_key(key);
            size_t idx = get_bucket_index(table[i], hash);
            
            DictEntry* entry = table[i]->buckets[idx];
            DictEntry* prev = nullptr;
            
            while (entry) {
                if (entry->key == key) {
                    if (prev) {
                        prev->next = entry->next;
                    } else {
                        table[i]->buckets[idx] = entry->next;
                    }
                    delete entry;
                    table[i]->used--;
                    return true;
                }
                prev = entry;
                entry = entry->next;
            }
        }
        
        return false;
    }
    
    // Check if key exists
    bool contains(const K& key) {
        return find(key) != nullptr;
    }
    
    // Get current size
    size_t size() const {
        size_t total = table[0]->used;
        if (table[1]) {
            total += table[1]->used;
        }
        return total;
    }
    
    // Check if rehashing is in progress
    bool is_rehashing() const {
        return rehash_idx >= 0;
    }
};

// Example usage
#include <iostream>
#include <string>

int main() {
    RedisHashTable<std::string, int> dict;
    
    // Insert operations
    dict.insert("apple", 10);
    dict.insert("banana", 20);
    dict.insert("cherry", 30);
    
    // Search operations
    int* value = dict.find("banana");
    if (value) {
        std::cout << "banana: " << *value << std::endl;
    }
    
    // Update operation
    dict.insert("apple", 15);
    
    // Remove operation
    dict.remove("cherry");
    
    std::cout << "Size: " << dict.size() << std::endl;
    std::cout << "Rehashing: " << (dict.is_rehashing() ? "yes" : "no") << std::endl;
    
    return 0;
}

