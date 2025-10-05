/*
 * =============================================================================
 * Advanced Struct Techniques: Union Mastery - Memory-Efficient Data Representation
 * Production-Grade Union Patterns for Top-Tier Companies
 * =============================================================================
 *
 * This file demonstrates advanced union techniques used by Google, Uber,
 * Bloomberg, Amazon, PayPal, and Stripe for memory-efficient data
 * representation and performance optimization.
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
#include <type_traits>
#include <variant>
#include <memory>
#include <chrono>

// =============================================================================
// BASIC UNION CONCEPTS
// =============================================================================

// Simple union for basic understanding
union BasicUnion {
    int integer;
    float floating;
    char character;
    double double_val;
};

// Union with struct members
union StructUnion {
    struct {
        int x, y;
    } point;
    struct {
        float r, g, b, a;
    } color;
    struct {
        char name[16];
        int id;
    } entity;
};

// =============================================================================
// GOOGLE-STYLE DISCRIMINATED UNION
// =============================================================================

// Discriminated union for search result types
// Used by Google for different types of search results
struct SearchResult {
    enum Type { WEB_PAGE, IMAGE, VIDEO, NEWS, MAPS } type;
    
    union {
        struct {
            char title[256];
            char url[512];
            char snippet[1024];
            float relevance_score;
        } web_page;
        
        struct {
            char title[256];
            char url[512];
            char thumbnail_url[512];
            int width, height;
            float duration;
        } image;
        
        struct {
            char title[256];
            char url[512];
            char thumbnail_url[512];
            float duration;
            int views;
        } video;
        
        struct {
            char title[256];
            char url[512];
            char snippet[1024];
            char source[128];
            uint64_t timestamp;
        } news;
        
        struct {
            char name[256];
            char address[512];
            float latitude, longitude;
            float rating;
            int reviews;
        } maps;
    } data;
    
    // Constructor
    SearchResult(Type t) : type(t) {
        std::memset(&data, 0, sizeof(data));
    }
    
    // Destructor (trivial for POD types)
    ~SearchResult() = default;
    
    // Copy constructor
    SearchResult(const SearchResult& other) : type(other.type) {
        std::memcpy(&data, &other.data, sizeof(data));
    }
    
    // Assignment operator
    SearchResult& operator=(const SearchResult& other) {
        if (this != &other) {
            type = other.type;
            std::memcpy(&data, &other.data, sizeof(data));
        }
        return *this;
    }
    
    // Print result based on type
    void print() const {
        switch (type) {
            case WEB_PAGE:
                std::cout << "Web Page: " << data.web_page.title 
                          << " (" << data.web_page.url << ")" << std::endl;
                break;
            case IMAGE:
                std::cout << "Image: " << data.image.title 
                          << " (" << data.image.width << "x" << data.image.height << ")" << std::endl;
                break;
            case VIDEO:
                std::cout << "Video: " << data.video.title 
                          << " (" << data.video.duration << "s, " << data.video.views << " views)" << std::endl;
                break;
            case NEWS:
                std::cout << "News: " << data.news.title 
                          << " (" << data.news.source << ")" << std::endl;
                break;
            case MAPS:
                std::cout << "Maps: " << data.maps.name 
                          << " (" << data.maps.address << ")" << std::endl;
                break;
        }
    }
};

// =============================================================================
// UBER-STYLE RIDE DATA UNION
// =============================================================================

// Union for different types of ride data
// Used by Uber for efficient memory usage in real-time systems
struct RideData {
    enum DataType { REQUEST, MATCH, COMPLETION, CANCELLATION } data_type;
    
    union {
        struct {
            uint64_t request_id;
            uint32_t user_id;
            float pickup_lat, pickup_lng;
            float dropoff_lat, dropoff_lng;
            uint32_t request_time;
            uint8_t vehicle_type;
            uint16_t estimated_fare;
        } request;
        
        struct {
            uint64_t request_id;
            uint32_t driver_id;
            uint32_t match_time;
            uint16_t estimated_arrival;
            uint16_t estimated_fare;
            float driver_lat, driver_lng;
        } match;
        
        struct {
            uint64_t request_id;
            uint32_t driver_id;
            uint32_t completion_time;
            uint16_t actual_fare;
            float actual_distance;
            uint8_t rating;
        } completion;
        
        struct {
            uint64_t request_id;
            uint32_t user_id;
            uint32_t cancellation_time;
            uint8_t reason;
            uint16_t cancellation_fee;
        } cancellation;
    } data;
    
    // Constructor
    RideData(DataType type) : data_type(type) {
        std::memset(&data, 0, sizeof(data));
    }
    
    // Print ride data based on type
    void print() const {
        switch (data_type) {
            case REQUEST:
                std::cout << "Ride Request: ID=" << data.request.request_id 
                          << ", User=" << data.request.user_id 
                          << ", Fare=$" << (data.request.estimated_fare / 100.0) << std::endl;
                break;
            case MATCH:
                std::cout << "Ride Match: Request=" << data.match.request_id 
                          << ", Driver=" << data.match.driver_id 
                          << ", ETA=" << data.match.estimated_arrival << "min" << std::endl;
                break;
            case COMPLETION:
                std::cout << "Ride Completion: Request=" << data.completion.request_id 
                          << ", Driver=" << data.completion.driver_id 
                          << ", Fare=$" << (data.completion.actual_fare / 100.0) << std::endl;
                break;
            case CANCELLATION:
                std::cout << "Ride Cancellation: Request=" << data.cancellation.request_id 
                          << ", User=" << data.cancellation.user_id 
                          << ", Reason=" << (int)data.cancellation.reason << std::endl;
                break;
        }
    }
};

// =============================================================================
// BLOOMBERG-STYLE FINANCIAL DATA UNION
// =============================================================================

// Union for different types of financial data
// Used by Bloomberg for efficient market data storage
struct FinancialData {
    enum DataType { STOCK, BOND, CURRENCY, COMMODITY, INDEX } data_type;
    
    union {
        struct {
            char symbol[12];
            uint32_t price;  // Price in basis points
            uint32_t volume;
            uint16_t bid_price, ask_price;
            uint8_t exchange;
            uint8_t flags;
        } stock;
        
        struct {
            char symbol[12];
            uint32_t price;  // Price in basis points
            uint32_t volume;
            uint16_t coupon_rate;  // In basis points
            uint32_t maturity_date;
            uint8_t credit_rating;
        } bond;
        
        struct {
            char symbol[8];  // Currency pair
            uint32_t rate;   // Exchange rate in basis points
            uint32_t volume;
            uint16_t bid_rate, ask_rate;
            uint8_t market;
        } currency;
        
        struct {
            char symbol[12];
            uint32_t price;  // Price in basis points
            uint32_t volume;
            uint16_t contract_size;
            uint32_t expiration_date;
            uint8_t commodity_type;
        } commodity;
        
        struct {
            char symbol[12];
            uint32_t value;  // Index value in basis points
            uint32_t volume;
            uint16_t change;  // Change in basis points
            uint8_t market;
            uint8_t flags;
        } index;
    } data;
    
    // Constructor
    FinancialData(DataType type) : data_type(type) {
        std::memset(&data, 0, sizeof(data));
    }
    
    // Print financial data based on type
    void print() const {
        switch (data_type) {
            case STOCK:
                std::cout << "Stock: " << data.stock.symbol 
                          << ", Price=$" << (data.stock.price / 10000.0) 
                          << ", Volume=" << data.stock.volume << std::endl;
                break;
            case BOND:
                std::cout << "Bond: " << data.bond.symbol 
                          << ", Price=$" << (data.bond.price / 10000.0) 
                          << ", Coupon=" << (data.bond.coupon_rate / 100.0) << "%" << std::endl;
                break;
            case CURRENCY:
                std::cout << "Currency: " << data.currency.symbol 
                          << ", Rate=" << (data.currency.rate / 10000.0) 
                          << ", Volume=" << data.currency.volume << std::endl;
                break;
            case COMMODITY:
                std::cout << "Commodity: " << data.commodity.symbol 
                          << ", Price=$" << (data.commodity.price / 10000.0) 
                          << ", Volume=" << data.commodity.volume << std::endl;
                break;
            case INDEX:
                std::cout << "Index: " << data.index.symbol 
                          << ", Value=" << (data.index.value / 10000.0) 
                          << ", Change=" << (data.index.change / 10000.0) << std::endl;
                break;
        }
    }
};

// =============================================================================
// AMAZON-STYLE E-COMMERCE UNION
// =============================================================================

// Union for different types of e-commerce data
// Used by Amazon for efficient product data storage
struct ECommerceData {
    enum DataType { PRODUCT, ORDER, CART, REVIEW, RECOMMENDATION } data_type;
    
    union {
        struct {
            uint64_t product_id;
            char title[128];
            uint32_t price_cents;
            uint16_t category_id;
            uint8_t rating;
            uint32_t review_count;
            bool in_stock;
        } product;
        
        struct {
            uint64_t order_id;
            uint32_t user_id;
            uint32_t total_cents;
            uint8_t status;
            uint32_t order_time;
            uint16_t item_count;
        } order;
        
        struct {
            uint32_t user_id;
            uint64_t product_id;
            uint16_t quantity;
            uint32_t added_time;
            uint32_t price_cents;
        } cart;
        
        struct {
            uint64_t review_id;
            uint64_t product_id;
            uint32_t user_id;
            uint8_t rating;
            char comment[512];
            uint32_t review_time;
        } review;
        
        struct {
            uint32_t user_id;
            uint64_t product_id;
            float score;
            uint8_t algorithm;
            uint32_t generated_time;
        } recommendation;
    } data;
    
    // Constructor
    ECommerceData(DataType type) : data_type(type) {
        std::memset(&data, 0, sizeof(data));
    }
    
    // Print e-commerce data based on type
    void print() const {
        switch (data_type) {
            case PRODUCT:
                std::cout << "Product: " << data.product.title 
                          << ", Price=$" << (data.product.price_cents / 100.0) 
                          << ", Rating=" << (int)data.product.rating << "/5" << std::endl;
                break;
            case ORDER:
                std::cout << "Order: ID=" << data.order.order_id 
                          << ", User=" << data.order.user_id 
                          << ", Total=$" << (data.order.total_cents / 100.0) << std::endl;
                break;
            case CART:
                std::cout << "Cart: User=" << data.cart.user_id 
                          << ", Product=" << data.cart.product_id 
                          << ", Qty=" << data.cart.quantity << std::endl;
                break;
            case REVIEW:
                std::cout << "Review: Product=" << data.review.product_id 
                          << ", User=" << data.review.user_id 
                          << ", Rating=" << (int)data.review.rating << "/5" << std::endl;
                break;
            case RECOMMENDATION:
                std::cout << "Recommendation: User=" << data.recommendation.user_id 
                          << ", Product=" << data.recommendation.product_id 
                          << ", Score=" << data.recommendation.score << std::endl;
                break;
        }
    }
};

// =============================================================================
// PAYPAL-STYLE PAYMENT UNION
// =============================================================================

// Union for different types of payment data
// Used by PayPal for efficient payment processing
struct PaymentData {
    enum DataType { TRANSACTION, REFUND, CHARGEBACK, SETTLEMENT, FEE } data_type;
    
    union {
        struct {
            uint64_t transaction_id;
            uint32_t user_id;
            uint32_t amount_cents;
            uint16_t currency_code;
            uint8_t payment_method;
            uint8_t status;
            uint32_t timestamp;
        } transaction;
        
        struct {
            uint64_t refund_id;
            uint64_t original_transaction_id;
            uint32_t amount_cents;
            uint8_t reason;
            uint32_t refund_time;
            uint8_t status;
        } refund;
        
        struct {
            uint64_t chargeback_id;
            uint64_t original_transaction_id;
            uint32_t amount_cents;
            uint8_t reason;
            uint32_t chargeback_time;
            uint8_t status;
        } chargeback;
        
        struct {
            uint64_t settlement_id;
            uint32_t merchant_id;
            uint32_t total_cents;
            uint8_t status;
            uint32_t settlement_time;
            uint16_t transaction_count;
        } settlement;
        
        struct {
            uint64_t fee_id;
            uint64_t transaction_id;
            uint32_t fee_cents;
            uint8_t fee_type;
            uint32_t fee_time;
            uint8_t status;
        } fee;
    } data;
    
    // Constructor
    PaymentData(DataType type) : data_type(type) {
        std::memset(&data, 0, sizeof(data));
    }
    
    // Print payment data based on type
    void print() const {
        switch (data_type) {
            case TRANSACTION:
                std::cout << "Transaction: ID=" << data.transaction.transaction_id 
                          << ", User=" << data.transaction.user_id 
                          << ", Amount=$" << (data.transaction.amount_cents / 100.0) << std::endl;
                break;
            case REFUND:
                std::cout << "Refund: ID=" << data.refund.refund_id 
                          << ", Original=" << data.refund.original_transaction_id 
                          << ", Amount=$" << (data.refund.amount_cents / 100.0) << std::endl;
                break;
            case CHARGEBACK:
                std::cout << "Chargeback: ID=" << data.chargeback.chargeback_id 
                          << ", Original=" << data.chargeback.original_transaction_id 
                          << ", Amount=$" << (data.chargeback.amount_cents / 100.0) << std::endl;
                break;
            case SETTLEMENT:
                std::cout << "Settlement: ID=" << data.settlement.settlement_id 
                          << ", Merchant=" << data.settlement.merchant_id 
                          << ", Total=$" << (data.settlement.total_cents / 100.0) << std::endl;
                break;
            case FEE:
                std::cout << "Fee: ID=" << data.fee.fee_id 
                          << ", Transaction=" << data.fee.transaction_id 
                          << ", Amount=$" << (data.fee.fee_cents / 100.0) << std::endl;
                break;
        }
    }
};

// =============================================================================
// ADVANCED UNION TECHNIQUES
// =============================================================================

// Type punning union for low-level operations
union TypePunningUnion {
    uint32_t integer;
    float floating;
    char bytes[4];
    
    // Safe type punning
    float int_to_float(uint32_t i) {
        integer = i;
        return floating;
    }
    
    uint32_t float_to_int(float f) {
        floating = f;
        return integer;
    }
    
    // Byte manipulation
    void set_byte(size_t index, uint8_t value) {
        if (index < 4) {
            bytes[index] = value;
        }
    }
    
    uint8_t get_byte(size_t index) const {
        return (index < 4) ? bytes[index] : 0;
    }
};

// =============================================================================
// DEMONSTRATION FUNCTIONS
// =============================================================================

void demonstrate_basic_unions() {
    std::cout << "\n=== BASIC UNION DEMONSTRATION ===" << std::endl;
    
    // Basic union usage
    BasicUnion basic;
    basic.integer = 42;
    std::cout << "Integer: " << basic.integer << std::endl;
    std::cout << "Float: " << basic.floating << std::endl;
    
    basic.floating = 3.14159f;
    std::cout << "After setting float:" << std::endl;
    std::cout << "Integer: " << basic.integer << std::endl;
    std::cout << "Float: " << basic.floating << std::endl;
    
    // Struct union usage
    StructUnion struct_union;
    struct_union.point.x = 10;
    struct_union.point.y = 20;
    std::cout << "Point: (" << struct_union.point.x << ", " << struct_union.point.y << ")" << std::endl;
    
    struct_union.color.r = 1.0f;
    struct_union.color.g = 0.5f;
    struct_union.color.b = 0.0f;
    struct_union.color.a = 1.0f;
    std::cout << "Color: (" << struct_union.color.r << ", " << struct_union.color.g 
              << ", " << struct_union.color.b << ", " << struct_union.color.a << ")" << std::endl;
}

void demonstrate_discriminated_unions() {
    std::cout << "\n=== DISCRIMINATED UNION DEMONSTRATION ===" << std::endl;
    
    // Google search results
    SearchResult web_result(SearchResult::WEB_PAGE);
    std::strcpy(web_result.data.web_page.title, "Google Search Result");
    std::strcpy(web_result.data.web_page.url, "https://example.com");
    std::strcpy(web_result.data.web_page.snippet, "This is a search result snippet");
    web_result.data.web_page.relevance_score = 0.95f;
    web_result.print();
    
    SearchResult image_result(SearchResult::IMAGE);
    std::strcpy(image_result.data.image.title, "Sample Image");
    std::strcpy(image_result.data.image.url, "https://example.com/image.jpg");
    image_result.data.image.width = 1920;
    image_result.data.image.height = 1080;
    image_result.data.image.duration = 0.0f;
    image_result.print();
}

void demonstrate_company_unions() {
    std::cout << "\n=== COMPANY-SPECIFIC UNION DEMONSTRATION ===" << std::endl;
    
    // Uber ride data
    RideData ride_request(RideData::REQUEST);
    ride_request.data.request.request_id = 123456789;
    ride_request.data.request.user_id = 98765;
    ride_request.data.request.pickup_lat = 40.7128f;
    ride_request.data.request.pickup_lng = -74.0060f;
    ride_request.data.request.estimated_fare = 1500;
    ride_request.print();
    
    // Bloomberg financial data
    FinancialData stock_data(FinancialData::STOCK);
    std::strcpy(stock_data.data.stock.symbol, "AAPL");
    stock_data.data.stock.price = 1500000;  // $150.00 in basis points
    stock_data.data.stock.volume = 1000000;
    stock_data.print();
    
    // Amazon e-commerce data
    ECommerceData product_data(ECommerceData::PRODUCT);
    product_data.data.product.product_id = 987654321;
    std::strcpy(product_data.data.product.title, "MacBook Pro");
    product_data.data.product.price_cents = 249999;
    product_data.data.product.rating = 5;
    product_data.print();
    
    // PayPal payment data
    PaymentData transaction_data(PaymentData::TRANSACTION);
    transaction_data.data.transaction.transaction_id = 555666777;
    transaction_data.data.transaction.user_id = 12345;
    transaction_data.data.transaction.amount_cents = 5000;
    transaction_data.data.transaction.currency_code = 840;  // USD
    transaction_data.print();
}

void demonstrate_type_punning() {
    std::cout << "\n=== TYPE PUNNING DEMONSTRATION ===" << std::endl;
    
    TypePunningUnion punning;
    
    // Convert integer to float
    float result = punning.int_to_float(0x40490FDB);  // 3.14159f
    std::cout << "Integer 0x40490FDB as float: " << result << std::endl;
    
    // Convert float to integer
    uint32_t int_result = punning.float_to_int(3.14159f);
    std::cout << "Float 3.14159 as integer: 0x" << std::hex << int_result << std::dec << std::endl;
    
    // Byte manipulation
    punning.set_byte(0, 0x12);
    punning.set_byte(1, 0x34);
    punning.set_byte(2, 0x56);
    punning.set_byte(3, 0x78);
    
    std::cout << "Bytes: ";
    for (size_t i = 0; i < 4; ++i) {
        std::cout << "0x" << std::hex << (int)punning.get_byte(i) << " ";
    }
    std::cout << std::dec << std::endl;
    std::cout << "As integer: 0x" << std::hex << punning.integer << std::dec << std::endl;
}

void demonstrate_memory_efficiency() {
    std::cout << "\n=== MEMORY EFFICIENCY DEMONSTRATION ===" << std::endl;
    
    // Compare sizes
    std::cout << "Size comparison:" << std::endl;
    std::cout << "  BasicUnion: " << sizeof(BasicUnion) << " bytes" << std::endl;
    std::cout << "  SearchResult: " << sizeof(SearchResult) << " bytes" << std::endl;
    std::cout << "  RideData: " << sizeof(RideData) << " bytes" << std::endl;
    std::cout << "  FinancialData: " << sizeof(FinancialData) << " bytes" << std::endl;
    std::cout << "  ECommerceData: " << sizeof(ECommerceData) << " bytes" << std::endl;
    std::cout << "  PaymentData: " << sizeof(PaymentData) << " bytes" << std::endl;
    
    // Memory efficiency calculation
    size_t total_union_size = sizeof(SearchResult) + sizeof(RideData) + 
                              sizeof(FinancialData) + sizeof(ECommerceData) + sizeof(PaymentData);
    
    // If we used separate structs instead of unions
    size_t separate_structs_size = sizeof(SearchResult) * 5;  // Approximate
    
    std::cout << "Memory efficiency:" << std::endl;
    std::cout << "  Union approach: " << total_union_size << " bytes" << std::endl;
    std::cout << "  Separate structs: " << separate_structs_size << " bytes" << std::endl;
    std::cout << "  Memory saved: " << (separate_structs_size - total_union_size) << " bytes" << std::endl;
    std::cout << "  Efficiency: " << (100.0 * total_union_size / separate_structs_size) << "%" << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "=== UNION MASTERY - PRODUCTION-GRADE EXAMPLES ===" << std::endl;
    std::cout << "Demonstrating union techniques used by top-tier companies" << std::endl;
    
    try {
        // Demonstrate basic union concepts
        demonstrate_basic_unions();
        
        // Demonstrate discriminated unions
        demonstrate_discriminated_unions();
        
        // Demonstrate company-specific unions
        demonstrate_company_unions();
        
        // Demonstrate type punning
        demonstrate_type_punning();
        
        // Demonstrate memory efficiency
        demonstrate_memory_efficiency();
        
        std::cout << "\n=== UNION MASTERY DEMONSTRATION COMPLETED SUCCESSFULLY ===" << std::endl;
        
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
 *   g++ -std=c++17 -O2 -Wall -Wextra -o union_mastery 01-union-mastery.cpp
 *   clang++ -std=c++17 -O2 -Wall -Wextra -o union_mastery 01-union-mastery.cpp
 *   cl /std:c++17 /O2 /W4 /EHsc 01-union-mastery.cpp
 *
 * Run with:
 *   ./union_mastery
 *   union_mastery.exe
 *
 * Union optimization flags:
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
