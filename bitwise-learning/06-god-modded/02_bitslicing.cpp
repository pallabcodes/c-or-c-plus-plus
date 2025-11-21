/*
 * God-Modded: Bitslicing boolean SIMD
 * 
 * Demonstrates bitslicing technique where each bit position
 * represents a different boolean value, enabling parallel
 * boolean operations via SIMD.
 */
#include <immintrin.h>
#include <iostream>
#include <cstdint>

#ifndef __AVX2__
#warning "AVX2 not available, code may not compile or run correctly"
#endif

int main() {
    __m256i a = _mm256_set1_epi8(static_cast<char>(0xAA));
    __m256i b = _mm256_set1_epi8(static_cast<char>(0xCC));
    __m256i andv = _mm256_and_si256(a, b);
    alignas(32) unsigned char out[32];
    _mm256_store_si256(reinterpret_cast<__m256i*>(out), andv);
    std::cout << std::hex << static_cast<int>(out[0]) << std::endl;
    return 0;
}

