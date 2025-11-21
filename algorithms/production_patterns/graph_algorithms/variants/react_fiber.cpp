/*
 * React Fiber Reconciliation - Graph Traversal with Work Scheduling
 * 
 * Source: https://github.com/facebook/react/blob/main/packages/react-reconciler/src/ReactFiberReconciler.js
 * Repository: facebook/react
 * File: `packages/react-reconciler/src/ReactFiberReconciler.js`
 * 
 * What Makes It Ingenious:
 * - Fiber tree (graph) representation of component tree
 * - Depth-first traversal with work scheduling
 * - Incremental rendering (can pause/resume)
 * - Priority-based work scheduling
 * - Time-slicing for responsive UI
 * 
 * When to Use:
 * - Graph traversal with scheduling
 * - Incremental processing
 * - Priority-based traversal
 * - Need to pause/resume traversal
 * - UI rendering systems
 * 
 * Real-World Usage:
 * - React's reconciliation algorithm
 * - UI rendering engines
 * - Incremental graph processing
 * - Priority-based task scheduling
 * 
 * Time Complexity:
 * - Traversal: O(n) where n is number of nodes
 * - Scheduling: O(log n) for priority queue
 * - Overall: O(n log n) with scheduling
 * 
 * Space Complexity: O(n) for fiber tree
 */

#include <vector>
#include <queue>
#include <functional>
#include <cstdint>

// Fiber node (simplified from React)
struct FiberNode {
    int id;
    int priority; // Lower = higher priority
    std::vector<FiberNode*> children;
    FiberNode* sibling;
    FiberNode* return_node; // Parent
    bool visited;
    
    FiberNode(int i, int p = 0) 
        : id(i)
        , priority(p)
        , sibling(nullptr)
        , return_node(nullptr)
        , visited(false) {}
};

// Priority queue comparator (lower priority number = higher priority)
struct FiberComparator {
    bool operator()(FiberNode* a, FiberNode* b) {
        return a->priority > b->priority; // Min-heap
    }
};

class ReactFiberReconciler {
private:
    FiberNode* root;
    
    // Depth-first traversal (React's reconciliation pattern)
    void reconcile_node(FiberNode* node, std::function<void(FiberNode*)> work) {
        if (!node || node->visited) return;
        
        // Process current node
        work(node);
        node->visited = true;
        
        // Process children (depth-first)
        for (FiberNode* child : node->children) {
            reconcile_node(child, work);
        }
        
        // Process sibling
        if (node->sibling) {
            reconcile_node(node->sibling, work);
        }
    }
    
    // Work scheduling with priority
    void schedule_work(FiberNode* node, 
                      std::priority_queue<FiberNode*, 
                                         std::vector<FiberNode*>, 
                                         FiberComparator>& work_queue) {
        if (!node || node->visited) return;
        
        work_queue.push(node);
        node->visited = true;
        
        // Add children to work queue
        for (FiberNode* child : node->children) {
            schedule_work(child, work_queue);
        }
        
        // Add sibling to work queue
        if (node->sibling) {
            schedule_work(node->sibling, work_queue);
        }
    }
    
public:
    ReactFiberReconciler(FiberNode* r) : root(r) {}
    
    // Depth-first reconciliation (React's pattern)
    void reconcile(std::function<void(FiberNode*)> work) {
        if (!root) return;
        
        // Reset visited flags
        reset_visited(root);
        
        // Start reconciliation
        reconcile_node(root, work);
    }
    
    // Priority-based work scheduling (React's scheduler)
    void reconcile_with_priority(std::function<void(FiberNode*)> work) {
        if (!root) return;
        
        // Reset visited flags
        reset_visited(root);
        
        // Priority queue for work scheduling
        std::priority_queue<FiberNode*, 
                           std::vector<FiberNode*>, 
                           FiberComparator> work_queue;
        
        // Schedule all work
        schedule_work(root, work_queue);
        
        // Process work in priority order
        while (!work_queue.empty()) {
            FiberNode* node = work_queue.top();
            work_queue.pop();
            work(node);
        }
    }
    
    // Incremental reconciliation (time-slicing)
    bool reconcile_incremental(std::function<void(FiberNode*)> work, 
                               int max_work_units) {
        if (!root) return true;
        
        static std::vector<FiberNode*> work_list;
        static size_t current_index = 0;
        
        // Initialize work list on first call
        if (work_list.empty()) {
            reset_visited(root);
            collect_nodes(root, work_list);
            current_index = 0;
        }
        
        // Process up to max_work_units
        int units_processed = 0;
        while (current_index < work_list.size() && units_processed < max_work_units) {
            FiberNode* node = work_list[current_index++];
            work(node);
            units_processed++;
        }
        
        // Return true if all work done
        bool done = (current_index >= work_list.size());
        if (done) {
            work_list.clear();
            current_index = 0;
        }
        
        return done;
    }
    
private:
    void reset_visited(FiberNode* node) {
        if (!node) return;
        node->visited = false;
        for (FiberNode* child : node->children) {
            reset_visited(child);
        }
        if (node->sibling) {
            reset_visited(node->sibling);
        }
    }
    
    void collect_nodes(FiberNode* node, std::vector<FiberNode*>& nodes) {
        if (!node || node->visited) return;
        nodes.push_back(node);
        node->visited = true;
        for (FiberNode* child : node->children) {
            collect_nodes(child, nodes);
        }
        if (node->sibling) {
            collect_nodes(node->sibling, nodes);
        }
    }
};

// Example usage
#include <iostream>

int main() {
    // Create fiber tree
    FiberNode* root = new FiberNode(1, 0);
    FiberNode* child1 = new FiberNode(2, 1);
    FiberNode* child2 = new FiberNode(3, 2);
    FiberNode* grandchild = new FiberNode(4, 1);
    
    root->children.push_back(child1);
    root->children.push_back(child2);
    child1->children.push_back(grandchild);
    child1->sibling = child2;
    
    // Reconciliation
    ReactFiberReconciler reconciler(root);
    
    std::cout << "Depth-first reconciliation:" << std::endl;
    reconciler.reconcile([](FiberNode* node) {
        std::cout << "Processing node " << node->id << std::endl;
    });
    
    std::cout << "\nPriority-based reconciliation:" << std::endl;
    reconciler.reconcile_with_priority([](FiberNode* node) {
        std::cout << "Processing node " << node->id 
                  << " (priority " << node->priority << ")" << std::endl;
    });
    
    return 0;
}

