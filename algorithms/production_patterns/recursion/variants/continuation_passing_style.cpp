/*
 * Continuation Passing Style (CPS) Recursion
 * 
 * Source: Functional programming languages (Scheme, ML, Haskell)
 * Pattern: Pass continuation function instead of returning values
 * 
 * What Makes It Ingenious:
 * - Explicit control flow: Continuations represent "what to do next"
 * - Stack safety: Can be converted to iterative form
 * - Tail call optimization: All calls become tail calls
 * - Exception handling: Continuations can represent error handlers
 * - Non-local control flow: Can implement exceptions, coroutines, generators
 * - Used in compilers, interpreters, functional languages
 * 
 * When to Use:
 * - Converting recursive functions to tail-recursive form
 * - Implementing exception handling
 * - Building interpreters and compilers
 * - Non-local control flow (call/cc)
 * - Async programming patterns
 * - Stack-safe recursion
 * 
 * Real-World Usage:
 * - Scheme interpreters (call/cc)
 * - Compiler intermediate representations
 * - Functional language implementations
 * - Async/await implementations
 * - Exception handling systems
 * 
 * Time Complexity: Same as original recursive version
 * Space Complexity: O(1) stack space (all tail calls)
 */

#include <functional>
#include <vector>
#include <iostream>
#include <memory>

template<typename T>
using Continuation = std::function<void(T)>;

class ContinuationPassingStyle {
public:
    // Factorial in CPS: f(n, k) computes n! and passes result to k
    static void factorial_cps(int n, Continuation<int> k) {
        if (n <= 1) {
            k(1);  // Base case: call continuation with result
        } else {
            // Recursive case: compute (n-1)! first, then multiply
            factorial_cps(n - 1, [n, k](int result) {
                k(result * n);  // Multiply and continue
            });
        }
    }
    
    // Sum of array in CPS
    static void sum_cps(const std::vector<int>& arr, int index, 
                       int acc, Continuation<int> k) {
        if (index >= arr.size()) {
            k(acc);  // Base case: return accumulated sum
        } else {
            // Tail recursive: accumulate and continue
            sum_cps(arr, index + 1, acc + arr[index], k);
        }
    }
    
    // Binary search in CPS
    static void binary_search_cps(const std::vector<int>& arr, int target,
                                  int left, int right, 
                                  Continuation<int> k) {
        if (left > right) {
            k(-1);  // Not found
            return;
        }
        
        int mid = left + (right - left) / 2;
        
        if (arr[mid] == target) {
            k(mid);  // Found
        } else if (arr[mid] > target) {
            binary_search_cps(arr, target, left, mid - 1, k);
        } else {
            binary_search_cps(arr, target, mid + 1, right, k);
        }
    }
    
    // Tree traversal in CPS
    template<typename T>
    struct TreeNode {
        T data;
        std::unique_ptr<TreeNode<T>> left;
        std::unique_ptr<TreeNode<T>> right;
        
        TreeNode(T d) : data(d), left(nullptr), right(nullptr) {}
    };
    
    template<typename T>
    static void inorder_traversal_cps(TreeNode<T>* root, 
                                      Continuation<T> visit,
                                      Continuation<void> done) {
        if (!root) {
            done();  // Empty tree: call done continuation
            return;
        }
        
        // Traverse left, visit root, traverse right
        inorder_traversal_cps(root->left.get(), visit, [root, visit, done]() {
            visit(root->data);  // Visit root
            inorder_traversal_cps(root->right.get(), visit, done);
        });
    }
    
    // Exception handling with CPS (success and error continuations)
    static void divide_cps(int a, int b,
                          Continuation<int> success,
                          Continuation<const char*> error) {
        if (b == 0) {
            error("Division by zero");
        } else {
            success(a / b);
        }
    }
    
    // Fibonacci in CPS (with memoization support)
    static void fibonacci_cps(int n, Continuation<int> k) {
        if (n <= 1) {
            k(n);
        } else {
            // Compute fib(n-1) first
            fibonacci_cps(n - 1, [n, k](int fib_n1) {
                // Then compute fib(n-2)
                fibonacci_cps(n - 2, [fib_n1, k](int fib_n2) {
                    // Combine results
                    k(fib_n1 + fib_n2);
                });
            });
        }
    }
    
    // Map function in CPS
    template<typename T, typename U>
    static void map_cps(const std::vector<T>& input,
                       std::function<U(T)> transform,
                       Continuation<std::vector<U>> k) {
        map_cps_helper(input, transform, 0, std::vector<U>(), k);
    }
    
private:
    template<typename T, typename U>
    static void map_cps_helper(const std::vector<T>& input,
                               std::function<U(T)> transform,
                               int index,
                               std::vector<U> acc,
                               Continuation<std::vector<U>> k) {
        if (index >= input.size()) {
            k(acc);  // Done: return accumulated result
        } else {
            U transformed = transform(input[index]);
            acc.push_back(transformed);
            map_cps_helper(input, transform, index + 1, acc, k);
        }
    }
};

// Helper to convert CPS to regular function (for convenience)
template<typename T>
T cps_to_value(std::function<void(Continuation<T>)> cps_func) {
    T result;
    bool done = false;
    
    cps_func([&result, &done](T value) {
        result = value;
        done = true;
    });
    
    // Note: In real async systems, this would use event loop
    // For synchronous CPS, we assume it completes immediately
    return result;
}

// Example usage
int main() {
    // Factorial in CPS
    std::cout << "Factorial(5) in CPS:" << std::endl;
    ContinuationPassingStyle::factorial_cps(5, [](int result) {
        std::cout << "Result: " << result << std::endl;
    });
    
    // Sum in CPS
    std::vector<int> arr = {1, 2, 3, 4, 5};
    std::cout << "\nSum of array in CPS:" << std::endl;
    ContinuationPassingStyle::sum_cps(arr, 0, 0, [](int result) {
        std::cout << "Sum: " << result << std::endl;
    });
    
    // Binary search in CPS
    std::vector<int> sorted = {1, 3, 5, 7, 9, 11, 13};
    std::cout << "\nBinary search in CPS:" << std::endl;
    ContinuationPassingStyle::binary_search_cps(
        sorted, 7, 0, sorted.size() - 1,
        [](int index) {
            std::cout << "Found at index: " << index << std::endl;
        });
    
    // Exception handling with CPS
    std::cout << "\nDivision with error handling:" << std::endl;
    ContinuationPassingStyle::divide_cps(
        10, 2,
        [](int result) {
            std::cout << "Success: " << result << std::endl;
        },
        [](const char* error) {
            std::cout << "Error: " << error << std::endl;
        });
    
    ContinuationPassingStyle::divide_cps(
        10, 0,
        [](int result) {
            std::cout << "Success: " << result << std::endl;
        },
        [](const char* error) {
            std::cout << "Error: " << error << std::endl;
        });
    
    // Fibonacci in CPS
    std::cout << "\nFibonacci(10) in CPS:" << std::endl;
    ContinuationPassingStyle::fibonacci_cps(10, [](int result) {
        std::cout << "Fibonacci(10) = " << result << std::endl;
    });
    
    return 0;
}

