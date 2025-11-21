#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Fenwick Tree (Binary Indexed Tree) - God modded implementation
// Time: O(log n) update/query, Space: O(n)
// Uses bit manipulation tricks for maximum performance
class FenwickTree {
private:
    vector<int> tree;
    int n;

    // Extract lowest set bit: i & (-i)
    // This is the magic bit manipulation trick
    inline int lsb(int i) {
        return i & (-i);
    }

public:
    FenwickTree(int size) : n(size + 1), tree(size + 1, 0) {}

    // Build from array - O(n log n)
    FenwickTree(const vector<int>& arr) : n(arr.size() + 1), tree(arr.size() + 1, 0) {
        for (int i = 0; i < arr.size(); i++) {
            update(i, arr[i]);
        }
    }

    // Update: Add delta to position idx (1-indexed internally)
    // Traverses up the tree using bit manipulation
    void update(int idx, int delta) {
        idx++; // Convert to 1-indexed
        while (idx < n) {
            tree[idx] += delta;
            idx += lsb(idx); // Move to parent
        }
    }

    // Query prefix sum [0, idx]
    // Traverses down the tree using bit manipulation
    int query(int idx) {
        idx++; // Convert to 1-indexed
        int sum = 0;
        while (idx > 0) {
            sum += tree[idx];
            idx -= lsb(idx); // Move to parent
        }
        return sum;
    }

    // Range query [l, r]
    int rangeQuery(int l, int r) {
        return query(r) - query(l - 1);
    }

    // Get value at index (requires original array or range query)
    int get(int idx) {
        return rangeQuery(idx, idx);
    }

    // Find index with given cumulative frequency (binary search)
    int find(int cumFreq) {
        int idx = 0;
        int bitMask = 1;
        while (bitMask < n) bitMask <<= 1;
        bitMask >>= 1;

        while (bitMask > 0 && idx < n) {
            int nextIdx = idx + bitMask;
            if (nextIdx < n && tree[nextIdx] <= cumFreq) {
                cumFreq -= tree[nextIdx];
                idx = nextIdx;
            }
            bitMask >>= 1;
        }
        return idx - 1; // Convert back to 0-indexed
    }
};

// 2D Fenwick Tree for matrix range queries
class FenwickTree2D {
private:
    vector<vector<int>> tree;
    int n, m;

    inline int lsb(int i) {
        return i & (-i);
    }

public:
    FenwickTree2D(int rows, int cols) : n(rows + 1), m(cols + 1), 
                                         tree(rows + 1, vector<int>(cols + 1, 0)) {}

    void update(int row, int col, int delta) {
        row++; col++;
        for (int i = row; i < n; i += lsb(i)) {
            for (int j = col; j < m; j += lsb(j)) {
                tree[i][j] += delta;
            }
        }
    }

    int query(int row, int col) {
        row++; col++;
        int sum = 0;
        for (int i = row; i > 0; i -= lsb(i)) {
            for (int j = col; j > 0; j -= lsb(j)) {
                sum += tree[i][j];
            }
        }
        return sum;
    }

    int rangeQuery(int r1, int c1, int r2, int c2) {
        return query(r2, c2) - query(r1 - 1, c2) - 
               query(r2, c1 - 1) + query(r1 - 1, c1 - 1);
    }
};

int main() {
    vector<int> arr = {1, 3, 5, 7, 9, 11};
    FenwickTree ft(arr);

    cout << "Prefix sum [0, 3]: " << ft.query(3) << endl; // 16
    cout << "Prefix sum [0, 5]: " << ft.query(5) << endl; // 36
    cout << "Range sum [1, 3]: " << ft.rangeQuery(1, 3) << endl; // 15

    ft.update(2, 5); // Add 5 to index 2
    cout << "After update, prefix sum [0, 3]: " << ft.query(3) << endl; // 21

    // 2D example
    FenwickTree2D ft2d(4, 4);
    ft2d.update(1, 1, 5);
    ft2d.update(2, 2, 10);
    cout << "2D Range query [(1,1), (2,2)]: " << ft2d.rangeQuery(1, 1, 2, 2) << endl;

    return 0;
}

