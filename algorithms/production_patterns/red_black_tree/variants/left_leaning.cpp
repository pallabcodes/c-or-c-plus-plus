/*
 * Left-Leaning Red-Black Tree - Simplified Red-Black Tree
 * 
 * Source: "Left-Leaning Red-Black Trees" by Robert Sedgewick
 * Paper: Various papers and presentations by Robert Sedgewick
 * 
 * What Makes It Ingenious:
 * - Simplified implementation (fewer cases to handle)
 * - Red nodes can only be left children (left-leaning invariant)
 * - Reduces insertion cases from 3 to 2
 * - Easier to implement correctly
 * - Same O(log n) guarantees as standard red-black trees
 * 
 * When to Use:
 * - Need red-black tree but want simpler implementation
 * - Educational purposes (easier to understand)
 * - When correctness is more important than micro-optimizations
 * - Standard use cases for balanced BST
 * 
 * Real-World Usage:
 * - Java's TreeMap (uses similar approach)
 * - Simplified tree implementations
 * - Educational implementations
 * - When code clarity is important
 * 
 * Time Complexity:
 * - Insert: O(log n)
 * - Search: O(log n)
 * - Delete: O(log n)
 * 
 * Space Complexity: O(n) where n is number of nodes
 */

#include <cstdint>

enum Color {
    RED = 0,
    BLACK = 1
};

template<typename K, typename V>
class LeftLeaningRBTree {
private:
    struct Node {
        K key;
        V value;
        Node* left;
        Node* right;
        Color color;
        
        Node(const K& k, const V& v, Color c = RED)
            : key(k), value(v), left(nullptr), right(nullptr), color(c) {}
    };
    
    Node* root;
    
    // Check if node is red
    bool is_red(Node* node) const {
        if (node == nullptr) return false;
        return node->color == RED;
    }
    
    // Rotate left
    Node* rotate_left(Node* node) {
        Node* right = node->right;
        node->right = right->left;
        right->left = node;
        right->color = node->color;
        node->color = RED;
        return right;
    }
    
    // Rotate right
    Node* rotate_right(Node* node) {
        Node* left = node->left;
        node->left = left->right;
        left->right = node;
        left->color = node->color;
        node->color = RED;
        return left;
    }
    
    // Flip colors (make children black, parent red)
    void flip_colors(Node* node) {
        node->color = RED;
        if (node->left) node->left->color = BLACK;
        if (node->right) node->right->color = BLACK;
    }
    
    // Fix up after insertion (simplified - only 2 cases)
    Node* fix_up(Node* node) {
        // Case 1: Right child is red, left child is black -> rotate left
        if (is_red(node->right) && !is_red(node->left)) {
            node = rotate_left(node);
        }
        
        // Case 2: Left child and left grandchild are red -> rotate right
        if (is_red(node->left) && is_red(node->left->left)) {
            node = rotate_right(node);
        }
        
        // Case 3: Both children are red -> flip colors
        if (is_red(node->left) && is_red(node->right)) {
            flip_colors(node);
        }
        
        return node;
    }
    
    // Insert helper
    Node* insert_node(Node* node, const K& key, const V& value) {
        if (node == nullptr) {
            return new Node(key, value, RED);
        }
        
        if (key < node->key) {
            node->left = insert_node(node->left, key, value);
        } else if (key > node->key) {
            node->right = insert_node(node->right, key, value);
        } else {
            node->value = value; // Update existing
        }
        
        return fix_up(node);
    }
    
    // Search helper
    V* search_node(Node* node, const K& key) {
        if (node == nullptr) {
            return nullptr;
        }
        
        if (key < node->key) {
            return search_node(node->left, key);
        } else if (key > node->key) {
            return search_node(node->right, key);
        } else {
            return &node->value;
        }
    }
    
public:
    LeftLeaningRBTree() : root(nullptr) {}
    
    // Insert key-value pair
    void insert(const K& key, const V& value) {
        root = insert_node(root, key, value);
        root->color = BLACK; // Root is always black
    }
    
    // Search for key
    V* search(const K& key) {
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
    LeftLeaningRBTree<int, std::string> tree;
    
    // Insert operations
    tree.insert(10, "ten");
    tree.insert(5, "five");
    tree.insert(15, "fifteen");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    // Search operations
    std::string* value = tree.search(15);
    if (value) {
        std::cout << "Found: " << *value << std::endl;
    }
    
    std::cout << "Contains 5: " << (tree.contains(5) ? "yes" : "no") << std::endl;
    
    return 0;
}

