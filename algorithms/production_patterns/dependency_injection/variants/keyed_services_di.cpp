/*
 * Keyed Services Dependency Injection
 * 
 * Source: .NET Core DI, Autofac, Spring Framework
 * Pattern: Register and resolve services using keys/names
 * 
 * What Makes It Ingenious:
 * - Multiple implementations: Register multiple implementations of same interface
 * - Key-based resolution: Resolve specific implementation by key
 * - Named services: Use strings or enums as keys
 * - Flexible configuration: Choose implementation at runtime
 * - Used in frameworks, plugins, multi-tenant applications
 * 
 * When to Use:
 * - Multiple implementations of same interface
 * - Plugin architectures
 * - Strategy pattern with DI
 * - Multi-tenant applications
 * - Feature flags / A/B testing
 * - Environment-specific implementations
 * 
 * Real-World Usage:
 * - .NET Core DI (Keyed services)
 * - Autofac (Keyed services)
 * - Spring Framework (Qualifiers)
 * - Plugin systems
 * - Multi-tenant SaaS applications
 * 
 * Time Complexity: O(1) for registration and resolution
 * Space Complexity: O(n) where n is number of keyed services
 */

#include <memory>
#include <unordered_map>
#include <string>
#include <functional>
#include <iostream>
#include <any>
#include <vector>

// Keyed service container
template<typename TKey>
class KeyedServiceContainer {
private:
    std::unordered_map<TKey, std::function<std::any()>> factories_;
    
public:
    // Register service with key
    template<typename TInterface>
    void register_keyed(const TKey& key, std::function<std::shared_ptr<TInterface>()> factory) {
        factories_[key] = [factory]() -> std::any {
            return std::make_any<std::shared_ptr<TInterface>>(factory());
        };
    }
    
    // Resolve service by key
    template<typename TInterface>
    std::shared_ptr<TInterface> resolve(const TKey& key) {
        auto it = factories_.find(key);
        if (it == factories_.end()) {
            throw std::runtime_error("Service not found for key");
        }
        return std::any_cast<std::shared_ptr<TInterface>>(it->second());
    }
    
    // Get all registered keys for an interface
    std::vector<TKey> get_keys() const {
        std::vector<TKey> keys;
        for (const auto& pair : factories_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // Check if key is registered
    bool is_registered(const TKey& key) const {
        return factories_.find(key) != factories_.end();
    }
};

// Example: Multiple logger implementations
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

class DatabaseLogger : public ILogger {
public:
    void log(const std::string& message) override {
        std::cout << "[DATABASE] " << message << std::endl;
    }
};

// Example: Service using keyed logger
class LoggingService {
private:
    KeyedServiceContainer<std::string>& container_;
    
public:
    explicit LoggingService(KeyedServiceContainer<std::string>& container)
        : container_(container) {}
    
    void log_to_console(const std::string& message) {
        auto logger = container_.resolve<ILogger>("console");
        logger->log(message);
    }
    
    void log_to_file(const std::string& message) {
        auto logger = container_.resolve<ILogger>("file");
        logger->log(message);
    }
    
    void log_to_database(const std::string& message) {
        auto logger = container_.resolve<ILogger>("database");
        logger->log(message);
    }
};

// Enum-based keys
enum class LoggerType {
    Console,
    File,
    Database
};

// Example usage
int main() {
    // String-based keys
    KeyedServiceContainer<std::string> string_container;
    
    string_container.register_keyed<ILogger>("console", []() {
        return std::make_shared<ConsoleLogger>();
    });
    
    string_container.register_keyed<ILogger>("file", []() {
        return std::make_shared<FileLogger>("app.log");
    });
    
    string_container.register_keyed<ILogger>("database", []() {
        return std::make_shared<DatabaseLogger>();
    });
    
    // Resolve by key
    auto console_logger = string_container.resolve<ILogger>("console");
    console_logger->log("Console logging");
    
    auto file_logger = string_container.resolve<ILogger>("file");
    file_logger->log("File logging");
    
    auto db_logger = string_container.resolve<ILogger>("database");
    db_logger->log("Database logging");
    
    // Use in service
    LoggingService logging_service(string_container);
    logging_service.log_to_console("From service");
    logging_service.log_to_file("From service");
    logging_service.log_to_database("From service");
    
    // Enum-based keys
    KeyedServiceContainer<LoggerType> enum_container;
    
    enum_container.register_keyed<ILogger>(LoggerType::Console, []() {
        return std::make_shared<ConsoleLogger>();
    });
    
    enum_container.register_keyed<ILogger>(LoggerType::File, []() {
        return std::make_shared<FileLogger>("app.log");
    });
    
    enum_container.register_keyed<ILogger>(LoggerType::Database, []() {
        return std::make_shared<DatabaseLogger>();
    });
    
    auto enum_logger = enum_container.resolve<ILogger>(LoggerType::Console);
    enum_logger->log("Enum-based key");
    
    return 0;
}

