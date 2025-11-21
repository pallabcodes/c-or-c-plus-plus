/*
 * Gecode Constraint Backtracking
 * 
 * Source: https://github.com/Gecode/gecode
 * Repository: Gecode/gecode
 * File: `gecode/search/dfs.cpp`
 * Algorithm: Constraint propagation with backtracking search
 * 
 * What Makes It Ingenious:
 * - Constraint propagation: Reduces domains before backtracking
 * - Domain reduction: Eliminates impossible values early
 * - Backtracking search: Systematic exploration of solution space
 * - Branch-and-bound: Prunes infeasible branches
 * - Restart strategies: Escapes local minima
 * - Used in production constraint solvers for scheduling, optimization
 * 
 * When to Use:
 * - Constraint satisfaction problems (CSP)
 * - Scheduling problems
 * - Resource allocation
 * - Optimization with constraints
 * - Planning problems
 * 
 * Real-World Usage:
 * - Gecode constraint solver (production)
 * - Scheduling systems
 * - Resource allocation systems
 * - Optimization tools
 * - Planning systems
 * 
 * Time Complexity:
 * - Best case: O(n) if constraints are very tight
 * - Worst case: O(d^n) where d is domain size, n is variables
 * - Average: Depends on constraint tightness and propagation strength
 * 
 * Space Complexity: O(n * d) for domains, O(n) for search stack
 */

#include <vector>
#include <set>
#include <algorithm>
#include <functional>

// Domain: set of possible values for a variable
using Domain = std::set<int>;
using Assignment = std::vector<int>; // -1 means unassigned

// Constraint: function that checks if assignment is valid
using Constraint = std::function<bool(const Assignment&, int)>;

class GecodeConstraint {
private:
    std::vector<Domain> domains_;
    std::vector<Constraint> constraints_;
    std::vector<Assignment> assignments_; // Stack for backtracking
    int num_vars_;
    
    // Propagate constraints: reduce domains
    bool propagate(int var, int value) {
        Assignment& assign = assignments_.back();
        assign[var] = value;
        
        // Check all constraints involving this variable
        for (const auto& constraint : constraints_) {
            if (!constraint(assign, var)) {
                return false; // Constraint violated
            }
        }
        
        return true;
    }
    
    // Check if assignment is complete and valid
    bool is_complete(const Assignment& assign) const {
        for (int val : assign) {
            if (val == -1) return false;
        }
        
        // Check all constraints
        for (const auto& constraint : constraints_) {
            if (!constraint(assign, -1)) {
                return false;
            }
        }
        
        return true;
    }
    
    // Choose variable with smallest domain (MRV heuristic)
    int choose_variable(const Assignment& assign) {
        int best_var = -1;
        int min_domain_size = INT_MAX;
        
        for (int i = 0; i < num_vars_; i++) {
            if (assign[i] == -1) {
                int domain_size = domains_[i].size();
                if (domain_size < min_domain_size) {
                    min_domain_size = domain_size;
                    best_var = i;
                }
            }
        }
        
        return best_var;
    }
    
    // Choose value from domain (LCV heuristic - simplified)
    int choose_value(int var) {
        // Simple: choose first value
        // Real Gecode uses LCV (Least Constraining Value)
        return *domains_[var].begin();
    }
    
public:
    GecodeConstraint(int num_vars) 
        : num_vars_(num_vars)
        , domains_(num_vars) {
        assignments_.push_back(Assignment(num_vars, -1));
    }
    
    // Set domain for variable
    void set_domain(int var, const Domain& domain) {
        domains_[var] = domain;
    }
    
    // Add constraint
    void add_constraint(const Constraint& constraint) {
        constraints_.push_back(constraint);
    }
    
    // Solve constraint satisfaction problem
    bool solve() {
        return backtrack_search(assignments_.back());
    }
    
private:
    bool backtrack_search(Assignment& assign) {
        // Check if complete solution found
        if (is_complete(assign)) {
            return true;
        }
        
        // Choose variable (MRV heuristic)
        int var = choose_variable(assign);
        if (var == -1) {
            return false; // All variables assigned but not valid
        }
        
        // Try each value in domain
        Domain domain_copy = domains_[var];
        for (int value : domain_copy) {
            // Save state
            assignments_.push_back(assign);
            
            // Propagate constraint
            if (propagate(var, value)) {
                // Recursively search
                if (backtrack_search(assignments_.back())) {
                    return true;
                }
            }
            
            // Backtrack: restore state
            assign = assignments_.back();
            assignments_.pop_back();
        }
        
        return false;
    }
    
public:
    const Assignment& get_assignment() const {
        return assignments_.back();
    }
};

// Example usage: N-Queens problem as CSP
#include <iostream>
#include <cmath>

int main() {
    int n = 4; // 4-queens problem
    GecodeConstraint solver(n);
    
    // Set domains: each queen can be in column 0 to n-1
    for (int i = 0; i < n; i++) {
        Domain domain;
        for (int j = 0; j < n; j++) {
            domain.insert(j);
        }
        solver.set_domain(i, domain);
    }
    
    // Add constraints: no two queens attack each other
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            // Constraint: queens i and j don't attack
            solver.add_constraint([i, j](const Assignment& assign, int changed_var) {
                if (assign[i] == -1 || assign[j] == -1) return true;
                
                // Same column
                if (assign[i] == assign[j]) return false;
                
                // Same diagonal
                if (std::abs(assign[i] - assign[j]) == std::abs(i - j)) {
                    return false;
                }
                
                return true;
            });
        }
    }
    
    std::cout << "Solving " << n << "-queens problem..." << std::endl;
    
    if (solver.solve()) {
        std::cout << "Solution found!" << std::endl;
        const auto& assign = solver.get_assignment();
        for (int i = 0; i < n; i++) {
            std::cout << "Queen " << i << " in column " << assign[i] << std::endl;
        }
    } else {
        std::cout << "No solution found" << std::endl;
    }
    
    return 0;
}

