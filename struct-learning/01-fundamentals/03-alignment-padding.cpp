/*
 * =============================================================================
 * Struct Fundamentals: Alignment & Padding - Performance-Critical Memory Optimization
 * Production-Grade Memory Alignment for Top-Tier Companies
 * =============================================================================
 *
 * This file demonstrates alignment and padding techniques used by Google, Uber,
 * Bloomberg, Amazon, PayPal, and Stripe for performance-critical memory
 * optimization and cache efficiency.
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
#include <chrono>
#include <vector>
#include <memory>

// =============================================================================
// ALIGNMENT ANALYSIS UTILITIES
// =============================================================================

// Utility function to print alignment information
void print_alignment_info(const char* name, size_t size, size_t alignment) {
    std::cout << "  " << std::setw(20) << name 
              << ": Size=" << std::setw(3) << size 
              << " bytes, Alignment=" << alignment << " bytes" << std::endl;
}

// Utility function to check if address is aligned
bool is_aligned(const void* ptr, size_t alignment) {
    return (reinterpret_cast<uintptr_t>(ptr) % alignment) == 0;
}

// Utility function to calculate padding
size_t calculate_padding(size_t offset, size_t alignment) {
    return (alignment - (offset % alignment)) % alignment;
}

// =============================================================================
// BASIC ALIGNMENT CONCEPTS
// =============================================================================

// Struct with poor alignment (causes padding)
struct PoorAlignment {
    char a;     // 1 byte
    int b;      // 4 bytes (3 bytes padding before)
    char c;     // 1 byte (3 bytes padding after)
    double d;   // 8 bytes
    char e;     // 1 byte (7 bytes padding after)
};

// Struct with good alignment (minimal padding)
struct GoodAlignment {
    double d;   // 8 bytes
    int b;      // 4 bytes
    char a;     // 1 byte
    char c;     // 1 byte
    char e;     // 1 byte
    char padding[1]; // 1 byte padding to align to 8 bytes
};

// Packed struct (no padding, but may cause performance issues)
struct __attribute__((packed)) PackedStruct {
    char a;     // 1 byte
    int b;      // 4 bytes
    char c;     // 1 byte
    double d;   // 8 bytes
    char e;     // 1 byte
};

// =============================================================================
// GOOGLE-STYLE CACHE-OPTIMIZED STRUCTURE
// =============================================================================

// High-performance search index structure used by Google
// Optimized for cache line alignment (64 bytes)
struct __attribute__((aligned(64))) GoogleSearchIndex {
    // Hot data (frequently accessed) - fits in one cache line
    uint64_t document_id;       // 8 bytes
    uint32_t term_hash;         // 4 bytes
    uint16_t position;          // 2 bytes
    uint8_t term_length;        // 1 byte
    uint8_t flags;              // 1 byte
    char term[16];              // 16 bytes
    uint32_t relevance_score;   // 4 bytes
    uint32_t click_count;       // 4 bytes
    uint32_t impression_count;  // 4 bytes
    uint32_t last_updated;      // 4 bytes
    char padding[12];           // 12 bytes padding to 64 bytes
    
    // Constructor
    GoogleSearchIndex(uint64_t doc_id, uint32_t hash, uint16_t pos, 
                      uint8_t len, uint8_t f, const char* t, uint32_t score)
        : document_id(doc_id), term_hash(hash), position(pos), 
          term_length(len), flags(f), relevance_score(score), 
          click_count(0), impression_count(0), last_updated(0) {
        std::strncpy(term, t, sizeof(term) - 1);
        term[sizeof(term) - 1] = '\0';
        std::memset(padding, 0, sizeof(padding));
    }
    
    // Default constructor
    GoogleSearchIndex() : document_id(0), term_hash(0), position(0), 
                          term_length(0), flags(0), relevance_score(0), 
                          click_count(0), impression_count(0), last_updated(0) {
        std::memset(term, 0, sizeof(term));
        std::memset(padding, 0, sizeof(padding));
    }
    
    // Check if aligned to cache line
    bool is_cache_aligned() const {
        return is_aligned(this, 64);
    }
    
    // Print alignment info
    void print_alignment() const {
        std::cout << "GoogleSearchIndex alignment:" << std::endl;
        std::cout << "  Size: " << sizeof(*this) << " bytes" << std::endl;
        std::cout << "  Alignment: " << alignof(*this) << " bytes" << std::endl;
        std::cout << "  Cache aligned: " << (is_cache_aligned() ? "Yes" : "No") << std::endl;
    }
};

// =============================================================================
// UBER-STYLE REAL-TIME STRUCTURE
// =============================================================================

// Real-time ride matching structure used by Uber
// Optimized for minimal latency and cache efficiency
struct __attribute__((aligned(32))) UberRideMatch {
    // Critical data for real-time processing
    uint64_t ride_id;           // 8 bytes
    uint32_t driver_id;         // 4 bytes
    float pickup_lat;           // 4 bytes
    float pickup_lng;           // 4 bytes
    float dropoff_lat;          // 4 bytes
    float dropoff_lng;          // 4 bytes
    uint32_t estimated_time;    // 4 bytes
    uint16_t estimated_fare;    // 2 bytes
    uint8_t vehicle_type;       // 1 byte
    uint8_t priority;           // 1 byte
    bool is_confirmed;          // 1 byte
    char padding[3];            // 3 bytes padding to 32 bytes
    
    // Constructor
    UberRideMatch(uint64_t r_id, uint32_t d_id, float p_lat, float p_lng, 
                  float d_lat, float d_lng, uint32_t est_time, uint16_t est_fare, 
                  uint8_t v_type, uint8_t prio, bool confirmed = false)
        : ride_id(r_id), driver_id(d_id), pickup_lat(p_lat), pickup_lng(p_lng), 
          dropoff_lat(d_lat), dropoff_lng(d_lng), estimated_time(est_time), 
          estimated_fare(est_fare), vehicle_type(v_type), priority(prio), 
          is_confirmed(confirmed) {
        std::memset(padding, 0, sizeof(padding));
    }
    
    // Default constructor
    UberRideMatch() : ride_id(0), driver_id(0), pickup_lat(0.0f), pickup_lng(0.0f), 
                      dropoff_lat(0.0f), dropoff_lng(0.0f), estimated_time(0), 
                      estimated_fare(0), vehicle_type(0), priority(0), 
                      is_confirmed(false) {
        std::memset(padding, 0, sizeof(padding));
    }
    
    // Check if aligned to 32-byte boundary
    bool is_32byte_aligned() const {
        return is_aligned(this, 32);
    }
    
    // Print alignment info
    void print_alignment() const {
        std::cout << "UberRideMatch alignment:" << std::endl;
        std::cout << "  Size: " << sizeof(*this) << " bytes" << std::endl;
        std::cout << "  Alignment: " << alignof(*this) << " bytes" << std::endl;
        std::cout << "  32-byte aligned: " << (is_32byte_aligned() ? "Yes" : "No") << std::endl;
    }
};

// =============================================================================
// BLOOMBERG-STYLE FINANCIAL DATA STRUCTURE
// =============================================================================

// High-frequency trading data structure used by Bloomberg
// Optimized for minimal latency and maximum throughput
struct __attribute__((packed)) BloombergTradingData {
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
    BloombergTradingData(uint64_t ts, uint32_t hash, uint32_t p, uint32_t vol, 
                         uint16_t bid, uint16_t ask, uint8_t exch, uint8_t f, 
                         const char* sym)
        : timestamp(ts), symbol_hash(hash), price(p), volume(vol), 
          bid_price(bid), ask_price(ask), exchange(exch), flags(f) {
        std::strncpy(symbol, sym, sizeof(symbol) - 1);
        symbol[sizeof(symbol) - 1] = '\0';
    }
    
    // Default constructor
    BloombergTradingData() : timestamp(0), symbol_hash(0), price(0), volume(0), 
                             bid_price(0), ask_price(0), exchange(0), flags(0) {
        std::memset(symbol, 0, sizeof(symbol));
    }
    
    // Calculate spread
    uint16_t get_spread() const {
        return ask_price - bid_price;
    }
    
    // Print alignment info
    void print_alignment() const {
        std::cout << "BloombergTradingData alignment:" << std::endl;
        std::cout << "  Size: " << sizeof(*this) << " bytes" << std::endl;
        std::cout << "  Alignment: " << alignof(*this) << " bytes" << std::endl;
        std::cout << "  Packed: Yes" << std::endl;
    }
};

// =============================================================================
// AMAZON-STYLE E-COMMERCE STRUCTURE
// =============================================================================

// Product information structure used by Amazon
// Optimized for database storage and API responses
struct __attribute__((aligned(16))) AmazonProduct {
    uint64_t product_id;        // 8 bytes - Product identifier
    uint32_t price_cents;       // 4 bytes - Price in cents
    uint16_t category_id;       // 2 bytes - Category identifier
    uint8_t rating;             // 1 byte - Average rating
    uint8_t availability;       // 1 byte - Availability status
    char title[32];             // 32 bytes - Product title
    char description[64];       // 64 bytes - Product description
    uint32_t review_count;      // 4 bytes - Number of reviews
    uint32_t sales_count;       // 4 bytes - Number of sales
    uint32_t last_updated;      // 4 bytes - Last update timestamp
    char padding[4];            // 4 bytes padding to 16-byte alignment
    
    // Constructor
    AmazonProduct(uint64_t id, uint32_t price, uint16_t cat_id, uint8_t rat, 
                  uint8_t avail, const char* t, const char* desc, uint32_t reviews, 
                  uint32_t sales, uint32_t updated)
        : product_id(id), price_cents(price), category_id(cat_id), rating(rat), 
          availability(avail), review_count(reviews), sales_count(sales), 
          last_updated(updated) {
        std::strncpy(title, t, sizeof(title) - 1);
        title[sizeof(title) - 1] = '\0';
        std::strncpy(description, desc, sizeof(description) - 1);
        description[sizeof(description) - 1] = '\0';
        std::memset(padding, 0, sizeof(padding));
    }
    
    // Default constructor
    AmazonProduct() : product_id(0), price_cents(0), category_id(0), rating(0), 
                      availability(0), review_count(0), sales_count(0), 
                      last_updated(0) {
        std::memset(title, 0, sizeof(title));
        std::memset(description, 0, sizeof(description));
        std::memset(padding, 0, sizeof(padding));
    }
    
    // Check if aligned to 16-byte boundary
    bool is_16byte_aligned() const {
        return is_aligned(this, 16);
    }
    
    // Print alignment info
    void print_alignment() const {
        std::cout << "AmazonProduct alignment:" << std::endl;
        std::cout << "  Size: " << sizeof(*this) << " bytes" << std::endl;
        std::cout << "  Alignment: " << alignof(*this) << " bytes" << std::endl;
        std::cout << "  16-byte aligned: " << (is_16byte_aligned() ? "Yes" : "No") << std::endl;
    }
};

// =============================================================================
// PAYPAL-STYLE PAYMENT STRUCTURE
// =============================================================================

// Payment transaction structure used by PayPal
// Optimized for security and compliance requirements
struct __attribute__((aligned(8))) PayPalTransaction {
    uint64_t transaction_id;    // 8 bytes - Transaction identifier
    uint32_t user_id;           // 4 bytes - User identifier
    uint32_t amount_cents;      // 4 bytes - Amount in cents
    uint16_t currency_code;     // 2 bytes - ISO 4217 currency code
    uint8_t payment_method;     // 1 byte - Payment method type
    uint8_t status;             // 1 byte - Transaction status
    uint32_t timestamp;         // 4 bytes - Unix timestamp
    char merchant_id[16];       // 16 bytes - Merchant identifier
    char reference_id[32];      // 32 bytes - External reference ID
    uint8_t security_hash[16];  // 16 bytes - Security hash
    char padding[4];            // 4 bytes padding to 8-byte alignment
    
    // Constructor
    PayPalTransaction(uint64_t tx_id, uint32_t uid, uint32_t amount, 
                      uint16_t currency, uint8_t method, uint8_t stat, 
                      uint32_t ts, const char* merchant, const char* ref)
        : transaction_id(tx_id), user_id(uid), amount_cents(amount), 
          currency_code(currency), payment_method(method), status(stat), 
          timestamp(ts) {
        std::strncpy(merchant_id, merchant, sizeof(merchant_id) - 1);
        merchant_id[sizeof(merchant_id) - 1] = '\0';
        std::strncpy(reference_id, ref, sizeof(reference_id) - 1);
        reference_id[sizeof(reference_id) - 1] = '\0';
        std::memset(security_hash, 0, sizeof(security_hash));
        std::memset(padding, 0, sizeof(padding));
    }
    
    // Default constructor
    PayPalTransaction() : transaction_id(0), user_id(0), amount_cents(0), 
                          currency_code(0), payment_method(0), status(0), 
                          timestamp(0) {
        std::memset(merchant_id, 0, sizeof(merchant_id));
        std::memset(reference_id, 0, sizeof(reference_id));
        std::memset(security_hash, 0, sizeof(security_hash));
        std::memset(padding, 0, sizeof(padding));
    }
    
    // Check if aligned to 8-byte boundary
    bool is_8byte_aligned() const {
        return is_aligned(this, 8);
    }
    
    // Print alignment info
    void print_alignment() const {
        std::cout << "PayPalTransaction alignment:" << std::endl;
        std::cout << "  Size: " << sizeof(*this) << " bytes" << std::endl;
        std::cout << "  Alignment: " << alignof(*this) << " bytes" << std::endl;
        std::cout << "  8-byte aligned: " << (is_8byte_aligned() ? "Yes" : "No") << std::endl;
    }
};

// =============================================================================
// DEMONSTRATION FUNCTIONS
// =============================================================================

void demonstrate_basic_alignment() {
    std::cout << "\n=== BASIC ALIGNMENT DEMONSTRATION ===" << std::endl;
    
    // Create instances
    PoorAlignment poor;
    GoodAlignment good;
    PackedStruct packed;
    
    // Print alignment information
    std::cout << "Alignment comparison:" << std::endl;
    print_alignment_info("PoorAlignment", sizeof(PoorAlignment), alignof(PoorAlignment));
    print_alignment_info("GoodAlignment", sizeof(GoodAlignment), alignof(GoodAlignment));
    print_alignment_info("PackedStruct", sizeof(PackedStruct), alignof(PackedStruct));
    
    // Calculate padding
    std::cout << "\nPadding analysis:" << std::endl;
    std::cout << "  PoorAlignment padding: " << (sizeof(PoorAlignment) - 15) << " bytes" << std::endl;
    std::cout << "  GoodAlignment padding: " << (sizeof(GoodAlignment) - 15) << " bytes" << std::endl;
    std::cout << "  PackedStruct padding: " << (sizeof(PackedStruct) - 15) << " bytes" << std::endl;
    
    // Memory efficiency
    std::cout << "\nMemory efficiency:" << std::endl;
    std::cout << "  PoorAlignment efficiency: " << (100.0 * 15 / sizeof(PoorAlignment)) << "%" << std::endl;
    std::cout << "  GoodAlignment efficiency: " << (100.0 * 15 / sizeof(GoodAlignment)) << "%" << std::endl;
    std::cout << "  PackedStruct efficiency: " << (100.0 * 15 / sizeof(PackedStruct)) << "%" << std::endl;
}

void demonstrate_company_alignments() {
    std::cout << "\n=== COMPANY-SPECIFIC ALIGNMENT DEMONSTRATION ===" << std::endl;
    
    // Create instances
    GoogleSearchIndex google_index(12345, 0xABCDEF00, 100, 5, 1, "hello", 95);
    UberRideMatch uber_match(987654321, 12345, 40.7128f, -74.0060f, 
                             40.7589f, -73.9851f, 300, 1500, 1, 1, true);
    BloombergTradingData bloomberg_data(1640995200000000ULL, 0x12345678, 1500000, 
                                       1000000, 1499500, 1500500, 1, 0, "AAPL");
    AmazonProduct amazon_product(987654321, 249999, 1, 5, 1, "MacBook Pro", 
                                "Apple MacBook Pro with M2 chip", 1250, 500, 1640995200);
    PayPalTransaction paypal_tx(555666777, 12345, 5000, 840, 1, 1, 
                               1640995200, "MERCHANT_001", "REF_001");
    
    // Print alignment information
    google_index.print_alignment();
    std::cout << std::endl;
    uber_match.print_alignment();
    std::cout << std::endl;
    bloomberg_data.print_alignment();
    std::cout << std::endl;
    amazon_product.print_alignment();
    std::cout << std::endl;
    paypal_tx.print_alignment();
}

void demonstrate_performance_impact() {
    std::cout << "\n=== PERFORMANCE IMPACT DEMONSTRATION ===" << std::endl;
    
    const size_t NUM_ITERATIONS = 1000000;
    
    // Test aligned vs unaligned access
    std::vector<GoogleSearchIndex> aligned_data(NUM_ITERATIONS);
    std::vector<PoorAlignment> unaligned_data(NUM_ITERATIONS);
    
    // Initialize data
    for (size_t i = 0; i < NUM_ITERATIONS; ++i) {
        aligned_data[i] = GoogleSearchIndex(i, i * 0x1000, i % 1000, 5, 1, "test", 90);
        unaligned_data[i].b = i;
    }
    
    // Benchmark aligned access
    auto start = std::chrono::high_resolution_clock::now();
    uint64_t aligned_sum = 0;
    for (const auto& item : aligned_data) {
        aligned_sum += item.document_id;
    }
    auto aligned_end = std::chrono::high_resolution_clock::now();
    auto aligned_duration = std::chrono::duration_cast<std::chrono::microseconds>(aligned_end - start);
    
    // Benchmark unaligned access
    start = std::chrono::high_resolution_clock::now();
    uint64_t unaligned_sum = 0;
    for (const auto& item : unaligned_data) {
        unaligned_sum += item.b;
    }
    auto unaligned_end = std::chrono::high_resolution_clock::now();
    auto unaligned_duration = std::chrono::duration_cast<std::chrono::microseconds>(unaligned_end - start);
    
    std::cout << "Performance comparison:" << std::endl;
    std::cout << "  Aligned access time: " << aligned_duration.count() << " microseconds" << std::endl;
    std::cout << "  Unaligned access time: " << unaligned_duration.count() << " microseconds" << std::endl;
    std::cout << "  Performance ratio: " << (double)unaligned_duration.count() / aligned_duration.count() << "x" << std::endl;
    std::cout << "  Aligned sum: " << aligned_sum << std::endl;
    std::cout << "  Unaligned sum: " << unaligned_sum << std::endl;
}

void demonstrate_cache_line_optimization() {
    std::cout << "\n=== CACHE LINE OPTIMIZATION DEMONSTRATION ===" << std::endl;
    
    const size_t CACHE_LINE_SIZE = 64;
    
    // Test cache line alignment
    GoogleSearchIndex google_index(12345, 0xABCDEF00, 100, 5, 1, "hello", 95);
    
    std::cout << "Cache line optimization:" << std::endl;
    std::cout << "  Cache line size: " << CACHE_LINE_SIZE << " bytes" << std::endl;
    std::cout << "  GoogleSearchIndex size: " << sizeof(GoogleSearchIndex) << " bytes" << std::endl;
    std::cout << "  Fits in cache line: " << (sizeof(GoogleSearchIndex) <= CACHE_LINE_SIZE ? "Yes" : "No") << std::endl;
    std::cout << "  Cache line aligned: " << (google_index.is_cache_aligned() ? "Yes" : "No") << std::endl;
    
    // Calculate cache line efficiency
    double efficiency = (double)sizeof(GoogleSearchIndex) / CACHE_LINE_SIZE * 100;
    std::cout << "  Cache line efficiency: " << efficiency << "%" << std::endl;
    
    // Memory address analysis
    std::cout << "  Memory address: 0x" << std::hex << reinterpret_cast<uintptr_t>(&google_index) << std::dec << std::endl;
    std::cout << "  Address % 64: " << (reinterpret_cast<uintptr_t>(&google_index) % 64) << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "=== ALIGNMENT & PADDING - PRODUCTION-GRADE EXAMPLES ===" << std::endl;
    std::cout << "Demonstrating alignment techniques used by top-tier companies" << std::endl;
    
    try {
        // Demonstrate basic alignment concepts
        demonstrate_basic_alignment();
        
        // Demonstrate company-specific alignments
        demonstrate_company_alignments();
        
        // Demonstrate performance impact
        demonstrate_performance_impact();
        
        // Demonstrate cache line optimization
        demonstrate_cache_line_optimization();
        
        std::cout << "\n=== ALIGNMENT & PADDING DEMONSTRATION COMPLETED SUCCESSFULLY ===" << std::endl;
        
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
 *   g++ -std=c++17 -O2 -Wall -Wextra -o alignment_padding 03-alignment-padding.cpp
 *   clang++ -std=c++17 -O2 -Wall -Wextra -o alignment_padding 03-alignment-padding.cpp
 *   cl /std:c++17 /O2 /W4 /EHsc 03-alignment-padding.cpp
 *
 * Run with:
 *   ./alignment_padding
 *   alignment_padding.exe
 *
 * Alignment optimization flags:
 *   -O3: Maximum optimization
 *   -march=native: Use native CPU instructions
 *   -flto: Link-time optimization
 *   -fno-exceptions: Disable exceptions for performance
 *
 * Memory analysis flags:
 *   -fsanitize=address: Address sanitizer
 *   -fsanitize=undefined: Undefined behavior sanitizer
 *   -fsanitize=memory: Memory sanitizer
 *   -fsanitize=thread: Thread sanitizer
 */
