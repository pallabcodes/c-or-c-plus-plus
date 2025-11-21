/*
 * V8 Threaded List (Singly Linked)
 * 
 * Source: node/deps/v8/src/base/threaded-list.h
 * Repository: v8/v8 (via nodejs/node)
 * File: `src/base/threaded-list.h`
 * 
 * What Makes It Ingenious:
 * - Intrusive singly-linked list that threads through nodes
 * - Tail pointer caching for O(1) append operations
 * - Iterator support with STL-compatible iterators
 * - Unsafe insertion support for performance-critical paths
 * - Template-based with traits for customization
 * - Used in V8 for compiler intermediate representation
 * 
 * When to Use:
 * - Need singly-linked list with O(1) append
 * - Iterator support required
 * - Performance-critical insertion operations
 * - Compiler/interpreter data structures
 * - Need to customize node access patterns
 * 
 * Real-World Usage:
 * - V8 JavaScript engine compiler (intermediate representation)
 * - V8 TurboFan compiler work lists
 * - Code generation data structures
 * - Compiler optimization passes
 * 
 * Time Complexity:
 * - Add (append): O(1) with tail caching
 * - AddFront: O(1)
 * - Remove: O(n) worst case (must find previous node)
 * - Traversal: O(n)
 * 
 * Space Complexity: O(1) per element (no extra allocations)
 */

#include <cstddef>
#include <iterator>

/*
 * Threaded list traits - default implementation
 * Nodes must have a next() method returning T**
 */
template<typename T>
struct ThreadedListTraits {
    static T** next(T* t) { return t->next(); }
    static T** start(T** t) { return t; }
    static T* const* start(T* const* t) { return t; }
};

/*
 * Threaded list base implementation
 * 
 * Key features:
 * - Head pointer: Points to first element
 * - Tail pointer: Points to location where next element will be stored
 * - Intrusive: Node pointers are stored in the nodes themselves
 */
template<typename T, typename TLTraits = ThreadedListTraits<T>,
         bool kSupportsUnsafeInsertion = false>
class ThreadedList {
private:
    T* head_;
    mutable T** tail_;  // Mutable for const iterators
    
    void EnsureValidTail() const {
        if (!kSupportsUnsafeInsertion) {
            return;  // Tail is always valid
        }
        // Recover tail if unsafe insertion was used
        if (*tail_ == nullptr) return;
        T* last = *tail_;
        while (*TLTraits::next(last) != nullptr) {
            last = *TLTraits::next(last);
        }
        tail_ = TLTraits::next(last);
    }
    
public:
    ThreadedList() : head_(nullptr), tail_(&head_) {}
    
    // Add element at end (O(1) with tail caching)
    void Add(T* v) {
        EnsureValidTail();
        *tail_ = v;
        tail_ = TLTraits::next(v);
    }
    
    // Add element at front (O(1))
    void AddFront(T* v) {
        T** const next = TLTraits::next(v);
        *next = head_;
        if (head_ == nullptr) tail_ = next;
        head_ = v;
    }
    
    // Remove element (O(n) worst case)
    bool Remove(T* v) {
        T* current = head_;
        if (current == v) {
            head_ = *TLTraits::next(head_);
            if (head_ == nullptr) tail_ = &head_;
            return true;
        }
        
        EnsureValidTail();
        while (current != nullptr) {
            T* next = *TLTraits::next(current);
            if (next == v) {
                *TLTraits::next(current) = *TLTraits::next(next);
                *TLTraits::next(next) = nullptr;
                
                if (TLTraits::next(next) == tail_) {
                    tail_ = TLTraits::next(current);
                }
                return true;
            }
            current = next;
        }
        return false;
    }
    
    // Iterator support
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T*;
        using reference = value_type;
        using pointer = value_type*;
        
        Iterator& operator++() {
            entry_ = TLTraits::next(*entry_);
            return *this;
        }
        
        bool operator==(const Iterator& other) const {
            return entry_ == other.entry_;
        }
        
        T*& operator*() { return *entry_; }
        T* operator->() { return *entry_; }
        
    private:
        explicit Iterator(T** entry) : entry_(entry) {}
        T** entry_;
        friend class ThreadedList;
    };
    
    Iterator begin() {
        EnsureValidTail();
        return Iterator(TLTraits::start(&head_));
    }
    
    Iterator end() {
        EnsureValidTail();
        return Iterator(tail_);
    }
    
    bool is_empty() const { return head_ == nullptr; }
    T* first() const { return head_; }
    
    void Clear() {
        head_ = nullptr;
        tail_ = &head_;
    }
};

// Example usage
#include <iostream>

struct MyNode {
    int data;
    MyNode* next_ptr;
    
    MyNode(int d) : data(d), next_ptr(nullptr) {}
    
    MyNode** next() { return &next_ptr; }
};

int main() {
    ThreadedList<MyNode> list;
    
    MyNode node1(10);
    MyNode node2(20);
    MyNode node3(30);
    
    list.Add(&node1);
    list.Add(&node2);
    list.Add(&node3);
    
    // Traverse using iterator
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << "Node: " << (*it)->data << std::endl;
    }
    
    // Remove middle node
    list.Remove(&node2);
    
    std::cout << "After removal:" << std::endl;
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << "Node: " << (*it)->data << std::endl;
    }
    
    return 0;
}

