/*
 * PostgreSQL B-Tree - Disk-Based with Concurrency Control
 * 
 * Source: https://github.com/postgres/postgres/blob/master/src/backend/access/nbtree/
 * Repository: postgres/postgres
 * Directory: `src/backend/access/nbtree/`
 * 
 * What Makes It Ingenious:
 * - Disk-based structure (pages instead of nodes)
 * - MVCC (Multi-Version Concurrency Control) for concurrency
 * - Split/merge operations for maintaining balance
 * - Page management for efficient disk I/O
 * - High fan-out (many keys per node) for shallow trees
 * 
 * When to Use:
 * - Large datasets that don't fit in memory
 * - Database indexes
 * - Disk-based storage systems
 * - Need concurrent access
 * - Range queries important
 * 
 * Real-World Usage:
 * - PostgreSQL B-tree indexes (default index type)
 * - Database storage engines
 * - File systems
 * - Large-scale data storage
 * 
 * Time Complexity:
 * - Insert: O(log n) where n is number of keys
 * - Search: O(log n)
 * - Delete: O(log n)
 * - Range Query: O(log n + k) where k is result size
 * 
 * Space Complexity: O(n) where n is number of keys
 * 
 * Note: This is a simplified in-memory version focusing on core algorithm.
 * Real PostgreSQL B-tree is disk-based with pages, buffers, and MVCC.
 */

#include <vector>
#include <algorithm>
#include <cstdint>

template<typename K, typename V>
class PostgreSQLBTree {
private:
    static constexpr size_t MIN_KEYS = 1; // Minimum keys per node (for order 3)
    static constexpr size_t MAX_KEYS = 5; // Maximum keys per node (order 6)
    
    struct BTreeNode {
        bool is_leaf;
        size_t num_keys;
        std::vector<K> keys;
        std::vector<V> values;
        std::vector<BTreeNode*> children;
        
        BTreeNode(bool leaf = true) 
            : is_leaf(leaf)
            , num_keys(0) {
            keys.resize(MAX_KEYS);
            values.resize(MAX_KEYS);
            if (!is_leaf) {
                children.resize(MAX_KEYS + 1, nullptr);
            }
        }
    };
    
    BTreeNode* root;
    size_t order; // B-tree order (max children per node)
    
    // Split full node
    void split_child(BTreeNode* parent, size_t index, BTreeNode* child) {
        BTreeNode* new_node = new BTreeNode(child->is_leaf);
        new_node->num_keys = MIN_KEYS;
        
        // Move half keys to new node
        for (size_t i = 0; i < MIN_KEYS; i++) {
            new_node->keys[i] = child->keys[i + MIN_KEYS + 1];
            new_node->values[i] = child->values[i + MIN_KEYS + 1];
        }
        
        if (!child->is_leaf) {
            for (size_t i = 0; i <= MIN_KEYS; i++) {
                new_node->children[i] = child->children[i + MIN_KEYS + 1];
            }
        }
        
        child->num_keys = MIN_KEYS;
        
        // Move parent keys to make room
        for (size_t i = parent->num_keys; i > index; i--) {
            parent->keys[i] = parent->keys[i - 1];
            parent->values[i] = parent->values[i - 1];
        }
        
        if (!parent->is_leaf) {
            for (size_t i = parent->num_keys + 1; i > index + 1; i--) {
                parent->children[i] = parent->children[i - 1];
            }
        }
        
        // Insert middle key into parent
        parent->keys[index] = child->keys[MIN_KEYS];
        parent->values[index] = child->values[MIN_KEYS];
        parent->children[index + 1] = new_node;
        parent->num_keys++;
    }
    
    // Insert into non-full node
    void insert_non_full(BTreeNode* node, const K& key, const V& value) {
        size_t i = node->num_keys - 1;
        
        if (node->is_leaf) {
            // Insert into leaf
            while (i >= 0 && key < node->keys[i]) {
                node->keys[i + 1] = node->keys[i];
                node->values[i + 1] = node->values[i];
                i--;
            }
            node->keys[i + 1] = key;
            node->values[i + 1] = value;
            node->num_keys++;
        } else {
            // Find child to insert into
            while (i >= 0 && key < node->keys[i]) {
                i--;
            }
            i++;
            
            // Split if child is full
            if (node->children[i]->num_keys == MAX_KEYS) {
                split_child(node, i, node->children[i]);
                if (key > node->keys[i]) {
                    i++;
                }
            }
            
            insert_non_full(node->children[i], key, value);
        }
    }
    
    // Search in subtree
    V* search_node(BTreeNode* node, const K& key) {
        size_t i = 0;
        while (i < node->num_keys && key > node->keys[i]) {
            i++;
        }
        
        if (i < node->num_keys && key == node->keys[i]) {
            return &node->values[i];
        }
        
        if (node->is_leaf) {
            return nullptr;
        }
        
        return search_node(node->children[i], key);
    }
    
public:
    PostgreSQLBTree() : root(nullptr), order(MAX_KEYS + 1) {}
    
    ~PostgreSQLBTree() {
        // Cleanup would require recursive deletion
        // Simplified for demonstration
    }
    
    // Insert key-value pair
    void insert(const K& key, const V& value) {
        if (root == nullptr) {
            root = new BTreeNode(true);
            root->keys[0] = key;
            root->values[0] = value;
            root->num_keys = 1;
            return;
        }
        
        // If root is full, split it
        if (root->num_keys == MAX_KEYS) {
            BTreeNode* new_root = new BTreeNode(false);
            new_root->children[0] = root;
            split_child(new_root, 0, root);
            
            size_t i = 0;
            if (key > new_root->keys[0]) {
                i++;
            }
            insert_non_full(new_root->children[i], key, value);
            root = new_root;
        } else {
            insert_non_full(root, key, value);
        }
    }
    
    // Search for key
    V* search(const K& key) {
        if (root == nullptr) {
            return nullptr;
        }
        return search_node(root, key);
    }
    
    // Check if key exists
    bool contains(const K& key) {
        return search(key) != nullptr;
    }
    
    // Check if tree is empty
    bool empty() const {
        return root == nullptr;
    }
};

// Example usage
#include <iostream>
#include <string>

int main() {
    PostgreSQLBTree<int, std::string> btree;
    
    // Insert operations
    btree.insert(10, "ten");
    btree.insert(20, "twenty");
    btree.insert(5, "five");
    btree.insert(15, "fifteen");
    btree.insert(25, "twenty-five");
    
    // Search operations
    std::string* value = btree.search(15);
    if (value) {
        std::cout << "Found: " << *value << std::endl;
    }
    
    std::cout << "Contains 20: " << (btree.contains(20) ? "yes" : "no") << std::endl;
    
    return 0;
}

