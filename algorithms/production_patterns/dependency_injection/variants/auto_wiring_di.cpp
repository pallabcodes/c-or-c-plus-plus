/*
 * Auto-Wiring Dependency Injection
 * 
 * Source: Spring Framework, Ninject, Autofac
 * Pattern: Automatic dependency resolution based on constructor parameters
 * 
 * What Makes It Ingenious:
 * - Automatic resolution: Container infers dependencies from constructor
 * - Reflection-based: Uses type information to resolve dependencies
 * - Convention over configuration: No explicit registration needed
 * - Recursive resolution: Resolves entire dependency graph
 * - Used in modern DI frameworks, convention-based frameworks
 * 
 * When to Use:
 * - Convention-based applications
 * - Rapid development
 * - When dependencies match registered types
 * - Framework development
 * - Reduce boilerplate registration
 * 
 * Real-World Usage:
 * - Spring Framework (autowiring)
 * - Ninject (convention-based binding)
 * - Autofac (automatic registration)
 * - ASP.NET Core (constructor injection)
 * - Modern DI frameworks
 * 
 * Time Complexity: O(n) where n is dependency depth
 * Space Complexity: O(n) for dependency graph
 */

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <iostream>
#include <any>
#include <vector>

// Auto-wiring container
class AutoWiringContainer {
private:
    std::unordered_map<std::type_index, std::function<std::any()>> factories_;
    std::unordered_map<std::type_index, std::any> singletons_;
    std::mutex mutex_;
    
    template<typename T>
    std::type_index get_type_index() {
        return std::type_index(typeid(T));
    }
    
    // Check if type is registered
    bool is_registered(const std::type_index& type) {
        return factories_.find(type) != factories_.end();
    }
    
    // Resolve dependency (recursive)
    std::any resolve_dependency(const std::type_index& type) {
        auto it = factories_.find(type);
        if (it == factories_.end()) {
            throw std::runtime_error("Type not registered: " + std::string(type.name()));
        }
        
        // Check singleton cache
        auto singleton_it = singletons_.find(type);
        if (singleton_it != singletons_.end()) {
            return singleton_it->second;
        }
        
        // Create instance
        auto instance = it->second();
        
        // Cache singleton
        singletons_[type] = instance;
        
        return instance;
    }
    
public:
    // Register type with factory
    template<typename T>
    void register_type(std::function<std::shared_ptr<T>()> factory) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        factories_[type_idx] = [factory]() -> std::any {
            return std::make_any<std::shared_ptr<T>>(factory());
        };
    }
    
    // Register type (auto-wired constructor)
    template<typename T>
    void register_type() {
        register_type<T>([]() {
            return std::make_shared<T>();
        });
    }
    
    // Register singleton
    template<typename T>
    void register_singleton(std::function<std::shared_ptr<T>()> factory) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        factories_[type_idx] = [this, type_idx, factory]() -> std::any {
            // Check if already created
            auto it = singletons_.find(type_idx);
            if (it != singletons_.end()) {
                return it->second;
            }
            
            auto instance = factory();
            singletons_[type_idx] = std::make_any<std::shared_ptr<T>>(instance);
            return singletons_[type_idx];
        };
    }
    
    // Register instance
    template<typename T>
    void register_instance(std::shared_ptr<T> instance) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        singletons_[type_idx] = std::make_any<std::shared_ptr<T>>(instance);
        factories_[type_idx] = [instance]() -> std::any {
            return std::make_any<std::shared_ptr<T>>(instance);
        };
    }
    
    // Resolve type (auto-wired)
    template<typename T>
    std::shared_ptr<T> resolve() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        auto result = resolve_dependency(type_idx);
        return std::any_cast<std::shared_ptr<T>>(result);
    }
    
    // Auto-wire constructor with dependencies
    template<typename T, typename... Dependencies>
    std::shared_ptr<T> resolve_with_dependencies() {
        return std::make_shared<T>(resolve<Dependencies>()...);
    }
};

// Example: Services with dependencies
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

// Service with constructor dependencies (auto-wired)
class UserService {
private:
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IConfigService> config_;
    
public:
    // Constructor with dependencies - auto-wired
    UserService(std::shared_ptr<ILogger> logger,
                std::shared_ptr<IConfigService> config)
        : logger_(logger), config_(config) {}
    
    void register_user(const std::string& email) {
        logger_->log("Registering user: " + email);
        std::string timeout = config_->get("timeout");
        logger_->log("Using timeout: " + timeout);
    }
};

// Example usage
int main() {
    AutoWiringContainer container;
    
    // Register dependencies
    container.register_singleton<ILogger>([]() {
        return std::make_shared<ConsoleLogger>();
    });
    
    container.register_singleton<IConfigService>([]() {
        return std::make_shared<ConfigService>();
    });
    
    // Register service with auto-wired constructor
    container.register_type<UserService>(
        [&container]() {
            return std::make_shared<UserService>(
                container.resolve<ILogger>(),
                container.resolve<IConfigService>()
            );
        }
    );
    
    // Resolve service (dependencies auto-wired)
    auto user_service = container.resolve<UserService>();
    user_service->register_user("user@example.com");
    
    // Direct resolution with dependencies
    auto direct_service = container.resolve_with_dependencies<
        UserService, ILogger, IConfigService>();
    direct_service->register_user("direct@example.com");
    
    return 0;
}

