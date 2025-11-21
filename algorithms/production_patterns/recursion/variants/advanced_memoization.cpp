/*
 * Advanced Memoization Patterns
 * 
 * Source: Functional programming, dynamic programming optimizations
 * Pattern: Caching recursive function results with various strategies
 * 
 * What Makes It Ingenious:
 * - Automatic memoization: Decorator pattern for functions
 * - LRU cache: Bounded cache with eviction
 * - Weak memoization: Memory-efficient with weak references
 * - Custom hash functions: For complex argument types
 * - Memoization decorators: Reusable pattern
 * - Used in dynamic programming, functional programming, interpreters
 * 
 * When to Use:
 * - Functions with overlapping subproblems
 * - Expensive recursive computations
 * - Dynamic programming optimizations
 * - Functional programming patterns
 * - Interpreter implementations
 * - Repeated computations with same inputs
 * 
 * Real-World Usage:
 * - Python functools.lru_cache
 * - JavaScript memoization libraries
 * - Dynamic programming solutions
 * - Compiler optimizations
 * - Interpreter caching
 * 
 * Time Complexity: O(1) lookup after first computation
 * Space Complexity: O(n) for n unique inputs
 */

#include <functional>
#include <unordered_map>
#include <memory>
#include <list>
#include <tuple>
#include <iostream>
#include <chrono>

// Hash function for tuples (for multi-argument functions)
template<typename... Args>
struct TupleHash {
    size_t operator()(const std::tuple<Args...>& t) const {
        return hash_impl(t, std::index_sequence_for<Args...>{});
    }
    
private:
    template<size_t... Is>
    size_t hash_impl(const std::tuple<Args...>& t, std::index_sequence<Is...>) const {
        size_t seed = 0;
        ((seed ^= std::hash<std::decay_t<decltype(std::get<Is>(t))>>{}(std::get<Is>(t)) + 0x9e3779b9 + (seed << 6) + (seed >> 2)), ...);
        return seed;
    }
};

// Simple memoization decorator
template<typename Result, typename... Args>
class MemoizedFunction {
private:
    std::function<Result(Args...)> func_;
    mutable std::unordered_map<std::tuple<Args...>, Result, TupleHash<Args...>> cache_;
    
public:
    MemoizedFunction(std::function<Result(Args...)> func) : func_(func) {}
    
    Result operator()(Args... args) const {
        auto key = std::make_tuple(args...);
        
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return it->second;
        }
        
        Result result = func_(args...);
        cache_[key] = result;
        return result;
    }
    
    void clear_cache() {
        cache_.clear();
    }
    
    size_t cache_size() const {
        return cache_.size();
    }
};

// LRU Cache for memoization (bounded cache)
template<typename Key, typename Value>
class LRUCache {
private:
    size_t capacity_;
    std::list<std::pair<Key, Value>> items_;
    std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> cache_;
    
public:
    LRUCache(size_t capacity) : capacity_(capacity) {}
    
    bool get(const Key& key, Value& value) {
        auto it = cache_.find(key);
        if (it == cache_.end()) {
            return false;
        }
        
        // Move to front (most recently used)
        items_.splice(items_.begin(), items_, it->second);
        value = it->second->second;
        return true;
    }
    
    void put(const Key& key, const Value& value) {
        auto it = cache_.find(key);
        
        if (it != cache_.end()) {
            // Update existing
            it->second->second = value;
            items_.splice(items_.begin(), items_, it->second);
        } else {
            // Add new
            if (items_.size() >= capacity_) {
                // Remove least recently used
                auto last = items_.back();
                cache_.erase(last.first);
                items_.pop_back();
            }
            
            items_.emplace_front(key, value);
            cache_[key] = items_.begin();
        }
    }
    
    void clear() {
        items_.clear();
        cache_.clear();
    }
    
    size_t size() const {
        return items_.size();
    }
};

// LRU memoization decorator
template<typename Result, typename... Args>
class LRUMemoizedFunction {
private:
    std::function<Result(Args...)> func_;
    mutable LRUCache<std::tuple<Args...>, Result> cache_;
    
public:
    LRUMemoizedFunction(std::function<Result(Args...)> func, size_t capacity = 128)
        : func_(func), cache_(capacity) {}
    
    Result operator()(Args... args) const {
        auto key = std::make_tuple(args...);
        Result result;
        
        if (cache_.get(key, result)) {
            return result;
        }
        
        result = func_(args...);
        cache_.put(key, result);
        return result;
    }
    
    void clear_cache() {
        cache_.clear();
    }
    
    size_t cache_size() const {
        return cache_.size();
    }
};

class AdvancedMemoization {
public:
    // Fibonacci with simple memoization
    static int fibonacci_naive(int n) {
        if (n <= 1) return n;
        return fibonacci_naive(n - 1) + fibonacci_naive(n - 2);
    }
    
