// Yen's K-Shortest Paths Algorithm
// Finds K shortest paths between two nodes in a graph
// Based on research paper by Jin Y. Yen
// Time: O(K * n * (m + n log n)) where n is nodes, m is edges
// Space: O(K * n)

#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
#include <climits>
#include <set>

struct Edge {
    int to;
    int weight;
    
    Edge(int t, int w) : to(t), weight(w) {}
};

class Graph {
public:
    int n;
    std::vector<std::vector<Edge>> adj;
    
    Graph(int nodes) : n(nodes), adj(nodes) {}
    
    void addEdge(int from, int to, int weight) {
        adj[from].push_back(Edge(to, weight));
    }
};

// Dijkstra's algorithm for shortest path
std::pair<int, std::vector<int>> dijkstra(const Graph& g, int src, int dest, 
                                          const std::set<std::pair<int, int>>& blockedEdges = {}) {
    std::vector<int> dist(g.n, INT_MAX);
    std::vector<int> parent(g.n, -1);
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, 
                       std::greater<std::pair<int, int>>> pq;
    
    dist[src] = 0;
    pq.push({0, src});
    
    while (!pq.empty()) {
        int u = pq.top().second;
        int d = pq.top().first;
        pq.pop();
        
        if (d > dist[u]) continue;
        if (u == dest) break;
        
        for (const Edge& e : g.adj[u]) {
            if (blockedEdges.count({u, e.to})) continue;
            
            if (dist[e.to] > dist[u] + e.weight) {
                dist[e.to] = dist[u] + e.weight;
                parent[e.to] = u;
                pq.push({dist[e.to], e.to});
            }
        }
    }
    
    if (dist[dest] == INT_MAX) {
        return {INT_MAX, {}};
    }
    
    std::vector<int> path;
    int curr = dest;
    while (curr != -1) {
        path.push_back(curr);
        curr = parent[curr];
    }
    std::reverse(path.begin(), path.end());
    
    return {dist[dest], path};
}

// Yen's K-shortest paths algorithm
std::vector<std::pair<int, std::vector<int>>> yenKShortestPaths(const Graph& g, int src, int dest, int k) {
    std::vector<std::pair<int, std::vector<int>>> A; // List of k shortest paths
    std::priority_queue<std::pair<int, std::vector<int>>, 
                       std::vector<std::pair<int, std::vector<int>>>,
                       std::function<bool(const std::pair<int, std::vector<int>>&, 
                                        const std::pair<int, std::vector<int>>&)>> 
        B([](const auto& a, const auto& b) { return a.first > b.first; }); // Candidate paths
    
    // Find shortest path
    auto firstPath = dijkstra(g, src, dest);
    if (firstPath.first == INT_MAX) {
        return {};
    }
    A.push_back(firstPath);
    
    for (int kth = 1; kth < k; kth++) {
        if (A.empty()) break;
        
        std::vector<int> prevPath = A[kth - 1].second;
        
        for (size_t i = 0; i < prevPath.size() - 1; i++) {
            int spurNode = prevPath[i];
            std::vector<int> rootPath(prevPath.begin(), prevPath.begin() + i + 1);
            
            std::set<std::pair<int, int>> blockedEdges;
            
            for (const auto& path : A) {
                if (path.second.size() > i + 1) {
                    bool matches = true;
                    for (size_t j = 0; j <= i; j++) {
                        if (path.second[j] != rootPath[j]) {
                            matches = false;
                            break;
                        }
                    }
                    if (matches) {
                        blockedEdges.insert({path.second[i], path.second[i + 1]});
                    }
                }
            }
            
            std::set<int> blockedNodes;
            for (size_t j = 0; j < rootPath.size() - 1; j++) {
                blockedNodes.insert(rootPath[j]);
            }
            
            auto spurResult = dijkstra(g, spurNode, dest, blockedEdges);
            
            if (spurResult.first != INT_MAX) {
                std::vector<int> totalPath = rootPath;
                totalPath.insert(totalPath.end(), 
                               spurResult.second.begin() + 1, 
                               spurResult.second.end());
                
                int totalCost = 0;
                for (size_t j = 0; j < totalPath.size() - 1; j++) {
                    for (const Edge& e : g.adj[totalPath[j]]) {
                        if (e.to == totalPath[j + 1]) {
                            totalCost += e.weight;
                            break;
                        }
                    }
                }
                
                B.push({totalCost, totalPath});
            }
        }
        
        if (B.empty()) break;
        
        A.push_back(B.top());
        B.pop();
        
        while (!B.empty() && B.top().second == A.back().second) {
            B.pop();
        }
    }
    
    return A;
}

// Example usage
int main() {
    Graph g(6);
    g.addEdge(0, 1, 4);
    g.addEdge(0, 2, 2);
    g.addEdge(1, 2, 1);
    g.addEdge(1, 3, 5);
    g.addEdge(2, 3, 8);
    g.addEdge(2, 4, 10);
    g.addEdge(3, 4, 2);
    g.addEdge(3, 5, 6);
    g.addEdge(4, 5, 3);
    
    int src = 0, dest = 5, k = 3;
    
    std::cout << "Finding " << k << " shortest paths from " << src << " to " << dest << std::endl;
    
    auto paths = yenKShortestPaths(g, src, dest, k);
    
    for (size_t i = 0; i < paths.size(); i++) {
        std::cout << "Path " << (i + 1) << " (cost: " << paths[i].first << "): ";
        for (int node : paths[i].second) {
            std::cout << node << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}

