/*
 * God-Modded: Bitslicing boolean SIMD
 */
#include <immintrin.h>
#include <iostream>

int main() {
    __m256i a = _mm256_set1_epi8((char)0xAA); // 1010...
    __m256i b = _mm256_set1_epi8((char)0xCC); // 1100...
    __m256i andv = _mm256_and_si256(a, b);
    alignas(32) unsigned char out[32];
    _mm256_store_si256((__m256i*)out, andv);
    std::cout << std::hex << (int)out[0] << std::endl;
    return 0;
}

