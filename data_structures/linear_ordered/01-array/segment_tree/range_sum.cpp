#include <bits/stdc++.h>
using namespace std;

class SegmentTree {
public:
  vector<int> tree;
  int n;

  SegmentTree(vector<int> &arr) {
    n = arr.size();
    tree.resize(4 * n); // Safe size for segment tree
    build(arr, 0, 0, n - 1);
  }

  void build(vector<int> &arr, int node, int start, int end) {
    if (start == end) { // Leaf node
      tree[node] = arr[start];
    } else {
      int mid = (start + end) / 2;
      build(arr, 2 * node + 1, start, mid);                 // Left child
      build(arr, 2 * node + 2, mid + 1, end);               // Right child
      tree[node] = tree[2 * node + 1] + tree[2 * node + 2]; // Merge
    }
  }

  int query(int node, int start, int end, int L, int R) {
    if (R < start || L > end)
      return 0; // Outside range
    if (L <= start && end <= R)
      return tree[node]; // Inside range

    int mid = (start + end) / 2;
    int leftSum = query(2 * node + 1, start, mid, L, R);
    int rightSum = query(2 * node + 2, mid + 1, end, L, R);
    return leftSum + rightSum;
  }

  void update(int node, int start, int end, int idx, int newVal) {
    if (start == end) { // Leaf node
      tree[node] = newVal;
    } else {
      int mid = (start + end) / 2;
      if (idx <= mid)
        update(2 * node + 1, start, mid, idx, newVal);
      else
        update(2 * node + 2, mid + 1, end, idx, newVal);

      tree[node] = tree[2 * node + 1] + tree[2 * node + 2]; // Merge
    }
  }
};

int main() {
  vector<int> arr = {1, 3, 5, 7, 9, 11};
  SegmentTree st(arr);

  cout << "Sum of range [1, 3]: " << st.query(0, 0, arr.size() - 1, 1, 3)
       << endl;

  st.update(0, 0, arr.size() - 1, 2, 6);
  cout << "Sum of range [1, 3] after update: "
       << st.query(0, 0, arr.size() - 1, 1, 3) << endl;

  return 0;
}
