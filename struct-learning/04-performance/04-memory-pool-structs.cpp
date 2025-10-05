/*
 * =============================================================================
 * Performance Engineering: Memory Pool Structs
 * Fixed block allocator for small objects
 * =============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <new>

struct Pool {
    struct Node { Node* next; };
    const size_t block_size;
    const size_t capacity;
    void* storage;
    Node* free_list;

    Pool(size_t block, size_t cap)
        : block_size(block < sizeof(Node) ? sizeof(Node) : block), capacity(cap), storage(nullptr), free_list(nullptr) {
        storage = std::malloc(block_size * capacity);
        if (!storage) throw std::bad_alloc();
        // build free list
        char* p = static_cast<char*>(storage);
        for (size_t i = 0; i < capacity; ++i) {
            auto node = reinterpret_cast<Node*>(p + i * block_size);
            node->next = free_list;
            free_list = node;
        }
    }

    ~Pool() { std::free(storage); }

    void* allocate() {
        if (!free_list) return nullptr;
        Node* n = free_list; free_list = free_list->next; return n;
    }

    void deallocate(void* ptr) {
        if (!ptr) return;
        Node* n = reinterpret_cast<Node*>(ptr);
        n->next = free_list; free_list = n;
    }
};

struct alignas(16) Small { int a; double b; };

int main() {
    try {
        std::cout << "\n=== MEMORY POOL STRUCTS ===" << std::endl;
        Pool pool(sizeof(Small), 128);
        std::vector<Small*> v;
        for (int i = 0; i < 10; ++i) {
            void* p = pool.allocate();
            Small* s = new(p) Small{ i, i * 0.5 };
            v.push_back(s);
        }
        for (auto* s : v) {
            std::cout << s->a << ':' << s->b << ' ';
            s->~Small();
            pool.deallocate(s);
        }
        std::cout << "\n=== MEMORY POOL COMPLETED SUCCESSFULLY ===" << std::endl;
    } catch (...) { std::cerr << "error" << std::endl; return 1; }
    return 0;
}
