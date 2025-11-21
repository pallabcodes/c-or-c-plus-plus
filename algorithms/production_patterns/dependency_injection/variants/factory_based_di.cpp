/*
 * Factory-Based Dependency Injection
 * 
 * Source: Factory pattern, Abstract Factory, DI frameworks
 * Pattern: Use factories to create objects with dependencies
 * 
 * What Makes It Ingenious:
 * - Factory abstraction: Hide object creation complexity
 * - Dependency injection: Factories inject dependencies
 * - Flexible creation: Different factories for different contexts
 * - Testability: Easy to mock factories
 * - Used in frameworks, libraries, enterprise applications
 * 
 * When to Use:
 * - Complex object creation logic
 * - Need different creation strategies
 * - Testing with mock objects
 * - Plugin architectures
 * - Configuration-driven object creation
 * 
 * Real-World Usage:
 * - Spring Framework (BeanFactory)
 * - .NET Core (IServiceProvider)
 * - Factory pattern implementations
 * - Abstract Factory pattern
 * - Builder pattern with DI
 * 
 * Time Complexity: O(1) for factory creation, O(n) for object creation
 * Space Complexity: O(n) where n is number of factories
 */

#include <memory>
#include <functional>
#include <unordered_map>
#include <string>
#include <iostream>

// Base factory interface
template<typename T>
class IFactory {
public:
    virtual ~IFactory() = default;
    virtual std::unique_ptr<T> create() = 0;
};

// Simple factory implementation
template<typename T, typename... Args>
class Factory : public IFactory<T> {
private:
    std::function<std::unique_ptr<T>(Args...)> factory_func_;
    std::tuple<Args...> dependencies_;
    
public:
    Factory(std::function<std::unique_ptr<T>(Args...)> factory_func, Args... deps)
        : factory_func_(factory_func), dependencies_(deps...) {}
    
    std::unique_ptr<T> create() override {
        return std::apply(factory_func_, dependencies_);
    }
};

// Factory with dependency injection
template<typename TProduct, typename... TDependencies>
class DIFactory {
private:
    std::function<std::unique_ptr<TProduct>(TDependencies...)> factory_func_;
    std::tuple<TDependencies...> dependencies_;
    
public:
    DIFactory(std::function<std::unique_ptr<TProduct>(TDependencies...)> factory_func,
              TDependencies... deps)
        : factory_func_(factory_func), dependencies_(deps...) {}
    
    std::unique_ptr<TProduct> create() {
        return std::apply(factory_func_, dependencies_);
    }
    
    // Create with different dependencies
    template<typename... NewDeps>
    std::unique_ptr<TProduct> create_with(NewDeps... new_deps) {
        return factory_func_(new_deps...);
    }
};

// Abstract factory pattern with DI
template<typename TAbstractProduct>
class AbstractFactory {
public:
    virtual ~AbstractFactory() = default;
    virtual std::unique_ptr<TAbstractProduct> create() = 0;
};

// Concrete factory with dependencies
template<typename TConcreteProduct, typename TAbstractProduct, typename... TDependencies>
class ConcreteFactory : public AbstractFactory<TAbstractProduct> {
private:
    std::function<std::unique_ptr<TConcreteProduct>(TDependencies...)> factory_func_;
    std::tuple<TDependencies...> dependencies_;
    
public:
    ConcreteFactory(std::function<std::unique_ptr<TConcreteProduct>(TDependencies...)> factory_func,
                    TDependencies... deps)
        : factory_func_(factory_func), dependencies_(deps...) {}
    
    std::unique_ptr<TAbstractProduct> create() override {
        return std::apply(factory_func_, dependencies_);
    }
};

// Factory registry for managing multiple factories
template<typename TKey, typename TProduct>
class FactoryRegistry {
private:
    std::unordered_map<TKey, std::function<std::unique_ptr<TProduct>()>> factories_;
    
public:
    // Register factory
    template<typename TFactory>
    void register_factory(const TKey& key, std::shared_ptr<TFactory> factory) {
        factories_[key] = [factory]() {
            return factory->create();
        };
    }
    
    // Register factory function
    void register_factory(const TKey& key, std::function<std::unique_ptr<TProduct>()> factory) {
        factories_[key] = factory;
    }
    
    // Create product using key
    std::unique_ptr<TProduct> create(const TKey& key) {
        auto it = factories_.find(key);
        if (it == factories_.end()) {
            return nullptr;
        }
        return it->second();
    }
    
    // Check if factory is registered
    bool is_registered(const TKey& key) const {
        return factories_.find(key) != factories_.end();
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
private:
    std::string filename_;
    
public:
    explicit FileLogger(const std::string& filename) : filename_(filename) {}
    
    void log(const std::string& message) override {
        std::cout << "[FILE:" << filename_ << "] " << message << std::endl;
    }
};

// Example: Service using factory
class UserService {
private:
    std::shared_ptr<ILogger> logger_;
    
public:
    explicit UserService(std::shared_ptr<ILogger> logger) : logger_(logger) {}
    
    void register_user(const std::string& email) {
        logger_->log("Registering user: " + email);
    }
};

// Example usage
int main() {
    // Pattern 1: Simple factory
    auto logger_factory = std::make_shared<Factory<ILogger>>(
        []() { return std::make_unique<ConsoleLogger>(); }
    );
    auto logger = logger_factory->create();
    logger->log("From factory");
    
    // Pattern 2: Factory with DI
    auto file_logger_factory = DIFactory<FileLogger, std::string>(
        [](const std::string& filename) {
            return std::make_unique<FileLogger>(filename);
        },
        "app.log"
    );
    auto file_logger = file_logger_factory.create();
    file_logger->log("From DI factory");
    
    // Pattern 3: Abstract factory
    auto abstract_factory = std::make_shared<ConcreteFactory<ConsoleLogger, ILogger>>(
        []() { return std::make_unique<ConsoleLogger>(); }
    );
    auto abstract_logger = abstract_factory->create();
    abstract_logger->log("From abstract factory");
    
    // Pattern 4: Factory registry
    FactoryRegistry<std::string, ILogger> registry;
    registry.register_factory("console", []() {
        return std::make_unique<ConsoleLogger>();
    });
    registry.register_factory("file", []() {
        return std::make_unique<FileLogger>("default.log");
    });
    
    auto console_logger = registry.create("console");
    if (console_logger) {
        console_logger->log("From registry");
    }
    
    // Pattern 5: Service factory with dependencies
    auto user_service_factory = DIFactory<UserService, std::shared_ptr<ILogger>>(
        [](std::shared_ptr<ILogger> logger) {
            return std::make_unique<UserService>(logger);
        },
        std::make_shared<ConsoleLogger>()
    );
    auto user_service = user_service_factory.create();
    user_service->register_user("user@example.com");
    
    return 0;
}

