/*
 * =============================================================================
 * Struct Fundamentals: Memory Layout - Deep Understanding of Struct Memory
 * Production-Grade Memory Analysis and Optimization
 * =============================================================================
 *
 * This file demonstrates memory layout analysis techniques used by top-tier
 * companies for performance optimization. It covers memory visualization,
 * alignment analysis, and cache optimization strategies.
 *
 * Author: System Engineering Team
 * Version: 1.0
 * Last Modified: 2024-01-15
 *
 * =============================================================================
 */

#include <iostream>
#include <cstring>
#include <cassert>
#include <iomanip>
#include <vector>
#include <memory>
#include <type_traits>

// =============================================================================
// MEMORY LAYOUT ANALYSIS UTILITIES
// =============================================================================

// Utility function to print memory addresses in hex
void print_memory_address(const void* ptr, const char* name) {
    std::cout << "  " << std::setw(20) << name << ": 0x" 
              << std::hex << std::setfill('0') << std::setw(16) 
              << reinterpret_cast<uintptr_t>(ptr) << std::dec << std::setfill(' ') << std::endl;
}

// Utility function to print memory content as hex dump
void print_memory_dump(const void* ptr, size_t size, const char* name) {
    std::cout << "\nMemory dump for " << name << " (" << size << " bytes):" << std::endl;
    const unsigned char* bytes = static_cast<const unsigned char*>(ptr);
    
    for (size_t i = 0; i < size; i += 16) {
        std::cout << "  " << std::hex << std::setfill('0') << std::setw(8) << i << ": ";
        
        // Print hex values
        for (size_t j = 0; j < 16 && i + j < size; j++) {
            std::cout << std::setw(2) << static_cast<int>(bytes[i + j]) << " ";
        }
        
        // Print ASCII representation
        std::cout << " |";
        for (size_t j = 0; j < 16 && i + j < size; j++) {
            char c = bytes[i + j];
            std::cout << (std::isprint(c) ? c : '.');
        }
        std::cout << "|" << std::endl;
    }
    std::cout << std::dec << std::setfill(' ') << std::endl;
}

// =============================================================================
// BASIC MEMORY LAYOUT STRUCTURES
// =============================================================================

// Simple struct to demonstrate basic memory layout
struct BasicStruct {
    char a;     // 1 byte
    int b;      // 4 bytes
    char c;     // 1 byte
    double d;   // 8 bytes
    char e;     // 1 byte
};

// Packed struct to eliminate padding
struct __attribute__((packed)) PackedStruct {
    char a;     // 1 byte
    int b;      // 4 bytes
    char c;     // 1 byte
    double d;   // 8 bytes
    char e;     // 1 byte
};

// Aligned struct for optimal performance
struct __attribute__((aligned(64))) AlignedStruct {
    char a;     // 1 byte
    int b;      // 4 bytes
    char c;     // 1 byte
    double d;   // 8 bytes
    char e;     // 1 byte
};

// =============================================================================
// GOOGLE-STYLE SEARCH INDEX STRUCTURE
// =============================================================================

// High-performance search index structure used by Google
// Optimized for cache performance and memory efficiency
struct SearchIndexEntry {
    uint64_t document_id;       // 8 bytes - Document identifier
    uint32_t term_hash;         // 4 bytes - Term hash for fast lookup
    uint16_t position;          // 2 bytes - Position in document
    uint8_t term_length;        // 1 byte - Length of term
    uint8_t flags;              // 1 byte - Status flags
    char term[16];              // 16 bytes - Term string (padded)
    
    // Constructor
    SearchIndexEntry(uint64_t doc_id, uint32_t hash, uint16_t pos, 
                     uint8_t len, uint8_t f, const char* t)
        : document_id(doc_id), term_hash(hash), position(pos), 
          term_length(len), flags(f) {
        std::strncpy(term, t, sizeof(term) - 1);
        term[sizeof(term) - 1] = '\0';
    }
    
    // Default constructor
    SearchIndexEntry() : document_id(0), term_hash(0), position(0), 
                         term_length(0), flags(0) {
        std::memset(term, 0, sizeof(term));
    }
    
    // Check if entry is valid
    bool is_valid() const {
        return document_id > 0 && term_hash > 0 && term_length > 0;
    }
    
