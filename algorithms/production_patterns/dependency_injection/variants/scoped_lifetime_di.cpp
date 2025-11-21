/*
 * Scoped Lifetime Dependency Injection
 * 
 * Source: .NET Core DI, Spring Framework, ASP.NET Core
 * Pattern: Scoped service lifetimes with dependency injection
 * 
 * What Makes It Ingenious:
 * - Request scope: Single instance per HTTP request
 * - Thread scope: Single instance per thread
 * - Transaction scope: Single instance per transaction
 * - Custom scopes: Application-defined scopes
 * - Automatic disposal: Scoped services disposed when scope ends
 * - Used in web frameworks, transaction management, multi-threaded apps
 * 
 * When to Use:
 * - Web applications (request scope)
 * - Multi-threaded applications (thread scope)
 * - Transaction management (transaction scope)
 * - Unit of Work pattern
 * - Database context per request
 * 
 * Real-World Usage:
 * - ASP.NET Core (request scope)
 * - Spring Framework (request scope)
 * - Entity Framework (DbContext scope)
 * - Transaction management
 * - Web frameworks
 * 
 * Time Complexity: O(1) for scope creation, O(n) for resolution
 * Space Complexity: O(n) where n is number of scoped services
 */

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <mutex>
#include <thread>
#include <iostream>
#include <any>
#include <functional>

class ScopedLifetimeContainer {
public:
    enum class Lifetime {
        Singleton,  // Single instance for entire application
        Transient,  // New instance every time
        Scoped,     // Single instance per scope
        ThreadLocal // Single instance per thread
    };
    
    // Scope identifier
    class Scope {
    private:
        std::string scope_id_;
        static thread_local std::string current_scope_;
        
    public:
        explicit Scope(const std::string& id) : scope_id_(id) {
            current_scope_ = scope_id_;
        }
        
        ~Scope() {
            current_scope_.clear();
        }
        
        static std::string get_current_scope() {
            return current_scope_;
        }
        
        std::string get_id() const {
            return scope_id_;
        }
    };
    
private:
    struct ServiceRegistration {
        Lifetime lifetime;
        std::function<std::any()> factory;
        std::any singleton_instance;
        std::unordered_map<std::string, std::any> scoped_instances;
        std::mutex mutex_;
        
        // Thread-local storage helper
        static std::unordered_map<std::string, std::any>& get_thread_local_instances() {
            thread_local static std::unordered_map<std::string, std::any> instances;
            return instances;
        }
        
        ServiceRegistration(Lifetime lt, std::function<std::any()> f)
            : lifetime(lt), factory(f) {}
        
        std::any get_instance(const std::string& scope_id) {
            std::lock_guard<std::mutex> lock(mutex_);
            
            switch (lifetime) {
                case Lifetime::Singleton:
                    if (!singleton_instance.has_value()) {
                        singleton_instance = factory();
                    }
                    return singleton_instance;
                    
                case Lifetime::Transient:
                    return factory();
                    
                case Lifetime::Scoped:
                    if (scope_id.empty()) {
                        throw std::runtime_error("No active scope");
                    }
                    auto it = scoped_instances.find(scope_id);
                    if (it == scoped_instances.end()) {
                        scoped_instances[scope_id] = factory();
                    }
                    return scoped_instances[scope_id];
                    
                case Lifetime::ThreadLocal: {
                    auto& tl_instances = get_thread_local_instances();
                    auto tl_it = tl_instances.find(scope_id);
                    if (tl_it == tl_instances.end()) {
                        tl_instances[scope_id] = factory();
                    }
                    return tl_instances[scope_id];
                }
            }
            return factory();
        }
        
        void clear_scope(const std::string& scope_id) {
            std::lock_guard<std::mutex> lock(mutex_);
            scoped_instances.erase(scope_id);
        }
    };
    
    std::unordered_map<std::type_index, ServiceRegistration> services_;
    std::mutex mutex_;
    
    template<typename T>
    std::type_index get_type_index() {
        return std::type_index(typeid(T));
    }
    
public:
    // Register service with lifetime
    template<typename TInterface>
    void register_service(Lifetime lifetime, std::function<std::shared_ptr<TInterface>()> factory) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<TInterface>();
        
