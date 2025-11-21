/*
 * Convention-Based Dependency Injection
 * 
 * Source: Ninject, StructureMap, convention over configuration
 * Pattern: Automatic registration based on naming conventions
 * 
 * What Makes It Ingenious:
 * - Convention over configuration: Automatic registration by convention
 * - Naming conventions: Interface -> Implementation mapping
 * - Assembly scanning: Auto-discover and register types
 * - Attribute-based: Use attributes to control registration
 * - Used in modern frameworks, rapid development
 * 
 * When to Use:
 * - Convention-based applications
 * - Rapid development
 * - Large codebases with consistent naming
 * - Framework development
 * - Reduce registration boilerplate
 * 
 * Real-World Usage:
 * - Ninject (convention-based binding)
 * - StructureMap (convention scanning)
 * - ASP.NET Core (convention-based services)
 * - Spring Framework (component scanning)
 * - Modern DI frameworks
 * 
 * Time Complexity: O(n) for scanning, O(1) for resolution
 * Space Complexity: O(n) where n is number of types
 */

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <iostream>
#include <any>
#include <vector>
#include <string>

// Convention-based container
class ConventionBasedContainer {
public:
    // Naming convention types
    enum class NamingConvention {
        InterfacePrefix,      // ILogger -> Logger
        InterfaceSuffix,      // LoggerInterface -> Logger
        SameName,            // Logger -> Logger
        Custom               // Custom mapping function
    };
    
private:
    std::unordered_map<std::type_index, std::function<std::any()>> factories_;
    std::unordered_map<std::type_index, std::any> singletons_;
    NamingConvention convention_;
    std::mutex mutex_;
    
    template<typename T>
    std::type_index get_type_index() {
        return std::type_index(typeid(T));
    }
    
    // Extract implementation name from interface name
    std::string get_implementation_name(const std::string& interface_name,
                                       NamingConvention convention) {
        switch (convention) {
            case NamingConvention::InterfacePrefix:
                if (interface_name[0] == 'I' && 
                    interface_name.length() > 1 &&
                    std::isupper(interface_name[1])) {
                    return interface_name.substr(1);
                }
                break;
            case NamingConvention::InterfaceSuffix:
                if (interface_name.length() > 9 &&
                    interface_name.substr(interface_name.length() - 9) == "Interface") {
                    return interface_name.substr(0, interface_name.length() - 9);
                }
                break;
            case NamingConvention::SameName:
                return interface_name;
            default:
                break;
        }
        return interface_name;
    }
    
public:
    explicit ConventionBasedContainer(NamingConvention convention = 
                                     NamingConvention::InterfacePrefix)
        : convention_(convention) {}
    
    // Register by convention
    template<typename TInterface, typename TImplementation>
    void register_by_convention() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<TInterface>();
        
        factories_[type_idx] = []() -> std::any {
            return std::make_any<std::shared_ptr<TInterface>>(
                std::make_shared<TImplementation>());
        };
    }
    
    // Register singleton by convention
    template<typename TInterface, typename TImplementation>
    void register_singleton_by_convention() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<TInterface>();
        
        factories_[type_idx] = [this, type_idx]() -> std::any {
            // Check singleton cache
            auto it = singletons_.find(type_idx);
            if (it != singletons_.end()) {
                return it->second;
            }
            
            auto instance = std::make_any<std::shared_ptr<TInterface>>(
                std::make_shared<TImplementation>());
            singletons_[type_idx] = instance;
            return instance;
        };
    }
    
    // Register all types matching convention (simplified)
    template<typename... Types>
    void scan_and_register() {
        // In real implementation, would use reflection to scan assemblies
        // This is a simplified version
    }
    
    // Resolve by convention
    template<typename T>
    std::shared_ptr<T> resolve() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto type_idx = get_type_index<T>();
        
        auto it = factories_.find(type_idx);
        if (it == factories_.end()) {
            throw std::runtime_error("Type not registered: " + 
                                   std::string(typeid(T).name()));
        }
        
        auto result = it->second();
        return std::any_cast<std::shared_ptr<T>>(result);
    }
    
    // Set naming convention
    void set_convention(NamingConvention convention) {
        convention_ = convention;
    }
};

// Example: Convention-based services
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const std::string& message) = 0;
};

// Convention: ILogger -> Logger
class Logger : public ILogger {
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

// Convention: IEmailService -> EmailService
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
    ConventionBasedContainer container;
    
    // Register by convention (ILogger -> Logger)
    container.register_singleton_by_convention<ILogger, Logger>();
    
    // Register by convention (IEmailService -> EmailService)
    container.register_singleton_by_convention<IEmailService, EmailService>();
    
    // Resolve using convention
    auto logger = container.resolve<ILogger>();
    logger->log("Convention-based registration working");
    
    // Note: In real implementation, scanning would automatically
    // discover and register ILogger -> Logger, IEmailService -> EmailService
    // based on naming conventions
    
    return 0;
}

