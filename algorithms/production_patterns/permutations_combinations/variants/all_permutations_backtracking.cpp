/*
 * All Permutations Backtracking
 *
 * Source: Algorithm design, competitive programming, constraint satisfaction
 * Repository: Backtracking algorithm collections, programming competition libraries
 * Files: Permutation generation with constraints, backtracking implementations
 * Algorithm: Recursive backtracking with pruning for permutation generation
 *
 * What Makes It Ingenious:
 * - Flexible constraint application during generation
 * - Early pruning of invalid partial solutions
 * - Natural for problems with complex constraints
 * - Easy to add custom validation functions
 * - Memory efficient for constrained problems
 *
 * When to Use:
 * - Need all permutations with custom constraints
 * - Partial solution validation is important
 * - Complex constraint satisfaction problems
 * - Educational implementations
 * - When you need to process permutations with filters
 *
 * Real-World Usage:
 * - Constraint satisfaction problems
 * - Puzzle solving algorithms
 * - N-Queens and similar combinatorial problems
 * - Scheduling with constraints
 * - Resource allocation problems
 * - Cryptographic key generation
 *
 * Time Complexity: O(n! * constraint_time) worst case
 * Space Complexity: O(n) for recursion stack
 * Pruning: Can be much faster with good constraints
 */

#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <memory>
#include <unordered_set>
#include <string>
#include <cmath>

// Basic backtracking permutation generator
class BacktrackingPermutations {
private:
    // Generate all permutations using backtracking
    template<typename T, typename Callback>
    void generate_permutations(std::vector<T>& current, std::vector<bool>& used,
                             const std::vector<T>& elements, Callback callback) {
        if (current.size() == elements.size()) {
            callback(current);
            return;
        }

        for (size_t i = 0; i < elements.size(); ++i) {
            if (!used[i]) {
                // Try this element
                used[i] = true;
                current.push_back(elements[i]);

                // Recurse
                generate_permutations(current, used, elements, callback);

                // Backtrack
                current.pop_back();
                used[i] = false;
            }
        }
    }

public:
    // Generate all permutations
    template<typename T, typename Callback>
    void generate_all(const std::vector<T>& elements, Callback callback) {
        std::vector<T> current;
        std::vector<bool> used(elements.size(), false);
        generate_permutations(current, used, elements, callback);
    }

    // Return all permutations as vector
    template<typename T>
    std::vector<std::vector<T>> generate_all(const std::vector<T>& elements) {
        std::vector<std::vector<T>> results;
        generate_all(elements, [&](const std::vector<T>& perm) {
            results.push_back(perm);
        });
        return results;
    }
};

// Advanced backtracking with constraints
class ConstrainedPermutations {
private:
    template<typename T, typename Constraint, typename Callback>
    void generate_constrained(std::vector<T>& current, std::vector<bool>& used,
                            const std::vector<T>& elements, Constraint constraint,
                            Callback callback, size_t& evaluations) {
        evaluations++;

        // Partial constraint check (can prune early)
        if (!constraint(current, true)) {
            return;
        }

        if (current.size() == elements.size()) {
            // Final constraint check
            if (constraint(current, false)) {
                callback(current);
            }
            return;
        }

        for (size_t i = 0; i < elements.size(); ++i) {
            if (!used[i]) {
                used[i] = true;
                current.push_back(elements[i]);

                generate_constrained(current, used, elements, constraint, callback, evaluations);

                current.pop_back();
                used[i] = false;
            }
        }
    }

public:
    // Generate permutations with constraints
    template<typename T, typename Constraint, typename Callback>
    void generate_with_constraints(const std::vector<T>& elements, Constraint constraint,
                                 Callback callback, size_t& evaluations) {
        std::vector<T> current;
        std::vector<bool> used(elements.size(), false);
        evaluations = 0;
        generate_constrained(current, used, elements, constraint, callback, evaluations);
    }

    // Generate permutations with constraints (convenience wrapper)
    template<typename T, typename Constraint>
    std::vector<std::vector<T>> generate_with_constraints(const std::vector<T>& elements,
                                                        Constraint constraint, size_t& evaluations) {
        std::vector<std::vector<T>> results;
        generate_with_constraints(elements, constraint,
            [&](const std::vector<T>& perm) {
                results.push_back(perm);
            }, evaluations);
        return results;
    }
};

