/*
 * Functional Dependency Injection (Reader Monad Pattern)
 * 
 * Source: Functional programming, Haskell Reader monad, Scala implicits
 * Pattern: Use functional composition for dependency injection
 * 
 * What Makes It Ingenious:
 * - Pure functions: No side effects, easier to test
 * - Composition: Compose functions with dependencies
 * - Type safety: Compiler ensures dependencies are provided
 * - Immutability: Dependencies are immutable
 * - Used in functional programming, Haskell, Scala, F#
 * 
 * When to Use:
 * - Functional programming style
 * - Need pure functions
 * - Type-safe dependency management
 * - Composable functions
 * - Testing pure functions
 * 
 * Real-World Usage:
 * - Haskell applications
 * - Scala applications
 * - F# applications
 * - Functional JavaScript/TypeScript
 * - Pure functional libraries
 * 
 * Time Complexity: O(1) for function composition
 * Space Complexity: O(1) - no runtime overhead
 */

#include <memory>
#include <functional>
#include <iostream>
#include <string>

// Reader monad for dependency injection
template<typename Dependencies, typename Result>
class Reader {
private:
    std::function<Result(Dependencies)> run_reader_;
    
public:
    explicit Reader(std::function<Result(Dependencies)> f) : run_reader_(f) {}
    
    // Run reader with dependencies
    Result run(Dependencies deps) const {
        return run_reader_(deps);
    }
    
    // Map over result
    template<typename NewResult>
    Reader<Dependencies, NewResult> map(std::function<NewResult(Result)> f) const {
        return Reader<Dependencies, NewResult>([this, f](Dependencies deps) {
            return f(run_reader_(deps));
        });
    }
    
    // FlatMap (bind) for composition
    template<typename NewResult>
    Reader<Dependencies, NewResult> flat_map(
        std::function<Reader<Dependencies, NewResult>(Result)> f) const {
        return Reader<Dependencies, NewResult>([this, f](Dependencies deps) {
            auto result = run_reader_(deps);
            return f(result).run(deps);
        });
    }
    
    // Pure value (no dependencies)
    static Reader<Dependencies, Result> pure(Result value) {
        return Reader<Dependencies, Result>([value](Dependencies) {
            return value;
        });
    }
    
    // Ask for dependencies
    static Reader<Dependencies, Dependencies> ask() {
        return Reader<Dependencies, Dependencies>([](Dependencies deps) {
            return deps;
        });
    }
};

// Dependency environment
struct Dependencies {
    std::shared_ptr<class ILogger> logger;
    std::shared_ptr<class IConfigService> config;
    
    Dependencies(std::shared_ptr<class ILogger> log,
                std::shared_ptr<class IConfigService> cfg)
        : logger(log), config(cfg) {}
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

// Functional service using Reader monad
class FunctionalService {
public:
    // Function that requires dependencies
    static Reader<Dependencies, std::string> process_data(const std::string& input) {
        return Reader<Dependencies, std::string>([input](Dependencies deps) {
            deps.logger->log("Processing: " + input);
            std::string config_value = deps.config->get("timeout");
            return "Processed: " + input + " with config: " + config_value;
        });
    }
    
    // Compose multiple operations
    static Reader<Dependencies, std::string> complex_operation(const std::string& input) {
        return process_data(input)
            .flat_map<std::string>([](const std::string& result) {
                return Reader<Dependencies, std::string>([result](Dependencies deps) {
                    deps.logger->log("Second step: " + result);
                    return result + " (completed)";
                });
            });
    }
    
    // Function that uses ask to get dependencies
    static Reader<Dependencies, void> log_message(const std::string& message) {
        return Reader<Dependencies, Dependencies>::ask()
            .map<void>([message](Dependencies deps) {
                deps.logger->log(message);
            });
    }
};

// Helper to extract dependency from Reader
template<typename T>
Reader<Dependencies, T> get_logger() {
    return Reader<Dependencies, Dependencies>::ask()
        .map<T>([](Dependencies deps) {
            return deps.logger;
        });
}

// Example usage
int main() {
    // Create dependencies
    auto logger = std::make_shared<ConsoleLogger>();
    auto config = std::make_shared<ConfigService>();
    Dependencies deps(logger, config);
    
    // Use functional service
    auto result_reader = FunctionalService::process_data("test_data");
    auto result = result_reader.run(deps);
    std::cout << "Result: " << result << std::endl;
    
    // Compose operations
    auto complex_reader = FunctionalService::complex_operation("complex_data");
    auto complex_result = complex_reader.run(deps);
    std::cout << "Complex result: " << complex_result << std::endl;
    
    // Use ask pattern
    auto log_reader = FunctionalService::log_message("Functional DI works!");
    log_reader.run(deps);
    
    return 0;
}

