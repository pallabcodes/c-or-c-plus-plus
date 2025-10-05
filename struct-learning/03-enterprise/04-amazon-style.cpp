/*
 * =============================================================================
 * Enterprise Patterns: Amazon Style Structs
 * Ecommerce product, catalog, and order friendly layouts
 * =============================================================================
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <array>

struct alignas(16) ProductLite {
    uint64_t id;
    uint32_t price_cents;
    uint16_t category;
    uint8_t rating;   // 0..5
    uint8_t stock;    // 0 or 1
    char title[48];
};

struct CartItem { uint64_t product_id; uint16_t qty; };

struct alignas(32) CartSnapshot {
    uint32_t user_id;
    uint32_t item_count;
    std::array<CartItem, 8> items; // fixed
    uint32_t subtotal_cents;
};

struct alignas(32) Recommendation {
    uint64_t product_id;
    float score; // model score
    uint32_t algo_id;
};

void demo_amazon_patterns() {
    std::cout << "\n=== ENTERPRISE: AMAZON STYLE ===" << std::endl;
    ProductLite p{}; p.id = 10001; p.price_cents = 249999; p.category = 1; p.rating = 5; p.stock = 1; std::strcpy(p.title, "MacBook Pro 16");
    std::cout << "product " << p.id << " $" << (p.price_cents/100.0) << " title=" << p.title << std::endl;

    CartSnapshot c{}; c.user_id = 42; c.item_count = 2; c.items[0] = CartItem{p.id, 1}; c.items[1] = CartItem{20002, 2}; c.subtotal_cents = 249999 + 2*12999;
    std::cout << "cart user=" << c.user_id << " items=" << c.item_count << " subtotal=" << (c.subtotal_cents/100.0) << std::endl;

    Recommendation r{p.id, 0.9123f, 7u};
    std::cout << "recommend id=" << r.product_id << " score=" << r.score << std::endl;
}

int main() {
    try { demo_amazon_patterns(); std::cout << "\n=== AMAZON STYLE COMPLETED SUCCESSFULLY ===" << std::endl; }
    catch (...) { std::cerr << "error" << std::endl; return 1; }
    return 0;
}