// Specialized constraint functions
class PermutationConstraints {
public:
    // No two adjacent elements can be the same (for strings)
    template<typename T>
    static std::function<bool(const std::vector<T>&, bool)> no_adjacent_duplicates() {
        return [](const std::vector<T>& perm, bool is_partial) -> bool {
            if (perm.size() <= 1) return true;

            for (size_t i = 1; i < perm.size(); ++i) {
                if (perm[i] == perm[i-1]) return false;
            }
            return true;
        };
    }

    // Elements must be in non-decreasing order (for partial permutations)
    template<typename T>
    static std::function<bool(const std::vector<T>&, bool)> non_decreasing() {
        return [](const std::vector<T>& perm, bool is_partial) -> bool {
            for (size_t i = 1; i < perm.size(); ++i) {
                if (perm[i] < perm[i-1]) return false;
            }
            return true;
        };
    }

    // Custom predicate-based constraint
    template<typename T, typename Predicate>
    static std::function<bool(const std::vector<T>&, bool)> custom_predicate(Predicate pred) {
        return [pred](const std::vector<T>& perm, bool is_partial) -> bool {
            return pred(perm, is_partial);
        };
    }

    // Sum constraint
    template<typename T>
    static std::function<bool(const std::vector<T>&, bool)> sum_constraint(T min_sum, T max_sum) {
        return [min_sum, max_sum](const std::vector<T>& perm, bool is_partial) -> bool {
            if (perm.empty()) return true;

            T sum = 0;
            for (const auto& val : perm) {
                if constexpr (std::is_arithmetic_v<T>) {
                    sum += val;
                }
            }

            if (is_partial) {
                // For partial solutions, check upper bound
                return sum <= max_sum;
            } else {
                // For complete solutions, check both bounds
                return sum >= min_sum && sum <= max_sum;
            }
        };
    }

    // Distance constraint (no two elements closer than minimum distance)
    template<typename T>
    static std::function<bool(const std::vector<T>&, bool)> minimum_distance(size_t min_dist) {
        return [min_dist](const std::vector<T>& perm, bool is_partial) -> bool {
            for (size_t i = 0; i < perm.size(); ++i) {
                for (size_t j = i + 1; j < perm.size(); ++j) {
                    if constexpr (std::is_arithmetic_v<T>) {
                        T diff = std::abs(perm[i] - perm[j]);
                        if (diff < static_cast<T>(min_dist)) return false;
                    }
                }
            }
            return true;
        };
    }
};

// Real-world applications using backtracking permutations
class BacktrackingApplications {
public:
    // N-Queens problem (place N queens on N×N board so no queen attacks another)
    class NQueensSolver {
    private:
        int n;
        std::vector<std::vector<int>> solutions;

        bool is_safe(const std::vector<int>& queens, int row, int col) {
            for (int prev_row = 0; prev_row < row; ++prev_row) {
                int prev_col = queens[prev_row];

                // Check same column or diagonals
                if (prev_col == col ||
                    prev_col - prev_row == col - row ||
                    prev_col + prev_row == col + row) {
                    return false;
                }
            }
            return true;
        }

        void solve_n_queens(std::vector<int>& queens, int row) {
            if (row == n) {
                solutions.push_back(queens);
                return;
            }

            for (int col = 0; col < n; ++col) {
                if (is_safe(queens, row, col)) {
                    queens[row] = col;
                    solve_n_queens(queens, row + 1);
                    // No need to reset queens[row] as we'll overwrite it
                }
            }
        }

    public:
        NQueensSolver(int board_size) : n(board_size) {}

        std::vector<std::vector<int>> solve() {
            solutions.clear();
            std::vector<int> queens(n, -1);
            solve_n_queens(queens, 0);
            return solutions;
        }

