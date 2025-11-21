/*
 * =============================================================================
 * System Programming: Advanced Hardware Structs - Memory-Mapped I/O
 * Production-Grade Hardware Interface Structures
 * =============================================================================
 *
 * This file demonstrates advanced hardware interface techniques including:
 * - Memory-mapped register access patterns
 * - DMA descriptor structures
 * - Interrupt controller structures
 * - PCI/PCIe configuration structures
 * - Hardware abstraction layers
 * - Register bit field definitions
 * - Memory barriers and ordering
 *
 * Author: System Engineering Team
 * Version: 2.0
 * Last Modified: 2024-01-15
 *
 * =============================================================================
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <atomic>
#include <array>

// =============================================================================
// MEMORY-MAPPED REGISTER STRUCTURES
// =============================================================================

// Register layout for a hypothetical device
struct __attribute__((packed)) DeviceRegs {
    volatile uint32_t ctrl;     // Control register
    volatile uint32_t status;    // Status register
    volatile uint32_t cfg;       // Configuration register
    volatile uint32_t data;      // Data port register
    volatile uint32_t interrupt; // Interrupt register
    volatile uint32_t dma_addr;  // DMA address register
    volatile uint32_t dma_len;   // DMA length register
    volatile uint32_t reserved[8]; // Reserved for future use
};

// Register bit field definitions
constexpr uint32_t CTRL_ENABLE = 1u << 0;
constexpr uint32_t CTRL_RESET  = 1u << 1;
constexpr uint32_t CTRL_INTERRUPT_EN = 1u << 2;
constexpr uint32_t CTRL_DMA_EN = 1u << 3;

constexpr uint32_t STATUS_READY = 1u << 0;
constexpr uint32_t STATUS_ERROR = 1u << 1;
constexpr uint32_t STATUS_BUSY  = 1u << 2;
constexpr uint32_t STATUS_DONE  = 1u << 3;

// Register access helpers with memory barriers
static inline void write_reg(volatile DeviceRegs* r, uint32_t offset, uint32_t val) {
    volatile uint32_t* p = reinterpret_cast<volatile uint32_t*>(
        reinterpret_cast<uintptr_t>(r) + offset);
    std::atomic_signal_fence(std::memory_order_release);
    *p = val;
    std::atomic_signal_fence(std::memory_order_seq_cst);
}

static inline uint32_t read_reg(volatile DeviceRegs* r, uint32_t offset) {
    volatile uint32_t* p = reinterpret_cast<volatile uint32_t*>(
        reinterpret_cast<uintptr_t>(r) + offset);
    std::atomic_signal_fence(std::memory_order_acquire);
    uint32_t val = *p;
    std::atomic_signal_fence(std::memory_order_seq_cst);
    return val;
}

// =============================================================================
// DMA DESCRIPTOR STRUCTURES
// =============================================================================

struct alignas(16) DMADescriptor {
    uint64_t src_addr;      // Source address
    uint64_t dst_addr;      // Destination address
    uint32_t length;        // Transfer length
    uint32_t control;       // Control flags
    
    // Control bit fields
    static constexpr uint32_t CTRL_INC_SRC = 1u << 0;
    static constexpr uint32_t CTRL_INC_DST = 1u << 1;
    static constexpr uint32_t CTRL_IRQ_EN = 1u << 2;
    static constexpr uint32_t CTRL_COMPLETE = 1u << 3;
    
    bool is_complete() const {
        return (control & CTRL_COMPLETE) != 0;
    }
    
    void set_complete() {
        control |= CTRL_COMPLETE;
    }
};

// DMA channel structure
struct alignas(64) DMAChannel {
    volatile DMADescriptor* descriptor_ring;
    volatile uint32_t head_index;
    volatile uint32_t tail_index;
    volatile uint32_t ring_size;
    volatile uint32_t status;
    
    static constexpr uint32_t STATUS_IDLE = 0;
    static constexpr uint32_t STATUS_RUNNING = 1;
    static constexpr uint32_t STATUS_ERROR = 2;
    
    bool is_idle() const {
        return status == STATUS_IDLE;
    }
    
    void start() {
        status = STATUS_RUNNING;
    }
    
    void stop() {
        status = STATUS_IDLE;
    }
};

// =============================================================================
// INTERRUPT CONTROLLER STRUCTURES
// =============================================================================

struct alignas(4) InterruptController {
    volatile uint32_t enable_reg;      // Interrupt enable register
    volatile uint32_t disable_reg;     // Interrupt disable register
    volatile uint32_t status_reg;       // Interrupt status register
    volatile uint32_t mask_reg;         // Interrupt mask register
    volatile uint32_t priority_reg[8]; // Priority registers
    volatile uint32_t ack_reg;          // Acknowledge register
    
    void enable_irq(uint8_t irq) {
        enable_reg = 1u << irq;
    }
    
    void disable_irq(uint8_t irq) {
        disable_reg = 1u << irq;
    }
    
    bool is_pending(uint8_t irq) const {
        return (status_reg & (1u << irq)) != 0;
    }
    
    void acknowledge(uint8_t irq) {
        ack_reg = 1u << irq;
    }
    
    void set_priority(uint8_t irq, uint8_t priority) {
        uint32_t reg_idx = irq / 4;
        uint32_t bit_offset = (irq % 4) * 8;
        priority_reg[reg_idx] = (priority_reg[reg_idx] & ~(0xFFu << bit_offset)) |
                                 (priority << bit_offset);
    }
};

// =============================================================================
// PCI/PCIe CONFIGURATION STRUCTURES
// =============================================================================

struct __attribute__((packed)) PCIConfigHeader {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
    uint32_t bar[6];      // Base address registers
    uint32_t cardbus_cis;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint32_t expansion_rom_base;
    uint8_t capabilities_ptr;
    uint8_t reserved[7];
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_gnt;
    uint8_t max_lat;
};

// PCIe extended capabilities
struct __attribute__((packed)) PCIeCapability {
    uint16_t cap_id;
    uint16_t next_ptr;
    uint32_t cap_data;
};

// =============================================================================
// HARDWARE ABSTRACTION LAYER (HAL)
// =============================================================================

class HardwareAbstraction {
private:
    volatile DeviceRegs* device_regs_;
    DMAChannel* dma_channel_;
    InterruptController* interrupt_ctrl_;
    
public:
    HardwareAbstraction(volatile DeviceRegs* regs, 
                       DMAChannel* dma,
                       InterruptController* int_ctrl)
        : device_regs_(regs), dma_channel_(dma), interrupt_ctrl_(int_ctrl) {}
    
    // Device control
    void enable_device() {
        uint32_t ctrl = read_reg(device_regs_, offsetof(DeviceRegs, ctrl));
        ctrl |= CTRL_ENABLE;
        write_reg(device_regs_, offsetof(DeviceRegs, ctrl), ctrl);
    }
    
    void disable_device() {
        uint32_t ctrl = read_reg(device_regs_, offsetof(DeviceRegs, ctrl));
        ctrl &= ~CTRL_ENABLE;
        write_reg(device_regs_, offsetof(DeviceRegs, ctrl), ctrl);
    }
    
    void reset_device() {
        uint32_t ctrl = read_reg(device_regs_, offsetof(DeviceRegs, ctrl));
        ctrl |= CTRL_RESET;
        write_reg(device_regs_, offsetof(DeviceRegs, ctrl), ctrl);
        
        // Wait for reset to complete
        while (read_reg(device_regs_, offsetof(DeviceRegs, status)) & STATUS_BUSY) {
            // Busy wait (in production, use proper delay)
        }
        
        ctrl &= ~CTRL_RESET;
        write_reg(device_regs_, offsetof(DeviceRegs, ctrl), ctrl);
    }
    
    bool is_device_ready() const {
        uint32_t status = read_reg(device_regs_, offsetof(DeviceRegs, status));
        return (status & STATUS_READY) != 0;
    }
    
    // DMA operations
    void setup_dma_transfer(uint64_t src, uint64_t dst, uint32_t len) {
        DMADescriptor desc{};
        desc.src_addr = src;
        desc.dst_addr = dst;
        desc.length = len;
        desc.control = DMADescriptor::CTRL_INC_SRC | 
                      DMADescriptor::CTRL_INC_DST |
                      DMADescriptor::CTRL_IRQ_EN;
        
        // Add descriptor to ring (simplified)
        if (dma_channel_) {
            dma_channel_->descriptor_ring[0] = desc;
            dma_channel_->start();
        }
    }
    
    // Interrupt handling
    void enable_interrupt(uint8_t irq) {
        if (interrupt_ctrl_) {
            interrupt_ctrl_->enable_irq(irq);
        }
        
        uint32_t ctrl = read_reg(device_regs_, offsetof(DeviceRegs, ctrl));
        ctrl |= CTRL_INTERRUPT_EN;
        write_reg(device_regs_, offsetof(DeviceRegs, ctrl), ctrl);
    }
    
    void disable_interrupt(uint8_t irq) {
        if (interrupt_ctrl_) {
            interrupt_ctrl_->disable_irq(irq);
        }
        
        uint32_t ctrl = read_reg(device_regs_, offsetof(DeviceRegs, ctrl));
        ctrl &= ~CTRL_INTERRUPT_EN;
        write_reg(device_regs_, offsetof(DeviceRegs, ctrl), ctrl);
    }
};

// =============================================================================
// REGISTER BIT FIELD STRUCTURES (SAFE ACCESS)
// =============================================================================

struct alignas(4) ControlRegister {
    uint32_t value;
    
    // Bit field accessors
    bool enable() const { return (value & CTRL_ENABLE) != 0; }
    void set_enable(bool en) {
        if (en) value |= CTRL_ENABLE;
        else value &= ~CTRL_ENABLE;
    }
    
    bool reset() const { return (value & CTRL_RESET) != 0; }
    void set_reset(bool rst) {
        if (rst) value |= CTRL_RESET;
        else value &= ~CTRL_RESET;
    }
    
    bool interrupt_enabled() const { return (value & CTRL_INTERRUPT_EN) != 0; }
    void set_interrupt_enable(bool en) {
        if (en) value |= CTRL_INTERRUPT_EN;
        else value &= ~CTRL_INTERRUPT_EN;
    }
    
    bool dma_enabled() const { return (value & CTRL_DMA_EN) != 0; }
    void set_dma_enable(bool en) {
        if (en) value |= CTRL_DMA_EN;
        else value &= ~CTRL_DMA_EN;
    }
};

// =============================================================================
// DEMONSTRATION FUNCTIONS
// =============================================================================

void demonstrate_register_access() {
    std::cout << "\n=== MEMORY-MAPPED REGISTER ACCESS ===" << std::endl;
    
    alignas(4) DeviceRegs regs{};  // Stand-in for mmap'd region
    volatile DeviceRegs* mm = &regs;
    
    write_reg(mm, offsetof(DeviceRegs, ctrl), CTRL_RESET);
    write_reg(mm, offsetof(DeviceRegs, ctrl), CTRL_ENABLE);
    write_reg(mm, offsetof(DeviceRegs, data), 0xDEADBEEF);
    
    uint32_t ctrl = read_reg(mm, offsetof(DeviceRegs, ctrl));
    uint32_t data = read_reg(mm, offsetof(DeviceRegs, data));
    
    std::cout << std::hex << "ctrl=0x" << ctrl << std::dec << std::endl;
    std::cout << std::hex << "data=0x" << data << std::dec << std::endl;
    std::cout << "Device registers size: " << sizeof(DeviceRegs) << " bytes" << std::endl;
}

void demonstrate_dma_descriptors() {
    std::cout << "\n=== DMA DESCRIPTOR STRUCTURES ===" << std::endl;
    
    DMADescriptor desc{};
    desc.src_addr = 0x10000000ULL;
    desc.dst_addr = 0x20000000ULL;
    desc.length = 4096;
    desc.control = DMADescriptor::CTRL_INC_SRC | 
                   DMADescriptor::CTRL_INC_DST |
                   DMADescriptor::CTRL_IRQ_EN;
    
    std::cout << "DMA Descriptor:" << std::endl;
    std::cout << "  Source: 0x" << std::hex << desc.src_addr << std::dec << std::endl;
    std::cout << "  Destination: 0x" << std::hex << desc.dst_addr << std::dec << std::endl;
    std::cout << "  Length: " << desc.length << " bytes" << std::endl;
    std::cout << "  Complete: " << desc.is_complete() << std::endl;
    
    desc.set_complete();
    std::cout << "  After completion: " << desc.is_complete() << std::endl;
    std::cout << "  Descriptor size: " << sizeof(DMADescriptor) << " bytes" << std::endl;
}

void demonstrate_interrupt_controller() {
    std::cout << "\n=== INTERRUPT CONTROLLER ===" << std::endl;
    
    InterruptController int_ctrl{};
    
    int_ctrl.enable_irq(5);
    int_ctrl.set_priority(5, 3);
    
    std::cout << "IRQ 5 enabled: " << (int_ctrl.status_reg != 0) << std::endl;
    std::cout << "IRQ 5 pending: " << int_ctrl.is_pending(5) << std::endl;
    
    int_ctrl.status_reg = 1u << 5;  // Simulate interrupt
    std::cout << "After interrupt: pending=" << int_ctrl.is_pending(5) << std::endl;
    
    int_ctrl.acknowledge(5);
    std::cout << "After acknowledge: pending=" << int_ctrl.is_pending(5) << std::endl;
}

void demonstrate_pci_configuration() {
    std::cout << "\n=== PCI CONFIGURATION STRUCTURES ===" << std::endl;
    
    PCIConfigHeader pci_header{};
    pci_header.vendor_id = 0x8086;  // Intel
    pci_header.device_id = 0x1234;
    pci_header.class_code = 0x02;   // Network controller
    pci_header.subclass = 0x00;     // Ethernet
    pci_header.bar[0] = 0xFEE00000; // MMIO base address
    
    std::cout << "Vendor ID: 0x" << std::hex << pci_header.vendor_id << std::dec << std::endl;
    std::cout << "Device ID: 0x" << std::hex << pci_header.device_id << std::dec << std::endl;
    std::cout << "Class: 0x" << std::hex << (int)pci_header.class_code << std::dec << std::endl;
    std::cout << "BAR0: 0x" << std::hex << pci_header.bar[0] << std::dec << std::endl;
    std::cout << "PCI header size: " << sizeof(PCIConfigHeader) << " bytes" << std::endl;
}

void demonstrate_hardware_abstraction() {
    std::cout << "\n=== HARDWARE ABSTRACTION LAYER ===" << std::endl;
    
    alignas(4) DeviceRegs regs{};
    DMAChannel dma_channel{};
    InterruptController int_ctrl{};
    
    HardwareAbstraction hal(&regs, &dma_channel, &int_ctrl);
    
    hal.reset_device();
    hal.enable_device();
    
    std::cout << "Device enabled: " << hal.is_device_ready() << std::endl;
    
    hal.setup_dma_transfer(0x10000000ULL, 0x20000000ULL, 4096);
    std::cout << "DMA transfer setup" << std::endl;
    
    hal.enable_interrupt(5);
    std::cout << "Interrupt 5 enabled" << std::endl;
}

void demonstrate_register_bit_fields() {
    std::cout << "\n=== REGISTER BIT FIELD ACCESS ===" << std::endl;
    
    ControlRegister ctrl{};
    ctrl.value = 0;
    
    ctrl.set_enable(true);
    ctrl.set_interrupt_enable(true);
    ctrl.set_dma_enable(false);
    
    std::cout << "Enable: " << ctrl.enable() << std::endl;
    std::cout << "Interrupt enabled: " << ctrl.interrupt_enabled() << std::endl;
    std::cout << "DMA enabled: " << ctrl.dma_enabled() << std::endl;
    std::cout << "Control value: 0x" << std::hex << ctrl.value << std::dec << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "=== GOD-MODDED ADVANCED HARDWARE STRUCTS ===" << std::endl;
    std::cout << "Demonstrating production-grade hardware interface structures" << std::endl;
    
    try {
        demonstrate_register_access();
        demonstrate_dma_descriptors();
        demonstrate_interrupt_controller();
        demonstrate_pci_configuration();
        demonstrate_hardware_abstraction();
        demonstrate_register_bit_fields();
        
        std::cout << "\n=== HARDWARE STRUCTS COMPLETED SUCCESSFULLY ===" << std::endl;
        std::cout << "\nNOTE: In production use volatile, memory barriers, and correct privileges." << std::endl;
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
 *   g++ -std=c++17 -O2 -Wall -Wextra -o hardware_structs 04-hardware-structs.cpp
 *   clang++ -std=c++17 -O2 -Wall -Wextra -o hardware_structs 04-hardware-structs.cpp
 *
 * Advanced hardware techniques:
 *   - Memory-mapped register access with barriers
 *   - DMA descriptor structures
 *   - Interrupt controller structures
 *   - PCI/PCIe configuration structures
 *   - Hardware abstraction layer
 *   - Register bit field accessors
 */
