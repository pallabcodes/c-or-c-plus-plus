/*
 * Dependency Validation and Verification - Dependency Injection
 * 
 * Source: Simple Injector, .NET Core DI, Autofac
 * Pattern: Validate dependency graph at startup/compile time
 * 
 * What Makes It Ingenious:
 * - Early error detection: Catch missing dependencies at startup
 * - Dependency graph validation: Verify all dependencies can be resolved
 * - Circular dependency detection: Find and report cycles
 * - Configuration verification: Ensure all services are properly configured
 * - Used in production DI frameworks, enterprise applications
 * 
 * When to Use:
 * - Production applications
 * - Complex dependency graphs
 * - Need early error detection
 * - Configuration validation
 * - Development-time checks
 * 
 * Real-World Usage:
 * - Simple Injector (Verify())
 * - .NET Core DI (Service validation)
 * - Autofac (Container validation)
 * - Spring Framework (Bean validation)
 * - Enterprise applications
 * 
 * Time Complexity: O(n + e) where n is services, e is dependencies
 * Space Complexity: O(n) for validation state
 */

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <functional>
#include <typeindex>
#include <iostream>
#include <stdexcept>
#include <any>

// Validation result
struct ValidationResult {
    bool is_valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : is_valid(true) {}
    
    void add_error(const std::string& error) {
        is_valid = false;
        errors.push_back(error);
    }
    
    void add_warning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Simple container with validation
class ValidatedContainer {
private:
    std::unordered_map<std::type_index, std::function<std::any()>> factories_;
    std::unordered_map<std::type_index, std::vector<std::type_index>> dependencies_;
    
    template<typename T>
    std::type_index get_type_index() {
        return std::type_index(typeid(T));
    }
    
    // Check for circular dependencies using DFS
    bool has_circular_dependency(std::type_index type, 
                                 std::unordered_set<std::type_index>& visited,
                                 std::unordered_set<std::type_index>& rec_stack,
                                 std::vector<std::type_index>& cycle_path) {
        visited.insert(type);
        rec_stack.insert(type);
        cycle_path.push_back(type);
        
        auto deps_it = dependencies_.find(type);
        if (deps_it != dependencies_.end()) {
            for (const auto& dep : deps_it->second) {
                if (visited.find(dep) == visited.end()) {
                    if (has_circular_dependency(dep, visited, rec_stack, cycle_path)) {
                        return true;
                    }
                } else if (rec_stack.find(dep) != rec_stack.end()) {
                    // Circular dependency found
                    cycle_path.push_back(dep);
                    return true;
                }
            }
        }
        
        rec_stack.erase(type);
        cycle_path.pop_back();
        return false;
    }
    
public:
    template<typename TInterface>
    void register_service(std::function<std::shared_ptr<TInterface>()> factory) {
        auto type_idx = get_type_index<TInterface>();
        factories_[type_idx] = [factory]() -> std::any {
            return std::make_any<std::shared_ptr<TInterface>>(factory());
        };
    }
    
    template<typename TInterface, typename TDependency>
    void register_with_dependency(std::function<std::shared_ptr<TInterface>(std::shared_ptr<TDependency>)> factory) {
        auto type_idx = get_type_index<TInterface>();
        auto dep_idx = get_type_index<TDependency>();
        
        dependencies_[type_idx].push_back(dep_idx);
        
        factories_[type_idx] = [factory, this]() -> std::any {
            auto dep = this->resolve<TDependency>();
            return std::make_any<std::shared_ptr<TInterface>>(factory(dep));
        };
    }
    
    template<typename T>
    std::shared_ptr<T> resolve() {
        auto type_idx = get_type_index<T>();
        auto it = factories_.find(type_idx);
        if (it == factories_.end()) {
            throw std::runtime_error("Service not registered: " + std::string(typeid(T).name()));
        }
        return std::any_cast<std::shared_ptr<T>>(it->second());
    }
    
    // Validate dependency graph
    ValidationResult validate() {
        ValidationResult result;
        
        // Check all registered services can be resolved
        for (const auto& pair : factories_) {
            try {
                // Try to resolve (this will check dependencies)
                pair.second();
            } catch (const std::exception& e) {
                result.add_error("Cannot resolve service: " + std::string(typeid(pair.first).name()) + 
                                " - " + e.what());
            }
        }
        
        // Check for circular dependencies
        std::unordered_set<std::type_index> visited;
        for (const auto& pair : dependencies_) {
            if (visited.find(pair.first) == visited.end()) {
                std::unordered_set<std::type_index> rec_stack;
                std::vector<std::type_index> cycle_path;
                
                if (has_circular_dependency(pair.first, visited, rec_stack, cycle_path)) {
                    std::string cycle_str = "Circular dependency detected: ";
                    for (size_t i = 0; i < cycle_path.size(); i++) {
                        cycle_str += std::string(typeid(cycle_path[i]).name());
                        if (i < cycle_path.size() - 1) {
                            cycle_str += " -> ";
                        }
                    }
                    result.add_error(cycle_str);
                }
            }
        }
        
        // Check for missing dependencies
        for (const auto& pair : dependencies_) {
            for (const auto& dep : pair.second) {
                if (factories_.find(dep) == factories_.end()) {
                    result.add_error("Missing dependency: " + std::string(typeid(dep).name()) + 
                                    " required by " + std::string(typeid(pair.first).name()));
                }
            }
        }
        
        return result;
    }
    
    // Verify container (throws if invalid)
    void verify() {
        auto result = validate();
        if (!result.is_valid) {
            std::string error_msg = "Container validation failed:\n";
            for (const auto& error : result.errors) {
                error_msg += "  ERROR: " + error + "\n";
            }
            for (const auto& warning : result.warnings) {
                error_msg += "  WARNING: " + warning + "\n";
            }
            throw std::runtime_error(error_msg);
        }
    }
};

// Example services
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
    virtual void send(const std::string& to, const std::string& message) = 0;
};

class EmailService : public IEmailService {
private:
    std::shared_ptr<ILogger> logger_;
    
public:
    explicit EmailService(std::shared_ptr<ILogger> logger) : logger_(logger) {}
    
    void send(const std::string& to, const std::string& message) override {
        logger_->log("Sending email to: " + to);
    }
};

// Example usage
int main() {
    ValidatedContainer container;
    
    // Register services
    container.register_service<ILogger>([]() {
        return std::make_shared<ConsoleLogger>();
    });
    
    container.register_with_dependency<IEmailService, ILogger>([](std::shared_ptr<ILogger> logger) {
        return std::make_shared<EmailService>(logger);
    });
    
    // Validate container
    auto result = container.validate();
    if (result.is_valid) {
        std::cout << "Container validation passed!" << std::endl;
        
        // Verify (throws if invalid)
        container.verify();
        
        // Use services
        auto email_service = container.resolve<IEmailService>();
        email_service->send("user@example.com", "Hello");
    } else {
        std::cout << "Container validation failed:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  ERROR: " << error << std::endl;
        }
    }
    
    return 0;
}

