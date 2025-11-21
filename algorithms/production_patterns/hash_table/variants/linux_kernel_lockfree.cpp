/*
 * Linux Kernel Hash Table - Lock-Free with RCU Support
 * 
 * Source: `/Users/picon/Learning/c-or-c-plus-plus/linux/include/linux/hashtable.h`
 * File: `linux/include/linux/hashtable.h`
 * 
 * What Makes It Ingenious:
 * - RCU (Read-Copy-Update) for lock-free reads
 * - Separate chaining with hlist (head-only list)
 * - Power-of-2 table sizes (compile-time or runtime)
 * - Memory barriers for multi-core safety
 * - Intrusive data structures (no extra allocations)
 * - Lock-free iteration with RCU
 * 
 * When to Use:
 * - High-concurrency read-heavy workloads
 * - Kernel-level code requiring lock-free operations
 * - Systems with many readers, few writers
 * - Need to avoid reader-writer lock overhead
 * - Real-time systems requiring predictable latency
 * 
 * Real-World Usage:
 * - Linux kernel process management
 * - Linux kernel file descriptor tables
 * - Linux kernel network subsystem
 * - High-performance server applications
 * 
 * Time Complexity:
 * - Insert: O(1) average (with RCU grace period)
 * - Search: O(1) average, O(k) worst case where k is chain length
 * - Delete: O(1) average (with RCU grace period)
 * - Iteration: O(n) where n is number of elements
 * 
 * Space Complexity: O(n + m) where n is elements, m is buckets
 */

#include <cstdint>
#include <vector>
#include <functional>
#include <atomic>

// Simplified hlist (head-only list) implementation
// Linux kernel uses hlist_head and hlist_node
template<typename T>
struct HListNode {
    T data;
    HListNode* next;
    HListNode* pprev; // Pointer to previous node's next pointer
    
    HListNode(const T& d) : data(d), next(nullptr), pprev(nullptr) {}
};

template<typename T>
struct HListHead {
    HListNode<T>* first;
    
    HListHead() : first(nullptr) {}
};

// RCU read-side critical section marker (simplified)
class RCUReadLock {
public:
    RCUReadLock() {
        // In kernel: rcu_read_lock()
        // Marks beginning of RCU read-side critical section
    }
    
    ~RCUReadLock() {
        // In kernel: rcu_read_unlock()
        // Marks end of RCU read-side critical section
    }
};

// RCU synchronization point (simplified)
void synchronize_rcu() {
    // In kernel: synchronize_rcu()
    // Waits for all readers to exit RCU critical sections
    // This is where memory is actually freed
}

template<typename K, typename V>
class LinuxKernelHashTable {
private:
    struct HashEntry {
        K key;
        V value;
        HListNode<HashEntry>* node;
        
        HashEntry(const K& k, const V& v) : key(k), value(v), node(nullptr) {}
    };
    
    std::vector<HListHead<HashEntry*>> buckets;
    size_t num_buckets;
    size_t num_elements;
    std::atomic<size_t> version; // For RCU versioning
    
    // Hash function (simplified version of hash_min)
    size_t hash_key(const K& key, size_t bits) const {
        std::hash<K> hasher;
        size_t hash_val = hasher(key);
        
        // Simplified hash_32/hash_long logic
        // Linux uses hash_32 for 32-bit values, hash_long for 64-bit
        if (sizeof(hash_val) <= 4) {
            // hash_32 equivalent
            hash_val ^= hash_val >> 16;
            hash_val *= 0x85ebca6b;
            hash_val ^= hash_val >> 13;
            hash_val *= 0xc2b2ae35;
            hash_val ^= hash_val >> 16;
        } else {
            // hash_long equivalent (simplified)
            hash_val ^= hash_val >> 33;
            hash_val *= 0xff51afd7ed558ccdULL;
            hash_val ^= hash_val >> 33;
            hash_val *= 0xc4ceb9fe1a85ec53ULL;
            hash_val ^= hash_val >> 33;
        }
        
        return hash_val & ((1ULL << bits) - 1);
    }
    
    size_t get_bucket_bits() const {
        size_t bits = 0;
        size_t size = num_buckets;
        while (size > 1) {
            bits++;
            size >>= 1;
        }
        return bits;
    }
    
public:
    LinuxKernelHashTable(size_t bits = 4) : num_elements(0), version(0) {
        num_buckets = 1ULL << bits;
        buckets.resize(num_buckets);
    }
    
