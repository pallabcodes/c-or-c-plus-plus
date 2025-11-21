/*
 * Linux Kernel Red-Black Tree - Generic Intrusive Implementation
 * 
 * Source: `/Users/picon/Learning/c-or-c-plus-plus/linux/include/linux/rbtree.h`
 * File: `linux/include/linux/rbtree.h`
 * Implementation: `linux/lib/rbtree.c`
 * 
 * What Makes It Ingenious:
 * - Intrusive data structures (rb_node embedded in containing structure)
 * - Parent pointer + color packed in single field (__rb_parent_color)
 * - Generic type-agnostic implementation (no callbacks for performance)
 * - RCU support for lock-free reads
 * - Leftmost caching for O(1) minimum
 * - Memory-efficient (no extra allocations)
 * 
 * When to Use:
 * - Need generic tree implementation
 * - Memory efficiency critical
 * - Kernel-level code
 * - High-performance systems
 * - Need O(log n) guaranteed operations
 * 
 * Real-World Usage:
 * - Linux kernel process scheduler
 * - Linux kernel virtual memory system
 * - Linux kernel I/O schedulers
 * - High-performance data structures
 * 
 * Time Complexity:
 * - Insert: O(log n)
 * - Search: O(log n)
 * - Delete: O(log n)
 * - Minimum: O(1) with caching, O(log n) without
 * 
 * Space Complexity: O(n) where n is number of nodes
 */

#include <cstdint>
#include <cstddef>

// Color constants
enum rb_color {
    RB_RED = 0,
    RB_BLACK = 1
};

// Red-black tree node (intrusive)
struct rb_node {
    unsigned long __rb_parent_color; // Parent pointer + color (LSB)
    struct rb_node* rb_right;
    struct rb_node* rb_left;
    
    rb_node() : __rb_parent_color(0), rb_right(nullptr), rb_left(nullptr) {}
};

// Red-black tree root
struct rb_root {
    struct rb_node* rb_node;
    
    rb_root() : rb_node(nullptr) {}
};

// Helper macros (simplified from Linux kernel)
#define rb_parent(r) ((struct rb_node*)((r)->__rb_parent_color & ~3))
#define rb_color(r) ((r)->__rb_parent_color & 1)
#define rb_is_red(r) (!rb_color(r))
#define rb_is_black(r) (rb_color(r))
#define rb_set_red(r) ((r)->__rb_parent_color &= ~1)
#define rb_set_black(r) ((r)->__rb_parent_color |= 1)
#define rb_set_parent(r, p) ((r)->__rb_parent_color = (unsigned long)(p) | rb_color(r))
#define rb_set_parent_color(r, p, c) ((r)->__rb_parent_color = (unsigned long)(p) | (c))

// Generic red-black tree implementation
template<typename T>
class LinuxRBTree {
private:
    rb_root root;
    
    // Get rb_node from containing structure (container_of pattern)
    static rb_node* get_rb_node(T* item) {
        // In real implementation, this uses offsetof and container_of
        // Simplified here - assumes T has rb_node as first member
        return reinterpret_cast<rb_node*>(item);
    }
    
    static T* get_item(rb_node* node) {
        return reinterpret_cast<T*>(node);
    }
    
    // Left rotation
    void rb_rotate_left(rb_node* node, rb_node** root_ptr) {
        rb_node* right = node->rb_right;
        rb_node* parent = rb_parent(node);
        
        node->rb_right = right->rb_left;
        if (right->rb_left) {
            rb_set_parent(right->rb_left, node);
        }
        
        rb_set_parent(right, parent);
        if (parent) {
            if (node == parent->rb_left) {
                parent->rb_left = right;
            } else {
                parent->rb_right = right;
            }
        } else {
            *root_ptr = right;
        }
        
        right->rb_left = node;
        rb_set_parent(node, right);
    }
    
    // Right rotation
    void rb_rotate_right(rb_node* node, rb_node** root_ptr) {
        rb_node* left = node->rb_left;
        rb_node* parent = rb_parent(node);
        
        node->rb_left = left->rb_right;
        if (left->rb_right) {
            rb_set_parent(left->rb_right, node);
        }
        
        rb_set_parent(left, parent);
        if (parent) {
            if (node == parent->rb_right) {
                parent->rb_right = left;
            } else {
                parent->rb_left = left;
            }
        } else {
            *root_ptr = left;
        }
        
        left->rb_right = node;
        rb_set_parent(node, left);
    }
    
