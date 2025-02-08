// string alienOrder(vector<string> &words) {
//   unordered_map<char, vector<char>> adj;
//   unordered_map<char, int> inDegree;
//   for (string word : words)
//     for (char c : word)
//       inDegree[c] = 0;

//   for (int i = 0; i < words.size() - 1; i++) {
//     string w1 = words[i], w2 = words[i + 1];
//     int len = min(w1.size(), w2.size());

//     if (w1.size() > w2.size() && w1.substr(0, len) == w2.substr(0, len))
//       return ""; // Invalid order (prefix issue)

//     for (int j = 0; j < len; j++) {
//       if (w1[j] != w2[j]) {
//         adj[w1[j]].push_back(w2[j]);
//         inDegree[w2[j]]++;
//         break;
//       }
//     }
//   }

//   queue<char> q;
//   string order = "";
//   for (auto &[ch, deg] : inDegree)
//     if (deg == 0)
//       q.push(ch);

//   while (!q.empty()) {
//     char ch = q.front();
//     q.pop();
//     order += ch;
//     for (char neighbor : adj[ch])
//       if (--inDegree[neighbor] == 0)
//         q.push(neighbor);
//   }

//   return order.size() == inDegree.size() ? order : "";
// }
