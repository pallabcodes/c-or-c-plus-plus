// vector<int> findRedundantConnection(vector<vector<int>> &edges) {
//   int n = edges.size();
//   UnionFind uf(n + 1); // 1-based index
//   for (auto &edge : edges) {
//     if (uf.find(edge[0]) == uf.find(edge[1]))
//       return edge;
//     uf.unite(edge[0], edge[1]);
//   }
//   return {};
// }
