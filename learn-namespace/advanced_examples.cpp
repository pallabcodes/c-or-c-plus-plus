/**
 * Advanced Namespace Examples - JavaScript/TypeScript Developer Edition
 *
 * Building on basic concepts, this file shows advanced namespace patterns.
 * Think of these as:
 * - Nested modules: bloomberg.analytics.risk (like lodash.string.upperCase)
 * - Private modules: Anonymous namespaces (like internal helper modules)
 * - Module aliases: import * as bbg from 'bloomberg' (like namespace aliases)
 *
 * Key advanced concepts:
 * - Nested namespaces: Hierarchical organization like file system paths
 * - Anonymous namespaces: File-private code (like non-exported module members)
 * - Namespace aliases: Shortcuts for long paths (like import aliases)
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// =============================================================================
// 1. NESTED NAMESPACES - TRADITIONAL APPROACH
// =============================================================================
// In JS/TS: This is like nested module structure
// const bloomberg = {
//   analytics: {
//     risk: { ValueAtRiskCalculator: ... },
//     pricing: { options: { BlackScholes: ... } }
//   }
// };

namespace bloomberg {
    // First level: bloomberg (like root module)
    namespace analytics {
        // Second level: bloomberg.analytics (like submodule)
        namespace risk {
            class ValueAtRisk {
            public:
                ValueAtRisk(double confidence) : confidence_(confidence) {}

                double calculate(const std::vector<double>& returns) {
                    // Simplified VaR calculation
                    double mean = 0.0;
                    for (double ret : returns) mean += ret;
                    mean /= returns.size();

                    double variance = 0.0;
                    for (double ret : returns) {
                        double diff = ret - mean;
                        variance += diff * diff;
                    }
                    variance /= returns.size();

                    // VaR approximation using normal distribution
                    return mean - confidence_ * sqrt(variance);
                }

            private:
                double confidence_;
            };

            void printRiskMetrics(const ValueAtRisk& var) {
                std::cout << "VaR calculated with " << var.confidence_ * 100
                          << "% confidence" << std::endl;
            }
        }

        namespace pricing {
            class BlackScholes {
            public:
                static double callPrice(double spot, double strike,
                                      double time, double rate, double vol) {
                    // Simplified Black-Scholes (actual implementation would be more complex)
                    double d1 = (log(spot/strike) + (rate + vol*vol/2)*time) / (vol*sqrt(time));
                    double d2 = d1 - vol*sqrt(time);
                    return spot * normCdf(d1) - strike * exp(-rate*time) * normCdf(d2);
                }

            private:
                static double normCdf(double x) {
                    // Simplified normal CDF approximation
                    return 0.5 * (1.0 + erf(x / sqrt(2.0)));
                }
            };
        }
    }
}

// =============================================================================
// 2. NESTED NAMESPACES - C++17 CONCISE SYNTAX
// =============================================================================
// In JS/TS: ES6 modules don't have nested syntax, but conceptually:
// import { SmartOrderRouter } from 'bloomberg/trading/execution';
//
// C++17 allows: namespace bloomberg::trading::execution { ... }
// Instead of: namespace bloomberg { namespace trading { namespace execution { ... } } }

namespace bloomberg::trading::execution {
    // C++17 syntax: bloomberg::trading::execution all at once
    // Much cleaner than traditional nested namespace declarations!
    class SmartOrderRouter {
    public:
        enum class Venue { NYSE, NASDAQ, LSE, TSE };

        SmartOrderRouter(std::string symbol) : symbol_(symbol) {}

        void routeOrder(Venue venue, int quantity, double price) {
            std::cout << "Routing " << quantity << " shares of " << symbol_
                      << " to " << venueToString(venue)
                      << " at $" << price << std::endl;
        }

    private:
        std::string venueToString(Venue venue) {
            switch (venue) {
                case Venue::NYSE: return "NYSE";
                case Venue::NASDAQ: return "NASDAQ";
                case Venue::LSE: return "LSE";
                case Venue::TSE: return "TSE";
                default: return "UNKNOWN";
            }
        }

        std::string symbol_;
    };

    namespace algorithms {
        class VWAP {
        public:
            void executeOrder(int totalQuantity) {
                std::cout << "Executing VWAP order for " << totalQuantity
                          << " shares using volume profile" << std::endl;
            }
        };

        class TWAP {
        public:
            void executeOrder(int totalQuantity, double timeHorizon) {
                std::cout << "Executing TWAP order for " << totalQuantity
                          << " shares over " << timeHorizon << " hours" << std::endl;
            }
        };
    }
}

// =============================================================================
// 3. ANONYMOUS NAMESPACES - FILE-LOCAL LINKAGE
// =============================================================================
// In JS/TS: This is like having non-exported functions/classes in a module
// Only accessible within the same file (translation unit)
//
// In JS/TS:
// // file1.js
// function helper() { return 42; }  // Not exported, file-local
// export function publicFunction() { return helper(); }
//
// // file2.js
// // Cannot access helper() from file1.js
//
// In C++: Anonymous namespace provides the same file-local behavior

namespace {
    // Everything in anonymous namespace has internal linkage
    // Like non-exported members of a JS/TS module
    // These are only visible within this translation unit

    class FileLocalHelper {
    public:
        static std::string formatCurrency(double amount) {
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "$%.2f", amount);
            return buffer;
        }

        static std::string formatPercentage(double value) {
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%.2f%%", value * 100.0);
            return buffer;
        }
    };

    // Anonymous namespace variables have internal linkage
    const double DEFAULT_CONFIDENCE_LEVEL = 0.95;
    const int MAX_RETRY_ATTEMPTS = 3;

    // Helper function for internal use only
    bool validateOrderParameters(double price, int quantity) {
        return price > 0.0 && quantity > 0 && quantity < 1000000;
    }
}

// =============================================================================
// 4. NAMESPACE ALIASES
// =============================================================================
// In JS/TS: This is like import aliases
// import { Security } from 'bloomberg' as bbg;
// import * as risk from 'bloomberg/analytics/risk';
//
// Namespace aliases create shortcuts for long namespace paths
// Very useful for deep namespace hierarchies!

namespace bbg = bloomberg;                    // Alias for root namespace
namespace risk = bloomberg::analytics::risk; // Alias for deep namespace
namespace pricing = bloomberg::analytics::pricing;
namespace trading = bloomberg::trading;
namespace execution = bloomberg::trading::execution;
namespace algos = bloomberg::trading::execution::algorithms;

// Multiple aliases for the same namespace (common in large codebases)
// Like having different import names for the same module
namespace blp = bloomberg;   // Alternative Bloomberg alias
namespace bberg = bloomberg; // Another alternative

// =============================================================================
// 5. COMPLEX HIERARCHY WITH ALIASES
// =============================================================================

namespace bloomberg::data::market {
    class RealTimeFeed {
    public:
        void subscribe(const std::string& symbol) {
            std::cout << "Subscribed to real-time feed for " << symbol << std::endl;
        }

        void unsubscribe(const std::string& symbol) {
            std::cout << "Unsubscribed from real-time feed for " << symbol << std::endl;
        }
    };
}

namespace market_data = bloomberg::data::market;

// =============================================================================
// 6. DEMONSTRATION FUNCTIONS
// =============================================================================

void demonstrate_nested_namespaces() {
    std::cout << "\n=== Nested Namespace Access ===\n";

    // Traditional nested access
    bloomberg::analytics::risk::ValueAtRisk var(DEFAULT_CONFIDENCE_LEVEL);
    std::vector<double> returns = {-0.02, 0.01, -0.005, 0.015, -0.01};
    double riskValue = var.calculate(returns);
    std::cout << "Value at Risk: " << FileLocalHelper::formatCurrency(riskValue) << std::endl;

    // Black-Scholes pricing
    double callPrice = bloomberg::analytics::pricing::BlackScholes::callPrice(
        100.0, 105.0, 0.5, 0.05, 0.2
    );
    std::cout << "Call option price: " << FileLocalHelper::formatCurrency(callPrice) << std::endl;
}

void demonstrate_namespace_aliases() {
    std::cout << "\n=== Namespace Aliases ===\n";

    // Using aliases makes code more readable
    risk::ValueAtRisk var(DEFAULT_CONFIDENCE_LEVEL);
    pricing::BlackScholes bs;

    // Trading system using aliases
    execution::SmartOrderRouter router("AAPL");
    router.routeOrder(execution::SmartOrderRouter::Venue::NASDAQ, 1000, 150.25);

    algos::VWAP vwapAlgo;
    vwapAlgo.executeOrder(50000);

    algos::TWAP twapAlgo;
    twapAlgo.executeOrder(10000, 2.5);
}

void demonstrate_anonymous_namespace() {
    std::cout << "\n=== Anonymous Namespace (File-Local) ===\n";

    // These functions are only accessible within this file
    std::cout << "Formatted currency: " << FileLocalHelper::formatCurrency(1234.56) << std::endl;
    std::cout << "Formatted percentage: " << FileLocalHelper::formatPercentage(0.1234) << std::endl;
    std::cout << "Default confidence: " << FileLocalHelper::formatPercentage(DEFAULT_CONFIDENCE_LEVEL) << std::endl;

    // Validate some parameters
    bool isValid = validateOrderParameters(150.25, 100);
    std::cout << "Order parameters valid: " << (isValid ? "YES" : "NO") << std::endl;
}

void demonstrate_market_data() {
    std::cout << "\n=== Market Data with Alias ===\n";

    market_data::RealTimeFeed feed;
    feed.subscribe("AAPL");
    feed.subscribe("GOOGL");
    feed.unsubscribe("AAPL");
}

// =============================================================================
// 7. ADVANCED USAGE PATTERNS
// =============================================================================

namespace bloomberg::utils {
    // Utility functions in their own namespace
    template<typename T>
    T clamp(T value, T min, T max) {
        return (value < min) ? min : (value > max) ? max : value;
    }

    namespace string {
        std::string toUpper(const std::string& str);
        std::string toLower(const std::string& str);
        std::string trim(const std::string& str);
    }

    namespace datetime {
        class Timestamp {
        public:
            Timestamp() : timestamp_(std::time(nullptr)) {}
            std::string toString() const;
        private:
            std::time_t timestamp_;
        };
    }
}

namespace utils = bloomberg::utils;
namespace string_utils = bloomberg::utils::string;
namespace datetime_utils = bloomberg::utils::datetime;

void demonstrate_utility_namespaces() {
    std::cout << "\n=== Utility Namespace Patterns ===\n";

    // Using utility functions
    double clampedValue = utils::clamp(15.7, 10.0, 20.0);
    std::cout << "Clamped value: " << clampedValue << std::endl;

    // Date/time utilities
    datetime_utils::Timestamp now;
    std::cout << "Current timestamp created" << std::endl;
}

// =============================================================================
// 8. BLOOMBERG-STYLE NAMESPACE ORGANIZATION
// =============================================================================
// Real-world Bloomberg codebase uses hierarchical namespaces like:
// bloomberg::bdem::aggregate
// bloomberg::emsx::api
// bloomberg::dapi::service
//
// This creates clear ownership and prevents naming conflicts
// Like organizing code into folders: bloomberg/bdem/aggregate/

namespace bloomberg::bdem {  // Bloomberg Data Environment - like a department
    namespace aggregate {   // Specific component within BDEM
        class Aggregate {
        public:
            virtual ~Aggregate() = default;  // Virtual destructor - like base class in TS
            virtual void print() const = 0;  // Pure virtual - like abstract method
        };
    }
}

namespace bloomberg::bdlp {  // Bloomberg Data License Platform
    namespace service {
        class EntitlementService {
        public:
            bool hasPermission(const std::string& user, const std::string& resource) {
                // Simplified permission check
                return user.length() > 0 && resource.length() > 0;
            }
        };
    }
}

namespace bloomberg::emsx {  // Execution Management System
    namespace api {
        class OrderManager {
        public:
            void submitOrder(const std::string& symbol, int quantity, double price) {
                std::cout << "EMSX: Submitted order for " << quantity
                          << " " << symbol << " at $" << price << std::endl;
            }
        };
    }
}

// Create convenient aliases (common in Bloomberg codebase)
namespace bdem = bloomberg::bdem;
namespace bdlp = bloomberg::bdlp;
namespace emsx = bloomberg::emsx;

void demonstrate_bloomberg_organization() {
    std::cout << "\n=== Bloomberg-Style Organization ===\n";

    bdem::aggregate::Aggregate* agg = nullptr;  // Would be concrete implementation

    bdlp::service::EntitlementService entitlement;
    bool hasAccess = entitlement.hasPermission("trader1", "market_data");
    std::cout << "Entitlement check: " << (hasAccess ? "GRANTED" : "DENIED") << std::endl;

    emsx::api::OrderManager orderManager;
    orderManager.submitOrder("IBM", 500, 140.25);
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Advanced C++ Namespace Examples\n";
    std::cout << "=================================\n";

    demonstrate_nested_namespaces();
    demonstrate_namespace_aliases();
    demonstrate_anonymous_namespace();
    demonstrate_market_data();
    demonstrate_utility_namespaces();
    demonstrate_bloomberg_organization();

    std::cout << "\n=== Advanced Namespace Takeaways for JS/TS Devs ===\n";
    std::cout << "1. C++17 syntax: namespace A::B::C {} (like cleaner module paths)\n";
    std::cout << "2. Anonymous namespaces: File-private code (like non-exported module members)\n";
    std::cout << "3. Namespace aliases: Shortcuts for deep paths (like import aliases)\n";
    std::cout << "4. Hierarchical organization: Like folder structure in large codebases\n";
    std::cout << "5. Bloomberg pattern: company::department::component (clear ownership)\n";
    std::cout << "6. No 'static' for file-local: Use anonymous namespaces instead\n";

    return 0;
}
