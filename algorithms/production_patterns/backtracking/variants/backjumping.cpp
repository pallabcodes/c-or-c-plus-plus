/*
 * Backjumping - Advanced Backtracking Optimization
 * 
 * Source: Constraint satisfaction research, conflict-directed backjumping
 * Pattern: Skip levels in search tree when backtracking
 * 
 * What Makes It Ingenious:
 * - Conflict-directed backjumping: Jump back to conflict source
 * - Skip irrelevant levels: Don't backtrack level by level
 * - Conflict sets: Track which variables cause conflicts
 * - More efficient than chronological backtracking
 * - Used in CSP solvers, SAT solvers
 * 
 * When to Use:
 * - Constraint satisfaction problems
 * - When conflicts are localized
 * - Need faster backtracking
 * - CSP with many variables
 * 
 * Real-World Usage:
 * - CSP solvers
 * - Constraint programming systems
 * - SAT solvers (as part of CDCL)
 * - Scheduling systems
 * 
 * Time Complexity: O(d^n) worst case, but much better in practice
 * Space Complexity: O(n) for conflict sets
 */

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iostream>

class Backjumping {
public:
    // Variable assignment
    struct Assignment {
        int variable;
        int value;
        std::unordered_set<int> conflict_set;  // Variables that conflict with this
        
        Assignment(int var, int val) : variable(var), value(val) {}
    };
    
    // Constraint: checks if assignment is valid
    class Constraint {
    public:
        virtual ~Constraint() = default;
        virtual bool is_satisfied(const std::vector<Assignment>& assignments) const = 0;
        virtual std::unordered_set<int> get_conflict_variables(
            const std::vector<Assignment>& assignments) const = 0;
    };
    
    // Simple constraint: two variables must be different
    class DifferentConstraint : public Constraint {
    private:
        int var1_, var2_;
        
    public:
        DifferentConstraint(int v1, int v2) : var1_(v1), var2_(v2) {}
        
        bool is_satisfied(const std::vector<Assignment>& assignments) const override {
            int val1 = -1, val2 = -1;
            for (const auto& assn : assignments) {
                if (assn.variable == var1_) val1 = assn.value;
                if (assn.variable == var2_) val2 = assn.value;
            }
            if (val1 == -1 || val2 == -1) return true;  // Not yet assigned
            return val1 != val2;
        }
        
        std::unordered_set<int> get_conflict_variables(
            const std::vector<Assignment>& assignments) const override {
            std::unordered_set<int> conflicts;
            int val1 = -1, val2 = -1;
            int idx1 = -1, idx2 = -1;
            
            for (size_t i = 0; i < assignments.size(); i++) {
                if (assignments[i].variable == var1_) {
                    val1 = assignments[i].value;
                    idx1 = i;
                }
                if (assignments[i].variable == var2_) {
                    val2 = assignments[i].value;
                    idx2 = i;
                }
            }
            
            if (val1 != -1 && val2 != -1 && val1 == val2) {
                if (idx1 != -1) conflicts.insert(assignments[idx1].variable);
                if (idx2 != -1) conflicts.insert(assignments[idx2].variable);
            }
            
            return conflicts;
        }
    };
    
    // Backjumping solver
    class BackjumpingSolver {
    private:
        std::vector<Constraint*> constraints_;
        std::vector<int> variables_;
        std::vector<int> domain_;
        std::vector<Assignment> assignments_;
        
        // Check if current assignment is consistent
        bool is_consistent(const Assignment& new_assignment) {
            assignments_.push_back(new_assignment);
            
            for (auto* constraint : constraints_) {
                if (!constraint->is_satisfied(assignments_)) {
                    assignments_.pop_back();
                    return false;
                }
            }
            
            assignments_.pop_back();
            return true;
        }
        
