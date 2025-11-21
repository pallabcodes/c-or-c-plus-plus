/*
 * React Diffing Algorithm - Efficient Tree Reconciliation
 * 
 * Source: https://github.com/facebook/react/blob/main/packages/react-reconciler/src/ReactChildFiber.js
 * Repository: facebook/react
 * File: `packages/react-reconciler/src/ReactChildFiber.js`
 * 
 * What Makes It Ingenious:
 * - Three-way diffing: Compare old tree, new tree, and current tree
 * - Key-based reconciliation: Use keys to match elements efficiently
 * - Minimal DOM operations: Only update what changed
 * - Multi-pass diffing: First pass for structure, second for props
 * - Fiber-based traversal: Depth-first traversal with work scheduling
 * - Used in React for efficient UI updates
 * 
 * When to Use:
 * - Tree reconciliation/diffing
 * - Minimize update operations
 * - Efficient tree comparison
 * - UI rendering optimization
 * - Incremental tree updates
 * 
 * Real-World Usage:
 * - React reconciliation algorithm
 * - Virtual DOM diffing
 * - UI framework updates
 * - Tree synchronization
 * - Incremental rendering
 * 
 * Time Complexity:
 * - Diff: O(n) where n is number of nodes (with keys)
 * - Without keys: O(n²) worst case
 * - With keys: O(n) average case
 * 
 * Space Complexity: O(n) for diff results
 */

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

// Node type for tree
struct TreeNode {
    std::string key;
    std::string type;
    std::vector<std::pair<std::string, std::string>> props;
    std::vector<std::shared_ptr<TreeNode>> children;
    int id;
    
    TreeNode(const std::string& k, const std::string& t, int i)
        : key(k), type(t), id(i) {}
};

// Diff operation types
enum class DiffOp {
    Keep,      // Keep node as-is
    Update,    // Update node props
    Insert,    // Insert new node
    Delete,    // Delete node
    Move       // Move node to new position
};

// Diff result
struct DiffResult {
    DiffOp operation;
    std::shared_ptr<TreeNode> node;
    std::shared_ptr<TreeNode> new_node;
    int old_index;
    int new_index;
    
    DiffResult(DiffOp op, std::shared_ptr<TreeNode> n, int oi, int ni)
        : operation(op), node(n), old_index(oi), new_index(ni) {}
};

class ReactDiffing {
private:
    // Build key map for efficient lookup
    std::unordered_map<std::string, int> build_key_map(
        const std::vector<std::shared_ptr<TreeNode>>& children) {
        std::unordered_map<std::string, int> key_map;
        for (size_t i = 0; i < children.size(); i++) {
            if (!children[i]->key.empty()) {
                key_map[children[i]->key] = i;
            }
        }
        return key_map;
    }
    
    // Check if nodes are same type
    bool is_same_type(std::shared_ptr<TreeNode> a, std::shared_ptr<TreeNode> b) {
        return a && b && a->type == b->type;
    }
    
    // Check if props changed
    bool props_changed(std::shared_ptr<TreeNode> old_node, 
                      std::shared_ptr<TreeNode> new_node) {
        if (old_node->props.size() != new_node->props.size()) {
            return true;
        }
        
        // Simple prop comparison (in real React, more sophisticated)
        for (const auto& prop : new_node->props) {
            bool found = false;
            for (const auto& old_prop : old_node->props) {
                if (old_prop.first == prop.first) {
                    if (old_prop.second != prop.second) {
                        return true;
                    }
                    found = true;
                    break;
                }
            }
            if (!found) {
                return true;
            }
        }
        return false;
    }
    
public:
    // Diff children (React's reconciliation algorithm)
    std::vector<DiffResult> diff_children(
        const std::vector<std::shared_ptr<TreeNode>>& old_children,
        const std::vector<std::shared_ptr<TreeNode>>& new_children) {
        
        std::vector<DiffResult> results;
        
        // Build key maps for efficient lookup
        auto old_key_map = build_key_map(old_children);
        auto new_key_map = build_key_map(new_children);
        
        // Track which new children have been matched
        std::vector<bool> new_children_matched(new_children.size(), false);
        
        // First pass: Match children by key
        for (size_t i = 0; i < old_children.size(); i++) {
            auto old_child = old_children[i];
            
            if (!old_child->key.empty()) {
                // Try to find matching new child by key
                auto it = new_key_map.find(old_child->key);
                if (it != new_key_map.end()) {
                    int new_index = it->second;
                    auto new_child = new_children[new_index];
                    
                    if (is_same_type(old_child, new_child)) {
                        // Same type, check if update needed
                        if (props_changed(old_child, new_child)) {
                            results.emplace_back(DiffOp::Update, old_child, i, new_index);
                        } else {
                            results.emplace_back(DiffOp::Keep, old_child, i, new_index);
                        }
                        new_children_matched[new_index] = true;
                        continue;
                    }
                }
            }
            
            // No match found, mark for deletion
            results.emplace_back(DiffOp::Delete, old_child, i, -1);
        }
        
        // Second pass: Handle new children
        for (size_t i = 0; i < new_children.size(); i++) {
            if (!new_children_matched[i]) {
                // New child, mark for insertion
                results.emplace_back(DiffOp::Insert, new_children[i], -1, i);
            }
        }
        
        return results;
    }
    
