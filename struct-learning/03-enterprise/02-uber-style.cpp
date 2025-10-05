/*
 * =============================================================================
 * Enterprise Patterns: Uber Style Structs
 * Real time dispatch, geospatial, and pricing friendly layouts
 * =============================================================================
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <cmath>

struct GeoPoint { float lat; float lng; };

struct alignas(32) DriverState {
    uint32_t driver_id;
    GeoPoint location;
    uint16_t status;      // bit flags
    uint16_t vehicle;     // type id
    uint32_t last_update; // epoch seconds
    float surge_factor;   // cached pricing multiplier
    float eta_min;        // cached eta
};

struct alignas(32) RideSearchKey {
    GeoPoint pickup;
    float max_radius_km;
    uint16_t vehicle_mask;
    uint16_t priority;
};

static inline float haversine_km(const GeoPoint& a, const GeoPoint& b) {
    constexpr float R = 6371.0f;
    float dlat = (b.lat - a.lat) * 0.01745329252f;
    float dlon = (b.lng - a.lng) * 0.01745329252f;
    float sa = std::sin(dlat * 0.5f), sb = std::sin(dlon * 0.5f);
    float c = 2.0f * std::asin(std::sqrt(sa*sa + std::cos(a.lat*0.01745329252f)*std::cos(b.lat*0.01745329252f)*sb*sb));
    return R * c;
}

void demo_uber_patterns() {
    std::cout << "\n=== ENTERPRISE: UBER STYLE ===" << std::endl;
    DriverState d{12345, GeoPoint{37.7749f, -122.4194f}, 1u, 2u, 1700000000u, 1.25f, 3.5f};
    RideSearchKey k{GeoPoint{37.7800f, -122.4200f}, 2.0f, 0xFFFFu, 1u};

    float dist = haversine_km(d.location, k.pickup);
    std::cout << "driver=" << d.driver_id << " dist_km=" << dist << " surge=" << d.surge_factor << std::endl;
}

int main() {
    try {
        demo_uber_patterns();
        std::cout << "\n=== UBER STYLE COMPLETED SUCCESSFULLY ===" << std::endl;
    } catch (...) { std::cerr << "error" << std::endl; return 1; }
    return 0;
}
