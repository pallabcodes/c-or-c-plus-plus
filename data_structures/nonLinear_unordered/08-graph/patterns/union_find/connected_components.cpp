// int countComponents(int n, vector<vector<int>>& edges) {
//     UnionFind uf(n);
//     for (auto &edge : edges) uf.unite(edge[0], edge[1]);

//     unordered_set<int> uniqueParents;
//     for (int i = 0; i < n; i++) uniqueParents.insert(uf.find(i));

//     return uniqueParents.size();
// }
