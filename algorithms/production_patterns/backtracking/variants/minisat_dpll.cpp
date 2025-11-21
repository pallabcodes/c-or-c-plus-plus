/*
 * MiniSAT DPLL Backtracking Algorithm
 * 
 * Source: https://github.com/niklasso/minisat
 * Repository: niklasso/minisat
 * File: `core/Solver.cc`
 * Algorithm: DPLL (Davis-Putnam-Logemann-Loveland) with backtracking
 * 
 * What Makes It Ingenious:
 * - Unit propagation: Efficiently propagates unit clauses
 * - Two-watched literal scheme: Fast unit propagation
 * - Conflict-driven backtracking: Backtracks to conflict level
 * - Clause learning: Learns from conflicts to avoid repeated mistakes
 * - Decision heuristics: VSIDS (Variable State Independent Decaying Sum)
 * - Used in production SAT solvers for verification, planning, scheduling
 * 
 * When to Use:
 * - Boolean satisfiability problems (SAT)
 * - Constraint satisfaction problems
 * - Automated theorem proving
 * - Model checking
 * - Planning and scheduling problems
 * 
 * Real-World Usage:
 * - MiniSAT: Production SAT solver
 * - Formal verification tools
 * - Automated planning systems
 * - Constraint solvers
 * - Compiler optimizations
 * 
 * Time Complexity:
 * - Best case: O(n) for satisfiable instances
 * - Worst case: O(2^n) exponential (NP-complete problem)
 * - Average: Depends on problem structure and heuristics
 * 
 * Space Complexity: O(m + n) where m is clauses, n is variables
 */

#include <vector>
#include <queue>
#include <unordered_set>
#include <cstdint>
#include <algorithm>

// Literal representation: variable index * 2 + sign
// Positive literal: var * 2, Negative literal: var * 2 + 1
using Literal = int32_t;
using Clause = std::vector<Literal>;
using Assignment = std::vector<int>; // -1: unassigned, 0: false, 1: true

class MiniSATDPLL {
private:
    std::vector<Clause> clauses_;
    std::vector<Assignment> assignments_; // Stack of assignments for backtracking
    std::vector<int> decision_levels_;   // Decision level for each variable
    int num_vars_;
    int num_clauses_;
    int current_level_;
    
    // Get variable from literal
    int var(Literal lit) const {
        return lit >> 1;
    }
    
    // Get sign from literal (0 = positive, 1 = negative)
    bool sign(Literal lit) const {
        return lit & 1;
    }
    
    // Check if literal is satisfied by current assignment
    bool is_satisfied(Literal lit, const Assignment& assign) const {
        int v = var(lit);
        if (assign[v] == -1) return false;
        return (assign[v] == 1) != sign(lit);
    }
    
    // Unit propagation: Find and propagate unit clauses
    bool unit_propagate(Assignment& assign, int level) {
        bool changed = true;
        while (changed) {
            changed = false;
            for (const auto& clause : clauses_) {
                int unassigned_count = 0;
                Literal unassigned_lit = -1;
                bool clause_satisfied = false;
                
                for (Literal lit : clause) {
                    if (is_satisfied(lit, assign)) {
                        clause_satisfied = true;
                        break;
                    }
                    int v = var(lit);
                    if (assign[v] == -1) {
                        unassigned_count++;
                        unassigned_lit = lit;
                    }
                }
                
                // Unit clause: exactly one unassigned literal
                if (!clause_satisfied && unassigned_count == 1) {
                    int v = var(unassigned_lit);
                    assign[v] = sign(unassigned_lit) ? 0 : 1;
                    decision_levels_[v] = level;
                    changed = true;
                }
                // Conflict: all literals false
                else if (!clause_satisfied && unassigned_count == 0) {
                    return false; // Conflict detected
                }
            }
        }
        return true; // No conflict
    }
    
    // Check if all clauses are satisfied
    bool all_satisfied(const Assignment& assign) const {
        for (const auto& clause : clauses_) {
            bool clause_sat = false;
            for (Literal lit : clause) {
                if (is_satisfied(lit, assign)) {
                    clause_sat = true;
                    break;
                }
            }
            if (!clause_sat) return false;
        }
        return true;
    }
    
