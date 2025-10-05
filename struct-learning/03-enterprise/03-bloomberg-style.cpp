/*
 * =============================================================================
 * Enterprise Patterns: Bloomberg Style Structs
 * Market data feed friendly layouts and low latency calculations
 * =============================================================================
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <array>

// Packed top of book quote used in fast paths
struct __attribute__((packed)) TopOfBook {
    uint64_t ts_us;         // microsecond timestamp
    char symbol[8];         // symbol ascii padded
    uint32_t bid_bp;        // bid price in basis points
    uint32_t ask_bp;        // ask price in basis points
    uint32_t bid_size;      // bid size
    uint32_t ask_size;      // ask size
    uint8_t venue;          // venue id
    uint8_t flags;          // status flags
};

// Level two book with fixed depth for cache predictability
struct BookLevel { uint32_t px_bp; uint32_t sz; };
struct OrderBook5 {
    char symbol[8];
    uint64_t ts_us;
    std::array<BookLevel, 5> bids;
    std::array<BookLevel, 5> asks;
};

// Simple risk snapshot
struct RiskSnapshot {
    char book[8];
    uint64_t ts_us;
    double pnl;
    double delta;
    double gamma;
    double vega;
};

static inline uint32_t spread_bp(const TopOfBook& t) { return t.ask_bp - t.bid_bp; }

void demo_bloomberg_patterns() {
    std::cout << "\n=== ENTERPRISE: BLOOMBERG STYLE ===" << std::endl;
    TopOfBook t{};
    t.ts_us = 1711111111111ULL;
    std::strcpy(t.symbol, "AAPL");
    t.bid_bp = 1499950; t.ask_bp = 1500050; t.bid_size = 1200; t.ask_size = 800; t.venue = 1; t.flags = 0;
    std::cout << t.symbol << " spread bp=" << spread_bp(t) << " bid_sz=" << t.bid_size << " ask_sz=" << t.ask_size << std::endl;

    OrderBook5 ob{}; std::strcpy(ob.symbol, "AAPL"); ob.ts_us = t.ts_us;
    for (int i = 0; i < 5; ++i) { ob.bids[i] = BookLevel{1500000u - (uint32_t)(i*5), 1000u + (uint32_t)(i*50)}; }
    for (int i = 0; i < 5; ++i) { ob.asks[i] = BookLevel{1500000u + (uint32_t)(i*5), 900u - (uint32_t)(i*40)}; }
    std::cout << "top bid bp=" << ob.bids[0].px_bp << " top ask bp=" << ob.asks[0].px_bp << std::endl;

    RiskSnapshot r{}; std::strcpy(r.book, "BOOK1"); r.ts_us = t.ts_us; r.pnl = 125000.25; r.delta = 100.0; r.gamma = 2.5; r.vega = 55.0;
    std::cout << "risk pnl=" << r.pnl << " delta=" << r.delta << std::endl;
}

int main() {
    try { demo_bloomberg_patterns(); std::cout << "\n=== BLOOMBERG STYLE COMPLETED SUCCESSFULLY ===" << std::endl; }
    catch (...) { std::cerr << "error" << std::endl; return 1; }
    return 0;
}
