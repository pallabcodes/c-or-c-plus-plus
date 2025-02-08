// vector<int> findOrder(int numCourses, vector<vector<int>> &prerequisites) {
//   vector<int> inDegree(numCourses, 0);
//   vector<vector<int>> adj(numCourses);
//   for (auto &pre : prerequisites) {
//     adj[pre[1]].push_back(pre[0]);
//     inDegree[pre[0]]++;
//   }

//   queue<int> q;
//   for (int i = 0; i < numCourses; i++)
//     if (inDegree[i] == 0)
//       q.push(i);

//   vector<int> order;
//   while (!q.empty()) {
//     int course = q.front();
//     q.pop();
//     order.push_back(course);

//     for (int neighbor : adj[course])
//       if (--inDegree[neighbor] == 0)
//         q.push(neighbor);
//   }

//   return order.size() == numCourses ? order : vector<int>{}; // Cycle
//   detected
// }
