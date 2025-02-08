#include <bits/stdc++.h>
using namespace std;

class SegmentTreeLazy {
public:
  vector<int> tree, lazy;
  int n;

  SegmentTreeLazy(vector<int> &arr) {
    n = arr.size();
    tree.resize(4 * n);
    lazy.resize(4 * n, 0);
    build(arr, 0, 0, n - 1);
  }

  void build(vector<int> &arr, int node, int start, int end) {
    if (start == end) {
      tree[node] = arr[start];
    } else {
      int mid = (start + end) / 2;
      build(arr, 2 * node + 1, start, mid);
      build(arr, 2 * node + 2, mid + 1, end);
      tree[node] = tree[2 * node + 1] + tree[2 * node + 2];
    }
  }

  void propagate(int node, int start, int end) {
    if (lazy[node] != 0) {
      tree[node] += (end - start + 1) * lazy[node];
      if (start != end) {
        lazy[2 * node + 1] += lazy[node];
        lazy[2 * node + 2] += lazy[node];
      }
      lazy[node] = 0;
    }
  }

  void rangeUpdate(int node, int start, int end, int L, int R, int val) {
    propagate(node, start, end);

    if (R < start || L > end)
      return;

    if (L <= start && end <= R) {
      lazy[node] += val;
      propagate(node, start, end);
      return;
    }

    int mid = (start + end) / 2;
    rangeUpdate(2 * node + 1, start, mid, L, R, val);
    rangeUpdate(2 * node + 2, mid + 1, end, L, R, val);
    tree[node] = tree[2 * node + 1] + tree[2 * node + 2];
  }

  int query(int node, int start, int end, int L, int R) {
    propagate(node, start, end);

    if (R < start || L > end)
      return 0;

    if (L <= start && end <= R)
      return tree[node];

    int mid = (start + end) / 2;
    int leftSum = query(2 * node + 1, start, mid, L, R);
    int rightSum = query(2 * node + 2, mid + 1, end, L, R);
    return leftSum + rightSum;
  }
};

int main() {
  vector<int> arr = {1, 3, 5, 7, 9, 11};
  SegmentTreeLazy st(arr);

  cout << "Sum of range [1, 3]: " << st.query(0, 0, arr.size() - 1, 1, 3)
       << endl;

  st.rangeUpdate(0, 0, arr.size() - 1, 1, 3, 5);
  cout << "Sum of range [1, 3] after range update: "
       << st.query(0, 0, arr.size() - 1, 1, 3) << endl;

  return 0;
}
