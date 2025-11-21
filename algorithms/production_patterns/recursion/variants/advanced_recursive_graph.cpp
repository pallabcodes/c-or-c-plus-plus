/*
 * Advanced Recursive Graph Algorithms
 * 
 * Source: Production graph algorithms, research papers
 * Pattern: Advanced recursive patterns for graph problems
 * 
 * What Makes It Ingenious:
 * - Recursive DFS with advanced patterns
 * - Strongly connected components (Tarjan's algorithm)
 * - Articulation points and bridges
 * - Recursive path finding
 * - Used in compilers, network analysis, social networks
 * 
 * When to Use:
 * - Graph traversal with advanced requirements
 * - Finding cycles, components, critical nodes
 * - Network analysis
 * - Compiler dependency resolution
 * - Social network analysis
 * 
 * Real-World Usage:
 * - Compiler dependency graphs
 * - Network routing algorithms
 * - Social network analysis
 * - Web crawlers
 * - Dependency resolution systems
 * 
 * Time Complexity: O(V + E) typically
 * Space Complexity: O(V) for recursion stack
 */

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <stack>
#include <iostream>

class AdvancedRecursiveGraph {
public:
    // Graph representation
    struct Graph {
        int vertices;
        std::vector<std::vector<int>> adj_list;
        
        Graph(int v) : vertices(v) {
            adj_list.resize(v);
        }
        
        void add_edge(int u, int v) {
            adj_list[u].push_back(v);
        }
    };
    
    // Tarjan's algorithm for strongly connected components (recursive)
    static void tarjan_scc_recursive(
        const Graph& graph,
        std::vector<std::vector<int>>& components) {
        
        std::vector<int> disc(graph.vertices, -1);
        std::vector<int> low(graph.vertices, -1);
        std::vector<bool> in_stack(graph.vertices, false);
        std::stack<int> st;
        int time = 0;
        
        std::function<void(int)> dfs = [&](int u) {
            disc[u] = low[u] = ++time;
            st.push(u);
            in_stack[u] = true;
            
            for (int v : graph.adj_list[u]) {
                if (disc[v] == -1) {
                    dfs(v);
                    low[u] = std::min(low[u], low[v]);
                } else if (in_stack[v]) {
                    low[u] = std::min(low[u], disc[v]);
                }
            }
            
            if (low[u] == disc[u]) {
                std::vector<int> component;
                while (st.top() != u) {
                    int v = st.top();
                    st.pop();
                    in_stack[v] = false;
                    component.push_back(v);
                }
                int v = st.top();
                st.pop();
                in_stack[v] = false;
                component.push_back(v);
                components.push_back(component);
            }
        };
        
        for (int i = 0; i < graph.vertices; i++) {
            if (disc[i] == -1) {
                dfs(i);
            }
        }
    }
    
    // Find articulation points (cut vertices) recursively
    static void find_articulation_points_recursive(
        const Graph& graph,
        std::vector<bool>& is_articulation) {
        
        is_articulation.assign(graph.vertices, false);
        std::vector<int> disc(graph.vertices, -1);
        std::vector<int> low(graph.vertices, -1);
        std::vector<int> parent(graph.vertices, -1);
        int time = 0;
        
        std::function<void(int)> dfs = [&](int u) {
            disc[u] = low[u] = ++time;
            int children = 0;
            
            for (int v : graph.adj_list[u]) {
                if (disc[v] == -1) {
                    children++;
                    parent[v] = u;
                    dfs(v);
                    
                    low[u] = std::min(low[u], low[v]);
                    
                    // Root with multiple children is articulation point
                    if (parent[u] == -1 && children > 1) {
                        is_articulation[u] = true;
                    }
                    
                    // Non-root: u is articulation if low[v] >= disc[u]
                    if (parent[u] != -1 && low[v] >= disc[u]) {
                        is_articulation[u] = true;
                    }
                } else if (v != parent[u]) {
                    low[u] = std::min(low[u], disc[v]);
                }
            }
        };
        
        for (int i = 0; i < graph.vertices; i++) {
            if (disc[i] == -1) {
                dfs(i);
            }
        }
    }
    
    // Find bridges (cut edges) recursively
    static void find_bridges_recursive(
        const Graph& graph,
        std::vector<std::pair<int, int>>& bridges) {
        
        std::vector<int> disc(graph.vertices, -1);
        std::vector<int> low(graph.vertices, -1);
        std::vector<int> parent(graph.vertices, -1);
        int time = 0;
        
        std::function<void(int)> dfs = [&](int u) {
            disc[u] = low[u] = ++time;
            
            for (int v : graph.adj_list[u]) {
                if (disc[v] == -1) {
                    parent[v] = u;
                    dfs(v);
                    
                    low[u] = std::min(low[u], low[v]);
                    
                    // Bridge found: low[v] > disc[u]
                    if (low[v] > disc[u]) {
                        bridges.push_back({u, v});
                    }
                } else if (v != parent[u]) {
                    low[u] = std::min(low[u], disc[v]);
                }
            }
        };
        
        for (int i = 0; i < graph.vertices; i++) {
            if (disc[i] == -1) {
                dfs(i);
            }
        }
    }
    
