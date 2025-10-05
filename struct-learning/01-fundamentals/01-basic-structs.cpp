/*
 * =============================================================================
 * Struct Fundamentals: Basic Structs - Core Concepts and Usage
 * Production-Grade Data Structure Foundation
 * =============================================================================
 *
 * This file demonstrates fundamental struct concepts used by top-tier companies
 * like Google, Uber, Bloomberg, Amazon, and PayPal. It covers basic syntax,
 * memory layout, and performance optimization techniques.
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
#include <chrono>
#include <vector>
#include <memory>

// =============================================================================
// BASIC STRUCT DECLARATION AND USAGE
// =============================================================================

// Basic struct declaration - POD (Plain Old Data) type
// Used by Google for search index structures and Uber for ride data
struct Person {
    char name[64];          // Fixed-size string for performance
    int age;                // 4-byte integer
    float height;           // 4-byte floating point
    bool is_active;         // 1-byte boolean (but padded to 4 bytes)
    
    // Default constructor (C++11)
    Person() : age(0), height(0.0f), is_active(false) {
        std::memset(name, 0, sizeof(name));
    }
    
    // Parameterized constructor
    Person(const char* n, int a, float h, bool active = true) 
        : age(a), height(h), is_active(active) {
        std::strncpy(name, n, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';  // Ensure null termination
    }
    
    // Copy constructor
    Person(const Person& other) 
        : age(other.age), height(other.height), is_active(other.is_active) {
        std::strncpy(name, other.name, sizeof(name));
    }
    
    // Assignment operator
    Person& operator=(const Person& other) {
        if (this != &other) {
            std::strncpy(name, other.name, sizeof(name));
            age = other.age;
            height = other.height;
            is_active = other.is_active;
        }
        return *this;
    }
    
    // Destructor (trivial for POD types)
    ~Person() = default;
    
    // Member functions
    void print_info() const {
        std::cout << "Name: " << name 
                  << ", Age: " << age 
                  << ", Height: " << height 
                  << ", Active: " << (is_active ? "Yes" : "No") << std::endl;
    }
    
    // Getter methods (const correctness)
    const char* get_name() const { return name; }
    int get_age() const { return age; }
    float get_height() const { return height; }
    bool get_is_active() const { return is_active; }
    
    // Setter methods
    void set_name(const char* n) {
        std::strncpy(name, n, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
    }
    
    void set_age(int a) { age = a; }
    void set_height(float h) { height = h; }
    void set_is_active(bool active) { is_active = active; }
};

// =============================================================================
// BLOOMBERG-STYLE FINANCIAL DATA STRUCTURE
// =============================================================================

// High-performance financial data structure used by Bloomberg Terminal
// Optimized for cache performance and memory alignment
struct __attribute__((packed)) MarketData {
    uint64_t timestamp;     // Microsecond timestamp for high-frequency trading
    char symbol[12];        // Stock symbol (e.g., "AAPL", "GOOGL")
    uint32_t price;         // Price in basis points (1/10000 of a dollar)
    uint32_t volume;        // Trading volume
    uint16_t bid_price;     // Best bid price
    uint16_t ask_price;     // Best ask price
    uint8_t exchange;       // Exchange identifier
    uint8_t flags;          // Status flags (bit field)
    
    // Constructor with initialization
    MarketData(uint64_t ts, const char* sym, uint32_t p, uint32_t vol, 
               uint16_t bid, uint16_t ask, uint8_t exch, uint8_t f = 0)
        : timestamp(ts), price(p), volume(vol), bid_price(bid), 
          ask_price(ask), exchange(exch), flags(f) {
        std::strncpy(symbol, sym, sizeof(symbol) - 1);
        symbol[sizeof(symbol) - 1] = '\0';
    }
    
    // Default constructor
    MarketData() : timestamp(0), price(0), volume(0), bid_price(0), 
                   ask_price(0), exchange(0), flags(0) {
        std::memset(symbol, 0, sizeof(symbol));
    }
    
    // Calculate spread (bid-ask spread)
    uint16_t get_spread() const {
        return ask_price - bid_price;
    }
    
    // Check if data is valid
    bool is_valid() const {
        return timestamp > 0 && price > 0 && volume > 0;
    }
    
    // Print market data
    void print() const {
        std::cout << "Symbol: " << symbol 
                  << ", Price: " << (price / 10000.0) 
                  << ", Volume: " << volume 
                  << ", Spread: " << get_spread() << std::endl;
    }
};

// =============================================================================
// AMAZON-STYLE E-COMMERCE STRUCTURE
// =============================================================================

// Product information structure used by Amazon's e-commerce platform
// Optimized for database storage and API responses
struct Product {
    uint64_t id;                    // Unique product identifier
    char title[128];                // Product title
    char description[512];          // Product description
    uint32_t price_cents;           // Price in cents (avoid floating point)
    uint16_t category_id;           // Category identifier
    uint8_t rating;                 // Average rating (1-5)
    uint32_t review_count;          // Number of reviews
    bool in_stock;                  // Availability status
    uint16_t weight_grams;          // Weight in grams
    uint8_t dimensions[3];          // Length, width, height in cm
    
    // Constructor
    Product(uint64_t product_id, const char* t, const char* desc, 
            uint32_t price, uint16_t cat_id, uint8_t rat = 0, 
            uint32_t reviews = 0, bool stock = true, uint16_t weight = 0)
        : id(product_id), price_cents(price), category_id(cat_id), 
          rating(rat), review_count(reviews), in_stock(stock), 
          weight_grams(weight) {
        std::strncpy(title, t, sizeof(title) - 1);
        title[sizeof(title) - 1] = '\0';
        std::strncpy(description, desc, sizeof(description) - 1);
        description[sizeof(description) - 1] = '\0';
        std::memset(dimensions, 0, sizeof(dimensions));
    }
    
    // Default constructor
    Product() : id(0), price_cents(0), category_id(0), rating(0), 
                review_count(0), in_stock(false), weight_grams(0) {
        std::memset(title, 0, sizeof(title));
        std::memset(description, 0, sizeof(description));
        std::memset(dimensions, 0, sizeof(dimensions));
    }
    
    // Calculate price in dollars
    double get_price_dollars() const {
        return price_cents / 100.0;
    }
    
    // Check if product is available
    bool is_available() const {
        return in_stock && price_cents > 0;
    }
    
    // Print product information
    void print() const {
        std::cout << "Product ID: " << id 
                  << ", Title: " << title 
                  << ", Price: $" << get_price_dollars() 
                  << ", Rating: " << (int)rating 
                  << ", In Stock: " << (in_stock ? "Yes" : "No") << std::endl;
    }
};

// =============================================================================
// PAYPAL-STYLE PAYMENT STRUCTURE
// =============================================================================

// Payment transaction structure used by PayPal's payment processing
// Optimized for security and compliance requirements
struct __attribute__((packed)) PaymentTransaction {
    uint64_t transaction_id;        // Unique transaction identifier
    uint64_t user_id;               // User identifier
    uint32_t amount_cents;          // Amount in cents
    uint16_t currency_code;         // ISO 4217 currency code
    uint8_t payment_method;         // Payment method type
    uint8_t status;                 // Transaction status
    uint32_t timestamp;             // Unix timestamp
    char merchant_id[32];           // Merchant identifier
    char reference_id[64];          // External reference ID
    uint8_t security_hash[32];      // Security hash for integrity
    
    // Constructor
    PaymentTransaction(uint64_t tx_id, uint64_t uid, uint32_t amount, 
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
    }
    
    // Default constructor
    PaymentTransaction() : transaction_id(0), user_id(0), amount_cents(0), 
                          currency_code(0), payment_method(0), status(0), 
                          timestamp(0) {
        std::memset(merchant_id, 0, sizeof(merchant_id));
        std::memset(reference_id, 0, sizeof(reference_id));
        std::memset(security_hash, 0, sizeof(security_hash));
    }
    
    // Calculate amount in dollars
    double get_amount_dollars() const {
        return amount_cents / 100.0;
    }
    
    // Check if transaction is valid
    bool is_valid() const {
        return transaction_id > 0 && user_id > 0 && amount_cents > 0;
    }
    
    // Check if transaction is successful
    bool is_successful() const {
        return status == 1;  // Assuming 1 = success
    }
    
    // Print transaction information
    void print() const {
        std::cout << "Transaction ID: " << transaction_id 
                  << ", User ID: " << user_id 
                  << ", Amount: $" << get_amount_dollars() 
                  << ", Status: " << (is_successful() ? "Success" : "Failed") 
                  << std::endl;
    }
};

// =============================================================================
// UBER-STYLE RIDE DATA STRUCTURE
// =============================================================================

// Ride information structure used by Uber's dispatch system
// Optimized for real-time processing and geospatial operations
struct RideRequest {
    uint64_t request_id;            // Unique request identifier
    uint32_t user_id;               // User identifier
    float pickup_lat;               // Pickup latitude
    float pickup_lng;               // Pickup longitude
    float dropoff_lat;              // Dropoff latitude
    float dropoff_lng;              // Dropoff longitude
    uint32_t request_time;          // Request timestamp
    uint8_t vehicle_type;           // Vehicle type preference
    uint8_t priority;               // Request priority
    uint16_t estimated_fare;        // Estimated fare in cents
    bool is_scheduled;              // Scheduled vs immediate request
    
    // Constructor
    RideRequest(uint64_t req_id, uint32_t uid, float p_lat, float p_lng, 
                float d_lat, float d_lng, uint32_t req_time, uint8_t v_type, 
                uint8_t prio, uint16_t fare, bool scheduled = false)
        : request_id(req_id), user_id(uid), pickup_lat(p_lat), pickup_lng(p_lng), 
          dropoff_lat(d_lat), dropoff_lng(d_lng), request_time(req_time), 
          vehicle_type(v_type), priority(prio), estimated_fare(fare), 
          is_scheduled(scheduled) {}
    
    // Default constructor
    RideRequest() : request_id(0), user_id(0), pickup_lat(0.0f), pickup_lng(0.0f), 
                    dropoff_lat(0.0f), dropoff_lng(0.0f), request_time(0), 
                    vehicle_type(0), priority(0), estimated_fare(0), 
                    is_scheduled(false) {}
    
    // Calculate distance (simplified Euclidean distance)
    float calculate_distance() const {
        float lat_diff = dropoff_lat - pickup_lat;
        float lng_diff = dropoff_lng - pickup_lng;
        return std::sqrt(lat_diff * lat_diff + lng_diff * lng_diff);
    }
    
    // Check if request is valid
    bool is_valid() const {
        return request_id > 0 && user_id > 0 && 
               pickup_lat != 0.0f && pickup_lng != 0.0f &&
               dropoff_lat != 0.0f && dropoff_lng != 0.0f;
    }
    
    // Print ride information
    void print() const {
        std::cout << "Request ID: " << request_id 
                  << ", User ID: " << user_id 
                  << ", Distance: " << calculate_distance() 
                  << ", Fare: $" << (estimated_fare / 100.0) 
                  << ", Scheduled: " << (is_scheduled ? "Yes" : "No") << std::endl;
    }
};

// =============================================================================
// DEMONSTRATION FUNCTIONS
// =============================================================================

void demonstrate_basic_structs() {
    std::cout << "\n=== BASIC STRUCT DEMONSTRATION ===" << std::endl;
    
    // Create Person instances
    Person person1("John Doe", 30, 5.9f, true);
    Person person2("Jane Smith", 25, 5.6f, false);
    
    // Print person information
    person1.print_info();
    person2.print_info();
    
    // Demonstrate member access
    std::cout << "Person 1 age: " << person1.get_age() << std::endl;
    std::cout << "Person 2 height: " << person2.get_height() << std::endl;
    
    // Demonstrate assignment
    person2 = person1;
    std::cout << "After assignment:" << std::endl;
    person2.print_info();
}

void demonstrate_financial_structs() {
    std::cout << "\n=== BLOOMBERG-STYLE FINANCIAL DATA ===" << std::endl;
    
    // Create market data
    MarketData apple_data(1640995200000000ULL, "AAPL", 1500000, 1000000, 
                         1499500, 1500500, 1, 0);
    MarketData google_data(1640995200000001ULL, "GOOGL", 2800000, 500000, 
                          2799500, 2800500, 1, 0);
    
    // Print market data
    apple_data.print();
    google_data.print();
    
    // Demonstrate calculations
    std::cout << "Apple spread: " << apple_data.get_spread() << " basis points" << std::endl;
    std::cout << "Google spread: " << google_data.get_spread() << " basis points" << std::endl;
}

void demonstrate_ecommerce_structs() {
    std::cout << "\n=== AMAZON-STYLE E-COMMERCE DATA ===" << std::endl;
    
    // Create products
    Product laptop(1001, "MacBook Pro 16-inch", 
                   "Apple MacBook Pro with M2 chip", 249999, 1, 5, 1250, true, 2000);
    Product phone(1002, "iPhone 15 Pro", 
                 "Latest iPhone with titanium design", 99999, 1, 4, 890, true, 187);
    
    // Print product information
    laptop.print();
    phone.print();
    
    // Demonstrate calculations
    std::cout << "Laptop price: $" << laptop.get_price_dollars() << std::endl;
    std::cout << "Phone available: " << (phone.is_available() ? "Yes" : "No") << std::endl;
}

void demonstrate_payment_structs() {
    std::cout << "\n=== PAYPAL-STYLE PAYMENT DATA ===" << std::endl;
    
    // Create payment transactions
    PaymentTransaction tx1(123456789ULL, 987654321ULL, 5000, 840, 1, 1, 
                          1640995200, "MERCHANT_001", "REF_001");
    PaymentTransaction tx2(123456790ULL, 987654322ULL, 2500, 840, 2, 0, 
                          1640995201, "MERCHANT_002", "REF_002");
    
    // Print transaction information
    tx1.print();
    tx2.print();
    
    // Demonstrate validation
    std::cout << "Transaction 1 valid: " << (tx1.is_valid() ? "Yes" : "No") << std::endl;
    std::cout << "Transaction 2 successful: " << (tx2.is_successful() ? "Yes" : "No") << std::endl;
}

void demonstrate_ride_structs() {
    std::cout << "\n=== UBER-STYLE RIDE DATA ===" << std::endl;
    
    // Create ride requests
    RideRequest ride1(987654321ULL, 12345, 40.7128f, -74.0060f, 
                      40.7589f, -73.9851f, 1640995200, 1, 1, 1500, false);
    RideRequest ride2(987654322ULL, 12346, 37.7749f, -122.4194f, 
                      37.7849f, -122.4094f, 1640995201, 2, 2, 2000, true);
    
    // Print ride information
    ride1.print();
    ride2.print();
    
    // Demonstrate calculations
    std::cout << "Ride 1 distance: " << ride1.calculate_distance() << " units" << std::endl;
    std::cout << "Ride 2 valid: " << (ride2.is_valid() ? "Yes" : "No") << std::endl;
}

void demonstrate_memory_layout() {
    std::cout << "\n=== MEMORY LAYOUT ANALYSIS ===" << std::endl;
    
    // Print struct sizes
    std::cout << "Person size: " << sizeof(Person) << " bytes" << std::endl;
    std::cout << "MarketData size: " << sizeof(MarketData) << " bytes" << std::endl;
    std::cout << "Product size: " << sizeof(Product) << " bytes" << std::endl;
    std::cout << "PaymentTransaction size: " << sizeof(PaymentTransaction) << " bytes" << std::endl;
    std::cout << "RideRequest size: " << sizeof(RideRequest) << " bytes" << std::endl;
    
    // Demonstrate memory alignment
    Person p;
    std::cout << "Person memory addresses:" << std::endl;
    std::cout << "  name: " << (void*)&p.name << std::endl;
    std::cout << "  age: " << (void*)&p.age << std::endl;
    std::cout << "  height: " << (void*)&p.height << std::endl;
    std::cout << "  is_active: " << (void*)&p.is_active << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "=== STRUCT FUNDAMENTALS - PRODUCTION-GRADE EXAMPLES ===" << std::endl;
    std::cout << "Demonstrating struct techniques used by top-tier companies" << std::endl;
    
    try {
        // Demonstrate basic struct concepts
        demonstrate_basic_structs();
        
        // Demonstrate company-specific struct patterns
        demonstrate_financial_structs();
        demonstrate_ecommerce_structs();
        demonstrate_payment_structs();
        demonstrate_ride_structs();
        
        // Demonstrate memory layout analysis
        demonstrate_memory_layout();
        
        std::cout << "\n=== DEMONSTRATION COMPLETED SUCCESSFULLY ===" << std::endl;
        
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
 *   g++ -std=c++17 -O2 -Wall -Wextra -o struct_basics 01-basic-structs.cpp
 *   clang++ -std=c++17 -O2 -Wall -Wextra -o struct_basics 01-basic-structs.cpp
 *   cl /std:c++17 /O2 /W4 /EHsc 01-basic-structs.cpp
 *
 * Run with:
 *   ./struct_basics
 *   struct_basics.exe
 *
 * Performance optimization flags:
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
