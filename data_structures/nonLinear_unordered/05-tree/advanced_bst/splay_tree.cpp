#include <iostream>

using namespace std;

// Splay Tree - Self adjusting BST
// Recently accessed elements move to root (cache locality)
// Amortized O(log n) operations
struct SplayNode {
    int key;
    SplayNode* left;
    SplayNode* right;
    SplayNode* parent;

    SplayNode(int k) : key(k), left(nullptr), right(nullptr), parent(nullptr) {}
};

class SplayTree {
private:
    SplayNode* root;

    void rotateLeft(SplayNode* x) {
        SplayNode* y = x->right;
        if (y) {
            x->right = y->left;
            if (y->left) {
                y->left->parent = x;
            }
            y->parent = x->parent;
        }

        if (!x->parent) {
            root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }

        if (y) {
            y->left = x;
        }
        x->parent = y;
    }

    void rotateRight(SplayNode* x) {
        SplayNode* y = x->left;
        if (y) {
            x->left = y->right;
            if (y->right) {
                y->right->parent = x;
            }
            y->parent = x->parent;
        }

        if (!x->parent) {
            root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }

        if (y) {
            y->right = x;
        }
        x->parent = y;
    }

    void splay(SplayNode* x) {
        while (x->parent) {
            if (!x->parent->parent) {
                // Zig case
                if (x->parent->left == x) {
                    rotateRight(x->parent);
                } else {
                    rotateLeft(x->parent);
                }
            } else if (x->parent->left == x && x->parent->parent->left == x->parent) {
                // Zig-zig case
                rotateRight(x->parent->parent);
                rotateRight(x->parent);
            } else if (x->parent->right == x && x->parent->parent->right == x->parent) {
                // Zig-zig case
                rotateLeft(x->parent->parent);
                rotateLeft(x->parent);
            } else if (x->parent->left == x && x->parent->parent->right == x->parent) {
                // Zig-zag case
                rotateRight(x->parent);
                rotateLeft(x->parent);
            } else {
                // Zig-zag case
                rotateLeft(x->parent);
                rotateRight(x->parent);
            }
        }
    }

    SplayNode* search(SplayNode* node, int key) {
        if (!node || node->key == key) {
            return node;
        }

        if (key < node->key) {
            if (!node->left) {
                return node;
            }
            return search(node->left, key);
        } else {
            if (!node->right) {
                return node;
            }
            return search(node->right, key);
        }
    }

    SplayNode* insert(SplayNode* node, int key) {
        if (!node) {
            return new SplayNode(key);
        }

        if (key < node->key) {
            node->left = insert(node->left, key);
            node->left->parent = node;
        } else if (key > node->key) {
            node->right = insert(node->right, key);
            node->right->parent = node;
        }

        return node;
    }

    SplayNode* findMax(SplayNode* node) {
        while (node->right) {
            node = node->right;
        }
        return node;
    }

    void inorder(SplayNode* node) {
        if (node) {
            inorder(node->left);
            cout << node->key << " ";
            inorder(node->right);
        }
    }

public:
    SplayTree() : root(nullptr) {}

    bool search(int key) {
        if (!root) {
            return false;
        }
        SplayNode* node = search(root, key);
        splay(node);
        root = node;
        return node->key == key;
    }

    void insert(int key) {
        if (!root) {
            root = new SplayNode(key);
            return;
        }

        SplayNode* node = search(root, key);
        if (node->key == key) {
            splay(node);
            root = node;
            return;
        }

        root = insert(root, key);
        node = search(root, key);
        splay(node);
        root = node;
    }

    void remove(int key) {
        if (!root) {
            return;
        }

        SplayNode* node = search(root, key);
        splay(node);
        root = node;

        if (node->key != key) {
            return;
        }

        if (!node->left) {
            root = node->right;
            if (root) {
                root->parent = nullptr;
            }
            delete node;
        } else if (!node->right) {
            root = node->left;
            if (root) {
                root->parent = nullptr;
            }
            delete node;
        } else {
            SplayNode* maxLeft = findMax(node->left);
            splay(maxLeft);
            root = maxLeft;
            root->right = node->right;
            if (root->right) {
                root->right->parent = root;
            }
            delete node;
        }
    }

    void inorder() {
        inorder(root);
        cout << endl;
    }
};

int main() {
    SplayTree tree;

    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(40);
    tree.insert(50);

    cout << "Inorder traversal: ";
    tree.inorder();

    cout << "Search 30: " << tree.search(30) << endl;
    cout << "After search, root is: " << (tree.search(30) ? "30" : "other") << endl;

    tree.remove(30);
    cout << "After removing 30: ";
    tree.inorder();

    return 0;
}

