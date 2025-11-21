/*
 * System: Advanced Register Manipulation
 * 
 * Production-grade register manipulation patterns for device drivers
 * and embedded systems, including bit field operations and volatile access.
 */
#include <iostream>
#include <cstdint>
#include <cassert>

// Thread-safety: Not thread-safe (volatile access)
// Ownership: None (borrows register pointer)
// Invariants: reg must point to valid register
// Failure modes: Undefined behavior if reg is invalid
static inline void set_register_bit(volatile uint32_t* reg, unsigned bit) {
    assert(reg != nullptr);
    assert(bit < 32);
    *reg |= (1U << bit);
}

// Thread-safety: Not thread-safe (volatile access)
// Ownership: None (borrows register pointer)
// Invariants: reg must point to valid register
// Failure modes: Undefined behavior if reg is invalid
static inline void clear_register_bit(volatile uint32_t* reg, unsigned bit) {
    assert(reg != nullptr);
    assert(bit < 32);
    *reg &= ~(1U << bit);
}

// Thread-safety: Not thread-safe (volatile access)
// Ownership: None (borrows register pointer)
// Invariants: reg must point to valid register
// Failure modes: Undefined behavior if reg is invalid
static inline bool read_register_bit(volatile const uint32_t* reg, unsigned bit) {
    assert(reg != nullptr);
    assert(bit < 32);
    return (*reg & (1U << bit)) != 0;
}

// Thread-safety: Not thread-safe (volatile access)
// Ownership: None (borrows register pointer)
// Invariants: reg must point to valid register, start < end, end <= 32
// Failure modes: Undefined behavior if invariants violated
static inline uint32_t read_register_field(volatile const uint32_t* reg, 
                                           unsigned start, unsigned end) {
    assert(reg != nullptr);
    assert(start < end && end <= 32);
    uint32_t mask = ((1U << (end - start)) - 1U) << start;
    return (*reg & mask) >> start;
}

// Thread-safety: Not thread-safe (volatile access)
// Ownership: None (borrows register pointer)
// Invariants: reg must point to valid register, start < end, end <= 32
// Failure modes: Undefined behavior if invariants violated
static inline void write_register_field(volatile uint32_t* reg,
                                       unsigned start, unsigned end,
                                       uint32_t value) {
    assert(reg != nullptr);
    assert(start < end && end <= 32);
    uint32_t mask = ((1U << (end - start)) - 1U) << start;
    *reg = (*reg & ~mask) | ((value << start) & mask);
}

struct DeviceRegister {
    volatile uint32_t control;
    volatile uint32_t status;
    volatile uint32_t data;
    
    // Thread-safety: Not thread-safe (constructor)
    // Ownership: Owns register fields
    // Invariants: None
    // Failure modes: None
    DeviceRegister() : control(0), status(0), data(0) {}
    
    // Thread-safety: Not thread-safe (volatile write)
    // Ownership: Modifies owned control register
    // Invariants: None
    // Failure modes: None
    void enable() {
        set_register_bit(&control, 0);
    }
    
    // Thread-safety: Not thread-safe (volatile write)
    // Ownership: Modifies owned control register
    // Invariants: None
    // Failure modes: None
    void disable() {
        clear_register_bit(&control, 0);
    }
    
    // Thread-safety: Thread-safe (volatile read)
    // Ownership: None (read-only access)
    // Invariants: None
    // Failure modes: None
    bool is_ready() const {
        return read_register_bit(&status, 0);
    }
};

int main() {
    DeviceRegister reg;
    reg.enable();
    std::cout << reg.is_ready() << std::endl;
    return 0;
}

