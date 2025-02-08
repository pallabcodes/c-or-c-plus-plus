// class UnionFind {
// public:
//   vector<int> parent, rank;

//   UnionFind(int n) {
//     parent.resize(n);
//     rank.resize(n, 1);
//     for (int i = 0; i < n; i++)
//       parent[i] = i;
//   }

//   int find(int x) {
//     if (parent[x] != x)
//       parent[x] = find(parent[x]); // Path compression
//     return parent[x];
//   }

//   void unite(int x, int y) {
//     int rootX = find(x), rootY = find(y);
//     if (rootX != rootY) {
//       if (rank[rootX] > rank[rootY])
//         parent[rootY] = rootX;
//       else if (rank[rootX] < rank[rootY])
//         parent[rootX] = rootY;
//       else {
//         parent[rootY] = rootX;
//         rank[rootX]++;
//       }
//     }
//   }
// };
