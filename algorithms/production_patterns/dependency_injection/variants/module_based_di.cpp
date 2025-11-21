/*
 * Module-Based Dependency Injection
 * 
 * Source: Angular modules, .NET Core modules, Spring modules
 * Pattern: Organize services into modules with isolated dependency graphs
 * 
 * What Makes It Ingenious:
 * - Module isolation: Each module has its own service container
 * - Module dependencies: Modules can depend on other modules
 * - Lazy loading: Modules loaded on demand
 * - Feature modules: Organize by feature/domain
 * - Used in large applications, microservices, plugin systems
 * 
 * When to Use:
 * - Large applications with many services
 * - Feature-based architecture
 * - Plugin systems
 * - Microservices
 * - Need module isolation
 * 
 * Real-World Usage:
 * - Angular modules (NgModule)
 * - .NET Core modules
 * - Spring Framework modules
 * - Plugin architectures
 * - Microservice architectures
 * 
 * Time Complexity: O(1) for module registration, O(n) for resolution
 * Space Complexity: O(n) where n is number of modules
 */

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include <typeindex>
#include <mutex>
#include <iostream>
#include <any>

// Forward declarations
class IModule;
class ModuleContainer;

// Module interface
class IModule {
public:
    virtual ~IModule() = default;
    virtual void configure(ModuleContainer& container) = 0;
    virtual std::string get_name() const = 0;
    virtual std::vector<std::string> get_dependencies() const = 0;
};

// Module container (simplified IoC container)
class ModuleContainer {
private:
    std::unordered_map<std::type_index, std::any> services_;
    std::mutex mutex_;
    
    template<typename T>
    std::type_index get_type_index() {
        return std::type_index(typeid(T));
    }
    
public:
    template<typename TInterface, typename TImplementation>
    void register_singleton() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<TInterface>();
        services_[type_idx] = std::make_any<std::shared_ptr<TInterface>>(
            std::make_shared<TImplementation>());
    }
    
    template<typename TInterface>
    void register_instance(std::shared_ptr<TInterface> instance) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<TInterface>();
        services_[type_idx] = std::make_any<std::shared_ptr<TInterface>>(instance);
    }
    
    template<typename T>
    std::shared_ptr<T> resolve() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        auto it = services_.find(type_idx);
        if (it == services_.end()) {
            throw std::runtime_error("Service not registered: " + std::string(typeid(T).name()));
        }
        return std::any_cast<std::shared_ptr<T>>(it->second);
    }
    
    template<typename T>
    bool is_registered() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        return services_.find(type_idx) != services_.end();
    }
};

// Module manager
class ModuleManager {
private:
    std::unordered_map<std::string, std::shared_ptr<IModule>> modules_;
    std::unordered_map<std::string, ModuleContainer> containers_;
    std::vector<std::string> load_order_;
    std::mutex mutex_;
    
    void load_module_dependencies(const std::string& module_name,
                                  std::unordered_set<std::string>& loaded) {
        auto it = modules_.find(module_name);
        if (it == modules_.end()) {
            throw std::runtime_error("Module not found: " + module_name);
        }
        
        auto module = it->second;
        auto dependencies = module->get_dependencies();
        
        // Load dependencies first
        for (const auto& dep : dependencies) {
            if (loaded.find(dep) == loaded.end()) {
                load_module_dependencies(dep, loaded);
            }
        }
        
        // Load this module
        if (loaded.find(module_name) == loaded.end()) {
            containers_[module_name] = ModuleContainer();
            module->configure(containers_[module_name]);
            loaded.insert(module_name);
            load_order_.push_back(module_name);
        }
    }
    
public:
    void register_module(std::shared_ptr<IModule> module) {
        std::lock_guard<std::mutex> lock(mutex_);
        modules_[module->get_name()] = module;
    }
    
    void load_module(const std::string& module_name) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::unordered_set<std::string> loaded;
        load_module_dependencies(module_name, loaded);
    }
    
    ModuleContainer& get_container(const std::string& module_name) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = containers_.find(module_name);
        if (it == containers_.end()) {
            throw std::runtime_error("Module not loaded: " + module_name);
        }
        return it->second;
    }
    
    std::vector<std::string> get_load_order() const {
        return load_order_;
    }
};

// Example: Logger module
class LoggerModule : public IModule {
public:
    std::string get_name() const override {
        return "Logger";
    }
    
    std::vector<std::string> get_dependencies() const override {
        return {};  // No dependencies
    }
    
    void configure(ModuleContainer& container) override {
        container.register_singleton<class ILogger, class ConsoleLogger>();
    }
};

// Example: Email module (depends on Logger)
class EmailModule : public IModule {
public:
    std::string get_name() const override {
        return "Email";
    }
    
    std::vector<std::string> get_dependencies() const override {
        return {"Logger"};  // Depends on Logger module
    }
    
    void configure(ModuleContainer& container) override {
        // Email module can use Logger from its own container
        // In real implementation, would resolve from parent or dependency modules
        container.register_singleton<class IEmailService, class EmailService>();
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
public:
    void send(const std::string& to, const std::string& subject) override {
        std::cout << "Sending email to: " << to << " - " << subject << std::endl;
    }
};

// Example usage
int main() {
    ModuleManager manager;
    
    // Register modules
    manager.register_module(std::make_shared<LoggerModule>());
    manager.register_module(std::make_shared<EmailModule>());
    
    // Load modules (dependencies loaded automatically)
    manager.load_module("Email");
    
    // Access module containers
    auto& logger_container = manager.get_container("Logger");
    auto logger = logger_container.resolve<ILogger>();
    logger->log("Logger module loaded");
    
    auto& email_container = manager.get_container("Email");
    auto email_service = email_container.resolve<IEmailService>();
    email_service->send("user@example.com", "Hello");
    
    // Show load order
    auto load_order = manager.get_load_order();
    std::cout << "Module load order: ";
    for (const auto& name : load_order) {
        std::cout << name << " ";
    }
    std::cout << std::endl;
    
    return 0;
}

