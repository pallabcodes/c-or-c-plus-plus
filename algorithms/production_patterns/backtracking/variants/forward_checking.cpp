/*
 * Forward Checking with Arc Consistency - Advanced Backtracking
 * 
 * Source: Constraint satisfaction research, MAC (Maintaining Arc Consistency)
 * Pattern: Propagate constraints forward to reduce domains
 * 
 * What Makes It Ingenious:
 * - Forward checking: Check constraints on unassigned variables
 * - Arc consistency: Maintain consistency between variable pairs
 * - Domain reduction: Remove inconsistent values before assignment
 * - Early failure detection: Detect dead ends early
 * - Used in CSP solvers, constraint programming
 * 
 * When to Use:
 * - Constraint satisfaction problems
 * - Problems with many constraints
 * - Need early pruning
 * - CSP with tight constraints
 * 
 * Real-World Usage:
 * - CSP solvers (Gecode, Choco)
 * - Constraint programming systems
 * - Scheduling systems
 * - Resource allocation
 * 
 * Time Complexity: O(d^n) worst case, but much better with propagation
 * Space Complexity: O(n * d) for domain storage
 */

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iostream>

class ForwardChecking {
public:
    // Variable with domain
    struct Variable {
        int id;
        std::vector<int> domain;
        int assigned_value;
        bool is_assigned;
        
        Variable(int i, const std::vector<int>& dom)
            : id(i), domain(dom), assigned_value(-1), is_assigned(false) {}
    };
    
    // Constraint between two variables
    class BinaryConstraint {
    public:
        virtual ~BinaryConstraint() = default;
        virtual bool is_satisfied(int val1, int val2) const = 0;
        virtual bool supports(int var1_val, int var2_id, 
                             const std::vector<int>& var2_domain) const = 0;
    };
    
    // Not-equal constraint
    class NotEqualConstraint : public BinaryConstraint {
    public:
        bool is_satisfied(int val1, int val2) const override {
            return val1 != val2;
        }
        
        bool supports(int var1_val, int var2_id,
                     const std::vector<int>& var2_domain) const override {
            for (int val2 : var2_domain) {
                if (is_satisfied(var1_val, val2)) {
                    return true;  // At least one supporting value exists
                }
            }
            return false;
        }
    };
    
    // Forward checking solver
    class ForwardCheckingSolver {
    private:
        std::vector<Variable> variables_;
        std::unordered_map<int, std::vector<std::pair<int, BinaryConstraint*>>> constraints_;
        std::vector<int> assignment_order_;
        
        // Forward check: reduce domains of unassigned variables
        bool forward_check(int assigned_var_id, int assigned_value,
                          std::unordered_map<int, std::vector<int>>& saved_domains) {
            // Check constraints involving assigned variable
            if (constraints_.find(assigned_var_id) == constraints_.end()) {
                return true;
            }
            
            for (const auto& [other_var_id, constraint] : constraints_[assigned_var_id]) {
                Variable& other_var = variables_[other_var_id];
                
                if (other_var.is_assigned) {
                    continue;  // Already assigned
                }
                
                // Save original domain
                if (saved_domains.find(other_var_id) == saved_domains.end()) {
                    saved_domains[other_var_id] = other_var.domain;
                }
                
                // Remove inconsistent values
                std::vector<int> new_domain;
                for (int val : other_var.domain) {
                    if (constraint->is_satisfied(assigned_value, val)) {
                        new_domain.push_back(val);
                    }
                }
                
                other_var.domain = new_domain;
                
                // Check if domain became empty (dead end)
                if (other_var.domain.empty()) {
                    return false;  // Failure detected
                }
            }
            
            return true;
        }
        
        // Restore domains (backtrack)
        void restore_domains(const std::unordered_map<int, std::vector<int>>& saved_domains) {
            for (const auto& [var_id, domain] : saved_domains) {
                variables_[var_id].domain = domain;
            }
        }
        
        // Select next variable (MRV heuristic)
        int select_unassigned_variable() {
            int best_var = -1;
            int min_domain_size = INT_MAX;
            
            for (auto& var : variables_) {
                if (!var.is_assigned && var.domain.size() < min_domain_size) {
                    min_domain_size = var.domain.size();
                    best_var = var.id;
                }
            }
            
            return best_var;
        }
        
        // Recursive backtracking with forward checking
        bool backtrack_search() {
            // Check if all variables assigned
            bool all_assigned = true;
            for (const auto& var : variables_) {
                if (!var.is_assigned) {
                    all_assigned = false;
                    break;
                }
            }
            if (all_assigned) {
                return true;  // Solution found
            }
            
            // Select next variable (MRV)
            int var_id = select_unassigned_variable();
            if (var_id == -1) {
                return false;
            }
            
            Variable& var = variables_[var_id];
            
            // Try each value in domain
            std::vector<int> domain_copy = var.domain;  // Copy for iteration
            for (int value : domain_copy) {
                // Assign value
                var.assigned_value = value;
                var.is_assigned = true;
                
                // Forward check
                std::unordered_map<int, std::vector<int>> saved_domains;
                if (forward_check(var_id, value, saved_domains)) {
                    // Continue search
                    if (backtrack_search()) {
                        return true;  // Solution found
                    }
                }
                
                // Backtrack: unassign and restore domains
                var.is_assigned = false;
                var.assigned_value = -1;
                restore_domains(saved_domains);
            }
            
            return false;
        }
        
    public:
        ForwardCheckingSolver(const std::vector<Variable>& vars)
            : variables_(vars) {}
        
        void add_constraint(int var1_id, int var2_id, BinaryConstraint* constraint) {
            constraints_[var1_id].push_back({var2_id, constraint});
            constraints_[var2_id].push_back({var1_id, constraint});
        }
        
        bool solve() {
            return backtrack_search();
        }
        
        std::vector<std::pair<int, int>> get_solution() const {
            std::vector<std::pair<int, int>> solution;
            for (const auto& var : variables_) {
                if (var.is_assigned) {
                    solution.push_back({var.id, var.assigned_value});
                }
            }
            return solution;
        }
    };
};

// Example usage
int main() {
    // Graph coloring: 3 variables, 3 colors
    std::vector<int> domain = {1, 2, 3};
    std::vector<ForwardChecking::Variable> variables;
    variables.emplace_back(0, domain);
    variables.emplace_back(1, domain);
    variables.emplace_back(2, domain);
    
    ForwardChecking::ForwardCheckingSolver solver(variables);
    
    // Add constraints: all must be different
    ForwardChecking::NotEqualConstraint constraint;
    solver.add_constraint(0, 1, &constraint);
    solver.add_constraint(1, 2, &constraint);
    solver.add_constraint(0, 2, &constraint);
    
    if (solver.solve()) {
        std::cout << "Solution found:" << std::endl;
        for (const auto& [var_id, value] : solver.get_solution()) {
            std::cout << "Variable " << var_id << " = " << value << std::endl;
        }
    } else {
        std::cout << "No solution found" << std::endl;
    }
    
    return 0;
}

