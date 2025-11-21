#ifndef BLOOMBERG_TRADING_ORDER_H
#define BLOOMBERG_TRADING_ORDER_H

#include <string>
#include <memory>
#include <vector>

namespace bloomberg {
namespace trading {

// Forward declarations to minimize includes
class Portfolio;
class Account;
class ExecutionReport;

// Enums in namespace scope
enum class OrderType {
    MARKET,
    LIMIT,
    STOP,
    STOP_LIMIT,
    TRAILING_STOP
};

enum class OrderSide {
    BUY,
    SELL
};

enum class TimeInForce {
    DAY,
    GTC,      // Good Till Cancelled
    IOC,      // Immediate Or Cancel
    FOK       // Fill Or Kill
};

// Main class declaration
class Order {
public:
    // Constructor
    Order(const std::string& symbol,
          OrderSide side,
          OrderType type,
          int quantity,
          double price = 0.0);

    // Destructor
    ~Order();

    // Copy/Move operations
    Order(const Order& other);
    Order& operator=(const Order& other);
    Order(Order&& other) noexcept;
    Order& operator=(Order&& other) noexcept;

    // Accessors
    const std::string& getSymbol() const { return symbol_; }
    OrderSide getSide() const { return side_; }
    OrderType getType() const { return type_; }
    int getQuantity() const { return quantity_; }
    double getPrice() const { return price_; }
    TimeInForce getTimeInForce() const { return tif_; }

    // Modifiers
    void setQuantity(int quantity) { quantity_ = quantity; }
    void setPrice(double price) { price_ = price; }
    void setTimeInForce(TimeInForce tif) { tif_ = tif; }

    // Business logic
    bool isBuyOrder() const { return side_ == OrderSide::BUY; }
    bool isSellOrder() const { return side_ == OrderSide::SELL; }
    double getNotionalValue() const { return quantity_ * price_; }

    // State management
    void submit();
    void cancel();
    bool isActive() const;
    bool isFilled() const;

private:
    std::string symbol_;
    OrderSide side_;
    OrderType type_;
    int quantity_;
    double price_;
    TimeInForce tif_;
    bool submitted_;
    bool cancelled_;

    // Forward declared dependencies
    std::shared_ptr<Portfolio> portfolio_;
    std::shared_ptr<Account> account_;
};

// Free functions in same namespace
Order* createMarketOrder(const std::string& symbol,
                        OrderSide side,
                        int quantity);

Order* createLimitOrder(const std::string& symbol,
                       OrderSide side,
                       int quantity,
                       double limitPrice);

void submitOrder(Order* order);
void cancelOrder(Order* order);

// Utility functions
std::string orderTypeToString(OrderType type);
std::string orderSideToString(OrderSide side);
std::string timeInForceToString(TimeInForce tif);

}  // namespace trading
}  // namespace bloomberg

#endif  // BLOOMBERG_TRADING_ORDER_H
