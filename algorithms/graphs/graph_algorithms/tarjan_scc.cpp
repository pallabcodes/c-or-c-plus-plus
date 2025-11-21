// Tarjan's Algorithm: Find strongly connected components in directed graph
// Uses DFS with low-link values and stack
// Time: O(V + E)
// Space: O(V)

#include <vector>
#include <stack>
#include <iostream>
#include <algorithm>

class TarjanSCC {
private:
    std::vector<std::vector<int>> graph;
    std::vector<int> ids;
    std::vector<int> low;
    std::vector<bool> onStack;
    std::stack<int> stack;
    int id;
    int sccCount;
    std::vector<std::vector<int>> sccs;
    
    void dfs(int at) {
        stack.push(at);
        onStack[at] = true;
        ids[at] = low[at] = id++;
        
        for (int to : graph[at]) {
            if (ids[to] == -1) {
                dfs(to);
            }
            if (onStack[to]) {
                low[at] = std::min(low[at], low[to]);
            }
        }
        
        // Found SCC root
        if (ids[at] == low[at]) {
            std::vector<int> scc;
            while (true) {
                int node = stack.top();
                stack.pop();
                onStack[node] = false;
                low[node] = ids[at];
                scc.push_back(node);
                if (node == at) break;
            }
            sccs.push_back(scc);
            sccCount++;
        }
    }
    
public:
    TarjanSCC(const std::vector<std::vector<int>>& adjList) 
        : graph(adjList), id(0), sccCount(0) {
        int n = graph.size();
        ids.assign(n, -1);
        low.assign(n, 0);
        onStack.assign(n, false);
    }
    
    std::vector<std::vector<int>> findSCCs() {
        int n = graph.size();
        for (int i = 0; i < n; i++) {
            if (ids[i] == -1) {
                dfs(i);
            }
        }
        return sccs;
    }
    
    int getSCCCount() const {
        return sccCount;
    }
    
    std::vector<int> getLowLinkValues() const {
        return low;
    }
};

// Example usage
int main() {
    int n = 8;
    std::vector<std::vector<int>> graph(n);
    
    // Example graph
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
    
    TarjanSCC tarjan(graph);
    std::vector<std::vector<int>> sccs = tarjan.findSCCs();
    
    std::cout << "Number of strongly connected components: " 
              << tarjan.getSCCCount() << std::endl;
    
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

