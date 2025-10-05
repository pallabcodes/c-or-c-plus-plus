/*
 * =============================================================================
 * Performance Engineering: Cache Optimization
 * Hot cold splitting, AoS vs SoA, and prefetch friendly layouts
 * =============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <chrono>

struct alignas(64) OrderHot {
    uint64_t id;
    uint32_t user_id;
    uint32_t ts_sec;
    double amount;
};

struct OrderCold {
    char notes[64];
    uint32_t metadata[8];
};

struct OrderAoS {
    OrderHot hot;
    OrderCold cold;
};

struct SoA {
    std::vector<uint64_t> id;
    std::vector<uint32_t> user_id;
    std::vector<uint32_t> ts_sec;
    std::vector<double> amount;
};

static inline uint64_t now_us() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void bench_aos(size_t n) {
    std::vector<OrderAoS> v(n);
    for (size_t i = 0; i < n; ++i) { v[i].hot.id = i; v[i].hot.user_id = i; v[i].hot.ts_sec = (uint32_t)i; v[i].hot.amount = (double)i; }
    uint64_t t0 = now_us();
    double sum = 0.0;
    for (size_t i = 0; i < n; ++i) sum += v[i].hot.amount;
    uint64_t t1 = now_us();
    std::cout << "AoS sum=" << sum << " time_us=" << (t1 - t0) << std::endl;
}

void bench_soa(size_t n) {
    SoA s; s.id.resize(n); s.user_id.resize(n); s.ts_sec.resize(n); s.amount.resize(n);
    for (size_t i = 0; i < n; ++i) { s.id[i] = i; s.user_id[i] = i; s.ts_sec[i] = (uint32_t)i; s.amount[i] = (double)i; }
    uint64_t t0 = now_us();
    double sum = 0.0;
    for (size_t i = 0; i < n; ++i) sum += s.amount[i];
    uint64_t t1 = now_us();
    std::cout << "SoA sum=" << sum << " time_us=" << (t1 - t0) << std::endl;
}

int main() {
    const size_t N = 1'000'00; // 100k
    try {
        std::cout << "\n=== CACHE OPTIMIZATION ===" << std::endl;
        bench_aos(N);
        bench_soa(N);
        std::cout << "\n=== CACHE OPTIMIZATION COMPLETED SUCCESSFULLY ===" << std::endl;
    } catch (...) { std::cerr << "error" << std::endl; return 1; }
    return 0;
}
