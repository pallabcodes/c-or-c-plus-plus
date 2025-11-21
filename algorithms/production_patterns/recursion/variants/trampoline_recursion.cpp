/*
 * Trampoline Recursion Pattern
 * 
 * Source: Functional programming languages (Scala, JavaScript, Python)
 * Pattern: Convert recursive calls to iterative using trampoline
 * 
 * What Makes It Ingenious:
 * - Stack safety: Avoids stack overflow for deep recursion
 * - Tail call elimination: Converts tail recursion to iteration
 * - Generic pattern: Works for any tail-recursive function
 * - No compiler support needed: Pure library implementation
 * - Used in functional languages, interpreters, compilers
 * 
 * When to Use:
 * - Deep recursion that may cause stack overflow
 * - Languages without tail call optimization
 * - Converting recursive algorithms to iterative
 * - Functional programming in imperative languages
 * - Stack-constrained environments
 * 
 * Real-World Usage:
 * - Scala standard library (trampoline for tail recursion)
 * - JavaScript functional libraries
 * - Python functional programming
 * - Compiler implementations
 * - Interpreter implementations
 * 
 * Time Complexity: Same as original recursive version
 * Space Complexity: O(1) stack space, O(n) heap for thunks
 */

#include <functional>
#include <variant>
#include <memory>
#include <vector>
#include <iostream>

// Result type: either a final value or a thunk (continuation)
template<typename T>
struct TrampolineResult {
    std::variant<T, std::function<TrampolineResult<T>()>> value;
    
    bool is_done() const {
        return value.index() == 0;  // Contains final value
    }
    
    T get_value() const {
        return std::get<0>(value);
    }
    
    std::function<TrampolineResult<T>()> get_thunk() const {
        return std::get<1>(value);
    }
    
    static TrampolineResult<T> done(T val) {
        return TrampolineResult<T>{val};
    }
    
    static TrampolineResult<T> more(std::function<TrampolineResult<T>()> thunk) {
        return TrampolineResult<T>{thunk};
    }
};

// Trampoline: repeatedly evaluate thunks until we get a final value
template<typename T>
T trampoline(std::function<TrampolineResult<T>()> func) {
    TrampolineResult<T> result = func();
    
    while (!result.is_done()) {
        result = result.get_thunk()();
    }
    
    return result.get_value();
}

class TrampolineRecursion {
public:
    // Factorial using trampoline
    static int factorial_trampoline(int n) {
        return trampoline<int>([n]() {
            return factorial_helper(n, 1);
        });
    }
    
private:
    static TrampolineResult<int> factorial_helper(int n, int acc) {
        if (n <= 1) {
            return TrampolineResult<int>::done(acc);
        } else {
            // Return thunk instead of making recursive call
            return TrampolineResult<int>::more([n, acc]() {
                return factorial_helper(n - 1, acc * n);
            });
        }
    }
    
public:
    // Sum of array using trampoline
    static int sum_trampoline(const std::vector<int>& arr) {
        return trampoline<int>([arr]() {
            return sum_helper(arr, 0, 0);
        });
    }
    
private:
    static TrampolineResult<int> sum_helper(const std::vector<int>& arr, 
                                           int index, int acc) {
        if (index >= arr.size()) {
            return TrampolineResult<int>::done(acc);
        } else {
            return TrampolineResult<int>::more([arr, index, acc]() {
                return sum_helper(arr, index + 1, acc + arr[index]);
            });
        }
    }
    
public:
    // Greatest common divisor using trampoline
    static int gcd_trampoline(int a, int b) {
        return trampoline<int>([a, b]() {
            return gcd_helper(a, b);
        });
    }
    
private:
    static TrampolineResult<int> gcd_helper(int a, int b) {
        if (b == 0) {
            return TrampolineResult<int>::done(a);
        } else {
            return TrampolineResult<int>::more([a, b]() {
                return gcd_helper(b, a % b);
            });
        }
    }
    
public:
    // Binary search using trampoline
    static int binary_search_trampoline(const std::vector<int>& arr, 
                                       int target) {
        return trampoline<int>([arr, target]() {
            return binary_search_helper(arr, target, 0, arr.size() - 1);
        });
    }
    
private:
    static TrampolineResult<int> binary_search_helper(
        const std::vector<int>& arr, int target, int left, int right) {
        
        if (left > right) {
            return TrampolineResult<int>::done(-1);
        }
        
        int mid = left + (right - left) / 2;
        
        if (arr[mid] == target) {
            return TrampolineResult<int>::done(mid);
        } else if (arr[mid] > target) {
            return TrampolineResult<int>::more([arr, target, left, mid]() {
                return binary_search_helper(arr, target, left, mid - 1);
            });
        } else {
            return TrampolineResult<int>::more([arr, target, mid, right]() {
                return binary_search_helper(arr, target, mid + 1, right);
            });
        }
    }
    
public:
    // Fibonacci using trampoline (with accumulator)
    static int fibonacci_trampoline(int n) {
        return trampoline<int>([n]() {
            return fibonacci_helper(n, 0, 1);
        });
    }
    
private:
    static TrampolineResult<int> fibonacci_helper(int n, int a, int b) {
        if (n == 0) {
            return TrampolineResult<int>::done(a);
        } else if (n == 1) {
            return TrampolineResult<int>::done(b);
        } else {
            return TrampolineResult<int>::more([n, a, b]() {
                return fibonacci_helper(n - 1, b, a + b);
            });
        }
    }
    
public:
    // Count nodes in tree using trampoline
    template<typename T>
    struct TreeNode {
        T data;
        std::unique_ptr<TreeNode<T>> left;
        std::unique_ptr<TreeNode<T>> right;
        
        TreeNode(T d) : data(d), left(nullptr), right(nullptr) {}
    };
    
    template<typename T>
    static int count_nodes_trampoline(TreeNode<T>* root) {
        return trampoline<int>([root]() {
            return count_nodes_helper(root);
        });
    }
    
private:
    template<typename T>
    static TrampolineResult<int> count_nodes_helper(TreeNode<T>* root) {
        if (!root) {
            return TrampolineResult<int>::done(0);
        } else {
            // Need to handle multiple recursive calls
            // This is a simplified version - full implementation would
            // need to handle multiple thunks
            return TrampolineResult<int>::more([root]() {
                int left_count = count_nodes_trampoline(root->left.get());
                int right_count = count_nodes_trampoline(root->right.get());
                return TrampolineResult<int>::done(1 + left_count + right_count);
            });
        }
    }
};

// Example usage
int main() {
    // Factorial
    std::cout << "Factorial(10) using trampoline: " 
              << TrampolineRecursion::factorial_trampoline(10) << std::endl;
    
    // Sum
    std::vector<int> arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::cout << "Sum of array using trampoline: " 
              << TrampolineRecursion::sum_trampoline(arr) << std::endl;
    
    // GCD
    std::cout << "GCD(48, 18) using trampoline: " 
              << TrampolineRecursion::gcd_trampoline(48, 18) << std::endl;
    
    // Binary search
    std::vector<int> sorted = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    int index = TrampolineRecursion::binary_search_trampoline(sorted, 11);
    std::cout << "Binary search for 11: index " << index << std::endl;
    
    // Fibonacci
    std::cout << "Fibonacci(20) using trampoline: " 
              << TrampolineRecursion::fibonacci_trampoline(20) << std::endl;
    
    return 0;
}

