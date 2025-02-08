// int kruskalMST(int n, vector<vector<int>> &edges) {
//   sort(edges.begin(), edges.end(),
//        [](auto &a, auto &b) { return a[2] < b[2]; });
//   UnionFind uf(n);
//   int mstCost = 0, count = 0;

//   for (auto &edge : edges) {
//     int u = edge[0], v = edge[1], weight = edge[2];
//     if (uf.find(u) != uf.find(v)) {
//       uf.unite(u, v);
//       mstCost += weight;
//       count++;
//     }
//     if (count == n - 1)
//       break; // Early stopping when we get n-1 edges
//   }
//   return mstCost;
// }
