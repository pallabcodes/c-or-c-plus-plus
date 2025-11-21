/*
 * Lazy Proxy Dependency Injection
 * 
 * Source: Spring Framework, .NET Core DI, Autofac
 * Pattern: Use lazy loading and proxies to defer dependency resolution
 * 
 * What Makes It Ingenious:
 * - Lazy initialization: Dependencies created only when accessed
 * - Proxy pattern: Intercept calls to lazy dependencies
 * - Performance: Avoid creating unused dependencies
 * - Circular dependency resolution: Break cycles with lazy loading
 * - Used in frameworks, large applications, performance-critical code
 * 
 * When to Use:
 * - Expensive dependency creation
 * - Optional dependencies
 * - Circular dependencies
 * - Performance optimization
 * - Large dependency graphs
 * 
 * Real-World Usage:
 * - Spring Framework (Lazy<T>)
 * - .NET Core DI (Lazy<T>)
 * - Autofac (Lazy<T>)
 * - Enterprise applications
 * - Performance-critical systems
 * 
 * Time Complexity: O(1) for proxy creation, O(n) for first access
 * Space Complexity: O(1) until first access, then O(n)
 */

#include <memory>
#include <functional>
#include <mutex>
#include <iostream>

// Lazy proxy template
template<typename T>
class LazyProxy {
private:
    mutable std::function<std::shared_ptr<T>()> factory_;
    mutable std::shared_ptr<T> instance_;
    mutable std::mutex mutex_;
    mutable bool initialized_;
    
public:
    explicit LazyProxy(std::function<std::shared_ptr<T>()> factory)
        : factory_(factory), initialized_(false) {}
    
    // Get instance (lazy initialization)
    std::shared_ptr<T> get() const {
        if (!initialized_) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!initialized_) {
                instance_ = factory_();
                initialized_ = true;
            }
        }
        return instance_;
    }
    
    // Dereference operators
    T& operator*() const {
        return *get();
    }
    
    T* operator->() const {
        return get().get();
    }
    
    // Check if initialized
    bool is_initialized() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return initialized_;
    }
    
    // Reset (for testing)
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        instance_.reset();
        initialized_ = false;
    }
};

// Lazy factory for creating lazy dependencies
template<typename T>
class LazyFactory {
private:
    std::function<std::shared_ptr<T>()> factory_;
    
public:
    explicit LazyFactory(std::function<std::shared_ptr<T>()> factory)
        : factory_(factory) {}
    
    LazyProxy<T> create() {
        return LazyProxy<T>(factory_);
    }
};

// Example interfaces
class IExpensiveService {
public:
    virtual ~IExpensiveService() = default;
    virtual void do_work() = 0;
};

class ExpensiveService : public IExpensiveService {
public:
    ExpensiveService() {
        std::cout << "ExpensiveService created (expensive operation)" << std::endl;
    }
    
    void do_work() override {
        std::cout << "ExpensiveService doing work" << std::endl;
    }
};

class IOptionalService {
public:
    virtual ~IOptionalService() = default;
    virtual void optional_operation() = 0;
};

class OptionalService : public IOptionalService {
public:
    OptionalService() {
        std::cout << "OptionalService created" << std::endl;
    }
    
    void optional_operation() override {
        std::cout << "Optional operation executed" << std::endl;
    }
};

// Service using lazy dependencies
class BusinessService {
private:
    LazyProxy<IExpensiveService> expensive_service_;
    LazyProxy<IOptionalService> optional_service_;
    
public:
    BusinessService(LazyProxy<IExpensiveService> expensive_service,
                   LazyProxy<IOptionalService> optional_service)
        : expensive_service_(expensive_service), optional_service_(optional_service) {
        std::cout << "BusinessService created (dependencies not yet created)" << std::endl;
    }
    
    void do_business_logic() {
        std::cout << "Doing business logic..." << std::endl;
        // Expensive service created only when needed
        expensive_service_->do_work();
    }
    
    void do_optional_logic() {
        // Optional service created only if this method is called
        if (optional_service_.is_initialized()) {
            optional_service_->optional_operation();
        } else {
            std::cout << "Optional service not yet initialized" << std::endl;
            optional_service_->optional_operation();
        }
    }
};

// Lazy dependency container
template<typename T>
class LazyContainer {
private:
    std::function<std::shared_ptr<T>()> factory_;
    
public:
    explicit LazyContainer(std::function<std::shared_ptr<T>()> factory)
        : factory_(factory) {}
    
    LazyProxy<T> create_lazy() {
        return LazyProxy<T>(factory_);
    }
    
    std::shared_ptr<T> create_eager() {
        return factory_();
    }
};

// Example usage
int main() {
    // Create lazy factories
    LazyFactory<IExpensiveService> expensive_factory([]() {
        return std::make_shared<ExpensiveService>();
    });
    
    LazyFactory<IOptionalService> optional_factory([]() {
        return std::make_shared<OptionalService>();
    });
    
    // Create lazy proxies
    auto expensive_lazy = expensive_factory.create();
    auto optional_lazy = optional_factory.create();
    
    // Create business service (dependencies not created yet)
    BusinessService business_service(expensive_lazy, optional_lazy);
    
    std::cout << "\n--- Before using expensive service ---" << std::endl;
    std::cout << "Expensive service initialized: " 
              << expensive_lazy.is_initialized() << std::endl;
    
    // Use business logic (expensive service created now)
    std::cout << "\n--- Using business logic ---" << std::endl;
    business_service.do_business_logic();
    
    std::cout << "\n--- After using expensive service ---" << std::endl;
    std::cout << "Expensive service initialized: " 
              << expensive_lazy.is_initialized() << std::endl;
    
    // Optional service still not created
    std::cout << "\n--- Before using optional service ---" << std::endl;
    std::cout << "Optional service initialized: " 
              << optional_lazy.is_initialized() << std::endl;
    
    // Use optional logic (optional service created now)
    std::cout << "\n--- Using optional logic ---" << std::endl;
    business_service.do_optional_logic();
    
    std::cout << "\n--- After using optional service ---" << std::endl;
    std::cout << "Optional service initialized: " 
              << optional_lazy.is_initialized() << std::endl;
    
    return 0;
}

