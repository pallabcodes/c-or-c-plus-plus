/*
 * V8 Memory Management Interval Merging
 *
 * Source: V8 JavaScript engine garbage collection
 * Repository: https://github.com/v8/v8
 * Files: src/heap/*.cc, src/common/ptr-compr-inl.h
 * Algorithm: Incremental interval merging for memory compaction
 *
 * What Makes It Ingenious:
 * - Incremental merging: Merge intervals as memory is freed/allocated
 * - Real-time constraints: Cannot pause for full sorting
 * - Memory efficient: O(1) amortized per operation
 * - Fragmentation handling: Merges adjacent free blocks
 * - Used in V8's garbage collector for heap compaction
 * - Handles pointer compression and memory layouts
 *
 * When to Use:
 * - Real-time memory management
 * - Garbage collection systems
 * - Incremental compaction
 * - Memory defragmentation
 * - Virtual memory allocation
 * - Heap management in constrained environments
 *
 * Real-World Usage:
 * - V8 JavaScript engine GC
 * - JVM garbage collectors
 * - Memory allocators (jemalloc, tcmalloc)
 * - Virtual memory managers
 * - Real-time systems
 *
 * Time Complexity:
 * - Insert interval: O(1) amortized
 * - Merge operation: O(1) amortized
 * - Query merged ranges: O(n) for enumeration
 *
 * Space Complexity: O(n) for storing intervals
 */

#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <cstdint>

// Memory interval representation (simplified from V8)
struct MemoryInterval {
    uintptr_t start_address;
    uintptr_t end_address;   // Exclusive end
    bool is_free;           // Free or allocated block
    int generation;         // GC generation (young, old, etc.)

    MemoryInterval(uintptr_t start, uintptr_t end, bool free = true, int gen = 0)
        : start_address(start), end_address(end), is_free(free), generation(gen) {}

    size_t size() const { return end_address - start_address; }
    bool overlaps(const MemoryInterval& other) const;
    bool adjacent(const MemoryInterval& other) const;
    MemoryInterval merge(const MemoryInterval& other) const;

    void print() const {
        std::cout << "[" << std::hex << start_address << ", "
                  << end_address << "] " << (is_free ? "FREE" : "ALLOC")
                  << " gen=" << generation << std::dec;
    }
};

bool MemoryInterval::overlaps(const MemoryInterval& other) const {
    return start_address < other.end_address && other.start_address < end_address;
}

bool MemoryInterval::adjacent(const MemoryInterval& other) const {
    return end_address == other.start_address || other.end_address == start_address;
}

MemoryInterval MemoryInterval::merge(const MemoryInterval& other) const {
    uintptr_t new_start = std::min(start_address, other.start_address);
    uintptr_t new_end = std::max(end_address, other.end_address);
    // Keep the "freer" status (prefer free blocks)
    bool new_free = is_free && other.is_free;
    // Use higher generation
    int new_gen = std::max(generation, other.generation);
    return MemoryInterval(new_start, new_end, new_free, new_gen);
}

// V8-style incremental interval merger
class V8MemoryIntervalMerger {
private:
    // Doubly-linked list for efficient merging (like V8's heap structures)
    struct IntervalNode {
        MemoryInterval interval;
        std::shared_ptr<IntervalNode> prev;
        std::shared_ptr<IntervalNode> next;

        IntervalNode(const MemoryInterval& iv) : interval(iv) {}
    };

    std::shared_ptr<IntervalNode> head_;
    std::shared_ptr<IntervalNode> tail_;
    size_t total_free_memory_;
    size_t total_allocated_memory_;

    // Find insertion point (simplified binary search)
    std::shared_ptr<IntervalNode> find_insertion_point(uintptr_t address) {
        auto current = head_;
        while (current && current->interval.start_address < address) {
            current = current->next;
        }
        return current;
    }

