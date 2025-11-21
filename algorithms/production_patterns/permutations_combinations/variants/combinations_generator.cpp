/*
 * Combinations Generator
 *
 * Source: Combinatorial algorithms, competitive programming, mathematics libraries
 * Repository: Algorithm libraries, competitive coding platforms, mathematical software
 * Files: Combination generation algorithms, subset selection methods
 * Algorithm: Lexicographic combination generation, iterative and recursive approaches
 *
 * What Makes It Ingenious:
 * - Generates C(n,k) combinations in lexicographic order
 * - Memory-efficient generation (O(k) space per combination)
 * - Supports early termination and filtering
 * - Handles large n with small k efficiently
 * - Used in lottery systems, sampling, and optimization
 *
 * When to Use:
 * - Need to select k items from n without regard to order
 * - Lottery number generation
 * - Feature subset selection
 * - Combinatorial optimization
 * - Statistical sampling
 * - Team selection algorithms
 *
 * Real-World Usage:
 * - Lottery and gambling systems
 * - Machine learning feature selection
 * - Combinatorial optimization problems
 * - Statistical sampling methods
 * - Team formation algorithms
 * - Password generation systems
 * - Resource allocation problems
 *
 * Time Complexity: O(C(n,k) * k) - generate all combinations
 * Space Complexity: O(k) - store current combination
 * Generation: Lexicographic order, efficient for large n
 */

#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <memory>
#include <iterator>
#include <numeric>
#include <set>
#include <unordered_set>

// Lexicographic combination generator
class LexicographicCombinations {
private:
    // Recursive combination generation
    template<typename T, typename Callback>
    void generate_combinations_recursive(size_t start, size_t k, const std::vector<T>& elements,
                                       std::vector<T>& current, Callback callback) {
        if (current.size() == k) {
            callback(current);
            return;
        }

        for (size_t i = start; i < elements.size(); ++i) {
            current.push_back(elements[i]);
            generate_combinations_recursive(i + 1, k, elements, current, callback);
            current.pop_back();
        }
    }

    // Iterative combination generation
    template<typename T, typename Callback>
    void generate_combinations_iterative(size_t n, size_t k, const std::vector<T>& elements,
                                       Callback callback) {
        std::vector<size_t> indices(k);
        std::iota(indices.begin(), indices.end(), 0);

        while (true) {
            // Generate current combination
            std::vector<T> combination;
            for (size_t idx : indices) {
                combination.push_back(elements[idx]);
            }
            callback(combination);

            // Find the rightmost index that can be incremented
            size_t i = k;
            while (i > 0) {
                --i;
                if (indices[i] < n - k + i) {
                    // Increment this index
                    indices[i]++;
                    // Reset all indices after this one
                    for (size_t j = i + 1; j < k; ++j) {
                        indices[j] = indices[j - 1] + 1;
                    }
                    break;
                }
            }

            // If no index could be incremented, we're done
            if (i == 0 && indices[0] >= n - k) {
                break;
            }
        }
    }

public:
    // Generate all combinations of size k from elements
    template<typename T, typename Callback>
    void generate(size_t k, const std::vector<T>& elements, Callback callback,
                 bool use_iterative = true) {
        if (k == 0) {
            callback(std::vector<T>{});
            return;
        }

        if (k > elements.size()) return;

        if (use_iterative) {
            generate_combinations_iterative(elements.size(), k, elements, callback);
        } else {
            std::vector<T> current;
            generate_combinations_recursive(0, k, elements, current, callback);
        }
    }

    // Return all combinations as a vector
    template<typename T>
    std::vector<std::vector<T>> generate_all(size_t k, const std::vector<T>& elements,
                                           bool use_iterative = true) {
        std::vector<std::vector<T>> results;
        generate(k, elements, [&](const std::vector<T>& combo) {
            results.push_back(combo);
        }, use_iterative);
        return results;
    }

    // Generate combinations with early termination
    template<typename T, typename Predicate, typename Callback>
    void generate_until(size_t k, const std::vector<T>& elements,
                       Predicate should_stop, Callback callback) {
        if (k == 0) {
            callback(std::vector<T>{});
            return;
        }

        if (k > elements.size()) return;

        std::vector<size_t> indices(k);
        std::iota(indices.begin(), indices.end(), 0);

        while (true) {
            // Generate current combination
            std::vector<T> combination;
            for (size_t idx : indices) {
                combination.push_back(elements[idx]);
            }

            callback(combination);

            // Check if we should stop
            if (should_stop(combination)) break;

            // Find the rightmost index that can be incremented
            size_t i = k;
            while (i > 0) {
                --i;
                if (indices[i] < elements.size() - k + i) {
                    indices[i]++;
                    for (size_t j = i + 1; j < k; ++j) {
                        indices[j] = indices[j - 1] + 1;
                    }
                    break;
                }
            }

            if (i == 0 && indices[0] >= elements.size() - k) {
                break;
            }
        }
    }

