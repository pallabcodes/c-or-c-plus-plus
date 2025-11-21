/*
 * Next Permutation (STL-Style)
 *
 * Source: C++ Standard Library (<algorithm>), competitive programming
 * Repository: libstdc++, libc++, competitive coding libraries
 * Files: std::next_permutation implementation, STL algorithms
 * Algorithm: Lexicographic successor generation
 *
 * What Makes It Ingenious:
 * - Finds next permutation in lexicographic order
 * - Transforms sequence to next dictionary-ordered arrangement
 * - Returns false when sequence is already the last permutation
 * - O(n) time complexity with amortized constant factors
 * - In-place modification for memory efficiency
 *
 * When to Use:
 * - Generate permutations in dictionary order
 * - Need only the next permutation, not all permutations
 * - Memory-constrained applications
 * - Standard library replacement
 * - Competitive programming problems requiring permutation enumeration
 *
 * Real-World Usage:
 * - C++ std::next_permutation algorithm
 * - Dictionary word generation
 * - Sorting algorithm implementations
 * - Puzzle solvers with permutation constraints
 * - Competitive programming (LeetCode, CodeForces)
 * - Algorithm research and testing
 *
 * Time Complexity: O(n) - single pass through the array
 * Space Complexity: O(1) - in-place modification
 * Stability: Yes - preserves relative order where possible
 */

#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <string>
#include <numeric>
#include <chrono>

// STL-style next_permutation implementation
class NextPermutation {
private:
    // Helper to reverse a subrange
    template<typename Iterator>
    void reverse_range(Iterator first, Iterator last) {
        while (first < last) {
            --last;
            std::iter_swap(first, last);
            ++first;
        }
    }

    // Find the rightmost ascent (i < i+1)
    template<typename Iterator>
    Iterator find_rightmost_ascent(Iterator first, Iterator last) {
        if (first == last) return last;

        Iterator current = last;
        --current;

        while (current != first) {
            Iterator prev = current;
            --prev;

            if (*prev < *current) {
                return prev;
            }

            current = prev;
        }

        return last; // No ascent found
    }

    // Find the smallest element larger than pivot to the right
    template<typename Iterator>
    Iterator find_smallest_larger(Iterator first, Iterator pivot, Iterator last) {
        Iterator result = pivot;
        ++result; // Start with element after pivot

        for (Iterator it = result; it != last; ++it) {
            if (*it > *pivot && (*it < *result || result == pivot)) {
                result = it;
            }
        }

        return result;
    }

public:
    // Main next_permutation function (STL-style interface)
    template<typename Iterator>
    bool next_permutation(Iterator first, Iterator last) {
        if (first == last) return false;

        // Find the rightmost ascent
        Iterator pivot = find_rightmost_ascent(first, last);

        if (pivot == last) {
            // Already the last permutation
            reverse_range(first, last);
            return false;
        }

        // Find the smallest element larger than pivot
        Iterator change = find_smallest_larger(first, pivot, last);

        // Swap pivot and change
        std::iter_swap(pivot, change);

        // Reverse the suffix after pivot
        reverse_range(pivot + 1, last);

        return true;
    }

    // Convenience function for containers
    template<typename Container>
    bool next_permutation(Container& container) {
        return next_permutation(container.begin(), container.end());
    }

    // Generate all permutations using next_permutation
    template<typename Container>
    std::vector<Container> generate_all_permutations(Container container) {
        std::vector<Container> result;
        std::sort(container.begin(), container.end()); // Start with sorted order

        do {
            result.push_back(container);
        } while (next_permutation(container));

        return result;
    }

    // Count how many permutations are possible
    template<typename Container>
    size_t count_permutations(const Container& container) {
        // For unique elements, it's n!
        // For duplicates, it's n! / (k1! * k2! * ...)
        std::unordered_map<typename Container::value_type, int> frequency;

        for (const auto& elem : container) {
            frequency[elem]++;
        }

        size_t numerator = 1;
        for (size_t i = 2; i <= container.size(); ++i) {
            numerator *= i;
        }

        size_t denominator = 1;
        for (const auto& pair : frequency) {
            for (size_t i = 2; i <= pair.second; ++i) {
                denominator *= i;
            }
        }

        return numerator / denominator;
    }
};

// Advanced permutation utilities
class PermutationUtilities {
public:
    // Check if a sequence is a valid permutation of another
    template<typename Container1, typename Container2>
    bool is_permutation(const Container1& a, const Container2& b) {
        if (a.size() != b.size()) return false;

        Container1 sorted_a = a;
        Container2 sorted_b = b;

        std::sort(sorted_a.begin(), sorted_a.end());
        std::sort(sorted_b.begin(), sorted_b.end());

        return sorted_a == sorted_b;
    }

