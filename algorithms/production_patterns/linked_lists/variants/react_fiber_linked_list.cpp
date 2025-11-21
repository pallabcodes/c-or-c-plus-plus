/*
 * React Fiber Linked List - Multi-Pointer Tree Traversal
 * 
 * Source: https://github.com/facebook/react/blob/main/packages/react-reconciler/src/ReactFiber.js
 * Repository: facebook/react
 * File: `packages/react-reconciler/src/ReactFiber.js`
 * 
 * What Makes It Ingenious:
 * - Multi-pointer structure: child, sibling, return (parent) pointers
 * - Enables depth-first traversal WITHOUT call stack (iterative, not recursive)
 * - Can pause/resume traversal at any point (critical for concurrent rendering)
 * - Tree structure represented as linked list for efficient traversal
 * - Work-in-progress (WIP) tree alongside current tree
 * - Used in React Fiber architecture for incremental rendering
 * 
 * When to Use:
 * - Tree traversal without recursion (avoid stack overflow)
 * - Need to pause/resume traversal (incremental processing)
 * - Tree structure with efficient traversal
 * - Component tree representation
 * - Work scheduling on tree nodes
 * 
 * Real-World Usage:
 * - React Fiber reconciliation
 * - Component tree traversal
 * - Incremental rendering systems
 * - Work scheduling on hierarchical data
 * 
 * Time Complexity:
 * - Traversal: O(n) where n is number of nodes
 * - Insert/Remove: O(1) at current position
 * - Find: O(n) worst case
 * 
 * Space Complexity: O(n) for fiber tree
 */

#include <cstdint>
#include <functional>

// Fiber node structure (simplified from React)
struct FiberNode {
    int id;
    void* element;  // Component/element data
    
    // Linked list pointers for tree traversal
    FiberNode* child;      // First child
    FiberNode* sibling;    // Next sibling
    FiberNode* return_node; // Parent (return to parent after processing)
    
    // Work-in-progress tree
    FiberNode* alternate;  // Points to alternate tree (current/WIP)
    
    // Effect flags
    int effect_tag;
    
    FiberNode(int i, void* elem = nullptr)
        : id(i)
        , element(elem)
        , child(nullptr)
        , sibling(nullptr)
        , return_node(nullptr)
        , alternate(nullptr)
        , effect_tag(0) {}
};

class ReactFiberLinkedList {
private:
    FiberNode* root_;
    FiberNode* work_in_progress_root_;
    
    // Depth-first traversal WITHOUT recursion (React's pattern)
    // Uses return pointer to go back up the tree
    void traverse_depth_first_iterative(
        FiberNode* node,
        std::function<void(FiberNode*)> visit) {
        
        FiberNode* current = node;
        FiberNode* next = nullptr;
        
        while (current != nullptr) {
            // Visit current node
            visit(current);
            
            // Process children first (depth-first)
            if (current->child != nullptr) {
                next = current->child;
            } else {
                // No children, go to sibling
                if (current->sibling != nullptr) {
                    next = current->sibling;
                } else {
                    // No sibling, go back up using return pointer
                    FiberNode* parent = current->return_node;
                    while (parent != nullptr && parent->sibling == nullptr) {
                        parent = parent->return_node;
                    }
                    if (parent != nullptr) {
                        next = parent->sibling;
                    } else {
                        next = nullptr; // Done
                    }
                }
            }
            
            current = next;
        }
    }
    
    // Clone fiber tree (React's pattern for work-in-progress)
    FiberNode* clone_fiber(FiberNode* node, FiberNode* return_node) {
        if (node == nullptr) return nullptr;
        
        FiberNode* cloned = new FiberNode(node->id, node->element);
        cloned->return_node = return_node;
        cloned->alternate = node;  // Link to original
        
        // Clone children recursively
        if (node->child != nullptr) {
            cloned->child = clone_fiber(node->child, cloned);
        }
        
        // Clone siblings
        if (node->sibling != nullptr) {
            cloned->sibling = clone_fiber(node->sibling, return_node);
        }
        
        return cloned;
    }
    
public:
    ReactFiberLinkedList() : root_(nullptr), work_in_progress_root_(nullptr) {}
    