    // Print entry information
    void print() const {
        std::cout << "Doc ID: " << document_id 
                  << ", Hash: 0x" << std::hex << term_hash << std::dec
                  << ", Pos: " << position 
                  << ", Term: " << term << std::endl;
    }
};

// =============================================================================
// UBER-STYLE RIDE MATCHING STRUCTURE
// =============================================================================

// Ride matching structure used by Uber's dispatch system
// Optimized for real-time geospatial operations
struct RideMatch {
    uint64_t ride_id;           // 8 bytes - Ride identifier
    uint32_t driver_id;         // 4 bytes - Driver identifier
    float pickup_lat;           // 4 bytes - Pickup latitude
    float pickup_lng;           // 4 bytes - Pickup longitude
    float dropoff_lat;          // 4 bytes - Dropoff latitude
    float dropoff_lng;          // 4 bytes - Dropoff longitude
    uint32_t estimated_time;    // 4 bytes - Estimated arrival time
    uint16_t estimated_fare;    // 2 bytes - Estimated fare in cents
    uint8_t vehicle_type;       // 1 byte - Vehicle type
    uint8_t priority;           // 1 byte - Match priority
    bool is_confirmed;          // 1 byte - Confirmation status
    char padding[3];            // 3 bytes - Padding for alignment
    
    // Constructor
    RideMatch(uint64_t r_id, uint32_t d_id, float p_lat, float p_lng, 
              float d_lat, float d_lng, uint32_t est_time, uint16_t est_fare, 
              uint8_t v_type, uint8_t prio, bool confirmed = false)
        : ride_id(r_id), driver_id(d_id), pickup_lat(p_lat), pickup_lng(p_lng), 
          dropoff_lat(d_lat), dropoff_lng(d_lng), estimated_time(est_time), 
          estimated_fare(est_fare), vehicle_type(v_type), priority(prio), 
          is_confirmed(confirmed) {
        std::memset(padding, 0, sizeof(padding));
    }
    
    // Default constructor
    RideMatch() : ride_id(0), driver_id(0), pickup_lat(0.0f), pickup_lng(0.0f), 
                  dropoff_lat(0.0f), dropoff_lng(0.0f), estimated_time(0), 
                  estimated_fare(0), vehicle_type(0), priority(0), 
                  is_confirmed(false) {
        std::memset(padding, 0, sizeof(padding));
    }
    
    // Calculate distance (simplified)
    float calculate_distance() const {
        float lat_diff = dropoff_lat - pickup_lat;
        float lng_diff = dropoff_lng - pickup_lng;
        return std::sqrt(lat_diff * lat_diff + lng_diff * lng_diff);
    }
    
    // Check if match is valid
    bool is_valid() const {
        return ride_id > 0 && driver_id > 0 && 
               pickup_lat != 0.0f && pickup_lng != 0.0f;
    }
    
    // Print match information
    void print() const {
        std::cout << "Ride ID: " << ride_id 
                  << ", Driver ID: " << driver_id 
                  << ", Distance: " << calculate_distance() 
                  << ", Fare: $" << (estimated_fare / 100.0) 
                  << ", Confirmed: " << (is_confirmed ? "Yes" : "No") << std::endl;
    }
};

// =============================================================================
// BLOOMBERG-STYLE FINANCIAL DATA STRUCTURE
// =============================================================================

// High-frequency trading data structure used by Bloomberg
// Optimized for minimal latency and maximum throughput
struct __attribute__((packed)) TradingData {
    uint64_t timestamp;         // 8 bytes - Microsecond timestamp
    uint32_t symbol_hash;       // 4 bytes - Symbol hash for fast lookup
    uint32_t price;             // 4 bytes - Price in basis points
    uint32_t volume;            // 4 bytes - Trading volume
    uint16_t bid_price;         // 2 bytes - Best bid price
    uint16_t ask_price;         // 2 bytes - Best ask price
    uint8_t exchange;           // 1 byte - Exchange identifier
    uint8_t flags;              // 1 byte - Status flags
    char symbol[8];             // 8 bytes - Symbol string
    
