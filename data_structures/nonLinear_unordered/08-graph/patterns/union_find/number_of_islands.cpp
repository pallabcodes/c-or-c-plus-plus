// class Solution {
// public:
//     int numIslands(vector<vector<char>>& grid) {
//         int m = grid.size(), n = grid[0].size();
//         UnionFind uf(m * n);
//         int directions[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
//         int islandCount = 0;

//         for (int i = 0; i < m; i++)
//             for (int j = 0; j < n; j++)
//                 if (grid[i][j] == '1') {
//                     islandCount++;
//                     for (auto &d : directions) {
//                         int x = i + d[0], y = j + d[1];
//                         if (x >= 0 && x < m && y >= 0 && y < n && grid[x][y]
//                         == '1') {
//                             int id1 = i * n + j, id2 = x * n + y;
//                             if (uf.find(id1) != uf.find(id2)) {
//                                 uf.unite(id1, id2);
//                                 islandCount--;
//                             }
//                         }
//                     }
//                 }
//         return islandCount;
//     }
// };