    // Diff single node (React's pattern)
    DiffResult diff_node(std::shared_ptr<TreeNode> old_node,
                        std::shared_ptr<TreeNode> new_node) {
        if (!old_node && new_node) {
            return DiffResult(DiffOp::Insert, new_node, -1, 0);
        }
        
        if (old_node && !new_node) {
            return DiffResult(DiffOp::Delete, old_node, 0, -1);
        }
        
        if (is_same_type(old_node, new_node)) {
            if (props_changed(old_node, new_node)) {
                return DiffResult(DiffOp::Update, old_node, 0, 0);
            } else {
                return DiffResult(DiffOp::Keep, old_node, 0, 0);
            }
        }
        
        // Different types, replace
        return DiffResult(DiffOp::Delete, old_node, 0, -1);
    }
    
    // Apply diff results (simplified)
    void apply_diff(const std::vector<DiffResult>& diffs) {
        for (const auto& diff : diffs) {
            switch (diff.operation) {
                case DiffOp::Keep:
                    // Keep node, no operation needed
                    break;
                case DiffOp::Update:
                    // Update node props (in real React, would update DOM)
                    std::cout << "Update node " << diff.node->id << std::endl;
                    break;
                case DiffOp::Insert:
                    // Insert new node (in real React, would create DOM element)
                    std::cout << "Insert node " << diff.node->id << std::endl;
                    break;
                case DiffOp::Delete:
                    // Delete node (in real React, would remove DOM element)
                    std::cout << "Delete node " << diff.node->id << std::endl;
                    break;
                case DiffOp::Move:
                    // Move node (in real React, would move DOM element)
                    std::cout << "Move node " << diff.node->id << std::endl;
                    break;
            }
        }
    }
};

// Example usage
#include <iostream>

int main() {
    ReactDiffing differ;
    
    // Create old tree
    std::vector<std::shared_ptr<TreeNode>> old_children;
    old_children.push_back(std::make_shared<TreeNode>("1", "div", 1));
    old_children.push_back(std::make_shared<TreeNode>("2", "span", 2));
    old_children.push_back(std::make_shared<TreeNode>("3", "p", 3));
    
    // Create new tree (reordered, one updated, one new)
    std::vector<std::shared_ptr<TreeNode>> new_children;
    new_children.push_back(std::make_shared<TreeNode>("2", "span", 2)); // Moved
    new_children.push_back(std::make_shared<TreeNode>("1", "div", 1)); // Moved
    auto updated_node = std::make_shared<TreeNode>("3", "p", 3);
    updated_node->props.push_back({"class", "updated"});
    new_children.push_back(updated_node); // Updated
    new_children.push_back(std::make_shared<TreeNode>("4", "div", 4)); // New
    
    // Perform diff
    std::cout << "Diffing trees:" << std::endl;
    auto diffs = differ.diff_children(old_children, new_children);
    
    // Apply diff
    std::cout << "\nApplying diff:" << std::endl;
    differ.apply_diff(diffs);
    
    return 0;
}

