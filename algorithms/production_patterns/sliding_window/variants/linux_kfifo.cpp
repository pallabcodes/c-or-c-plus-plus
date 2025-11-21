/*
 * Linux Kernel kfifo (Ring Buffer)
 * 
 * Source: linux/include/linux/kfifo.h
 * 
 * What Makes It Ingenious:
 * - Lock-free for single reader/writer
 * - Power-of-2 size for efficient modulo (mask instead of modulo)
 * - Bitwise AND instead of modulo operation
 * - Memory barriers for thread safety
 * - Supports DMA operations
 * - Zero-copy operations
 * 
 * When to Use:
 * - Producer-consumer scenarios
 * - Lock-free single reader/writer
 * - High-performance I/O buffers
 * - Kernel-level operations
 * 
 * Real-World Usage:
 * - Linux kernel device drivers
 * - Network packet buffers
 * - Audio/video streaming buffers
 */

#include <cstdint>
#include <cstring>
#include <algorithm>

/*
 * Linux kernel-style kfifo implementation
 * 
 * Key optimizations:
 * 1. Power-of-2 size → use mask instead of modulo
 * 2. Lock-free for single reader/writer
 * 3. Memory barriers for visibility
 * 4. Efficient wrap-around handling
 */
template<typename T>
class Kfifo {
private:
    T* buffer_;
    uint32_t size_;      // Must be power of 2
    uint32_t mask_;      // size_ - 1 (for efficient modulo)
    uint32_t in_;        // Write position
    uint32_t out_;       // Read position
    
    // Ensure size is power of 2
    static uint32_t roundup_pow2(uint32_t size) {
        size--;
        size |= size >> 1;
        size |= size >> 2;
        size |= size >> 4;
        size |= size >> 8;
        size |= size >> 16;
        return size + 1;
    }
    
public:
    Kfifo(uint32_t size) 
        : size_(roundup_pow2(size))
        , mask_(size_ - 1)
        , in_(0)
        , out_(0) {
        buffer_ = new T[size_];
    }
    
    ~Kfifo() {
        delete[] buffer_;
    }
    
    // Put element into fifo (single writer, no locking needed)
    bool put(const T& val) {
        if (is_full()) {
            return false;
        }
        
        // Write element
        buffer_[in_ & mask_] = val;
        
        // Memory barrier (ensures write completes before increment)
        // In real kernel: smp_wmb() or std::atomic_thread_fence
        std::atomic_thread_fence(std::memory_order_release);
        
        in_++;
        return true;
    }
    
    // Get element from fifo (single reader, no locking needed)
    bool get(T& val) {
        if (is_empty()) {
            return false;
        }
        
        // Read element
        val = buffer_[out_ & mask_];
        
        // Memory barrier (ensures read completes before increment)
        // In real kernel: smp_rmb() or std::atomic_thread_fence
        std::atomic_thread_fence(std::memory_order_acquire);
        
        out_++;
        return true;
    }
    
    // Peek at element without removing
    bool peek(T& val) const {
        if (is_empty()) {
            return false;
        }
        val = buffer_[out_ & mask_];
        return true;
    }
    
    // Get length (number of used elements)
    uint32_t len() const {
        return in_ - out_;
    }
    
    // Check if empty
    bool is_empty() const {
        return in_ == out_;
    }
    
    // Check if full
    bool is_full() const {
        return len() > mask_;
    }
    
    // Available space
    uint32_t avail() const {
        return size_ - len();
    }
    
    // Reset fifo
    void reset() {
        in_ = out_ = 0;
    }
    
    // Skip elements (advance read pointer)
    void skip(uint32_t count) {
        out_ += std::min(count, len());
    }
};

// Example usage
#include <iostream>
#include <atomic>

int main() {
    Kfifo<int> fifo(8); // Will round up to 8 (power of 2)
    
    // Put elements
    for (int i = 0; i < 5; i++) {
        fifo.put(i);
    }
    
    std::cout << "Length: " << fifo.len() << std::endl;
    std::cout << "Available: " << fifo.avail() << std::endl;
    
    // Get elements
    int val;
    while (fifo.get(val)) {
        std::cout << "Got: " << val << std::endl;
    }
    
    return 0;
}

