// Kosaraju's Algorithm: Find strongly connected components using two DFS passes
// First pass: Fill order stack, Second pass: Process in reverse order
// Time: O(V + E)
// Space: O(V)

#include <vector>
#include <stack>
#include <iostream>
#include <algorithm>

class KosarajuSCC {
private:
    std::vector<std::vector<int>> graph;
    std::vector<std::vector<int>> reverseGraph;
    std::vector<bool> visited;
    std::stack<int> order;
    std::vector<std::vector<int>> sccs;
    
    void dfs1(int node) {
        visited[node] = true;
        for (int neighbor : graph[node]) {
            if (!visited[neighbor]) {
                dfs1(neighbor);
            }
        }
        order.push(node);
    }
    
    void dfs2(int node, std::vector<int>& scc) {
        visited[node] = true;
        scc.push_back(node);
        for (int neighbor : reverseGraph[node]) {
            if (!visited[neighbor]) {
                dfs2(neighbor, scc);
            }
        }
    }
    
public:
    KosarajuSCC(const std::vector<std::vector<int>>& adjList) 
        : graph(adjList) {
        int n = graph.size();
        reverseGraph.assign(n, std::vector<int>());
        visited.assign(n, false);
        
        // Build reverse graph
        for (int i = 0; i < n; i++) {
            for (int j : graph[i]) {
                reverseGraph[j].push_back(i);
            }
        }
    }
    
    std::vector<std::vector<int>> findSCCs() {
        int n = graph.size();
        
        // First DFS: Fill order stack
        for (int i = 0; i < n; i++) {
            if (!visited[i]) {
                dfs1(i);
            }
        }
        
        // Reset visited array
        visited.assign(n, false);
        
        // Second DFS: Process in reverse order
        while (!order.empty()) {
            int node = order.top();
            order.pop();
            
            if (!visited[node]) {
                std::vector<int> scc;
                dfs2(node, scc);
                sccs.push_back(scc);
            }
        }
        
        return sccs;
    }
    
    int getSCCCount() const {
        return sccs.size();
    }
};

// Example usage
int main() {
    int n = 8;
    std::vector<std::vector<int>> graph(n);
    
    // Example graph (same as Tarjan example)
    graph[0].push_back(1);
    graph[1].push_back(2);
    graph[2].push_back(0);
    graph[2].push_back(3);
    graph[3].push_back(4);
    graph[4].push_back(5);
    graph[5].push_back(3);
    graph[6].push_back(5);
    graph[6].push_back(7);
    graph[7].push_back(6);
    
    KosarajuSCC kosaraju(graph);
    std::vector<std::vector<int>> sccs = kosaraju.findSCCs();
    
    std::cout << "Number of strongly connected components: " 
              << kosaraju.getSCCCount() << std::endl;
    
    std::cout << "Strongly Connected Components:" << std::endl;
    for (size_t i = 0; i < sccs.size(); i++) {
        std::cout << "SCC " << i << ": ";
        for (int node : sccs[i]) {
            std::cout << node << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}

