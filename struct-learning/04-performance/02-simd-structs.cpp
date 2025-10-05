/*
 * =============================================================================
 * Performance Engineering: SIMD Structs
 * Vector friendly layouts and simple AVX like loops
 * =============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <immintrin.h>

struct alignas(32) Vec8f { float v[8]; };

void add_vec8f(const Vec8f* a, const Vec8f* b, Vec8f* out, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        __m256 va = _mm256_load_ps(a[i].v);
        __m256 vb = _mm256_load_ps(b[i].v);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_store_ps(out[i].v, vc);
    }
}

int main() {
    try {
        std::cout << "\n=== SIMD STRUCTS ===" << std::endl;
        const size_t N = 4;
        std::vector<Vec8f> a(N), b(N), c(N);
        for (size_t i = 0; i < N; ++i) for (int j = 0; j < 8; ++j) { a[i].v[j] = (float)j; b[i].v[j] = 1.0f; }
        add_vec8f(a.data(), b.data(), c.data(), N);
        std::cout << "c[0][0]=" << c[0].v[0] << " c[0][7]=" << c[0].v[7] << std::endl;
        std::cout << "\n=== SIMD STRUCTS COMPLETED SUCCESSFULLY ===" << std::endl;
    } catch (...) { std::cerr << "error" << std::endl; return 1; }
    return 0;
}
