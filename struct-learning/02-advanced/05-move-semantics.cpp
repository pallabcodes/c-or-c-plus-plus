/*
 * =============================================================================
 * Advanced Struct Techniques: Move Semantics - Advanced Performance Patterns
 * Production-Grade Move Semantics for Top-Tier Companies
 * =============================================================================
 *
 * This file demonstrates advanced move semantics techniques including:
 * - Move-only types
 * - noexcept optimization
 * - Move-aware containers
 * - Perfect forwarding with move
 * - RAII with move semantics
 * - Move assignment optimization
 * - Small buffer optimization (SBO)
 *
 * Author: System Engineering Team
 * Version: 2.0
 * Last Modified: 2024-01-15
 *
 * =============================================================================
 */

#include <iostream>
#include <cstring>
#include <utility>
#include <vector>
#include <memory>
#include <type_traits>
#include <algorithm>

// =============================================================================
// MOVE-ONLY TYPE (GOOGLE-STYLE)
// =============================================================================

class MoveOnlyBuffer {
private:
    char* data_;
    size_t size_;
    
public:
    MoveOnlyBuffer() : data_(nullptr), size_(0) {}
    
    explicit MoveOnlyBuffer(size_t n) : data_(static_cast<char*>(std::malloc(n))), size_(n) {
        if (!data_) throw std::bad_alloc();
        std::memset(data_, 0, n);
    }
    
    ~MoveOnlyBuffer() {
        std::free(data_);
    }
    
    // Delete copy operations
    MoveOnlyBuffer(const MoveOnlyBuffer&) = delete;
    MoveOnlyBuffer& operator=(const MoveOnlyBuffer&) = delete;
    
