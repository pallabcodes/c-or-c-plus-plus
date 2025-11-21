/**
 * Modern C++ Namespace Features
 *
 * This file demonstrates modern C++ features related to namespaces:
 * - C++11: Inline namespaces, strongly-typed enums
 * - C++17: Nested namespace definitions
 * - C++20: Modules (when available)
 * - Other modern features and patterns
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <optional>

// =============================================================================
// C++11: INLINE NAMESPACES FOR API VERSIONING
// =============================================================================

namespace bloomberg {
    inline namespace v1 {

        class API {
        public:
            API() : version_("v1") {}
            virtual ~API() = default;

            virtual std::string getVersion() const {
                return version_;
            }

            virtual void processData(const std::string& data) {
                std::cout << "API v1 processing: " << data << std::endl;
            }

        protected:
            std::string version_;
        };

        // Factory function
        std::unique_ptr<API> createAPI() {
            return std::make_unique<API>();
        }

    }  // namespace v1

    // Non-inline namespace for v2
    namespace v2 {

        class API : public bloomberg::v1::API {
        public:
            API() { version_ = "v2"; }

            void processData(const std::string& data) override {
                std::cout << "API v2 processing with enhanced features: " << data << std::endl;
                // Additional v2 processing
            }

            void newFeature(const std::string& advancedData) {
                std::cout << "API v2 exclusive feature: " << advancedData << std::endl;
            }
        };

        // Overload factory for v2
        std::unique_ptr<API> createAPI() {
            return std::make_unique<API>();
        }

    }  // namespace v2

}  // namespace bloomberg

void demonstrate_inline_namespaces() {
    std::cout << "\n=== C++11 Inline Namespaces ===\n";

    // Uses v1 by default (inline)
    auto api1 = bloomberg::createAPI();
    std::cout << "Default API version: " << api1->getVersion() << std::endl;
    api1->processData("Hello World");

    // Explicit v2 usage
    auto api2 = bloomberg::v2::createAPI();
    std::cout << "Explicit v2 API version: " << api2->getVersion() << std::endl;
    api2->processData("Hello World");
    api2->newFeature("Advanced processing");

    // When ready to migrate to v2, just change 'inline namespace v1' to 'namespace v1'
    // and 'inline namespace v2' to 'inline namespace v2'
}

// =============================================================================
// C++11: STRONGLY-TYPED ENUMS WITH NAMESPACES
// =============================================================================

namespace bloomberg {
    namespace trading {

        // Scoped enum (C++11) - better than unscoped enums
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
            GTC,    // Good Till Cancelled
            IOC,    // Immediate Or Cancel
            FOK     // Fill Or Kill
        };

        class Order {
        public:
            Order(std::string symbol, OrderType type, OrderSide side,
                  int quantity, double price = 0.0)
                : symbol_(std::move(symbol)), type_(type), side_(side),
                  quantity_(quantity), price_(price),
                  tif_(TimeInForce::DAY) {}

            // Accessors with scoped enums
            OrderType getType() const { return type_; }
            OrderSide getSide() const { return side_; }
            TimeInForce getTimeInForce() const { return tif_; }

            void setTimeInForce(TimeInForce tif) { tif_ = tif; }

            void print() const {
                std::cout << "Order{"
                          << "symbol: " << symbol_ << ", "
                          << "type: " << orderTypeToString(type_) << ", "
                          << "side: " << orderSideToString(side_) << ", "
                          << "quantity: " << quantity_ << ", "
                          << "price: $" << price_ << ", "
                          << "tif: " << timeInForceToString(tif_)
                          << "}" << std::endl;
            }

        private:
            std::string symbol_;
            OrderType type_;
            OrderSide side_;
            int quantity_;
            double price_;
            TimeInForce tif_;

            // Helper functions for enum conversion
            static std::string orderTypeToString(OrderType type) {
                switch (type) {
                    case OrderType::MARKET: return "MARKET";
                    case OrderType::LIMIT: return "LIMIT";
                    case OrderType::STOP: return "STOP";
                    case OrderType::STOP_LIMIT: return "STOP_LIMIT";
                    case OrderType::TRAILING_STOP: return "TRAILING_STOP";
                    default: return "UNKNOWN";
                }
            }

            static std::string orderSideToString(OrderSide side) {
                switch (side) {
                    case OrderSide::BUY: return "BUY";
                    case OrderSide::SELL: return "SELL";
                    default: return "UNKNOWN";
                }
            }

            static std::string timeInForceToString(TimeInForce tif) {
                switch (tif) {
                    case TimeInForce::DAY: return "DAY";
                    case TimeInForce::GTC: return "GTC";
                    case TimeInForce::IOC: return "IOC";
                    case TimeInForce::FOK: return "FOK";
                    default: return "UNKNOWN";
                }
            }
        };

    }  // namespace trading
}  // namespace bloomberg

void demonstrate_scoped_enums() {
    std::cout << "\n=== C++11 Scoped Enums ===\n";

    using bloomberg::trading::Order;
    using bloomberg::trading::OrderType;
    using bloomberg::trading::OrderSide;
    using bloomberg::trading::TimeInForce;

    Order order("AAPL", OrderType::LIMIT, OrderSide::BUY, 100, 150.25);
    order.setTimeInForce(TimeInForce::GTC);
    order.print();

    // Scoped enums prevent name conflicts and are more type-safe
    // No implicit conversion to int
    // Must qualify: OrderType::MARKET, not just MARKET
}

// =============================================================================
// C++17: NESTED NAMESPACE DEFINITIONS
// =============================================================================

namespace bloomberg::analytics::risk {  // C++17 concise syntax

    class ValueAtRiskCalculator {
    public:
        struct Parameters {
            double confidenceLevel = 0.95;
            std::size_t lookbackDays = 252;
            std::string method = "Historical";
        };

        ValueAtRiskCalculator(Parameters params = {}) : params_(params) {}

        double calculate(const std::vector<double>& returns) {
            if (returns.empty()) return 0.0;

            // Simplified VaR calculation
            double mean = calculateMean(returns);
            double stddev = calculateStdDev(returns, mean);

            // VaR = mean - z_score * stddev (simplified)
            double z_score = 1.645;  // 95% confidence
            return mean - z_score * stddev;
        }

    private:
        Parameters params_;

        double calculateMean(const std::vector<double>& data) {
            double sum = 0.0;
            for (double val : data) sum += val;
            return sum / data.size();
        }

        double calculateStdDev(const std::vector<double>& data, double mean) {
            double sum_sq = 0.0;
            for (double val : data) {
                double diff = val - mean;
                sum_sq += diff * diff;
            }
            return std::sqrt(sum_sq / data.size());
        }
    };

}  // namespace bloomberg::analytics::risk

namespace bloomberg::analytics::pricing::options {  // Another C++17 nested namespace

    class BlackScholes {
    public:
        static double callPrice(double spot, double strike, double timeToExpiry,
                               double riskFreeRate, double volatility) {
            double d1 = calculateD1(spot, strike, timeToExpiry, riskFreeRate, volatility);
            double d2 = d1 - volatility * std::sqrt(timeToExpiry);

            return spot * normalCdf(d1) -
                   strike * std::exp(-riskFreeRate * timeToExpiry) * normalCdf(d2);
        }

        static double putPrice(double spot, double strike, double timeToExpiry,
                              double riskFreeRate, double volatility) {
            double d1 = calculateD1(spot, strike, timeToExpiry, riskFreeRate, volatility);
            double d2 = d1 - volatility * std::sqrt(timeToExpiry);

            return strike * std::exp(-riskFreeRate * timeToExpiry) * normalCdf(-d2) -
                   spot * normalCdf(-d1);
        }

    private:
        static double calculateD1(double spot, double strike, double time,
                                 double rate, double vol) {
            return (std::log(spot / strike) + (rate + vol * vol / 2.0) * time) /
                   (vol * std::sqrt(time));
        }

        static double normalCdf(double x) {
            // Abramowitz & Stegun approximation
            static const double a1 =  0.254829592;
            static const double a2 = -0.284496736;
            static const double a3 =  1.421413741;
            static const double a4 = -1.453152027;
            static const double a5 =  1.061405429;
            static const double p  =  0.3275911;

            int sign = (x < 0) ? -1 : 1;
            x = std::abs(x) / std::sqrt(2.0);

            double t = 1.0 / (1.0 + p * x);
            double y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * std::exp(-x * x);

            return 0.5 * (1.0 + sign * y);
        }
    };

}  // namespace bloomberg::analytics::pricing::options

void demonstrate_cpp17_nested_namespaces() {
    std::cout << "\n=== C++17 Nested Namespace Definitions ===\n";

    // Traditional verbose way (still valid):
    // bloomberg::analytics::risk::ValueAtRiskCalculator calc;

    // C++17 concise way:
    bloomberg::analytics::risk::ValueAtRiskCalculator varCalc;

    std::vector<double> returns = {-0.02, 0.01, -0.005, 0.015, -0.01, 0.008};
    double var = varCalc.calculate(returns);
    std::cout << "Value at Risk (95%): $" << var * 1000000 << " (portfolio value)" << std::endl;

    // Options pricing
    double callPrice = bloomberg::analytics::pricing::options::BlackScholes::callPrice(
        100.0, 105.0, 0.5, 0.05, 0.2
    );
    double putPrice = bloomberg::analytics::pricing::options::BlackScholes::putPrice(
        100.0, 105.0, 0.5, 0.05, 0.2
    );

    std::cout << "Call option price: $" << callPrice << std::endl;
    std::cout << "Put option price: $" << putPrice << std::endl;
}

// =============================================================================
// C++17: CONSTEXPR AND NAMESPACES
// =============================================================================

namespace bloomberg::constants {  // C++17

    // constexpr variables in namespaces
    constexpr double PI = 3.141592653589793;
    constexpr double E = 2.718281828459045;
    constexpr int FIBONACCI_10 = 55;

    // constexpr functions
    constexpr double degreesToRadians(double degrees) {
        return degrees * PI / 180.0;
    }

    constexpr double radiansToDegrees(double radians) {
        return radians * 180.0 / PI;
    }

    // Financial constants
    namespace finance {
        constexpr double RISK_FREE_RATE = 0.0425;  // 4.25%
        constexpr int TRADING_DAYS_PER_YEAR = 252;
        constexpr double DEFAULT_VOLATILITY = 0.20;  // 20%
    }

}  // namespace bloomberg::constants

void demonstrate_constexpr_namespaces() {
    std::cout << "\n=== C++17 constexpr in Namespaces ===\n";

    constexpr double angle_deg = 90.0;
    constexpr double angle_rad = bloomberg::constants::degreesToRadians(angle_deg);

    std::cout << angle_deg << " degrees = " << angle_rad << " radians" << std::endl;

    // Financial calculations at compile time
    constexpr double annualized_vol = bloomberg::constants::finance::DEFAULT_VOLATILITY *
                                     std::sqrt(bloomberg::constants::finance::TRADING_DAYS_PER_YEAR);

    std::cout << "Annualized volatility: " << annualized_vol * 100 << "%" << std::endl;
}

// =============================================================================
// C++17: STRUCTURED BINDINGS WITH NAMESPACES
// =============================================================================

namespace bloomberg::data {

    struct MarketData {
        std::string symbol;
        double bid, ask, last;
        int volume;
        std::string timestamp;
    };

    // Function returning structured data
    MarketData getMarketData(const std::string& symbol) {
        // Mock data
        return {
            symbol,
            150.25, 150.30, 150.27,
            1000000,
            "2024-01-15 14:30:00"
        };
    }

    // Function with multiple return values (using tuple)
    std::tuple<double, double, double> calculateReturns(
        const std::vector<double>& prices) {

        if (prices.size() < 2) {
            return {0.0, 0.0, 0.0};
        }

        double total_return = (prices.back() - prices.front()) / prices.front();
        double annualized_return = total_return * (252.0 / (prices.size() - 1));

        // Calculate volatility (simplified)
        double volatility = 0.0;
        for (size_t i = 1; i < prices.size(); ++i) {
            double daily_return = (prices[i] - prices[i-1]) / prices[i-1];
            volatility += daily_return * daily_return;
        }
        volatility = std::sqrt(volatility / (prices.size() - 1)) * std::sqrt(252.0);

        return {total_return, annualized_return, volatility};
    }

}  // namespace bloomberg::data

void demonstrate_structured_bindings() {
    std::cout << "\n=== C++17 Structured Bindings ===\n";

    // Structured binding with namespace-qualified function
    auto [symbol, bid, ask, last, volume, timestamp] =
        bloomberg::data::getMarketData("AAPL");

    std::cout << "Market Data for " << symbol << ":" << std::endl;
    std::cout << "  Bid: $" << bid << std::endl;
    std::cout << "  Ask: $" << ask << std::endl;
    std::cout << "  Last: $" << last << std::endl;
    std::cout << "  Volume: " << volume << std::endl;
    std::cout << "  Timestamp: " << timestamp << std::endl;

    // Multiple return values
    std::vector<double> prices = {100, 102, 98, 105, 103, 108};
    auto [total_ret, ann_ret, vol] = bloomberg::data::calculateReturns(prices);

    std::cout << "Performance Analysis:" << std::endl;
    std::cout << "  Total Return: " << total_ret * 100 << "%" << std::endl;
    std::cout << "  Annualized Return: " << ann_ret * 100 << "%" << std::endl;
    std::cout << "  Volatility: " << vol * 100 << "%" << std::endl;
}

// =============================================================================
// MODERN C++: OPTIONAL AND VARIANT WITH NAMESPACES
// =============================================================================

namespace bloomberg::trading::orders {

    enum class OrderStatus {
        PENDING,
        FILLED,
        PARTIAL_FILL,
        CANCELLED,
        REJECTED
    };

    struct OrderResult {
        std::string orderId;
        OrderStatus status;
        std::optional<int> filledQuantity;
        std::optional<double> averagePrice;
        std::optional<std::string> errorMessage;
    };

    class OrderManager {
    public:
        std::optional<OrderResult> submitOrder(const std::string& symbol,
                                             int quantity,
                                             double price) {
            // Simulate order processing
            if (quantity <= 0 || price <= 0) {
                return OrderResult{
                    "INVALID",
                    OrderStatus::REJECTED,
                    std::nullopt,
                    std::nullopt,
                    "Invalid quantity or price"
                };
            }

            // Simulate successful order
            return OrderResult{
                generateOrderId(),
                OrderStatus::PENDING,
                std::nullopt,
                std::nullopt,
                std::nullopt
            };
        }

        std::optional<OrderResult> getOrderStatus(const std::string& orderId) {
            // Mock implementation
            if (orderId == "ORD001") {
                return OrderResult{
                    orderId,
                    OrderStatus::FILLED,
                    100,
                    150.25,
                    std::nullopt
                };
            }
            return std::nullopt;  // Order not found
        }

    private:
        std::string generateOrderId() {
            static int counter = 1;
            return "ORD" + std::to_string(counter++);
        }
    };

}  // namespace bloomberg::trading::orders

void demonstrate_optional_and_modern_features() {
    std::cout << "\n=== Modern C++ Features in Namespaces ===\n";

    bloomberg::trading::orders::OrderManager manager;

    // Using optional for potentially missing results
    auto result1 = manager.submitOrder("AAPL", 100, 150.25);
    if (result1) {
        std::cout << "Order submitted: " << result1->orderId
                  << " (Status: " << static_cast<int>(result1->status) << ")" << std::endl;
    }

    auto result2 = manager.submitOrder("AAPL", -50, 150.25);
    if (result2 && result2->errorMessage) {
        std::cout << "Order failed: " << *result2->errorMessage << std::endl;
    }

    auto status = manager.getOrderStatus("ORD001");
    if (status && status->filledQuantity && status->averagePrice) {
        std::cout << "Order ORD001 filled: " << *status->filledQuantity
                  << " shares at $" << *status->averagePrice << std::endl;
    }
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Modern C++ Namespace Features\n";
    std::cout << "=============================\n";

    demonstrate_inline_namespaces();
    demonstrate_scoped_enums();
    demonstrate_cpp17_nested_namespaces();
    demonstrate_constexpr_namespaces();
    demonstrate_structured_bindings();
    demonstrate_optional_and_modern_features();

    std::cout << "\n=== Modern C++ Namespace Features Summary ===\n";
    std::cout << "C++11:\n";
    std::cout << "  • Inline namespaces for API versioning\n";
    std::cout << "  • Scoped enums (enum class) for type safety\n";
    std::cout << "  • Strongly-typed enumerations\n";

    std::cout << "\nC++17:\n";
    std::cout << "  • Nested namespace definitions (namespace A::B::C)\n";
    std::cout << "  • constexpr variables and functions in namespaces\n";
    std::cout << "  • Structured bindings with namespace functions\n";

    std::cout << "\nModern Patterns:\n";
    std::cout << "  • Optional<T> for potentially missing values\n";
    std::cout << "  • Smart pointers with namespaces\n";
    std::cout << "  • Type-safe programming practices\n";
    std::cout << "  • API design with backward compatibility\n";

    return 0;
}
