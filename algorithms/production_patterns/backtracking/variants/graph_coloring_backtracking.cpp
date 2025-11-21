/*
 * Graph Coloring with Backtracking - Advanced Backtracking
 * 
 * Source: Graph algorithms, constraint satisfaction
 * Pattern: Recursive backtracking for graph coloring problems
 * 
 * What Makes It Ingenious:
 * - Constraint checking: Check adjacent vertices
 * - Heuristic ordering: Order vertices by degree
 * - Color ordering: Try colors in optimal order
 * - Early pruning: Stop when no valid color exists
 * - Used in register allocation, scheduling, map coloring
 * 
 * When to Use:
 * - Graph coloring problems
 * - Register allocation in compilers
 * - Scheduling problems
 * - Map coloring
 * - Frequency assignment
 * 
 * Real-World Usage:
 * - Compiler register allocation
 * - Map coloring algorithms
 * - Scheduling systems
 * - Frequency assignment in wireless networks
 * - Timetabling systems
 * 
 * Time Complexity: O(m * k^n) where k is colors, n is vertices
 * Space Complexity: O(n) for color assignment
 */

#include <vector>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <climits>

class GraphColoringBacktracking {
public:
    // Graph representation
    class Graph {
    private:
        int num_vertices_;
        std::vector<std::vector<int>> adjacency_list_;
        
    public:
        Graph(int n) : num_vertices_(n) {
            adjacency_list_.resize(n);
        }
        
        void add_edge(int u, int v) {
            adjacency_list_[u].push_back(v);
            adjacency_list_[v].push_back(u);
        }
        
        const std::vector<int>& get_neighbors(int v) const {
            return adjacency_list_[v];
        }
        
        int get_degree(int v) const {
            return adjacency_list_[v].size();
        }
        
        int num_vertices() const { return num_vertices_; }
    };
    
    // Graph coloring solver
    class GraphColoringSolver {
    private:
        const Graph& graph_;
        int num_colors_;
        std::vector<int> color_assignment_;
        bool solution_found_;
        
        // Check if color is safe for vertex
        bool is_safe(int vertex, int color) {
            for (int neighbor : graph_.get_neighbors(vertex)) {
                if (color_assignment_[neighbor] == color) {
                    return false;  // Conflict with neighbor
                }
            }
            return true;
        }
        
        // Select next vertex (heuristic: highest degree first)
        int select_next_vertex(const std::vector<bool>& colored) {
            int best_vertex = -1;
            int max_degree = -1;
            
            for (int v = 0; v < graph_.num_vertices(); v++) {
                if (!colored[v]) {
                    int degree = graph_.get_degree(v);
                    if (degree > max_degree) {
                        max_degree = degree;
                        best_vertex = v;
                    }
                }
            }
            
            return best_vertex;
        }
        
        // Recursive coloring with backtracking
        bool color_graph_recursive(int colored_count) {
            if (colored_count == graph_.num_vertices()) {
                solution_found_ = true;
                return true;  // All vertices colored
            }
            
            // Select next vertex to color
            std::vector<bool> colored(graph_.num_vertices(), false);
            for (int i = 0; i < graph_.num_vertices(); i++) {
                if (color_assignment_[i] != -1) {
                    colored[i] = true;
                }
            }
            
            int vertex = select_next_vertex(colored);
            if (vertex == -1) {
                return false;
            }
            
            // Try each color
            for (int color = 0; color < num_colors_; color++) {
                if (is_safe(vertex, color)) {
                    color_assignment_[vertex] = color;
                    
                    if (color_graph_recursive(colored_count + 1)) {
                        return true;
                    }
                    
                    // Backtrack
                    color_assignment_[vertex] = -1;
                }
            }
            
            return false;
        }
        
    public:
        GraphColoringSolver(const Graph& g, int colors)
            : graph_(g), num_colors_(colors), solution_found_(false) {
            color_assignment_.resize(graph_.num_vertices(), -1);
        }
        
        bool solve() {
            solution_found_ = false;
            return color_graph_recursive(0);
        }
        
        std::vector<int> get_coloring() const {
            return color_assignment_;
        }
        
        bool is_solution_found() const {
            return solution_found_;
        }
    };
    
    // Minimum graph coloring (find minimum number of colors)
    class MinimumColoringSolver {
    private:
        const Graph& graph_;
        int min_colors_;
        std::vector<int> best_coloring_;
        
        bool can_color_with_k_colors(int k) {
            GraphColoringSolver solver(graph_, k);
            if (solver.solve()) {
                best_coloring_ = solver.get_coloring();
                return true;
            }
            return false;
        }
        
    public:
        MinimumColoringSolver(const Graph& g)
            : graph_(g), min_colors_(INT_MAX) {}
        
        int solve() {
            // Binary search on number of colors
            int left = 1;
            int right = graph_.num_vertices();  // Upper bound: one color per vertex
            
            while (left <= right) {
                int mid = left + (right - left) / 2;
                
                if (can_color_with_k_colors(mid)) {
                    min_colors_ = mid;
                    right = mid - 1;  // Try fewer colors
                } else {
                    left = mid + 1;  // Need more colors
                }
            }
            
            return min_colors_;
        }
        
        std::vector<int> get_coloring() const {
            return best_coloring_;
        }
    };
};

// Example usage
int main() {
    // Create graph: triangle (needs 3 colors)
    GraphColoringBacktracking::Graph graph(3);
    graph.add_edge(0, 1);
    graph.add_edge(1, 2);
    graph.add_edge(2, 0);
    
    // Try 3-coloring
    GraphColoringBacktracking::GraphColoringSolver solver(graph, 3);
    if (solver.solve()) {
        std::cout << "Graph colored with 3 colors:" << std::endl;
        auto coloring = solver.get_coloring();
        for (size_t i = 0; i < coloring.size(); i++) {
            std::cout << "Vertex " << i << " -> Color " << coloring[i] << std::endl;
        }
    }
    
    // Find minimum coloring
    GraphColoringBacktracking::MinimumColoringSolver min_solver(graph);
    int min_colors = min_solver.solve();
    std::cout << "\nMinimum colors needed: " << min_colors << std::endl;
    
    return 0;
}

