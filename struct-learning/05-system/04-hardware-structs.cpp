/*
 * =============================================================================
 * System Programming: Hardware Structs
 * Memory mapped register layout example
 * =============================================================================
 */

#include <iostream>
#include <cstdint>
#include <cstring> // Required for offsetof

// Register layout for a hypothetical device
struct __attribute__((packed)) DeviceRegs {
    uint32_t ctrl;     // control
    uint32_t status;   // status
    uint32_t cfg;      // configuration
    uint32_t data;     // data port
};

constexpr uint32_t CTRL_ENABLE = 1u << 0;
constexpr uint32_t CTRL_RESET  = 1u << 1;

static inline void write_reg(volatile DeviceRegs* r, uint32_t offset, uint32_t val) {
    volatile uint32_t* p = reinterpret_cast<volatile uint32_t*>(reinterpret_cast<uintptr_t>(r) + offset);
    *p = val;
}

static inline uint32_t read_reg(volatile DeviceRegs* r, uint32_t offset) {
    volatile uint32_t* p = reinterpret_cast<volatile uint32_t*>(reinterpret_cast<uintptr_t>(r) + offset);
    return *p;
}

void demo_hw() {
    std::cout << "\n=== SYSTEM: HARDWARE STRUCTS ===" << std::endl;
    alignas(4) DeviceRegs regs{}; // stand-in for mmap'ed region
    volatile DeviceRegs* mm = &regs;

    write_reg(mm, offsetof(DeviceRegs, ctrl), CTRL_RESET);
    write_reg(mm, offsetof(DeviceRegs, ctrl), CTRL_ENABLE);
    write_reg(mm, offsetof(DeviceRegs, data), 0xDEADBEEF);

    std::cout << std::hex << "ctrl=0x" << read_reg(mm, offsetof(DeviceRegs, ctrl))
              << " data=0x" << read_reg(mm, offsetof(DeviceRegs, data)) << std::dec << std::endl;
    std::cout << "NOTE: In production use volatile, memory barriers, and correct privileges." << std::endl;
}

int main() {
    try { demo_hw(); std::cout << "\n=== HARDWARE STRUCTS COMPLETED SUCCESSFULLY ===" << std::endl; }
    catch (...) { std::cerr << "error" << std::endl; return 1; }
    return 0;
}
