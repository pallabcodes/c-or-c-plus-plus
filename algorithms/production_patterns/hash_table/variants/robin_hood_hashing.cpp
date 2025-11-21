/*
 * Robin Hood Hashing - Open Addressing with Distance Tracking
 * 
 * Source: "Robin Hood Hashing" by Pedro Celis
 * Paper: University of Waterloo Technical Report CS-86-14 (1986)
 * 
 * What Makes It Ingenious:
 * - Reduced variance in probe lengths (more uniform distribution)
 * - Better cache performance than standard open addressing
 * - Backward shift deletion (maintains probe order)
 * - "Steal from the rich, give to the poor" - swaps entries to balance distances
 * - Predictable worst-case performance
 * 
 * When to Use:
 * - Need better cache performance than standard open addressing
 * - Want reduced variance in probe lengths
 * - High load factors are acceptable
 * - Read-heavy workloads benefit from better cache locality
 * 
 * Real-World Usage:
 * - High-performance hash tables
 * - Game engines (fast lookups)
 * - Compiler symbol tables
 * - Database indexes
 * 
 * Time Complexity:
 * - Insert: O(1) average, O(log n) worst case
 * - Search: O(1) average, O(log n) worst case
 * - Delete: O(1) average with backward shift
 * 
 * Space Complexity: O(n) where n is number of elements
 * 
 * Load Factor: Can handle higher load factors (0.8-0.9) than standard open addressing
 */

#include <cstdint>
#include <vector>
#include <functional>
#include <algorithm>

template<typename K, typename V>
class RobinHoodHashTable {
private:
    struct Entry {
        K key;
        V value;
        size_t distance; // Distance from ideal position
        bool occupied;
        
        Entry() : distance(0), occupied(false) {}
    };
    
    std::vector<Entry> table;
    size_t capacity;
    size_t num_elements;
    double max_load_factor;
    
    // Hash function
    size_t hash_key(const K& key) const {
        std::hash<K> hasher;
        return hasher(key) % capacity;
    }
    
    // Get ideal position for key
    size_t ideal_position(const K& key) const {
        return hash_key(key);
    }
    
    // Calculate distance from ideal position
    size_t calculate_distance(size_t ideal_pos, size_t current_pos) const {
        if (current_pos >= ideal_pos) {
            return current_pos - ideal_pos;
        } else {
            // Wrapped around
            return capacity - ideal_pos + current_pos;
        }
    }
    
    // Resize table (double capacity)
    void resize() {
        std::vector<Entry> old_table = table;
        size_t old_capacity = capacity;
        
        capacity *= 2;
        table.clear();
        table.resize(capacity);
        num_elements = 0;
        
        // Reinsert all elements
        for (size_t i = 0; i < old_capacity; i++) {
            if (old_table[i].occupied) {
                insert(old_table[i].key, old_table[i].value);
            }
        }
    }
    
    // Check if resize is needed
    void check_resize() {
        double load_factor = static_cast<double>(num_elements) / capacity;
        if (load_factor > max_load_factor) {
            resize();
        }
    }
    
public:
    RobinHoodHashTable(size_t cap = 16, double max_load = 0.8)
        : capacity(cap)
        , num_elements(0)
        , max_load_factor(max_load) {
        table.resize(capacity);
    }
    
    // Insert key-value pair
    bool insert(const K& key, const V& value) {
        check_resize();
        
        size_t ideal_pos = ideal_position(key);
        size_t pos = ideal_pos;
        size_t distance = 0;
        
        Entry new_entry;
        new_entry.key = key;
        new_entry.value = value;
        new_entry.distance = distance;
        new_entry.occupied = true;
        
        // Robin Hood: find position, swapping if we find entry with smaller distance
        while (true) {
            if (!table[pos].occupied) {
                // Found empty slot
                table[pos] = new_entry;
                num_elements++;
                return true;
            }
            
            if (table[pos].key == key) {
                // Key already exists, update value
                table[pos].value = value;
                return false;
            }
            
            // Robin Hood: if current entry is "richer" (closer to ideal), swap
            if (distance > table[pos].distance) {
                std::swap(new_entry, table[pos]);
            }
            
            // Continue probing
            pos = (pos + 1) % capacity;
            distance++;
            new_entry.distance = distance;
            
            // Safety check (shouldn't happen with proper load factor)
            if (distance >= capacity) {
                resize();
                return insert(key, value);
            }
        }
    }
    
    // Search for key
    V* find(const K& key) {
        size_t ideal_pos = ideal_position(key);
        size_t pos = ideal_pos;
        size_t distance = 0;
        
        while (table[pos].occupied && distance <= table[pos].distance) {
            if (table[pos].key == key) {
                return &table[pos].value;
            }
            pos = (pos + 1) % capacity;
            distance++;
        }
        
        return nullptr;
    }
    
    // Check if key exists
    bool contains(const K& key) {
        return find(key) != nullptr;
    }
    
    // Remove key-value pair (backward shift deletion)
    bool remove(const K& key) {
        size_t ideal_pos = ideal_position(key);
        size_t pos = ideal_pos;
        size_t distance = 0;
        
        // Find the entry
        while (table[pos].occupied && distance <= table[pos].distance) {
            if (table[pos].key == key) {
                // Found it - use backward shift deletion
                size_t next_pos = (pos + 1) % capacity;
                
                // Shift entries backward until we hit an empty slot or entry at ideal position
                while (table[next_pos].occupied && 
                       table[next_pos].distance > 0) {
                    table[pos] = table[next_pos];
                    table[pos].distance--;
                    pos = next_pos;
                    next_pos = (pos + 1) % capacity;
                }
                
                // Clear the last position
                table[pos].occupied = false;
                num_elements--;
                return true;
            }
            pos = (pos + 1) % capacity;
            distance++;
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
        return static_cast<double>(num_elements) / capacity;
    }
    
    // Get average probe distance (for analysis)
    double average_probe_distance() const {
        if (num_elements == 0) return 0.0;
        
        size_t total_distance = 0;
        for (size_t i = 0; i < capacity; i++) {
            if (table[i].occupied) {
                total_distance += table[i].distance;
            }
        }
        
        return static_cast<double>(total_distance) / num_elements;
    }
    
    // Get maximum probe distance (for analysis)
    size_t max_probe_distance() const {
        size_t max_dist = 0;
        for (size_t i = 0; i < capacity; i++) {
            if (table[i].occupied) {
                max_dist = std::max(max_dist, table[i].distance);
            }
        }
        return max_dist;
    }
};

// Example usage
#include <iostream>
#include <string>

int main() {
    RobinHoodHashTable<std::string, int> hash_table(16, 0.8);
    
    // Insert operations
    hash_table.insert("apple", 10);
    hash_table.insert("banana", 20);
    hash_table.insert("cherry", 30);
    hash_table.insert("date", 40);
    hash_table.insert("elderberry", 50);
    
    // Search operations
    int* value = hash_table.find("banana");
    if (value) {
        std::cout << "banana: " << *value << std::endl;
    }
    
    // Update operation
    hash_table.insert("apple", 15);
    
    // Remove operation (backward shift)
    hash_table.remove("cherry");
    
    std::cout << "Size: " << hash_table.size() << std::endl;
    std::cout << "Capacity: " << hash_table.get_capacity() << std::endl;
    std::cout << "Load factor: " << hash_table.load_factor() << std::endl;
    std::cout << "Average probe distance: " << hash_table.average_probe_distance() << std::endl;
    std::cout << "Max probe distance: " << hash_table.max_probe_distance() << std::endl;
    
    return 0;
}

