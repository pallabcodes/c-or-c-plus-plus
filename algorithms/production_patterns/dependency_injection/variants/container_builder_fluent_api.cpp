/*
 * Container Builder with Fluent API - Dependency Injection
 * 
 * Source: Autofac, .NET Core DI, Spring Framework
 * Pattern: Fluent builder API for container configuration
 * 
 * What Makes It Ingenious:
 * - Fluent interface: Readable, chainable configuration
 * - Builder pattern: Step-by-step container construction
 * - Type-safe: Compile-time type checking
 * - Expressive: Self-documenting configuration code
 * - Used in Autofac, .NET Core, Spring Boot, modern DI frameworks
 * 
 * When to Use:
 * - Need readable container configuration
 * - Complex registration scenarios
 * - Framework development
 * - Library development
 * - Production applications
 * 
 * Real-World Usage:
 * - Autofac ContainerBuilder
 * - .NET Core ServiceCollection
 * - Spring Boot configuration
 * - Modern DI frameworks
 * - Enterprise applications
 * 
 * Time Complexity: O(1) per registration, O(n) for build
 * Space Complexity: O(n) where n is number of registrations
 */

#include <memory>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <iostream>
#include <any>
#include <vector>
#include <string>

// Forward declarations
class Container;
class ContainerBuilder;

// Service lifetime
enum class ServiceLifetime {
    Singleton,
    Transient,
    Scoped
};

// Registration builder for fluent API
template<typename TInterface>
class RegistrationBuilder {
private:
    ContainerBuilder* builder_;
    ServiceLifetime lifetime_;
    std::function<std::shared_ptr<TInterface>()> factory_;
    std::shared_ptr<TInterface> instance_;
    
public:
    RegistrationBuilder(ContainerBuilder* builder, ServiceLifetime lifetime)
        : builder_(builder), lifetime_(lifetime) {}
    
    // Register implementation type
    template<typename TImplementation>
    RegistrationBuilder& as() {
        static_assert(std::is_base_of_v<TInterface, TImplementation>,
                     "TImplementation must inherit from TInterface");
        factory_ = []() {
            return std::static_pointer_cast<TInterface>(
                std::make_shared<TImplementation>());
        };
        return *this;
    }
    
    // Register with factory function
    RegistrationBuilder& using_factory(std::function<std::shared_ptr<TInterface>()> factory) {
        factory_ = factory;
        return *this;
    }
    
    // Register instance
    RegistrationBuilder& as_instance(std::shared_ptr<TInterface> instance) {
        instance_ = instance;
        factory_ = [instance]() { return instance; };
        return *this;
    }
    
    // Single instance (singleton)
    RegistrationBuilder& single_instance() {
        lifetime_ = ServiceLifetime::Singleton;
        return *this;
    }
    
    // Instance per dependency (transient)
    RegistrationBuilder& instance_per_dependency() {
        lifetime_ = ServiceLifetime::Transient;
        return *this;
    }
    
    // Instance per scope
    RegistrationBuilder& instance_per_scope() {
        lifetime_ = ServiceLifetime::Scoped;
        return *this;
    }
    
    // Build and register
    ContainerBuilder& build() {
        return *builder_;
    }
    
    // Getters for container builder
    ServiceLifetime get_lifetime() const { return lifetime_; }
    std::function<std::shared_ptr<TInterface>()> get_factory() const { return factory_; }
    std::shared_ptr<TInterface> get_instance() const { return instance_; }
};

// Container builder with fluent API
class ContainerBuilder {
private:
    struct Registration {
        ServiceLifetime lifetime;
        std::function<std::any()> factory;
        std::any instance;
        bool has_instance;
    };
    
    std::unordered_map<std::type_index, Registration> registrations_;
    
    template<typename T>
    std::type_index get_type_index() {
        return std::type_index(typeid(T));
    }
    
public:
    // Register service with fluent API
    template<typename TInterface>
    RegistrationBuilder<TInterface> register_type(ServiceLifetime lifetime = ServiceLifetime::Transient) {
        return RegistrationBuilder<TInterface>(this, lifetime);
    }
    
    // Register singleton
    template<typename TInterface>
    RegistrationBuilder<TInterface> register_singleton() {
        return register_type<TInterface>(ServiceLifetime::Singleton);
    }
    