    static int fibonacci_memoized(int n) {
        static std::unordered_map<int, int> memo;
        
        if (n <= 1) {
            return n;
        }
        
        if (memo.find(n) != memo.end()) {
            return memo[n];
        }
        
        int result = fibonacci_memoized(n - 1) + fibonacci_memoized(n - 2);
        memo[n] = result;
        return result;
    }
    
    // Binomial coefficient with memoization
    static long long binomial_naive(int n, int k) {
        if (k == 0 || k == n) return 1;
        return binomial_naive(n - 1, k - 1) + binomial_naive(n - 1, k);
    }
    
    static long long binomial_memoized(int n, int k) {
        static std::unordered_map<std::pair<int, int>, long long, 
                                  std::hash<std::pair<int, int>>> memo;
        
        if (k == 0 || k == n) return 1;
        if (k > n) return 0;
        
        auto key = std::make_pair(n, k);
        if (memo.find(key) != memo.end()) {
            return memo[key];
        }
        
        long long result = binomial_memoized(n - 1, k - 1) + 
                          binomial_memoized(n - 1, k);
        memo[key] = result;
        return result;
    }
    
    // Edit distance with memoization
    static int edit_distance_naive(const std::string& s1, const std::string& s2, 
                                   int i, int j) {
        if (i == 0) return j;
        if (j == 0) return i;
        
        if (s1[i - 1] == s2[j - 1]) {
            return edit_distance_naive(s1, s2, i - 1, j - 1);
        }
        
        return 1 + std::min({
            edit_distance_naive(s1, s2, i - 1, j),      // Delete
            edit_distance_naive(s1, s2, i, j - 1),      // Insert
            edit_distance_naive(s1, s2, i - 1, j - 1)   // Replace
        });
    }
    
    static int edit_distance_memoized(const std::string& s1, const std::string& s2,
                                     int i, int j) {
        static std::unordered_map<std::tuple<std::string, std::string, int, int>, int,
                                  TupleHash<std::string, std::string, int, int>> memo;
        
        if (i == 0) return j;
        if (j == 0) return i;
        
        auto key = std::make_tuple(s1, s2, i, j);
        if (memo.find(key) != memo.end()) {
            return memo[key];
        }
        
        int result;
        if (s1[i - 1] == s2[j - 1]) {
            result = edit_distance_memoized(s1, s2, i - 1, j - 1);
        } else {
            result = 1 + std::min({
                edit_distance_memoized(s1, s2, i - 1, j),
                edit_distance_memoized(s1, s2, i, j - 1),
                edit_distance_memoized(s1, s2, i - 1, j - 1)
            });
        }
        
        memo[key] = result;
        return result;
    }
    
    // Using memoization decorator
    static int fibonacci_decorated(int n) {
        static MemoizedFunction<int, int> memo([](int n) -> int {
            if (n <= 1) return n;
            return fibonacci_decorated(n - 1) + fibonacci_decorated(n - 2);
        });
        
        return memo(n);
    }
    
    // Using LRU memoization
    static int fibonacci_lru(int n) {
        static LRUMemoizedFunction<int, int> lru_memo([](int n) -> int {
            if (n <= 1) return n;
            return fibonacci_lru(n - 1) + fibonacci_lru(n - 2);
        }, 100);
        
        return lru_memo(n);
    }
};

// Example usage
int main() {
    // Compare performance
    int n = 35;
    
    std::cout << "Computing Fibonacci(" << n << "):" << std::endl;
    
    // Naive (very slow)
    auto start = std::chrono::high_resolution_clock::now();
    // int result_naive = AdvancedMemoization::fibonacci_naive(n);  // Too slow!
    auto end = std::chrono::high_resolution_clock::now();
    // std::cout << "Naive: " << result_naive << " (took too long)" << std::endl;
    
    // Memoized
    start = std::chrono::high_resolution_clock::now();
    int result_memo = AdvancedMemoization::fibonacci_memoized(n);
    end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Memoized: " << result_memo << " (took " << duration.count() << " ms)" << std::endl;
    
    // Decorated
    start = std::chrono::high_resolution_clock::now();
    int result_decorated = AdvancedMemoization::fibonacci_decorated(n);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Decorated: " << result_decorated << " (took " << duration.count() << " ms)" << std::endl;
    
    // LRU
    start = std::chrono::high_resolution_clock::now();
    int result_lru = AdvancedMemoization::fibonacci_lru(n);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "LRU: " << result_lru << " (took " << duration.count() << " ms)" << std::endl;
    
    // Binomial coefficient
    std::cout << "\nBinomial coefficient C(20, 10):" << std::endl;
    std::cout << "Memoized: " << AdvancedMemoization::binomial_memoized(20, 10) << std::endl;
    
    // Edit distance
    std::cout << "\nEdit distance between 'kitten' and 'sitting':" << std::endl;
    std::string s1 = "kitten";
    std::string s2 = "sitting";
    int dist = AdvancedMemoization::edit_distance_memoized(s1, s2, s1.length(), s2.length());
    std::cout << "Distance: " << dist << std::endl;
    
    return 0;
}