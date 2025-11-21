/*
 * Builder and Strategy Patterns with DI
 * 
 * Source: Design patterns, GoF, modern frameworks
 * Pattern: Builder and Strategy patterns integrated with dependency injection
 * 
 * What Makes It Ingenious:
 * - Builder pattern: Fluent interface for object construction
 * - Strategy pattern: Algorithm selection via dependency injection
 * - DI integration: Strategies injected via DI container
 * - Flexible construction: Build complex objects with dependencies
 * - Used in frameworks, libraries, enterprise applications
 * 
 * When to Use:
 * - Complex object construction
 * - Multiple construction strategies
 * - Algorithm selection at runtime
 * - Fluent APIs
 * - Configuration-driven behavior
 * 
 * Real-World Usage:
 * - Query builders (Entity Framework, LINQ)
 * - HTTP client builders
 * - Configuration builders
 * - Payment processing (different strategies)
 * - Sorting algorithms (different strategies)
 * 
 * Time Complexity: O(1) for builder operations, O(n) for strategy execution
 * Space Complexity: O(n) where n is builder state size
 */

#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <iostream>

// Pattern 1: Builder Pattern with DI
class HttpClient {
private:
    std::string base_url_;
    int timeout_;
    std::vector<std::string> headers_;
    
public:
    class Builder {
    private:
        std::string base_url_;
        int timeout_ = 30;
        std::vector<std::string> headers_;
        
    public:
        Builder& with_base_url(const std::string& url) {
            base_url_ = url;
            return *this;
        }
        
        Builder& with_timeout(int seconds) {
            timeout_ = seconds;
            return *this;
        }
        
        Builder& add_header(const std::string& header) {
            headers_.push_back(header);
            return *this;
        }
        
        std::unique_ptr<HttpClient> build() {
            return std::make_unique<HttpClient>(base_url_, timeout_, headers_);
        }
    };
    
    HttpClient(const std::string& base_url, int timeout, 
               const std::vector<std::string>& headers)
        : base_url_(base_url), timeout_(timeout), headers_(headers) {}
    
    void make_request(const std::string& endpoint) {
        std::cout << "Request to: " << base_url_ << endpoint << std::endl;
        std::cout << "Timeout: " << timeout_ << "s" << std::endl;
    }
};

// Pattern 2: Strategy Pattern with DI
class ISortingStrategy {
public:
    virtual ~ISortingStrategy() = default;
    virtual void sort(std::vector<int>& data) = 0;
};

class QuickSortStrategy : public ISortingStrategy {
public:
    void sort(std::vector<int>& data) override {
        std::cout << "Using QuickSort" << std::endl;
        std::sort(data.begin(), data.end());
    }
};

class MergeSortStrategy : public ISortingStrategy {
public:
    void sort(std::vector<int>& data) override {
        std::cout << "Using MergeSort" << std::endl;
        std::sort(data.begin(), data.end());  // Simplified
    }
};

class BubbleSortStrategy : public ISortingStrategy {
public:
    void sort(std::vector<int>& data) override {
        std::cout << "Using BubbleSort" << std::endl;
        std::sort(data.begin(), data.end());  // Simplified
    }
};

// Sorter with strategy injection
class Sorter {
private:
    std::shared_ptr<ISortingStrategy> strategy_;
    
public:
    explicit Sorter(std::shared_ptr<ISortingStrategy> strategy)
        : strategy_(strategy) {}
    
    void set_strategy(std::shared_ptr<ISortingStrategy> strategy) {
        strategy_ = strategy;
    }
    
    void sort(std::vector<int>& data) {
        if (strategy_) {
            strategy_->sort(data);
        }
    }
};

// Pattern 3: Builder with Strategy DI
class IPaymentProcessor {
public:
    virtual ~IPaymentProcessor() = default;
    virtual void process_payment(double amount) = 0;
};

class CreditCardProcessor : public IPaymentProcessor {
public:
    void process_payment(double amount) override {
        std::cout << "Processing credit card payment: $" << amount << std::endl;
    }
};

class PayPalProcessor : public IPaymentProcessor {
public:
    void process_payment(double amount) override {
        std::cout << "Processing PayPal payment: $" << amount << std::endl;
    }
};

