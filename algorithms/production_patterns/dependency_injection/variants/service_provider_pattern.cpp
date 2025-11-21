/*
 * Service Provider Pattern - Dependency Injection
 * 
 * Source: .NET Core, ASP.NET Core, Microsoft.Extensions.DependencyInjection
 * Pattern: Service provider interface for dependency resolution
 * 
 * What Makes It Ingenious:
 * - Service provider interface: Abstraction over container
 * - GetService pattern: Resolve services by type
 * - Optional services: Returns nullptr if not found
 * - Service collection: Build provider from collection
 * - Used in .NET Core, ASP.NET Core, modern frameworks
 * 
 * When to Use:
 * - Framework development
 * - Plugin architectures
 * - Need service provider abstraction
 * - Integration with existing DI containers
 * - Service resolution at runtime
 * 
 * Real-World Usage:
 * - .NET Core IServiceProvider
 * - ASP.NET Core dependency injection
 * - Microsoft.Extensions.DependencyInjection
 * - Framework integrations
 * - Plugin systems
 * 
 * Time Complexity: O(1) for service resolution
 * Space Complexity: O(n) where n is number of services
 */

#include <memory>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <iostream>
#include <any>
#include <vector>

// Service provider interface
class IServiceProvider {
public:
    virtual ~IServiceProvider() = default;
    
    // Get service by type
    template<typename T>
    std::shared_ptr<T> get_service() {
        return get_service_impl<T>();
    }
    
    // Get optional service (returns nullptr if not found)
    template<typename T>
    std::shared_ptr<T> get_service_optional() {
        try {
            return get_service_impl<T>();
        } catch (...) {
            return nullptr;
        }
    }
    
    // Get required service (throws if not found)
    template<typename T>
    std::shared_ptr<T> get_required_service() {
        auto service = get_service_impl<T>();
        if (!service) {
            throw std::runtime_error("Required service not found: " + 
                                   std::string(typeid(T).name()));
        }
        return service;
    }
    
    // Get multiple services of same type
    template<typename T>
    std::vector<std::shared_ptr<T>> get_services() {
        return get_services_impl<T>();
    }
    
protected:
    virtual std::any get_service_impl(const std::type_index& type) = 0;
    
private:
    template<typename T>
    std::shared_ptr<T> get_service_impl() {
        auto result = get_service_impl(std::type_index(typeid(T)));
        if (result.has_value()) {
            try {
                return std::any_cast<std::shared_ptr<T>>(result);
            } catch (...) {
                return nullptr;
            }
        }
        return nullptr;
    }
    
    template<typename T>
    std::vector<std::shared_ptr<T>> get_services_impl() {
        // Simplified - would need multi-registration support
        auto service = get_service_impl<T>();
        if (service) {
            return {service};
        }
        return {};
    }
};

// Service descriptor
struct ServiceDescriptor {
    std::type_index service_type;
    std::type_index implementation_type;
    std::function<std::any()> factory;
    enum Lifetime { Singleton, Transient, Scoped } lifetime;
    
    ServiceDescriptor(std::type_index svc_type, std::type_index impl_type,
                     std::function<std::any()> fact, Lifetime lt)
        : service_type(svc_type), implementation_type(impl_type),
          factory(fact), lifetime(lt) {}
};

// Service collection for building provider
class ServiceCollection {
private:
    std::vector<ServiceDescriptor> descriptors_;
    
public:
    // Add singleton
    template<typename TInterface, typename TImplementation>
    void add_singleton() {
        descriptors_.emplace_back(
            std::type_index(typeid(TInterface)),
            std::type_index(typeid(TImplementation)),
            []() -> std::any {
                return std::make_any<std::shared_ptr<TInterface>>(
                    std::make_shared<TImplementation>());
            },
            ServiceDescriptor::Singleton
        );
    }
    
