/*
 * Enterprise: H3 Hexagonal Hierarchical Spatial Index
 * 
 * Uber's H3-style hexagonal indexing using bit manipulation
 * for efficient geospatial queries and hierarchical encoding.
 */
#include <iostream>
#include <cstdint>
#include <cassert>
#include <cmath>

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: lat in [-90, 90], lon in [-180, 180]
// Failure modes: Undefined behavior if coordinates out of range
static inline uint64_t encode_h3_style(double lat, double lon, int resolution) {
    assert(resolution >= 0 && resolution <= 15);
    assert(lat >= -90.0 && lat <= 90.0);
    assert(lon >= -180.0 && lon <= 180.0);
    
    uint64_t h = 0;
    double min_lat = -90.0, max_lat = 90.0;
    double min_lon = -180.0, max_lon = 180.0;
    
    for (int i = 0; i < resolution; ++i) {
        double mid_lat = (min_lat + max_lat) / 2.0;
        double mid_lon = (min_lon + max_lon) / 2.0;
        
        h <<= 3;
        uint64_t cell = 0;
        if (lat >= mid_lat) {
            cell |= 1;
            min_lat = mid_lat;
        } else {
            max_lat = mid_lat;
        }
        if (lon >= mid_lon) {
            cell |= 2;
            min_lon = mid_lon;
        } else {
            max_lon = mid_lon;
        }
        h |= cell;
    }
    
    return h;
}

// Thread-safety: Thread-safe (pure function)
// Ownership: None (value semantics)
// Invariants: resolution >= 0 && resolution <= 15
// Failure modes: Undefined behavior if resolution out of range
static inline void decode_h3_style(uint64_t h, int resolution, double* lat, double* lon) {
    assert(resolution >= 0 && resolution <= 15);
    assert(lat != nullptr && lon != nullptr);
    
    double min_lat = -90.0, max_lat = 90.0;
    double min_lon = -180.0, max_lon = 180.0;
    
    for (int i = resolution - 1; i >= 0; --i) {
        uint64_t cell = (h >> (3 * i)) & 7;
        double mid_lat = (min_lat + max_lat) / 2.0;
        double mid_lon = (min_lon + max_lon) / 2.0;
        
        if (cell & 1) {
            min_lat = mid_lat;
        } else {
            max_lat = mid_lat;
        }
        if (cell & 2) {
            min_lon = mid_lon;
        } else {
            max_lon = mid_lon;
        }
    }
    
    *lat = (min_lat + max_lat) / 2.0;
    *lon = (min_lon + max_lon) / 2.0;
}

int main() {
    double lat = 37.7749, lon = -122.4194;
    uint64_t h3 = encode_h3_style(lat, lon, 10);
    std::cout << std::hex << h3 << std::endl;
    
    double decoded_lat, decoded_lon;
    decode_h3_style(h3, 10, &decoded_lat, &decoded_lon);
    std::cout << decoded_lat << ", " << decoded_lon << std::endl;
    return 0;
}

