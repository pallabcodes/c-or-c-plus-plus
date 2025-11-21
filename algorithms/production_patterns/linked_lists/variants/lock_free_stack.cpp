/*
 * Lock-Free Stack (Singly Linked)
 * 
 * Source: data_structures/linear_ordered/02-linked_list/lock_free/lock_free_stack.cpp
 * Local Path: `/Users/picon/Learning/c-or-c-plus-plus/data_structures/linear_ordered/02-linked_list/lock_free/lock_free_stack.cpp`
 * 
 * What Makes It Ingenious:
 * - Lock-free implementation using compare-and-swap (CAS)
 * - Thread-safe without mutexes or locks
 * - Wait-free for push operations (no blocking)
 * - Memory barriers ensure visibility across threads
 * - Used in high-performance concurrent systems
 * - ABA problem handled by memory management
 * 
 * When to Use:
 * - High-concurrency scenarios
 * - Need lock-free data structures
 * - Multi-threaded push/pop operations
 * - Performance-critical concurrent systems
 * - Real-time systems requiring predictable latency
 * 
 * Real-World Usage:
 * - High-performance concurrent systems
 * - Real-time systems
 * - Lock-free programming patterns
 * - Concurrent data structures
 * - Multi-threaded applications
 * 
 * Time Complexity:
 * - Push: O(1) (wait-free)
 * - Pop: O(1) average (lock-free, may retry)
 * - Empty check: O(1)
 * 
 * Space Complexity: O(n) where n is number of elements
 */

#include <atomic>
#include <cstddef>

/*
 * Lock-free stack using compare-and-swap
 * 
 * Key technique: Compare-and-swap (CAS) atomic operation
 * - Atomically compare head with expected value
 * - If equal, update to new value
 * - If not equal, retry (another thread modified it)
 */
template<typename T>
class LockFreeStack {
private:
    struct Node {
        T data;
        Node* next;
        
        Node(const T& d) : data(d), next(nullptr) {}
    };
    
    std::atomic<Node*> head_;
    
public:
    LockFreeStack() : head_(nullptr) {}
    
    // Push element onto stack (wait-free)
    void Push(const T& data) {
        Node* new_node = new Node(data);
        Node* old_head = head_.load(std::memory_order_relaxed);
        
        do {
            new_node->next = old_head;
            // Compare-and-swap: if head == old_head, set head = new_node
            // Otherwise, old_head is updated to current head value
        } while (!head_.compare_exchange_weak(
            old_head, new_node,
            std::memory_order_release,
            std::memory_order_relaxed
        ));
    }
    
    // Pop element from stack (lock-free, may retry)
    bool Pop(T& result) {
        Node* old_head = head_.load(std::memory_order_acquire);
        
        do {
            if (old_head == nullptr) {
                return false;  // Stack is empty
            }
            // Compare-and-swap: if head == old_head, set head = old_head->next
            // Otherwise, old_head is updated to current head value
        } while (!head_.compare_exchange_weak(
            old_head, old_head->next,
            std::memory_order_release,
            std::memory_order_acquire
        ));
        
        result = old_head->data;
        delete old_head;
        return true;
    }
    
    // Check if stack is empty
    bool IsEmpty() const {
        return head_.load(std::memory_order_acquire) == nullptr;
    }
    
    // Peek at top element (without removing)
    bool Peek(T& result) const {
        Node* top = head_.load(std::memory_order_acquire);
        if (top == nullptr) {
            return false;
        }
        result = top->data;
        return true;
    }
};

// Example usage
#include <iostream>
#include <thread>
#include <vector>

int main() {
    LockFreeStack<int> stack;
    
    // Single-threaded test
    stack.Push(1);
    stack.Push(2);
    stack.Push(3);
    
    int val;
    std::cout << "Popping elements:" << std::endl;
    while (stack.Pop(val)) {
        std::cout << "Popped: " << val << std::endl;
    }
    
    // Multi-threaded test
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&stack, i]() {
            stack.Push(i);
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "Multi-threaded push completed" << std::endl;
    std::cout << "Popping from multi-threaded stack:" << std::endl;
    while (stack.Pop(val)) {
        std::cout << "Popped: " << val << std::endl;
    }
    
    return 0;
}

