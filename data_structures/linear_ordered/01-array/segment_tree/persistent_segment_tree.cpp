#include <iostream>
#include <vector>

using namespace std;

// Persistent Segment Tree - Maintains history of all versions
// O(log n) space per update, O(log n) query time
class PersistentSegmentTree {
private:
    struct Node {
        int sum;
        Node* left;
        Node* right;

        Node(int s = 0) : sum(s), left(nullptr), right(nullptr) {}
        Node(Node* l, Node* r) : left(l), right(r) {
            sum = (l ? l->sum : 0) + (r ? r->sum : 0);
        }
    };

    vector<Node*> roots;
    int n;

    Node* build(const vector<int>& arr, int start, int end) {
        if (start == end) {
            return new Node(arr[start]);
        }

        int mid = (start + end) / 2;
        Node* left = build(arr, start, mid);
        Node* right = build(arr, mid + 1, end);
        return new Node(left, right);
    }

    Node* update(Node* node, int start, int end, int idx, int val) {
        if (start == end) {
            return new Node(val);
        }

        int mid = (start + end) / 2;
        if (idx <= mid) {
            return new Node(update(node->left, start, mid, idx, val), node->right);
        } else {
            return new Node(node->left, update(node->right, mid + 1, end, idx, val));
        }
    }

    int query(Node* node, int start, int end, int l, int r) {
        if (start > r || end < l) {
            return 0;
        }

        if (start >= l && end <= r) {
            return node->sum;
        }

        int mid = (start + end) / 2;
        return query(node->left, start, mid, l, r) +
               query(node->right, mid + 1, end, l, r);
    }

public:
    PersistentSegmentTree(const vector<int>& arr) : n(arr.size()) {
        roots.push_back(build(arr, 0, n - 1));
    }

    void update(int version, int idx, int val) {
        Node* newRoot = update(roots[version], 0, n - 1, idx, val);
        roots.push_back(newRoot);
    }

    int query(int version, int l, int r) {
        return query(roots[version], 0, n - 1, l, r);
    }

    int getLatestVersion() {
        return roots.size() - 1;
    }
};

int main() {
    vector<int> arr = {1, 3, 5, 7, 9, 11};
    PersistentSegmentTree pst(arr);

    cout << "Version 0, Range [1, 3]: " << pst.query(0, 1, 3) << endl;

    pst.update(0, 2, 10);
    cout << "Version 1, Range [1, 3]: " << pst.query(1, 1, 3) << endl;
    cout << "Version 0 still intact, Range [1, 3]: " << pst.query(0, 1, 3) << endl;

    pst.update(1, 4, 20);
    cout << "Version 2, Range [1, 5]: " << pst.query(2, 1, 5) << endl;

    return 0;
}

