// Splay Tree: Self-adjusting binary search tree
// Based on research by Sleator and Tarjan
// Amortized O(log n) operations
// Space: O(n)
// God modded implementation with splaying operations

#include <iostream>
#include <memory>

struct Node {
    int key;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    std::shared_ptr<Node> parent;
    
    Node(int k) : key(k), left(nullptr), right(nullptr), parent(nullptr) {}
};

class SplayTree {
private:
    std::shared_ptr<Node> root;
    
    void leftRotate(std::shared_ptr<Node> x) {
        std::shared_ptr<Node> y = x->right;
        if (y) {
            x->right = y->left;
            if (y->left) {
                y->left->parent = x;
            }
            y->parent = x->parent;
            if (!x->parent) {
                root = y;
            } else if (x == x->parent->left) {
                x->parent->left = y;
            } else {
                x->parent->right = y;
            }
            y->left = x;
            x->parent = y;
        }
    }
    
    void rightRotate(std::shared_ptr<Node> x) {
        std::shared_ptr<Node> y = x->left;
        if (y) {
            x->left = y->right;
            if (y->right) {
                y->right->parent = x;
            }
            y->parent = x->parent;
            if (!x->parent) {
                root = y;
            } else if (x == x->parent->left) {
                x->parent->left = y;
            } else {
                x->parent->right = y;
            }
            y->right = x;
            x->parent = y;
        }
    }
    
    void splay(std::shared_ptr<Node> x) {
        while (x->parent) {
            if (!x->parent->parent) {
                if (x == x->parent->left) {
                    rightRotate(x->parent);
                } else {
                    leftRotate(x->parent);
                }
            } else if (x == x->parent->left && 
                      x->parent == x->parent->parent->left) {
                rightRotate(x->parent->parent);
                rightRotate(x->parent);
            } else if (x == x->parent->right && 
                      x->parent == x->parent->parent->right) {
                leftRotate(x->parent->parent);
                leftRotate(x->parent);
            } else if (x == x->parent->right && 
                      x->parent == x->parent->parent->left) {
                leftRotate(x->parent);
                rightRotate(x->parent);
            } else {
                rightRotate(x->parent);
                leftRotate(x->parent);
            }
        }
        root = x;
    }
    
    std::shared_ptr<Node> find(int key) {
        std::shared_ptr<Node> curr = root;
        std::shared_ptr<Node> prev = nullptr;
        
        while (curr) {
            prev = curr;
            if (key < curr->key) {
                curr = curr->left;
            } else if (key > curr->key) {
                curr = curr->right;
            } else {
                splay(curr);
                return curr;
            }
        }
        
        if (prev) {
            splay(prev);
        }
        return nullptr;
    }
    
public:
    SplayTree() : root(nullptr) {}
    
    void insert(int key) {
        if (!root) {
            root = std::make_shared<Node>(key);
            return;
        }
        
        std::shared_ptr<Node> curr = root;
        std::shared_ptr<Node> parent = nullptr;
        
        while (curr) {
            parent = curr;
            if (key < curr->key) {
                curr = curr->left;
            } else if (key > curr->key) {
                curr = curr->right;
            } else {
                splay(curr);
                return;
            }
        }
        
        std::shared_ptr<Node> newNode = std::make_shared<Node>(key);
        newNode->parent = parent;
        
        if (key < parent->key) {
            parent->left = newNode;
        } else {
            parent->right = newNode;
        }
        
        splay(newNode);
    }
    
    bool search(int key) {
        return find(key) != nullptr;
    }
    
    void remove(int key) {
        std::shared_ptr<Node> node = find(key);
        if (!node) return;
        
        if (!node->left) {
            root = node->right;
            if (root) root->parent = nullptr;
        } else if (!node->right) {
            root = node->left;
            if (root) root->parent = nullptr;
        } else {
            std::shared_ptr<Node> minRight = node->right;
            while (minRight->left) {
                minRight = minRight->left;
            }
            
            if (minRight->parent != node) {
                minRight->parent->left = minRight->right;
                if (minRight->right) {
                    minRight->right->parent = minRight->parent;
                }
                minRight->right = node->right;
                minRight->right->parent = minRight;
            }
            
            root = minRight;
            root->left = node->left;
            root->left->parent = root;
            root->parent = nullptr;
        }
    }
    
    void inorder() {
        inorderHelper(root);
        std::cout << std::endl;
    }
    
private:
    void inorderHelper(std::shared_ptr<Node> node) {
        if (node) {
            inorderHelper(node->left);
            std::cout << node->key << " ";
            inorderHelper(node->right);
        }
    }
};

// Example usage
int main() {
    SplayTree tree;
    
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(40);
    tree.insert(50);
    
    std::cout << "Inorder traversal: ";
    tree.inorder();
    
    std::cout << "Search 30: " << (tree.search(30) ? "Found" : "Not found") << std::endl;
    std::cout << "Search 25: " << (tree.search(25) ? "Found" : "Not found") << std::endl;
    
    tree.remove(30);
    std::cout << "After removing 30: ";
    tree.inorder();
    
    return 0;
}