    // Constructor
    TradingData(uint64_t ts, uint32_t hash, uint32_t p, uint32_t vol, 
                uint16_t bid, uint16_t ask, uint8_t exch, uint8_t f, 
                const char* sym)
        : timestamp(ts), symbol_hash(hash), price(p), volume(vol), 
          bid_price(bid), ask_price(ask), exchange(exch), flags(f) {
        std::strncpy(symbol, sym, sizeof(symbol) - 1);
        symbol[sizeof(symbol) - 1] = '\0';
    }
    
    // Default constructor
    TradingData() : timestamp(0), symbol_hash(0), price(0), volume(0), 
                    bid_price(0), ask_price(0), exchange(0), flags(0) {
        std::memset(symbol, 0, sizeof(symbol));
    }
    
    // Calculate spread
    uint16_t get_spread() const {
        return ask_price - bid_price;
    }
    
    // Check if data is valid
    bool is_valid() const {
        return timestamp > 0 && price > 0 && volume > 0;
    }
    
    // Print trading data
    void print() const {
        std::cout << "Symbol: " << symbol 
                  << ", Price: " << (price / 10000.0) 
                  << ", Volume: " << volume 
                  << ", Spread: " << get_spread() << std::endl;
    }
};

// =============================================================================
// MEMORY LAYOUT ANALYSIS FUNCTIONS
// =============================================================================

void analyze_basic_memory_layout() {
    std::cout << "\n=== BASIC MEMORY LAYOUT ANALYSIS ===" << std::endl;
    
    // Create instances
    BasicStruct basic;
    PackedStruct packed;
    AlignedStruct aligned;
    
    // Initialize with test data
    basic.a = 'A';
    basic.b = 42;
    basic.c = 'C';
    basic.d = 3.14159;
    basic.e = 'E';
    
    packed.a = 'A';
    packed.b = 42;
    packed.c = 'C';
    packed.d = 3.14159;
    packed.e = 'E';
    
    aligned.a = 'A';
    aligned.b = 42;
    aligned.c = 'C';
    aligned.d = 3.14159;
    aligned.e = 'E';
    
    // Print sizes
    std::cout << "Struct sizes:" << std::endl;
    std::cout << "  BasicStruct: " << sizeof(BasicStruct) << " bytes" << std::endl;
    std::cout << "  PackedStruct: " << sizeof(PackedStruct) << " bytes" << std::endl;
    std::cout << "  AlignedStruct: " << sizeof(AlignedStruct) << " bytes" << std::endl;
    
    // Print memory addresses
    std::cout << "\nBasicStruct memory addresses:" << std::endl;
    print_memory_address(&basic.a, "a (char)");
    print_memory_address(&basic.b, "b (int)");
    print_memory_address(&basic.c, "c (char)");
    print_memory_address(&basic.d, "d (double)");
    print_memory_address(&basic.e, "e (char)");
    
    std::cout << "\nPackedStruct memory addresses:" << std::endl;
    print_memory_address(&packed.a, "a (char)");
    print_memory_address(&packed.b, "b (int)");
    print_memory_address(&packed.c, "c (char)");
    print_memory_address(&packed.d, "d (double)");
    print_memory_address(&packed.e, "e (char)");
    
    std::cout << "\nAlignedStruct memory addresses:" << std::endl;
    print_memory_address(&aligned.a, "a (char)");
    print_memory_address(&aligned.b, "b (int)");
    print_memory_address(&aligned.c, "c (char)");
    print_memory_address(&aligned.d, "d (double)");
    print_memory_address(&aligned.e, "e (char)");
    
    // Print memory dumps
    print_memory_dump(&basic, sizeof(BasicStruct), "BasicStruct");
    print_memory_dump(&packed, sizeof(PackedStruct), "PackedStruct");
    print_memory_dump(&aligned, sizeof(AlignedStruct), "AlignedStruct");
}

