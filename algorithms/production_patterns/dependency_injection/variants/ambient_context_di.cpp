/*
 * Ambient Context Pattern - Dependency Injection
 * 
 * Source: Enterprise patterns, .NET patterns
 * Pattern: Implicit context available throughout call stack
 * 
 * What Makes It Ingenious:
 * - Implicit context: Access services without explicit injection
 * - Call stack propagation: Context flows through call stack
 * - Thread-local storage: Per-thread context isolation
 * - Fallback mechanism: Default context when none set
 * - Used in logging, security, transaction management
 * 
 * When to Use:
 * - Cross-cutting concerns (logging, security)
 * - Transaction management
 * - Request context propagation
 * - When explicit injection is impractical
 * - Legacy code integration
 * 
 * Real-World Usage:
 * - Logging frameworks (NLog, Log4Net)
 * - Security context (ASP.NET)
 * - Transaction scopes
 * - Request context (HTTP)
 * - Thread-local storage patterns
 * 
 * Time Complexity: O(1) for context access
 * Space Complexity: O(n) where n is number of threads
 */

#include <memory>
#include <thread>
#include <unordered_map>
#include <mutex>
#include <iostream>
#include <any>

template<typename T>
class AmbientContext {
private:
    static thread_local std::shared_ptr<T> current_context_;
    static std::shared_ptr<T> default_context_;
    static std::mutex default_mutex_;
    
public:
    // Get current context (thread-local)
    static std::shared_ptr<T> get_current() {
        if (current_context_) {
            return current_context_;
        }
        
        // Fallback to default
        std::lock_guard<std::mutex> lock(default_mutex_);
        return default_context_;
    }
    
    // Set current context (thread-local)
    static void set_current(std::shared_ptr<T> context) {
        current_context_ = context;
    }
    
    // Set default context (shared across threads)
    static void set_default(std::shared_ptr<T> context) {
        std::lock_guard<std::mutex> lock(default_mutex_);
        default_context_ = context;
    }
    
    // Clear current context
    static void clear() {
        current_context_.reset();
    }
    
    // Check if context is available
    static bool is_available() {
        return current_context_ != nullptr || default_context_ != nullptr;
    }
};

// Static member definitions
template<typename T>
thread_local std::shared_ptr<T> AmbientContext<T>::current_context_;

template<typename T>
std::shared_ptr<T> AmbientContext<T>::default_context_;

template<typename T>
std::mutex AmbientContext<T>::default_mutex_;

// Scoped context (RAII pattern)
template<typename T>
class ScopedContext {
private:
    std::shared_ptr<T> previous_context_;
    
public:
    explicit ScopedContext(std::shared_ptr<T> context) {
        previous_context_ = AmbientContext<T>::get_current();
        AmbientContext<T>::set_current(context);
    }
    
    ~ScopedContext() {
        AmbientContext<T>::set_current(previous_context_);
    }
    
    // Non-copyable
    ScopedContext(const ScopedContext&) = delete;
    ScopedContext& operator=(const ScopedContext&) = delete;
};

// Example: Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const std::string& message) = 0;
};

class ConsoleLogger : public ILogger {
private:
    std::string prefix_;
    
public:
    explicit ConsoleLogger(const std::string& prefix = "") : prefix_(prefix) {}
    
    void log(const std::string& message) override {
        std::cout << prefix_ << "[LOG] " << message << std::endl;
    }
};

// Example: Security context
class ISecurityContext {
public:
    virtual ~ISecurityContext() = default;
    virtual std::string get_user_id() = 0;
    virtual bool is_authenticated() = 0;
};

class SecurityContext : public ISecurityContext {
private:
    std::string user_id_;
    bool authenticated_;
    
public:
    SecurityContext(const std::string& user_id, bool authenticated)
        : user_id_(user_id), authenticated_(authenticated) {}
    
    std::string get_user_id() override {
        return user_id_;
    }
    
    bool is_authenticated() override {
        return authenticated_;
    }
};

// Example: Service using ambient context
class BusinessService {
public:
    void do_work() {
        // Access logger from ambient context
        auto logger = AmbientContext<ILogger>::get_current();
        if (logger) {
            logger->log("Doing work");
        }
        
        // Access security context
        auto security = AmbientContext<ISecurityContext>::get_current();
        if (security && security->is_authenticated()) {
            logger->log("User: " + security->get_user_id());
        }
    }
};

// Example usage
int main() {
    // Set default logger
    AmbientContext<ILogger>::set_default(
        std::make_shared<ConsoleLogger>("[DEFAULT] "));
    
    // Use default context
    auto default_logger = AmbientContext<ILogger>::get_current();
    default_logger->log("Using default context");
    
    // Set thread-local context
    {
        ScopedContext<ILogger> scoped_logger(
            std::make_shared<ConsoleLogger>("[SCOPED] "));
        
        auto scoped = AmbientContext<ILogger>::get_current();
        scoped->log("Using scoped context");
        
        // Set security context
        ScopedContext<ISecurityContext> scoped_security(
            std::make_shared<SecurityContext>("user123", true));
        
        BusinessService service;
        service.do_work();
    }
    
    // Back to default context
    auto back_to_default = AmbientContext<ILogger>::get_current();
    back_to_default->log("Back to default");
    
    return 0;
}

