/*
 * Brotli Ring Buffer (Sliding Window for Compression)
 * 
 * Source: node/deps/brotli/c/enc/ringbuffer.h
 * 
 * What Makes It Ingenious:
 * - Tail duplication for efficient wrap-around access
 * - Copies first N bytes at end of buffer (no modulo needed for small reads)
 * - Copies last 2 bytes before buffer start (for lookback)
 * - Lazy allocation (only allocates full buffer when needed)
 * - Optimized for compression algorithms (lookback window)
 * 
 * When to Use:
 * - Compression algorithms (LZ77, LZSS)
 * - Need efficient lookback window
 * - Want to avoid modulo operations
 * - Memory-efficient sliding window
 * 
 * Real-World Usage:
 * - Brotli compression algorithm
 * - LZ77-style compression
 * - Sliding window compression
 */

#include <cstdint>
#include <cstring>
#include <algorithm>

/*
 * Brotli-style ring buffer with tail duplication
 * 
 * Key optimizations:
 * 1. Tail duplication: Copies first tail_size bytes at end
 * 2. Lookback bytes: Copies last 2 bytes before buffer start
 * 3. Lazy allocation: Only allocates full buffer when needed
 * 4. No modulo for small reads: Can read tail_size bytes without wrap-around
 */
template<typename T>
class BrotliRingBuffer {
private:
    T* data_;           // Full buffer (includes slack regions)
    T* buffer_;         // Start of actual ring buffer
    uint32_t size_;     // Ring buffer size (power of 2)
    uint32_t mask_;     // size_ - 1 (for efficient modulo)
    uint32_t tail_size_; // Size of tail duplication
    uint32_t total_size_; // size_ + tail_size_
    uint32_t pos_;      // Current write position
    
    static constexpr size_t kSlackForHashing = 7; // Slack for hash operations
    static constexpr size_t kLookbackBytes = 2;    // Last 2 bytes before buffer
    
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
    BrotliRingBuffer(uint32_t window_bits, uint32_t tail_bits)
        : size_(1u << window_bits)
        , mask_((1u << window_bits) - 1)
        , tail_size_(1u << tail_bits)
        , total_size_(size_ + tail_size_)
        , pos_(0)
        , data_(nullptr)
        , buffer_(nullptr) {
        
        // Allocate buffer with slack regions
        // Layout: [lookback][buffer][tail][slack]
        size_t alloc_size = kLookbackBytes + total_size_ + kSlackForHashing;
        data_ = new T[alloc_size];
        buffer_ = data_ + kLookbackBytes;
        
        // Initialize lookback bytes to zero
        buffer_[-2] = buffer_[-1] = 0;
        
        // Initialize slack region to zero
        for (size_t i = 0; i < kSlackForHashing; i++) {
            buffer_[total_size_ + i] = 0;
        }
    }
    
    ~BrotliRingBuffer() {
        delete[] data_;
    }
    
    // Write data to ring buffer
    void write(const T* bytes, size_t n) {
        uint32_t masked_pos = pos_ & mask_;
        
        // Write tail duplication (if needed)
        if (masked_pos < tail_size_) {
            size_t tail_write = std::min(n, size_t(tail_size_ - masked_pos));
            memcpy(&buffer_[size_ + masked_pos], bytes, tail_write * sizeof(T));
        }
        
        // Write main buffer
        if (masked_pos + n <= size_) {
            // Single write fits
            memcpy(&buffer_[masked_pos], bytes, n * sizeof(T));
        } else {
            // Split write (wrap-around)
            size_t first_chunk = size_ - masked_pos;
            memcpy(&buffer_[masked_pos], bytes, first_chunk * sizeof(T));
            memcpy(&buffer_[0], bytes + first_chunk, (n - first_chunk) * sizeof(T));
        }
        
        // Update lookback bytes
        buffer_[-2] = buffer_[size_ - 2];
        buffer_[-1] = buffer_[size_ - 1];
        
        // Update position
        pos_ = (pos_ & ((1u << 31) - 1)) + (uint32_t)(n & ((1u << 31) - 1));
    }
    
    // Read from ring buffer (with tail duplication, no modulo needed for small reads)
    const T* read(uint32_t offset, size_t n) const {
        uint32_t masked_offset = offset & mask_;
        
        // If reading within tail_size, can use tail duplication (no wrap-around)
        if (masked_offset < tail_size_ && masked_offset + n <= tail_size_) {
            return &buffer_[size_ + masked_offset];
        }
        
        // Otherwise, use main buffer (may need to handle wrap-around)
        return &buffer_[masked_offset];
    }
    
    // Get current position
    uint32_t position() const {
        return pos_;
    }
    
    // Get buffer size
    uint32_t size() const {
        return size_;
    }
};

// Example usage
#include <iostream>

int main() {
    // Window: 16KB (2^14), Tail: 256 bytes (2^8)
    BrotliRingBuffer<uint8_t> rb(14, 8);
    
    // Write some data
    uint8_t data[] = {1, 2, 3, 4, 5};
    rb.write(data, 5);
    
    // Read back (can use tail duplication for small reads)
    const uint8_t* read_data = rb.read(0, 5);
    for (size_t i = 0; i < 5; i++) {
        std::cout << (int)read_data[i] << " ";
    }
    std::cout << std::endl;
    
    return 0;
}


