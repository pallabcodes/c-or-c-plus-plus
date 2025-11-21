/*
 * Combinatorial Number Systems
 *
 * Source: Combinatorial mathematics, computer science research, algorithm libraries
 * Repository: Mathematical algorithm libraries, research papers, computational mathematics
 * Files: Combinatorial ranking/unranking algorithms, mathematical computation systems
 * Algorithm: Bijection between combinations and integers using binomial coefficients
 *
 * What Makes It Ingenious:
 * - Establishes bijection between combinations and integers
- Enables efficient storage and retrieval of combinations
 * - O(k) time complexity for ranking/unranking operations
 * - Memory-efficient representation of large combination spaces
 * - Used in combinatorial algorithms and data structures
 *
 * When to Use:
 * - Need to rank/unrank combinations efficiently
 * - Store large sets of combinations compactly
 * - Generate combinations in specific orders
 * - Combinatorial indexing and database applications
 * - Mathematical computing and algorithm research
 *
 * Real-World Usage:
 * - Database indexing schemes
 * - Combinatorial algorithm optimization
 * - Mathematical computing libraries
 * - Cryptographic applications
 * - Data compression algorithms
 * - Statistical sampling methods
 *
 * Time Complexity: O(k) for ranking/unranking operations
 * Space Complexity: O(k) for combination storage
 * Mathematical Foundation: Binomial coefficients and combinatorics
 */

#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <memory>
#include <unordered_map>
#include <map>
#include <cmath>
#include <limits>

// Combinatorial number system utilities
class CombinatorialNumberSystem {
private:
    // Binomial coefficient computation with memoization
    std::map<std::pair<size_t, size_t>, size_t> binomial_cache;

    size_t binomial_coefficient(size_t n, size_t k) {
        if (k > n) return 0;
        if (k == 0 || k == n) return 1;

        auto key = std::make_pair(n, k);
        if (binomial_cache.count(key)) {
            return binomial_cache[key];
        }

        // Use multiplicative formula to avoid overflow
        size_t result = 1;
        for (size_t i = 1; i <= k; ++i) {
            result *= (n - k + i);
            result /= i;
        }

        binomial_cache[key] = result;
        return result;
    }

public:
    // Rank a combination (convert combination to its lexicographic index)
    template<typename T>
    size_t rank(const std::vector<T>& combination, const std::vector<T>& universe) {
        // Assume combination and universe are sorted
        size_t rank = 0;
        size_t n = universe.size();
        size_t k = combination.size();

        for (size_t i = 0; i < k; ++i) {
            // Find position of current element in remaining universe
            auto it = std::lower_bound(universe.begin() + i, universe.end(), combination[i]);
            size_t pos = std::distance(universe.begin() + i, it);

            // Add combinations from elements before this position
            if (pos > 0) {
                rank += binomial_coefficient(n - i - 1, k - i - 1);
            }
        }

        return rank;
    }

    // Unrank a combination (convert index to combination)
    template<typename T>
    std::vector<T> unrank(size_t rank, size_t k, const std::vector<T>& universe) {
        std::vector<T> combination;
        size_t n = universe.size();
        size_t remaining_rank = rank;

        for (size_t i = 0; i < k; ++i) {
            size_t remaining_elements = n - i;
            size_t remaining_to_choose = k - i;

            // Find the smallest j such that C(n-i-j-1, k-i-1) <= remaining_rank
            size_t j = 0;
            while (j < remaining_elements - remaining_to_choose + 1) {
                size_t combinations_after = binomial_coefficient(
                    remaining_elements - j - 1, remaining_to_choose - 1);

                if (combinations_after <= remaining_rank) {
                    break;
                }
                j++;
            }

            combination.push_back(universe[i + j]);
            remaining_rank -= binomial_coefficient(remaining_elements - j - 1, remaining_to_choose - 1);
        }

        return combination;
    }

    // Get the total number of combinations
    size_t total_combinations(size_t n, size_t k) {
        return binomial_coefficient(n, k);
    }