        void print_solution(const std::vector<int>& queens) {
            std::cout << "N-Queens solution:" << std::endl;
            for (int row = 0; row < n; ++row) {
                for (int col = 0; col < n; ++col) {
                    if (queens[row] == col) {
                        std::cout << "Q ";
                    } else {
                        std::cout << ". ";
                    }
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    };

    // Sudoku solver using permutation constraints
    class SudokuSolver {
    private:
        std::vector<std::vector<int>> board;
        int n; // 9 for standard Sudoku

        bool is_valid(int row, int col, int num) {
            // Check row
            for (int c = 0; c < n; ++c) {
                if (board[row][c] == num) return false;
            }

            // Check column
            for (int r = 0; r < n; ++r) {
                if (board[r][col] == num) return false;
            }

            // Check 3x3 box
            int box_row = (row / 3) * 3;
            int box_col = (col / 3) * 3;
            for (int r = box_row; r < box_row + 3; ++r) {
                for (int c = box_col; c < box_col + 3; ++c) {
                    if (board[r][c] == num) return false;
                }
            }

            return true;
        }

        bool solve_sudoku(int pos = 0) {
            if (pos == n * n) return true; // Solved

            int row = pos / n;
            int col = pos % n;

            if (board[row][col] != 0) {
                return solve_sudoku(pos + 1); // Skip filled cells
            }

            for (int num = 1; num <= 9; ++num) {
                if (is_valid(row, col, num)) {
                    board[row][col] = num;
                    if (solve_sudoku(pos + 1)) {
                        return true;
                    }
                    board[row][col] = 0; // Backtrack
                }
            }

            return false;
        }

    public:
        SudokuSolver(const std::vector<std::vector<int>>& initial_board)
            : board(initial_board), n(9) {}

        bool solve() {
            return solve_sudoku();
        }

        const std::vector<std::vector<int>>& get_solution() const {
            return board;
        }

        void print_board() const {
            std::cout << "Sudoku board:" << std::endl;
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    std::cout << board[i][j] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    };

    // Graph coloring using backtracking
    class GraphColoring {
    private:
        std::vector<std::vector<int>> adjacency_list;
        int num_colors;
        std::vector<int> colors;
        std::vector<std::vector<int>> solutions;

        bool is_safe(int vertex, int color) {
            for (int neighbor : adjacency_list[vertex]) {
                if (colors[neighbor] == color) {
                    return false;
                }
            }
            return true;
        }

        void color_graph(int vertex) {
            if (vertex == adjacency_list.size()) {
                solutions.push_back(colors);
                return;
            }

            for (int color = 1; color <= num_colors; ++color) {
                if (is_safe(vertex, color)) {
                    colors[vertex] = color;
                    color_graph(vertex + 1);

                    // Don't reset color for finding all solutions
                    // colors[vertex] = 0;
                }
            }
        }

    public:
        GraphColoring(const std::vector<std::vector<int>>& adj_list, int colors)
            : adjacency_list(adj_list), num_colors(colors), colors(adj_list.size(), 0) {}

        std::vector<std::vector<int>> find_all_colorings() {
            solutions.clear();
            color_graph(0);
            return solutions;
        }

        bool can_color_with_k_colors(int k) {
            num_colors = k;
            solutions.clear();
            color_graph(0);
            return !solutions.empty();
        }
    };
};

// Performance analysis and optimization
class BacktrackingOptimizer {
public:
    template<typename T, typename Constraint>
    struct OptimizationResult {
        std::vector<std::vector<T>> solutions;
        size_t total_evaluations;
        size_t pruned_branches;
        double branching_factor;
        size_t max_depth_reached;
    };

    template<typename T, typename Constraint, typename Callback>
    static OptimizationResult<T> analyze_performance(const std::vector<T>& elements,
                                                   Constraint constraint, Callback callback) {
        OptimizationResult<T> result;
        result.total_evaluations = 0;
        result.pruned_branches = 0;
        result.max_depth_reached = 0;

        ConstrainedPermutations generator;

        generator.generate_with_constraints(elements, constraint,
            [&](const std::vector<T>& perm) {
                result.solutions.push_back(perm);
                callback(perm);
            }, result.total_evaluations);

        // Calculate branching factor (simplified)
        if (!elements.empty()) {
            result.branching_factor = std::pow(result.total_evaluations, 1.0 / elements.size());
        }

        result.max_depth_reached = elements.size(); // In this case

        return result;
    }

    // Find most constraining order
    template<typename T>
    static std::vector<T> find_best_order(const std::vector<T>& elements,
                                        std::function<bool(const std::vector<T>&, bool)> constraint) {
        // Try different orderings and find the one with least evaluations
        std::vector<T> best_order = elements;
        size_t min_evaluations = SIZE_MAX;

        std::vector<T> current_order = elements;
        do {
            size_t evaluations = 0;
            ConstrainedPermutations generator;
            generator.generate_with_constraints(current_order, constraint,
                [](const std::vector<T>&){}, evaluations);

            if (evaluations < min_evaluations) {
                min_evaluations = evaluations;
                best_order = current_order;
            }
        } while (std::next_permutation(current_order.begin(), current_order.end()));

        return best_order;
    }
};

// Example usage
int main() {
    std::cout << "All Permutations with Backtracking:" << std::endl;

    // Basic backtracking permutations
    BacktrackingPermutations basic_gen;
    std::vector<char> elements = {'A', 'B', 'C'};

    std::cout << "All permutations of {'A', 'B', 'C'}:" << std::endl;
    basic_gen.generate_all(elements, [](const std::vector<char>& perm) {
        for (char c : perm) std::cout << c << " ";
        std::cout << std::endl;
    });

    // Constrained permutations
    std::cout << "\nConstrained Permutations:" << std::endl;
    ConstrainedPermutations constrained_gen;

    // No adjacent duplicates
    std::cout << "Permutations with no adjacent duplicates:" << std::endl;
    size_t evaluations1 = 0;
    constrained_gen.generate_with_constraints(std::vector<char>{'A', 'A', 'B', 'B'},
        PermutationConstraints::no_adjacent_duplicates<char>(),
        [](const std::vector<char>& perm) {
            for (char c : perm) std::cout << c << " ";
            std::cout << std::endl;
        }, evaluations1);
    std::cout << "Evaluations: " << evaluations1 << std::endl;

    // Sum constraint
    std::cout << "\nPermutations with sum between 10 and 15:" << std::endl;
    size_t evaluations2 = 0;
    constrained_gen.generate_with_constraints(std::vector<int>{1, 2, 3, 4, 5},
        PermutationConstraints::sum_constraint<int>(10, 15),
        [](const std::vector<int>& perm) {
            int sum = 0;
            for (int n : perm) sum += n;
            for (int n : perm) std::cout << n << " ";
            std::cout << "(sum=" << sum << ")" << std::endl;
        }, evaluations2);
    std::cout << "Evaluations: " << evaluations2 << std::endl;

    // N-Queens problem
    std::cout << "\nN-Queens Problem (N=4):" << std::endl;
    BacktrackingApplications::NQueensSolver queens(4);
    auto queen_solutions = queens.solve();

    std::cout << "Found " << queen_solutions.size() << " solutions" << std::endl;
    if (!queen_solutions.empty()) {
        queens.print_solution(queen_solutions[0]);
    }

    // Sudoku solver
    std::cout << "Sudoku Solver:" << std::endl;
    std::vector<std::vector<int>> sudoku_board = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };

    BacktrackingApplications::SudokuSolver sudoku(sudoku_board);
    std::cout << "Original Sudoku:" << std::endl;
    sudoku.print_board();

    if (sudoku.solve()) {
        std::cout << "Solved Sudoku:" << std::endl;
        sudoku.print_board();
    } else {
        std::cout << "No solution found" << std::endl;
    }

    // Graph coloring
    std::cout << "Graph Coloring (3 colors):" << std::endl;
    // Simple triangle graph
    std::vector<std::vector<int>> triangle_graph = {{1, 2}, {0, 2}, {0, 1}};
    BacktrackingApplications::GraphColoring coloring(triangle_graph, 3);
    auto colorings = coloring.find_all_colorings();
    std::cout << "Found " << colorings.size() << " valid 3-colorings" << std::endl;

    // Performance analysis
    std::cout << "\nPerformance Analysis:" << std::endl;
    auto analysis = BacktrackingOptimizer::analyze_performance(
        std::vector<int>{1, 2, 3, 4, 5},
        PermutationConstraints::sum_constraint<int>(10, 15),
        [](const std::vector<int>&){} // No-op callback
    );

    std::cout << "Analysis Results:" << std::endl;
    std::cout << "Solutions found: " << analysis.solutions.size() << std::endl;
    std::cout << "Total evaluations: " << analysis.total_evaluations << std::endl;
    std::cout << "Average branching factor: " << analysis.branching_factor << std::endl;
    std::cout << "Max depth reached: " << analysis.max_depth_reached << std::endl;

    // Best ordering analysis
    std::cout << "\nOptimization - Finding best variable ordering:" << std::endl;
    auto best_order = BacktrackingOptimizer::find_best_order(
        std::vector<int>{1, 2, 3, 4},
        PermutationConstraints::sum_constraint<int>(6, 8)
    );

    std::cout << "Best ordering: ";
    for (int n : best_order) std::cout << n << " ";
    std::cout << std::endl;

    std::cout << "\nDemonstrates:" << std::endl;
    std::cout << "- Backtracking permutation generation" << std::endl;
    std::cout << "- Constraint satisfaction during generation" << std::endl;
    std::cout << "- Early pruning with partial solution validation" << std::endl;
    std::cout << "- Real-world applications (N-Queens, Sudoku, Graph Coloring)" << std::endl;
    std::cout << "- Performance analysis and optimization" << std::endl;
    std::cout << "- Variable ordering for improved efficiency" << std::endl;
    std::cout << "- Production-grade constraint programming" << std::endl;

    return 0;
}

