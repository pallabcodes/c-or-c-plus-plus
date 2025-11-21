/**
 * Header Organization Demonstration
 *
 * This file shows how to properly use headers organized with namespaces.
 */

#include "header_organization.h"
#include <iostream>
#include <memory>

// =============================================================================
// 1. USING NAMESPACE LOCALLY (ACCEPTABLE IN IMPLEMENTATION FILES)
// =============================================================================

void demonstrate_local_using() {
    std::cout << "\n=== Local Using Declarations ===\n";

    // Using declarations are OK in local scope in implementation files
    using bloomberg::trading::Order;
    using bloomberg::trading::OrderType;
    using bloomberg::trading::OrderSide;
    using bloomberg::trading::TimeInForce;

    // Create orders using factory functions
    std::unique_ptr<Order> marketOrder(
        bloomberg::trading::createMarketOrder("AAPL", OrderSide::BUY, 100)
    );

    std::unique_ptr<Order> limitOrder(
        bloomberg::trading::createLimitOrder("GOOGL", OrderSide::SELL, 50, 2500.00)
    );

    // Modify orders
    marketOrder->setTimeInForce(TimeInForce::IOC);
    limitOrder->setPrice(2525.00);

    // Display orders
    std::cout << "Market order: " << *marketOrder << std::endl;
    std::cout << "Limit order: " << *limitOrder << std::endl;

    // Business logic
    std::cout << "Market order notional: $" << marketOrder->getNotionalValue() << std::endl;
    std::cout << "Limit order notional: $" << limitOrder->getNotionalValue() << std::endl;
}

// =============================================================================
// 2. FULLY QUALIFIED ACCESS (PREFERRED IN HEADERS)
// =============================================================================

void demonstrate_qualified_access() {
    std::cout << "\n=== Fully Qualified Access ===\n";

    // Always use fully qualified names - no ambiguity, clear dependencies
    bloomberg::trading::Order* order = bloomberg::trading::createMarketOrder(
        "MSFT", bloomberg::trading::OrderSide::BUY, 200
    );

    std::unique_ptr<bloomberg::trading::Order> orderPtr(order);

    // Use fully qualified names for all access
    orderPtr->bloomberg::trading::Order::setPrice(305.50);
    orderPtr->bloomberg::trading::Order::submit();

    std::cout << "Qualified access order: " << *orderPtr << std::endl;
    std::cout << "Is active: " << (orderPtr->isActive() ? "YES" : "NO") << std::endl;
}

// =============================================================================
// 3. NAMESPACE ALIASES FOR READABILITY
// =============================================================================

void demonstrate_namespace_aliases() {
    std::cout << "\n=== Namespace Aliases ===\n";

    // Create convenient aliases (common in large codebases)
    namespace bt = bloomberg::trading;

    // Use aliases to reduce typing while maintaining clarity
    bt::Order* order = bt::createLimitOrder("TSLA", bt::OrderSide::BUY, 75, 800.00);
    std::unique_ptr<bt::Order> orderPtr(order);

    // Still clear where things come from
    bt::submitOrder(order);
    bt::cancelOrder(order);  // Won't cancel since already submitted

    std::cout << "Aliased order: " << *orderPtr << std::endl;
}

// =============================================================================
// 4. PROPER ERROR HANDLING AND RESOURCE MANAGEMENT
// =============================================================================

void demonstrate_resource_management() {
    std::cout << "\n=== Resource Management ===\n";

    // Use RAII for automatic cleanup
    std::unique_ptr<bloomberg::trading::Order> order(
        bloomberg::trading::createMarketOrder("NVDA", bloomberg::trading::OrderSide::SELL, 25)
    );

    // Exception safety: unique_ptr will clean up even if exceptions occur
    try {
        order->submit();
        // ... more processing ...

        if (order->isBuyOrder()) {
            std::cout << "Processing buy order logic..." << std::endl;
        } else {
            std::cout << "Processing sell order logic..." << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error processing order: " << e.what() << std::endl;
        // Order will be automatically cleaned up
    }

    std::cout << "Order processing completed (resources automatically cleaned up)" << std::endl;
}

// =============================================================================
// 5. AVOIDING USING DIRECTIVES IN IMPLEMENTATION FILES
// =============================================================================

// BAD EXAMPLE (commented out to avoid issues):
// void bad_example() {
//     using namespace bloomberg::trading;  // DON'T DO THIS
//     using namespace std;                  // DON'T DO THIS
//
//     // Now all names from both namespaces are visible
//     // Code becomes ambiguous and hard to maintain
//     Order order("SYMBOL", BUY, MARKET, 100);  // What BUY? What MARKET?
// }

// =============================================================================
// 6. FORWARD DECLARATION USAGE
// =============================================================================

namespace client_code {

    // Forward declare to avoid including the full header
    // (In real code, this would be in a separate header)
    namespace bt = bloomberg::trading;

    class OrderProcessor {
    public:
        // Store by pointer to avoid full definition
        OrderProcessor(bt::Order* order) : order_(order) {}

        void process() {
            if (order_) {
                std::cout << "Processing order: " << order_->getSymbol() << std::endl;
                // Access public interface only
            }
        }

    private:
        bt::Order* order_;  // Forward declared, no full include needed
    };

}  // namespace client_code

void demonstrate_forward_declaration() {
    std::cout << "\n=== Forward Declaration Usage ===\n";

    using bt = bloomberg::trading;

    std::unique_ptr<bt::Order> order(bt::createMarketOrder("AMD", bt::OrderSide::BUY, 150));

    // Use in client code without full header knowledge
    client_code::OrderProcessor processor(order.get());
    processor.process();
}

// =============================================================================
// 7. HEADER INCLUDE PATTERNS
// =============================================================================

// Pattern 1: Interface header (minimal includes)
class IOrderBook {
public:
    virtual ~IOrderBook() = default;
    virtual void addOrder(bloomberg::trading::Order* order) = 0;
    virtual void removeOrder(const std::string& orderId) = 0;
};

// Pattern 2: Implementation header (includes what it needs)
#include <unordered_map>

class OrderBook : public IOrderBook {
public:
    void addOrder(bloomberg::trading::Order* order) override {
        if (order) {
            orders_[order->getSymbol()].push_back(order);
        }
    }

    void removeOrder(const std::string& orderId) override {
        // Implementation
    }

private:
    std::unordered_map<std::string, std::vector<bloomberg::trading::Order*>> orders_;
};

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Header Organization and Namespace Usage\n";
    std::cout << "========================================\n";

    demonstrate_local_using();
    demonstrate_qualified_access();
    demonstrate_namespace_aliases();
    demonstrate_resource_management();
    demonstrate_forward_declaration();

    std::cout << "\n=== Header Organization Best Practices ===\n";
    std::cout << "1. Use fully qualified names in headers (never 'using namespace')\n";
    std::cout << "2. Using declarations are OK in implementation files (local scope)\n";
    std::cout << "3. Namespace aliases improve readability for deep hierarchies\n";
    std::cout << "4. Forward declare when possible to reduce compilation dependencies\n";
    std::cout << "5. Use RAII for resource management (unique_ptr, shared_ptr)\n";
    std::cout << "6. Separate interface from implementation headers when beneficial\n";
    std::cout << "7. Include only what you need, prefer forward declarations\n";

    return 0;
}