class PaymentBuilder {
private:
    std::shared_ptr<IPaymentProcessor> processor_;
    double amount_ = 0.0;
    std::string currency_ = "USD";
    
public:
    PaymentBuilder& with_processor(std::shared_ptr<IPaymentProcessor> processor) {
        processor_ = processor;
        return *this;
    }
    
    PaymentBuilder& with_amount(double amount) {
        amount_ = amount;
        return *this;
    }
    
    PaymentBuilder& with_currency(const std::string& currency) {
        currency_ = currency;
        return *this;
    }
    
    void process() {
        if (!processor_) {
            throw std::runtime_error("Payment processor not set");
        }
        processor_->process_payment(amount_);
    }
};

// Pattern 4: Factory Builder with DI
template<typename T>
class FactoryBuilder {
private:
    std::function<std::unique_ptr<T>()> factory_;
    std::vector<std::function<void(T*)>> configurators_;
    
public:
    FactoryBuilder& with_factory(std::function<std::unique_ptr<T>()> factory) {
        factory_ = factory;
        return *this;
    }
    
    FactoryBuilder& configure(std::function<void(T*)> configurator) {
        configurators_.push_back(configurator);
        return *this;
    }
    
    std::unique_ptr<T> build() {
        if (!factory_) {
            throw std::runtime_error("Factory not set");
        }
        auto instance = factory_();
        for (auto& configurator : configurators_) {
            configurator(instance.get());
        }
        return instance;
    }
};

// Pattern 5: Strategy Factory with DI Container
class StrategyFactory {
private:
    std::unordered_map<std::string, std::function<std::shared_ptr<ISortingStrategy>()>> factories_;
    
public:
    void register_strategy(const std::string& name,
                          std::function<std::shared_ptr<ISortingStrategy>()> factory) {
        factories_[name] = factory;
    }
    
    std::shared_ptr<ISortingStrategy> create_strategy(const std::string& name) {
        auto it = factories_.find(name);
        if (it == factories_.end()) {
            return nullptr;
        }
        return it->second();
    }
    
    std::vector<std::string> get_available_strategies() const {
        std::vector<std::string> names;
        for (const auto& pair : factories_) {
            names.push_back(pair.first);
        }
        return names;
    }
};

// Example usage
int main() {
    // Pattern 1: Builder pattern
    auto http_client = HttpClient::Builder()
        .with_base_url("https://api.example.com")
        .with_timeout(60)
        .add_header("Content-Type: application/json")
        .add_header("Authorization: Bearer token")
        .build();
    
    http_client->make_request("/users");
    
    // Pattern 2: Strategy pattern with DI
    auto quick_sort = std::make_shared<QuickSortStrategy>();
    Sorter sorter(quick_sort);
    
    std::vector<int> data = {3, 1, 4, 1, 5, 9, 2, 6};
    sorter.sort(data);
    
    // Change strategy
    auto merge_sort = std::make_shared<MergeSortStrategy>();
    sorter.set_strategy(merge_sort);
    sorter.sort(data);
    
    // Pattern 3: Builder with Strategy DI
    auto credit_card = std::make_shared<CreditCardProcessor>();
    PaymentBuilder payment;
    payment.with_processor(credit_card)
           .with_amount(100.50)
           .with_currency("USD")
           .process();
    
    // Pattern 4: Factory Builder
    FactoryBuilder<HttpClient> factory_builder;
    auto configured_client = factory_builder
        .with_factory([]() {
            return std::make_unique<HttpClient>("https://api.example.com", 30, std::vector<std::string>());
        })
        .configure([](HttpClient* client) {
            // Configure client
        })
        .build();
    
    // Pattern 5: Strategy Factory
    StrategyFactory strategy_factory;
    strategy_factory.register_strategy("quicksort", []() {
        return std::make_shared<QuickSortStrategy>();
    });
    strategy_factory.register_strategy("mergesort", []() {
        return std::make_shared<MergeSortStrategy>();
    });
    strategy_factory.register_strategy("bubblesort", []() {
        return std::make_shared<BubbleSortStrategy>();
    });
    
    auto strategy = strategy_factory.create_strategy("quicksort");
    if (strategy) {
        std::vector<int> test_data = {5, 2, 8, 1, 9};
        strategy->sort(test_data);
    }
    
    return 0;
}

