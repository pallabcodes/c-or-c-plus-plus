/*
 * Co-recursion Pattern
 * 
 * Source: Functional programming languages (Haskell, Scala, F#)
 * Pattern: Generating infinite data structures using recursion
 * 
 * What Makes It Ingenious:
 * - Lazy evaluation: Generate values on demand
 * - Infinite structures: Represent infinite sequences
 * - Generators: Produce values incrementally
 * - Memoization: Cache generated values
 * - Used in functional languages, stream processing, generators
 * 
 * When to Use:
 * - Generating infinite sequences
 * - Lazy evaluation patterns
 * - Stream processing
 * - Generator functions
 * - Memoized sequences
 * - Functional programming patterns
 * 
 * Real-World Usage:
 * - Haskell lazy lists
 * - Python generators
 * - Scala streams
 * - Functional reactive programming
 * - Infinite data structures
 * 
 * Time Complexity: O(1) per generated value (amortized)
 * Space Complexity: O(n) for memoization, O(1) for pure generators
 */

#include <vector>
#include <functional>
#include <memory>
#include <iostream>
#include <unordered_map>

template<typename T>
class LazySequence {
private:
    struct Node {
        T value;
        std::shared_ptr<std::function<Node()>> next;
        bool computed;
        
        Node(T v) : value(v), computed(true) {}
        Node(std::function<Node()> gen) : next(std::make_shared<std::function<Node()>>(gen)), computed(false) {}
    };
    
    std::shared_ptr<Node> head_;
    mutable std::unordered_map<int, T> cache_;
    
    T compute_value(int index) const {
        if (cache_.find(index) != cache_.end()) {
            return cache_[index];
        }
        
        // For demonstration, we'll use a simple approach
        // In real implementation, would use lazy evaluation
        return T{};
    }
    
public:
    LazySequence(std::function<T(int)> generator) {
        // Create lazy sequence
        head_ = std::make_shared<Node>([generator]() {
            return Node(generator(0));
        });
    }
    
    T get(int index) const {
        if (cache_.find(index) != cache_.end()) {
            return cache_[index];
        }
        
        // Compute and cache
        T value = compute_value(index);
        cache_[index] = value;
        return value;
    }
};

class CoRecursion {
public:
    // Fibonacci sequence using co-recursion (generator pattern)
    class FibonacciGenerator {
    private:
        int a_, b_;
        int index_;
        
    public:
        FibonacciGenerator() : a_(0), b_(1), index_(0) {}
        
        int next() {
            if (index_ == 0) {
                index_++;
                return 0;
            } else if (index_ == 1) {
                index_++;
                return 1;
            } else {
                int next_val = a_ + b_;
                a_ = b_;
                b_ = next_val;
                index_++;
                return next_val;
            }
        }
        
        void reset() {
            a_ = 0;
            b_ = 1;
            index_ = 0;
        }
    };
    
    // Prime numbers using co-recursion (sieve pattern)
    class PrimeGenerator {
    private:
        std::vector<int> primes_;
        int current_;
        
        bool is_prime(int n) {
            if (n < 2) return false;
            for (int p : primes_) {
                if (p * p > n) break;
                if (n % p == 0) return false;
            }
            return true;
        }
        
    public:
        PrimeGenerator() : current_(2) {}
        
        int next() {
            while (!is_prime(current_)) {
                current_++;
            }
            int prime = current_;
            primes_.push_back(prime);
            current_++;
            return prime;
        }
        
        void reset() {
            primes_.clear();
            current_ = 2;
        }
    };
    
    // Factorial sequence using co-recursion
    class FactorialGenerator {
    private:
        long long current_;
        long long value_;
        int index_;
        
    public:
        FactorialGenerator() : current_(1), value_(1), index_(0) {}
        
        long long next() {
            if (index_ == 0) {
                index_++;
                return 1;
            }
            value_ *= current_;
            current_++;
            index_++;
            return value_;
        }
        
        void reset() {
            current_ = 1;
            value_ = 1;
            index_ = 0;
        }
    };
    
    // Collatz sequence generator
    class CollatzGenerator {
    private:
        long long current_;
        