    // Check if a rank is valid
    bool is_valid_rank(size_t rank, size_t n, size_t k) {
        return rank < total_combinations(n, k);
    }
};

// Advanced combinatorial number system with optimizations
class AdvancedCombinatorialSystem {
private:
    CombinatorialNumberSystem base_system;

    // Precomputed binomial coefficients for small values
    std::vector<std::vector<size_t>> binomial_table;

    void build_binomial_table(size_t max_n) {
        binomial_table.assign(max_n + 1, std::vector<size_t>(max_n + 1, 0));

        for (size_t i = 0; i <= max_n; ++i) {
            binomial_table[i][0] = 1;
            if (i <= max_n) binomial_table[i][i] = 1;

            for (size_t j = 1; j < i && j <= max_n; ++j) {
                binomial_table[i][j] = binomial_table[i-1][j-1] + binomial_table[i-1][j];
            }
        }
    }

public:
    AdvancedCombinatorialSystem(size_t max_n = 100) {
        build_binomial_table(max_n);
    }

    // Fast ranking with precomputed table (for small n)
    template<typename T>
    size_t fast_rank(const std::vector<T>& combination, const std::vector<T>& universe) {
        size_t rank = 0;
        size_t n = universe.size();
        size_t k = combination.size();

        for (size_t i = 0; i < k; ++i) {
            // Find position
            auto it = std::lower_bound(universe.begin() + i, universe.end(), combination[i]);
            size_t pos = std::distance(universe.begin() + i, it);

            // Add combinations before this position
            if (pos > 0 && n - i - 1 < binomial_table.size() &&
                k - i - 1 < binomial_table[n - i - 1].size()) {
                rank += binomial_table[n - i - 1][k - i - 1];
            }
        }

        return rank;
    }

    // Fast unranking with precomputed table
    template<typename T>
    std::vector<T> fast_unrank(size_t rank, size_t k, const std::vector<T>& universe) {
        std::vector<T> combination;
        size_t n = universe.size();
        size_t remaining_rank = rank;

        for (size_t i = 0; i < k; ++i) {
            size_t remaining_elements = n - i;
            size_t remaining_to_choose = k - i;

            // Find position using binary search on binomial coefficients
            size_t left = 0;
            size_t right = remaining_elements - remaining_to_choose;

            while (left < right) {
                size_t mid = (left + right) / 2;
                size_t combinations_after = (remaining_elements - mid - 1 < binomial_table.size() &&
                                           remaining_to_choose - 1 < binomial_table[remaining_elements - mid - 1].size())
                                          ? binomial_table[remaining_elements - mid - 1][remaining_to_choose - 1]
                                          : 0;

                if (combinations_after <= remaining_rank) {
                    right = mid;
                } else {
                    left = mid + 1;
                }
            }

            combination.push_back(universe[i + left]);
            size_t combinations_after = (remaining_elements - left - 1 < binomial_table.size() &&
                                       remaining_to_choose - 1 < binomial_table[remaining_elements - left - 1].size())
                                      ? binomial_table[remaining_elements - left - 1][remaining_to_choose - 1]
                                      : 0;
            remaining_rank -= combinations_after;
        }

        return combination;
    }

    // Generate combinations in rank order
    template<typename T, typename Callback>
    void generate_by_rank(size_t k, const std::vector<T>& universe, Callback callback) {
        size_t total = base_system.total_combinations(universe.size(), k);

        for (size_t rank = 0; rank < total; ++rank) {
            auto combination = fast_unrank(rank, k, universe);
            callback(combination, rank);
        }
    }

    // Find combinations within a rank range
    template<typename T>
    std::vector<std::vector<T>> combinations_in_range(size_t start_rank, size_t end_rank,
                                                     size_t k, const std::vector<T>& universe) {
        std::vector<std::vector<T>> results;

        for (size_t rank = start_rank; rank <= end_rank; ++rank) {
            if (base_system.is_valid_rank(rank, universe.size(), k)) {
                results.push_back(fast_unrank(rank, k, universe));
            }
        }

        return results;
    }
};

