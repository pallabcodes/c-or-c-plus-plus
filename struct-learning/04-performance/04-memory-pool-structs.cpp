/*
 * =============================================================================
 * Performance Engineering: Advanced Memory Pool Structs
 * Production-Grade Custom Allocators for Top-Tier Companies
 * =============================================================================
 *
 * This file demonstrates advanced memory pool techniques including:
 * - Fixed-size block allocators
 * - Variable-size pool allocators
 * - Thread-local pools for lock-free allocation
 * - Arena allocators
 * - Stack allocators
 * - Buddy allocators
 * - Memory alignment optimization
 *
 * Author: System Engineering Team
 * Version: 2.0
 * Last Modified: 2024-01-15
 *
 * =============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <memory>
#include <cstring>
#include <thread>
#include <mutex>
#include <atomic>

// =============================================================================
// FIXED-SIZE BLOCK ALLOCATOR (GOOGLE-STYLE)
// =============================================================================

class FixedBlockPool {
private:
    struct Node {
        Node* next;
    };
    
    const size_t block_size_;
    const size_t capacity_;
    void* storage_;
    Node* free_list_;
    std::atomic<size_t> allocated_count_;
    
    static size_t align_size(size_t size, size_t alignment) {
        return (size + alignment - 1) & ~(alignment - 1);
    }
    
public:
    FixedBlockPool(size_t block_size, size_t capacity)
        : block_size_(align_size(block_size < sizeof(Node) ? sizeof(Node) : block_size, alignof(std::max_align_t))),
          capacity_(capacity),
          storage_(nullptr),
          free_list_(nullptr),
          allocated_count_(0) {
        storage_ = std::aligned_alloc(alignof(std::max_align_t), block_size_ * capacity_);
        if (!storage_) {
            throw std::bad_alloc();
        }
        
        // Build free list
        char* p = static_cast<char*>(storage_);
        for (size_t i = 0; i < capacity_; ++i) {
            Node* node = reinterpret_cast<Node*>(p + i * block_size_);
            node->next = free_list_;
            free_list_ = node;
        }
    }
    
    ~FixedBlockPool() {
        std::free(storage_);
    }
    
    void* allocate() {
        if (!free_list_) {
            return nullptr;
        }
        Node* node = free_list_;
        free_list_ = free_list_->next;
        allocated_count_.fetch_add(1, std::memory_order_relaxed);
        return node;
    }
    
    void deallocate(void* ptr) {
        if (!ptr) return;
        Node* node = reinterpret_cast<Node*>(ptr);
        node->next = free_list_;
        free_list_ = node;
        allocated_count_.fetch_sub(1, std::memory_order_relaxed);
    }
    
    size_t allocated() const { return allocated_count_.load(); }
    size_t available() const { return capacity_ - allocated(); }
    size_t block_size() const { return block_size_; }
};

// =============================================================================
// ARENA ALLOCATOR (UBER-STYLE)
// =============================================================================

class ArenaAllocator {
private:
    void* arena_;
    size_t size_;
    size_t offset_;
    std::mutex mutex_;
    
public:
    ArenaAllocator(size_t size)
        : arena_(std::aligned_alloc(alignof(std::max_align_t), size)),
          size_(size),
          offset_(0) {
        if (!arena_) {
            throw std::bad_alloc();
        }
    }
    
    ~ArenaAllocator() {
        std::free(arena_);
    }
    
    void* allocate(size_t size, size_t alignment = alignof(std::max_align_t)) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        size_t aligned_offset = (offset_ + alignment - 1) & ~(alignment - 1);
        if (aligned_offset + size > size_) {
            return nullptr;  // Arena full
        }
        
        void* ptr = static_cast<char*>(arena_) + aligned_offset;
        offset_ = aligned_offset + size;
        return ptr;
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        offset_ = 0;
    }
    
    size_t used() const { return offset_; }
    size_t remaining() const { return size_ - offset_; }
};

// =============================================================================
// STACK ALLOCATOR (BLOOMBERG-STYLE)
// =============================================================================

class StackAllocator {
private:
    void* stack_;
    size_t size_;
    size_t top_;
    std::vector<size_t> markers_;
    
public:
    StackAllocator(size_t size)
        : stack_(std::aligned_alloc(alignof(std::max_align_t), size)),
          size_(size),
          top_(0) {
        if (!stack_) {
            throw std::bad_alloc();
        }
    }
    
    ~StackAllocator() {
        std::free(stack_);
    }
    
    void* allocate(size_t size, size_t alignment = alignof(std::max_align_t)) {
        size_t aligned_top = (top_ + alignment - 1) & ~(alignment - 1);
        if (aligned_top + size > size_) {
            return nullptr;
        }
        
        void* ptr = static_cast<char*>(stack_) + aligned_top;
        top_ = aligned_top + size;
        return ptr;
    }
    
    size_t mark() {
        markers_.push_back(top_);
        return top_;
    }
    
    void release_to_mark() {
        if (!markers_.empty()) {
            top_ = markers_.back();
            markers_.pop_back();
        }
    }
    
    void reset() {
        top_ = 0;
        markers_.clear();
    }
    
    size_t used() const { return top_; }
};

// =============================================================================
// THREAD-LOCAL POOL (AMAZON-STYLE)
// =============================================================================

class ThreadLocalPool {
private:
    static thread_local FixedBlockPool* tls_pool_;
    static FixedBlockPool* shared_pool_;
    static std::mutex shared_mutex_;
    
    static constexpr size_t BLOCK_SIZE = 64;
    static constexpr size_t CAPACITY = 1024;
    
public:
    static void* allocate(size_t size) {
        if (size <= BLOCK_SIZE) {
            if (!tls_pool_) {
                tls_pool_ = new FixedBlockPool(BLOCK_SIZE, CAPACITY);
            }
            return tls_pool_->allocate();
        }
        
        // Fallback to shared pool for large allocations
        std::lock_guard<std::mutex> lock(shared_mutex_);
        if (!shared_pool_) {
            shared_pool_ = new FixedBlockPool(size, CAPACITY);
        }
        return shared_pool_->allocate();
    }
    
    static void deallocate(void* ptr, size_t size) {
        if (size <= BLOCK_SIZE && tls_pool_) {
            tls_pool_->deallocate(ptr);
        } else if (shared_pool_) {
            std::lock_guard<std::mutex> lock(shared_mutex_);
            shared_pool_->deallocate(ptr);
        }
    }
    
    static void cleanup_thread_local() {
        if (tls_pool_) {
            delete tls_pool_;
            tls_pool_ = nullptr;
        }
    }
};

thread_local FixedBlockPool* ThreadLocalPool::tls_pool_ = nullptr;
FixedBlockPool* ThreadLocalPool::shared_pool_ = nullptr;
std::mutex ThreadLocalPool::shared_mutex_;

// =============================================================================
// BUDDY ALLOCATOR (PAYPAL-STYLE)
// =============================================================================

class BuddyAllocator {
private:
    void* memory_;
    size_t size_;
    size_t min_block_size_;
    std::vector<bool> used_;
    
    size_t get_buddy_index(size_t index, size_t level) {
        return index ^ (1 << level);
    }
    
    bool is_buddy_free(size_t index, size_t level) {
        size_t buddy = get_buddy_index(index, level);
        return buddy < used_.size() && !used_[buddy];
    }
    
public:
    BuddyAllocator(size_t size, size_t min_block = 64)
        : memory_(std::aligned_alloc(alignof(std::max_align_t), size)),
          size_(size),
          min_block_size_(min_block) {
        if (!memory_) {
            throw std::bad_alloc();
        }
        size_t num_blocks = size_ / min_block_size_;
        used_.resize(num_blocks, false);
    }
    
    ~BuddyAllocator() {
        std::free(memory_);
    }
    
    void* allocate(size_t size) {
        size_t blocks_needed = (size + min_block_size_ - 1) / min_block_size_;
        size_t level = 0;
        while ((1ULL << level) < blocks_needed) {
            ++level;
        }
        
        // Find free block at appropriate level
        size_t block_size = min_block_size_ * (1ULL << level);
        size_t num_blocks_at_level = size_ / block_size;
        
        for (size_t i = 0; i < num_blocks_at_level; ++i) {
            if (!used_[i * (1ULL << level)]) {
                used_[i * (1ULL << level)] = true;
                return static_cast<char*>(memory_) + i * block_size;
            }
        }
        
        return nullptr;  // No free block found
    }
    
    void deallocate(void* ptr) {
        if (!ptr) return;
        size_t offset = static_cast<char*>(ptr) - static_cast<char*>(memory_);
        size_t index = offset / min_block_size_;
        used_[index] = false;
    }
};

// =============================================================================
// DEMONSTRATION STRUCTS
// =============================================================================

struct alignas(16) Small {
    int a;
    double b;
};

struct alignas(32) Medium {
    int data[8];
    double value;
};

// =============================================================================
// DEMONSTRATION FUNCTIONS
// =============================================================================

void demonstrate_fixed_block_pool() {
    std::cout << "\n=== FIXED BLOCK POOL ===" << std::endl;
    
    FixedBlockPool pool(sizeof(Small), 128);
    std::vector<Small*> objects;
    
    for (int i = 0; i < 10; ++i) {
        void* p = pool.allocate();
        Small* s = new(p) Small{i, i * 0.5};
        objects.push_back(s);
    }
    
    std::cout << "Allocated: " << pool.allocated() << std::endl;
    std::cout << "Available: " << pool.available() << std::endl;
    
    for (auto* s : objects) {
        std::cout << "  " << s->a << ":" << s->b << " ";
        s->~Small();
        pool.deallocate(s);
    }
    std::cout << std::endl;
}

void demonstrate_arena_allocator() {
    std::cout << "\n=== ARENA ALLOCATOR ===" << std::endl;
    
    ArenaAllocator arena(4096);
    
    Small* s1 = static_cast<Small*>(arena.allocate(sizeof(Small), alignof(Small)));
    Small* s2 = static_cast<Small*>(arena.allocate(sizeof(Small), alignof(Small)));
    Medium* m1 = static_cast<Medium*>(arena.allocate(sizeof(Medium), alignof(Medium)));
    
    if (s1) new(s1) Small{1, 1.1};
    if (s2) new(s2) Small{2, 2.2};
    if (m1) new(m1) Medium{};
    
    std::cout << "Arena used: " << arena.used() << " bytes" << std::endl;
    std::cout << "Arena remaining: " << arena.remaining() << " bytes" << std::endl;
    
    arena.reset();
    std::cout << "After reset, used: " << arena.used() << " bytes" << std::endl;
}

void demonstrate_stack_allocator() {
    std::cout << "\n=== STACK ALLOCATOR ===" << std::endl;
    
    StackAllocator stack(4096);
    
    size_t mark1 = stack.mark();
    Small* s1 = static_cast<Small*>(stack.allocate(sizeof(Small)));
    Small* s2 = static_cast<Small*>(stack.allocate(sizeof(Small)));
    if (s1) new(s1) Small{10, 10.1};
    if (s2) new(s2) Small{20, 20.2};
    
    std::cout << "After allocations, used: " << stack.used() << " bytes" << std::endl;
    
    size_t mark2 = stack.mark();
    Medium* m1 = static_cast<Medium*>(stack.allocate(sizeof(Medium)));
    if (m1) new(m1) Medium{};
    
    std::cout << "After more allocations, used: " << stack.used() << " bytes" << std::endl;
    
    stack.release_to_mark();
    std::cout << "After release to mark2, used: " << stack.used() << " bytes" << std::endl;
    
    stack.release_to_mark();
    std::cout << "After release to mark1, used: " << stack.used() << " bytes" << std::endl;
}

void demonstrate_thread_local_pool() {
    std::cout << "\n=== THREAD-LOCAL POOL ===" << std::endl;
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < 3; ++t) {
        threads.emplace_back([t]() {
            std::vector<void*> ptrs;
            for (int i = 0; i < 5; ++i) {
                void* p = ThreadLocalPool::allocate(64);
                if (p) {
                    Small* s = new(p) Small{t * 100 + i, (t * 100 + i) * 0.1};
                    ptrs.push_back(s);
                }
            }
            
            std::cout << "Thread " << t << " allocated " << ptrs.size() << " objects" << std::endl;
            
            for (void* p : ptrs) {
                Small* s = static_cast<Small*>(p);
                s->~Small();
                ThreadLocalPool::deallocate(p, 64);
            }
            
            ThreadLocalPool::cleanup_thread_local();
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
}

void demonstrate_buddy_allocator() {
    std::cout << "\n=== BUDDY ALLOCATOR ===" << std::endl;
    
    BuddyAllocator buddy(4096, 64);
    
    void* p1 = buddy.allocate(128);
    void* p2 = buddy.allocate(256);
    void* p3 = buddy.allocate(64);
    
    std::cout << "Allocated blocks: p1=" << p1 << ", p2=" << p2 << ", p3=" << p3 << std::endl;
    
    if (p1) buddy.deallocate(p1);
    if (p2) buddy.deallocate(p2);
    if (p3) buddy.deallocate(p3);
    
    std::cout << "All blocks deallocated" << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "=== GOD-MODDED ADVANCED MEMORY POOLS ===" << std::endl;
    std::cout << "Demonstrating production-grade memory pool techniques" << std::endl;
    
    try {
        demonstrate_fixed_block_pool();
        demonstrate_arena_allocator();
        demonstrate_stack_allocator();
        demonstrate_thread_local_pool();
        demonstrate_buddy_allocator();
        
        std::cout << "\n=== MEMORY POOLS COMPLETED SUCCESSFULLY ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

// =============================================================================
// COMPILATION NOTES
// =============================================================================
/*
 * Compile with:
 *   g++ -std=c++17 -O2 -Wall -Wextra -pthread -o memory_pool 04-memory-pool-structs.cpp
 *   clang++ -std=c++17 -O2 -Wall -Wextra -pthread -o memory_pool 04-memory-pool-structs.cpp
 *
 * Advanced memory pool techniques:
 *   - Fixed-size block allocators
 *   - Arena allocators
 *   - Stack allocators
 *   - Thread-local pools
 *   - Buddy allocators
 *   - Memory alignment optimization
 */