    // Add singleton with factory
    template<typename TInterface>
    void add_singleton(std::function<std::shared_ptr<TInterface>()> factory) {
        descriptors_.emplace_back(
            std::type_index(typeid(TInterface)),
            std::type_index(typeid(TInterface)),
            [factory]() -> std::any {
                return std::make_any<std::shared_ptr<TInterface>>(factory());
            },
            ServiceDescriptor::Singleton
        );
    }
    
    // Add transient
    template<typename TInterface, typename TImplementation>
    void add_transient() {
        descriptors_.emplace_back(
            std::type_index(typeid(TInterface)),
            std::type_index(typeid(TImplementation)),
            []() -> std::any {
                return std::make_any<std::shared_ptr<TInterface>>(
                    std::make_shared<TImplementation>());
            },
            ServiceDescriptor::Transient
        );
    }
    
    // Add instance
    template<typename TInterface>
    void add_instance(std::shared_ptr<TInterface> instance) {
        descriptors_.emplace_back(
            std::type_index(typeid(TInterface)),
            std::type_index(typeid(TInterface)),
            [instance]() -> std::any {
                return std::make_any<std::shared_ptr<TInterface>>(instance);
            },
            ServiceDescriptor::Singleton
        );
    }
    
    // Build service provider
    std::unique_ptr<IServiceProvider> build_service_provider();
    
    const std::vector<ServiceDescriptor>& get_descriptors() const {
        return descriptors_;
    }
};

// Service provider implementation
class ServiceProvider : public IServiceProvider {
private:
    std::unordered_map<std::type_index, ServiceDescriptor> descriptors_;
    std::unordered_map<std::type_index, std::any> singletons_;
    std::mutex mutex_;
    
public:
    explicit ServiceProvider(const std::vector<ServiceDescriptor>& descriptors) {
        for (const auto& desc : descriptors) {
            descriptors_[desc.service_type] = desc;
        }
    }
    
protected:
    std::any get_service_impl(const std::type_index& type) override {
        auto it = descriptors_.find(type);
        if (it == descriptors_.end()) {
            return {};
        }
        
        const auto& desc = it->second;
        
        // Handle singleton
        if (desc.lifetime == ServiceDescriptor::Singleton) {
            std::lock_guard<std::mutex> lock(mutex_);
            auto singleton_it = singletons_.find(type);
            if (singleton_it != singletons_.end()) {
                return singleton_it->second;
            }
            
            auto instance = desc.factory();
            singletons_[type] = instance;
            return instance;
        }
        
        // Handle transient
        return desc.factory();
    }
};

std::unique_ptr<IServiceProvider> ServiceCollection::build_service_provider() {
    return std::make_unique<ServiceProvider>(descriptors_);
}

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

class IEmailService {
public:
    virtual ~IEmailService() = default;
    virtual void send(const std::string& to, const std::string& subject) = 0;
};

class EmailService : public IEmailService {
private:
    std::shared_ptr<ILogger> logger_;
    
public:
    explicit EmailService(std::shared_ptr<ILogger> logger) : logger_(logger) {}
    
    void send(const std::string& to, const std::string& subject) override {
        logger_->log("Sending email to: " + to);
    }
};

// Example usage
int main() {
    ServiceCollection services;
    
    // Register services
    services.add_singleton<ILogger, ConsoleLogger>();
    services.add_singleton<IEmailService>(
        [](IServiceProvider* provider) {
            return std::make_shared<EmailService>(
                provider->get_service<ILogger>());
        }
    );
    
    // Build provider
    auto provider = services.build_service_provider();
    
    // Resolve services
    auto logger = provider->get_service<ILogger>();
    logger->log("Service provider working");
    
    auto email_service = provider->get_service<IEmailService>();
    email_service->send("user@example.com", "Test");
    
    // Optional service
    auto optional = provider->get_service_optional<ILogger>();
    if (optional) {
        optional->log("Optional service found");
    }
    
    return 0;
}

