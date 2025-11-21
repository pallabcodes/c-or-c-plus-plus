#include <iostream>
#include <algorithm>
#include <climits>

using namespace std;

// Interval Tree - For interval queries and overlaps
// O(log n) insert, delete, search
struct Interval {
    int low;
    int high;

    Interval(int l, int h) : low(l), high(h) {}
};

struct IntervalNode {
    Interval interval;
    int max;
    IntervalNode* left;
    IntervalNode* right;

    IntervalNode(Interval i) : interval(i), max(i.high), left(nullptr), right(nullptr) {}
};

class IntervalTree {
private:
    IntervalNode* root;

    IntervalNode* insert(IntervalNode* node, Interval interval) {
        if (!node) {
            return new IntervalNode(interval);
        }

        if (interval.low < node->interval.low) {
            node->left = insert(node->left, interval);
        } else {
            node->right = insert(node->right, interval);
        }

        if (node->max < interval.high) {
            node->max = interval.high;
        }

        return node;
    }

    bool doOverlap(Interval i1, Interval i2) {
        return i1.low <= i2.high && i2.low <= i1.high;
    }

    IntervalNode* searchOverlap(IntervalNode* node, Interval interval) {
        if (!node) {
            return nullptr;
        }

        if (doOverlap(node->interval, interval)) {
            return node;
        }

        if (node->left && node->left->max >= interval.low) {
            return searchOverlap(node->left, interval);
        }

        return searchOverlap(node->right, interval);
    }

    IntervalNode* findMin(IntervalNode* node) {
        while (node->left) {
            node = node->left;
        }
        return node;
    }

    IntervalNode* remove(IntervalNode* node, Interval interval) {
        if (!node) {
            return node;
        }

        if (interval.low < node->interval.low) {
            node->left = remove(node->left, interval);
        } else if (interval.low > node->interval.low) {
            node->right = remove(node->right, interval);
        } else if (interval.high == node->interval.high) {
            if (!node->left) {
                IntervalNode* temp = node->right;
                delete node;
                return temp;
            } else if (!node->right) {
                IntervalNode* temp = node->left;
                delete node;
                return temp;
            }

            IntervalNode* temp = findMin(node->right);
            node->interval = temp->interval;
            node->right = remove(node->right, temp->interval);
        }

        if (node->left) {
            node->max = max(node->interval.high, node->left->max);
        }
        if (node->right) {
            node->max = max(node->max, node->right->max);
        }

        return node;
    }

public:
    IntervalTree() : root(nullptr) {}

    void insert(Interval interval) {
        root = insert(root, interval);
    }

    Interval* searchOverlap(Interval interval) {
        IntervalNode* result = searchOverlap(root, interval);
        return result ? &result->interval : nullptr;
    }

    void remove(Interval interval) {
        root = remove(root, interval);
    }
};

int main() {
    IntervalTree tree;

    tree.insert(Interval(15, 20));
    tree.insert(Interval(10, 30));
    tree.insert(Interval(17, 19));
    tree.insert(Interval(5, 20));
    tree.insert(Interval(12, 15));
    tree.insert(Interval(30, 40));

    Interval searchInterval(6, 7);
    Interval* result = tree.searchOverlap(searchInterval);
    if (result) {
        cout << "Overlap found: [" << result->low << ", " << result->high << "]" << endl;
    } else {
        cout << "No overlap found" << endl;
    }

    return 0;
}

