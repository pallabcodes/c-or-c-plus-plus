#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

// Rope Data Structure - Efficient string manipulation
// O(log n) insert, delete, substring operations
struct RopeNode {
    string data;
    int weight;
    RopeNode* left;
    RopeNode* right;

    RopeNode(const string& s) : data(s), weight(s.length()), left(nullptr), right(nullptr) {}
    RopeNode(RopeNode* l, RopeNode* r) : left(l), right(r) {
        weight = l ? l->weight + (l->right ? getWeight(l->right) : 0) : 0;
    }

    static int getWeight(RopeNode* node) {
        if (!node) return 0;
        if (!node->left && !node->right) return node->weight;
        return node->weight + getWeight(node->right);
    }
};

class Rope {
private:
    RopeNode* root;

    RopeNode* concat(RopeNode* left, RopeNode* right) {
        if (!left) return right;
        if (!right) return left;
        return new RopeNode(left, right);
    }

    pair<RopeNode*, RopeNode*> split(RopeNode* node, int pos) {
        if (!node) {
            return {nullptr, nullptr};
        }

        if (!node->left && !node->right) {
            if (pos >= node->weight) {
                return {node, nullptr};
            }
            RopeNode* left = new RopeNode(node->data.substr(0, pos));
            RopeNode* right = new RopeNode(node->data.substr(pos));
            return {left, right};
        }

        int leftWeight = node->left ? RopeNode::getWeight(node->left) : 0;
        if (pos < leftWeight) {
            auto [l, r] = split(node->left, pos);
            return {l, concat(r, node->right)};
        } else {
            auto [l, r] = split(node->right, pos - leftWeight);
            return {concat(node->left, l), r};
        }
    }

    RopeNode* insert(RopeNode* node, int pos, const string& s) {
        auto [left, right] = split(node, pos);
        RopeNode* newNode = new RopeNode(s);
        return concat(concat(left, newNode), right);
    }

    RopeNode* remove(RopeNode* node, int start, int len) {
        auto [left, temp] = split(node, start);
        auto [mid, right] = split(temp, len);
        return concat(left, right);
    }

    string toString(RopeNode* node) {
        if (!node) return "";
        if (!node->left && !node->right) {
            return node->data;
        }
        return toString(node->left) + toString(node->right);
    }

    char getChar(RopeNode* node, int pos) {
        if (!node->left && !node->right) {
            return node->data[pos];
        }

        int leftWeight = node->left ? RopeNode::getWeight(node->left) : 0;
        if (pos < leftWeight) {
            return getChar(node->left, pos);
        } else {
            return getChar(node->right, pos - leftWeight);
        }
    }

public:
    Rope(const string& s = "") {
        root = s.empty() ? nullptr : new RopeNode(s);
    }

    void insert(int pos, const string& s) {
        root = insert(root, pos, s);
    }

    void remove(int start, int len) {
        root = remove(root, start, len);
    }

    string substring(int start, int len) {
        auto [left, temp] = split(root, start);
        auto [mid, right] = split(temp, len);
        string result = toString(mid);
        root = concat(concat(left, mid), right);
        return result;
    }

    char at(int pos) {
        return getChar(root, pos);
    }

    string toString() {
        return toString(root);
    }

    int length() {
        return root ? RopeNode::getWeight(root) : 0;
    }
};

int main() {
    Rope rope("Hello World");

    cout << "Original: " << rope.toString() << endl;
    cout << "Char at 6: " << rope.at(6) << endl;

    rope.insert(5, " Beautiful");
    cout << "After insert: " << rope.toString() << endl;

    rope.remove(5, 10);
    cout << "After remove: " << rope.toString() << endl;

    string sub = rope.substring(0, 5);
    cout << "Substring [0, 5]: " << sub << endl;

    return 0;
}

