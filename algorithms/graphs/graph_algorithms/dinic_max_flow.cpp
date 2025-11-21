// Dinic's Algorithm: Maximum flow in networks
// Based on research by Yefim Dinitz
// Time: O(V^2 * E) worst case, O(V * E) for unit capacity graphs
// Space: O(V + E)
// God modded implementation with level graph optimization

#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
#include <climits>

struct Edge {
    int to;
    int capacity;
    int flow;
    int rev;
    
    Edge(int t, int c, int r) : to(t), capacity(c), flow(0), rev(r) {}
};

class Dinic {
private:
    int n;
    std::vector<std::vector<Edge>> graph;
    std::vector<int> level;
    std::vector<int> ptr;
    
    bool bfs(int s, int t) {
        level.assign(n, -1);
        level[s] = 0;
        
        std::queue<int> q;
        q.push(s);
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (const Edge& e : graph[u]) {
                if (level[e.to] == -1 && e.flow < e.capacity) {
                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
            }
        }
        
        return level[t] != -1;
    }
    
    int dfs(int u, int t, int flow) {
        if (u == t) return flow;
        
        for (int& i = ptr[u]; i < graph[u].size(); i++) {
            Edge& e = graph[u][i];
            
            if (level[e.to] == level[u] + 1 && e.flow < e.capacity) {
                int pushed = dfs(e.to, t, std::min(flow, e.capacity - e.flow));
                
                if (pushed > 0) {
                    e.flow += pushed;
                    graph[e.to][e.rev].flow -= pushed;
                    return pushed;
                }
            }
        }
        
        return 0;
    }
    
public:
    Dinic(int nodes) : n(nodes), graph(nodes), level(nodes), ptr(nodes) {}
    
    void addEdge(int from, int to, int capacity) {
        Edge e1(to, capacity, graph[to].size());
        Edge e2(from, 0, graph[from].size());
        
        graph[from].push_back(e1);
        graph[to].push_back(e2);
    }
    
    int maxFlow(int s, int t) {
        int totalFlow = 0;
        
        while (bfs(s, t)) {
            ptr.assign(n, 0);
            
            while (int pushed = dfs(s, t, INT_MAX)) {
                totalFlow += pushed;
            }
        }
        
        return totalFlow;
    }
    
    std::vector<std::vector<int>> getFlow() {
        std::vector<std::vector<int>> flowMatrix(n, std::vector<int>(n, 0));
        
        for (int u = 0; u < n; u++) {
            for (const Edge& e : graph[u]) {
                if (e.flow > 0) {
                    flowMatrix[u][e.to] = e.flow;
                }
            }
        }
        
        return flowMatrix;
    }
};

// Example usage
int main() {
    Dinic d(6);
    
    // Source: 0, Sink: 5
    d.addEdge(0, 1, 16);
    d.addEdge(0, 2, 13);
    d.addEdge(1, 2, 10);
    d.addEdge(1, 3, 12);
    d.addEdge(2, 1, 4);
    d.addEdge(2, 4, 14);
    d.addEdge(3, 2, 9);
    d.addEdge(3, 5, 20);
    d.addEdge(4, 3, 7);
    d.addEdge(4, 5, 4);
    
    int maxFlow = d.maxFlow(0, 5);
    
    std::cout << "Maximum flow: " << maxFlow << std::endl;
    
    return 0;
}