void analyze_company_structs() {
    std::cout << "\n=== COMPANY-SPECIFIC STRUCT ANALYSIS ===" << std::endl;
    
    // Create instances
    SearchIndexEntry search_entry(12345, 0xABCDEF00, 100, 5, 1, "hello");
    RideMatch ride_match(987654321, 12345, 40.7128f, -74.0060f, 
                         40.7589f, -73.9851f, 300, 1500, 1, 1, true);
    TradingData trading_data(1640995200000000ULL, 0x12345678, 1500000, 1000000, 
                            1499500, 1500500, 1, 0, "AAPL");
    
    // Print sizes
    std::cout << "Company struct sizes:" << std::endl;
    std::cout << "  SearchIndexEntry: " << sizeof(SearchIndexEntry) << " bytes" << std::endl;
    std::cout << "  RideMatch: " << sizeof(RideMatch) << " bytes" << std::endl;
    std::cout << "  TradingData: " << sizeof(TradingData) << " bytes" << std::endl;
    
    // Print memory addresses
    std::cout << "\nSearchIndexEntry memory addresses:" << std::endl;
    print_memory_address(&search_entry.document_id, "document_id");
    print_memory_address(&search_entry.term_hash, "term_hash");
    print_memory_address(&search_entry.position, "position");
    print_memory_address(&search_entry.term_length, "term_length");
    print_memory_address(&search_entry.flags, "flags");
    print_memory_address(&search_entry.term, "term");
    
    std::cout << "\nRideMatch memory addresses:" << std::endl;
    print_memory_address(&ride_match.ride_id, "ride_id");
    print_memory_address(&ride_match.driver_id, "driver_id");
    print_memory_address(&ride_match.pickup_lat, "pickup_lat");
    print_memory_address(&ride_match.pickup_lng, "pickup_lng");
    print_memory_address(&ride_match.dropoff_lat, "dropoff_lat");
    print_memory_address(&ride_match.dropoff_lng, "dropoff_lng");
    print_memory_address(&ride_match.estimated_time, "estimated_time");
    print_memory_address(&ride_match.estimated_fare, "estimated_fare");
    print_memory_address(&ride_match.vehicle_type, "vehicle_type");
    print_memory_address(&ride_match.priority, "priority");
    print_memory_address(&ride_match.is_confirmed, "is_confirmed");
    
    std::cout << "\nTradingData memory addresses:" << std::endl;
    print_memory_address(&trading_data.timestamp, "timestamp");
    print_memory_address(&trading_data.symbol_hash, "symbol_hash");
    print_memory_address(&trading_data.price, "price");
    print_memory_address(&trading_data.volume, "volume");
    print_memory_address(&trading_data.bid_price, "bid_price");
    print_memory_address(&trading_data.ask_price, "ask_price");
    print_memory_address(&trading_data.exchange, "exchange");
    print_memory_address(&trading_data.flags, "flags");
    print_memory_address(&trading_data.symbol, "symbol");
    
    // Print memory dumps
    print_memory_dump(&search_entry, sizeof(SearchIndexEntry), "SearchIndexEntry");
    print_memory_dump(&ride_match, sizeof(RideMatch), "RideMatch");
    print_memory_dump(&trading_data, sizeof(TradingData), "TradingData");
}

void demonstrate_alignment_analysis() {
    std::cout << "\n=== ALIGNMENT ANALYSIS ===" << std::endl;
    
    // Test different alignment scenarios
    struct TestStruct1 {
        char a;
        int b;
        char c;
    };
    
    struct TestStruct2 {
        int a;
        char b;
        char c;
    };
    
    struct TestStruct3 {
        char a;
        char b;
        int c;
    };
    
    TestStruct1 ts1;
    TestStruct2 ts2;
    TestStruct3 ts3;
    
    std::cout << "Alignment analysis:" << std::endl;
    std::cout << "  TestStruct1 size: " << sizeof(TestStruct1) << " bytes" << std::endl;
    std::cout << "  TestStruct2 size: " << sizeof(TestStruct2) << " bytes" << std::endl;
    std::cout << "  TestStruct3 size: " << sizeof(TestStruct3) << " bytes" << std::endl;
    
    // Print memory addresses
    std::cout << "\nTestStruct1 addresses:" << std::endl;
    print_memory_address(&ts1.a, "a");
    print_memory_address(&ts1.b, "b");
    print_memory_address(&ts1.c, "c");
    
    std::cout << "\nTestStruct2 addresses:" << std::endl;
    print_memory_address(&ts2.a, "a");
    print_memory_address(&ts2.b, "b");
    print_memory_address(&ts2.c, "c");
    
    std::cout << "\nTestStruct3 addresses:" << std::endl;
    print_memory_address(&ts3.a, "a");
    print_memory_address(&ts3.b, "b");
    print_memory_address(&ts3.c, "c");
}