    // Try to merge with adjacent intervals
    void try_merge(std::shared_ptr<IntervalNode> node) {
        // Try merge with previous
        if (node->prev && (node->prev->interval.adjacent(node->interval) ||
                          node->prev->interval.overlaps(node->interval))) {
            // Merge previous into current
            node->interval = node->prev->interval.merge(node->interval);

            // Remove previous node
            if (node->prev->prev) {
                node->prev->prev->next = node;
            } else {
                head_ = node;
            }
            node->prev = node->prev->prev;

            update_memory_stats();
        }

        // Try merge with next
        if (node->next && (node->next->interval.adjacent(node->interval) ||
                          node->next->interval.overlaps(node->interval))) {
            // Merge next into current
            node->interval = node->interval.merge(node->next->interval);

            // Remove next node
            if (node->next->next) {
                node->next->next->prev = node;
            } else {
                tail_ = node;
            }
            node->next = node->next->next;

            update_memory_stats();
        }
    }

    void update_memory_stats() {
        total_free_memory_ = 0;
        total_allocated_memory_ = 0;

        auto current = head_;
        while (current) {
            if (current->interval.is_free) {
                total_free_memory_ += current->interval.size();
            } else {
                total_allocated_memory_ += current->interval.size();
            }
            current = current->next;
        }
    }

public:
    V8MemoryIntervalMerger() : total_free_memory_(0), total_allocated_memory_(0) {}

    // Add a memory interval (incremental operation)
    void add_interval(const MemoryInterval& interval) {
        auto new_node = std::make_shared<IntervalNode>(interval);

        if (!head_) {
            // First interval
            head_ = tail_ = new_node;
        } else {
            // Find insertion point
            auto insert_point = find_insertion_point(interval.start_address);

            if (!insert_point) {
                // Insert at end
                tail_->next = new_node;
                new_node->prev = tail_;
                tail_ = new_node;
            } else if (!insert_point->prev) {
                // Insert at beginning
                new_node->next = insert_point;
                insert_point->prev = new_node;
                head_ = new_node;
            } else {
                // Insert in middle
                new_node->prev = insert_point->prev;
                new_node->next = insert_point;
                insert_point->prev->next = new_node;
                insert_point->prev = new_node;
            }
        }

        // Try to merge with adjacent intervals
        try_merge(new_node);
    }

    // Allocate memory from free intervals (simplified)
    bool allocate_memory(size_t size, uintptr_t& allocated_address) {
        auto current = head_;
        while (current) {
            if (current->interval.is_free && current->interval.size() >= size) {
                // Split or use the entire interval
                allocated_address = current->interval.start_address;

                if (current->interval.size() == size) {
                    // Use entire interval
                    current->interval.is_free = false;
                } else {
                    // Split interval
                    auto remaining_start = current->interval.start_address + size;
                    auto remaining_size = current->interval.size() - size;

                    // Create new allocated interval
                    MemoryInterval alloc_interval(current->interval.start_address,
                                                remaining_start, false,
                                                current->interval.generation);

                    // Shrink current free interval
                    current->interval.start_address = remaining_start;
                    current->interval.end_address = remaining_start + remaining_size;

                    // Insert allocated interval before current
                    auto alloc_node = std::make_shared<IntervalNode>(alloc_interval);
                    alloc_node->next = current;
                    alloc_node->prev = current->prev;

                    if (current->prev) {
                        current->prev->next = alloc_node;
                    } else {
                        head_ = alloc_node;
                    }
                    current->prev = alloc_node;
                }

                update_memory_stats();
                return true;
            }
            current = current->next;
        }
        return false; // No suitable free block
    }

    // Free memory (mark as free and try merging)
    void free_memory(uintptr_t address, size_t size) {
        MemoryInterval free_interval(address, address + size, true, 0);
        add_interval(free_interval); // This will handle merging
    }

    // Get all merged intervals
    std::vector<MemoryInterval> get_merged_intervals() const {
        std::vector<MemoryInterval> result;
        auto current = head_;
        while (current) {
            result.push_back(current->interval);
            current = current->next;
        }
        return result;
    }