        services_[type_idx] = ServiceRegistration(
            lifetime,
            [factory]() -> std::any {
                return std::make_any<std::shared_ptr<TInterface>>(factory());
            }
        );
    }
    
    // Resolve service (uses current scope)
    template<typename T>
    std::shared_ptr<T> resolve(const std::string& scope_id = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        
        auto it = services_.find(type_idx);
        if (it == services_.end()) {
            throw std::runtime_error("Service not registered: " + std::string(typeid(T).name()));
        }
        
        std::string active_scope = scope_id.empty() ? Scope::get_current_scope() : scope_id;
        auto instance = it->second.get_instance(active_scope);
        return std::any_cast<std::shared_ptr<T>>(instance);
    }
    
    // Clear scope (dispose scoped instances)
    void clear_scope(const std::string& scope_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& pair : services_) {
            pair.second.clear_scope(scope_id);
        }
    }
};

// Thread-local storage for current scope
thread_local std::string ScopedLifetimeContainer::Scope::current_scope_;

// Example: Database context (typically scoped per request)
class IDbContext {
public:
    virtual ~IDbContext() = default;
    virtual void save_changes() = 0;
    virtual std::string get_connection_string() = 0;
};

class DbContext : public IDbContext {
private:
    std::string connection_string_;
    
public:
    explicit DbContext(const std::string& connection_string)
        : connection_string_(connection_string) {
        std::cout << "DbContext created: " << connection_string_ << std::endl;
    }
    
    ~DbContext() {
        std::cout << "DbContext disposed: " << connection_string_ << std::endl;
    }
    
    void save_changes() override {
        std::cout << "Saving changes to: " << connection_string_ << std::endl;
    }
    
    std::string get_connection_string() override {
        return connection_string_;
    }
};

// Example: Repository (scoped per request)
class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    virtual void add_user(const std::string& email) = 0;
};

class UserRepository : public IUserRepository {
private:
    std::shared_ptr<IDbContext> db_context_;
    
public:
    explicit UserRepository(std::shared_ptr<IDbContext> db_context)
        : db_context_(db_context) {}
    
    void add_user(const std::string& email) override {
        std::cout << "Adding user: " << email 
                  << " (using context: " << db_context_->get_connection_string() << ")" << std::endl;
    }
};

// Example: Service using scoped dependencies
class UserService {
private:
    std::shared_ptr<IUserRepository> repository_;
    std::shared_ptr<IDbContext> db_context_;
    
public:
    UserService(std::shared_ptr<IUserRepository> repository,
                std::shared_ptr<IDbContext> db_context)
        : repository_(repository), db_context_(db_context) {}
    
    void register_user(const std::string& email) {
        repository_->add_user(email);
        db_context_->save_changes();
    }
};

// Example usage
int main() {
    ScopedLifetimeContainer container;
    
    // Register services with different lifetimes
    container.register_service<IDbContext>(
        ScopedLifetimeContainer::Lifetime::Scoped,
        []() { return std::make_shared<DbContext>("connection_string_1"); }
    );
    
    container.register_service<IUserRepository>(
        ScopedLifetimeContainer::Lifetime::Scoped,
        [&container]() {
            return std::make_shared<UserRepository>(
                container.resolve<IDbContext>("request_1")
            );
        }
    );
    
    // UserService would be created manually with resolved dependencies
    
    // Simulate request scope
    {
        ScopedLifetimeContainer::Scope request_scope("request_1");
        
        auto db_context1 = container.resolve<IDbContext>();
        auto db_context2 = container.resolve<IDbContext>();
        
        // Should be same instance (scoped)
        std::cout << "Same instance: " 
                  << (db_context1.get() == db_context2.get() ? "Yes" : "No") << std::endl;
        
        auto user_service = std::make_shared<UserService>(
            container.resolve<IUserRepository>(),
            container.resolve<IDbContext>()
        );
        user_service->register_user("user1@example.com");
    }
    
    // New request scope
    {
        ScopedLifetimeContainer::Scope request_scope("request_2");
        
        auto db_context3 = container.resolve<IDbContext>();
        // Different instance from request_1
        
        auto user_service = std::make_shared<UserService>(
            container.resolve<IUserRepository>(),
            container.resolve<IDbContext>()
        );
        user_service->register_user("user2@example.com");
    }
    
    // Clear scope
    container.clear_scope("request_1");
    container.clear_scope("request_2");
    
    return 0;
}

