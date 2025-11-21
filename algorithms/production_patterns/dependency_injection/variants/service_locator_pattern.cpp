/*
 * Service Locator Pattern - Dependency Injection
 * 
 * Source: Enterprise patterns, Martin Fowler, game development
 * Pattern: Central registry for locating services
 * 
 * What Makes It Ingenious:
 * - Global access: Services accessible from anywhere
 * - Lazy initialization: Services created on first access
 * - Service discovery: Find services by type or name
 * - Decoupling: Clients don't know service implementation
 * - Used in game engines, enterprise applications, frameworks
 * 
 * When to Use:
 * - Need global service access
 * - Plugin architectures
 * - Game engines (Unity, Unreal use service locator)
 * - Legacy code integration
 * - When DI container is too heavy
 * 
 * Real-World Usage:
 * - Unity Engine (Service Locator)
 * - Unreal Engine (Subsystem system)
 * - Enterprise frameworks
 * - Plugin systems
 * - Game development
 * 
 * Time Complexity: O(1) for service lookup
 * Space Complexity: O(n) where n is number of services
 */

#include <memory>
#include <unordered_map>
#include <string>
#include <functional>
#include <typeindex>
#include <mutex>
#include <iostream>
#include <any>

class ServiceLocator {
public:
    // Service factory function type
    using ServiceFactory = std::function<std::any()>;
    
private:
    static std::unordered_map<std::type_index, ServiceFactory> factories_;
    static std::unordered_map<std::type_index, std::any> instances_;
    static std::mutex mutex_;
    
    template<typename T>
    static std::type_index get_type_index() {
        return std::type_index(typeid(T));
    }
    
public:
    // Register service factory
    template<typename TInterface>
    static void register_service(std::function<std::shared_ptr<TInterface>()> factory) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<TInterface>();
        
        factories_[type_idx] = [factory]() -> std::any {
            return std::make_any<std::shared_ptr<TInterface>>(factory());
        };
    }
    
    // Register singleton instance
    template<typename TInterface>
    static void register_instance(std::shared_ptr<TInterface> instance) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<TInterface>();
        instances_[type_idx] = std::make_any<std::shared_ptr<TInterface>>(instance);
    }
    
    // Resolve service (creates on first access)
    template<typename T>
    static std::shared_ptr<T> resolve() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        
        // Check if instance already exists
        auto instance_it = instances_.find(type_idx);
        if (instance_it != instances_.end()) {
            return std::any_cast<std::shared_ptr<T>>(instance_it->second);
        }
        
        // Create new instance using factory
        auto factory_it = factories_.find(type_idx);
        if (factory_it == factories_.end()) {
            throw std::runtime_error("Service not registered: " + std::string(typeid(T).name()));
        }
        
        auto instance = factory_it->second();
        instances_[type_idx] = instance;
        return std::any_cast<std::shared_ptr<T>>(instance);
    }
    
    // Resolve optional service (returns nullptr if not found)
    template<typename T>
    static std::shared_ptr<T> resolve_optional() {
        try {
            return resolve<T>();
        } catch (...) {
            return nullptr;
        }
    }
    
    // Check if service is registered
    template<typename T>
    static bool is_registered() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        return factories_.find(type_idx) != factories_.end() ||
               instances_.find(type_idx) != instances_.end();
    }
    
    // Clear all services
    static void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        factories_.clear();
        instances_.clear();
    }
    
    // Reset service (remove instance, keep factory)
    template<typename T>
    static void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        instances_.erase(type_idx);
    }
};

// Static member definitions
std::unordered_map<std::type_index, ServiceLocator::ServiceFactory> ServiceLocator::factories_;
std::unordered_map<std::type_index, std::any> ServiceLocator::instances_;
std::mutex ServiceLocator::mutex_;

// Example interfaces
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

// Example: Service that uses Service Locator
class BusinessService {
public:
    void do_work() {
        // Access services through Service Locator
        auto logger = ServiceLocator::resolve<ILogger>();
        auto config = ServiceLocator::resolve<IConfigService>();
        
        logger->log("Starting work");
        std::string setting = config->get("timeout");
        logger->log("Got config: " + setting);
    }
};

// Example usage
int main() {
    // Register services
    ServiceLocator::register_service<ILogger>([]() {
        return std::make_shared<ConsoleLogger>();
    });
    
    ServiceLocator::register_service<IConfigService>([]() {
        return std::make_shared<ConfigService>();
    });
    
    // Use services from anywhere
    auto logger = ServiceLocator::resolve<ILogger>();
    logger->log("Application started");
    
    auto config = ServiceLocator::resolve<IConfigService>();
    std::cout << "Config value: " << config->get("database_url") << std::endl;
    
    // Service can use Service Locator internally
    BusinessService business_service;
    business_service.do_work();
    
    // Optional resolution
    auto optional_service = ServiceLocator::resolve_optional<ILogger>();
    if (optional_service) {
        optional_service->log("Optional service found");
    }
    
    return 0;
}