    // Move constructor (noexcept for optimization)
    MoveOnlyBuffer(MoveOnlyBuffer&& other) noexcept 
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }
    
    // Move assignment (noexcept for optimization)
    MoveOnlyBuffer& operator=(MoveOnlyBuffer&& other) noexcept {
        if (this != &other) {
            std::free(data_);
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    char* data() { return data_; }
    const char* data() const { return data_; }
    size_t size() const { return size_; }
    
    bool empty() const { return data_ == nullptr; }
};

// =============================================================================
// NOEXCEPT OPTIMIZATION (UBER-STYLE)
// =============================================================================

template<typename T>
class NoexceptMovable {
private:
    T* ptr_;
    
public:
    NoexceptMovable() : ptr_(nullptr) {}
    
    explicit NoexceptMovable(const T& value) : ptr_(new T(value)) {}
    
    ~NoexceptMovable() {
        delete ptr_;
    }
    
    // Copy operations
    NoexceptMovable(const NoexceptMovable& other) : ptr_(other.ptr_ ? new T(*other.ptr_) : nullptr) {}
    
    NoexceptMovable& operator=(const NoexceptMovable& other) {
        if (this != &other) {
            delete ptr_;
            ptr_ = other.ptr_ ? new T(*other.ptr_) : nullptr;
        }
        return *this;
    }
    
    // Move operations with noexcept
    NoexceptMovable(NoexceptMovable&& other) noexcept 
        : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }
    
    NoexceptMovable& operator=(NoexceptMovable&& other) noexcept {
        if (this != &other) {
            delete ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }
    
    T& operator*() { return *ptr_; }
    const T& operator*() const { return *ptr_; }
    T* operator->() { return ptr_; }
    const T* operator->() const { return ptr_; }
    
    bool empty() const { return ptr_ == nullptr; }
};

// =============================================================================
// SMALL BUFFER OPTIMIZATION (BLOOMBERG-STYLE)
// =============================================================================

template<typename T, size_t SmallSize = 16>
class SmallBufferOptimized {
private:
    union {
        T* large_ptr_;
        char small_buffer_[SmallSize];
    };
    size_t size_;
    bool is_small_;
    
    void destroy() {
        if (is_small_) {
            T* ptr = reinterpret_cast<T*>(small_buffer_);
            ptr->~T();
        } else {
            delete large_ptr_;
        }
    }
    
public:
    SmallBufferOptimized() : size_(0), is_small_(true) {}
    
    explicit SmallBufferOptimized(const T& value) {
        if (sizeof(T) <= SmallSize) {
            new(small_buffer_) T(value);
            is_small_ = true;
        } else {
            large_ptr_ = new T(value);
            is_small_ = false;
        }
        size_ = 1;
    }
    
    ~SmallBufferOptimized() {
        if (size_ > 0) {
            destroy();
        }
    }
    
    // Move constructor
    SmallBufferOptimized(SmallBufferOptimized&& other) noexcept 
        : size_(other.size_), is_small_(other.is_small_) {
        if (is_small_) {
            new(small_buffer_) T(std::move(*reinterpret_cast<T*>(other.small_buffer_)));
            reinterpret_cast<T*>(other.small_buffer_)->~T();
        } else {
            large_ptr_ = other.large_ptr_;
            other.large_ptr_ = nullptr;
        }
        other.size_ = 0;
    }
    
    // Move assignment
    SmallBufferOptimized& operator=(SmallBufferOptimized&& other) noexcept {
        if (this != &other) {
            if (size_ > 0) {
                destroy();
            }
            
            size_ = other.size_;
            is_small_ = other.is_small_;
            
            if (is_small_) {
                new(small_buffer_) T(std::move(*reinterpret_cast<T*>(other.small_buffer_)));
                reinterpret_cast<T*>(other.small_buffer_)->~T();
            } else {
                large_ptr_ = other.large_ptr_;
                other.large_ptr_ = nullptr;
            }
            
            other.size_ = 0;
        }
        return *this;
    }
    
    T& get() {
        return is_small_ ? *reinterpret_cast<T*>(small_buffer_) : *large_ptr_;
    }
    
    const T& get() const {
        return is_small_ ? *reinterpret_cast<const T*>(small_buffer_) : *large_ptr_;
    }
    
    bool is_small() const { return is_small_; }
};

// =============================================================================
// MOVE-AWARE CONTAINER (AMAZON-STYLE)
// =============================================================================

template<typename T>
class MoveAwareVector {
private:
    std::vector<T> data_;
    
public:
    template<typename U>
    void emplace_back(U&& value) {
        data_.emplace_back(std::forward<U>(value));
    }
    
    template<typename... Args>
    void emplace_back(Args&&... args) {
        data_.emplace_back(std::forward<Args>(args)...);
    }
    
    void push_back(const T& value) {
        data_.push_back(value);
    }
    
    void push_back(T&& value) {
        data_.push_back(std::move(value));
    }
    
    size_t size() const { return data_.size(); }
    
    T& operator[](size_t index) { return data_[index]; }
    const T& operator[](size_t index) const { return data_[index]; }
};

// =============================================================================
// RAII WITH MOVE SEMANTICS (PAYPAL-STYLE)
// =============================================================================

class RAIIResource {
private:
    int* resource_;
    
public:
    explicit RAIIResource(int value) : resource_(new int(value)) {}
    
    ~RAIIResource() {
        delete resource_;
    }
    
    // Delete copy
    RAIIResource(const RAIIResource&) = delete;
    RAIIResource& operator=(const RAIIResource&) = delete;
    
    // Move constructor
    RAIIResource(RAIIResource&& other) noexcept 
        : resource_(other.resource_) {
        other.resource_ = nullptr;
    }
    
    // Move assignment
    RAIIResource& operator=(RAIIResource&& other) noexcept {
        if (this != &other) {
            delete resource_;
            resource_ = other.resource_;
            other.resource_ = nullptr;
        }
        return *this;
    }
    
    int& get() { return *resource_; }
    const int& get() const { return *resource_; }
    
    bool valid() const { return resource_ != nullptr; }
};

// =============================================================================
// PERFECT FORWARDING WITH MOVE (STRIPE-STYLE)
// =============================================================================

template<typename T>
class ForwardingWrapper {
private:
    T value_;
    
public:
    template<typename U>
    ForwardingWrapper(U&& u) : value_(std::forward<U>(u)) {}
    
    template<typename U>
    ForwardingWrapper& operator=(U&& u) {
        value_ = std::forward<U>(u);
        return *this;
    }
    
    // Move constructor
    ForwardingWrapper(ForwardingWrapper&&) noexcept = default;
    
    // Move assignment
    ForwardingWrapper& operator=(ForwardingWrapper&&) noexcept = default;
    
    T& get() { return value_; }
    const T& get() const { return value_; }
};

// =============================================================================
// DEMONSTRATION FUNCTIONS
// =============================================================================

void demonstrate_move_only() {
    std::cout << "\n=== MOVE-ONLY TYPE ===" << std::endl;
    
    MoveOnlyBuffer buf1(128);
    std::strcpy(buf1.data(), "Hello, Move!");
    
    MoveOnlyBuffer buf2 = std::move(buf1);
    
    std::cout << "buf2 data: " << buf2.data() << std::endl;
    std::cout << "buf1 empty after move: " << buf1.empty() << std::endl;
}

void demonstrate_noexcept_optimization() {
    std::cout << "\n=== NOEXCEPT OPTIMIZATION ===" << std::endl;
    
    NoexceptMovable<int> nm1(42);
    NoexceptMovable<int> nm2 = std::move(nm1);
    
    std::cout << "nm2 value: " << *nm2 << std::endl;
    std::cout << "nm1 empty after move: " << nm1.empty() << std::endl;
    std::cout << "Move is noexcept: " << std::is_nothrow_move_constructible_v<NoexceptMovable<int>> << std::endl;
}

void demonstrate_small_buffer_optimization() {
    std::cout << "\n=== SMALL BUFFER OPTIMIZATION ===" << std::endl;
    
    SmallBufferOptimized<int> sbo1(100);
    SmallBufferOptimized<int> sbo2 = std::move(sbo1);
    
    std::cout << "sbo2 value: " << sbo2.get() << std::endl;
    std::cout << "sbo2 is small: " << sbo2.is_small() << std::endl;
}

void demonstrate_move_aware_container() {
    std::cout << "\n=== MOVE-AWARE CONTAINER ===" << std::endl;
    
    MoveAwareVector<std::string> vec;
    
    std::string str1 = "Hello";
    vec.push_back(str1);  // Copy
    vec.push_back(std::move(str1));  // Move
    
    vec.emplace_back("World");  // Perfect forwarding
    
    std::cout << "Vector size: " << vec.size() << std::endl;
    std::cout << "vec[0]: " << vec[0] << std::endl;
    std::cout << "vec[1]: " << vec[1] << std::endl;
    std::cout << "vec[2]: " << vec[2] << std::endl;
}

void demonstrate_raii_with_move() {
    std::cout << "\n=== RAII WITH MOVE SEMANTICS ===" << std::endl;
    
    RAIIResource res1(42);
    RAIIResource res2 = std::move(res1);
    
    std::cout << "res2 value: " << res2.get() << std::endl;
    std::cout << "res1 valid after move: " << res1.valid() << std::endl;
}

void demonstrate_perfect_forwarding_move() {
    std::cout << "\n=== PERFECT FORWARDING WITH MOVE ===" << std::endl;
    
    std::string str = "test";
    ForwardingWrapper<std::string> wrapper1(std::move(str));
    ForwardingWrapper<std::string> wrapper2 = std::move(wrapper1);
    
    std::cout << "wrapper2: " << wrapper2.get() << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "=== GOD-MODDED ADVANCED MOVE SEMANTICS ===" << std::endl;
    std::cout << "Demonstrating production-grade move semantics techniques" << std::endl;
    
    try {
        demonstrate_move_only();
        demonstrate_noexcept_optimization();
        demonstrate_small_buffer_optimization();
        demonstrate_move_aware_container();
        demonstrate_raii_with_move();
        demonstrate_perfect_forwarding_move();
        
        std::cout << "\n=== MOVE SEMANTICS COMPLETED SUCCESSFULLY ===" << std::endl;
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
 *   g++ -std=c++17 -O2 -Wall -Wextra -o move_semantics 05-move-semantics.cpp
 *   clang++ -std=c++17 -O2 -Wall -Wextra -o move_semantics 05-move-semantics.cpp
 *
 * Advanced move semantics techniques:
 *   - Move-only types
 *   - noexcept optimization
 *   - Small buffer optimization
 *   - Move-aware containers
 *   - RAII with move semantics
 *   - Perfect forwarding with move
 */
