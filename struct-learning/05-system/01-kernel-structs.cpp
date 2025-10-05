/*
 * =============================================================================
 * System Programming: Kernel Structs
 * PCB and page table style examples
 * =============================================================================
 */

#include <iostream>
#include <cstdint>
#include <array>

struct alignas(16) Pcb {
    uint32_t pid;
    uint32_t ppid;
    uint64_t sched_ticks;
    uint64_t cpu_time_ns;
    uint8_t state;     // new, ready, running, blocked, terminated
    uint8_t priority;
    uint16_t flags;
};

struct alignas(8) PageTableEntry {
    uint64_t present : 1;
    uint64_t rw      : 1;
    uint64_t user    : 1;
    uint64_t pwt     : 1;
    uint64_t pcd     : 1;
    uint64_t a       : 1;
    uint64_t d       : 1;
    uint64_t pat     : 1;
    uint64_t g       : 1;
    uint64_t ignored : 3;
    uint64_t addr    : 40; // physical frame number
    uint64_t avail   : 11;
    uint64_t nx      : 1;
};

void demo_kernel_structs() {
    std::cout << "\n=== SYSTEM: KERNEL STRUCTS ===" << std::endl;
    Pcb p{1234u, 1u, 1000u, 50'000'000u, 1u, 10u, 0u};
    std::cout << "pid=" << p.pid << " state=" << (int)p.state << " prio=" << (int)p.priority << std::endl;

    PageTableEntry e{}; e.present = 1; e.rw = 1; e.user = 0; e.addr = 0xABCDEFu;
    std::cout << "pte present=" << e.present << " rw=" << e.rw << " addr=0x" << std::hex << e.addr << std::dec << std::endl;
}

int main() {
    try { demo_kernel_structs(); std::cout << "\n=== KERNEL STRUCTS COMPLETED SUCCESSFULLY ===" << std::endl; }
    catch (...) { std::cerr << "error" << std::endl; return 1; }
    return 0;
}
