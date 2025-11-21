/*
 * Conditional Dependency Injection
 * 
 * Source: .NET Core DI, Spring Framework, feature flags
 * Pattern: Conditionally inject dependencies based on conditions
 * 
 * What Makes It Ingenious:
 * - Feature flags: Inject different implementations based on features
 * - Environment-based: Different implementations for dev/prod/test
 * - Configuration-driven: Choose implementation from config
 * - Runtime conditions: Dynamic dependency selection
 * - Used in feature toggles, A/B testing, multi-tenant apps
 * 
 * When to Use:
 * - Feature flags / feature toggles
 * - A/B testing
 * - Environment-specific implementations
 * - Multi-tenant applications
 * - Configuration-driven behavior
 * - Runtime feature selection
 * 
 * Real-World Usage:
 * - .NET Core DI (conditional registration)
 * - Spring Framework (conditional beans)
 * - Feature flag systems
 * - A/B testing frameworks
 * - Multi-tenant SaaS applications
 * 
 * Time Complexity: O(1) for conditional check, O(1) for resolution
 * Space Complexity: O(n) where n is number of conditional services
 */

#include <memory>
#include <functional>
#include <unordered_map>
#include <string>
#include <iostream>
#include <any>
#include <typeindex>

// Condition function type
using Condition = std::function<bool()>;

// Conditional service container
class ConditionalContainer {
private:
    struct ConditionalRegistration {
        Condition condition;
        std::function<std::any()> factory;
        int priority;  // Higher priority wins
        
        ConditionalRegistration(Condition cond, std::function<std::any()> fact, int prio = 0)
            : condition(cond), factory(fact), priority(prio) {}
    };
    
    std::unordered_map<std::type_index, std::vector<ConditionalRegistration>> registrations_;
    
    template<typename T>
    std::type_index get_type_index() {
        return std::type_index(typeid(T));
    }
    
public:
    // Register service with condition
    template<typename TInterface>
    void register_conditional(Condition condition,
                             std::function<std::shared_ptr<TInterface>()> factory,
                             int priority = 0) {
        auto type_idx = get_type_index<TInterface>();
        registrations_[type_idx].emplace_back(
            condition,
            [factory]() -> std::any {
                return std::make_any<std::shared_ptr<TInterface>>(factory());
            },
            priority
        );
        
        // Sort by priority (higher first)
        std::sort(registrations_[type_idx].begin(), registrations_[type_idx].end(),
                 [](const ConditionalRegistration& a, const ConditionalRegistration& b) {
                     return a.priority > b.priority;
                 });
    }
    
    // Register default (no condition, lowest priority)
    template<typename TInterface>
    void register_default(std::function<std::shared_ptr<TInterface>()> factory) {
        register_conditional<TInterface>([]() { return true; }, factory, -1);
    }
    
    // Resolve service (first matching condition)
    template<typename T>
    std::shared_ptr<T> resolve() {
        auto type_idx = get_type_index<T>();
        auto it = registrations_.find(type_idx);
        
        if (it == registrations_.end()) {
            throw std::runtime_error("Service not registered: " + std::string(typeid(T).name()));
        }
        
        // Find first matching condition
        for (const auto& reg : it->second) {
            if (reg.condition()) {
                return std::any_cast<std::shared_ptr<T>>(reg.factory());
            }
        }
        
        throw std::runtime_error("No matching condition for service: " + std::string(typeid(T).name()));
    }
};

// Example: Feature flag service
class FeatureFlags {
private:
    std::unordered_map<std::string, bool> flags_;
    
public:
    void set_flag(const std::string& flag, bool value) {
        flags_[flag] = value;
    }
    
    bool is_enabled(const std::string& flag) const {
        auto it = flags_.find(flag);
        return it != flags_.end() && it->second;
    }
};

// Example: Logger interface and implementations
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const std::string& message) = 0;
};

class ConsoleLogger : public ILogger {
public:
    void log(const std::string& message) override {
        std::cout << "[CONSOLE] " << message << std::endl;
    }
};

class FileLogger : public ILogger {
public:
    void log(const std::string& message) override {
        std::cout << "[FILE] " << message << std::endl;
    }
};

class DatabaseLogger : public ILogger {
public:
    void log(const std::string& message) override {
        std::cout << "[DATABASE] " << message << std::endl;
    }
};

// Example usage
int main() {
    ConditionalContainer container;
    FeatureFlags feature_flags;
    
    // Register conditional services based on feature flags
    container.register_conditional<ILogger>(
        [&feature_flags]() { return feature_flags.is_enabled("use_database_logging"); },
        []() { return std::make_shared<DatabaseLogger>(); },
        10  // High priority
    );
    
    container.register_conditional<ILogger>(
        [&feature_flags]() { return feature_flags.is_enabled("use_file_logging"); },
        []() { return std::make_shared<FileLogger>(); },
        5   // Medium priority
    );
    
    // Default to console logger
    container.register_default<ILogger>([]() {
        return std::make_shared<ConsoleLogger>();
    });
    
    // Test with different feature flag states
    std::cout << "=== Test 1: No flags enabled (default)" << std::endl;
    auto logger1 = container.resolve<ILogger>();
    logger1->log("Default logger");
    
    std::cout << "\n=== Test 2: File logging enabled" << std::endl;
    feature_flags.set_flag("use_file_logging", true);
    auto logger2 = container.resolve<ILogger>();
    logger2->log("File logger");
    
    std::cout << "\n=== Test 3: Database logging enabled (higher priority)" << std::endl;
    feature_flags.set_flag("use_database_logging", true);
    auto logger3 = container.resolve<ILogger>();
    logger3->log("Database logger");
    
    // Environment-based conditional injection
    ConditionalContainer env_container;
    
    #ifdef PRODUCTION
        env_container.register_default<ILogger>([]() {
            return std::make_shared<DatabaseLogger>();
        });
    #elif defined(DEVELOPMENT)
        env_container.register_default<ILogger>([]() {
            return std::make_shared<FileLogger>();
        });
    #else
        env_container.register_default<ILogger>([]() {
            return std::make_shared<ConsoleLogger>();
        });
    #endif
    
    std::cout << "\n=== Environment-based injection" << std::endl;
    auto env_logger = env_container.resolve<ILogger>();
    env_logger->log("Environment logger");
    
    return 0;
}

