/*
 * =============================================================================
 * Advanced Struct Techniques: Move Semantics
 * Production grade move constructors and assignments for performance
 * =============================================================================
 */

#include <iostream>
#include <cstring>
#include <utility>

struct BigBuffer {
    char* data;
    size_t size;

    BigBuffer() : data(nullptr), size(0) {}
    explicit BigBuffer(size_t n) : data(static_cast<char*>(std::malloc(n))), size(n) {
        if (!data) throw std::bad_alloc();
        std::memset(data, 0, n);
    }

    ~BigBuffer() { std::free(data); }

    // non copyable (simulate expensive copy)
    BigBuffer(const BigBuffer&) = delete;
    BigBuffer& operator=(const BigBuffer&) = delete;

    // movable
    BigBuffer(BigBuffer&& other) noexcept : data(other.data), size(other.size) {
        other.data = nullptr; other.size = 0;
    }
    BigBuffer& operator=(BigBuffer&& other) noexcept {
        if (this != &other) {
            std::free(data);
            data = other.data; size = other.size;
            other.data = nullptr; other.size = 0;
        }
        return *this;
    }
};

struct Holder {
    BigBuffer buf;
    explicit Holder(size_t n) : buf(n) {}

    // move to transfer ownership
    Holder(Holder&&) noexcept = default;
    Holder& operator=(Holder&&) noexcept = default;

    // no copy
    Holder(const Holder&) = delete;
    Holder& operator=(const Holder&) = delete;
};

void demo_move_semantics() {
    std::cout << "\n=== MOVE SEMANTICS ===" << std::endl;
    Holder h1(128);
    std::strcpy(h1.buf.data, "move fast");

    Holder h2(64);
    h2 = std::move(h1);

    std::cout << "h2: " << h2.buf.data << std::endl;
    std::cout << "h1.data null after move: " << (h1.buf.data == nullptr) << std::endl;
}

int main() {
    try {
        demo_move_semantics();
        std::cout << "\n=== MOVE SEMANTICS COMPLETED SUCCESSFULLY ===" << std::endl;
    } catch (...) {
        std::cerr << "error" << std::endl; return 1;
    }
    return 0;
}