    ~LinuxKernelHashTable() {
        clear();
    }
    
    // RCU-safe insert (writer)
    void insert_rcu(const K& key, const V& value) {
        HashEntry* entry = new HashEntry(key, value);
        entry->node = new HListNode<HashEntry*>(entry);
        
        size_t bits = get_bucket_bits();
        size_t idx = hash_key(key, bits);
        
        // Add to head of list (RCU-safe)
        HListHead<HashEntry*>& head = buckets[idx];
        entry->node->next = head.first;
        if (head.first) {
            head.first->pprev = &entry->node->next;
        }
        head.first = entry->node;
        entry->node->pprev = &head.first;
        
        // Memory barrier for visibility
        std::atomic_thread_fence(std::memory_order_release);
        
        num_elements++;
        version.fetch_add(1, std::memory_order_relaxed);
    }
    
    // RCU-safe search (reader)
    V* find_rcu(const K& key) {
        RCUReadLock lock; // Begin RCU read-side critical section
        
        size_t bits = get_bucket_bits();
        size_t idx = hash_key(key, bits);
        
        HListHead<HashEntry*>& head = buckets[idx];
        HListNode<HashEntry*>* node = head.first;
        
        while (node) {
            HashEntry* entry = node->data;
            if (entry && entry->key == key) {
                return &entry->value;
            }
            node = node->next;
        }
        
        return nullptr;
    }
    
    // RCU-safe delete (writer)
    bool remove_rcu(const K& key) {
        size_t bits = get_bucket_bits();
        size_t idx = hash_key(key, bits);
        
        HListHead<HashEntry*>& head = buckets[idx];
        HListNode<HashEntry*>* node = head.first;
        
        while (node) {
            HashEntry* entry = node->data;
            if (entry && entry->key == key) {
                // Remove from list
                if (node->next) {
                    node->next->pprev = node->pprev;
                }
                *node->pprev = node->next;
                
                // Memory barrier
                std::atomic_thread_fence(std::memory_order_release);
                
                // Schedule RCU callback to free memory
                // In kernel: call_rcu(&entry->rcu_head, free_entry)
                // Here we'll free immediately after synchronize_rcu()
                synchronize_rcu();
                
                delete entry;
                delete node;
                num_elements--;
                version.fetch_add(1, std::memory_order_relaxed);
                return true;
            }
            node = node->next;
        }
        
        return false;
    }
    
    // Check if key exists (RCU-safe)
    bool contains_rcu(const K& key) {
        return find_rcu(key) != nullptr;
    }
    
    // Get current size (approximate, RCU-safe)
    size_t size() const {
        return num_elements;
    }
    
    // Clear all entries
    void clear() {
        for (size_t i = 0; i < num_buckets; i++) {
            HListNode<HashEntry*>* node = buckets[i].first;
            while (node) {
                HListNode<HashEntry*>* next = node->next;
                if (node->data) {
                    delete node->data;
                }
                delete node;
                node = next;
            }
            buckets[i].first = nullptr;
        }
        num_elements = 0;
    }
    
    // Iterate over all entries (RCU-safe)
    template<typename Func>
    void for_each_rcu(Func func) {
        RCUReadLock lock;
        
        for (size_t i = 0; i < num_buckets; i++) {
            HListNode<HashEntry*>* node = buckets[i].first;
            while (node) {
                HashEntry* entry = node->data;
                if (entry) {
                    func(entry->key, entry->value);
                }
                node = node->next;
            }
        }
    }
};

// Example usage
#include <iostream>
#include <string>

int main() {
    LinuxKernelHashTable<std::string, int> hash_table(4); // 2^4 = 16 buckets
    
    // Insert operations (writers)
    hash_table.insert_rcu("apple", 10);
    hash_table.insert_rcu("banana", 20);
    hash_table.insert_rcu("cherry", 30);
    
    // Search operations (readers, RCU-safe)
    int* value = hash_table.find_rcu("banana");
    if (value) {
        std::cout << "banana: " << *value << std::endl;
    }
    
    // Iterate (RCU-safe)
    hash_table.for_each_rcu([](const std::string& key, int& val) {
        std::cout << key << ": " << val << std::endl;
    });
    
    // Remove operation (writer)
    hash_table.remove_rcu("cherry");
    
    std::cout << "Size: " << hash_table.size() << std::endl;
    
    return 0;
}

