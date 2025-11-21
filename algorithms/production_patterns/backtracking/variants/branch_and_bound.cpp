/*
 * Branch and Bound with Backtracking - Optimization Backtracking
 * 
 * Source: Optimization algorithms, combinatorial optimization
 * Pattern: Prune branches using bounds to find optimal solution
 * 
 * What Makes It Ingenious:
 * - Bounding function: Estimate best possible value in subtree
 * - Pruning: Cut branches that can't improve best solution
 * - Optimal solution: Guarantees finding optimal (not just feasible)
 * - Backtracking: Systematic exploration with bounds
 * - Used in optimization problems, TSP, knapsack
 * 
 * When to Use:
 * - Optimization problems
 * - Need optimal solution (not just feasible)
 * - Can compute bounds efficiently
 * - Traveling salesman problem
 * - Knapsack problems
 * 
 * Real-World Usage:
 * - Traveling salesman solvers
 * - Knapsack solvers
 * - Scheduling optimization
 * - Resource allocation optimization
 * - Combinatorial optimization
 * 
 * Time Complexity: O(2^n) worst case, but pruned significantly
 * Space Complexity: O(n) for recursion stack
 */

#include <vector>
#include <algorithm>
#include <climits>
#include <iostream>
#include <cmath>

class BranchAndBound {
public:
    // Traveling Salesman Problem solver
    class TSPSolver {
    private:
        std::vector<std::vector<int>> graph_;
        int n_;
        int best_cost_;
        std::vector<int> best_path_;
        
        // Calculate lower bound for partial path
        int calculate_bound(const std::vector<int>& path) {
            if (path.empty()) return 0;
            
            int bound = 0;
            std::vector<bool> visited(n_, false);
            
            // Add cost of edges in path
            for (size_t i = 0; i < path.size() - 1; i++) {
                bound += graph_[path[i]][path[i + 1]];
                visited[path[i]] = true;
            }
            visited[path.back()] = true;
            
            // Add minimum outgoing edge for each unvisited node
            for (int i = 0; i < n_; i++) {
                if (!visited[i]) {
                    int min_edge = INT_MAX;
                    for (int j = 0; j < n_; j++) {
                        if (i != j && graph_[i][j] < min_edge) {
                            min_edge = graph_[i][j];
                        }
                    }
                    if (min_edge != INT_MAX) {
                        bound += min_edge;
                    }
                }
            }
            
            return bound;
        }
        
        // Branch and bound search
        void branch_and_bound_search(std::vector<int>& path, int current_cost) {
            // Prune if bound exceeds best cost
            int bound = calculate_bound(path);
            if (bound >= best_cost_) {
                return;  // Prune this branch
            }
            
            // Check if complete path
            if (path.size() == n_) {
                // Add return to start
                int return_cost = graph_[path.back()][path[0]];
                int total_cost = current_cost + return_cost;
                
                if (total_cost < best_cost_) {
                    best_cost_ = total_cost;
                    best_path_ = path;
                }
                return;
            }
            
            // Try each unvisited city
            std::vector<bool> visited(n_, false);
            for (int city : path) {
                visited[city] = true;
            }
            
            for (int next_city = 0; next_city < n_; next_city++) {
                if (!visited[next_city]) {
                    int edge_cost = graph_[path.back()][next_city];
                    int new_cost = current_cost + edge_cost;
                    
                    path.push_back(next_city);
                    branch_and_bound_search(path, new_cost);
                    path.pop_back();  // Backtrack
                }
            }
        }
        
    public:
        TSPSolver(const std::vector<std::vector<int>>& graph)
            : graph_(graph), n_(graph.size()), best_cost_(INT_MAX) {}
        
        int solve() {
            if (n_ == 0) return 0;
            
            std::vector<int> path;
            path.push_back(0);  // Start from city 0
            
            branch_and_bound_search(path, 0);
            
            return best_cost_;
        }
        
        std::vector<int> get_path() const {
            return best_path_;
        }
    };
    
    // 0/1 Knapsack solver
    class KnapsackSolver {
    private:
        struct Item {
            int weight;
            int value;
            double ratio;  // value/weight for greedy bound
            
            Item(int w, int v) : weight(w), value(v) {
                ratio = (w > 0) ? static_cast<double>(v) / w : 0.0;
            }
        };
        
        std::vector<Item> items_;
        int capacity_;
        int best_value_;
        std::vector<bool> best_selection_;
        
        // Greedy bound: assume we can take fractional items
        int greedy_bound(int index, int remaining_weight, int current_value) {
            if (remaining_weight <= 0) return current_value;
            
            int bound = current_value;
            int weight_left = remaining_weight;
            
            // Take items greedily by value/weight ratio
            for (int i = index; i < items_.size() && weight_left > 0; i++) {
                if (items_[i].weight <= weight_left) {
                    bound += items_[i].value;
                    weight_left -= items_[i].weight;
                } else {
                    // Take fraction
                    bound += static_cast<int>(items_[i].ratio * weight_left);
                    weight_left = 0;
                }
            }
            
            return bound;
        }
        
        // Branch and bound search
        void branch_and_bound_search(int index, int current_weight, 
                                    int current_value,
                                    std::vector<bool>& selection) {
            // Prune if bound is worse than best
            int bound = greedy_bound(index, capacity_ - current_weight, current_value);
            if (bound <= best_value_) {
                return;  // Prune
            }
            
            // Check if all items processed
            if (index >= items_.size()) {
                if (current_value > best_value_) {
                    best_value_ = current_value;
                    best_selection_ = selection;
                }
                return;
            }
            
            // Try including item
            if (current_weight + items_[index].weight <= capacity_) {
                selection[index] = true;
                branch_and_bound_search(index + 1,
                                       current_weight + items_[index].weight,
                                       current_value + items_[index].value,
                                       selection);
                selection[index] = false;  // Backtrack
            }
            
            // Try excluding item
            branch_and_bound_search(index + 1, current_weight, current_value, selection);
        }
        
    public:
        KnapsackSolver(const std::vector<std::pair<int, int>>& items, int capacity)
            : capacity_(capacity), best_value_(0) {
            for (const auto& [weight, value] : items) {
                items_.emplace_back(weight, value);
            }
            
            // Sort by value/weight ratio (greedy order)
            std::sort(items_.begin(), items_.end(),
                     [](const Item& a, const Item& b) {
                         return a.ratio > b.ratio;
                     });
        }
        
        int solve() {
            std::vector<bool> selection(items_.size(), false);
            branch_and_bound_search(0, 0, 0, selection);
            return best_value_;
        }
        
        std::vector<bool> get_selection() const {
            return best_selection_;
        }
    };
};

// Example usage
int main() {
    // TSP example
    std::vector<std::vector<int>> tsp_graph = {
        {0, 10, 15, 20},
        {10, 0, 35, 25},
        {15, 35, 0, 30},
        {20, 25, 30, 0}
    };
    
    BranchAndBound::TSPSolver tsp(tsp_graph);
    int cost = tsp.solve();
    std::cout << "TSP optimal cost: " << cost << std::endl;
    
    // Knapsack example
    std::vector<std::pair<int, int>> items = {
        {10, 60},  // weight, value
        {20, 100},
        {30, 120}
    };
    int capacity = 50;
    
    BranchAndBound::KnapsackSolver knapsack(items, capacity);
    int max_value = knapsack.solve();
    std::cout << "Knapsack maximum value: " << max_value << std::endl;
    
    return 0;
}

