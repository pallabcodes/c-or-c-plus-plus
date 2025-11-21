/**
 * Basic Namespace Examples - JavaScript/TypeScript Developer Edition
 *
 * In JS/TS, you use modules (import/export) and sometimes namespaces.
 * In C++, namespaces are the primary way to organize code and prevent naming conflicts.
 *
 * Think of C++ namespaces as:
 * - ES6 modules: `import { Security } from 'bloomberg'`
 * - TypeScript namespaces: `bloomberg.Security`
 * - A way to avoid polluting the global scope
 *
 * Key differences from JS/TS:
 * - Namespaces are compile-time only (no runtime module loading)
 * - Everything in a namespace is accessible unless made private
 * - No dynamic imports - all accessible at compile time
 */

#include <iostream>
#include <string>

// =============================================================================
// 1. BASIC NAMESPACE DECLARATION AND DEFINITION
// =============================================================================
// In JS/TS: This is like creating a module or namespace
// export namespace bloomberg { ... } in TypeScript
namespace bloomberg {
    // Forward declarations - like TypeScript interface declarations
    // Tells compiler "this class exists, I'll define it later"
    class Security;
    void printSecurityInfo(const Security& sec);

    // Full class definition - like a class in a TypeScript module
    // In JS/TS: export class Security { ... }
    class Security {
    public:
        // Constructor - like a class constructor in JS/TS
        Security(std::string ticker, double price)
            : ticker_(ticker), price_(price) {}

        // Getter methods - like TypeScript getters
        std::string getTicker() const { return ticker_; }
        double getPrice() const { return price_; }
        void setPrice(double price) { price_ = price; }

    private:
        // Private members - like private fields in TypeScript (#ticker_)
        std::string ticker_;
        double price_;
    };

    // Function definition - like an exported function in JS/TS module
    // In JS/TS: export function printSecurityInfo(sec) { ... }
    void printSecurityInfo(const Security& sec) {
        std::cout << "Security: " << sec.getTicker()
                  << ", Price: $" << sec.getPrice() << std::endl;
    }
}

// =============================================================================
// 2. NAMESPACE CAN BE REOPENED (MULTIPLE DECLARATIONS)
// =============================================================================
// In JS/TS: You can add to existing modules by importing and re-exporting
// In C++: You can reopen namespaces to add more content later
// This is like adding more exports to an existing module

namespace bloomberg {
    // Adding more to the existing bloomberg namespace
    // In JS/TS: This is like adding more exports to the same module file

    // Nested namespace - like a sub-module in JS/TS
    // In JS/TS: export namespace trading { ... } inside bloomberg namespace
    namespace trading {
        // enum class - like a TypeScript enum with better type safety
        // In JS/TS: export enum OrderType { MARKET = 0, LIMIT = 1, STOP = 2 }
        // But C++ enums are more type-safe (no implicit conversion to int)
        enum class OrderType { MARKET, LIMIT, STOP };

        // Class in nested namespace - like a class in a sub-module
        class Order {
        public:
            // Constructor with parameters - like TypeScript constructor
            Order(std::string symbol, OrderType type, int quantity)
                : symbol_(symbol), type_(type), quantity_(quantity) {}

            // Method - like a class method in JS/TS
            void execute() {
                std::cout << "Executing " << quantity_ << " shares of " << symbol_;
                // Switch statement - like switch in JS/TS but more strict
                switch (type_) {
                    case OrderType::MARKET:
                        std::cout << " at market price";
                        break;
                    case OrderType::LIMIT:
                        std::cout << " as limit order";
                        break;
                    case OrderType::STOP:
                        std::cout << " as stop order";
                        break;
                }
                std::cout << std::endl;
            }

        private:
            // Private data members - like private fields in TypeScript
            std::string symbol_;
            OrderType type_;
            int quantity_;
        };
    }
}

// =============================================================================
// 3. ACCESSING NAMESPACE MEMBERS
// =============================================================================
// In JS/TS: This is like importing and using things from modules
// import { Security, printSecurityInfo } from 'bloomberg';
// import { Order, OrderType } from 'bloomberg/trading';

void demonstrate_namespace_access() {
    std::cout << "\n=== Namespace Access Examples ===\n";

    // Fully qualified access - like using full module path in JS/TS
    // In JS/TS: const apple = new bloomberg.Security("AAPL", 150.25);
    bloomberg::Security apple("AAPL", 150.25);

    // Calling function from namespace - like calling imported function
    bloomberg::printSecurityInfo(apple);

    // Accessing nested namespace - like accessing sub-module
    // In JS/TS: const order = new bloomberg.trading.Order(...);
    bloomberg::trading::Order order("GOOGL", bloomberg::trading::OrderType::MARKET, 100);
    order.execute();

    // Modify through qualified access - still need full qualification
    apple.setPrice(152.50);
    bloomberg::printSecurityInfo(apple);
}

// =============================================================================
// 4. USING DECLARATIONS (PREFERRED APPROACH)
// =============================================================================
// In JS/TS: This is like named imports
// import { Security, printSecurityInfo } from 'bloomberg';
// import { Order, OrderType } from 'bloomberg/trading';
//
// Using declarations bring specific names into current scope
// This is like importing specific exports from a module