    public:
        CollatzGenerator(long long start) : current_(start) {}
        
        long long next() {
            long long value = current_;
            if (current_ % 2 == 0) {
                current_ = current_ / 2;
            } else {
                current_ = 3 * current_ + 1;
            }
            return value;
        }
        
        bool is_done() const {
            return current_ == 1;
        }
        
        void reset(long long start) {
            current_ = start;
        }
    };
    
    // Memoized recursive sequence (co-recursion with memoization)
    template<typename T>
    class MemoizedSequence {
    private:
        std::function<T(int)> generator_;
        mutable std::unordered_map<int, T> cache_;
        
    public:
        MemoizedSequence(std::function<T(int)> gen) : generator_(gen) {}
        
        T get(int n) const {
            if (cache_.find(n) != cache_.end()) {
                return cache_[n];
            }
            
            T value = generator_(n);
            cache_[n] = value;
            return value;
        }
        
        void clear_cache() {
            cache_.clear();
        }
    };
    
    // Fibonacci with memoization (co-recursive style)
    static int fibonacci_memoized(int n, std::unordered_map<int, int>& memo) {
        if (n <= 1) {
            return n;
        }
        
        if (memo.find(n) != memo.end()) {
            return memo[n];
        }
        
        int value = fibonacci_memoized(n - 1, memo) + fibonacci_memoized(n - 2, memo);
        memo[n] = value;
        return value;
    }
    
    // Infinite sequence of natural numbers
    class NaturalNumbers {
    private:
        int current_;
        
    public:
        NaturalNumbers(int start = 0) : current_(start) {}
        
        int next() {
            return current_++;
        }
        
        void reset(int start = 0) {
            current_ = start;
        }
    };
    
    // Powers of 2 generator
    class PowersOfTwo {
    private:
        long long current_;
        
    public:
        PowersOfTwo() : current_(1) {}
        
        long long next() {
            long long value = current_;
            current_ *= 2;
            return value;
        }
        
        void reset() {
            current_ = 1;
        }
    };
};

// Example usage
int main() {
    // Fibonacci generator
    std::cout << "Fibonacci sequence (first 10):" << std::endl;
    CoRecursion::FibonacciGenerator fib;
    for (int i = 0; i < 10; i++) {
        std::cout << fib.next() << " ";
    }
    std::cout << std::endl;
    
    // Prime generator
    std::cout << "\nPrime numbers (first 10):" << std::endl;
    CoRecursion::PrimeGenerator prime;
    for (int i = 0; i < 10; i++) {
        std::cout << prime.next() << " ";
    }
    std::cout << std::endl;
    
    // Factorial generator
    std::cout << "\nFactorial sequence (first 10):" << std::endl;
    CoRecursion::FactorialGenerator fact;
    for (int i = 0; i < 10; i++) {
        std::cout << fact.next() << " ";
    }
    std::cout << std::endl;
    
    // Collatz sequence
    std::cout << "\nCollatz sequence starting from 27:" << std::endl;
    CoRecursion::CollatzGenerator collatz(27);
    int count = 0;
    while (!collatz.is_done() && count < 20) {
        std::cout << collatz.next() << " ";
        count++;
    }
    std::cout << std::endl;
    
    // Memoized Fibonacci
    std::cout << "\nMemoized Fibonacci:" << std::endl;
    std::unordered_map<int, int> memo;
    for (int i = 0; i < 20; i++) {
        std::cout << CoRecursion::fibonacci_memoized(i, memo) << " ";
    }
    std::cout << std::endl;
    
    // Natural numbers
    std::cout << "\nNatural numbers (first 10):" << std::endl;
    CoRecursion::NaturalNumbers naturals;
    for (int i = 0; i < 10; i++) {
        std::cout << naturals.next() << " ";
    }
    std::cout << std::endl;
    
    // Powers of 2
    std::cout << "\nPowers of 2 (first 10):" << std::endl;
    CoRecursion::PowersOfTwo powers;
    for (int i = 0; i < 10; i++) {
        std::cout << powers.next() << " ";
    }
    std::cout << std::endl;
    
    return 0;
}