        // Get conflict set for current assignment
        std::unordered_set<int> get_conflict_set() {
            std::unordered_set<int> conflict_set;
            
            for (auto* constraint : constraints_) {
                if (!constraint->is_satisfied(assignments_)) {
                    auto conflicts = constraint->get_conflict_variables(assignments_);
                    conflict_set.insert(conflicts.begin(), conflicts.end());
                }
            }
            
            return conflict_set;
        }
        
        // Backjumping search
        bool backjump_search(int var_index, int& backjump_level) {
            if (var_index >= variables_.size()) {
                return true;  // All variables assigned
            }
            
            int current_var = variables_[var_index];
            std::unordered_set<int> current_conflict_set;
            
            for (int value : domain_) {
                Assignment new_assignment(current_var, value);
                
                if (is_consistent(new_assignment)) {
                    assignments_.push_back(new_assignment);
                    
                    int next_backjump = -1;
                    if (backjump_search(var_index + 1, next_backjump)) {
                        return true;  // Solution found
                    }
                    
                    // Backjump if needed
                    if (next_backjump != -1 && next_backjump < var_index) {
                        assignments_.pop_back();
                        backjump_level = next_backjump;
                        return false;
                    }
                    
                    // Update conflict set
                    auto conflicts = get_conflict_set();
                    for (int conflict_var : conflicts) {
                        // Find index of conflict variable
                        for (size_t i = 0; i < assignments_.size(); i++) {
                            if (assignments_[i].variable == conflict_var) {
                                current_conflict_set.insert(conflict_var);
                                break;
                            }
                        }
                    }
                    
                    assignments_.pop_back();
                } else {
                    // Get conflict set for failed assignment
                    assignments_.push_back(new_assignment);
                    auto conflicts = get_conflict_set();
                    for (int conflict_var : conflicts) {
                        current_conflict_set.insert(conflict_var);
                    }
                    assignments_.pop_back();
                }
            }
            
            // Determine backjump level
            if (!current_conflict_set.empty()) {
                int max_conflict_index = -1;
                for (int conflict_var : current_conflict_set) {
                    for (size_t i = 0; i < assignments_.size(); i++) {
                        if (assignments_[i].variable == conflict_var) {
                            max_conflict_index = std::max(max_conflict_index, static_cast<int>(i));
                            break;
                        }
                    }
                }
                
                if (max_conflict_index >= 0) {
                    backjump_level = max_conflict_index;
                } else {
                    backjump_level = var_index - 1;
                }
            } else {
                backjump_level = var_index - 1;
            }
            
            return false;
        }
        
    public:
        BackjumpingSolver(const std::vector<int>& vars, const std::vector<int>& dom)
            : variables_(vars), domain_(dom) {}
        
        void add_constraint(Constraint* constraint) {
            constraints_.push_back(constraint);
        }
        
        bool solve() {
            assignments_.clear();
            int backjump_level = -1;
            return backjump_search(0, backjump_level);
        }
        
        std::vector<Assignment> get_solution() const {
            return assignments_;
        }
    };
};

// Example usage
int main() {
    // Graph coloring: 3 variables, 3 colors, all must be different
    std::vector<int> variables = {0, 1, 2};
    std::vector<int> domain = {1, 2, 3};  // Colors
    
    Backjumping::BackjumpingSolver solver(variables, domain);
    
    // Add constraints: all variables must be different
    Backjumping::DifferentConstraint c1(0, 1);
    Backjumping::DifferentConstraint c2(1, 2);
    Backjumping::DifferentConstraint c3(0, 2);
    
    solver.add_constraint(&c1);
    solver.add_constraint(&c2);
    solver.add_constraint(&c3);
    
    if (solver.solve()) {
        std::cout << "Solution found:" << std::endl;
        for (const auto& assn : solver.get_solution()) {
            std::cout << "Variable " << assn.variable 
                      << " = " << assn.value << std::endl;
        }
    } else {
        std::cout << "No solution found" << std::endl;
    }
    
    return 0;
}

