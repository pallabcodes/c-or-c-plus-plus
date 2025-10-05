/*
 * =============================================================================
 * System Programming: Embedded Structs
 * Tight packing, small footprints, and realtime notes
 * =============================================================================
 */

#include <iostream>
#include <cstdint>
#include <cstring>

struct __attribute__((packed)) SensorSample {
    uint32_t ts_ms;
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t temp_c_x100; // temperature * 100
};

struct __attribute__((packed)) ControlCmd {
    uint8_t motor_duty;  // 0..100
    int8_t target_temp;  // Celsius
    uint8_t flags;       // bit flags
};

void demo_embedded() {
    std::cout << "\n=== SYSTEM: EMBEDDED STRUCTS ===" << std::endl;
    SensorSample s{ 1000u, 10, -5, 2, 2534 };
    std::cout << "ts=" << s.ts_ms << " ax=" << s.accel_x << " ay=" << s.accel_y << " T=" << (s.temp_c_x100/100.0) << std::endl;

    ControlCmd c{ 75u, 25, 0b00000101 };
    std::cout << "duty=" << (int)c.motor_duty << " targetT=" << (int)c.target_temp << " flags=0b" << std::bitset<8>(c.flags) << std::endl;
}

int main() {
    try { demo_embedded(); std::cout << "\n=== EMBEDDED STRUCTS COMPLETED SUCCESSFULLY ===" << std::endl; }
    catch (...) { std::cerr << "error" << std::endl; return 1; }
    return 0;
}
