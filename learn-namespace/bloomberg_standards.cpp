/**
 * Bloomberg-Style Namespace Coding Standards
 *
 * This file demonstrates Bloomberg-level namespace usage patterns,
 * coding standards, and best practices for large-scale C++ development.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// =============================================================================
// BLOOMBERG-STYLE NAMESPACE HIERARCHY
// =============================================================================

// Top-level namespace (company)
namespace bloomberg {

// Core infrastructure namespaces
namespace bsl {        // Bloomberg Standard Library
    // Core utilities, containers, algorithms
}

namespace bdl {        // Bloomberg Development Library
    // Development tools, testing, utilities
}

namespace bdem {       // Bloomberg Data Environment
    // Data structures, serialization
}

// Business domain namespaces
namespace emsx {       // Execution Management System
    // Trading execution functionality
}

namespace dapi {       // Data API
    // Market data interfaces
}

namespace bpipe {      // Bloomberg Pipeline
    // Data processing pipeline
}

// =============================================================================
// BSL - BLOOMBERG STANDARD LIBRARY STYLE
// =============================================================================

namespace bloomberg {
namespace bsl {

namespace algorithm {

// Bloomberg-style function with ADL-friendly placement
template<typename InputIt, typename UnaryPredicate>
InputIt find_if(InputIt first, InputIt last, UnaryPredicate pred) {
    for (; first != last; ++first) {
        if (pred(*first)) {
            return first;
        }
    }
    return last;
}

// Overload for Bloomberg custom iterators
template<typename BloombergIterator, typename UnaryPredicate>
BloombergIterator find_if(BloombergIterator first, BloombergIterator last, UnaryPredicate pred) {
    // Custom implementation for Bloomberg iterators
    return first;  // Simplified
}

}  // namespace algorithm

namespace container {

// Bloomberg-style vector wrapper
template<typename T>
class Vector {
public:
    using value_type = T;
    using size_type = std::size_t;

    explicit Vector(size_type capacity = 0) : data_(capacity) {}

    void push_back(const T& value) { data_.push_back(value); }
    void push_back(T&& value) { data_.push_back(std::move(value)); }

    size_type size() const { return data_.size(); }
    const T& operator[](size_type index) const { return data_[index]; }
    T& operator[](size_type index) { return data_[index]; }

private:
    std::vector<T> data_;
};

}  // namespace container

}  // namespace bsl
}  // namespace bloomberg

// =============================================================================
// BDEM - BLOOMBERG DATA ENVIRONMENT STYLE
// =============================================================================

namespace bloomberg {
namespace bdem {

// Forward declarations
class Aggregate;
class Choice;
class Sequence;

// Base class for all BDEM types
class BdemType {
public:
    virtual ~BdemType() = default;
    virtual void print() const = 0;
    virtual std::unique_ptr<BdemType> clone() const = 0;
};

// Aggregate (struct-like)
class Aggregate : public BdemType {
public:
    void addField(const std::string& name, std::unique_ptr<BdemType> field) {
        fields_[name] = std::move(field);
    }

    const BdemType* getField(const std::string& name) const {
        auto it = fields_.find(name);
        return it != fields_.end() ? it->second.get() : nullptr;
    }

    void print() const override {
        std::cout << "Aggregate{";
        for (const auto& pair : fields_) {
            std::cout << pair.first << ": ";
            if (pair.second) pair.second->print();
            std::cout << ", ";
        }
        std::cout << "}";
    }

    std::unique_ptr<BdemType> clone() const override {
        auto copy = std::make_unique<Aggregate>();
        for (const auto& pair : fields_) {
            if (pair.second) {
                copy->addField(pair.first, pair.second->clone());
            }
        }
        return copy;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<BdemType>> fields_;
};

// Choice (variant-like)
class Choice : public BdemType {
public:
    void setSelection(const std::string& selectionName,
                     std::unique_ptr<BdemType> value) {
        selectionName_ = selectionName;
        value_ = std::move(value);
    }

    void print() const override {
        std::cout << "Choice{" << selectionName_ << ": ";
        if (value_) value_->print();
        std::cout << "}";
    }

    std::unique_ptr<BdemType> clone() const override {
        auto copy = std::make_unique<Choice>();
        if (value_) {
            copy->setSelection(selectionName_, value_->clone());
        }
        return copy;
    }

private:
    std::string selectionName_;
    std::unique_ptr<BdemType> value_;
};

}  // namespace bdem
}  // namespace bloomberg

// =============================================================================
// EMSX - EXECUTION MANAGEMENT SYSTEM STYLE
// =============================================================================

namespace bloomberg {
namespace emsx {
namespace api {

// EMSX API interfaces
class Order {
public:
    virtual ~Order() = default;
    virtual std::string getId() const = 0;
    virtual void execute() = 0;
};

class MarketOrder : public Order {
public:
    MarketOrder(const std::string& id, const std::string& symbol, int quantity)
        : id_(id), symbol_(symbol), quantity_(quantity) {}

    std::string getId() const override { return id_; }

    void execute() override {
        std::cout << "EMSX: Executing market order " << id_
                  << " for " << quantity_ << " " << symbol_ << std::endl;
    }

private:
    std::string id_, symbol_;
    int quantity_;
};

class OrderManager {
public:
    void submitOrder(std::unique_ptr<Order> order) {
        if (order) {
            std::string id = order->getId();
            orders_[id] = std::move(order);
            orders_[id]->execute();
        }
    }

    Order* getOrder(const std::string& id) {
        auto it = orders_.find(id);
        return it != orders_.end() ? it->second.get() : nullptr;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Order>> orders_;
};

}  // namespace api
}  // namespace emsx
}  // namespace bloomberg

// =============================================================================
// DAPI - DATA API STYLE
// =============================================================================

namespace bloomberg {
namespace dapi {

// Data subscription types
enum class SubscriptionType {
    SNAPSHOT,
    STREAMING,
    HISTORICAL
};

class Subscription {
public:
    Subscription(const std::string& symbol, SubscriptionType type)
        : symbol_(symbol), type_(type), active_(false) {}

    virtual ~Subscription() = default;

    void activate() { active_ = true; }
    void deactivate() { active_ = false; }
    bool isActive() const { return active_; }

    const std::string& getSymbol() const { return symbol_; }
    SubscriptionType getType() const { return type_; }

private:
    std::string symbol_;
    SubscriptionType type_;
    bool active_;
};

class MarketDataFeed {
public:
    void subscribe(std::shared_ptr<Subscription> sub) {
        if (sub) {
            subscriptions_[sub->getSymbol()] = sub;
            sub->activate();
            std::cout << "DAPI: Subscribed to " << sub->getSymbol() << std::endl;
        }
    }

    void unsubscribe(const std::string& symbol) {
        auto it = subscriptions_.find(symbol);
        if (it != subscriptions_.end()) {
            it->second->deactivate();
            subscriptions_.erase(it);
            std::cout << "DAPI: Unsubscribed from " << symbol << std::endl;
        }
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Subscription>> subscriptions_;
};

}  // namespace dapi
}  // namespace bloomberg

// =============================================================================
// BLOOMBERG CODING STANDARDS DEMONSTRATION
// =============================================================================

void demonstrate_bloomberg_standards() {
    std::cout << "\n=== Bloomberg Namespace Standards ===\n";

    // BSL usage - standard library replacements
    bloomberg::bsl::container::Vector<std::string> securities;
    securities.push_back("AAPL");
    securities.push_back("GOOGL");
    securities.push_back("MSFT");

    std::cout << "BSL Vector contents: ";
    for (bloomberg::bsl::container::Vector<std::string>::size_type i = 0; i < securities.size(); ++i) {
        std::cout << securities[i] << " ";
    }
    std::cout << std::endl;

    // BDEM usage - data structures
    auto aggregate = std::make_unique<bloomberg::bdem::Aggregate>();
    aggregate->addField("symbol", std::make_unique<bloomberg::bdem::Aggregate>());
    aggregate->print();
    std::cout << std::endl;

    // EMSX usage - trading systems
    bloomberg::emsx::api::OrderManager orderManager;
    auto order = std::make_unique<bloomberg::emsx::api::MarketOrder>("ORD001", "TSLA", 100);
    orderManager.submitOrder(std::move(order));

    // DAPI usage - market data
    bloomberg::dapi::MarketDataFeed feed;
    auto subscription = std::make_shared<bloomberg::dapi::Subscription>(
        "NVDA", bloomberg::dapi::SubscriptionType::STREAMING
    );
    feed.subscribe(subscription);
}

// =============================================================================
// BEST PRACTICES DEMONSTRATION
// =============================================================================

// 1. Prefer namespace aliases for readability
namespace bsl = bloomberg::bsl;
namespace bdem = bloomberg::bdem;
namespace emsx = bloomberg::emsx;
namespace dapi = bloomberg::dapi;

void demonstrate_namespace_aliases() {
    std::cout << "\n=== Namespace Aliases (Bloomberg Style) ===\n";

    // Use aliases throughout Bloomberg codebase for brevity
    bsl::container::Vector<int> data;
    data.push_back(1);
    data.push_back(2);
    data.push_back(3);

    auto agg = std::make_unique<bdem::Aggregate>();
    agg->print();
    std::cout << std::endl;
}

// 2. ADL-friendly operator placement
namespace bloomberg {
namespace math {

class Complex {
public:
    Complex(double real = 0.0, double imag = 0.0) : real_(real), imag_(imag) {}
    double real() const { return real_; }
    double imag() const { return imag_; }

private:
    double real_, imag_;
};

// Operators in same namespace as class (ADL-friendly)
Complex operator+(const Complex& a, const Complex& b) {
    return Complex(a.real() + b.real(), a.imag() + b.imag());
}

std::ostream& operator<<(std::ostream& os, const Complex& c) {
    os << "(" << c.real() << ", " << c.imag() << ")";
    return os;
}

}  // namespace math
}  // namespace bloomberg

void demonstrate_adl_best_practices() {
    std::cout << "\n=== ADL Best Practices ===\n";

    bloomberg::math::Complex a(1.0, 2.0);
    bloomberg::math::Complex b(3.0, 4.0);

    // ADL finds operator+ in bloomberg::math
    bloomberg::math::Complex sum = a + b;

    // ADL finds operator<< in bloomberg::math
    std::cout << "Complex sum: " << sum << std::endl;
}

// 3. Proper header organization (interfaces vs implementation)
namespace bloomberg {
namespace interface {

class IMarketDataProvider {
public:
    virtual ~IMarketDataProvider() = default;
    virtual double getPrice(const std::string& symbol) = 0;
    virtual bool isConnected() const = 0;
};

}  // namespace interface

namespace implementation {

class BloombergMarketDataProvider : public interface::IMarketDataProvider {
public:
    double getPrice(const std::string& symbol) override {
        // Implementation would connect to Bloomberg terminals
        return 100.0 + (rand() % 100);  // Mock implementation
    }

    bool isConnected() const override {
        return true;  // Mock implementation
    }
};

}  // namespace implementation
}  // namespace bloomberg

void demonstrate_interface_separation() {
    std::cout << "\n=== Interface vs Implementation Separation ===\n";

    using bloomberg::interface::IMarketDataProvider;
    using bloomberg::implementation::BloombergMarketDataProvider;

    std::unique_ptr<IMarketDataProvider> provider =
        std::make_unique<BloombergMarketDataProvider>();

    std::cout << "AAPL price: $" << provider->getPrice("AAPL") << std::endl;
    std::cout << "Connected: " << (provider->isConnected() ? "YES" : "NO") << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Bloomberg-Style Namespace Coding Standards\n";
    std::cout << "===========================================\n";

    demonstrate_bloomberg_standards();
    demonstrate_namespace_aliases();
    demonstrate_adl_best_practices();
    demonstrate_interface_separation();

    std::cout << "\n=== Bloomberg Namespace Standards Summary ===\n";
    std::cout << "1. Hierarchical namespace structure reflecting organization\n";
    std::cout << "2. Use namespace aliases for commonly used deep paths\n";
    std::cout << "3. Place operators in same namespace as operands (ADL)\n";
    std::cout << "4. Separate interfaces from implementations\n";
    std::cout << "5. Use fully qualified names in headers, aliases in implementations\n";
    std::cout << "6. Prefer unique_ptr/shared_ptr for resource management\n";
    std::cout << "7. Design for testability and dependency injection\n";
    std::cout << "8. Document namespace purposes and ownership\n";

    return 0;
}
