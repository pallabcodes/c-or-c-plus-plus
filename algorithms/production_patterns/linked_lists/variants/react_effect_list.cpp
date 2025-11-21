/*
 * React Effect List - Linear Linked List for Side Effects
 * 
 * Source: https://github.com/facebook/react/blob/main/packages/react-reconciler/src/ReactFiberCommitWork.js
 * Repository: facebook/react
 * File: `packages/react-reconciler/src/ReactFiberCommitWork.js`
 * 
 * What Makes It Ingenious:
 * - Linear linked list of only nodes that need side effects (DOM mutations, etc.)
 * - Uses nextEffect pointer to link effectful nodes
 * - Skips nodes without side effects during commit phase
 * - Built during render phase, consumed during commit phase
 * - O(n) traversal where n is only effectful nodes (not all nodes)
 * - Used in React for efficient DOM updates
 * 
 * When to Use:
 * - Need to process only subset of nodes (those with side effects)
 * - Separate render phase from commit phase
 * - Efficient traversal of filtered nodes
 * - Skip nodes without work to do
 * - Batch operations on filtered list
 * 
 * Real-World Usage:
 * - React commit phase (DOM mutations)
 * - Effect processing (useEffect hooks)
 * - Batch updates
 * - Efficient rendering pipelines
 * 
 * Time Complexity:
 * - Build effect list: O(n) where n is all nodes
 * - Traverse effect list: O(m) where m is effectful nodes (m <= n)
 * - Commit effects: O(m)
 * 
 * Space Complexity: O(m) for effect list (only effectful nodes)
 */

#include <cstdint>
#include <functional>

// Effect tags (simplified from React)
enum EffectTag {
    NoEffect = 0,
    Placement = 1 << 0,      // Insert node
    Update = 1 << 1,          // Update node
    Deletion = 1 << 2,       // Delete node
    ContentReset = 1 << 3,   // Reset content
    Callback = 1 << 4,       // Callback effect
    Ref = 1 << 5,            // Ref effect
    Snapshot = 1 << 6,       // Snapshot effect
    Passive = 1 << 7         // Passive effect (useEffect)
};

// Fiber node with effect list support
struct EffectFiberNode {
    int id;
    void* element;
    
    // Tree pointers
    EffectFiberNode* child;
    EffectFiberNode* sibling;
    EffectFiberNode* return_node;
    
    // Effect list pointer (links only effectful nodes)
    EffectFiberNode* next_effect;
    
    // Effect flags
    int effect_tag;
    
    EffectFiberNode(int i, void* elem = nullptr)
        : id(i)
        , element(elem)
        , child(nullptr)
        , sibling(nullptr)
        , return_node(nullptr)
        , next_effect(nullptr)
        , effect_tag(NoEffect) {}
    
    // Check if node has effects
    bool has_effects() const {
        return effect_tag != NoEffect;
    }
};

class ReactEffectList {
private:
    EffectFiberNode* root_;
    EffectFiberNode* first_effect_;  // Head of effect list
    EffectFiberNode* last_effect_;   // Tail of effect list
    
    // Build effect list during traversal (React's pattern)
    void build_effect_list(EffectFiberNode* node) {
        if (node == nullptr) return;
        
        // Traverse children
        EffectFiberNode* child = node->child;
        while (child != nullptr) {
            build_effect_list(child);
            child = child->sibling;
        }
        
        // Add to effect list if has effects
        if (node->has_effects()) {
            if (first_effect_ == nullptr) {
                first_effect_ = last_effect_ = node;
            } else {
                last_effect_->next_effect = node;
                last_effect_ = node;
            }
        }
    }
    
    // Clear effect list
    void clear_effect_list() {
        EffectFiberNode* current = first_effect_;
        while (current != nullptr) {
            EffectFiberNode* next = current->next_effect;
            current->next_effect = nullptr;
            current = next;
        }
        first_effect_ = last_effect_ = nullptr;
    }
    
public:
    ReactEffectList() 
        : root_(nullptr)
        , first_effect_(nullptr)
        , last_effect_(nullptr) {}
    
    // Set root
    void set_root(EffectFiberNode* root) {
        root_ = root;
    }
    
    // Build effect list from tree (called during render phase)
    void build_effects() {
        clear_effect_list();
        if (root_ != nullptr) {
            build_effect_list(root_);
        }
    }
    
    // Traverse effect list (only effectful nodes!)
    void traverse_effects(std::function<void(EffectFiberNode*)> process) {
        EffectFiberNode* current = first_effect_;
        while (current != nullptr) {
            process(current);
            current = current->next_effect;
        }
    }
    
    // Commit all effects (React's commit phase)
    void commit_effects() {
        traverse_effects([](EffectFiberNode* node) {
            // Process effect based on tag
            if (node->effect_tag & Placement) {
                // Insert node into DOM
                std::cout << "  Placing node " << node->id << std::endl;
            }
            if (node->effect_tag & Update) {
                // Update node in DOM
                std::cout << "  Updating node " << node->id << std::endl;
            }
            if (node->effect_tag & Deletion) {
                // Delete node from DOM
                std::cout << "  Deleting node " << node->id << std::endl;
            }
            if (node->effect_tag & Passive) {
                // Run passive effect (useEffect)
                std::cout << "  Running passive effect for node " << node->id << std::endl;
            }
        });
    }
    
    // Get effect list head
    EffectFiberNode* get_first_effect() const {
        return first_effect_;
    }
    
    // Check if has effects
    bool has_effects() const {
        return first_effect_ != nullptr;
    }
    
    // Count effectful nodes
    int count_effects() const {
        int count = 0;
        EffectFiberNode* current = first_effect_;
        while (current != nullptr) {
            count++;
            current = current->next_effect;
        }
        return count;
    }
};

// Example usage
#include <iostream>

int main() {
    ReactEffectList effect_list;
    
    // Create fiber tree
    EffectFiberNode* root = new EffectFiberNode(1);
    EffectFiberNode* child1 = new EffectFiberNode(2);
    EffectFiberNode* child2 = new EffectFiberNode(3);
    EffectFiberNode* child3 = new EffectFiberNode(4);
    
    root->child = child1;
    child1->sibling = child2;
    child2->sibling = child3;
    
    child1->return_node = root;
    child2->return_node = root;
    child3->return_node = root;
    
    // Mark some nodes with effects
    child1->effect_tag = Placement | Update;
    child3->effect_tag = Passive;
    // child2 has no effects
    
    effect_list.set_root(root);
    
    // Build effect list (only effectful nodes)
    std::cout << "Building effect list:" << std::endl;
    effect_list.build_effects();
    
    std::cout << "Effect list contains " << effect_list.count_effects() 
              << " nodes (out of 4 total)" << std::endl;
    
    // Traverse effect list (only effectful nodes!)
    std::cout << "\nTraversing effect list:" << std::endl;
    effect_list.traverse_effects([](EffectFiberNode* node) {
        std::cout << "Effectful node " << node->id << std::endl;
    });
    
    // Commit effects
    std::cout << "\nCommitting effects:" << std::endl;
    effect_list.commit_effects();
    
    return 0;
}

