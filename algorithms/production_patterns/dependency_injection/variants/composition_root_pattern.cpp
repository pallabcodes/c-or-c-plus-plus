/*
 * Composition Root Pattern - Dependency Injection
 * 
 * Source: Mark Seemann, Dependency Injection Principles
 * Pattern: Centralized dependency configuration at application entry point
 * 
 * What Makes It Ingenious:
 * - Single responsibility: All DI configuration in one place
 * - Application entry point: Configure dependencies at startup
 * - Separation of concerns: Business logic separate from DI setup
 * - Testability: Easy to swap configurations for testing
 * - Used in all major DI frameworks, enterprise applications
 * 
 * When to Use:
 * - Application startup configuration
 * - Centralized dependency management
 * - Different configurations for different environments
 * - Testing with mock configurations
 * - Plugin/module registration
 * 
 * Real-World Usage:
 * - .NET Core (Program.cs, Startup.cs)
 * - Spring Framework (ApplicationContext)
 * - Angular (main.ts, app.module.ts)
 * - ASP.NET Core (Startup.cs)
 * - Enterprise applications
 * 
 * Time Complexity: O(n) where n is number of services to register
 * Space Complexity: O(n) where n is number of registered services
 */

#include <memory>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <typeindex>
#include <any>

// Simple IoC container for demonstration
class Container {
private:
    std::unordered_map<std::type_index, std::function<std::any()>> factories_;
    
public:
    template<typename T>
    void register_factory(std::function<std::shared_ptr<T>()> factory) {
        factories_[std::type_index(typeid(T))] = [factory]() -> std::any {
            return std::make_any<std::shared_ptr<T>>(factory());
        };
    }
    
    template<typename T>
    std::shared_ptr<T> resolve() {
        auto it = factories_.find(std::type_index(typeid(T)));
        if (it == factories_.end()) {
            throw std::runtime_error("Service not registered");
        }
        return std::any_cast<std::shared_ptr<T>>(it->second());
    }
};

// Example services
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const std::string& message) = 0;
};

class ConsoleLogger : public ILogger {
public:
    void log(const std::string& message) override {
        std::cout << "[LOG] " << message << std::endl;
    }
};

class IConfigService {
public:
    virtual ~IConfigService() = default;
    virtual std::string get(const std::string& key) = 0;
};

class ConfigService : public IConfigService {
public:
    std::string get(const std::string& key) override {
        return "value_for_" + key;
    }
};

class UserService {
private:
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IConfigService> config_;
    
public:
    UserService(std::shared_ptr<ILogger> logger, std::shared_ptr<IConfigService> config)
        : logger_(logger), config_(config) {}
    
    void register_user(const std::string& email) {
        logger_->log("Registering user: " + email);
        std::string setting = config_->get("timeout");
        logger_->log("Using config: " + setting);
    }
};

// Composition Root - Centralized dependency configuration
class CompositionRoot {
public:
    // Configure dependencies for production
    static Container configure_production() {
        Container container;
        
        // Register services
        container.register_factory<ILogger>([]() {
            return std::make_shared<ConsoleLogger>();
        });
        
        container.register_factory<IConfigService>([]() {
            return std::make_shared<ConfigService>();
        });
        
        container.register_factory<UserService>([&container]() {
            return std::make_shared<UserService>(
                container.resolve<ILogger>(),
                container.resolve<IConfigService>()
            );
        });
        
        return container;
    }
    
    // Configure dependencies for testing
    static Container configure_testing() {
        Container container;
        
        // Register mock services
        container.register_factory<ILogger>([]() {
            return std::make_shared<ConsoleLogger>();  // Could be mock
        });
        
        container.register_factory<IConfigService>([]() {
            return std::make_shared<ConfigService>();  // Could be mock
        });
        
        container.register_factory<UserService>([&container]() {
            return std::make_shared<UserService>(
                container.resolve<ILogger>(),
                container.resolve<IConfigService>()
            );
        });
        
        return container;
    }
    
    // Configure dependencies for development
    static Container configure_development() {
        Container container;
        
        // Register development services (e.g., with debug logging)
        container.register_factory<ILogger>([]() {
            return std::make_shared<ConsoleLogger>();
        });
        
        container.register_factory<IConfigService>([]() {
            return std::make_shared<ConfigService>();
        });
        
        container.register_factory<UserService>([&container]() {
            return std::make_shared<UserService>(
                container.resolve<ILogger>(),
                container.resolve<IConfigService>()
            );
        });
        
        return container;
    }
};

// Application entry point
int main() {
    // Composition Root - configure dependencies at application entry
    Container container;
    
    // Choose configuration based on environment
    #ifdef PRODUCTION
        container = CompositionRoot::configure_production();
    #elif defined(TESTING)
        container = CompositionRoot::configure_testing();
    #else
        container = CompositionRoot::configure_development();
    #endif
    
    // Application code uses resolved services
    auto user_service = container.resolve<UserService>();
    user_service->register_user("user@example.com");
    
    return 0;
}

