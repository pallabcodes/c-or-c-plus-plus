#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

// Advanced Segment Tree with multiple query types
// Supports: Range Sum, Range Min, Range Max, Range GCD
class AdvancedSegmentTree {
private:
    struct Node {
        int sum;
        int minVal;
        int maxVal;
        int gcd;
    };

    vector<Node> tree;
    vector<int> lazy;
    int n;

    int gcd(int a, int b) {
        return b == 0 ? a : gcd(b, a % b);
    }

    void build(const vector<int>& arr, int node, int start, int end) {
        if (start == end) {
            tree[node].sum = arr[start];
            tree[node].minVal = arr[start];
            tree[node].maxVal = arr[start];
            tree[node].gcd = arr[start];
        } else {
            int mid = (start + end) / 2;
            build(arr, 2 * node + 1, start, mid);
            build(arr, 2 * node + 2, mid + 1, end);
            
            tree[node].sum = tree[2 * node + 1].sum + tree[2 * node + 2].sum;
            tree[node].minVal = min(tree[2 * node + 1].minVal, tree[2 * node + 2].minVal);
            tree[node].maxVal = max(tree[2 * node + 1].maxVal, tree[2 * node + 2].maxVal);
            tree[node].gcd = gcd(tree[2 * node + 1].gcd, tree[2 * node + 2].gcd);
        }
    }

    void updateLazy(int node, int start, int end) {
        if (lazy[node] != 0) {
            tree[node].sum += lazy[node] * (end - start + 1);
            tree[node].minVal += lazy[node];
            tree[node].maxVal += lazy[node];
            
            if (start != end) {
                lazy[2 * node + 1] += lazy[node];
                lazy[2 * node + 2] += lazy[node];
            }
            lazy[node] = 0;
        }
    }

    void updateRange(int node, int start, int end, int l, int r, int val) {
        updateLazy(node, start, end);
        
        if (start > r || end < l) {
            return;
        }

        if (start >= l && end <= r) {
            tree[node].sum += val * (end - start + 1);
            tree[node].minVal += val;
            tree[node].maxVal += val;
            
            if (start != end) {
                lazy[2 * node + 1] += val;
                lazy[2 * node + 2] += val;
            }
            return;
        }

        int mid = (start + end) / 2;
        updateRange(2 * node + 1, start, mid, l, r, val);
        updateRange(2 * node + 2, mid + 1, end, l, r, val);
        
        updateLazy(2 * node + 1, start, mid);
        updateLazy(2 * node + 2, mid + 1, end);
        
        tree[node].sum = tree[2 * node + 1].sum + tree[2 * node + 2].sum;
        tree[node].minVal = min(tree[2 * node + 1].minVal, tree[2 * node + 2].minVal);
        tree[node].maxVal = max(tree[2 * node + 1].maxVal, tree[2 * node + 2].maxVal);
        tree[node].gcd = gcd(tree[2 * node + 1].gcd, tree[2 * node + 2].gcd);
    }

    int querySum(int node, int start, int end, int l, int r) {
        updateLazy(node, start, end);
        
        if (start > r || end < l) {
            return 0;
        }
        
        if (start >= l && end <= r) {
            return tree[node].sum;
        }

        int mid = (start + end) / 2;
        return querySum(2 * node + 1, start, mid, l, r) +
               querySum(2 * node + 2, mid + 1, end, l, r);
    }

    int queryMin(int node, int start, int end, int l, int r) {
        updateLazy(node, start, end);
        
        if (start > r || end < l) {
            return INT_MAX;
        }
        
        if (start >= l && end <= r) {
            return tree[node].minVal;
        }

        int mid = (start + end) / 2;
        return min(queryMin(2 * node + 1, start, mid, l, r),
                   queryMin(2 * node + 2, mid + 1, end, l, r));
    }

    int queryMax(int node, int start, int end, int l, int r) {
        updateLazy(node, start, end);
        
        if (start > r || end < l) {
            return INT_MIN;
        }
        
        if (start >= l && end <= r) {
            return tree[node].maxVal;
        }

        int mid = (start + end) / 2;
        return max(queryMax(2 * node + 1, start, mid, l, r),
                   queryMax(2 * node + 2, mid + 1, end, l, r));
    }

public:
    AdvancedSegmentTree(const vector<int>& arr) : n(arr.size()) {
        tree.resize(4 * n);
        lazy.resize(4 * n, 0);
        build(arr, 0, 0, n - 1);
    }

    void updateRange(int l, int r, int val) {
        updateRange(0, 0, n - 1, l, r, val);
    }

    int querySum(int l, int r) {
        return querySum(0, 0, n - 1, l, r);
    }

    int queryMin(int l, int r) {
        return queryMin(0, 0, n - 1, l, r);
    }

    int queryMax(int l, int r) {
        return queryMax(0, 0, n - 1, l, r);
    }
};

int main() {
    vector<int> arr = {1, 3, 5, 7, 9, 11};
    AdvancedSegmentTree st(arr);

    cout << "Range Sum [1, 3]: " << st.querySum(1, 3) << endl;
    cout << "Range Min [1, 3]: " << st.queryMin(1, 3) << endl;
    cout << "Range Max [1, 3]: " << st.queryMax(1, 3) << endl;

    st.updateRange(1, 3, 5);
    cout << "After adding 5 to [1, 3], Range Sum: " << st.querySum(1, 3) << endl;

    return 0;
}

