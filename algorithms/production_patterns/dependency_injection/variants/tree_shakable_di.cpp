/*
 * Tree-Shakable Dependency Injection - Dead Code Elimination
 * 
 * Source: Modern JavaScript/TypeScript bundlers, Webpack, Rollup
 * Pattern: Design DI for static analysis and dead code elimination
 * 
 * What Makes It Ingenious:
 * - Static analysis friendly: Bundler can analyze dependencies
 * - Dead code elimination: Unused services removed from bundle
 * - ES module based: Uses import/export for tree-shaking
 * - No side effects: Pure functions, no global state
 * - Smaller bundles: Only include used code
 * - Used in modern web frameworks, libraries, build tools
 * 
 * When to Use:
 * - JavaScript/TypeScript applications
 * - Web applications with bundlers
 * - Library development
 * - Need to minimize bundle size
 * - Modern build toolchains (Webpack, Rollup, Vite)
 * 
 * Real-World Usage:
 * - React libraries
 * - Angular framework
 * - Vue.js
 * - Modern JavaScript libraries
 * - Tree-shakable utility libraries (Lodash ES, RxJS)
 * 
 * Time Complexity: O(1) - resolved at build time
 * Space Complexity: O(1) - unused code eliminated
 * 
 * Note: This is a C++ conceptual implementation showing patterns
 * that enable tree-shaking in JavaScript/TypeScript contexts
 */

#include <memory>
#include <iostream>
#include <unordered_map>
#include <string>

/*
 * Tree-shakable pattern principles:
 * 1. Use explicit exports (not dynamic)
 * 2. Avoid side effects in module scope
 * 3. Use pure functions
 * 4. Separate concerns into modules
 * 5. Use static analysis-friendly patterns
 */

// Pattern 1: Explicit service exports (tree-shakable)
// In JavaScript/TypeScript, this would be:
// export const createLogger = () => ({ log: (msg) => console.log(msg) });
// export const createEmailService = (logger) => ({ send: (to, msg) => {...} });

class TreeShakableLogger {
public:
    static std::unique_ptr<TreeShakableLogger> create() {
        return std::make_unique<TreeShakableLogger>();
    }
    
    void log(const std::string& message) {
        std::cout << "[LOG] " << message << std::endl;
    }
};

class TreeShakableEmailService {
private:
    TreeShakableLogger* logger_;
    
public:
    // Pure factory function - tree-shakable
    static std::unique_ptr<TreeShakableEmailService> create(TreeShakableLogger* logger) {
        return std::make_unique<TreeShakableEmailService>(logger);
    }
    
    explicit TreeShakableEmailService(TreeShakableLogger* logger) : logger_(logger) {}
    
    void send(const std::string& to, const std::string& message) {
        if (logger_) {
            logger_->log("Sending email to: " + to);
        }
        // Email sending logic
    }
};

// Pattern 2: Module-based registration (tree-shakable)
// Each module exports its factory, bundler can eliminate unused modules
class TreeShakableModule {
public:
    virtual ~TreeShakableModule() = default;
    virtual void initialize() = 0;
    virtual void cleanup() = 0;
};

// Pattern 3: Static analysis-friendly service registry
// Uses explicit types, not dynamic strings
template<typename T>
class TreeShakableRegistry {
private:
    static std::unordered_map<std::string, std::function<std::unique_ptr<T>()>> factories_;
    
public:
    // Explicit registration - bundler can see what's used
    static void register_factory(const std::string& name,
                                 std::function<std::unique_ptr<T>()> factory) {
        factories_[name] = factory;
    }
    
    // Explicit resolution - bundler can track usage
    static std::unique_ptr<T> create(const std::string& name) {
        auto it = factories_.find(name);
        if (it == factories_.end()) {
            return nullptr;
        }
        return it->second();
    }
    
    // Get all registered names (for static analysis)
    static std::vector<std::string> get_registered_names() {
        std::vector<std::string> names;
        for (const auto& pair : factories_) {
            names.push_back(pair.first);
        }
        return names;
    }
};

template<typename T>
std::unordered_map<std::string, std::function<std::unique_ptr<T>()>> 
    TreeShakableRegistry<T>::factories_;

// Pattern 4: Pure function composition (tree-shakable)
// No side effects, can be eliminated if unused
class TreeShakableComposer {
public:
    // Pure function - tree-shakable
    template<typename T, typename... Args>
    static std::unique_ptr<T> compose(Args... args) {
        return std::make_unique<T>(args...);
    }
    
    // Compose with dependencies
    template<typename TService, typename... TDependencies>
    static std::unique_ptr<TService> compose_with_dependencies(
        std::function<std::unique_ptr<TService>(TDependencies...)> factory,
        std::unique_ptr<TDependencies>... deps) {
        return factory(std::move(deps)...);
    }
};

// Pattern 5: Conditional exports (tree-shakable)
// Different implementations for different environments
#ifdef PRODUCTION
    using LoggerType = TreeShakableLogger;
#else
    using LoggerType = TreeShakableLogger;  // Could be different in debug
#endif

// Pattern 6: Lazy initialization with tree-shaking support
template<typename T>
class LazyTreeShakable {
private:
    std::function<std::unique_ptr<T>()> factory_;
    std::unique_ptr<T> instance_;
    
public:
    explicit LazyTreeShakable(std::function<std::unique_ptr<T>()> factory)
        : factory_(factory) {}
    
    T& get() {
        if (!instance_) {
            instance_ = factory_();
        }
        return *instance_;
    }
    
    void reset() {
        instance_.reset();
    }
};

// Example usage demonstrating tree-shakable patterns
int main() {
    // Pattern 1: Explicit factory usage
    auto logger = TreeShakableLogger::create();
    logger->log("Tree-shakable logger created");
    
    auto email_service = TreeShakableEmailService::create(logger.get());
    email_service->send("user@example.com", "Hello");
    
    // Pattern 3: Registry usage
    TreeShakableRegistry<TreeShakableLogger>::register_factory(
        "logger",
        []() { return TreeShakableLogger::create(); }
    );
    
    auto registered_logger = TreeShakableRegistry<TreeShakableLogger>::create("logger");
    if (registered_logger) {
        registered_logger->log("From registry");
    }
    
    // Pattern 4: Composition
    auto composed_logger = TreeShakableComposer::compose<TreeShakableLogger>();
    composed_logger->log("Composed logger");
    
    // Pattern 6: Lazy initialization
    LazyTreeShakable<TreeShakableLogger> lazy_logger(
        []() { return TreeShakableLogger::create(); }
    );
    lazy_logger.get().log("Lazy logger");
    
    return 0;
}

/*
 * Tree-shaking best practices (for JavaScript/TypeScript):
 * 
 * 1. Use ES modules (import/export)
 * 2. Avoid side effects in module scope
 * 3. Use pure functions
 * 4. Export individual functions, not entire objects
 * 5. Use static imports, avoid dynamic imports when possible
 * 6. Mark packages as "sideEffects: false" in package.json
 * 7. Use named exports instead of default exports
 * 8. Avoid circular dependencies
 * 9. Use explicit type annotations (TypeScript)
 * 10. Test with production builds to verify tree-shaking
 */

