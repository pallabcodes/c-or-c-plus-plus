/*
 * PostgreSQL Hash Table - Separate Chaining with Dynamic Resizing
 * 
 * Source: https://github.com/postgres/postgres/blob/master/src/backend/utils/hash/dynahash.c
 * Repository: postgres/postgres
 * File: `src/backend/utils/hash/dynahash.c`
 * 
 * What Makes It Ingenious:
 * - Separate chaining for collision resolution
 * - Dynamic hash table growth (doubles size when needed)
 * - Memory-efficient design (only allocates chains as needed)
 * - Concurrency-safe design (can be extended with locks)
 * - Flexible hash function support
 * 
 * When to Use:
 * - Need predictable worst-case performance
 * - Memory efficiency is important
 * - Want to avoid clustering issues
 * - Need to handle variable-length keys efficiently
 * 
 * Real-World Usage:
 * - PostgreSQL hash indexes
 * - PostgreSQL hash joins
 * - Database internal hash tables
 * - Systems requiring predictable performance
 * 
 * Time Complexity:
 * - Insert: O(1) average, O(k) worst case where k is chain length
 * - Search: O(1) average, O(k) worst case where k is chain length
 * - Delete: O(1) average, O(k) worst case where k is chain length
 * - Resize: O(n) where n is number of elements
 * 
 * Space Complexity: O(n + m) where n is elements, m is buckets
 */

#include <cstdint>
#include <vector>
#include <functional>
#include <algorithm>

template<typename K, typename V>
class PostgreSQLHashTable {
private:
    struct HashEntry {
        K key;
        V value;
        HashEntry* next;
        
        HashEntry(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
    };
    
    std::vector<HashEntry*> buckets;
    size_t num_buckets;
    size_t num_elements;
    double max_load_factor;
    
    // Hash function
    size_t hash_key(const K& key) const {
        std::hash<K> hasher;
        return hasher(key) % num_buckets;
    }
    
    // Resize hash table (double size)
    void resize() {
        size_t old_size = num_buckets;
        num_buckets *= 2;
        
        std::vector<HashEntry*> new_buckets(num_buckets, nullptr);
        
        // Rehash all elements
        for (size_t i = 0; i < old_size; i++) {
            HashEntry* entry = buckets[i];
            while (entry) {
                HashEntry* next = entry->next;
                
                // Rehash to new bucket
                size_t new_idx = hash_key(entry->key);
                entry->next = new_buckets[new_idx];
                new_buckets[new_idx] = entry;
                
                entry = next;
            }
        }
        
        buckets = std::move(new_buckets);
    }
    
    // Check if resize is needed
    void check_resize() {
        double load_factor = static_cast<double>(num_elements) / num_buckets;
        if (load_factor > max_load_factor) {
            resize();
        }
    }
    
public:
    PostgreSQLHashTable(size_t initial_buckets = 16, double max_load = 0.75)
        : num_buckets(initial_buckets)
        , num_elements(0)
        , max_load_factor(max_load) {
        buckets.resize(num_buckets, nullptr);
    }
    
    ~PostgreSQLHashTable() {
        clear();
    }
    
    // Insert or update key-value pair
    bool insert(const K& key, const V& value) {
        check_resize();
        
        size_t idx = hash_key(key);
        HashEntry* entry = buckets[idx];
        
        // Check if key already exists
        while (entry) {
            if (entry->key == key) {
                entry->value = value; // Update existing
                return false;
            }
            entry = entry->next;
        }
        
        // Insert new entry at head of chain
        HashEntry* new_entry = new HashEntry(key, value);
        new_entry->next = buckets[idx];
        buckets[idx] = new_entry;
        num_elements++;
        
        return true;
    }
    
    // Find value by key
    V* find(const K& key) {
        size_t idx = hash_key(key);
        HashEntry* entry = buckets[idx];
        
        while (entry) {
            if (entry->key == key) {
                return &entry->value;
            }
            entry = entry->next;
        }
        
        return nullptr;
    }
    
    // Check if key exists
    bool contains(const K& key) {
        return find(key) != nullptr;
    }
    
    // Remove key-value pair
    bool remove(const K& key) {
        size_t idx = hash_key(key);
        HashEntry* entry = buckets[idx];
        HashEntry* prev = nullptr;
        
        while (entry) {
            if (entry->key == key) {
                if (prev) {
                    prev->next = entry->next;
                } else {
                    buckets[idx] = entry->next;
                }
                delete entry;
                num_elements--;
                return true;
            }
            prev = entry;
            entry = entry->next;
        }
        
        return false;
    }
    
    // Get current size
    size_t size() const {
        return num_elements;
    }
    
    // Get number of buckets
    size_t bucket_count() const {
        return num_buckets;
    }
    
    // Get load factor
    double load_factor() const {
        return static_cast<double>(num_elements) / num_buckets;
    }
    
    // Clear all entries
    void clear() {
        for (size_t i = 0; i < num_buckets; i++) {
            HashEntry* entry = buckets[i];
            while (entry) {
                HashEntry* next = entry->next;
                delete entry;
                entry = next;
            }
            buckets[i] = nullptr;
        }
        num_elements = 0;
    }
    
    // Get average chain length (for analysis)
    double average_chain_length() const {
        if (num_buckets == 0) return 0.0;
        
        size_t total_chains = 0;
        size_t non_empty_buckets = 0;
        
        for (size_t i = 0; i < num_buckets; i++) {
            if (buckets[i]) {
                non_empty_buckets++;
                HashEntry* entry = buckets[i];
                size_t chain_len = 0;
                while (entry) {
                    chain_len++;
                    entry = entry->next;
                }
                total_chains += chain_len;
            }
        }
        
        return non_empty_buckets > 0 
            ? static_cast<double>(total_chains) / non_empty_buckets 
            : 0.0;
    }
};

// Example usage
#include <iostream>
#include <string>

int main() {
    PostgreSQLHashTable<std::string, int> hash_table;
    
    // Insert operations
    hash_table.insert("apple", 10);
    hash_table.insert("banana", 20);
    hash_table.insert("cherry", 30);
    hash_table.insert("date", 40);
    
    // Search operations
    int* value = hash_table.find("banana");
    if (value) {
        std::cout << "banana: " << *value << std::endl;
    }
    
    // Update operation
    hash_table.insert("apple", 15);
    
    // Remove operation
    hash_table.remove("cherry");
    
    std::cout << "Size: " << hash_table.size() << std::endl;
    std::cout << "Buckets: " << hash_table.bucket_count() << std::endl;
    std::cout << "Load factor: " << hash_table.load_factor() << std::endl;
    std::cout << "Average chain length: " << hash_table.average_chain_length() << std::endl;
    
    return 0;
}