    // Count combinations without generating them
    size_t count(size_t n, size_t k) {
        if (k > n) return 0;
        if (k == 0 || k == n) return 1;

        // Use multiplicative formula to avoid overflow
        size_t result = 1;
        for (size_t i = 1; i <= k; ++i) {
            result *= (n - k + i);
            result /= i;
        }
        return result;
    }

    // Get combination at specific index (0-based)
    template<typename T>
    std::vector<T> combination_at_index(size_t index, size_t k, const std::vector<T>& elements) {
        std::vector<T> result;
        size_t n = elements.size();

        for (size_t i = 0; i < k; ++i) {
            size_t remaining_combinations = count(n - 1 - i, k - 1 - i);

            size_t element_index = 0;
            while (remaining_combinations <= index) {
                index -= remaining_combinations;
                element_index++;
                remaining_combinations = count(n - 1 - i - element_index, k - 1 - i);
            }

            result.push_back(elements[element_index + i]);
            // Adjust remaining elements
            n = n - element_index - 1;
        }

        return result;
    }
};

// Advanced combination generators with constraints
class AdvancedCombinations {
public:
    // Generate combinations with sum constraint
    template<typename T, typename Callback>
    void generate_with_sum(size_t k, const std::vector<T>& elements, T target_sum,
                          Callback callback) {
        std::vector<T> current;
        generate_sum_recursive(0, k, elements, current, target_sum, callback);
    }

private:
    template<typename T, typename Callback>
    void generate_sum_recursive(size_t start, size_t k, const std::vector<T>& elements,
                              std::vector<T>& current, T target_sum, Callback callback) {
        if (current.size() == k) {
            T sum = std::accumulate(current.begin(), current.end(), T{0});
            if (sum == target_sum) {
                callback(current);
            }
            return;
        }

        for (size_t i = start; i < elements.size(); ++i) {
            current.push_back(elements[i]);
            generate_sum_recursive(i + 1, k, elements, current, target_sum, callback);
            current.pop_back();
        }
    }

public:
    // Generate combinations with custom constraints
    template<typename T, typename Constraint, typename Callback>
    void generate_constrained(size_t k, const std::vector<T>& elements,
                            Constraint constraint, Callback callback) {
        std::vector<T> current;
        generate_constrained_recursive(0, k, elements, current, constraint, callback);
    }

private:
    template<typename T, typename Constraint, typename Callback>
    void generate_constrained_recursive(size_t start, size_t k, const std::vector<T>& elements,
                                      std::vector<T>& current, Constraint constraint,
                                      Callback callback) {
        if (current.size() == k) {
            if (constraint(current)) {
                callback(current);
            }
            return;
        }

        for (size_t i = start; i < elements.size(); ++i) {
            current.push_back(elements[i]);
            generate_constrained_recursive(i + 1, k, elements, current, constraint, callback);
            current.pop_back();
        }
    }

public:
    // Generate combinations allowing repetitions
    template<typename T, typename Callback>
    void generate_with_repetitions(size_t k, const std::vector<T>& elements, Callback callback) {
        std::vector<T> current;
        generate_repetitions_recursive(0, k, elements, current, callback);
    }

private:
    template<typename T, typename Callback>
    void generate_repetitions_recursive(size_t start, size_t k, const std::vector<T>& elements,
                                      std::vector<T>& current, Callback callback) {
        if (current.size() == k) {
            callback(current);
            return;
        }

        for (size_t i = start; i < elements.size(); ++i) {
            current.push_back(elements[i]);
            generate_repetitions_recursive(i, k, elements, current, callback); // Note: i, not i+1
            current.pop_back();
        }
    }
};

// Real-world applications
class LotteryCombinations {
private:
    LexicographicCombinations combo_gen;

public:
    // Generate lottery combinations (6/49 style)
    std::vector<std::vector<int>> generate_lottery_combinations(int total_numbers = 49,
                                                              int pick_count = 6,
                                                              int num_combinations = 10) {
        std::vector<int> numbers(total_numbers);
        std::iota(numbers.begin(), numbers.end(), 1);

        std::vector<std::vector<int>> results;
        int count = 0;

        combo_gen.generate(pick_count, numbers, [&](const std::vector<int>& combo) {
            if (count < num_combinations) {
                results.push_back(combo);
                count++;
            }
        });

        return results;
    }

