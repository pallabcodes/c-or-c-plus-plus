/*
 * Bitwise System: CRC32
 */
#include <iostream>
#include <cstdint>
#include <cstring>

static uint32_t crc32_sw(const uint8_t* data, size_t len) {
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j)
            crc = (crc >> 1) ^ (0xEDB88320u & (-(int)(crc & 1)));
    }
    return crc ^ 0xFFFFFFFFu;
}

int main() {
    const char* s = "hello";
    uint32_t c = crc32_sw(reinterpret_cast<const uint8_t*>(s), std::strlen(s));
    std::cout << std::hex << c << std::endl;
    return 0;
}