void demonstrate_using_declarations() {
    std::cout << "\n=== Using Declarations (Preferred) ===\n";

    // Bring specific names into scope - like named imports in JS/TS
    // In JS/TS: import { Security } from 'bloomberg';
    using bloomberg::Security;
    using bloomberg::printSecurityInfo;
    using bloomberg::trading::Order;
    using bloomberg::trading::OrderType;

    // Now we can use them without qualification - like using imported names directly
    // In JS/TS: const msft = new Security("MSFT", 305.75);
    Security msft("MSFT", 305.75);
    printSecurityInfo(msft);

    Order limitOrder("TSLA", OrderType::LIMIT, 50);
    limitOrder.execute();

    // Note: We didn't bring trading::OrderType into scope,
    // so we still need to qualify it as bloomberg::trading::OrderType
    // This is selective - only imports what you explicitly name
}

// =============================================================================
// 5. USING DIRECTIVES (USE SPARINGLY)
// =============================================================================
// In JS/TS: This is like wildcard imports (import * from 'module')
// import * as trading from 'bloomberg/trading';
//
// Using directives bring ENTIRE namespace into scope
// This is convenient but dangerous - like wildcard imports

void demonstrate_using_directives() {
    std::cout << "\n=== Using Directives (Use Sparingly) ===\n";

    // Bring entire namespace into scope - like wildcard import in JS/TS
    // In JS/TS: import * as trading from 'bloomberg/trading';
    // Then you can use trading.Order, trading.OrderType, etc.
    using namespace bloomberg::trading;

    // Now all trading namespace members are accessible without qualification
    // This is like accessing everything through the imported namespace object
    Order stopOrder("NVDA", OrderType::STOP, 25);
    stopOrder.execute();

    // DANGER: This can cause name conflicts and make code less clear
    // In JS/TS: import * from 'module' can pollute your scope
    // In C++: using namespace can create ambiguous names

    // Why avoid in headers? It forces this pollution on ALL files that include the header
    // Like if a library did `import * from 'lodash'` in a header file
}

// =============================================================================
// 6. COMBINING APPROACHES
// =============================================================================
// In real code, you'd typically use using declarations at function scope
// This limits the scope pollution - like importing inside a function in JS/TS

namespace client_code {
    // This namespace represents client code that uses the bloomberg library
    // In JS/TS: This would be like a module that imports from bloomberg

    void processSecurities() {
        // Using declarations inside function - limited scope pollution
        // In JS/TS: import { Security, printSecurityInfo } from 'bloomberg'; (inside function)
        using bloomberg::Security;
        using bloomberg::printSecurityInfo;

        // Now we can use them without qualification in this function only
        Security security("IBM", 140.00);
        printSecurityInfo(security);
        security.setPrice(142.25);
        printSecurityInfo(security);
    }
}

// =============================================================================
// 7. DEMONSTRATION OF POTENTIAL NAME CONFLICTS
// =============================================================================
// This shows why namespaces are crucial - they prevent naming conflicts
// In JS/TS: Two libraries might both export a `Security` class
// Without namespaces, you'd have naming collisions

namespace external_library {
    // Another library also has a Security class
    // In JS/TS: export class Security { ... } in external-library module
    class Security {
    public:
        Security(std::string name) : name_(name) {}
        void print() const {
            std::cout << "External Security: " << name_ << std::endl;
        }
    private:
        std::string name_;
    };
}

void demonstrate_name_conflicts() {
    std::cout << "\n=== Name Conflict Resolution ===\n";

    // Without qualification - would be ambiguous!
    // In JS/TS: import { Security } from 'bloomberg';
    // import { Security } from 'external-library';
    // const sec = new Security("TEST");  // Which Security?! ERROR!
    // Security sec("TEST");  // COMPILER ERROR: ambiguous

    // Must qualify to resolve conflict - like using full module paths
    // In JS/TS: const bloombergSec = new bloomberg.Security("AAPL", 150.00);
    bloomberg::Security bloombergSec("AAPL", 150.00);
    external_library::Security externalSec("External Asset");

    bloombergSec.getTicker();  // Bloomberg's Security - has ticker and price
    externalSec.print();       // External library's Security - just has name

    // This is why namespaces are essential in large codebases!
    // They prevent the "which Security do you mean?" problem
}

// =============================================================================
// MAIN FUNCTION - Like a JavaScript/TypeScript main execution
// =============================================================================

int main() {
    std::cout << "C++ Namespace Basic Examples - JS/TS Developer Edition\n";
    std::cout << "======================================================\n";

    // Run our demonstrations - like calling functions in JS/TS
    demonstrate_namespace_access();
    demonstrate_using_declarations();
    demonstrate_using_directives();
    client_code::processSecurities();
    demonstrate_name_conflicts();

    std::cout << "\n=== Key Takeaways for JS/TS Developers ===\n";
    std::cout << "1. Namespaces = ES6 modules with better collision prevention\n";
    std::cout << "2. Fully qualified names = Full import paths for clarity\n";
    std::cout << "3. Using declarations = Named imports (preferred)\n";
    std::cout << "4. Using directives = Wildcard imports (avoid in headers!)\n";
    std::cout << "5. Namespaces can be reopened = Adding exports to existing modules\n";
    std::cout << "6. No runtime module loading - everything resolved at compile time\n";
    std::cout << "7. Everything is 'public' within namespace (unlike JS private fields)\n";

    return 0;  // Like returning from main in JS/TS
}
