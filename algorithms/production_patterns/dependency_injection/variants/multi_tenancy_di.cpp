/*
 * Multi-Tenancy Dependency Injection
 * 
 * Source: SaaS applications, cloud services, multi-tenant architectures
 * Pattern: Per-tenant dependency isolation and resolution
 * 
 * What Makes It Ingenious:
 * - Tenant isolation: Separate dependency instances per tenant
 * - Tenant context: Automatic tenant-aware resolution
 * - Scoped services: Per-tenant service scopes
 * - Tenant switching: Dynamic tenant context switching
 * - Used in SaaS applications, cloud services, multi-tenant systems
 * 
 * When to Use:
 * - Multi-tenant applications
 * - SaaS platforms
 * - Per-tenant configuration
 * - Tenant-specific services
 * - Data isolation requirements
 * - Cloud services
 * 
 * Real-World Usage:
 * - SaaS platforms (Salesforce, Microsoft 365)
 * - Cloud services (AWS, Azure multi-tenant)
 * - Enterprise applications
 * - Database per tenant systems
 * - Configuration per tenant
 * 
 * Time Complexity: O(1) for tenant lookup, O(n) for resolution
 * Space Complexity: O(n * m) where n is tenants, m is services per tenant
 */

#include <memory>
#include <unordered_map>
#include <string>
#include <functional>
#include <mutex>
#include <iostream>
#include <any>
#include <thread>

// Tenant context
class TenantContext {
private:
    std::string tenant_id_;
    static thread_local TenantContext* current_;
    
public:
    explicit TenantContext(const std::string& tenant_id) : tenant_id_(tenant_id) {
        current_ = this;
    }
    
    ~TenantContext() {
        if (current_ == this) {
            current_ = nullptr;
        }
    }
    
    const std::string& get_tenant_id() const {
        return tenant_id_;
    }
    
    static TenantContext* get_current() {
        return current_;
    }
    
    static std::string get_current_tenant_id() {
        if (current_) {
            return current_->tenant_id_;
        }
        return "default";
    }
};

thread_local TenantContext* TenantContext::current_ = nullptr;

// Multi-tenant service container
class MultiTenantContainer {
public:
    enum class Lifetime {
        Singleton,      // Single instance per tenant
        Transient,      // New instance per resolution
        Shared          // Shared across all tenants
    };
    
    struct ServiceRegistration {
        Lifetime lifetime;
        std::function<std::any(const std::string& tenant_id)> factory;
    };
    
private:
    // Per-tenant service instances
    std::unordered_map<std::string, std::unordered_map<std::string, std::any>> tenant_services_;
    // Shared services (across all tenants)
    std::unordered_map<std::string, std::any> shared_services_;
    // Service registrations
    std::unordered_map<std::string, ServiceRegistration> registrations_;
    std::mutex mutex_;
    
    template<typename T>
    std::string get_type_key() {
        return typeid(T).name();
    }
    
public:
    // Register service with tenant-aware factory
    template<typename TInterface>
    void register_service(Lifetime lifetime,
                         std::function<std::shared_ptr<TInterface>(const std::string& tenant_id)> factory) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto key = get_type_key<TInterface>();
        
        registrations_[key] = ServiceRegistration{
            lifetime,
            [factory](const std::string& tenant_id) -> std::any {
                return std::make_any<std::shared_ptr<TInterface>>(factory(tenant_id));
            }
        };
    }
    
    // Register shared service (same instance for all tenants)
    template<typename TInterface>
    void register_shared_service(std::function<std::shared_ptr<TInterface>()> factory) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto key = get_type_key<TInterface>();
        
        auto instance = factory();
        shared_services_[key] = std::make_any<std::shared_ptr<TInterface>>(instance);
        
        registrations_[key] = ServiceRegistration{
            Lifetime::Shared,
            [instance](const std::string&) -> std::any {
                return std::make_any<std::shared_ptr<TInterface>>(instance);
            }
        };
    }
    
    // Resolve service for current tenant
    template<typename T>
    std::shared_ptr<T> resolve() {
        std::string tenant_id = TenantContext::get_current_tenant_id();
        return resolve_for_tenant<T>(tenant_id);
    }
    
    // Resolve service for specific tenant
    template<typename T>
    std::shared_ptr<T> resolve_for_tenant(const std::string& tenant_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto key = get_type_key<T>();
        
        auto reg_it = registrations_.find(key);
        if (reg_it == registrations_.end()) {
            throw std::runtime_error("Service not registered: " + std::string(typeid(T).name()));
        }
        
        auto& registration = reg_it->second;
        
        // Handle shared services
        if (registration.lifetime == Lifetime::Shared) {
            auto shared_it = shared_services_.find(key);
            if (shared_it != shared_services_.end()) {
                return std::any_cast<std::shared_ptr<T>>(shared_it->second);
            }
        }
        
        // Handle per-tenant services
        auto& tenant_map = tenant_services_[tenant_id];
        auto tenant_it = tenant_map.find(key);
        
        if (registration.lifetime == Lifetime::Singleton) {
            // Return existing or create new
            if (tenant_it == tenant_map.end()) {
                auto instance = registration.factory(tenant_id);
                tenant_map[key] = instance;
                return std::any_cast<std::shared_ptr<T>>(instance);
            }
            return std::any_cast<std::shared_ptr<T>>(tenant_it->second);
        } else {
            // Transient - always create new
            auto instance = registration.factory(tenant_id);
            return std::any_cast<std::shared_ptr<T>>(instance);
        }
    }
    
    // Clear services for a tenant
    void clear_tenant(const std::string& tenant_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        tenant_services_.erase(tenant_id);
    }
};

// Example: Tenant-aware logger
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const std::string& message) = 0;
};

class TenantLogger : public ILogger {
private:
    std::string tenant_id_;
    
public:
    explicit TenantLogger(const std::string& tenant_id) : tenant_id_(tenant_id) {}
    
    void log(const std::string& message) override {
        std::cout << "[" << tenant_id_ << "] " << message << std::endl;
    }
};

// Example: Shared service
class ISharedService {
public:
    virtual ~ISharedService() = default;
    virtual void do_work() = 0;
};

class SharedService : public ISharedService {
public:
    void do_work() override {
        std::cout << "Shared service working" << std::endl;
    }
};

// Example usage
int main() {
    MultiTenantContainer container;
    
    // Register per-tenant service
    container.register_service<ILogger>(
        MultiTenantContainer::Lifetime::Singleton,
        [](const std::string& tenant_id) {
            return std::make_shared<TenantLogger>(tenant_id);
        }
    );
    
    // Register shared service
    container.register_shared_service<ISharedService>([]() {
        return std::make_shared<SharedService>();
    });
    
    // Use tenant context
    {
        TenantContext context("tenant1");
        auto logger = container.resolve<ILogger>();
        logger->log("Message from tenant1");
        
        auto shared = container.resolve<ISharedService>();
        shared->do_work();
    }
    
    {
        TenantContext context("tenant2");
        auto logger = container.resolve<ILogger>();
        logger->log("Message from tenant2");
        
        // Same shared service instance
        auto shared = container.resolve<ISharedService>();
        shared->do_work();
    }
    
    return 0;
}

