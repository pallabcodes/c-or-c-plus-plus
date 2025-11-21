/*
 * Runtime IoC Container - Dependency Injection
 * 
 * Source: Autofac, InversifyJS, .NET DI, Spring Framework
 * Pattern: Runtime dependency injection container with service registration
 * 
 * What Makes It Ingenious:
 * - Service registration: Register services with different lifetimes
 * - Automatic dependency resolution: Resolve dependency graphs automatically
 * - Lifetime management: Singleton, transient, scoped services
 * - Interface-based: Register implementations against interfaces
 * - Used in enterprise applications, frameworks, game engines
 * 
 * When to Use:
 * - Large applications with complex dependency graphs
 * - Need runtime flexibility in dependency resolution
 * - Testing with mock dependencies
 * - Plugin architectures
 * - Service-oriented architectures
 * 
 * Real-World Usage:
 * - .NET Core DI container
 * - Spring Framework (Java)
 * - Autofac (.NET)
 * - InversifyJS (TypeScript)
 * - Game engines (Unity, Unreal)
 * 
 * Time Complexity: O(1) for registration, O(n) for resolution (n = dependency depth)
 * Space Complexity: O(n) where n is number of registered services
 */

#include <memory>
#include <unordered_map>
#include <string>
#include <functional>
#include <typeindex>
#include <mutex>
#include <iostream>
#include <any>

class RuntimeIoCContainer {
public:
    enum class Lifetime {
        Singleton,  // Single instance for entire lifetime
        Transient,  // New instance every time
        Scoped      // Single instance per scope
    };
    
    // Service registration info
    struct ServiceRegistration {
        Lifetime lifetime;
        std::function<std::any()> factory;
        std::any instance;  // For singleton
        bool is_initialized;
        
        ServiceRegistration(Lifetime lt, std::function<std::any()> f)
            : lifetime(lt), factory(f), is_initialized(false) {}
    };
    
private:
    std::unordered_map<std::type_index, ServiceRegistration> services_;
    std::mutex mutex_;
    
    template<typename T>
    std::type_index get_type_index() {
        return std::type_index(typeid(T));
    }
    
public:
    // Register singleton service
    template<typename TInterface, typename TImplementation>
    void register_singleton() {
        static_assert(std::is_base_of_v<TInterface, TImplementation>,
                     "TImplementation must inherit from TInterface");
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<TInterface>();
        
        services_[type_idx] = ServiceRegistration(
            Lifetime::Singleton,
            []() -> std::any {
                return std::make_any<std::shared_ptr<TInterface>>(
                    std::make_shared<TImplementation>());
            }
        );
    }
    
    // Register singleton with factory
    template<typename TInterface>
    void register_singleton(std::function<std::shared_ptr<TInterface>()> factory) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<TInterface>();
        
        services_[type_idx] = ServiceRegistration(
            Lifetime::Singleton,
            [factory]() -> std::any {
                return std::make_any<std::shared_ptr<TInterface>>(factory());
            }
        );
    }
    
    // Register transient service
    template<typename TInterface, typename TImplementation>
    void register_transient() {
        static_assert(std::is_base_of_v<TInterface, TImplementation>,
                     "TImplementation must inherit from TInterface");
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<TInterface>();
        
        services_[type_idx] = ServiceRegistration(
            Lifetime::Transient,
            []() -> std::any {
                return std::make_any<std::shared_ptr<TInterface>>(
                    std::make_shared<TImplementation>());
            }
        );
    }
    
    // Register instance
    template<typename TInterface>
    void register_instance(std::shared_ptr<TInterface> instance) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<TInterface>();
        
        services_[type_idx] = ServiceRegistration(
            Lifetime::Singleton,
            [instance]() -> std::any {
                return std::make_any<std::shared_ptr<TInterface>>(instance);
            }
        );
        services_[type_idx].instance = std::make_any<std::shared_ptr<TInterface>>(instance);
        services_[type_idx].is_initialized = true;
    }
    
    // Resolve service
    template<typename T>
    std::shared_ptr<T> resolve() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        
        auto it = services_.find(type_idx);
        if (it == services_.end()) {
            throw std::runtime_error("Service not registered: " + std::string(typeid(T).name()));
        }
        
        auto& registration = it->second;
        
        // Handle singleton
        if (registration.lifetime == Lifetime::Singleton) {
            if (!registration.is_initialized) {
                registration.instance = registration.factory();
                registration.is_initialized = true;
            }
            return std::any_cast<std::shared_ptr<T>>(registration.instance);
        }
        
        // Handle transient
        if (registration.lifetime == Lifetime::Transient) {
            return std::any_cast<std::shared_ptr<T>>(registration.factory());
        }
        
        // Handle scoped (simplified - same as transient for now)
        return std::any_cast<std::shared_ptr<T>>(registration.factory());
    }
    
    // Check if service is registered
    template<typename T>
    bool is_registered() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        return services_.find(type_idx) != services_.end();
    }
    
    // Clear all registrations
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        services_.clear();
    }
};

// Example interfaces and implementations
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

class IEmailService {
public:
    virtual ~IEmailService() = default;
    virtual void send_email(const std::string& to, const std::string& subject) = 0;
};

class EmailService : public IEmailService {
private:
    std::shared_ptr<ILogger> logger_;
    
public:
    EmailService(std::shared_ptr<ILogger> logger) : logger_(logger) {}
    
    void send_email(const std::string& to, const std::string& subject) override {
        logger_->log("Sending email to: " + to + " - " + subject);
        // Email sending logic
    }
};

class UserService {
private:
    std::shared_ptr<IEmailService> email_service_;
    std::shared_ptr<ILogger> logger_;
    
public:
    UserService(std::shared_ptr<IEmailService> email_service,
                std::shared_ptr<ILogger> logger)
        : email_service_(email_service), logger_(logger) {}
    
    void register_user(const std::string& email) {
        logger_->log("Registering user: " + email);
        email_service_->send_email(email, "Welcome!");
    }
};

// Example usage
int main() {
    RuntimeIoCContainer container;
    
    // Register services
    container.register_singleton<ILogger, ConsoleLogger>();
    container.register_singleton<IEmailService, EmailService>(
        [&container]() {
            return std::make_shared<EmailService>(container.resolve<ILogger>());
        }
    );
    
    // Resolve services
    auto logger = container.resolve<ILogger>();
    logger->log("Application started");
    
    auto email_service = container.resolve<IEmailService>();
    email_service->send_email("user@example.com", "Test");
    
    // Create service with dependencies
    auto user_service = std::make_shared<UserService>(
        container.resolve<IEmailService>(),
        container.resolve<ILogger>()
    );
    user_service->register_user("newuser@example.com");
    
    return 0;
}