    // Fix up after insertion (simplified - full version has 3 cases)
    void rb_insert_fixup(rb_node* node, rb_node** root_ptr) {
        rb_node* parent;
        
        while ((parent = rb_parent(node)) && rb_is_red(parent)) {
            rb_node* gparent = rb_parent(parent);
            
            if (parent == gparent->rb_left) {
                rb_node* uncle = gparent->rb_right;
                
                if (uncle && rb_is_red(uncle)) {
                    // Case 1: Uncle is red - color flip
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
                
                if (node == parent->rb_right) {
                    // Case 2: Node is right child - left rotate
                    rb_rotate_left(parent, root_ptr);
                    rb_node* tmp = parent;
                    parent = node;
                    node = tmp;
                }
                
                // Case 3: Node is left child - right rotate
                rb_set_black(parent);
                rb_set_red(gparent);
                rb_rotate_right(gparent, root_ptr);
            } else {
                // Symmetric case (parent is right child)
                rb_node* uncle = gparent->rb_left;
                
                if (uncle && rb_is_red(uncle)) {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
                
                if (node == parent->rb_left) {
                    rb_rotate_right(parent, root_ptr);
                    rb_node* tmp = parent;
                    parent = node;
                    node = tmp;
                }
                
                rb_set_black(parent);
                rb_set_red(gparent);
                rb_rotate_left(gparent, root_ptr);
            }
        }
        
        rb_set_black(*root_ptr);
    }
    
    // Comparator function type
    typedef int (*compare_func_t)(const T* a, const T* b);
    
    compare_func_t compare;
    
public:
    LinuxRBTree(compare_func_t cmp) : compare(cmp) {}
    
    // Insert node into tree
    void insert(T* item) {
        rb_node* new_node = get_rb_node(item);
        rb_node* parent = nullptr;
        rb_node** link = &root.rb_node;
        
        // Find insertion point
        while (*link) {
            parent = *link;
            T* parent_item = get_item(parent);
            int cmp_result = compare(item, parent_item);
            
            if (cmp_result < 0) {
                link = &parent->rb_left;
            } else {
                link = &parent->rb_right;
            }
        }
        
        // Insert node
        rb_set_parent_color(new_node, parent, RB_RED);
        new_node->rb_left = nullptr;
        new_node->rb_right = nullptr;
        *link = new_node;
        
        // Fix up red-black properties
        rb_insert_fixup(new_node, &root.rb_node);
    }
    
    // Find node in tree
    T* find(const T* key) {
        rb_node* node = root.rb_node;
        
        while (node) {
            T* node_item = get_item(node);
            int cmp_result = compare(key, node_item);
            
            if (cmp_result < 0) {
                node = node->rb_left;
            } else if (cmp_result > 0) {
                node = node->rb_right;
            } else {
                return node_item;
            }
        }
        
        return nullptr;
    }
    
    // Find minimum node
    T* find_min() {
        rb_node* node = root.rb_node;
        if (!node) return nullptr;
        
        while (node->rb_left) {
            node = node->rb_left;
        }
        
        return get_item(node);
    }
    
    // Find maximum node
    T* find_max() {
        rb_node* node = root.rb_node;
        if (!node) return nullptr;
        
        while (node->rb_right) {
            node = node->rb_right;
        }
        
        return get_item(node);
    }
    
    // Check if tree is empty
    bool empty() const {
        return root.rb_node == nullptr;
    }
};

// Example usage
#include <iostream>
#include <string>

struct MyData {
    rb_node node; // Must be first member for container_of to work
    int key;
    std::string value;
    
    MyData(int k, const std::string& v) : key(k), value(v) {}
};

int compare_my_data(const MyData* a, const MyData* b) {
    if (a->key < b->key) return -1;
    if (a->key > b->key) return 1;
    return 0;
}

int main() {
    LinuxRBTree<MyData> tree(compare_my_data);
    
    MyData item1(10, "ten");
    MyData item2(5, "five");
    MyData item3(15, "fifteen");
    MyData item4(3, "three");
    
    tree.insert(&item1);
    tree.insert(&item2);
    tree.insert(&item3);
    tree.insert(&item4);
    
    // Search
    MyData key(5, "");
    MyData* found = tree.find(&key);
    if (found) {
        std::cout << "Found: " << found->value << std::endl;
    }
    
    // Find minimum
    MyData* min = tree.find_min();
    if (min) {
        std::cout << "Minimum: " << min->value << std::endl;
    }
    
    return 0;
}

