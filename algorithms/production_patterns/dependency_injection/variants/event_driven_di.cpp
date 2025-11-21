/*
 * Event-Driven Dependency Injection
 * 
 * Source: Reactive programming, event-driven architectures, RxJS
 * Pattern: Dependency injection for event-driven and reactive systems
 * 
 * What Makes It Ingenious:
 * - Event streams: Dependencies as observable event streams
 * - Reactive composition: Compose dependencies reactively
 * - Async dependency resolution: Handle async dependencies
 * - Event sourcing: Dependencies based on events
 * - Used in reactive systems, event-driven architectures, microservices
 * 
 * When to Use:
 * - Event-driven architectures
 * - Reactive programming
 * - Async dependency resolution
 * - Microservices with events
 * - Real-time systems
 * - Stream processing
 * 
 * Real-World Usage:
 * - RxJS (Reactive Extensions)
 * - Event-driven microservices
 * - Reactive frameworks
 * - Real-time systems
 * - Stream processing systems
 * 
 * Time Complexity: O(1) for event subscription, O(n) for event propagation
 * Space Complexity: O(n) where n is number of subscribers
 */

#include <memory>
#include <functional>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <iostream>
#include <string>

// Simple event/observable pattern for DI
template<typename T>
class Observable {
public:
    using Observer = std::function<void(const T&)>;
    
private:
    std::vector<Observer> observers_;
    std::mutex mutex_;
    
public:
    void subscribe(Observer observer) {
        std::lock_guard<std::mutex> lock(mutex_);
        observers_.push_back(observer);
    }
    
    void notify(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& observer : observers_) {
            observer(value);
        }
    }
    
    void unsubscribe_all() {
        std::lock_guard<std::mutex> lock(mutex_);
        observers_.clear();
    }
};

// Event-driven service locator
class EventDrivenServiceLocator {
private:
    std::unordered_map<std::string, std::any> services_;
    std::unordered_map<std::string, Observable<std::any>> service_events_;
    std::mutex mutex_;
    
public:
    // Register service and notify subscribers
    template<typename T>
    void register_service(const std::string& key, std::shared_ptr<T> service) {
        std::lock_guard<std::mutex> lock(mutex_);
        services_[key] = service;
        
        // Notify subscribers of service registration
        if (service_events_.find(key) != service_events_.end()) {
            service_events_[key].notify(service);
        }
    }
    
    // Subscribe to service registration events
    template<typename T>
    void subscribe_service(const std::string& key, 
                          std::function<void(std::shared_ptr<T>)> callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // If service already exists, call immediately
        auto it = services_.find(key);
        if (it != services_.end()) {
            try {
                auto service = std::any_cast<std::shared_ptr<T>>(it->second);
                callback(service);
                return;
            } catch (...) {
                // Type mismatch, continue to subscription
            }
        }
        
        // Subscribe for future registration
        service_events_[key].subscribe([callback](const std::any& service) {
            try {
                auto typed_service = std::any_cast<std::shared_ptr<T>>(service);
                callback(typed_service);
            } catch (...) {
                // Type mismatch, ignore
            }
        });
    }
    
    // Resolve service (synchronous)
    template<typename T>
    std::shared_ptr<T> resolve(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = services_.find(key);
        if (it == services_.end()) {
            return nullptr;
        }
        
        try {
            return std::any_cast<std::shared_ptr<T>>(it->second);
        } catch (...) {
            return nullptr;
        }
    }
};

// Reactive dependency container
template<typename T>
class ReactiveDependency {
private:
    std::shared_ptr<T> value_;
    Observable<std::shared_ptr<T>> observable_;
    
public:
    ReactiveDependency() = default;
    
    explicit ReactiveDependency(std::shared_ptr<T> value) : value_(value) {
        observable_.notify(value_);
    }
    
    void set(std::shared_ptr<T> value) {
        value_ = value;
        observable_.notify(value_);
    }
    
    std::shared_ptr<T> get() const {
        return value_;
    }
    
    void subscribe(std::function<void(std::shared_ptr<T>)> observer) {
        observable_.subscribe(observer);
        if (value_) {
            observer(value_);
        }
    }
};

// Event-driven service that reacts to dependency changes
template<typename TDependency>
class EventDrivenService {
private:
    ReactiveDependency<TDependency> dependency_;
    std::function<void(std::shared_ptr<TDependency>)> handler_;
    
public:
    EventDrivenService(ReactiveDependency<TDependency> dependency,
                      std::function<void(std::shared_ptr<TDependency>)> handler)
        : dependency_(dependency), handler_(handler) {
        dependency_.subscribe(handler_);
    }
    
    void update_dependency(std::shared_ptr<TDependency> new_dep) {
        dependency_.set(new_dep);
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

// Example usage
int main() {
    // Event-driven service locator
    EventDrivenServiceLocator locator;
    
    // Subscribe to service before it's registered
    locator.subscribe_service<ILogger>("logger", [](auto logger) {
        logger->log("Logger service available!");
    });
    
    // Register service (triggers subscription)
    auto logger = std::make_shared<ConsoleLogger>();
    locator.register_service("logger", logger);
    
    // Reactive dependency
    ReactiveDependency<IConfigService> config_dep;
    
    // Service that reacts to dependency changes
    EventDrivenService<IConfigService> config_service(
        config_dep,
        [](auto config) {
            std::cout << "Config service updated" << std::endl;
        }
    );
    
    // Update dependency (triggers reaction)
    auto config = std::make_shared<ConfigService>();
    config_dep.set(config);
    
    return 0;
}

