/*
 * Container Diagnostics and Health Checking - Dependency Injection
 * 
 * Source: Production DI frameworks, monitoring systems
 * Pattern: Diagnostics and health checking for DI containers
 * 
 * What Makes It Ingenious:
 * - Health checks: Verify container configuration is valid
 * - Dependency graph analysis: Visualize dependency relationships
 * - Validation: Check for missing registrations, circular dependencies
 * - Diagnostics: Debug container state and resolution issues
 * - Used in production frameworks, debugging tools, monitoring
 * 
 * When to Use:
 * - Production applications
 * - Debugging container issues
 * - Monitoring container health
 * - Validating configuration
 * - Development tools
 * 
 * Real-World Usage:
 * - Autofac diagnostics
 * - .NET Core health checks
 * - Spring Boot actuator
 * - Production monitoring
 * - Development tools
 * 
 * Time Complexity: O(n + e) where n is services, e is dependencies
 * Space Complexity: O(n + e) for dependency graph
 */

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <typeindex>
#include <iostream>
#include <sstream>
#include <functional>
#include <any>

// Forward declarations
class Container;

// Diagnostic result
struct DiagnosticResult {
    enum class Severity {
        Info,
        Warning,
        Error
    };
    
    Severity severity;
    std::string message;
    std::string service_name;
    
    DiagnosticResult(Severity sev, const std::string& msg, const std::string& svc = "")
        : severity(sev), message(msg), service_name(svc) {}
};

// Health check result
struct HealthCheckResult {
    bool is_healthy;
    std::vector<DiagnosticResult> diagnostics;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    
    HealthCheckResult() : is_healthy(true) {}
    
    void add_diagnostic(const DiagnosticResult& diagnostic) {
        diagnostics.push_back(diagnostic);
        if (diagnostic.severity == DiagnosticResult::Severity::Error) {
            errors.push_back(diagnostic.message);
            is_healthy = false;
        } else if (diagnostic.severity == DiagnosticResult::Severity::Warning) {
            warnings.push_back(diagnostic.message);
        }
    }
};

// Dependency graph node
struct DependencyNode {
    std::string service_name;
    std::type_index service_type;
    std::vector<std::type_index> dependencies;
    bool is_registered;
    bool is_resolved;
    
    DependencyNode(const std::string& name, std::type_index type)
        : service_name(name), service_type(type), is_registered(false), is_resolved(false) {}
};

// Container diagnostics
class ContainerDiagnostics {
private:
    Container* container_;
    std::unordered_map<std::type_index, DependencyNode> dependency_graph_;
    
    template<typename T>
    std::type_index get_type_index() {
        return std::type_index(typeid(T));
    }
    
    std::string get_type_name(std::type_index type_idx) {
        // Simplified - in real implementation, would demangle type names
        return "Type_" + std::to_string(type_idx.hash_code());
    }
    
    // Check for circular dependencies using DFS
    bool has_circular_dependency(std::type_index start, 
                                 std::type_index current,
                                 std::unordered_set<std::type_index>& visited,
                                 std::unordered_set<std::type_index>& rec_stack) {
        visited.insert(current);
        rec_stack.insert(current);
        
        auto it = dependency_graph_.find(current);
        if (it != dependency_graph_.end()) {
            for (const auto& dep : it->second.dependencies) {
                if (visited.find(dep) == visited.end()) {
                    if (has_circular_dependency(start, dep, visited, rec_stack)) {
                        return true;
                    }
                } else if (rec_stack.find(dep) != rec_stack.end()) {
                    return true;  // Circular dependency found
                }
            }
        }
        
        rec_stack.erase(current);
        return false;
    }
    
public:
    ContainerDiagnostics(Container* container) : container_(container) {}
    
    // Perform health check
    HealthCheckResult perform_health_check() {
        HealthCheckResult result;
        
        // Check 1: Verify all registered services can be resolved
        for (const auto& [type_idx, node] : dependency_graph_) {
            if (node.is_registered) {
                try {
                    // Try to resolve (simplified - would need actual resolve call)
                    result.add_diagnostic(DiagnosticResult(
                        DiagnosticResult::Severity::Info,
                        "Service registered: " + node.service_name,
                        node.service_name
                    ));
                } catch (...) {
                    result.add_diagnostic(DiagnosticResult(
                        DiagnosticResult::Severity::Error,
                        "Cannot resolve registered service: " + node.service_name,
                        node.service_name
                    ));
                }
            }
        }
        
        // Check 2: Detect circular dependencies
        std::unordered_set<std::type_index> visited;
        for (const auto& [type_idx, node] : dependency_graph_) {
            if (visited.find(type_idx) == visited.end()) {
                std::unordered_set<std::type_index> rec_stack;
                if (has_circular_dependency(type_idx, type_idx, visited, rec_stack)) {
                    result.add_diagnostic(DiagnosticResult(
                        DiagnosticResult::Severity::Error,
                        "Circular dependency detected involving: " + node.service_name,
                        node.service_name
                    ));
                }
            }
        }
        
        // Check 3: Check for missing dependencies
        for (const auto& [type_idx, node] : dependency_graph_) {
            for (const auto& dep_type : node.dependencies) {
                auto dep_it = dependency_graph_.find(dep_type);
                if (dep_it == dependency_graph_.end() || !dep_it->second.is_registered) {
                    result.add_diagnostic(DiagnosticResult(
                        DiagnosticResult::Severity::Error,
                        "Missing dependency for " + node.service_name + ": " + get_type_name(dep_type),
                        node.service_name
                    ));
                }
            }
        }
        
        return result;
    }
    
