/*
 * Decorator and Interceptor Pattern with DI
 * 
 * Source: AOP frameworks, Spring AOP, Castle DynamicProxy
 * Pattern: Decorator and interceptor patterns with dependency injection
 * 
 * What Makes It Ingenious:
 * - Decorator pattern: Add behavior without modifying original
 * - Interceptor pattern: Cross-cutting concerns (logging, caching, transactions)
 * - Chain of responsibility: Multiple decorators/interceptors
 * - DI integration: Decorators/interceptors injected via DI
 * - Used in AOP frameworks, enterprise applications, middleware
 * 
 * When to Use:
 * - Cross-cutting concerns (logging, caching, security)
 * - Need to add behavior without modifying existing code
 * - Aspect-oriented programming
 * - Middleware patterns
 * - Transaction management
 * 
 * Real-World Usage:
 * - Spring AOP (Java)
 * - Castle DynamicProxy (.NET)
 * - AspectJ (Java)
 * - Middleware in web frameworks
 * - Transaction interceptors
 * 
 * Time Complexity: O(n) where n is number of decorators/interceptors
 * Space Complexity: O(n) for decorator chain
 */

#include <memory>
#include <vector>
#include <functional>
#include <iostream>
#include <chrono>

// Example interface
class IDataService {
public:
    virtual ~IDataService() = default;
    virtual std::string fetch_data(const std::string& key) = 0;
    virtual void save_data(const std::string& key, const std::string& value) = 0;
};

// Concrete implementation
class DataService : public IDataService {
public:
    std::string fetch_data(const std::string& key) override {
        // Simulate data fetching
        return "data_for_" + key;
    }
    
    void save_data(const std::string& key, const std::string& value) override {
        // Simulate data saving
    }
};

// Pattern 1: Decorator Pattern with DI
class LoggingDecorator : public IDataService {
private:
    std::shared_ptr<IDataService> wrapped_;
    std::function<void(const std::string&)> logger_;
    
public:
    LoggingDecorator(std::shared_ptr<IDataService> wrapped,
                     std::function<void(const std::string&)> logger)
        : wrapped_(wrapped), logger_(logger) {}
    
    std::string fetch_data(const std::string& key) override {
        logger_("Fetching data for key: " + key);
        auto result = wrapped_->fetch_data(key);
        logger_("Fetched data: " + result);
        return result;
    }
    
    void save_data(const std::string& key, const std::string& value) override {
        logger_("Saving data for key: " + key);
        wrapped_->save_data(key, value);
        logger_("Saved data successfully");
    }
};

// Caching decorator
class CachingDecorator : public IDataService {
private:
    std::shared_ptr<IDataService> wrapped_;
    std::unordered_map<std::string, std::string> cache_;
    
public:
    explicit CachingDecorator(std::shared_ptr<IDataService> wrapped)
        : wrapped_(wrapped) {}
    
    std::string fetch_data(const std::string& key) override {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return it->second;
        }
        auto result = wrapped_->fetch_data(key);
        cache_[key] = result;
        return result;
    }
    
    void save_data(const std::string& key, const std::string& value) override {
        cache_.erase(key);  // Invalidate cache
        wrapped_->save_data(key, value);
    }
};

// Pattern 2: Interceptor Pattern
class Interceptor {
public:
    virtual ~Interceptor() = default;
    virtual void before(const std::string& method, const std::string& args) = 0;
    virtual void after(const std::string& method, const std::string& result) = 0;
    virtual void on_error(const std::string& method, const std::exception& error) = 0;
};

class LoggingInterceptor : public Interceptor {
private:
    std::function<void(const std::string&)> logger_;
    
public:
    explicit LoggingInterceptor(std::function<void(const std::string&)> logger)
        : logger_(logger) {}
    
    void before(const std::string& method, const std::string& args) override {
        logger_("Before " + method + " with args: " + args);
    }
    
    void after(const std::string& method, const std::string& result) override {
        logger_("After " + method + " with result: " + result);
    }
    
    void on_error(const std::string& method, const std::exception& error) override {
        logger_("Error in " + method + ": " + error.what());
    }
};

