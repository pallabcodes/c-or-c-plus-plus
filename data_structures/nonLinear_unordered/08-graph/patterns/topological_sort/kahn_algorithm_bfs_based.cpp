// vector<int> topologicalSort(int n, vector<vector<int>> &edges) {
//   vector<int> inDegree(n, 0);
//   vector<vector<int>> adj(n);
//   vector<int> topoOrder;

//   // Construct adjacency list and calculate in-degrees
//   for (auto &edge : edges) {
//     adj[edge[0]].push_back(edge[1]);
//     inDegree[edge[1]]++;
//   }

//   // Push all nodes with in-degree 0 into queue
//   queue<int> q;
//   for (int i = 0; i < n; i++)
//     if (inDegree[i] == 0)
//       q.push(i);

//   // Process nodes
//   while (!q.empty()) {
//     int node = q.front();
//     q.pop();
//     topoOrder.push_back(node);

//     for (int neighbor : adj[node]) {
//       if (--inDegree[neighbor] == 0)
//         q.push(neighbor);
//     }
//   }

//   return topoOrder.size() == n ? topoOrder
//                                : vector<int>{}; // Return empty if cycle
//                                exists
// }