    // Find the lexicographic rank of a permutation
    template<typename Container>
    size_t permutation_rank(const Container& perm) {
        Container sorted = perm;
        std::sort(sorted.begin(), sorted.end());

        size_t rank = 0;
        Container remaining = perm;

        for (size_t i = 0; i < perm.size(); ++i) {
            // Find position of current element in sorted remaining
            auto it = std::find(sorted.begin(), sorted.end(), remaining[i]);
            size_t pos = std::distance(sorted.begin(), it);

            // Add to rank: pos * (n-i-1)!
            size_t factorial = 1;
            for (size_t j = 1; j <= perm.size() - i - 1; ++j) {
                factorial *= j;
            }
            rank += pos * factorial;

            // Remove current element from sorted
            sorted.erase(it);
        }

        return rank;
    }

    // Generate permutation at given rank
    template<typename T>
    std::vector<T> permutation_at_rank(size_t rank, const std::vector<T>& elements) {
        std::vector<T> sorted = elements;
        std::sort(sorted.begin(), sorted.end());

        std::vector<T> result;
        size_t n = elements.size();

        for (size_t i = 0; i < n; ++i) {
            size_t factorial = 1;
            for (size_t j = 1; j <= n - i - 1; ++j) {
                factorial *= j;
            }

            size_t index = rank / factorial;
            result.push_back(sorted[index]);
            sorted.erase(sorted.begin() + index);
            rank %= factorial;
        }

        return result;
    }

    // Find all cycles in a permutation
    std::vector<std::vector<size_t>> find_cycles(const std::vector<size_t>& permutation) {
        std::vector<std::vector<size_t>> cycles;
        std::vector<bool> visited(permutation.size(), false);

        for (size_t start = 0; start < permutation.size(); ++start) {
            if (!visited[start]) {
                std::vector<size_t> cycle;
                size_t current = start;

                while (!visited[current]) {
                    visited[current] = true;
                    cycle.push_back(current);
                    current = permutation[current];
                }

                if (cycle.size() > 1) {
                    cycles.push_back(cycle);
                }
            }
        }

        return cycles;
    }

    // Calculate permutation parity (even/odd)
    bool is_even_permutation(const std::vector<size_t>& permutation) {
        auto cycles = find_cycles(permutation);
        int even_cycles = 0;

        for (const auto& cycle : cycles) {
            if (cycle.size() % 2 == 0) {
                even_cycles++;
            }
        }

        // Permutation is even if even number of even-length cycles
        return even_cycles % 2 == 0;
    }

    // Apply permutation to a sequence
    template<typename T>
    std::vector<T> apply_permutation(const std::vector<T>& sequence,
                                    const std::vector<size_t>& permutation) {
        if (sequence.size() != permutation.size()) {
            throw std::invalid_argument("Sequence and permutation sizes don't match");
        }

        std::vector<T> result(sequence.size());
        for (size_t i = 0; i < permutation.size(); ++i) {
            result[i] = sequence[permutation[i]];
        }
        return result;
    }

    // Inverse permutation
    std::vector<size_t> inverse_permutation(const std::vector<size_t>& permutation) {
        std::vector<size_t> inverse(permutation.size());
        for (size_t i = 0; i < permutation.size(); ++i) {
            inverse[permutation[i]] = i;
        }
        return inverse;
    }
};

// Performance benchmarking
class PermutationBenchmark {
public:
    template<typename Func>
    static double measure_time(Func&& func, int iterations = 10) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return static_cast<double>(duration.count()) / (iterations * 1000.0); // milliseconds
    }

    static void benchmark_next_permutation(size_t n) {
        std::cout << "Benchmarking next_permutation with n=" << n << ":" << std::endl;

        std::vector<int> data(n);
        for (size_t i = 0; i < n; ++i) data[i] = i;

        NextPermutation perm;
        size_t count = 0;

        double time = measure_time([&]() {
            count = 0;
            std::vector<int> temp = data;
            do {
                count++;
            } while (perm.next_permutation(temp) && count < 100000); // Limit for large n
        });

        std::cout << "Generated " << count << " permutations in " << time << " ms" << std::endl;
        std::cout << "Time per permutation: " << (time / count) * 1000.0 << " μs" << std::endl;
    }

    static void compare_with_std(size_t n) {
        std::cout << "Comparing with std::next_permutation (n=" << n << "):" << std::endl;

        std::vector<int> data1(n), data2(n);
        for (size_t i = 0; i < n; ++i) {
            data1[i] = i;
            data2[i] = i;
        }

        NextPermutation custom;
        size_t custom_count = 0, std_count = 0;

        double custom_time = measure_time([&]() {
            custom_count = 0;
            std::vector<int> temp = data1;
            do {
                custom_count++;
            } while (custom.next_permutation(temp) && custom_count < 10000);
        });

        double std_time = measure_time([&]() {
            std_count = 0;
            std::vector<int> temp = data2;
            do {
                std_count++;
            } while (std::next_permutation(temp.begin(), temp.end()) && std_count < 10000);
        });

        std::cout << "Custom implementation: " << custom_count << " perms, " << custom_time << " ms" << std::endl;
        std::cout << "STD implementation: " << std_count << " perms, " << std_time << " ms" << std::endl;
        std::cout << "Performance ratio: " << (custom_time / std_time) << "x" << std::endl;
    }
};