// Combinatorial data structures
class CombinatorialDataStructures {
private:
    CombinatorialNumberSystem cns;

public:
    // Compact combination storage
    template<typename T>
    class CompactCombinationSet {
    private:
        std::vector<T> universe;
        std::vector<size_t> ranks;
        size_t k;

    public:
        CompactCombinationSet(const std::vector<T>& universe, size_t combination_size)
            : universe(universe), k(combination_size) {}

        void add_combination(const std::vector<T>& combination) {
            size_t rank = cns.rank(combination, universe);
            ranks.push_back(rank);
        }

        bool contains(const std::vector<T>& combination) const {
            size_t rank = cns.rank(combination, universe);
            return std::find(ranks.begin(), ranks.end(), rank) != ranks.end();
        }

        std::vector<std::vector<T>> get_all_combinations() const {
            std::vector<std::vector<T>> results;
            for (size_t rank : ranks) {
                results.push_back(cns.unrank(rank, k, universe));
            }
            return results;
        }

        size_t memory_usage() const {
            return ranks.size() * sizeof(size_t); // Just the ranks
        }
    };

    // Combination iterator
    template<typename T>
    class CombinationIterator {
    private:
        std::vector<T> universe;
        size_t k;
        size_t current_rank;
        size_t max_rank;

    public:
        CombinationIterator(const std::vector<T>& universe, size_t combination_size)
            : universe(universe), k(combination_size), current_rank(0) {
            max_rank = cns.total_combinations(universe.size(), k);
        }

        bool has_next() const {
            return current_rank < max_rank;
        }

        std::vector<T> next() {
            if (!has_next()) {
                return {};
            }
            return cns.unrank(current_rank++, k, universe);
        }

        void reset() {
            current_rank = 0;
        }

        size_t total_combinations() const {
            return max_rank;
        }
    };
};

// Applications in computer science
class CombinatorialApplications {
private:
    CombinatorialNumberSystem cns;

public:
    // Generate Gray codes for combinations (adjacent combinations differ by one element)
    template<typename T>
    std::vector<std::vector<T>> generate_gray_codes(size_t k, const std::vector<T>& universe) {
        std::vector<std::vector<T>> gray_codes;

        // Generate all combinations and sort by Gray code order
        size_t total = cns.total_combinations(universe.size(), k);
        std::vector<std::pair<size_t, std::vector<T>>> combinations;

        for (size_t rank = 0; rank < total; ++rank) {
            auto combo = cns.unrank(rank, k, universe);
            // Compute Gray code rank (simplified)
            size_t gray_rank = rank ^ (rank >> 1);
            combinations.emplace_back(gray_rank, combo);
        }

        std::sort(combinations.begin(), combinations.end());
        for (const auto& pair : combinations) {
            gray_codes.push_back(pair.second);
        }

        return gray_codes;
    }

    // Combination hashing for fast lookup
    template<typename T>
    class CombinationHash {
    private:
        std::vector<T> universe;
        size_t k;
        std::unordered_map<size_t, bool> presence_map;

    public:
        CombinationHash(const std::vector<T>& universe, size_t combination_size)
            : universe(universe), k(combination_size) {}

        void insert(const std::vector<T>& combination) {
            size_t rank = cns.rank(combination, universe);
            presence_map[rank] = true;
        }

        bool contains(const std::vector<T>& combination) const {
            size_t rank = cns.rank(combination, universe);
            return presence_map.count(rank) > 0;
        }

        size_t size() const {
            return presence_map.size();
        }
    };

    // Combinatorial sampling
    template<typename T>
    std::vector<std::vector<T>> sample_combinations(size_t k, const std::vector<T>& universe,
                                                   size_t sample_size) {
        std::vector<std::vector<T>> samples;
        size_t total = cns.total_combinations(universe.size(), k);

        // Simple random sampling (without replacement)
        std::vector<size_t> ranks(total);
        std::iota(ranks.begin(), ranks.end(), 0);
        std::random_shuffle(ranks.begin(), ranks.end());

        for (size_t i = 0; i < std::min(sample_size, total); ++i) {
            samples.push_back(cns.unrank(ranks[i], k, universe));
        }

        return samples;
    }
};

