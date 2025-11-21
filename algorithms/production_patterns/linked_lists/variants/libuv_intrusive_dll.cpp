/*
 * libuv Intrusive Doubly-Linked Circular List
 * 
 * Source: node/deps/uv/src/queue.h
 * Repository: nodejs/node (libuv dependency)
 * File: `deps/uv/src/queue.h`
 * 
 * What Makes It Ingenious:
 * - Intrusive design: queue node embedded in containing structure (zero allocation overhead)
 * - Circular structure: empty queue points to itself (simplifies empty checks)
 * - O(1) insertion and removal operations
 * - Cache-friendly: data and links are together (better cache locality)
 * - Container-of macro: uses offsetof to get containing structure from queue pointer
 * - Used extensively in Node.js event loop for handle management
 * 
 * When to Use:
 * - Need O(1) insertion/removal at both ends
 * - Memory efficiency critical (no separate node allocations)
 * - Cache performance matters
 * - Queue/FIFO operations
 * - Handle/callback management
 * 
 * Real-World Usage:
 * - Node.js/libuv event loop (handle queues, watcher queues, callback queues)
 * - Process handle queues
 * - Thread pool work queues
 * - OS kernels (Linux kernel uses similar pattern)
 * - Database systems (PostgreSQL, MySQL connection pools)
 * - Networking stacks (nginx, Apache connection management)
 * 
 * Time Complexity:
 * - Insert at head/tail: O(1)
 * - Remove: O(1)
 * - Traversal: O(n)
 * - Empty check: O(1)
 * 
 * Space Complexity: O(1) per element (no extra allocations)
 */

#include <cstddef>
#include <cstdint>

/*
 * Intrusive doubly-linked circular list node
 * 
 * This structure is embedded directly in the containing structure,
 * eliminating the need for separate node allocations.
 */
struct QueueNode {
    QueueNode* next;
    QueueNode* prev;
    
    QueueNode() : next(this), prev(this) {}
};

/*
 * Container-of macro: Get containing structure from queue pointer
 * 
 * Uses offsetof to calculate the address of the containing structure
 * given a pointer to the queue member.
 * 
 * @param pointer: Pointer to the queue member
 * @param type: Type of the containing structure
 * @param field: Name of the queue field in the containing structure
 */
#define queue_data(pointer, type, field) \
    ((type*)((char*)(pointer) - offsetof(type, field)))

/*
 * Initialize an empty queue
 * 
 * An empty queue points to itself (circular structure).
 */
static inline void queue_init(QueueNode* q) {
    q->next = q;
    q->prev = q;
}

/*
 * Check if queue is empty
 */
static inline bool queue_empty(const QueueNode* q) {
    return q == q->next;
}

/*
 * Insert at head (after head node)
 */
static inline void queue_insert_head(QueueNode* h, QueueNode* q) {
    q->next = h->next;
    q->prev = h;
    q->next->prev = q;
    h->next = q;
}

/*
 * Insert at tail (before head node)
 */
static inline void queue_insert_tail(QueueNode* h, QueueNode* q) {
    q->next = h;
    q->prev = h->prev;
    q->prev->next = q;
    h->prev = q;
}

/*
 * Remove element from queue
 */
static inline void queue_remove(QueueNode* q) {
    q->prev->next = q->next;
    q->next->prev = q->prev;
}

/*
 * Add all elements from queue n to queue h
 */
static inline void queue_add(QueueNode* h, QueueNode* n) {
    h->prev->next = n->next;
    n->next->prev = h->prev;
    h->prev = n->prev;
    h->prev->next = h;
}

/*
 * Split queue at element q, creating new queue n
 */
static inline void queue_split(QueueNode* h, QueueNode* q, QueueNode* n) {
    n->prev = h->prev;
    n->prev->next = n;
    n->next = q;
    h->prev = q->prev;
    h->prev->next = h;
    q->prev = n;
}

/*
 * Move all elements from queue h to queue n
 */
static inline void queue_move(QueueNode* h, QueueNode* n) {
    if (queue_empty(h)) {
        queue_init(n);
    } else {
        queue_split(h, h->next, n);
    }
}

// Example usage
#include <iostream>

struct MyItem {
    int data;
    QueueNode q;  // Queue node embedded here
    
    MyItem(int d) : data(d) {}
};

int main() {
    QueueNode head;
    queue_init(&head);
    
    MyItem item1(10);
    MyItem item2(20);
    MyItem item3(30);
    
    // Insert at tail
    queue_insert_tail(&head, &item1.q);
    queue_insert_tail(&head, &item2.q);
    queue_insert_tail(&head, &item3.q);
    
    // Traverse queue
    QueueNode* q;
    for (q = head.next; q != &head; q = q->next) {
        MyItem* item = queue_data(q, MyItem, q);
        std::cout << "Item: " << item->data << std::endl;
    }
    
    // Remove middle item
    queue_remove(&item2.q);
    
    std::cout << "After removal:" << std::endl;
    for (q = head.next; q != &head; q = q->next) {
        MyItem* item = queue_data(q, MyItem, q);
        std::cout << "Item: " << item->data << std::endl;
    }
    
    return 0;
}

