/*
 * Child Container Dependency Injection
 * 
 * Source: Autofac, InversifyJS, Spring Framework
 * Pattern: Hierarchical containers with parent-child relationships
 * 
 * What Makes It Ingenious:
 * - Container hierarchy: Child containers inherit from parent
 * - Scope isolation: Child containers have isolated scopes
 * - Override services: Child can override parent services
 * - Request scope: Create child container per request
 * - Used in web frameworks, multi-tenant applications, request handling
 * 
 * When to Use:
 * - Request-scoped services
 * - Multi-tenant applications
 * - Feature modules
 * - Testing with isolated containers
 * - Override services in specific contexts
 * 
 * Real-World Usage:
 * - Autofac (ILifetimeScope)
 * - InversifyJS (Container hierarchies)
 * - Spring Framework (ApplicationContext hierarchies)
 * - ASP.NET Core (Request scope)
 * - Web frameworks
 * 
 * Time Complexity: O(1) for child creation, O(n) for resolution
 * Space Complexity: O(n) where n is number of services
 */

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <mutex>
#include <iostream>
#include <any>
#include <vector>

// Base container interface
class IContainer {
public:
    virtual ~IContainer() = default;
    virtual std::shared_ptr<IContainer> create_child() = 0;
    virtual bool has_parent() const = 0;
    virtual IContainer* get_parent() const = 0;
};

// Service registration info
struct ServiceRegistration {
    enum class Lifetime { Singleton, Transient, Scoped };
    
    Lifetime lifetime;
    std::function<std::any()> factory;
    std::any instance;
    bool is_initialized;
    
    ServiceRegistration(Lifetime lt, std::function<std::any()> f)
        : lifetime(lt), factory(f), is_initialized(false) {}
};

// Container implementation
class Container : public IContainer {
private:
    IContainer* parent_;
    std::unordered_map<std::type_index, ServiceRegistration> services_;
    std::mutex mutex_;
    
    template<typename T>
    std::type_index get_type_index() {
        return std::type_index(typeid(T));
    }
    
    template<typename T>
    std::shared_ptr<T> resolve_from_parent() {
        if (parent_) {
            return static_cast<Container*>(parent_)->resolve<T>();
        }
        return nullptr;
    }
    
public:
    Container(IContainer* parent = nullptr) : parent_(parent) {}
    
    // Register singleton
    template<typename TInterface, typename TImplementation>
    void register_singleton() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<TInterface>();
        services_[type_idx] = ServiceRegistration(
            ServiceRegistration::Lifetime::Singleton,
            []() -> std::any {
                return std::make_any<std::shared_ptr<TInterface>>(
                    std::make_shared<TImplementation>());
            }
        );
    }
    
    // Register scoped (per container)
    template<typename TInterface, typename TImplementation>
    void register_scoped() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<TInterface>();
        services_[type_idx] = ServiceRegistration(
            ServiceRegistration::Lifetime::Scoped,
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
            ServiceRegistration::Lifetime::Singleton,
            [instance]() -> std::any {
                return std::make_any<std::shared_ptr<TInterface>>(instance);
            }
        );
        services_[type_idx].instance = std::make_any<std::shared_ptr<TInterface>>(instance);
        services_[type_idx].is_initialized = true;
    }
    
    // Resolve service (checks child first, then parent)
    template<typename T>
    std::shared_ptr<T> resolve() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        
        // Check child container first
        auto it = services_.find(type_idx);
        if (it != services_.end()) {
            auto& registration = it->second;
            
            if (registration.lifetime == ServiceRegistration::Lifetime::Singleton ||
                registration.lifetime == ServiceRegistration::Lifetime::Scoped) {
                if (!registration.is_initialized) {
                    registration.instance = registration.factory();
                    registration.is_initialized = true;
                }
                return std::any_cast<std::shared_ptr<T>>(registration.instance);
            } else {
                return std::any_cast<std::shared_ptr<T>>(registration.factory());
            }
        }
        
        // Check parent container
        if (parent_) {
            return static_cast<Container*>(parent_)->resolve<T>();
        }
        
        throw std::runtime_error("Service not registered: " + std::string(typeid(T).name()));
    }
    
    // Create child container
    std::shared_ptr<IContainer> create_child() override {
        return std::make_shared<Container>(this);
    }
    
    bool has_parent() const override {
        return parent_ != nullptr;
    }
    
    IContainer* get_parent() const override {
        return parent_;
    }
    
    // Check if service is registered (in this container or parent)
    template<typename T>
    bool is_registered() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        if (services_.find(type_idx) != services_.end()) {
            return true;
        }
        if (parent_) {
            return static_cast<Container*>(parent_)->is_registered<T>();
        }
        return false;
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

class IRequestService {
public:
    virtual ~IRequestService() = default;
    virtual void handle_request(const std::string& request) = 0;
};

class RequestService : public IRequestService {
private:
    std::shared_ptr<ILogger> logger_;
    
public:
    explicit RequestService(std::shared_ptr<ILogger> logger) : logger_(logger) {}
    
    void handle_request(const std::string& request) override {
        logger_->log("Handling request: " + request);
    }
};

// Example usage
int main() {
    // Create root container
    auto root_container = std::make_shared<Container>();
    
    // Register services in root (shared across all children)
    root_container->register_singleton<ILogger, ConsoleLogger>();
    
    // Create child container (e.g., for a request)
    auto request_container = root_container->create_child();
    
    // Register request-scoped service in child
    static_cast<Container*>(request_container.get())->register_scoped<IRequestService, RequestService>();
    
    // Resolve services
    auto logger = root_container->resolve<ILogger>();
    logger->log("Root container logger");
    
    // Child can resolve parent services
    auto child_logger = request_container->resolve<ILogger>();
    child_logger->log("Child container logger (from parent)");
    
    // Child resolves its own scoped service
    auto request_service = request_container->resolve<IRequestService>();
    request_service->handle_request("GET /api/users");
    
    // Create another child (isolated scope)
    auto request_container2 = root_container->create_child();
    static_cast<Container*>(request_container2.get())->register_scoped<IRequestService, RequestService>();
    
    auto request_service2 = request_container2->resolve<IRequestService>();
    request_service2->handle_request("POST /api/users");
    
    // Override service in child
    class CustomLogger : public ILogger {
    public:
        void log(const std::string& message) override {
            std::cout << "[CUSTOM] " << message << std::endl;
        }
    };
    
    static_cast<Container*>(request_container2.get())->register_instance<ILogger>(
        std::make_shared<CustomLogger>());
    
    auto custom_logger = request_container2->resolve<ILogger>();
    custom_logger->log("Overridden logger in child");
    
    return 0;
}