    // Register transient
    template<typename TInterface>
    RegistrationBuilder<TInterface> register_transient() {
        return register_type<TInterface>(ServiceLifetime::Transient);
    }
    
    // Register scoped
    template<typename TInterface>
    RegistrationBuilder<TInterface> register_scoped() {
        return register_type<TInterface>(ServiceLifetime::Scoped);
    }
    
    // Register instance
    template<typename TInterface>
    ContainerBuilder& register_instance(std::shared_ptr<TInterface> instance) {
        auto type_idx = get_type_index<TInterface>();
        registrations_[type_idx] = {
            ServiceLifetime::Singleton,
            [instance]() -> std::any { return std::make_any<std::shared_ptr<TInterface>>(instance); },
            std::make_any<std::shared_ptr<TInterface>>(instance),
            true
        };
        return *this;
    }
    
    // Register factory
    template<typename TInterface>
    ContainerBuilder& register_factory(std::function<std::shared_ptr<TInterface>()> factory,
                                      ServiceLifetime lifetime = ServiceLifetime::Transient) {
        auto type_idx = get_type_index<TInterface>();
        registrations_[type_idx] = {
            lifetime,
            [factory]() -> std::any { return std::make_any<std::shared_ptr<TInterface>>(factory()); },
            {},
            false
        };
        return *this;
    }
    
    // Build container
    std::unique_ptr<Container> build() {
        return std::make_unique<Container>(std::move(registrations_));
    }
    
    // Get registrations (for internal use)
    std::unordered_map<std::type_index, Registration>& get_registrations() {
        return registrations_;
    }
};

// Container implementation
class Container {
private:
    struct Registration {
        ServiceLifetime lifetime;
        std::function<std::any()> factory;
        std::any instance;
        bool has_instance;
        bool is_initialized;
    };
    
    std::unordered_map<std::type_index, Registration> registrations_;
    
    template<typename T>
    std::type_index get_type_index() {
        return std::type_index(typeid(T));
    }
    
public:
    Container(std::unordered_map<std::type_index, ContainerBuilder::Registration> regs) {
        for (auto& [type_idx, reg] : regs) {
            registrations_[type_idx] = {
                reg.lifetime,
                reg.factory,
                reg.instance,
                reg.has_instance,
                reg.has_instance  // If instance provided, already initialized
            };
        }
    }
    
    template<typename T>
    std::shared_ptr<T> resolve() {
        auto type_idx = get_type_index<T>();
        auto it = registrations_.find(type_idx);
        if (it == registrations_.end()) {
            throw std::runtime_error("Service not registered: " + std::string(typeid(T).name()));
        }
        
        auto& reg = it->second;
        
        // Handle singleton
        if (reg.lifetime == ServiceLifetime::Singleton) {
            if (!reg.is_initialized) {
                reg.instance = reg.factory();
                reg.is_initialized = true;
            }
            return std::any_cast<std::shared_ptr<T>>(reg.instance);
        }
        
        // Handle transient/scoped
        return std::any_cast<std::shared_ptr<T>>(reg.factory());
    }
    
    template<typename T>
    bool is_registered() {
        auto type_idx = get_type_index<T>();
        return registrations_.find(type_idx) != registrations_.end();
    }
};

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
    EmailService(std::shared_ptr<ILogger> logger) : logger_(logger) {}
    
    void send(const std::string& to, const std::string& subject) override {
        logger_->log("Sending email to: " + to);
    }
};

// Example usage demonstrating fluent API
int main() {
    // Build container with fluent API
    ContainerBuilder builder;
    
    // Fluent registration examples
    builder
        .register_singleton<ILogger>()
        .as<ConsoleLogger>()
        .build();
    
    builder
        .register_singleton<IEmailService>()
        .using_factory([]() {
            // Would resolve ILogger from container in real implementation
            return std::make_shared<EmailService>(std::make_shared<ConsoleLogger>());
        })
        .build();
    
    // Register instance
    auto logger = std::make_shared<ConsoleLogger>();
    builder.register_instance<ILogger>(logger);
    
    // Register with factory
    builder.register_factory<ILogger>(
        []() { return std::make_shared<ConsoleLogger>(); },
        ServiceLifetime::Singleton
    );
    
    // Build container
    auto container = builder.build();
    
    // Resolve services
    auto resolved_logger = container->resolve<ILogger>();
    resolved_logger->log("Container built with fluent API");
    
    return 0;
}

