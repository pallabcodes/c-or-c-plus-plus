/**
 * Header Organization Implementation
 *
 * This file demonstrates proper implementation file organization
 * corresponding to the header file.
 */

#include "header_organization.h"
#include <iostream>
#include <algorithm>
#include <sstream>

// Include necessary headers that were forward declared
// (In a real implementation, these would be proper includes)
class Portfolio {
public:
    std::string getName() const { return "Default Portfolio"; }
};

class Account {
public:
    double getBalance() const { return 100000.0; }
};

class ExecutionReport {
public:
    void setOrderId(int id) { orderId_ = id; }
    int getOrderId() const { return orderId_; }
private:
    int orderId_;
};

namespace bloomberg {
namespace trading {

// =============================================================================
// ORDER CLASS IMPLEMENTATION
// =============================================================================

Order::Order(const std::string& symbol,
             OrderSide side,
             OrderType type,
             int quantity,
             double price)
    : symbol_(symbol),
      side_(side),
      type_(type),
      quantity_(quantity),
      price_(price),
      tif_(TimeInForce::DAY),
      submitted_(false),
      cancelled_(false),
      portfolio_(std::make_shared<Portfolio>()),
      account_(std::make_shared<Account>()) {
}

Order::~Order() = default;

Order::Order(const Order& other)
    : symbol_(other.symbol_),
      side_(other.side_),
      type_(other.type_),
      quantity_(other.quantity_),
      price_(other.price_),
      tif_(other.tif_),
      submitted_(other.submitted_),
      cancelled_(other.cancelled_),
      portfolio_(other.portfolio_),  // Shared ownership
      account_(other.account_) {     // Shared ownership
}

Order& Order::operator=(const Order& other) {
    if (this != &other) {
        symbol_ = other.symbol_;
        side_ = other.side_;
        type_ = other.type_;
        quantity_ = other.quantity_;
        price_ = other.price_;
        tif_ = other.tif_;
        submitted_ = other.submitted_;
        cancelled_ = other.cancelled_;
        portfolio_ = other.portfolio_;
        account_ = other.account_;
    }
    return *this;
}

Order::Order(Order&& other) noexcept
    : symbol_(std::move(other.symbol_)),
      side_(other.side_),
      type_(other.type_),
      quantity_(std::move(other.quantity_)),
      price_(other.price_),
      tif_(other.tif_),
      submitted_(other.submitted_),
      cancelled_(other.cancelled_),
      portfolio_(std::move(other.portfolio_)),
      account_(std::move(other.account_)) {
    other.submitted_ = false;
    other.cancelled_ = false;
}

Order& Order::operator=(Order&& other) noexcept {
    if (this != &other) {
        symbol_ = std::move(other.symbol_);
        side_ = other.side_;
        type_ = other.type_;
        quantity_ = std::move(other.quantity_);
        price_ = other.price_;
        tif_ = other.tif_;
        submitted_ = other.submitted_;
        cancelled_ = other.cancelled_;
        portfolio_ = std::move(other.portfolio_);
        account_ = std::move(other.account_);

        other.submitted_ = false;
        other.cancelled_ = false;
    }
    return *this;
}

bool Order::isActive() const {
    return submitted_ && !cancelled_ && !isFilled();
}

bool Order::isFilled() const {
    // Simplified: assume all orders are eventually filled
    return false;
}

void Order::submit() {
    if (!submitted_ && !cancelled_) {
        submitted_ = true;
        std::cout << "Order submitted: " << *this << std::endl;
    }
}

void Order::cancel() {
    if (submitted_ && !cancelled_) {
        cancelled_ = true;
        std::cout << "Order cancelled: " << symbol_ << std::endl;
    }
}

// =============================================================================
// FREE FUNCTIONS IMPLEMENTATION
// =============================================================================

Order* createMarketOrder(const std::string& symbol,
                        OrderSide side,
                        int quantity) {
    return new Order(symbol, side, OrderType::MARKET, quantity);
}

Order* createLimitOrder(const std::string& symbol,
                       OrderSide side,
                       int quantity,
                       double limitPrice) {
    return new Order(symbol, side, OrderType::LIMIT, quantity, limitPrice);
}

void submitOrder(Order* order) {
    if (order) {
        order->submit();
    }
}

void cancelOrder(Order* order) {
    if (order) {
        order->cancel();
    }
}

// =============================================================================
// UTILITY FUNCTIONS IMPLEMENTATION
// =============================================================================

std::string orderTypeToString(OrderType type) {
    switch (type) {
        case OrderType::MARKET: return "MARKET";
        case OrderType::LIMIT: return "LIMIT";
        case OrderType::STOP: return "STOP";
        case OrderType::STOP_LIMIT: return "STOP_LIMIT";
        case OrderType::TRAILING_STOP: return "TRAILING_STOP";
        default: return "UNKNOWN";
    }
}

std::string orderSideToString(OrderSide side) {
    switch (side) {
        case OrderSide::BUY: return "BUY";
        case OrderSide::SELL: return "SELL";
        default: return "UNKNOWN";
    }
}

std::string timeInForceToString(TimeInForce tif) {
    switch (tif) {
        case TimeInForce::DAY: return "DAY";
        case TimeInForce::GTC: return "GTC";
        case TimeInForce::IOC: return "IOC";
        case TimeInForce::FOK: return "FOK";
        default: return "UNKNOWN";
    }
}

// =============================================================================
// OUTPUT OPERATOR (ADL-FRIENDLY)
// =============================================================================

std::ostream& operator<<(std::ostream& os, const Order& order) {
    os << "Order{"
       << "symbol: " << order.getSymbol() << ", "
       << "side: " << orderSideToString(order.getSide()) << ", "
       << "type: " << orderTypeToString(order.getType()) << ", "
       << "quantity: " << order.getQuantity() << ", "
       << "price: $" << order.getPrice() << ", "
       << "tif: " << timeInForceToString(order.getTimeInForce())
       << "}";
    return os;
}

}  // namespace trading
}  // namespace bloomberg
