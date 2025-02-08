// void dfs(int node, vector<vector<int>> &adj, vector<bool> &visited,
//          stack<int> &stk) {
//   visited[node] = true;
//   for (int neighbor : adj[node]) {
//     if (!visited[neighbor])
//       dfs(neighbor, adj, visited, stk);
//   }
//   stk.push(node); // Add to stack after processing all children
// }

// vector<int> topologicalSortDFS(int n, vector<vector<int>> &edges) {
//   vector<vector<int>> adj(n);
//   for (auto &edge : edges)
//     adj[edge[0]].push_back(edge[1]);

//   vector<bool> visited(n, false);
//   stack<int> stk;

//   for (int i = 0; i < n; i++)
//     if (!visited[i])
//       dfs(i, adj, visited, stk);

//   vector<int> topoOrder;
//   while (!stk.empty()) {
//     topoOrder.push_back(stk.top());
//     stk.pop();
//   }
//   return topoOrder;
// }
