/*
 * Cuckoo Hashing - Two Hash Tables with Two Hash Functions
 * 
 * Source: "Cuckoo Hashing" by Rasmus Pagh and Flemming Friche Rodler
 * Paper: ESA 2001 (European Symposium on Algorithms)
 * 
 * What Makes It Ingenious:
 * - O(1) worst-case lookup guarantee
 * - Two hash tables with two independent hash functions
 * - Kick-out strategy: evicts existing element on collision
 * - Simple and elegant algorithm
 * - Good cache performance (only two locations to check)
 * 
 * When to Use:
 * - Need guaranteed O(1) worst-case lookup
 * - Can tolerate occasional rehashing
 * - Want simple implementation
 * - Read-heavy workloads (lookup is always O(1))
 * 
 * Real-World Usage:
 * - High-performance lookup tables
 * - Network routers (fast packet lookup)
 * - Compiler symbol tables
 * - Database indexes requiring O(1) lookup
 * 
 * Time Complexity:
 * - Insert: O(1) expected, O(n) worst case (requires rehashing)
 * - Search: O(1) worst case (only two locations to check)
 * - Delete: O(1) worst case
 * - Rehash: O(n) when needed
 * 
 * Space Complexity: O(n) where n is number of elements
 * 
 * Load Factor: Typically kept below 0.5 for good performance
 */

#include <cstdint>
#include <vector>
#include <functional>
#include <stdexcept>

template<typename K, typename V>
class CuckooHashTable {
private:
    struct Entry {
        K key;
        V value;
        bool occupied;
        
        Entry() : occupied(false) {}
    };
    
    std::vector<Entry> table1;
    std::vector<Entry> table2;
    size_t capacity;
    size_t num_elements;
    size_t max_iterations;
    double max_load_factor;
    
    // Two independent hash functions
    size_t hash1(const K& key) const {
        std::hash<K> hasher;
        return hasher(key) % capacity;
    }
    
    size_t hash2(const K& key) const {
        // Second hash function (different seed)
        std::hash<K> hasher;
        // Use a different hash combination
        size_t h = hasher(key);
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;
        return h % capacity;
    }
    
    // Rehash entire table (double capacity)
    void rehash() {
        std::vector<Entry> old_table1 = table1;
        std::vector<Entry> old_table2 = table2;
        size_t old_capacity = capacity;
        
        capacity *= 2;
        table1.clear();
        table2.clear();
        table1.resize(capacity);
        table2.resize(capacity);
        num_elements = 0;
        
        // Reinsert all elements
        for (size_t i = 0; i < old_capacity; i++) {
            if (old_table1[i].occupied) {
                insert(old_table1[i].key, old_table1[i].value);
            }
            if (old_table2[i].occupied) {
                insert(old_table2[i].key, old_table2[i].value);
            }
        }
    }
    
    // Check if rehashing is needed
    void check_rehash() {
        double load_factor = static_cast<double>(num_elements) / (2 * capacity);
        if (load_factor > max_load_factor) {
            rehash();
        }
    }
    
public:
    CuckooHashTable(size_t cap = 16, double max_load = 0.5, size_t max_iter = 100)
        : capacity(cap)
        , num_elements(0)
        , max_iterations(max_iter)
        , max_load_factor(max_load) {
        table1.resize(capacity);
        table2.resize(capacity);
    }
    
    // Insert key-value pair
    bool insert(const K& key, const V& value) {
        // Check if key already exists
        if (contains(key)) {
            // Update existing
            size_t idx1 = hash1(key);
            size_t idx2 = hash2(key);
            
            if (table1[idx1].occupied && table1[idx1].key == key) {
                table1[idx1].value = value;
                return false;
            }
            if (table2[idx2].occupied && table2[idx2].key == key) {
                table2[idx2].value = value;
                return false;
            }
        }
        
        check_rehash();
        
        K current_key = key;
        V current_value = value;
        bool use_table1 = true;
        size_t iterations = 0;
        
        // Cuckoo hashing: try to insert, kick out if needed
        while (iterations < max_iterations) {
            if (use_table1) {
                size_t idx = hash1(current_key);
                if (!table1[idx].occupied) {
                    // Found empty slot
                    table1[idx].key = current_key;
                    table1[idx].value = current_value;
                    table1[idx].occupied = true;
                    num_elements++;
                    return true;
                }
                // Kick out existing element
                std::swap(current_key, table1[idx].key);
                std::swap(current_value, table1[idx].value);
            } else {
                size_t idx = hash2(current_key);
                if (!table2[idx].occupied) {
                    // Found empty slot
                    table2[idx].key = current_key;
                    table2[idx].value = current_value;
                    table2[idx].occupied = true;
                    num_elements++;
                    return true;
                }
                // Kick out existing element
                std::swap(current_key, table2[idx].key);
                std::swap(current_value, table2[idx].value);
            }
            
            use_table1 = !use_table1; // Switch tables
            iterations++;
        }
        
        // Max iterations reached, need to rehash
        rehash();
        return insert(current_key, current_value);
    }
    
    // Search for key (O(1) worst case)
    V* find(const K& key) {
        // Check both tables (only two locations!)
        size_t idx1 = hash1(key);
        if (table1[idx1].occupied && table1[idx1].key == key) {
            return &table1[idx1].value;
        }
        
        size_t idx2 = hash2(key);
        if (table2[idx2].occupied && table2[idx2].key == key) {
            return &table2[idx2].value;
        }
        
        return nullptr;
    }
    
    // Check if key exists
    bool contains(const K& key) {
        return find(key) != nullptr;
    }
    
    // Remove key-value pair
    bool remove(const K& key) {
        // Check table1
        size_t idx1 = hash1(key);
        if (table1[idx1].occupied && table1[idx1].key == key) {
            table1[idx1].occupied = false;
            num_elements--;
            return true;
        }
        
        // Check table2
        size_t idx2 = hash2(key);
        if (table2[idx2].occupied && table2[idx2].key == key) {
            table2[idx2].occupied = false;
            num_elements--;
            return true;
        }
        
        return false;
    }
    
    // Get current size
    size_t size() const {
        return num_elements;
    }
    
    // Get capacity
    size_t get_capacity() const {
        return capacity;
    }
    
    // Get load factor
    double load_factor() const {
        return static_cast<double>(num_elements) / (2 * capacity);
    }
};

// Example usage
#include <iostream>
#include <string>

int main() {
    CuckooHashTable<std::string, int> hash_table(8, 0.5, 100);
    
    // Insert operations
    hash_table.insert("apple", 10);
    hash_table.insert("banana", 20);
    hash_table.insert("cherry", 30);
    hash_table.insert("date", 40);
    
    // Search operations (O(1) worst case)
    int* value = hash_table.find("banana");
    if (value) {
        std::cout << "banana: " << *value << std::endl;
    }
    
    // Update operation
    hash_table.insert("apple", 15);
    
    // Remove operation
    hash_table.remove("cherry");
    
    std::cout << "Size: " << hash_table.size() << std::endl;
    std::cout << "Capacity: " << hash_table.get_capacity() << std::endl;
    std::cout << "Load factor: " << hash_table.load_factor() << std::endl;
    
    return 0;
}

