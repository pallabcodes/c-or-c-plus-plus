/*
 * =============================================================================
 * System Programming: Advanced Embedded Structs - Real-Time Systems
 * Production-Grade Embedded Data Structures for Top-Tier Companies
 * =============================================================================
 *
 * This file demonstrates advanced embedded techniques including:
 * - Sensor fusion structures
 * - Real-time control loops
 * - Memory-mapped I/O structures
 * - Interrupt handling structures
 * - Watchdog and safety structures
 * - CAN bus message structures
 * - RTOS task structures
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
#include <bitset>
#include <array>
#include <cmath>

// =============================================================================
// SENSOR FUSION STRUCTURES
// =============================================================================

struct __attribute__((packed)) SensorSample {
    uint32_t ts_ms;
    int16_t accel_x;      // Accelerometer X (m/s² * 100)
    int16_t accel_y;      // Accelerometer Y
    int16_t accel_z;      // Accelerometer Z
    int16_t gyro_x;       // Gyroscope X (rad/s * 1000)
    int16_t gyro_y;       // Gyroscope Y
    int16_t gyro_z;       // Gyroscope Z
    int16_t mag_x;        // Magnetometer X (Gauss * 100)
    int16_t mag_y;        // Magnetometer Y
    int16_t mag_z;        // Magnetometer Z
    int16_t temp_c_x100;  // Temperature (°C * 100)
    uint8_t sensor_status; // Sensor health flags
    uint8_t reserved;
};

struct alignas(4) FusedOrientation {
    float roll;   // Roll angle (radians)
    float pitch;  // Pitch angle (radians)
    float yaw;    // Yaw angle (radians)
    float quaternion[4];  // Quaternion representation
    float confidence;     // Fusion confidence (0.0-1.0)
    uint32_t fusion_ts_ms;
};

// Kalman filter state for sensor fusion
struct alignas(8) KalmanState {
    float state[6];       // [x, y, z, vx, vy, vz]
    float covariance[36]; // 6x6 covariance matrix (packed)
    float process_noise;
    float measurement_noise;
    uint32_t update_count;
};

// =============================================================================
// REAL-TIME CONTROL LOOPS
// =============================================================================

struct __attribute__((packed)) ControlCmd {
    uint8_t motor_duty;    // PWM duty cycle (0-100)
    int8_t target_temp;    // Target temperature (°C)
    uint8_t flags;         // Control flags
    uint8_t safety_level;  // Safety level (0-3)
    uint16_t timeout_ms;   // Command timeout
    uint16_t checksum;     // CRC16 checksum
};

struct alignas(4) PIDController {
    float kp;              // Proportional gain
    float ki;              // Integral gain
    float kd;              // Derivative gain
    float setpoint;        // Desired value
    float integral;        // Integral accumulator
    float prev_error;      // Previous error for derivative
    float output_min;      // Output saturation min
    float output_max;      // Output saturation max
    uint32_t sample_count;
    
    float compute(float input, float dt) {
        float error = setpoint - input;
        integral += error * dt;
        
        // Anti-windup
        if (integral > output_max) integral = output_max;
        if (integral < output_min) integral = output_min;
        
        float derivative = (error - prev_error) / dt;
        prev_error = error;
        
        float output = kp * error + ki * integral + kd * derivative;
        
        // Saturation
        if (output > output_max) output = output_max;
        if (output < output_min) output = output_min;
        
        sample_count++;
        return output;
    }
};

// =============================================================================
// MEMORY-MAPPED I/O STRUCTURES
// =============================================================================

struct __attribute__((packed)) GPIO_Registers {
    volatile uint32_t MODER;    // Mode register
    volatile uint32_t OTYPER;   // Output type register
    volatile uint32_t OSPEEDR;  // Output speed register
    volatile uint32_t PUPDR;    // Pull-up/pull-down register
    volatile uint32_t IDR;      // Input data register
    volatile uint32_t ODR;      // Output data register
    volatile uint32_t BSRR;     // Bit set/reset register
    volatile uint32_t LCKR;     // Configuration lock register
    volatile uint32_t AFR[2];   // Alternate function registers
};

struct __attribute__((packed)) Timer_Registers {
    volatile uint32_t CR1;      // Control register 1
    volatile uint32_t CR2;      // Control register 2
    volatile uint32_t SMCR;     // Slave mode control register
    volatile uint32_t DIER;     // DMA/interrupt enable register
    volatile uint32_t SR;       // Status register
    volatile uint32_t EGR;      // Event generation register
    volatile uint32_t CCMR1;    // Capture/compare mode register 1
    volatile uint32_t CCMR2;    // Capture/compare mode register 2
    volatile uint32_t CCER;     // Capture/compare enable register
    volatile uint32_t CNT;      // Counter register
    volatile uint32_t PSC;      // Prescaler register
    volatile uint32_t ARR;      // Auto-reload register
    volatile uint32_t CCR[4];   // Capture/compare registers
};

// =============================================================================
// INTERRUPT HANDLING STRUCTURES
// =============================================================================

struct alignas(8) InterruptContext {
    uint32_t r0, r1, r2, r3, r4, r5, r6, r7;
    uint32_t r8, r9, r10, r11, r12;
    uint32_t sp;        // Stack pointer
    uint32_t lr;        // Link register
    uint32_t pc;        // Program counter
    uint32_t psr;       // Program status register
    uint32_t reserved;
};

struct InterruptHandler {
    uint8_t irq_number;
    void (*handler)(void*);
    void* context;
    uint32_t priority;
    bool is_enabled;
    uint32_t call_count;
};

// Interrupt vector table
struct alignas(256) InterruptVectorTable {
    void* handlers[256];  // Function pointers
    uint32_t priorities[256];
};

// =============================================================================
// WATCHDOG AND SAFETY STRUCTURES
// =============================================================================

struct alignas(4) WatchdogConfig {
    uint32_t timeout_ms;
    uint32_t window_ms;      // Window watchdog
    bool window_enabled;
    bool debug_stop;         // Stop in debug mode
    uint32_t reload_value;
};

struct SafetyState {
    uint8_t level;           // Safety level (0-4, SIL)
    uint32_t error_count;
    uint32_t last_error_ts_ms;
    uint32_t safety_flags;   // Safety status flags
    bool emergency_stop;
    bool watchdog_ok;
    uint16_t crc;            // Safety checksum
};

// =============================================================================
// CAN BUS MESSAGE STRUCTURES
// =============================================================================

struct __attribute__((packed)) CANMessage {
    uint32_t id;             // CAN ID (11 or 29 bit)
    uint8_t dlc;             // Data length code (0-8)
    uint8_t rtr;             // Remote transmission request
    uint8_t ide;             // Extended ID flag
    uint8_t data[8];         // Payload
    uint64_t timestamp_us;   // Timestamp in microseconds
    uint16_t crc;            // CRC checksum
};

struct alignas(8) CANFilter {
    uint32_t filter_id;
    uint32_t filter_mask;
    uint8_t filter_bank;
    bool is_enabled;
};

// CAN bus controller registers (simplified)
struct __attribute__((packed)) CAN_Registers {
    volatile uint32_t MCR;      // Master control register
    volatile uint32_t MSR;      // Master status register
    volatile uint32_t TSR;      // Transmit status register
    volatile uint32_t RF0R;     // Receive FIFO 0 register
    volatile uint32_t RF1R;     // Receive FIFO 1 register
    volatile uint32_t IER;      // Interrupt enable register
    volatile uint32_t ESR;      // Error status register
    volatile uint32_t BTR;      // Bit timing register
    // ... more registers
};

// =============================================================================
// RTOS TASK STRUCTURES
// =============================================================================

enum class TaskState : uint8_t {
    READY = 0,
    RUNNING = 1,
    BLOCKED = 2,
    SUSPENDED = 3,
    DELETED = 4
};

enum class TaskPriority : uint8_t {
    IDLE = 0,
    LOW = 1,
    NORMAL = 2,
    HIGH = 3,
    CRITICAL = 4
};

struct alignas(8) TaskControlBlock {
    void* stack_ptr;
    void* stack_base;
    uint32_t stack_size;
    TaskState state;
    TaskPriority priority;
    uint32_t priority_inherited;
    uint32_t time_slice_remaining;
    uint32_t wake_time_ms;
    void* wait_object;        // Semaphore, queue, etc.
    uint32_t task_id;
    char name[16];
    uint32_t run_count;
    uint64_t cpu_time_us;
    uint32_t context[16];     // Saved CPU context
};

// =============================================================================
// DEMONSTRATION FUNCTIONS
// =============================================================================

void demonstrate_sensor_fusion() {
    std::cout << "\n=== SENSOR FUSION ===" << std::endl;
    
    SensorSample sample{};
    sample.ts_ms = 1000;
    sample.accel_x = 980;  // 9.8 m/s²
    sample.accel_y = 0;
    sample.accel_z = 0;
    sample.gyro_x = 0;
    sample.gyro_y = 0;
    sample.gyro_z = 0;
    sample.temp_c_x100 = 2534;  // 25.34°C
    sample.sensor_status = 0xFF;  // All sensors OK
    
    std::cout << "Timestamp: " << sample.ts_ms << " ms" << std::endl;
    std::cout << "Acceleration X: " << (sample.accel_x / 100.0f) << " m/s²" << std::endl;
    std::cout << "Temperature: " << (sample.temp_c_x100 / 100.0f) << " °C" << std::endl;
    std::cout << "Sensor sample size: " << sizeof(SensorSample) << " bytes" << std::endl;
    
    FusedOrientation orientation{};
    orientation.roll = 0.1f;
    orientation.pitch = 0.05f;
    orientation.yaw = 1.57f;  // 90 degrees
    orientation.confidence = 0.95f;
    
    std::cout << "Fused orientation - Roll: " << orientation.roll 
              << ", Pitch: " << orientation.pitch 
              << ", Yaw: " << orientation.yaw << std::endl;
    std::cout << "Confidence: " << orientation.confidence << std::endl;
}

void demonstrate_control_loops() {
    std::cout << "\n=== REAL-TIME CONTROL LOOPS ===" << std::endl;
    
    PIDController pid{};
    pid.kp = 2.0f;
    pid.ki = 0.5f;
    pid.kd = 0.1f;
    pid.setpoint = 25.0f;  // Target 25°C
    pid.output_min = 0.0f;
    pid.output_max = 100.0f;
    
    float current_temp = 20.0f;
    float dt = 0.1f;  // 100ms control loop
    
    for (int i = 0; i < 5; ++i) {
        float output = pid.compute(current_temp, dt);
        current_temp += output * 0.01f;  // Simulate heating
        std::cout << "Iteration " << i << ": temp=" << current_temp 
                  << "°C, output=" << output << "%" << std::endl;
    }
    
    ControlCmd cmd{};
    cmd.motor_duty = 75;
    cmd.target_temp = 25;
    cmd.flags = 0b00000101;
    cmd.safety_level = 2;
    cmd.timeout_ms = 1000;
    cmd.checksum = 0x1234;  // Simplified
    
    std::cout << "Control command - Duty: " << (int)cmd.motor_duty 
              << "%, Target: " << (int)cmd.target_temp << "°C" << std::endl;
    std::cout << "Control command size: " << sizeof(ControlCmd) << " bytes" << std::endl;
}

void demonstrate_memory_mapped_io() {
    std::cout << "\n=== MEMORY-MAPPED I/O ===" << std::endl;
    
    // Simulated GPIO register (in real system, this would be mmap'd)
    GPIO_Registers gpio{};
    gpio.MODER = 0x55555555;  // Set pins 0-15 to output mode
    gpio.OTYPER = 0x0000;     // Push-pull output
    gpio.ODR = 0x00FF;        // Set pins 0-7 high
    
    std::cout << "GPIO MODER: 0x" << std::hex << gpio.MODER << std::dec << std::endl;
    std::cout << "GPIO ODR: 0x" << std::hex << gpio.ODR << std::dec << std::endl;
    std::cout << "GPIO registers size: " << sizeof(GPIO_Registers) << " bytes" << std::endl;
}

void demonstrate_interrupt_handling() {
    std::cout << "\n=== INTERRUPT HANDLING ===" << std::endl;
    
    InterruptHandler handler{};
    handler.irq_number = 42;
    handler.priority = 5;
    handler.is_enabled = true;
    handler.call_count = 0;
    
    std::cout << "IRQ number: " << (int)handler.irq_number << std::endl;
    std::cout << "Priority: " << handler.priority << std::endl;
    std::cout << "Enabled: " << handler.is_enabled << std::endl;
    
    InterruptContext ctx{};
    ctx.r0 = 0x12345678;
    ctx.sp = 0x20001000;
    ctx.pc = 0x08000000;
    
    std::cout << "Interrupt context size: " << sizeof(InterruptContext) << " bytes" << std::endl;
}

void demonstrate_watchdog_safety() {
    std::cout << "\n=== WATCHDOG AND SAFETY ===" << std::endl;
    
    WatchdogConfig wdt{};
    wdt.timeout_ms = 1000;
    wdt.window_ms = 100;
    wdt.window_enabled = true;
    wdt.debug_stop = false;
    wdt.reload_value = 1000;
    
    std::cout << "Watchdog timeout: " << wdt.timeout_ms << " ms" << std::endl;
    std::cout << "Window enabled: " << wdt.window_enabled << std::endl;
    
    SafetyState safety{};
    safety.level = 2;  // SIL 2
    safety.error_count = 0;
    safety.emergency_stop = false;
    safety.watchdog_ok = true;
    safety.safety_flags = 0xFFFF;
    
    std::cout << "Safety level: SIL " << (int)safety.level << std::endl;
    std::cout << "Watchdog OK: " << safety.watchdog_ok << std::endl;
}

void demonstrate_can_bus() {
    std::cout << "\n=== CAN BUS MESSAGES ===" << std::endl;
    
    CANMessage msg{};
    msg.id = 0x123;
    msg.dlc = 8;
    msg.rtr = 0;
    msg.ide = 0;  // Standard ID
    msg.data[0] = 0x01;
    msg.data[1] = 0x02;
    msg.data[2] = 0x03;
    msg.data[3] = 0x04;
    msg.timestamp_us = 1700000000ULL;
    msg.crc = 0xABCD;
    
    std::cout << "CAN ID: 0x" << std::hex << msg.id << std::dec << std::endl;
    std::cout << "DLC: " << (int)msg.dlc << std::endl;
    std::cout << "Data: ";
    for (int i = 0; i < msg.dlc; ++i) {
        std::cout << "0x" << std::hex << (int)msg.data[i] << " " << std::dec;
    }
    std::cout << std::endl;
    std::cout << "CAN message size: " << sizeof(CANMessage) << " bytes" << std::endl;
}

void demonstrate_rtos_tasks() {
    std::cout << "\n=== RTOS TASK STRUCTURES ===" << std::endl;
    
    TaskControlBlock task{};
    task.stack_size = 1024;
    task.state = TaskState::READY;
    task.priority = TaskPriority::HIGH;
    task.time_slice_remaining = 10;  // 10ms time slice
    task.task_id = 1;
    std::strcpy(task.name, "sensor_task");
    task.run_count = 100;
    task.cpu_time_us = 5000;
    
    std::cout << "Task name: " << task.name << std::endl;
    std::cout << "Task ID: " << task.task_id << std::endl;
    std::cout << "State: " << static_cast<int>(task.state) << " (READY)" << std::endl;
    std::cout << "Priority: " << static_cast<int>(task.priority) << " (HIGH)" << std::endl;
    std::cout << "Run count: " << task.run_count << std::endl;
    std::cout << "CPU time: " << task.cpu_time_us << " us" << std::endl;
    std::cout << "TCB size: " << sizeof(TaskControlBlock) << " bytes" << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "=== GOD-MODDED ADVANCED EMBEDDED STRUCTS ===" << std::endl;
    std::cout << "Demonstrating production-grade embedded data structures" << std::endl;
    
    try {
        demonstrate_sensor_fusion();
        demonstrate_control_loops();
        demonstrate_memory_mapped_io();
        demonstrate_interrupt_handling();
        demonstrate_watchdog_safety();
        demonstrate_can_bus();
        demonstrate_rtos_tasks();
        
        std::cout << "\n=== EMBEDDED STRUCTS COMPLETED SUCCESSFULLY ===" << std::endl;
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
 *   g++ -std=c++17 -O2 -Wall -Wextra -o embedded_structs 05-embedded-structs.cpp
 *   clang++ -std=c++17 -O2 -Wall -Wextra -o embedded_structs 05-embedded-structs.cpp
 *
 * Advanced embedded techniques:
 *   - Sensor fusion structures
 *   - Real-time control loops (PID)
 *   - Memory-mapped I/O structures
 *   - Interrupt handling structures
 *   - Watchdog and safety structures
 *   - CAN bus message structures
 *   - RTOS task structures
 */