// Example usage
int main() {
    std::cout << "Next Permutation (STL-Style):" << std::endl;

    // Basic next_permutation usage
    std::vector<int> numbers = {1, 2, 3};
    NextPermutation perm;

    std::cout << "Original: ";
    for (int n : numbers) std::cout << n << " ";
    std::cout << std::endl;

    // Generate next permutations
    int count = 0;
    do {
        std::cout << "Permutation " << ++count << ": ";
        for (int n : numbers) std::cout << n << " ";
        std::cout << std::endl;
    } while (perm.next_permutation(numbers) && count < 6);

    // Test with strings
    std::string word = "abc";
    std::cout << "\nString permutations for '" << word << "':" << std::endl;

    std::vector<std::string> string_perms = perm.generate_all_permutations(word);
    for (size_t i = 0; i < string_perms.size(); ++i) {
        std::cout << i + 1 << ": " << string_perms[i] << std::endl;
    }

    // Permutation utilities
    std::cout << "\nPermutation Utilities:" << std::endl;
    PermutationUtilities utils;

    std::vector<int> perm1 = {1, 2, 0};
    std::vector<int> perm2 = {0, 2, 1};
    std::vector<int> original = {0, 1, 2};

    std::cout << "Is {1,2,0} a permutation of {0,1,2}? " <<
              (utils.is_permutation(perm1, original) ? "Yes" : "No") << std::endl;

    std::cout << "Lexicographic rank of {1,2,0}: " << utils.permutation_rank(perm1) << std::endl;

    std::vector<int> elements = {0, 1, 2};
    auto reconstructed = utils.permutation_at_rank(5, elements);
    std::cout << "Permutation at rank 5: ";
    for (int n : reconstructed) std::cout << n << " ";
    std::cout << std::endl;

    // Cycle decomposition
    std::vector<size_t> test_perm = {1, 2, 0, 4, 3}; // (0 1 2)(3 4)
    auto cycles = utils.find_cycles(test_perm);
    std::cout << "Cycles in permutation {1,2,0,4,3}: ";
    for (const auto& cycle : cycles) {
        std::cout << "(";
        for (size_t i = 0; i < cycle.size(); ++i) {
            std::cout << cycle[i];
            if (i < cycle.size() - 1) std::cout << " ";
        }
        std::cout << ") ";
    }
    std::cout << std::endl;

    std::cout << "Is even permutation? " <<
              (utils.is_even_permutation(test_perm) ? "Yes" : "No") << std::endl;

    // Apply permutation
    std::vector<char> data = {'A', 'B', 'C'};
    std::vector<size_t> perm_indices = {2, 0, 1}; // Apply permutation (2,0,1)
    auto result = utils.apply_permutation(data, perm_indices);
    std::cout << "Applying permutation {2,0,1} to {'A','B','C'}: ";
    for (char c : result) std::cout << c << " ";
    std::cout << std::endl;

    // Inverse permutation
    auto inverse = utils.inverse_permutation(perm_indices);
    std::cout << "Inverse permutation: ";
    for (size_t n : inverse) std::cout << n << " ";
    std::cout << std::endl;

    // Performance benchmarking
    std::cout << "\nPerformance Benchmarking:" << std::endl;
    PermutationBenchmark::compare_with_std(8);

    std::cout << "\nDemonstrates:" << std::endl;
    std::cout << "- STL-style next_permutation implementation" << std::endl;
    std::cout << "- Lexicographic permutation ordering" << std::endl;
    std::cout << "- Permutation ranking and unranking" << std::endl;
    std::cout << "- Cycle decomposition and parity" << std::endl;
    std::cout << "- Permutation application and inversion" << std::endl;
    std::cout << "- Performance comparison with standard library" << std::endl;
    std::cout << "- Production-grade permutation algorithms" << std::endl;

    return 0;
}