// Database indexing with combinations
class CombinatorialIndexing {
private:
    CombinatorialNumberSystem cns;

public:
    // Multi-dimensional index using combinations
    template<typename T>
    class MultiDimIndex {
    private:
        std::vector<T> dimensions;
        std::unordered_map<size_t, std::vector<size_t>> index; // rank -> record_ids

    public:
        MultiDimIndex(const std::vector<T>& dimensions) : dimensions(dimensions) {}

        void insert(const std::vector<T>& combination, size_t record_id) {
            size_t rank = cns.rank(combination, dimensions);
            index[rank].push_back(record_id);
        }

        std::vector<size_t> query(const std::vector<T>& combination) const {
            size_t rank = cns.rank(combination, dimensions);
            auto it = index.find(rank);
            return it != index.end() ? it->second : std::vector<size_t>{};
        }

        size_t index_size() const {
            return index.size();
        }
    };
};

// Performance benchmarking
class CombinatorialBenchmark {
public:
    template<typename Func>
    static double measure_time(Func&& func, int iterations = 5) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return static_cast<double>(duration.count()) / (iterations * 1000.0); // milliseconds
    }

    static void benchmark_ranking_unranking(size_t n, size_t k, size_t num_operations = 1000) {
        std::cout << "Benchmarking ranking/unranking (n=" << n << ", k=" << k << "):" << std::endl;

        std::vector<int> universe(n);
        std::iota(universe.begin(), universe.end(), 0);

        CombinatorialNumberSystem cns;
        AdvancedCombinatorialSystem fast_cns;

        // Generate test combinations
        std::vector<std::vector<int>> test_combinations;
        for (size_t i = 0; i < num_operations && i < cns.total_combinations(n, k); ++i) {
            test_combinations.push_back(cns.unrank(i, k, universe));
        }

        // Benchmark standard ranking
        double rank_time = measure_time([&]() {
            for (const auto& combo : test_combinations) {
                volatile size_t rank = cns.rank(combo, universe);
            }
        });

        // Benchmark fast ranking
        double fast_rank_time = measure_time([&]() {
            for (const auto& combo : test_combinations) {
                volatile size_t rank = fast_cns.fast_rank(combo, universe);
            }
        });

        // Benchmark unranking
        std::vector<size_t> test_ranks;
        for (size_t i = 0; i < num_operations; ++i) {
            test_ranks.push_back(i % cns.total_combinations(n, k));
        }

        double unrank_time = measure_time([&]() {
            for (size_t rank : test_ranks) {
                volatile auto combo = cns.unrank(rank, k, universe);
            }
        });

        double fast_unrank_time = measure_time([&]() {
            for (size_t rank : test_ranks) {
                volatile auto combo = fast_cns.fast_unrank(rank, k, universe);
            }
        });

        std::cout << "Standard ranking: " << rank_time << " ms" << std::endl;
        std::cout << "Fast ranking: " << fast_rank_time << " ms" << std::endl;
        std::cout << "Standard unranking: " << unrank_time << " ms" << std::endl;
        std::cout << "Fast unranking: " << fast_unrank_time << " ms" << std::endl;
    }
};

