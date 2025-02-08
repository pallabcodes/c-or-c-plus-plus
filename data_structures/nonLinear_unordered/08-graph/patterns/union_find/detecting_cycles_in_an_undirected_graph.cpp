// bool hasCycle(int n, vector<vector<int>> &edges) {
//   UnionFind uf(n);
//   for (auto &edge : edges) {
//     int u = edge[0], v = edge[1];
//     if (uf.find(u) == uf.find(v))
//       return true; // Cycle detected
//     uf.unite(u, v);
//   }
//   return false;
// }
