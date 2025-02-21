#include <bits/stdc++.h>
using namespace std;

class SegmentTree {
private:
  vector<int> tree;
  int n;

  // Build Segment Tree
  void build(vector<int> &arr, int node, int start, int end) {
    if (start == end) {
      tree[node] = arr[start]; // Leaf node stores the array value
    } else {
      int mid = (start + end) / 2;
      build(arr, 2 * node + 1, start, mid);                 // Left subtree
      build(arr, 2 * node + 2, mid + 1, end);               // Right subtree
      tree[node] = tree[2 * node + 1] + tree[2 * node + 2]; // Sum of children
    }
  }

  // Range Query: Sum in range [l, r]
  int query(int node, int start, int end, int l, int r) {
    if (r < start || end < l)
      return 0; // Out of range
    if (l <= start && end <= r)
      return tree[node]; // Fully inside range

    int mid = (start + end) / 2;
    int leftSum = query(2 * node + 1, start, mid, l, r);
    int rightSum = query(2 * node + 2, mid + 1, end, l, r);
    return leftSum + rightSum;
  }

  // Point Update: Update arr[idx] to newValue
  void update(int node, int start, int end, int idx, int newValue) {
    if (start == end) {
      tree[node] = newValue; // Update leaf node
    } else {
      int mid = (start + end) / 2;
      if (idx <= mid)
        update(2 * node + 1, start, mid, idx, newValue);
      else
        update(2 * node + 2, mid + 1, end, idx, newValue);
      tree[node] = tree[2 * node + 1] + tree[2 * node + 2]; // Recalculate sum
    }
  }

public:
  // Constructor: Initialize segment tree
  SegmentTree(vector<int> &arr) {
    n = arr.size();
    tree.resize(4 * n, 0); // Allocate memory
    build(arr, 0, 0, n - 1);
  }

  // Public method for range sum query
  int query(int l, int r) { return query(0, 0, n - 1, l, r); }

  // Public method for point update
  void update(int idx, int newValue) { update(0, 0, n - 1, idx, newValue); }
};

// Driver Code
int main() {
  vector<int> arr = {1, 3, 5, 7, 9, 11}; // Sample input
  SegmentTree segTree(arr);

  // Test Queries
  cout << "Sum of range [1, 3]: " << segTree.query(1, 3)
       << endl; // Expected: 3+5+7 = 15
  cout << "Sum of range [0, 5]: " << segTree.query(0, 5)
       << endl; // Expected: 1+3+5+7+9+11 = 36

  // Update element at index 2 from 5 -> 10
  segTree.update(2, 10);
  cout << "After update, sum of range [1, 3]: " << segTree.query(1, 3)
       << endl; // Expected: 3+10+7 = 20

  return 0;
}