// Example usage
int main() {
    std::cout << "Combinatorial Number Systems:" << std::endl;

    // Basic ranking and unranking
    CombinatorialNumberSystem cns;
    std::vector<char> universe = {'A', 'B', 'C', 'D', 'E'};
    std::vector<char> combination = {'A', 'C', 'E'};

    std::cout << "Universe: ";
    for (char c : universe) std::cout << c << " ";
    std::cout << std::endl;

    std::cout << "Combination {A, C, E} rank: " << cns.rank(combination, universe) << std::endl;

    // Unrank examples
    for (size_t rank = 0; rank < 10; ++rank) {
        auto combo = cns.unrank(rank, 3, universe);
        std::cout << "Rank " << rank << ": ";
        for (char c : combo) std::cout << c << " ";
        std::cout << std::endl;
    }

    // Advanced system
    std::cout << "\nAdvanced Combinatorial System:" << std::endl;
    AdvancedCombinatorialSystem advanced;

    // Generate combinations by rank
    std::cout << "Combinations in rank order:" << std::endl;
    advanced.generate_by_rank(3, universe, [](const std::vector<char>& combo, size_t rank) {
        std::cout << "Rank " << rank << ": ";
        for (char c : combo) std::cout << c << " ";
        std::cout << std::endl;
    });

    // Range query
    auto range_combos = advanced.combinations_in_range(5, 8, 3, universe);
    std::cout << "Combinations in rank range [5,8]:" << std::endl;
    for (const auto& combo : range_combos) {
        for (char c : combo) std::cout << c << " ";
        std::cout << std::endl;
    }

    // Compact combination storage
    std::cout << "\nCompact Combination Storage:" << std::endl;
    CombinatorialDataStructures::CompactCombinationSet<char> compact_set(universe, 3);

    compact_set.add_combination({'A', 'B', 'C'});
    compact_set.add_combination({'A', 'C', 'E'});
    compact_set.add_combination({'B', 'D', 'E'});

    std::cout << "Compact set contains {A, C, E}: " <<
              (compact_set.contains({'A', 'C', 'E'}) ? "Yes" : "No") << std::endl;
    std::cout << "Memory usage: " << compact_set.memory_usage() << " bytes" << std::endl;

    // Combination iterator
    std::cout << "\nCombination Iterator:" << std::endl;
    CombinatorialDataStructures::CombinationIterator<char> iterator(universe, 3);

    std::cout << "First 5 combinations:" << std::endl;
    for (int i = 0; i < 5 && iterator.has_next(); ++i) {
        auto combo = iterator.next();
        for (char c : combo) std::cout << c << " ";
        std::cout << std::endl;
    }

    // Combinatorial applications
    std::cout << "\nCombinatorial Applications:" << std::endl;
    CombinatorialApplications apps;

    // Combination hash
    CombinatorialApplications::CombinationHash<char> combo_hash(universe, 3);
    combo_hash.insert({'A', 'B', 'C'});
    combo_hash.insert({'A', 'C', 'E'});

    std::cout << "Hash contains {A, C, E}: " <<
              (combo_hash.contains({'A', 'C', 'E'}) ? "Yes" : "No") << std::endl;

    // Sampling
    auto samples = apps.sample_combinations(3, universe, 3);
    std::cout << "Random samples:" << std::endl;
    for (const auto& sample : samples) {
        for (char c : sample) std::cout << c << " ";
        std::cout << std::endl;
    }

    // Database indexing
    std::cout << "\nCombinatorial Indexing:" << std::endl;
    CombinatorialIndexing::MultiDimIndex<char> index(universe);

    index.insert({'A', 'B', 'C'}, 1001);
    index.insert({'A', 'C', 'E'}, 1002);
    index.insert({'B', 'D', 'E'}, 1003);

    auto records = index.query({'A', 'C', 'E'});
    std::cout << "Records for combination {A, C, E}: ";
    for (size_t record : records) std::cout << record << " ";
    std::cout << std::endl;

    // Performance benchmark
    std::cout << "\nPerformance Benchmark:" << std::endl;
    CombinatorialBenchmark::benchmark_ranking_unranking(20, 5, 100);

    std::cout << "\nDemonstrates:" << std::endl;
    std::cout << "- Combinatorial number system ranking/unranking" << std::endl;
    std::cout << "- Fast operations with precomputed binomial coefficients" << std::endl;
    std::cout << "- Compact combination storage using ranks" << std::endl;
    std::cout << "- Iterator-based combination generation" << std::endl;
    std::cout << "- Combinatorial hashing and indexing" << std::endl;
    std::cout << "- Statistical sampling applications" << std::endl;
    std::cout << "- Database indexing with combinations" << std::endl;
    std::cout << "- Production-grade combinatorial algorithms" << std::endl;

    return 0;
}

