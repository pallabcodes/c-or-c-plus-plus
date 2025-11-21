/*
 * V8 Doubly-Threaded List
 * 
 * Source: node/deps/v8/src/base/doubly-threaded-list.h
 * Repository: v8/v8 (via nodejs/node)
 * File: `src/base/doubly-threaded-list.h`
 * 
 * What Makes It Ingenious:
 * - Intrusive doubly-linked list with special prev pointer design
 * - Prev pointer stores address of previous node's next pointer (not previous node itself)
 * - O(1) removal without knowing list head (can remove from middle)
 * - No special cases for head removal
 * - Iterator support with forward iteration
 * - Used in V8 for compiler data structures requiring efficient removal
 * 
 * When to Use:
 * - Need O(1) removal from middle without list head
 * - Doubly-linked list operations
 * - Compiler/interpreter data structures
 * - Need efficient removal during iteration
 * - Iterator-based algorithms
 * 
 * Real-World Usage:
 * - V8 JavaScript engine compiler
 * - V8 TurboFan optimization passes
 * - Code generation data structures
 * - Compiler intermediate representation
 * 
 * Time Complexity:
 * - PushFront: O(1)
 * - Remove: O(1) (no need to find previous node)
 * - Traversal: O(n)
 * 
 * Space Complexity: O(1) per element (no extra allocations)
 */

#include <cstddef>

/*
 * Doubly-threaded list traits
 * Nodes must have prev() and next() methods
 */
template<typename T>
struct DoublyThreadedListTraits {
    static T** prev(T t) { return t->prev(); }
    static T* next(T t) { return t->next(); }
    static bool non_empty(T t) { return t != nullptr; }
};

/*
 * Doubly-threaded list
 * 
 * Key innovation: prev pointer stores address of previous node's next pointer
 * This allows O(1) removal without knowing the list head.
 */
template<typename T, typename DTLTraits = DoublyThreadedListTraits<T>>
class DoublyThreadedList {
private:
    T head_;
    
    static bool empty(T x) { return !DTLTraits::non_empty(x); }
    
public:
    // End iterator marker
    class end_iterator {};
    
    // Forward iterator
    class iterator {
    public:
        explicit iterator(T head) : curr_(head) {}
        
        T operator*() { return curr_; }
        
        iterator& operator++() {
            curr_ = *DTLTraits::next(curr_);
            return *this;
        }
        
        iterator operator++(int) {
            iterator tmp(*this);
            operator++();
            return tmp;
        }
        
        bool operator==(end_iterator) {
            return !DTLTraits::non_empty(curr_);
        }
        
    private:
        friend DoublyThreadedList;
        T curr_;
    };
    
    DoublyThreadedList() : head_(nullptr) {}
    
    // Move constructor
    DoublyThreadedList(DoublyThreadedList&& other) {
        head_ = other.head_;
        if (DTLTraits::non_empty(head_)) {
            *DTLTraits::prev(head_) = &head_;
        }
        other.head_ = nullptr;
    }
    
    // Add element at front
    void PushFront(T x) {
        *DTLTraits::next(x) = head_;
        *DTLTraits::prev(x) = &head_;
        if (DTLTraits::non_empty(head_)) {
            *DTLTraits::prev(head_) = DTLTraits::next(x);
        }
        head_ = x;
    }
    
    // Remove element (O(1) - no need to know list head!)
    static void Remove(T x) {
        if (*DTLTraits::prev(x) == nullptr) {
            // Already removed
            return;
        }
        
        T** prev = DTLTraits::prev(x);
        T* next = DTLTraits::next(x);
        
        // Update previous node's next pointer
        **prev = *next;
        
        // Update next node's prev pointer (if exists)
        if (DTLTraits::non_empty(*next)) {
            *DTLTraits::prev(*next) = *prev;
        }
        
        // Clear x's pointers
        *DTLTraits::prev(x) = nullptr;
        *DTLTraits::next(x) = nullptr;
    }
    
    T Front() const {
        return head_;
    }
    
    void PopFront() {
        Remove(Front());
    }
    
    bool empty() const {
        return !DTLTraits::non_empty(head_);
    }
    
    iterator begin() const {
        return iterator{head_};
    }
    
    end_iterator end() const {
        return end_iterator{};
    }
    
    // Remove element at iterator position
    iterator RemoveAt(iterator& it) {
        T curr = *it;
        T next = *DTLTraits::next(curr);
        Remove(curr);
        return iterator{next};
    }
};

// Example usage
#include <iostream>

struct MyNode {
    int data;
    MyNode* next_ptr;
    MyNode** prev_ptr;  // Stores address of previous node's next pointer
    
    MyNode(int d) : data(d), next_ptr(nullptr), prev_ptr(nullptr) {}
    
    MyNode** prev() { return &prev_ptr; }
    MyNode* next() { return &next_ptr; }
};

int main() {
    DoublyThreadedList<MyNode*> list;
    
    MyNode node1(10);
    MyNode node2(20);
    MyNode node3(30);
    
    list.PushFront(&node1);
    list.PushFront(&node2);
    list.PushFront(&node3);
    
    // Traverse forward
    std::cout << "Forward traversal:" << std::endl;
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << "Node: " << (*it)->data << std::endl;
    }
    
    // Remove middle node (O(1) without knowing head!)
    DoublyThreadedList<MyNode*>::Remove(&node2);
    
    std::cout << "After removal:" << std::endl;
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << "Node: " << (*it)->data << std::endl;
    }
    
    return 0;
}

