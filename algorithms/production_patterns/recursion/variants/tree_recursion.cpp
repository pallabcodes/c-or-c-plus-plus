/*
 * Tree Recursion Patterns
 * 
 * Source: Various production codebases and algorithms
 * Patterns: Tree traversal, divide and conquer, memoization
 * 
 * What Makes It Ingenious:
 * - Multiple recursive calls create tree structure
 * - Divide and conquer: Break problem into subproblems
 * - Memoization: Cache results to avoid recomputation
 * - Tree traversal patterns: Pre-order, in-order, post-order
 * - Used extensively in compilers, data structures, algorithms
 * 
 * When to Use:
 * - Tree data structures
 * - Divide and conquer problems
 * - Problems with overlapping subproblems
 * - Tree traversal
 * - Recursive algorithms
 * 
 * Real-World Usage:
 * - Compiler AST traversal
 * - File system traversal
 * - Tree-based data structures
 * - Divide and conquer algorithms
 * - Dynamic programming (with memoization)
 * 
 * Time Complexity:
 * - Without memoization: O(2^n) for binary tree recursion
 * - With memoization: O(n) for n subproblems
 * - Tree traversal: O(n) where n is number of nodes
 * 
 * Space Complexity: O(h) where h is height of recursion tree
 */

#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

// Binary tree node
template<typename T>
struct TreeNode {
    T data;
    std::unique_ptr<TreeNode<T>> left;
    std::unique_ptr<TreeNode<T>> right;
    
    TreeNode(T d) : data(d), left(nullptr), right(nullptr) {}
};

// Tree recursion patterns
template<typename T>
class TreeRecursion {
public:
    // Pre-order traversal: Process root, then left, then right
    void preorder_traversal(TreeNode<T>* root, std::function<void(T)>& visit) {
        if (!root) return;
        
        visit(root->data);  // Process root
        preorder_traversal(root->left.get(), visit);  // Recurse left
        preorder_traversal(root->right.get(), visit); // Recurse right
    }
    
    // In-order traversal: Process left, then root, then right
    void inorder_traversal(TreeNode<T>* root, std::function<void(T)>& visit) {
        if (!root) return;
        
        inorder_traversal(root->left.get(), visit);  // Recurse left
        visit(root->data);  // Process root
        inorder_traversal(root->right.get(), visit); // Recurse right
    }
    
    // Post-order traversal: Process left, then right, then root
    void postorder_traversal(TreeNode<T>* root, std::function<void(T)>& visit) {
        if (!root) return;
        
        postorder_traversal(root->left.get(), visit);  // Recurse left
        postorder_traversal(root->right.get(), visit); // Recurse right
        visit(root->data);  // Process root
    }
    
    // Divide and conquer: Binary search in tree
    TreeNode<T>* search(TreeNode<T>* root, const T& key) {
        if (!root || root->data == key) {
            return root;
        }
        
        // Divide: Choose left or right subtree
        if (key < root->data) {
            return search(root->left.get(), key);  // Conquer left
        } else {
            return search(root->right.get(), key); // Conquer right
        }
    }
    
    // Tree recursion with memoization: Fibonacci-like tree recursion
    int fibonacci_tree(int n, std::unordered_map<int, int>& memo) {
        // Base case
        if (n <= 1) {
            return n;
        }
        
        // Check memo
        if (memo.find(n) != memo.end()) {
            return memo[n];
        }
        
        // Recursive case: Two recursive calls (tree structure)
        int result = fibonacci_tree(n - 1, memo) + fibonacci_tree(n - 2, memo);
        
        // Store in memo
        memo[n] = result;
        return result;
    }
    
    // Tree recursion without memoization (exponential)
    int fibonacci_tree_naive(int n) {
        if (n <= 1) {
            return n;
        }
        
        // Two recursive calls create tree structure
        return fibonacci_tree_naive(n - 1) + fibonacci_tree_naive(n - 2);
    }
    
    // Count nodes in tree (tree recursion)
    int count_nodes(TreeNode<T>* root) {
        if (!root) {
            return 0;  // Base case
        }
        
        // Recursive case: Count left + count right + 1
        return count_nodes(root->left.get()) + 
               count_nodes(root->right.get()) + 1;
    }
    
    // Calculate tree height (tree recursion)
    int tree_height(TreeNode<T>* root) {
        if (!root) {
            return -1;  // Base case: empty tree has height -1
        }
        
        // Recursive case: Max of left and right heights + 1
        int left_height = tree_height(root->left.get());
        int right_height = tree_height(root->right.get());
        return std::max(left_height, right_height) + 1;
    }
    
    // Check if tree is balanced (tree recursion)
    bool is_balanced(TreeNode<T>* root, int& height) {
        if (!root) {
            height = -1;
            return true;  // Base case: empty tree is balanced
        }
        
        int left_height, right_height;
        bool left_balanced = is_balanced(root->left.get(), left_height);
        bool right_balanced = is_balanced(root->right.get(), right_height);
        
        height = std::max(left_height, right_height) + 1;
        
        // Check balance condition
        return left_balanced && right_balanced && 
               std::abs(left_height - right_height) <= 1;
    }
};

// Example usage
#include <iostream>

int main() {
    // Create tree:     1
    //                 / \
    //                2   3
    //               / \
    //              4   5
    auto root = std::make_unique<TreeNode<int>>(1);
    root->left = std::make_unique<TreeNode<int>>(2);
    root->right = std::make_unique<TreeNode<int>>(3);
    root->left->left = std::make_unique<TreeNode<int>>(4);
    root->left->right = std::make_unique<TreeNode<int>>(5);
    
    TreeRecursion<int> tree;
    
    // Pre-order traversal
    std::cout << "Pre-order: ";
    std::function<void(int)> visit = [](int val) { std::cout << val << " "; };
    tree.preorder_traversal(root.get(), visit);
    std::cout << std::endl;
    
    // In-order traversal
    std::cout << "In-order: ";
    tree.inorder_traversal(root.get(), visit);
    std::cout << std::endl;
    
    // Post-order traversal
    std::cout << "Post-order: ";
    tree.postorder_traversal(root.get(), visit);
    std::cout << std::endl;
    
    // Count nodes
    std::cout << "Node count: " << tree.count_nodes(root.get()) << std::endl;
    
    // Tree height
    std::cout << "Tree height: " << tree.tree_height(root.get()) << std::endl;
    
    // Fibonacci with memoization
    std::unordered_map<int, int> memo;
    std::cout << "Fibonacci(10) with memoization: " 
              << tree.fibonacci_tree(10, memo) << std::endl;
    
    return 0;
}