    // Set root fiber
    void set_root(FiberNode* root) {
        root_ = root;
    }
    
    // Begin work (create work-in-progress tree)
    void begin_work() {
        if (root_ != nullptr) {
            work_in_progress_root_ = clone_fiber(root_, nullptr);
        }
    }
    
    // Commit work (replace current tree with WIP tree)
    void commit_work() {
        if (work_in_progress_root_ != nullptr) {
            root_ = work_in_progress_root_;
            work_in_progress_root_ = nullptr;
        }
    }
    
    // Traverse tree depth-first (iterative, no recursion)
    void traverse_depth_first(std::function<void(FiberNode*)> visit) {
        if (root_ != nullptr) {
            traverse_depth_first_iterative(root_, visit);
        }
    }
    
    // Traverse work-in-progress tree
    void traverse_wip(std::function<void(FiberNode*)> visit) {
        if (work_in_progress_root_ != nullptr) {
            traverse_depth_first_iterative(work_in_progress_root_, visit);
        }
    }
    
    // Find node by ID (using traversal)
    FiberNode* find_node(int id) {
        FiberNode* result = nullptr;
        traverse_depth_first([&](FiberNode* node) {
            if (node->id == id) {
                result = node;
            }
        });
        return result;
    }
    
    // Insert child (O(1) at current position)
    void insert_child(FiberNode* parent, FiberNode* new_child) {
        if (parent == nullptr || new_child == nullptr) return;
        
        new_child->return_node = parent;
        
        if (parent->child == nullptr) {
            parent->child = new_child;
        } else {
            // Insert at end of children list
            FiberNode* last_child = parent->child;
            while (last_child->sibling != nullptr) {
                last_child = last_child->sibling;
            }
            last_child->sibling = new_child;
        }
    }
    
    // Remove node (O(1) with parent pointer)
    void remove_node(FiberNode* node) {
        if (node == nullptr || node->return_node == nullptr) return;
        
        FiberNode* parent = node->return_node;
        
        if (parent->child == node) {
            // First child
            parent->child = node->sibling;
        } else {
            // Find previous sibling
            FiberNode* prev = parent->child;
            while (prev != nullptr && prev->sibling != node) {
                prev = prev->sibling;
            }
            if (prev != nullptr) {
                prev->sibling = node->sibling;
            }
        }
        
        node->sibling = nullptr;
        node->return_node = nullptr;
    }
};

// Example usage
#include <iostream>

int main() {
    ReactFiberLinkedList fiber_list;
    
    // Create fiber tree
    FiberNode* root = new FiberNode(1);
    FiberNode* child1 = new FiberNode(2);
    FiberNode* child2 = new FiberNode(3);
    FiberNode* grandchild = new FiberNode(4);
    
    root->child = child1;
    child1->sibling = child2;
    child1->return_node = root;
    child2->return_node = root;
    
    child1->child = grandchild;
    grandchild->return_node = child1;
    
    fiber_list.set_root(root);
    
    // Traverse depth-first (iterative, no recursion!)
    std::cout << "Depth-first traversal (iterative):" << std::endl;
    fiber_list.traverse_depth_first([](FiberNode* node) {
        std::cout << "Visiting node " << node->id << std::endl;
    });
    
    // Begin work (create WIP tree)
    std::cout << "\nCreating work-in-progress tree:" << std::endl;
    fiber_list.begin_work();
    
    // Traverse WIP tree
    std::cout << "Traversing WIP tree:" << std::endl;
    fiber_list.traverse_wip([](FiberNode* node) {
        std::cout << "WIP node " << node->id << std::endl;
    });
    
    // Commit work
    fiber_list.commit_work();
    
    return 0;
}

