/*
 * Hamiltonian Cycle with Backtracking - Advanced Backtracking
 * 
 * Source: Graph algorithms, path finding
 * Pattern: Recursive backtracking to find Hamiltonian cycles
 * 
 * What Makes It Ingenious:
 * - Cycle detection: Find cycle visiting all vertices once
 * - Path validation: Check if path can form cycle
 * - Early pruning: Stop when path can't complete cycle
 * - Used in TSP, route planning, circuit design
 * 
 * When to Use:
 * - Hamiltonian cycle problems
 * - Traveling salesman (finding tours)
 * - Route planning
 * - Circuit design
 * - Path finding with constraints
 * 
 * Real-World Usage:
 * - TSP solvers
 * - Route optimization
 * - Circuit board design
 * - Network routing
 * - Delivery route planning
 * 
 * Time Complexity: O(n!) worst case, O(2^n) with optimizations
 * Space Complexity: O(n) for path storage
 */

#include <vector>
#include <unordered_set>
#include <iostream>

class HamiltonianCycleBacktracking {
public:
    // Graph representation
    class Graph {
    private:
        int num_vertices_;
        std::vector<std::vector<bool>> adjacency_matrix_;
        
    public:
        Graph(int n) : num_vertices_(n) {
            adjacency_matrix_.resize(n, std::vector<bool>(n, false));
        }
        
        void add_edge(int u, int v) {
            adjacency_matrix_[u][v] = true;
            adjacency_matrix_[v][u] = true;
        }
        
        bool has_edge(int u, int v) const {
            return adjacency_matrix_[u][v];
        }
        
        int num_vertices() const { return num_vertices_; }
    };
    
    // Hamiltonian cycle solver
    class HamiltonianCycleSolver {
    private:
        const Graph& graph_;
        std::vector<int> path_;
        std::vector<bool> visited_;
        bool cycle_found_;
        
        // Check if vertex can be added to path
        bool is_safe(int vertex, int pos) {
            // Check if vertex already in path
            if (visited_[vertex]) {
                return false;
            }
            
            // Check if edge exists from last vertex to this vertex
            if (pos > 0 && !graph_.has_edge(path_[pos - 1], vertex)) {
                return false;
            }
            
            return true;
        }
        
        // Recursive backtracking search
        bool hamiltonian_cycle_recursive(int pos) {
            // Check if all vertices visited
            if (pos == graph_.num_vertices()) {
                // Check if last vertex connects to first
                if (graph_.has_edge(path_[pos - 1], path_[0])) {
                    cycle_found_ = true;
                    return true;
                }
                return false;
            }
            
            // Try each vertex (except start for positions > 0)
            int start = (pos == 0) ? 0 : 1;
            for (int v = start; v < graph_.num_vertices(); v++) {
                if (is_safe(v, pos)) {
                    path_[pos] = v;
                    visited_[v] = true;
                    
                    if (hamiltonian_cycle_recursive(pos + 1)) {
                        return true;
                    }
                    
                    // Backtrack
                    visited_[v] = false;
                    path_[pos] = -1;
                }
            }
            
            return false;
        }
        
    public:
        HamiltonianCycleSolver(const Graph& g)
            : graph_(g), cycle_found_(false) {
            path_.resize(graph_.num_vertices(), -1);
            visited_.resize(graph_.num_vertices(), false);
        }
        
        bool solve() {
            // Start from vertex 0
            path_[0] = 0;
            visited_[0] = true;
            
            cycle_found_ = false;
            return hamiltonian_cycle_recursive(1);
        }
        
        std::vector<int> get_cycle() const {
            if (cycle_found_) {
                std::vector<int> cycle = path_;
                cycle.push_back(path_[0]);  // Complete cycle
                return cycle;
            }
            return {};
        }
    };
    
    // Hamiltonian path solver (doesn't need to form cycle)
    class HamiltonianPathSolver {
    private:
        const Graph& graph_;
        std::vector<int> path_;
        std::vector<bool> visited_;
        bool path_found_;
        
        bool is_safe(int vertex, int pos) {
            if (visited_[vertex]) {
                return false;
            }
            
            if (pos > 0 && !graph_.has_edge(path_[pos - 1], vertex)) {
                return false;
            }
            
            return true;
        }
        
        bool hamiltonian_path_recursive(int pos) {
            if (pos == graph_.num_vertices()) {
                path_found_ = true;
                return true;
            }
            
            for (int v = 0; v < graph_.num_vertices(); v++) {
                if (is_safe(v, pos)) {
                    path_[pos] = v;
                    visited_[v] = true;
                    
                    if (hamiltonian_path_recursive(pos + 1)) {
                        return true;
                    }
                    
                    visited_[v] = false;
                    path_[pos] = -1;
                }
            }
            
            return false;
        }
        
    public:
        HamiltonianPathSolver(const Graph& g)
            : graph_(g), path_found_(false) {
            path_.resize(graph_.num_vertices(), -1);
            visited_.resize(graph_.num_vertices(), false);
        }
        
        bool solve() {
            // Try starting from each vertex
            for (int start = 0; start < graph_.num_vertices(); start++) {
                path_[0] = start;
                visited_[start] = true;
                
                if (hamiltonian_path_recursive(1)) {
                    return true;
                }
                
                visited_[start] = false;
                path_[0] = -1;
            }
            
            return false;
        }
        
        std::vector<int> get_path() const {
            return path_;
        }
    };
};

// Example usage
int main() {
    // Create graph with Hamiltonian cycle
    HamiltonianCycleBacktracking::Graph graph(5);
    graph.add_edge(0, 1);
    graph.add_edge(1, 2);
    graph.add_edge(2, 3);
    graph.add_edge(3, 4);
    graph.add_edge(4, 0);
    graph.add_edge(0, 2);
    graph.add_edge(1, 3);
    
    // Find Hamiltonian cycle
    HamiltonianCycleBacktracking::HamiltonianCycleSolver cycle_solver(graph);
    if (cycle_solver.solve()) {
        std::cout << "Hamiltonian cycle found:" << std::endl;
        auto cycle = cycle_solver.get_cycle();
        for (size_t i = 0; i < cycle.size(); i++) {
            std::cout << cycle[i];
            if (i < cycle.size() - 1) std::cout << " -> ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "No Hamiltonian cycle found" << std::endl;
    }
    
    // Find Hamiltonian path
    HamiltonianCycleBacktracking::HamiltonianPathSolver path_solver(graph);
    if (path_solver.solve()) {
        std::cout << "\nHamiltonian path found:" << std::endl;
        auto path = path_solver.get_path();
        for (size_t i = 0; i < path.size(); i++) {
            std::cout << path[i];
            if (i < path.size() - 1) std::cout << " -> ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}