void demonstrate_cache_optimization() {
    std::cout << "\n=== CACHE OPTIMIZATION ANALYSIS ===" << std::endl;
    
    // Cache line size (typically 64 bytes)
    const size_t CACHE_LINE_SIZE = 64;
    
    // Struct designed for cache optimization
    struct __attribute__((aligned(64))) CacheOptimizedStruct {
        uint64_t hot_data[8];    // Frequently accessed data (64 bytes)
        uint64_t cold_data[8];   // Rarely accessed data (64 bytes)
    };
    
    // Struct with poor cache performance
    struct PoorCacheStruct {
        uint64_t hot_data;
        uint64_t cold_data;
        uint64_t hot_data2;
        uint64_t cold_data2;
        uint64_t hot_data3;
        uint64_t cold_data3;
        uint64_t hot_data4;
        uint64_t cold_data4;
    };
    
    CacheOptimizedStruct optimized;
    PoorCacheStruct poor;
    
    std::cout << "Cache optimization analysis:" << std::endl;
    std::cout << "  CacheOptimizedStruct size: " << sizeof(CacheOptimizedStruct) << " bytes" << std::endl;
    std::cout << "  PoorCacheStruct size: " << sizeof(PoorCacheStruct) << " bytes" << std::endl;
    std::cout << "  Cache line size: " << CACHE_LINE_SIZE << " bytes" << std::endl;
    
    // Check alignment
    std::cout << "  CacheOptimizedStruct aligned to cache line: " 
              << (reinterpret_cast<uintptr_t>(&optimized) % CACHE_LINE_SIZE == 0 ? "Yes" : "No") << std::endl;
    std::cout << "  PoorCacheStruct aligned to cache line: " 
              << (reinterpret_cast<uintptr_t>(&poor) % CACHE_LINE_SIZE == 0 ? "Yes" : "No") << std::endl;
    
    // Print memory addresses
    std::cout << "\nCacheOptimizedStruct addresses:" << std::endl;
    print_memory_address(&optimized.hot_data, "hot_data");
    print_memory_address(&optimized.cold_data, "cold_data");
    
    std::cout << "\nPoorCacheStruct addresses:" << std::endl;
    print_memory_address(&poor.hot_data, "hot_data");
    print_memory_address(&poor.cold_data, "cold_data");
    print_memory_address(&poor.hot_data2, "hot_data2");
    print_memory_address(&poor.cold_data2, "cold_data2");
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "=== STRUCT MEMORY LAYOUT ANALYSIS ===" << std::endl;
    std::cout << "Demonstrating memory layout techniques used by top-tier companies" << std::endl;
    
    try {
        // Analyze basic memory layout
        analyze_basic_memory_layout();
        
        // Analyze company-specific structs
        analyze_company_structs();
        
        // Demonstrate alignment analysis
        demonstrate_alignment_analysis();
        
        // Demonstrate cache optimization
        demonstrate_cache_optimization();
        
        std::cout << "\n=== MEMORY LAYOUT ANALYSIS COMPLETED SUCCESSFULLY ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

// =============================================================================
// COMPILATION NOTES
// =============================================================================
/*
 * Compile with:
 *   g++ -std=c++17 -O2 -Wall -Wextra -o memory_layout 02-memory-layout.cpp
 *   clang++ -std=c++17 -O2 -Wall -Wextra -o memory_layout 02-memory-layout.cpp
 *   cl /std:c++17 /O2 /W4 /EHsc 02-memory-layout.cpp
 *
 * Run with:
 *   ./memory_layout
 *   memory_layout.exe
 *
 * Memory analysis flags:
 *   -fsanitize=address: Address sanitizer
 *   -fsanitize=undefined: Undefined behavior sanitizer
 *   -fsanitize=memory: Memory sanitizer
 *   -fsanitize=thread: Thread sanitizer
 *
 * Performance analysis flags:
 *   -O3: Maximum optimization
 *   -march=native: Use native CPU instructions
 *   -flto: Link-time optimization
 *   -fno-exceptions: Disable exceptions for performance
 */