    // Recursive path finding with backtracking
    static bool find_path_recursive(
        const Graph& graph,
        int start,
        int end,
        std::vector<int>& path,
        std::vector<bool>& visited) {
        
        if (start == end) {
            path.push_back(end);
            return true;
        }
        
        visited[start] = true;
        path.push_back(start);
        
        for (int neighbor : graph.adj_list[start]) {
            if (!visited[neighbor]) {
                if (find_path_recursive(graph, neighbor, end, path, visited)) {
                    return true;
                }
            }
        }
        
        // Backtrack
        path.pop_back();
        visited[start] = false;
        return false;
    }
    
    // Find all paths between two nodes (recursive)
    static void find_all_paths_recursive(
        const Graph& graph,
        int start,
        int end,
        std::vector<int>& current_path,
        std::vector<bool>& visited,
        std::vector<std::vector<int>>& all_paths) {
        
        visited[start] = true;
        current_path.push_back(start);
        
        if (start == end) {
            all_paths.push_back(current_path);
        } else {
            for (int neighbor : graph.adj_list[start]) {
                if (!visited[neighbor]) {
                    find_all_paths_recursive(graph, neighbor, end, 
                                           current_path, visited, all_paths);
                }
            }
        }
        
        // Backtrack
        current_path.pop_back();
        visited[start] = false;
    }
    
    // Recursive cycle detection
    static bool has_cycle_recursive(
        const Graph& graph,
        int vertex,
        std::vector<int>& color) {
        
        color[vertex] = 1;  // Gray: being processed
        
        for (int neighbor : graph.adj_list[vertex]) {
            if (color[neighbor] == 1) {
                return true;  // Back edge found (cycle)
            } else if (color[neighbor] == 0) {
                if (has_cycle_recursive(graph, neighbor, color)) {
                    return true;
                }
            }
        }
        
        color[vertex] = 2;  // Black: processed
        return false;
    }
    
    // Recursive topological sort
    static bool topological_sort_recursive(
        const Graph& graph,
        std::vector<int>& result) {
        
        std::vector<int> color(graph.vertices, 0);
        std::vector<int> finish_time(graph.vertices, 0);
        int time = 0;
        
        std::function<bool(int)> dfs = [&](int u) {
            color[u] = 1;
            
            for (int v : graph.adj_list[u]) {
                if (color[v] == 1) {
                    return false;  // Cycle detected
                } else if (color[v] == 0) {
                    if (!dfs(v)) {
                        return false;
                    }
                }
            }
            
            color[u] = 2;
            finish_time[u] = time++;
            return true;
        };
        
        for (int i = 0; i < graph.vertices; i++) {
            if (color[i] == 0) {
                if (!dfs(i)) {
                    return false;  // Cycle found
                }
            }
        }
        
        // Sort by finish time
        std::vector<std::pair<int, int>> nodes;
        for (int i = 0; i < graph.vertices; i++) {
            nodes.push_back({finish_time[i], i});
        }
        std::sort(nodes.rbegin(), nodes.rend());
        
        for (const auto& p : nodes) {
            result.push_back(p.second);
        }
        
        return true;
    }
};

// Example usage
int main() {
    // Create graph
    AdvancedRecursiveGraph::Graph graph(5);
    graph.add_edge(0, 1);
    graph.add_edge(1, 2);
    graph.add_edge(2, 0);  // Creates cycle
    graph.add_edge(1, 3);
    graph.add_edge(3, 4);
    
    // Find strongly connected components
    std::vector<std::vector<int>> components;
    AdvancedRecursiveGraph::tarjan_scc_recursive(graph, components);
    
    std::cout << "Strongly Connected Components:" << std::endl;
    for (const auto& comp : components) {
        for (int v : comp) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }
    
    // Find articulation points
    std::vector<bool> is_articulation;
    AdvancedRecursiveGraph::find_articulation_points_recursive(graph, is_articulation);
    
    std::cout << "\nArticulation Points:" << std::endl;
    for (int i = 0; i < graph.vertices; i++) {
        if (is_articulation[i]) {
            std::cout << i << " ";
        }
    }
    std::cout << std::endl;
    
    return 0;
}

