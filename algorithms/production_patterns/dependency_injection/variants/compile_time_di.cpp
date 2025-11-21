/*
 * Compile-Time Dependency Injection - C++ Templates
 * 
 * Source: Modern C++ DI patterns, template metaprogramming
 * Pattern: Compile-time dependency injection using templates and CRTP
 * 
 * What Makes It Ingenious:
 * - Zero runtime overhead: All resolution happens at compile time
 * - Type safety: Compiler ensures all dependencies are available
 * - Tree-shakable: Unused code eliminated by linker
 * - No virtual calls: Direct function calls, better performance
 * - Used in high-performance systems, embedded systems, game engines
 * 
 * When to Use:
 * - Performance-critical code
 * - Embedded systems with limited resources
 * - When dependencies are known at compile time
 * - Need maximum optimization
 * - Template-heavy codebases
 * 
 * Real-World Usage:
 * - High-frequency trading systems
 * - Game engines (compile-time systems)
 * - Embedded systems
 * - Template libraries (Boost, Eigen)
 * - Modern C++ frameworks
 * 
 * Time Complexity: O(1) - resolved at compile time
 * Space Complexity: O(1) - no runtime overhead
 */

#include <memory>
#include <iostream>

// Base template for compile-time DI
template<typename... Dependencies>
class Injectable {
public:
    using DependenciesList = std::tuple<Dependencies...>;
};

// Compile-time service container
template<typename... Services>
class CompileTimeContainer {
private:
    std::tuple<Services...> services_;
    
public:
    CompileTimeContainer(Services... services) : services_(services...) {}
    
    template<typename T>
    T& get() {
        return std::get<T>(services_);
    }
    
    template<typename T>
    const T& get() const {
        return std::get<T>(services_);
    }
};

// CRTP base for services with compile-time DI
template<typename Derived, typename... Dependencies>
class ServiceBase {
protected:
    std::tuple<Dependencies...> dependencies_;
    
public:
    ServiceBase(Dependencies... deps) : dependencies_(deps...) {}
    
    template<typename T>
    T& get_dependency() {
        return std::get<T>(dependencies_);
    }
    
    template<typename T>
    const T& get_dependency() const {
        return std::get<T>(dependencies_);
    }
};

// Example: Logger interface and implementation
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

// Example: Email service with compile-time DI
class EmailService {
private:
    ILogger& logger_;
    
public:
    // Constructor injection - dependencies known at compile time
    explicit EmailService(ILogger& logger) : logger_(logger) {}
    
    void send_email(const std::string& to, const std::string& subject) {
        logger_.log("Sending email to: " + to + " - " + subject);
    }
};

// Example: User service with multiple dependencies
class UserService {
private:
    EmailService& email_service_;
    ILogger& logger_;
    
public:
    // Multiple dependencies injected at compile time
    UserService(EmailService& email_service, ILogger& logger)
        : email_service_(email_service), logger_(logger) {}
    
    void register_user(const std::string& email) {
        logger_.log("Registering user: " + email);
        email_service_.send_email(email, "Welcome!");
    }
};

// Template-based factory for compile-time creation
template<typename T, typename... Args>
struct TypeFactory {
    static T create(Args... args) {
        return T(args...);
    }
};

// Compile-time service locator pattern
template<typename Container>
class CompileTimeServiceLocator {
private:
    static Container* container_;
    
public:
    static void set_container(Container& container) {
        container_ = &container;
    }
    
    template<typename T>
    static T& get() {
        if (!container_) {
            throw std::runtime_error("Container not set");
        }
        return container_->template get<T>();
    }
};

template<typename Container>
Container* CompileTimeServiceLocator<Container>::container_ = nullptr;

// Example usage
int main() {
    // Create services at compile time
    ConsoleLogger logger;
    EmailService email_service(logger);
    UserService user_service(email_service, logger);
    
    // Use compile-time container
    using ServiceContainer = CompileTimeContainer<ConsoleLogger, EmailService, UserService>;
    ServiceContainer container(logger, email_service, user_service);
    
    // Access services
    auto& service_logger = container.get<ConsoleLogger>();
    service_logger.log("Container initialized");
    
    auto& service_email = container.get<EmailService>();
    service_email.send_email("user@example.com", "Test");
    
    auto& service_user = container.get<UserService>();
    service_user.register_user("newuser@example.com");
    
    // Use service locator pattern
    CompileTimeServiceLocator<ServiceContainer>::set_container(container);
    auto& locator_logger = CompileTimeServiceLocator<ServiceContainer>::get<ConsoleLogger>();
    locator_logger.log("Service locator working");
    
    return 0;
}