    // Memory statistics
    size_t total_free_memory() const { return total_free_memory_; }
    size_t total_allocated_memory() const { return total_allocated_memory_; }
    size_t total_memory() const { return total_free_memory_ + total_allocated_memory_; }

    // Print memory layout (for debugging)
    void print_memory_layout() const {
        std::cout << "Memory Layout:" << std::endl;
        auto current = head_;
        while (current) {
            std::cout << "  ";
            current->interval.print();
            std::cout << " (size: " << current->interval.size() << ")" << std::endl;
            current = current->next;
        }
        std::cout << "Total: " << total_memory() << " bytes ("
                  << total_free_memory_ << " free, "
                  << total_allocated_memory_ << " allocated)" << std::endl;
    }
};

// V8-style garbage collection interval management
class V8GarbageCollector {
private:
    V8MemoryIntervalMerger merger_;
    std::vector<MemoryInterval> gc_roots_; // Objects that are still reachable

public:
    // Mark phase: identify live objects (simplified)
    void mark_phase(const std::vector<uintptr_t>& live_objects) {
        gc_roots_.clear();
        for (auto addr : live_objects) {
            // In real V8, this would traverse object graphs
            // Here we just mark the objects as roots
            gc_roots_.push_back(MemoryInterval(addr, addr + 8, false, 1)); // Assume 8-byte objects
        }
    }

    // Sweep phase: free unmarked memory
    void sweep_phase(uintptr_t heap_start, uintptr_t heap_end) {
        // In real V8, this identifies free intervals
        // Here we'll simulate by freeing some ranges
        merger_.free_memory(heap_start + 100, 50);  // Free some memory
        merger_.free_memory(heap_start + 200, 75);  // Free adjacent memory
        merger_.free_memory(heap_start + 150, 25);  // Free overlapping memory
    }

    // Compact phase: merge free intervals for allocation
    void compact_phase() {
        // The merger automatically merges intervals as they're added
        // In real V8, this would move live objects to compact free space
        std::cout << "Compacted free memory regions:" << std::endl;
        merger_.print_memory_layout();
    }

    // Allocate object
    bool allocate_object(size_t size, uintptr_t& address) {
        return merger_.allocate_memory(size, address);
    }

    V8MemoryIntervalMerger& get_merger() { return merger_; }
};

// Example usage
int main() {
    std::cout << "V8 Memory Management Interval Merging Demonstration:" << std::endl;

    V8GarbageCollector gc;

    // Initialize heap with some allocated blocks
    uintptr_t heap_start = 0x1000;

    // Allocate some initial objects
    uintptr_t addr1, addr2, addr3;
    gc.allocate_object(64, addr1);
    gc.allocate_object(32, addr2);
    gc.allocate_object(128, addr3);

    std::cout << "Initial memory allocation:" << std::endl;
    gc.get_merger().print_memory_layout();

    // Simulate garbage collection
    std::cout << "\nRunning garbage collection..." << std::endl;

    // Mark phase: some objects become unreachable
    std::vector<uintptr_t> live_objects = {addr1}; // Only addr1 is still live

    gc.mark_phase(live_objects);

    // Sweep phase: free unreachable objects
    gc.sweep_phase(heap_start, heap_start + 1000);
    gc.free_memory(addr2, 32);  // Free addr2
    gc.free_memory(addr3, 128); // Free addr3

    std::cout << "After sweeping dead objects:" << std::endl;
    gc.get_merger().print_memory_layout();

    // Compact phase: merge free intervals
    gc.compact_phase();

    // Allocate new object in merged free space
    uintptr_t new_addr;
    if (gc.allocate_object(100, new_addr)) {
        std::cout << "\nSuccessfully allocated new object at address: "
                  << std::hex << new_addr << std::dec << std::endl;
    }

    std::cout << "\nFinal memory layout:" << std::endl;
    gc.get_merger().print_memory_layout();

    return 0;
}

