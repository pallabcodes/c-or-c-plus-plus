/*
 * Linux Kernel Intrusive Doubly-Linked Circular List
 * 
 * Source: linux/include/linux/list.h
 * Local Path: `/Users/picon/Learning/c-or-c-plus-plus/linux/include/linux/list.h`
 * 
 * What Makes It Ingenious:
 * - Circular doubly-linked list implementation
 * - Intrusive design: list_head embedded in containing structure
 * - Container-of macro: uses offsetof to get containing structure
 * - List hardening: corruption detection in debug builds
 * - Memory barriers: WRITE_ONCE for multi-core safety
 * - Poison pointers: LIST_POISON1/2 for debugging use-after-free
 * - Extensive iterator macros: list_for_each, list_for_each_entry, etc.
 * - Used throughout Linux kernel for process management, file descriptors, etc.
 * 
 * When to Use:
 * - Kernel-level code requiring list operations
 * - Need corruption detection in debug builds
 * - Multi-core systems requiring memory barriers
 * - Memory-efficient list operations
 * - Need extensive iterator support
 * 
 * Real-World Usage:
 * - Linux kernel process management
 * - Linux kernel file descriptor tables
 * - Linux kernel network subsystem
 * - Linux kernel device drivers
 * - System-level list operations
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

// Simplified list_head structure (from Linux kernel)
struct list_head {
    struct list_head* next;
    struct list_head* prev;
    
    list_head() : next(this), prev(this) {}
};

// Container-of macro (from Linux kernel)
#define container_of(ptr, type, member) \
    ((type*)((char*)(ptr) - offsetof(type, member)))

// List poisoning (for debugging use-after-free)
#define LIST_POISON1 ((list_head*)0x00100100)
#define LIST_POISON2 ((list_head*)0x00200200)

// Initialize list head
static inline void INIT_LIST_HEAD(list_head* list) {
    list->next = list;
    list->prev = list;
}

// Check if list is empty
static inline int list_empty(const list_head* head) {
    return head->next == head;
}

// Internal function: Insert between two known consecutive entries
static inline void __list_add(list_head* new_entry,
                              list_head* prev,
                              list_head* next) {
    next->prev = new_entry;
    new_entry->next = next;
    new_entry->prev = prev;
    prev->next = new_entry;
}

// Add entry after head (for stacks)
static inline void list_add(list_head* new_entry, list_head* head) {
    __list_add(new_entry, head, head->next);
}

// Add entry before head (for queues)
static inline void list_add_tail(list_head* new_entry, list_head* head) {
    __list_add(new_entry, head->prev, head);
}

// Internal function: Delete between two known consecutive entries
static inline void __list_del(list_head* prev, list_head* next) {
    next->prev = prev;
    prev->next = next;
}

// Delete entry from list
static inline void list_del(list_head* entry) {
    __list_del(entry->prev, entry->next);
    entry->next = LIST_POISON1;
    entry->prev = LIST_POISON2;
}

// Delete entry and reinitialize
static inline void list_del_init(list_head* entry) {
    __list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}

// Replace old entry with new entry
static inline void list_replace(list_head* old, list_head* new_entry) {
    new_entry->next = old->next;
    new_entry->next->prev = new_entry;
    new_entry->prev = old->prev;
    new_entry->prev->next = new_entry;
}

// Move entry from one list to another
static inline void list_move(list_head* list, list_head* head) {
    __list_del(list->prev, list->next);
    list_add(list, head);
}

// Move entry to tail of another list
static inline void list_move_tail(list_head* list, list_head* head) {
    __list_del(list->prev, list->next);
    list_add_tail(list, head);
}

// Check if entry is last in list
static inline int list_is_last(const list_head* list, const list_head* head) {
    return list->next == head;
}

// Check if list has only one entry
static inline int list_is_singular(const list_head* head) {
    return !list_empty(head) && (head->next == head->prev);
}

// Rotate list to left (move first entry to end)
static inline void list_rotate_left(list_head* head) {
    if (!list_empty(head)) {
        list_move_tail(head->next, head);
    }
}

// Example usage
#include <iostream>

struct MyItem {
    int data;
    list_head list;  // List node embedded here
    
    MyItem(int d) : data(d) {}
};

// Iterator macro (simplified)
#define list_for_each_entry(pos, head, member) \
    for (pos = container_of((head)->next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = container_of(pos->member.next, typeof(*pos), member))

int main() {
    list_head head;
    INIT_LIST_HEAD(&head);
    
    MyItem item1(10);
    MyItem item2(20);
    MyItem item3(30);
    
    INIT_LIST_HEAD(&item1.list);
    INIT_LIST_HEAD(&item2.list);
    INIT_LIST_HEAD(&item3.list);
    
    // Add to tail (queue)
    list_add_tail(&item1.list, &head);
    list_add_tail(&item2.list, &head);
    list_add_tail(&item3.list, &head);
    
    // Traverse list
    MyItem* pos;
    list_for_each_entry(pos, &head, list) {
        std::cout << "Item: " << pos->data << std::endl;
    }
    
    // Remove middle item
    list_del(&item2.list);
    
    std::cout << "After removal:" << std::endl;
    list_for_each_entry(pos, &head, list) {
        std::cout << "Item: " << pos->data << std::endl;
    }
    
    return 0;
}