    // Generate random lottery combination
    std::vector<int> generate_random_lottery(int total_numbers = 49, int pick_count = 6) {
        std::vector<int> numbers(total_numbers);
        std::iota(numbers.begin(), numbers.end(), 1);

        // Simple random selection (not truly uniform, but good enough for demo)
        std::random_shuffle(numbers.begin(), numbers.end());

        std::vector<int> result(numbers.begin(), numbers.begin() + pick_count);
        std::sort(result.begin(), result.end());

        return result;
    }

    // Calculate lottery odds
    double calculate_odds(int total_numbers = 49, int pick_count = 6) {
        size_t combinations = combo_gen.count(total_numbers, pick_count);
        return 1.0 / combinations;
    }
};

class FeatureSelection {
private:
    LexicographicCombinations combo_gen;

public:
    // Generate feature subsets for machine learning
    template<typename Feature, typename Callback>
    void generate_feature_subsets(size_t subset_size, const std::vector<Feature>& features,
                                Callback callback) {
        combo_gen.generate(subset_size, features, callback);
    }

    // Best subset selection using custom scoring function
    template<typename Feature, typename ScoreFunc>
    std::vector<Feature> select_best_subset(size_t subset_size, const std::vector<Feature>& features,
                                          ScoreFunc score_function, size_t max_evaluations = 1000) {
        std::vector<Feature> best_subset;
        double best_score = std::numeric_limits<double>::lowest();
        size_t evaluations = 0;

        combo_gen.generate(subset_size, features, [&](const std::vector<Feature>& subset) {
            if (evaluations >= max_evaluations) return;

            double score = score_function(subset);
            if (score > best_score) {
                best_score = score;
                best_subset = subset;
            }
            evaluations++;
        });

        return best_subset;
    }
};

// Combinatorial optimization
class CombinatorialOptimization {
private:
    LexicographicCombinations combo_gen;

public:
    // Traveling salesman problem (brute force for small n)
    template<typename DistanceFunc>
    std::vector<size_t> tsp_brute_force(const std::vector<std::vector<double>>& distance_matrix,
                                      DistanceFunc distance_func) {
        size_t n = distance_matrix.size();
        std::vector<size_t> cities(n);
        std::iota(cities.begin(), cities.end(), 0);

        std::vector<size_t> best_path;
        double best_distance = std::numeric_limits<double>::max();

        // Generate all permutations of cities
        do {
            double current_distance = 0;
            for (size_t i = 0; i < n - 1; ++i) {
                current_distance += distance_func(cities[i], cities[i + 1]);
            }
            current_distance += distance_func(cities.back(), cities[0]); // Return to start

            if (current_distance < best_distance) {
                best_distance = current_distance;
                best_path = cities;
            }
        } while (std::next_permutation(cities.begin(), cities.end()));

        return best_path;
    }

    // Assignment problem (brute force)
    template<typename CostFunc>
    std::vector<size_t> assignment_problem(const std::vector<std::vector<double>>& cost_matrix,
                                        CostFunc cost_func) {
        size_t n = cost_matrix.size();
        std::vector<size_t> assignment(n);
        std::iota(assignment.begin(), assignment.end(), 0);

        std::vector<size_t> best_assignment;
        double best_cost = std::numeric_limits<double>::max();

        do {
            double current_cost = 0;
            for (size_t i = 0; i < n; ++i) {
                current_cost += cost_func(i, assignment[i]);
            }

            if (current_cost < best_cost) {
                best_cost = current_cost;
                best_assignment = assignment;
            }
        } while (std::next_permutation(assignment.begin(), assignment.end()));

        return best_assignment;
    }
};

// Performance benchmarking
class CombinationBenchmark {
public:
    template<typename Func>
    static double measure_time(Func&& func, int iterations = 5) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return static_cast<double>(duration.count()) / iterations;
    }

    static void benchmark_combination_generation(size_t n, size_t k) {
        std::cout << "Benchmarking combination generation (n=" << n << ", k=" << k << "):" << std::endl;

        std::vector<int> elements(n);
        std::iota(elements.begin(), elements.end(), 0);

        LexicographicCombinations gen;

        // Recursive approach
        double recursive_time = measure_time([&]() {
            size_t count = 0;
            gen.generate(k, elements, [&](const std::vector<int>&) { count++; }, false);
        });

        // Iterative approach
        double iterative_time = measure_time([&]() {
            size_t count = 0;
            gen.generate(k, elements, [&](const std::vector<int>&) { count++; }, true);
        });

        std::cout << "Recursive: " << recursive_time << " ms" << std::endl;
        std::cout << "Iterative: " << iterative_time << " ms" << std::endl;
        std::cout << "Iterative is " << (recursive_time / iterative_time) << "x faster" << std::endl;
    }
};

