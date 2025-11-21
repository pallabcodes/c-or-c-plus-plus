#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// B+ Tree - Optimized for database systems
// Internal nodes store keys, leaf nodes store data
// O(log n) operations, good for disk I/O
template<int ORDER = 4>
class BPlusTree {
private:
    struct Node {
        bool isLeaf;
        vector<int> keys;
        Node* parent;

        Node(bool leaf = false) : isLeaf(leaf), parent(nullptr) {}
    };

    struct InternalNode : public Node {
        vector<Node*> children;

        InternalNode() {
            this->isLeaf = false;
        }
    };

    struct LeafNode : public Node {
        vector<int> values;
        LeafNode* next;

        LeafNode() {
            this->isLeaf = true;
            next = nullptr;
        }
    };

    Node* root;
    int maxKeys;
    int minKeys;

    void splitInternal(InternalNode* node) {
        int mid = maxKeys / 2;
        int keyToPromote = node->keys[mid];

        InternalNode* newNode = new InternalNode();
        newNode->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
        newNode->children.assign(node->children.begin() + mid + 1, node->children.end());

        node->keys.resize(mid);
        node->children.resize(mid + 1);

        for (Node* child : newNode->children) {
            child->parent = newNode;
        }

        if (node->parent) {
            insertIntoInternal(static_cast<InternalNode*>(node->parent), keyToPromote, newNode);
        } else {
            InternalNode* newRoot = new InternalNode();
            newRoot->keys.push_back(keyToPromote);
            newRoot->children.push_back(node);
            newRoot->children.push_back(newNode);
            node->parent = newRoot;
            newNode->parent = newRoot;
            root = newRoot;
        }
    }

    void splitLeaf(LeafNode* node) {
        int mid = maxKeys / 2;
        int keyToPromote = node->keys[mid];

        LeafNode* newNode = new LeafNode();
        newNode->keys.assign(node->keys.begin() + mid, node->keys.end());
        newNode->values.assign(node->values.begin() + mid, node->values.end());
        newNode->next = node->next;

        node->keys.resize(mid);
        node->values.resize(mid);
        node->next = newNode;

        if (node->parent) {
            insertIntoInternal(static_cast<InternalNode*>(node->parent), keyToPromote, newNode);
        } else {
            InternalNode* newRoot = new InternalNode();
            newRoot->keys.push_back(keyToPromote);
            newRoot->children.push_back(node);
            newRoot->children.push_back(newNode);
            node->parent = newRoot;
            newNode->parent = newRoot;
            root = newRoot;
        }
    }

    void insertIntoInternal(InternalNode* node, int key, Node* rightChild) {
        auto it = lower_bound(node->keys.begin(), node->keys.end(), key);
        int pos = it - node->keys.begin();
        
        node->keys.insert(it, key);
        node->children.insert(node->children.begin() + pos + 1, rightChild);
        rightChild->parent = node;

        if (node->keys.size() > maxKeys) {
            splitInternal(node);
        }
    }

    void insertIntoLeaf(LeafNode* node, int key, int value) {
        auto it = lower_bound(node->keys.begin(), node->keys.end(), key);
        int pos = it - node->keys.begin();
        
        node->keys.insert(it, key);
        node->values.insert(node->values.begin() + pos, value);

        if (node->keys.size() > maxKeys) {
            splitLeaf(node);
        }
    }

    Node* findLeaf(int key) {
        Node* current = root;
        while (!current->isLeaf) {
            InternalNode* internal = static_cast<InternalNode*>(current);
            auto it = upper_bound(internal->keys.begin(), internal->keys.end(), key);
            int pos = it - internal->keys.begin();
            current = internal->children[pos];
        }
        return current;
    }

public:
    BPlusTree() : root(new LeafNode()), maxKeys(ORDER - 1), minKeys((ORDER - 1) / 2) {}

    void insert(int key, int value) {
        LeafNode* leaf = static_cast<LeafNode*>(findLeaf(key));
        insertIntoLeaf(leaf, key, value);
    }

    bool search(int key) {
        LeafNode* leaf = static_cast<LeafNode*>(findLeaf(key));
        return binary_search(leaf->keys.begin(), leaf->keys.end(), key);
    }

    vector<int> rangeQuery(int startKey, int endKey) {
        vector<int> result;
        LeafNode* leaf = static_cast<LeafNode*>(findLeaf(startKey));

        while (leaf) {
            for (size_t i = 0; i < leaf->keys.size(); i++) {
                if (leaf->keys[i] >= startKey && leaf->keys[i] <= endKey) {
                    result.push_back(leaf->values[i]);
                }
                if (leaf->keys[i] > endKey) {
                    return result;
                }
            }
            leaf = leaf->next;
        }

        return result;
    }
};

int main() {
    BPlusTree<4> tree;

    tree.insert(10, 100);
    tree.insert(20, 200);
    tree.insert(30, 300);
    tree.insert(40, 400);
    tree.insert(50, 500);

    cout << "Search 30: " << tree.search(30) << endl;
    cout << "Search 35: " << tree.search(35) << endl;

    vector<int> range = tree.rangeQuery(20, 40);
    cout << "Range query [20, 40]: ";
    for (int val : range) {
        cout << val << " ";
    }
    cout << endl;

    return 0;
}

