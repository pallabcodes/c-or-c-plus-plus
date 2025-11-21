// Hopcroft-Karp Algorithm: Maximum bipartite matching
// Based on research paper by Hopcroft and Karp
// Time: O(sqrt(n) * m) where n is nodes, m is edges
// Space: O(n + m)
// God modded implementation for maximum matching in bipartite graphs

#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
#include <climits>

class HopcroftKarp {
private:
    int nLeft, nRight;
    std::vector<std::vector<int>> graph;
    std::vector<int> pairU, pairV, dist;
    
    bool bfs() {
        std::queue<int> q;
        
        for (int u = 0; u < nLeft; u++) {
            if (pairU[u] == -1) {
                dist[u] = 0;
                q.push(u);
            } else {
                dist[u] = INT_MAX;
            }
        }
        
        dist[nLeft] = INT_MAX;
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            if (dist[u] < dist[nLeft]) {
                for (int v : graph[u]) {
                    if (dist[pairV[v]] == INT_MAX) {
                        dist[pairV[v]] = dist[u] + 1;
                        q.push(pairV[v]);
                    }
                }
            }
        }
        
        return dist[nLeft] != INT_MAX;
    }
    
    bool dfs(int u) {
        if (u == nLeft) return true;
        
        for (int v : graph[u]) {
            if (dist[pairV[v]] == dist[u] + 1) {
                if (dfs(pairV[v])) {
                    pairU[u] = v;
                    pairV[v] = u;
                    return true;
                }
            }
        }
        
        dist[u] = INT_MAX;
        return false;
    }
    
public:
    HopcroftKarp(int left, int right) 
        : nLeft(left), nRight(right), 
          graph(left), pairU(left, -1), pairV(right, -1), dist(left + 1) {}
    
    void addEdge(int u, int v) {
        graph[u].push_back(v);
    }
    
    int maxMatching() {
        int matching = 0;
        
        while (bfs()) {
            for (int u = 0; u < nLeft; u++) {
                if (pairU[u] == -1 && dfs(u)) {
                    matching++;
                }
            }
        }
        
        return matching;
    }
    
    std::vector<std::pair<int, int>> getMatching() {
        std::vector<std::pair<int, int>> result;
        for (int u = 0; u < nLeft; u++) {
            if (pairU[u] != -1) {
                result.push_back({u, pairU[u]});
            }
        }
        return result;
    }
};

// Example usage
int main() {
    // Bipartite graph: Left nodes {0, 1, 2, 3}, Right nodes {0, 1, 2, 3}
    HopcroftKarp hk(4, 4);
    
    hk.addEdge(0, 1);
    hk.addEdge(0, 2);
    hk.addEdge(1, 0);
    hk.addEdge(1, 3);
    hk.addEdge(2, 2);
    hk.addEdge(3, 2);
    hk.addEdge(3, 3);
    
    int matching = hk.maxMatching();
    
    std::cout << "Maximum matching size: " << matching << std::endl;
    
    auto matches = hk.getMatching();
    std::cout << "Matching edges: ";
    for (const auto& edge : matches) {
        std::cout << "(" << edge.first << ", " << edge.second << ") ";
    }
    std::cout << std::endl;
    
    return 0;
}

