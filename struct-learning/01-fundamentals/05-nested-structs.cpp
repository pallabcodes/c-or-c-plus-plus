/*
 * =============================================================================
 * Struct Fundamentals: Nested Structs - Complex Data Composition
 * Production-Grade Nested Structures for Top-Tier Companies
 * =============================================================================
 *
 * This file demonstrates nested struct patterns for realistic systems
 * including ecommerce orders, ride dispatches, and market data snapshots.
 * Focus is on clarity, locality, and predictable memory layout.
 *
 * Author: System Engineering Team
 * Version: 1.0
 * Last Modified: 2024-01-15
 *
 * =============================================================================
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <vector>
#include <array>

// Amazon-like ecommerce order -----------------------------------------------
struct Money {
    uint32_t cents;   // store money as integer to avoid fp error
    uint16_t currency; // ISO 4217
};

struct ProductRef {
    uint64_t product_id;
    uint16_t quantity;
};

struct Address {
    char line1[64];
    char line2[64];
    char city[32];
    char state[16];
    char country[16];
    char postal[12];
};

struct OrderItem {
    ProductRef ref;
    Money unit_price;
};

struct Order {
    uint64_t order_id;
    uint32_t user_id;
    Address shipping_address;
    std::array<OrderItem, 8> items; // fixed upper bound for demo
    uint8_t item_count;
    Money subtotal;
    Money shipping;
    Money tax;
    Money total;
};

// Uber-like dispatch ---------------------------------------------------------
struct Geo {
    float lat, lng;
};

struct RiderInfo {
    uint32_t user_id;
    Geo pickup;
    Geo dropoff;
};

struct DriverInfo {
    uint32_t driver_id;
    Geo location;
    uint8_t vehicle_type;
};

struct Dispatch {
    uint64_t request_id;
    RiderInfo rider;
    DriverInfo driver;
    uint32_t assigned_time;
    uint16_t eta_seconds;
    uint8_t status; // 0 search, 1 assigned, 2 enroute, 3 complete
};

// Bloomberg-like market snapshot --------------------------------------------
struct QuoteLevel {
    uint32_t price_bp; // price in basis points
    uint32_t size;
};

struct BookSide {
    std::array<QuoteLevel, 5> levels;
};

struct MarketSnapshot {
    char symbol[12];
    uint64_t timestamp_us;
    BookSide bids;
    BookSide asks;
};

// Helpers --------------------------------------------------------------------
static inline Money money(uint32_t cents, uint16_t ccy) { return Money{cents, ccy}; }

void print_money(const Money& m) {
    std::cout << (m.cents / 100) << '.' << (m.cents % 100) << " ccy=" << m.currency;
}

void demo_order() {
    std::cout << "\n=== NESTED: ORDER ===" << std::endl;
    Order o{};
    o.order_id = 900001;
    o.user_id = 42;
    std::strcpy(o.shipping_address.line1, "1 Hacker Way");
    std::strcpy(o.shipping_address.city, "Menlo Park");
    std::strcpy(o.shipping_address.state, "CA");
    std::strcpy(o.shipping_address.country, "US");
    std::strcpy(o.shipping_address.postal, "94025");

    o.item_count = 2;
    o.items[0] = OrderItem{ ProductRef{10001, 1}, money(249999, 840) };
    o.items[1] = OrderItem{ ProductRef{20002, 2}, money(12999, 840) };

    o.subtotal = money(249999 + 2 * 12999, 840);
    o.shipping = money(999, 840);
    o.tax = money(2500, 840);
    o.total = money(o.subtotal.cents + o.shipping.cents + o.tax.cents, 840);

    std::cout << "Order " << o.order_id << " items=" << (int)o.item_count << " total="; 
    print_money(o.total); std::cout << std::endl;
}

void demo_dispatch() {
    std::cout << "\n=== NESTED: DISPATCH ===" << std::endl;
    Dispatch d{};
    d.request_id = 777888999ULL;
    d.rider = RiderInfo{12345, Geo{37.7749f, -122.4194f}, Geo{37.7849f, -122.4094f}};
    d.driver = DriverInfo{67890, Geo{37.7800f, -122.4150f}, 2};
    d.assigned_time = 1700000000U;
    d.eta_seconds = 240;
    d.status = 1;

    std::cout << "Dispatch " << d.request_id << " rider=" << d.rider.user_id
              << " driver=" << d.driver.driver_id << " eta=" << d.eta_seconds << "s" << std::endl;
}

void demo_market_snapshot() {
    std::cout << "\n=== NESTED: MARKET SNAPSHOT ===" << std::endl;
    MarketSnapshot s{};
    std::strcpy(s.symbol, "AAPL");
    s.timestamp_us = 1711111111111ULL;
    for (int i = 0; i < 5; ++i) {
        s.bids.levels[i] = QuoteLevel{1500000U - (uint32_t)(i * 5), 1000U + (uint32_t)(i * 100)};
        s.asks.levels[i] = QuoteLevel{1500000U + (uint32_t)(i * 5), 900U - (uint32_t)(i * 50)};
    }
    std::cout << "Symbol " << s.symbol << " ts=" << s.timestamp_us << std::endl;
    std::cout << "Top bid bp=" << s.bids.levels[0].price_bp << " size=" << s.bids.levels[0].size << std::endl;
    std::cout << "Top ask bp=" << s.asks.levels[0].price_bp << " size=" << s.asks.levels[0].size << std::endl;
}

int main() {
    try {
        demo_order();
        demo_dispatch();
        demo_market_snapshot();
        std::cout << "\n=== NESTED STRUCTS DEMONSTRATION COMPLETED SUCCESSFULLY ===" << std::endl;
    } catch (...) {
        std::cerr << "error" << std::endl;
        return 1;
    }
    return 0;
}
