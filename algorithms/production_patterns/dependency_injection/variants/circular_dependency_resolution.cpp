/*
 * Circular Dependency Resolution - Dependency Injection
 * 
 * Source: DI frameworks, Spring Framework, .NET Core DI
 * Pattern: Resolve circular dependencies using proxies, lazy loading, or setter injection
 * 
 * What Makes It Ingenious:
 * - Proxy-based: Use proxies to break circular dependencies
 * - Lazy loading: Defer dependency resolution until needed
 * - Setter injection: Break cycles with optional dependencies
 * - Interface segregation: Split interfaces to break cycles
 * - Used in complex dependency graphs, legacy code integration
 * 
 * When to Use:
 * - Circular dependencies unavoidable
 * - Legacy code integration
 * - Complex domain models
 * - Event-driven architectures
 * - Observer patterns
 * 
 * Real-World Usage:
 * - Spring Framework (circular proxy)
 * - .NET Core DI (circular dependency detection)
 * - Autofac (circular dependency resolution)
 * - Enterprise applications
 * - Domain-driven design
 * 
 * Time Complexity: O(1) for proxy creation, O(n) for resolution
 * Space Complexity: O(n) where n is dependency depth
 */

#include <memory>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <mutex>
#include <iostream>
#include <any>

// Forward declaration proxy
template<typename T>
class CircularDependencyProxy;

// Circular dependency resolver
class CircularDependencyResolver {
private:
    std::unordered_map<std::type_index, std::any> instances_;
    std::unordered_map<std::type_index, std::function<std::any()>> factories_;
    std::unordered_map<std::type_index, bool> creating_;
    std::mutex mutex_;
    
    template<typename T>
    std::type_index get_type_index() {
        return std::type_index(typeid(T));
    }
    
public:
    template<typename T>
    void register_factory(std::function<std::shared_ptr<T>()> factory) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        factories_[type_idx] = [factory]() -> std::any {
            return std::make_any<std::shared_ptr<T>>(factory());
        };
    }
    
    template<typename T>
    std::shared_ptr<T> resolve() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        
        // Check if already created
        auto instance_it = instances_.find(type_idx);
        if (instance_it != instances_.end()) {
            return std::any_cast<std::shared_ptr<T>>(instance_it->second);
        }
        
        // Check if currently creating (circular dependency)
        if (creating_[type_idx]) {
            // Return proxy for circular dependency
            return std::make_shared<CircularDependencyProxy<T>>(*this);
        }
        
        // Mark as creating
        creating_[type_idx] = true;
        
        // Create instance
        auto factory_it = factories_.find(type_idx);
        if (factory_it == factories_.end()) {
            creating_[type_idx] = false;
            throw std::runtime_error("Service not registered: " + std::string(typeid(T).name()));
        }
        
        auto instance = std::any_cast<std::shared_ptr<T>>(factory_it->second());
        instances_[type_idx] = std::make_any<std::shared_ptr<T>>(instance);
        creating_[type_idx] = false;
        
        return instance;
    }
    
    template<typename T>
    void set_instance(std::shared_ptr<T> instance) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        instances_[type_idx] = std::make_any<std::shared_ptr<T>>(instance);
    }
};

// Proxy for circular dependencies
template<typename T>
class CircularDependencyProxy : public T {
private:
    CircularDependencyResolver& resolver_;
    mutable std::shared_ptr<T> real_instance_;
    
    std::shared_ptr<T> get_real_instance() const {
        if (!real_instance_) {
            real_instance_ = resolver_.resolve<T>();
        }
        return real_instance_;
    }
    
public:
    explicit CircularDependencyProxy(CircularDependencyResolver& resolver)
        : resolver_(resolver) {}
    
    // Delegate all calls to real instance
    // Note: This is simplified - in practice, would use more sophisticated proxying
};

// Lazy dependency wrapper
template<typename T>
class LazyDependency {
private:
    std::function<std::shared_ptr<T>()> factory_;
    mutable std::shared_ptr<T> instance_;
    
public:
    explicit LazyDependency(std::function<std::shared_ptr<T>()> factory)
        : factory_(factory) {}
    
    std::shared_ptr<T> get() const {
        if (!instance_) {
            instance_ = factory_();
        }
        return instance_;
    }
    
    T& operator*() const {
        return *get();
    }
    
    T* operator->() const {
        return get().get();
    }
};

// Example: Circular dependency scenario
class IUserService {
public:
    virtual ~IUserService() = default;
    virtual void create_user(const std::string& name) = 0;
    virtual void notify_user_created(const std::string& name) = 0;
};

class INotificationService {
public:
    virtual ~INotificationService() = default;
    virtual void send_notification(const std::string& message) = 0;
    virtual void register_user_service(IUserService* user_service) = 0;
};

// UserService depends on NotificationService
class UserService : public IUserService {
private:
    std::shared_ptr<INotificationService> notification_service_;
    
public:
    explicit UserService(std::shared_ptr<INotificationService> notification_service)
        : notification_service_(notification_service) {}
    
    void create_user(const std::string& name) override {
        std::cout << "Creating user: " << name << std::endl;
        notify_user_created(name);
    }
    
    void notify_user_created(const std::string& name) override {
        notification_service_->send_notification("User created: " + name);
    }
};

// NotificationService depends on UserService (circular!)
class NotificationService : public INotificationService {
private:
    LazyDependency<IUserService> user_service_;
    
public:
    explicit NotificationService(LazyDependency<IUserService> user_service)
        : user_service_(user_service) {}
    
    void send_notification(const std::string& message) override {
        std::cout << "[NOTIFICATION] " << message << std::endl;
    }
    
    void register_user_service(IUserService* user_service) override {
        // Can register user service if needed
    }
};

// Alternative: Setter injection to break cycle
class NotificationServiceSetter : public INotificationService {
private:
    IUserService* user_service_;  // Raw pointer to break cycle
    
public:
    NotificationServiceSetter() : user_service_(nullptr) {}
    
    void set_user_service(IUserService* user_service) {
        user_service_ = user_service;
    }
    
    void send_notification(const std::string& message) override {
        std::cout << "[NOTIFICATION] " << message << std::endl;
    }
    
    void register_user_service(IUserService* user_service) override {
        user_service_ = user_service;
    }
};

// Example usage
int main() {
    // Method 1: Lazy loading to break circular dependency
    CircularDependencyResolver resolver;
    
    resolver.register_factory<INotificationService>([&resolver]() {
        auto user_service_lazy = LazyDependency<IUserService>([&resolver]() {
            return resolver.resolve<IUserService>();
        });
        return std::make_shared<NotificationService>(user_service_lazy);
    });
    
    resolver.register_factory<IUserService>([&resolver]() {
        return std::make_shared<UserService>(resolver.resolve<INotificationService>());
    });
    
    // Resolve (circular dependency handled)
    auto user_service = resolver.resolve<IUserService>();
    user_service->create_user("John Doe");
    
    // Method 2: Setter injection
    auto notification_service_setter = std::make_shared<NotificationServiceSetter>();
    auto user_service_setter = std::make_shared<UserService>(
        std::static_pointer_cast<INotificationService>(notification_service_setter));
    notification_service_setter->set_user_service(user_service_setter.get());
    
    user_service_setter->create_user("Jane Doe");
    
    return 0;
}

