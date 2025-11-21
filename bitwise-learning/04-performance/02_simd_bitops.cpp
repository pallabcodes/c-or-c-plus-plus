/*
 * Bitwise Performance: SIMD Bit Ops
 * 
 * Demonstrates AVX2 SIMD bitwise operations for parallel
 * processing of 8 32-bit integers.
 */
#include <iostream>
#include <immintrin.h>
#include <cstdint>

#ifndef __AVX2__
#warning "AVX2 not available, code may not compile or run correctly"
#endif

int main() {
    __m256i a = _mm256_set1_epi32(0xF0F0F0F0);
    __m256i b = _mm256_set1_epi32(0x0FF00FF0);
    __m256i x = _mm256_and_si256(a, b);
    __m256i y = _mm256_or_si256(a, b);
    alignas(32) int out[8];
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(out), 
                         _mm256_xor_si256(x, y));
    std::cout << std::hex << out[0] << std::endl;
    return 0;
}