    // Get dependency graph visualization
    std::string get_dependency_graph() {
        std::ostringstream oss;
        oss << "Dependency Graph:\n";
        oss << "================\n\n";
        
        for (const auto& [type_idx, node] : dependency_graph_) {
            oss << node.service_name;
            if (node.is_registered) {
                oss << " [REGISTERED]";
            }
            oss << "\n";
            
            if (!node.dependencies.empty()) {
                oss << "  Dependencies:\n";
                for (const auto& dep : node.dependencies) {
                    oss << "    - " << get_type_name(dep) << "\n";
                }
            }
            oss << "\n";
        }
        
        return oss.str();
    }
    
    // Register service in graph
    template<typename T>
    void register_service() {
        auto type_idx = get_type_index<T>();
        std::string name = get_type_name(type_idx);
        
        auto it = dependency_graph_.find(type_idx);
        if (it == dependency_graph_.end()) {
            dependency_graph_[type_idx] = DependencyNode(name, type_idx);
        }
        dependency_graph_[type_idx].is_registered = true;
    }
    
    // Add dependency relationship
    template<typename TService, typename TDependency>
    void add_dependency() {
        auto service_idx = get_type_index<TService>();
        auto dep_idx = get_type_index<TDependency>();
        
        if (dependency_graph_.find(service_idx) == dependency_graph_.end()) {
            dependency_graph_[service_idx] = DependencyNode(
                get_type_name(service_idx), service_idx);
        }
        
        dependency_graph_[service_idx].dependencies.push_back(dep_idx);
    }
    
    // Get statistics
    struct Statistics {
        size_t total_services;
        size_t registered_services;
        size_t total_dependencies;
        size_t circular_dependencies;
    };
    
    Statistics get_statistics() {
        Statistics stats{};
        stats.total_services = dependency_graph_.size();
        
        for (const auto& [type_idx, node] : dependency_graph_) {
            if (node.is_registered) {
                stats.registered_services++;
            }
            stats.total_dependencies += node.dependencies.size();
        }
        
        // Check for circular dependencies
        std::unordered_set<std::type_index> visited;
        for (const auto& [type_idx, node] : dependency_graph_) {
            if (visited.find(type_idx) == visited.end()) {
                std::unordered_set<std::type_index> rec_stack;
                if (has_circular_dependency(type_idx, type_idx, visited, rec_stack)) {
                    stats.circular_dependencies++;
                }
            }
        }
        
        return stats;
    }
};

// Simplified container for demonstration
class Container {
private:
    std::unordered_set<std::type_index> registered_services_;
    
public:
    template<typename T>
    void register_service() {
        registered_services_.insert(std::type_index(typeid(T)));
    }
    
    template<typename T>
    bool is_registered() {
        return registered_services_.find(std::type_index(typeid(T))) != registered_services_.end();
    }
};

// Example usage
int main() {
    Container container;
    ContainerDiagnostics diagnostics(&container);
    
    // Register services
    container.register_service<class ILogger>();
    container.register_service<class IEmailService>();
    diagnostics.register_service<class ILogger>();
    diagnostics.register_service<class IEmailService>();
    
    // Add dependency relationships
    diagnostics.add_dependency<class IEmailService, class ILogger>();
    
    // Perform health check
    auto health = diagnostics.perform_health_check();
    
    std::cout << "Health Check Results:\n";
    std::cout << "====================\n";
    std::cout << "Is Healthy: " << (health.is_healthy ? "Yes" : "No") << "\n\n";
    
    for (const auto& diag : health.diagnostics) {
        std::string severity_str;
        switch (diag.severity) {
            case DiagnosticResult::Severity::Info:
                severity_str = "INFO";
                break;
            case DiagnosticResult::Severity::Warning:
                severity_str = "WARNING";
                break;
            case DiagnosticResult::Severity::Error:
                severity_str = "ERROR";
                break;
        }
        std::cout << "[" << severity_str << "] " << diag.message << "\n";
    }
    
    // Get statistics
    auto stats = diagnostics.get_statistics();
    std::cout << "\nStatistics:\n";
    std::cout << "===========\n";
    std::cout << "Total Services: " << stats.total_services << "\n";
    std::cout << "Registered Services: " << stats.registered_services << "\n";
    std::cout << "Total Dependencies: " << stats.total_dependencies << "\n";
    std::cout << "Circular Dependencies: " << stats.circular_dependencies << "\n";
    
    // Get dependency graph
    std::cout << "\n" << diagnostics.get_dependency_graph();
    
    return 0;
}