class TimingInterceptor : public Interceptor {
private:
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> start_times_;
    
public:
    void before(const std::string& method, const std::string& args) override {
        start_times_[method] = std::chrono::steady_clock::now();
    }
    
    void after(const std::string& method, const std::string& result) override {
        auto it = start_times_.find(method);
        if (it != start_times_.end()) {
            auto duration = std::chrono::steady_clock::now() - it->second;
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            std::cout << "Method " << method << " took " << ms << "ms" << std::endl;
            start_times_.erase(it);
        }
    }
    
    void on_error(const std::string& method, const std::exception& error) override {
        start_times_.erase(method);
    }
};

// Interceptor-based service proxy
class InterceptedDataService : public IDataService {
private:
    std::shared_ptr<IDataService> target_;
    std::vector<std::shared_ptr<Interceptor>> interceptors_;
    
public:
    InterceptedDataService(std::shared_ptr<IDataService> target,
                          std::vector<std::shared_ptr<Interceptor>> interceptors)
        : target_(target), interceptors_(interceptors) {}
    
    std::string fetch_data(const std::string& key) override {
        // Execute before interceptors
        for (auto& interceptor : interceptors_) {
            interceptor->before("fetch_data", key);
        }
        
        try {
            auto result = target_->fetch_data(key);
            
            // Execute after interceptors
            for (auto& interceptor : interceptors_) {
                interceptor->after("fetch_data", result);
            }
            
            return result;
        } catch (const std::exception& e) {
            // Execute error interceptors
            for (auto& interceptor : interceptors_) {
                interceptor->on_error("fetch_data", e);
            }
            throw;
        }
    }
    
    void save_data(const std::string& key, const std::string& value) override {
        // Execute before interceptors
        for (auto& interceptor : interceptors_) {
            interceptor->before("save_data", key + "=" + value);
        }
        
        try {
            target_->save_data(key, value);
            
            // Execute after interceptors
            for (auto& interceptor : interceptors_) {
                interceptor->after("save_data", "success");
            }
        } catch (const std::exception& e) {
            // Execute error interceptors
            for (auto& interceptor : interceptors_) {
                interceptor->on_error("save_data", e);
            }
            throw;
        }
    }
};

// Pattern 3: Chain of Decorators
template<typename T>
class DecoratorChain {
public:
    static std::shared_ptr<T> build_chain(
        std::shared_ptr<T> base,
        std::vector<std::function<std::shared_ptr<T>(std::shared_ptr<T>)>> decorators) {
        auto current = base;
        for (auto& decorator : decorators) {
            current = decorator(current);
        }
        return current;
    }
};

// Example usage
int main() {
    // Pattern 1: Decorator pattern
    auto base_service = std::make_shared<DataService>();
    
    // Add logging decorator
    auto logging_service = std::make_shared<LoggingDecorator>(
        base_service,
        [](const std::string& msg) { std::cout << "[LOG] " << msg << std::endl; }
    );
    
    // Add caching decorator
    auto cached_service = std::make_shared<CachingDecorator>(logging_service);
    
    cached_service->fetch_data("key1");
    cached_service->fetch_data("key1");  // From cache
    
    // Pattern 2: Interceptor pattern
    auto target_service = std::make_shared<DataService>();
    std::vector<std::shared_ptr<Interceptor>> interceptors = {
        std::make_shared<LoggingInterceptor>(
            [](const std::string& msg) { std::cout << "[INTERCEPTOR] " << msg << std::endl; }
        ),
        std::make_shared<TimingInterceptor>()
    };
    
    auto intercepted_service = std::make_shared<InterceptedDataService>(
        target_service, interceptors
    );
    
    intercepted_service->fetch_data("key2");
    intercepted_service->save_data("key3", "value3");
    
    // Pattern 3: Chain of decorators
    auto chained_service = DecoratorChain<IDataService>::build_chain(
        base_service,
        {
            [](std::shared_ptr<IDataService> service) {
                return std::make_shared<LoggingDecorator>(
                    service,
                    [](const std::string& msg) { std::cout << "[CHAIN] " << msg << std::endl; }
                );
            },
            [](std::shared_ptr<IDataService> service) {
                return std::make_shared<CachingDecorator>(service);
            }
        }
    );
    
    chained_service->fetch_data("key4");
    
    return 0;
}