// Example usage
int main() {
    std::cout << "Combinations Generator:" << std::endl;

    // Basic combination generation
    LexicographicCombinations combo_gen;
    std::vector<char> elements = {'A', 'B', 'C', 'D', 'E'};

    std::cout << "All combinations of 3 elements from {'A', 'B', 'C', 'D', 'E'}:" << std::endl;
    auto all_combos = combo_gen.generate_all(3, elements);

    for (size_t i = 0; i < all_combos.size(); ++i) {
        std::cout << i + 1 << ": ";
        for (char c : all_combos[i]) {
            std::cout << c << " ";
        }
        std::cout << std::endl;
    }

    // Combination at specific index
    auto combo_at_5 = combo_gen.combination_at_index(5, 3, elements);
    std::cout << "\nCombination at index 5: ";
    for (char c : combo_at_5) std::cout << c << " ";
    std::cout << std::endl;

    // Count combinations
    size_t total_combos = combo_gen.count(5, 3);
    std::cout << "Total combinations C(5,3) = " << total_combos << std::endl;

    // Advanced combinations
    std::cout << "\nAdvanced Combinations:" << std::endl;
    AdvancedCombinations advanced;

    // With repetitions
    std::cout << "Combinations with repetitions (k=2):" << std::endl;
    advanced.generate_with_repetitions(2, std::vector<char>{'A', 'B', 'C'},
        [](const std::vector<char>& combo) {
            for (char c : combo) std::cout << c << " ";
            std::cout << std::endl;
        });

    // With sum constraint
    std::cout << "Combinations with sum = 10 (k=3):" << std::endl;
    advanced.generate_with_sum(3, std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, 10,
        [](const std::vector<int>& combo) {
            for (int n : combo) std::cout << n << " ";
            std::cout << "(sum=" << std::accumulate(combo.begin(), combo.end(), 0) << ")" << std::endl;
        });

    // Lottery combinations
    std::cout << "\nLottery Combinations:" << std::endl;
    LotteryCombinations lottery;

    auto lotto_combos = lottery.generate_lottery_combinations(49, 6, 5);
    std::cout << "Sample lottery combinations (6/49):" << std::endl;
    for (const auto& combo : lotto_combos) {
        for (int num : combo) std::cout << num << " ";
        std::cout << std::endl;
    }

    std::cout << "Odds of winning: 1 in " << (1.0 / lottery.calculate_odds()) << std::endl;

    // Feature selection example
    std::cout << "\nFeature Selection:" << std::endl;
    FeatureSelection feature_sel;
    std::vector<std::string> features = {"color", "size", "shape", "texture", "weight"};

    // Mock scoring function (prefers features with 'e')
    auto best_features = feature_sel.select_best_subset(3, features,
        [](const std::vector<std::string>& subset) -> double {
            double score = 0;
            for (const std::string& f : subset) {
                if (f.find('e') != std::string::npos) score += 1.0;
            }
            return score;
        }, 20);

    std::cout << "Best feature subset (scoring by 'e' count):" << std::endl;
    for (const std::string& f : best_features) {
        std::cout << f << " ";
    }
    std::cout << std::endl;

    // Combinatorial optimization
    std::cout << "\nCombinatorial Optimization:" << std::endl;
    CombinatorialOptimization opt;

    // Simple TSP example
    std::vector<std::vector<double>> distances = {
        {0, 1, 2, 3},
        {1, 0, 4, 5},
        {2, 4, 0, 6},
        {3, 5, 6, 0}
    };

    auto tsp_path = opt.tsp_brute_force(distances,
        [&](size_t i, size_t j) { return distances[i][j]; });

    std::cout << "TSP optimal path: ";
    for (size_t city : tsp_path) std::cout << city << " ";
    std::cout << "0" << std::endl; // Return to start

    // Performance benchmark
    std::cout << "\nPerformance Benchmark:" << std::endl;
    CombinationBenchmark::benchmark_combination_generation(20, 5);

    std::cout << "\nDemonstrates:" << std::endl;
    std::cout << "- Lexicographic combination generation" << std::endl;
    std::cout << "- Recursive and iterative approaches" << std::endl;
    std::cout << "- Combination indexing and ranking" << std::endl;
    std::cout << "- Advanced constraints (sum, custom predicates)" << std::endl;
    std::cout << "- Combinations with repetitions" << std::endl;
    std::cout << "- Real-world applications (lottery, feature selection)" << std::endl;
    std::cout << "- Combinatorial optimization problems" << std::endl;
    std::cout << "- Production-grade combination algorithms" << std::endl;

    return 0;
}

