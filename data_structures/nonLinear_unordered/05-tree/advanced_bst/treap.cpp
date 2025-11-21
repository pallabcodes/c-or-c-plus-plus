#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

// Treap - Tree + Heap
// Combines BST property with heap property (randomized priority)
// Expected O(log n) operations
struct TreapNode {
    int key;
    int priority;
    TreapNode* left;
    TreapNode* right;

    TreapNode(int k) : key(k), priority(rand()), left(nullptr), right(nullptr) {}
};

class Treap {
private:
    TreapNode* root;

    void split(TreapNode* node, int key, TreapNode*& left, TreapNode*& right) {
        if (!node) {
            left = right = nullptr;
            return;
        }

        if (node->key <= key) {
            split(node->right, key, node->right, right);
            left = node;
        } else {
            split(node->left, key, left, node->left);
            right = node;
        }
    }

    TreapNode* merge(TreapNode* left, TreapNode* right) {
        if (!left) return right;
        if (!right) return left;

        if (left->priority > right->priority) {
            left->right = merge(left->right, right);
            return left;
        } else {
            right->left = merge(left, right->left);
            return right;
        }
    }

    TreapNode* insert(TreapNode* node, TreapNode* newNode) {
        if (!node) {
            return newNode;
        }

        if (newNode->priority > node->priority) {
            split(node, newNode->key, newNode->left, newNode->right);
            return newNode;
        }

        if (newNode->key < node->key) {
            node->left = insert(node->left, newNode);
        } else {
            node->right = insert(node->right, newNode);
        }

        return node;
    }

    TreapNode* remove(TreapNode* node, int key) {
        if (!node) {
            return node;
        }

        if (key < node->key) {
            node->left = remove(node->left, key);
        } else if (key > node->key) {
            node->right = remove(node->right, key);
        } else {
            TreapNode* temp = node;
            node = merge(node->left, node->right);
            delete temp;
        }

        return node;
    }

    bool search(TreapNode* node, int key) {
        if (!node) {
            return false;
        }
        if (key == node->key) {
            return true;
        }
        return key < node->key ? search(node->left, key) : search(node->right, key);
    }

    void inorder(TreapNode* node) {
        if (node) {
            inorder(node->left);
            cout << node->key << "(" << node->priority << ") ";
            inorder(node->right);
        }
    }

public:
    Treap() : root(nullptr) {
        srand(time(nullptr));
    }

    void insert(int key) {
        TreapNode* newNode = new TreapNode(key);
        root = insert(root, newNode);
    }

    void remove(int key) {
        root = remove(root, key);
    }

    bool search(int key) {
        return search(root, key);
    }

    void inorder() {
        inorder(root);
        cout << endl;
    }
};

int main() {
    Treap tree;

    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(40);
    tree.insert(50);

    cout << "Inorder traversal (key priority): ";
    tree.inorder();

    cout << "Search 30: " << tree.search(30) << endl;
    cout << "Search 35: " << tree.search(35) << endl;

    tree.remove(30);
    cout << "After removing 30: ";
    tree.inorder();

    return 0;
}

