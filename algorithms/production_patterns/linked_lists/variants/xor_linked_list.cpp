/*
 * XOR Linked List (Memory-Efficient Doubly-Linked List)
 * 
 * Source: Research/Algorithm technique
 * 
 * What Makes It Ingenious:
 * - Stores XOR of prev and next pointers instead of separate pointers
 * - Reduces memory overhead: 1 pointer instead of 2 for doubly-linked list
 * - Can traverse in both directions with XOR operations
 * - Memory-efficient for memory-constrained systems
 * - Useful when memory is at a premium
 * 
 * When to Use:
 * - Memory-constrained systems (embedded systems, IoT devices)
 * - Need doubly-linked list but memory is limited
 * - Can afford slightly slower traversal (XOR operations)
 * - Memory efficiency more important than speed
 * 
 * Real-World Usage:
 * - Embedded systems
 * - Memory-constrained devices
 * - Systems where memory overhead matters
 * - Educational/research purposes
 * 
 * Time Complexity:
 * - Insert: O(1) at head/tail
 * - Remove: O(1) with node pointer
 * - Traversal: O(n) (slightly slower due to XOR operations)
 * 
 * Space Complexity: O(n) but with 50% less pointer overhead than standard DLL
 */

#include <cstdint>
#include <cstddef>

/*
 * XOR linked list node
 * 
 * Instead of storing prev and next pointers separately,
 * stores XOR(prev, next) which allows bidirectional traversal.
 */
template<typename T>
struct XORNode {
    T data;
    XORNode* xor_ptr;  // XOR of prev and next pointers
    
    XORNode(const T& d) : data(d), xor_ptr(nullptr) {}
};

/*
 * XOR helper function
 * 
 * XOR of two pointers (reinterpret_cast to uintptr_t for arithmetic)
 */
template<typename T>
XORNode<T>* XOR(XORNode<T>* a, XORNode<T>* b) {
    return reinterpret_cast<XORNode<T>*>(
        reinterpret_cast<uintptr_t>(a) ^ reinterpret_cast<uintptr_t>(b)
    );
}

/*
 * XOR Linked List implementation
 */
template<typename T>
class XORLinkedList {
private:
    XORNode<T>* head_;
    XORNode<T>* tail_;
    
public:
    XORLinkedList() : head_(nullptr), tail_(nullptr) {}
    
    // Insert at head
    void InsertHead(const T& data) {
        XORNode<T>* new_node = new XORNode<T>(data);
        
        if (head_ == nullptr) {
            // First node
            new_node->xor_ptr = nullptr;
            head_ = tail_ = new_node;
        } else {
            // Link new node to head
            new_node->xor_ptr = XOR(nullptr, head_);
            head_->xor_ptr = XOR(new_node, XOR(nullptr, head_->xor_ptr));
            head_ = new_node;
        }
    }
    
    // Insert at tail
    void InsertTail(const T& data) {
        XORNode<T>* new_node = new XORNode<T>(data);
        
        if (tail_ == nullptr) {
            // First node
            new_node->xor_ptr = nullptr;
            head_ = tail_ = new_node;
        } else {
            // Link new node to tail
            new_node->xor_ptr = XOR(tail_, nullptr);
            tail_->xor_ptr = XOR(XOR(nullptr, tail_->xor_ptr), new_node);
            tail_ = new_node;
        }
    }
    
    // Traverse forward (head to tail)
    void TraverseForward() {
        XORNode<T>* curr = head_;
        XORNode<T>* prev = nullptr;
        XORNode<T>* next;
        
        while (curr != nullptr) {
            // Process current node
            std::cout << curr->data << " ";
            
            // Calculate next node
            next = XOR(prev, curr->xor_ptr);
            prev = curr;
            curr = next;
        }
        std::cout << std::endl;
    }
    
    // Traverse backward (tail to head)
    void TraverseBackward() {
        XORNode<T>* curr = tail_;
        XORNode<T>* next = nullptr;
        XORNode<T>* prev;
        
        while (curr != nullptr) {
            // Process current node
            std::cout << curr->data << " ";
            
            // Calculate previous node
            prev = XOR(next, curr->xor_ptr);
            next = curr;
            curr = prev;
        }
        std::cout << std::endl;
    }
    
    // Get head
    XORNode<T>* GetHead() const { return head_; }
    
    // Get tail
    XORNode<T>* GetTail() const { return tail_; }
    
    // Check if empty
    bool IsEmpty() const { return head_ == nullptr; }
};

// Example usage
#include <iostream>

int main() {
    XORLinkedList<int> list;
    
    list.InsertTail(10);
    list.InsertTail(20);
    list.InsertTail(30);
    list.InsertHead(5);
    
    std::cout << "Forward traversal: ";
    list.TraverseForward();
    
    std::cout << "Backward traversal: ";
    list.TraverseBackward();
    
    return 0;
}