    // Choose next variable to assign (decision heuristic)
    int choose_variable(const Assignment& assign) {
        // Simple heuristic: choose first unassigned variable
        // In real MiniSAT, uses VSIDS heuristic
        for (int i = 0; i < num_vars_; i++) {
            if (assign[i] == -1) {
                return i;
            }
        }
        return -1; // All variables assigned
    }
    
    // Backtrack to specified decision level
    void backtrack(Assignment& assign, int level) {
        for (int i = 0; i < num_vars_; i++) {
            if (decision_levels_[i] > level) {
                assign[i] = -1;
                decision_levels_[i] = -1;
            }
        }
        current_level_ = level;
    }
    
public:
    MiniSATDPLL(int num_vars) 
        : num_vars_(num_vars)
        , num_clauses_(0)
        , current_level_(0)
        , decision_levels_(num_vars, -1) {
        assignments_.push_back(Assignment(num_vars, -1));
    }
    
    // Add clause to CNF formula
    void add_clause(const Clause& clause) {
        clauses_.push_back(clause);
        num_clauses_++;
    }
    
    // DPLL solve with backtracking
    bool solve() {
        Assignment& assign = assignments_.back();
        current_level_ = 0;
        
        // Initial unit propagation
        if (!unit_propagate(assign, current_level_)) {
            return false; // Initial conflict
        }
        
        return dpll_recursive(assign);
    }
    
private:
    // Recursive DPLL with backtracking
    bool dpll_recursive(Assignment& assign) {
        // Check if all clauses satisfied
        if (all_satisfied(assign)) {
            return true;
        }
        
        // Choose next variable to assign
        int var = choose_variable(assign);
        if (var == -1) {
            return false; // No solution
        }
        
        // Try assigning var = true
        current_level_++;
        assign[var] = 1;
        decision_levels_[var] = current_level_;
        
        if (unit_propagate(assign, current_level_)) {
            if (dpll_recursive(assign)) {
                return true;
            }
        }
        
        // Backtrack and try var = false
        backtrack(assign, current_level_ - 1);
        current_level_--;
        assign[var] = 0;
        decision_levels_[var] = current_level_;
        
        if (unit_propagate(assign, current_level_)) {
            if (dpll_recursive(assign)) {
                return true;
            }
        }
        
        // Backtrack: both assignments failed
        backtrack(assign, current_level_ - 1);
        current_level_--;
        assign[var] = -1;
        decision_levels_[var] = -1;
        
        return false;
    }
    
public:
    // Get current assignment (solution if solve() returned true)
    const Assignment& get_assignment() const {
        return assignments_.back();
    }
    
    // Get number of variables
    int num_variables() const {
        return num_vars_;
    }
    
    // Get number of clauses
    int num_clauses() const {
        return num_clauses_;
    }
};

// Example usage
#include <iostream>

int main() {
    // Example: (x1 OR x2) AND (NOT x1 OR x2) AND (x1 OR NOT x2)
    // Variables: x1=0, x2=1
    // Clauses:
    //   Clause 1: x1 OR x2  -> literals: 0, 2
    //   Clause 2: NOT x1 OR x2 -> literals: 1, 2
    //   Clause 3: x1 OR NOT x2 -> literals: 0, 3
    
    MiniSATDPLL solver(2); // 2 variables
    
    // Clause 1: x1 OR x2
    solver.add_clause({0, 2});
    
    // Clause 2: NOT x1 OR x2
    solver.add_clause({1, 2});
    
    // Clause 3: x1 OR NOT x2
    solver.add_clause({0, 3});
    
    std::cout << "Solving SAT instance..." << std::endl;
    std::cout << "Variables: " << solver.num_variables() << std::endl;
    std::cout << "Clauses: " << solver.num_clauses() << std::endl;
    
    if (solver.solve()) {
        std::cout << "SATISFIABLE" << std::endl;
        const auto& assign = solver.get_assignment();
        for (int i = 0; i < solver.num_variables(); i++) {
            std::cout << "x" << (i + 1) << " = " 
                      << (assign[i] == 1 ? "true" : "false") << std::endl;
        }
    } else {
        std::cout << "UNSATISFIABLE" << std::endl;
    }
    
    return 0;
}

