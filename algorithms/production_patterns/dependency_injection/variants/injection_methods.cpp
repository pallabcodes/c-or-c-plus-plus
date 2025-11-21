/*
 * Injection Methods - Dependency Injection
 * 
 * Source: DI frameworks, Martin Fowler, Mark Seemann
 * Pattern: Multiple injection methods (constructor, property, method)
 * 
 * What Makes It Ingenious:
 * - Constructor injection: Mandatory dependencies, immutability
 * - Property injection: Optional dependencies, flexibility
 * - Method injection: Context-specific dependencies
 * - Setter injection: Late binding, optional dependencies
 * - Used in all DI frameworks, enterprise applications
 * 
 * When to Use:
 * - Constructor injection: Mandatory dependencies
 * - Property injection: Optional dependencies, legacy code
 * - Method injection: Context-specific, temporary dependencies
 * - Setter injection: Optional dependencies, late binding
 * 
 * Real-World Usage:
 * - Spring Framework (all injection types)
 * - .NET Core DI (constructor, property)
 * - Autofac (all injection types)
 * - Unity (all injection types)
 * - Enterprise applications
 * 
 * Time Complexity: O(1) for all injection types
 * Space Complexity: O(n) where n is number of dependencies
 */

#include <memory>
#include <functional>
#include <iostream>
#include <optional>

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

// Pattern 1: Constructor Injection (Recommended)
// Dependencies injected via constructor - mandatory, immutable
class UserService {
private:
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IConfigService> config_;
    
public:
    // Constructor injection - dependencies are mandatory
    UserService(std::shared_ptr<ILogger> logger,
                std::shared_ptr<IConfigService> config)
        : logger_(logger), config_(config) {}
    
    void register_user(const std::string& email) {
        logger_->log("Registering user: " + email);
        std::string timeout = config_->get("timeout");
        logger_->log("Using timeout: " + timeout);
    }
};

// Pattern 2: Property Injection (Optional Dependencies)
// Dependencies injected via properties - optional, mutable
class NotificationService {
private:
    std::shared_ptr<ILogger> logger_;
    std::optional<std::shared_ptr<IConfigService>> config_;
    
public:
    NotificationService() = default;
    
    // Property injection - optional dependency
    void set_logger(std::shared_ptr<ILogger> logger) {
        logger_ = logger;
    }
    
    void set_config(std::shared_ptr<IConfigService> config) {
        config_ = config;
    }
    
    void send_notification(const std::string& message) {
        if (logger_) {
            logger_->log("Sending notification: " + message);
        }
        if (config_) {
            std::string setting = config_.value()->get("notification_enabled");
            // Use setting
        }
    }
};

// Pattern 3: Method Injection (Context-Specific)
// Dependencies injected via method parameters - temporary, context-specific
class ReportService {
private:
    std::shared_ptr<ILogger> logger_;
    
public:
    // Constructor injection for mandatory dependency
    explicit ReportService(std::shared_ptr<ILogger> logger) : logger_(logger) {}
    
    // Method injection - dependency only needed for this method
    void generate_report(const std::string& report_type,
                        std::shared_ptr<IConfigService> config) {
        logger_->log("Generating report: " + report_type);
        std::string format = config->get("report_format");
        logger_->log("Using format: " + format);
    }
};

// Pattern 4: Setter Injection (Late Binding)
// Dependencies injected via setters - flexible, optional
class EmailService {
private:
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IConfigService> config_;
    bool initialized_;
    
public:
    EmailService() : initialized_(false) {}
    
    // Setter injection - can be called after construction
    void set_logger(std::shared_ptr<ILogger> logger) {
        logger_ = logger;
        check_initialization();
    }
    
    void set_config(std::shared_ptr<IConfigService> config) {
        config_ = config;
        check_initialization();
    }
    
private:
    void check_initialization() {
        if (logger_ && config_) {
            initialized_ = true;
        }
    }
    
public:
    void send_email(const std::string& to, const std::string& subject) {
        if (!initialized_) {
            throw std::runtime_error("EmailService not fully initialized");
        }
        logger_->log("Sending email to: " + to);
        std::string smtp_server = config_->get("smtp_server");
        // Send email
    }
};

// Pattern 5: Hybrid Injection (Constructor + Property)
// Mix of mandatory and optional dependencies
class OrderService {
private:
    std::shared_ptr<ILogger> logger_;  // Mandatory
    std::optional<std::shared_ptr<IConfigService>> config_;  // Optional
    
public:
    // Constructor injection for mandatory dependency
    explicit OrderService(std::shared_ptr<ILogger> logger) : logger_(logger) {}
    
    // Property injection for optional dependency
    void set_config(std::shared_ptr<IConfigService> config) {
        config_ = config;
    }
    
    void process_order(const std::string& order_id) {
        logger_->log("Processing order: " + order_id);
        if (config_) {
            std::string tax_rate = config_.value()->get("tax_rate");
            // Use tax rate
        }
    }
};

// Pattern 6: Initialization Method Injection
// Dependencies injected via initialization method
class PaymentService {
private:
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IConfigService> config_;
    bool initialized_;
    
public:
    PaymentService() : initialized_(false) {}
    
    // Initialization method - injects all dependencies at once
    void initialize(std::shared_ptr<ILogger> logger,
                    std::shared_ptr<IConfigService> config) {
        logger_ = logger;
        config_ = config;
        initialized_ = true;
    }
    
    void process_payment(const std::string& amount) {
        if (!initialized_) {
            throw std::runtime_error("PaymentService not initialized");
        }
        logger_->log("Processing payment: " + amount);
        std::string currency = config_->get("currency");
        // Process payment
    }
};

// Example usage
int main() {
    auto logger = std::make_shared<ConsoleLogger>();
    auto config = std::make_shared<ConfigService>();
    
    // Pattern 1: Constructor injection
    UserService user_service(logger, config);
    user_service.register_user("user@example.com");
    
    // Pattern 2: Property injection
    NotificationService notification_service;
    notification_service.set_logger(logger);
    notification_service.set_config(config);
    notification_service.send_notification("Hello");
    
    // Pattern 3: Method injection
    ReportService report_service(logger);
    report_service.generate_report("sales", config);
    
    // Pattern 4: Setter injection
    EmailService email_service;
    email_service.set_logger(logger);
    email_service.set_config(config);
    email_service.send_email("user@example.com", "Test");
    
    // Pattern 5: Hybrid injection
    OrderService order_service(logger);
    order_service.set_config(config);
    order_service.process_order("12345");
    
    // Pattern 6: Initialization method
    PaymentService payment_service;
    payment_service.initialize(logger, config);
    payment_service.process_payment("100.00");
    
    return 0;
}

