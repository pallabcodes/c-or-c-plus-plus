// Suurballe's Algorithm: Find two edge-disjoint shortest paths
// Based on research by Suurballe
// Time: O(m log n + k) where k is path length
// Space: O(n + m)
// God modded implementation for network reliability

#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
#include <climits>

struct Edge {
    int to;
    int weight;
    int id;
    
    Edge(int t, int w, int i) : to(t), weight(w), id(i) {}
};

class Graph {
public:
    int n;
    std::vector<std::vector<Edge>> adj;
    
    Graph(int nodes) : n(nodes), adj(nodes) {}
    
    void addEdge(int from, int to, int weight, int id) {
        adj[from].push_back(Edge(to, weight, id));
    }
};

// Dijkstra's algorithm
std::pair<std::vector<int>, std::vector<int>> dijkstra(const Graph& g, int src) {
    std::vector<int> dist(g.n, INT_MAX);
    std::vector<int> parent(g.n, -1);
    std::priority_queue<std::pair<int, int>, 
                       std::vector<std::pair<int, int>>,
                       std::greater<std::pair<int, int>>> pq;
    
    dist[src] = 0;
    pq.push({0, src});
    
    while (!pq.empty()) {
        int u = pq.top().second;
        int d = pq.top().first;
        pq.pop();
        
        if (d > dist[u]) continue;
        
        for (const Edge& e : g.adj[u]) {
            if (dist[e.to] > dist[u] + e.weight) {
                dist[e.to] = dist[u] + e.weight;
                parent[e.to] = u;
                pq.push({dist[e.to], e.to});
            }
        }
    }
    
    return {dist, parent};
}

// Suurballe's algorithm for two edge-disjoint shortest paths
std::pair<std::vector<int>, std::vector<int>> suurballe(const Graph& g, int src, int dest) {
    auto [dist1, parent1] = dijkstra(g, src);
    
    if (dist1[dest] == INT_MAX) {
        return {{}, {}};
    }
    
    Graph modifiedGraph = g;
    
    for (int u = 0; u < g.n; u++) {
        for (auto& e : modifiedGraph.adj[u]) {
            int v = e.to;
            e.weight = e.weight - dist1[v] + dist1[u];
        }
    }
    
    std::vector<int> path1;
    int curr = dest;
    while (curr != -1) {
        path1.push_back(curr);
        curr = parent1[curr];
    }
    std::reverse(path1.begin(), path1.end());
    
    for (size_t i = 0; i < path1.size() - 1; i++) {
        int u = path1[i];
        int v = path1[i + 1];
        
        for (auto& e : modifiedGraph.adj[u]) {
            if (e.to == v) {
                modifiedGraph.adj[v].push_back(Edge(u, -e.weight, e.id));
                break;
            }
        }
    }
    
    auto [dist2, parent2] = dijkstra(modifiedGraph, src);
    
    std::vector<int> path2;
    curr = dest;
    while (curr != -1) {
        path2.push_back(curr);
        curr = parent2[curr];
    }
    std::reverse(path2.begin(), path2.end());
    
    return {path1, path2};
}

// Example usage
int main() {
    Graph g(6);
    
    g.addEdge(0, 1, 1, 0);
    g.addEdge(0, 2, 2, 1);
    g.addEdge(1, 2, 1, 2);
    g.addEdge(1, 3, 3, 3);
    g.addEdge(2, 3, 1, 4);
    g.addEdge(2, 4, 2, 5);
    g.addEdge(3, 4, 1, 6);
    g.addEdge(3, 5, 2, 7);
    g.addEdge(4, 5, 1, 8);
    
    auto [path1, path2] = suurballe(g, 0, 5);
    
    std::cout << "Path 1: ";
    for (int node : path1) {
        std::cout << node << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Path 2: ";
    for (int node : path2) {
        std::cout << node << " ";
    }
    std::cout << std::endl;
    
    return 0;
}

