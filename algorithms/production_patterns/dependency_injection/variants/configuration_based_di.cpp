/*
 * Configuration-Based Dependency Injection
 * 
 * Source: Spring Framework, .NET Core configuration, YAML/JSON configs
 * Pattern: Load dependency configuration from external files
 * 
 * What Makes It Ingenious:
 * - External configuration: Dependencies defined in config files
 * - Runtime configuration: Change dependencies without recompilation
 * - Environment-specific: Different configs for dev/staging/prod
 * - Type mapping: Map configuration to types
 * - Used in frameworks, enterprise applications, cloud services
 * 
 * When to Use:
 * - Need runtime configuration changes
 * - Environment-specific dependencies
 * - Configuration-driven architecture
 * - Plugin systems with config
 * - Microservices configuration
 * - Cloud-native applications
 * 
 * Real-World Usage:
 * - Spring Framework (application.properties, application.yml)
 * - .NET Core (appsettings.json)
 * - Kubernetes ConfigMaps
 * - Environment variables
 * - Configuration servers
 * 
 * Time Complexity: O(n) for config loading, O(1) for resolution
 * Space Complexity: O(n) where n is number of configured services
 */

#include <memory>
#include <unordered_map>
#include <string>
#include <functional>
#include <fstream>
#include <sstream>
#include <iostream>
#include <any>

// Simple configuration structure (simplified JSON-like)
struct ServiceConfig {
    std::string type;
    std::string implementation;
    std::string lifetime;  // "singleton", "transient", "scoped"
    std::unordered_map<std::string, std::string> properties;
};

// Configuration parser (simplified)
class ConfigParser {
public:
    static std::unordered_map<std::string, ServiceConfig> parse(const std::string& config_text) {
        std::unordered_map<std::string, ServiceConfig> configs;
        
        // Simplified parser - in real implementation, use JSON/YAML library
        // Format: service_name:type:implementation:lifetime
        std::istringstream stream(config_text);
        std::string line;
        
        while (std::getline(stream, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            std::istringstream line_stream(line);
            std::string name, type, impl, lifetime;
            
            if (std::getline(line_stream, name, ':') &&
                std::getline(line_stream, type, ':') &&
                std::getline(line_stream, impl, ':') &&
                std::getline(line_stream, lifetime, ':')) {
                
                ServiceConfig config;
                config.type = type;
                config.implementation = impl;
                config.lifetime = lifetime;
                configs[name] = config;
            }
        }
        
        return configs;
    }
    
    static std::unordered_map<std::string, ServiceConfig> parse_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open config file: " + filename);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return parse(buffer.str());
    }
};

// Configuration-based container
class ConfigurationBasedContainer {
public:
    enum class Lifetime {
        Singleton,
        Transient,
        Scoped
    };
    
    struct FactoryRegistration {
        std::function<std::any()> factory;
        Lifetime lifetime;
    };
    
private:
    std::unordered_map<std::string, FactoryRegistration> factories_;
    std::unordered_map<std::string, std::any> singletons_;
    std::mutex mutex_;
    
    Lifetime parse_lifetime(const std::string& lifetime_str) {
        if (lifetime_str == "singleton") return Lifetime::Singleton;
        if (lifetime_str == "transient") return Lifetime::Transient;
        if (lifetime_str == "scoped") return Lifetime::Scoped;
        return Lifetime::Transient;
    }
    
public:
    // Register factory for a type
    template<typename T>
    void register_factory(const std::string& name,
                         std::function<std::shared_ptr<T>()> factory,
                         Lifetime lifetime = Lifetime::Transient) {
        std::lock_guard<std::mutex> lock(mutex_);
        factories_[name] = FactoryRegistration{
            [factory]() -> std::any {
                return std::make_any<std::shared_ptr<T>>(factory());
            },
            lifetime
        };
    }
    
    // Load configuration and register services
    void load_configuration(const std::unordered_map<std::string, ServiceConfig>& configs,
                           std::function<std::any(const ServiceConfig&)> factory_resolver) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (const auto& [name, config] : configs) {
            auto lifetime = parse_lifetime(config.lifetime);
            auto factory = factory_resolver(config);
            
            // Store factory (simplified - in real implementation, would store properly)
            // This is a conceptual implementation
        }
    }
    
    // Resolve service by name
    template<typename T>
    std::shared_ptr<T> resolve(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = factories_.find(name);
        if (it == factories_.end()) {
            throw std::runtime_error("Service not found: " + name);
        }
        
        auto& registration = it->second;
        
        // Handle singleton
        if (registration.lifetime == Lifetime::Singleton) {
            auto singleton_it = singletons_.find(name);
            if (singleton_it != singletons_.end()) {
                return std::any_cast<std::shared_ptr<T>>(singleton_it->second);
            }
            
            auto instance = registration.factory();
            singletons_[name] = instance;
            return std::any_cast<std::shared_ptr<T>>(instance);
        }
        
        // Handle transient
        auto instance = registration.factory();
        return std::any_cast<std::shared_ptr<T>>(instance);
    }
    
    // Register from environment variable
    template<typename T>
    void register_from_env(const std::string& env_var,
                          std::function<std::shared_ptr<T>(const std::string&)> factory) {
        const char* value = std::getenv(env_var.c_str());
        if (value) {
            register_factory<T>(env_var, [factory, value]() {
                return factory(value);
            });
        }
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

class FileLogger : public ILogger {
private:
    std::string filename_;
    
public:
    explicit FileLogger(const std::string& filename) : filename_(filename) {}
    
    void log(const std::string& message) override {
        std::cout << "[FILE:" << filename_ << "] " << message << std::endl;
    }
};

// Factory resolver
std::any create_logger(const ServiceConfig& config) {
    if (config.implementation == "ConsoleLogger") {
        return std::make_any<std::shared_ptr<ILogger>>(
            std::make_shared<ConsoleLogger>());
    } else if (config.implementation == "FileLogger") {
        std::string filename = config.properties.count("filename") 
            ? config.properties.at("filename") 
            : "app.log";
        return std::make_any<std::shared_ptr<ILogger>>(
            std::make_shared<FileLogger>(filename));
    }
    throw std::runtime_error("Unknown implementation: " + config.implementation);
}

// Example usage
int main() {
    ConfigurationBasedContainer container;
    
    // Register factories
    container.register_factory<ILogger>("console_logger",
        []() { return std::make_shared<ConsoleLogger>(); },
        ConfigurationBasedContainer::Lifetime::Singleton);
    
    container.register_factory<ILogger>("file_logger",
        []() { return std::make_shared<FileLogger>("app.log"); },
        ConfigurationBasedContainer::Lifetime::Singleton);
    
    // Load from configuration string
    std::string config_text = R"(
# Service configurations
logger:ILogger:ConsoleLogger:singleton
)";
    
    auto configs = ConfigParser::parse(config_text);
    
    // Resolve from configuration
    try {
        auto logger = container.resolve<ILogger>("console_logger");
        logger->log("Resolved from configuration");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}

