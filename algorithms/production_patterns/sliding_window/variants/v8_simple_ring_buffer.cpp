/*
 * V8 Simple Ring Buffer
 * 
 * Source: node/deps/v8/src/base/ring-buffer.h
 * 
 * What Makes It Ingenious:
 * - Constexpr (compile-time) ring buffer
 * - Fixed size template parameter
 * - Simple position tracking (no modulo needed until full)
 * - Efficient for small, fixed-size buffers
 * - Used for metrics/history tracking
 * 
 * When to Use:
 * - Small fixed-size buffers
 * - Metrics/history tracking
 * - Compile-time known size
 * - Simple circular buffer needs
 * 
 * Real-World Usage:
 * - V8 performance metrics
 * - History tracking
 * - Small circular buffers
 */

#include <cstdint>
#include <algorithm>

template<typename T, uint8_t SIZE = 10>
class V8RingBuffer {
public:
    static constexpr uint8_t kSize = SIZE;
    
    constexpr V8RingBuffer() 
        : pos_(0)
        , is_full_(false) {
    }
    
    // Push element (overwrites oldest when full)
    constexpr void Push(const T& value) {
        elements_[pos_++] = value;
        if (pos_ == kSize) {
            pos_ = 0;
            is_full_ = true;
        }
    }
    
    // Get current size
    constexpr uint8_t Size() const {
        return is_full_ ? kSize : pos_;
    }
    
    // Check if empty
    constexpr bool Empty() const {
        return Size() == 0;
    }
    
    // Clear buffer
    constexpr void Clear() {
        pos_ = 0;
        is_full_ = false;
    }
    
    // Reduce/fold over buffer (processes from oldest to newest)
    template<typename Callback>
    constexpr T Reduce(Callback callback, const T& initial) const {
        T result = initial;
        
        // Process elements from pos_ to end (oldest)
        for (uint8_t i = pos_; i > 0; --i) {
            result = callback(result, elements_[i - 1]);
        }
        
        // If full, process remaining elements (newest)
        if (is_full_) {
            for (uint8_t i = kSize; i > pos_; --i) {
                result = callback(result, elements_[i - 1]);
            }
        }
        
        return result;
    }
    
    // Access element by index (0 = oldest, Size()-1 = newest)
    constexpr const T& operator[](uint8_t index) const {
        if (is_full_) {
            return elements_[(pos_ + index) % kSize];
        }
        return elements_[index];
    }
    
private:
    T elements_[kSize];
    uint8_t pos_;
    bool is_full_;
};

// Example usage
#include <iostream>
#include <numeric>

int main() {
    V8RingBuffer<int, 5> rb;
    
    // Push elements
    for (int i = 1; i <= 7; i++) {
        rb.Push(i);
        std::cout << "Size: " << (int)rb.Size() << std::endl;
    }
    
    // Reduce (sum)
    int sum = rb.Reduce([](int acc, int val) { return acc + val; }, 0);
    std::cout << "Sum: " << sum << std::endl;
    
    // Access elements
    for (uint8_t i = 0; i < rb.Size(); i++) {
        std::cout << "[" << (int)i << "] = " << rb[i] << std::endl;
    }
    
    return 0;
}


