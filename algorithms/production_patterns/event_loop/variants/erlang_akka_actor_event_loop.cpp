/*
 * Message Queue Event Loop (Actor Model)
 *
 * Source: Erlang/OTP, Akka, Orleans, Actor model
 * Algorithm: Message-passing concurrency with actor isolation
 *
 * What Makes It Ingenious:
 * - Actor isolation and encapsulation
 * - Message-passing concurrency
 * - Pattern matching for message dispatch
 * - Fault tolerance with supervision
 * - Location transparency
 * - Hot code swapping
 * - Let it crash philosophy
 *
 * When to Use:
 * - Distributed systems
 * - Fault-tolerant applications
 * - Concurrent systems with complex interactions
 * - Real-time systems requiring high availability
 * - Microservices architectures
 *
 * Real-World Usage:
 * - Erlang/OTP (WhatsApp, RabbitMQ, CouchDB)
 * - Akka (Lightbend platform, Play Framework)
 * - Microsoft Orleans (halo, Azure)
 * - Actor-based game servers
 * - IoT device management
 * - Financial trading systems
 *
 * Time Complexity: O(1) message send, O(n) pattern matching
 * Space Complexity: O(m) per actor mailbox, O(a) total actors
 */

#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <any>
#include <typeindex>
#include <algorithm>

// Forward declarations
class ActorSystem;
class ActorRef;
class Actor;
class Message;

// Message base class
class Message {
public:
    Message() : sender_(nullptr), timestamp_(std::chrono::steady_clock::now()) {}
    virtual ~Message() = default;

    ActorRef* sender() const { return sender_; }
    void set_sender(ActorRef* sender) { sender_ = sender; }

    auto timestamp() const { return timestamp_; }

    virtual std::type_index type() const = 0;
    virtual std::string to_string() const = 0;

private:
    ActorRef* sender_;
    std::chrono::steady_clock::time_point timestamp_;
};

// Template message wrapper
template<typename T>
class TypedMessage : public Message {
public:
    explicit TypedMessage(const T& data) : data_(data) {}
    explicit TypedMessage(T&& data) : data_(std::move(data)) {}

    const T& data() const { return data_; }
    T& data() { return data_; }

    std::type_index type() const override {
        return std::type_index(typeid(T));
    }

    std::string to_string() const override {
        return std::string("TypedMessage<") + typeid(T).name() + ">";
    }

private:
    T data_;
};

// Common message types
struct PoisonPill : public Message {
    std::type_index type() const override { return std::type_index(typeid(PoisonPill)); }
    std::string to_string() const override { return "PoisonPill"; }
};

struct Kill : public Message {
    std::type_index type() const override { return std::type_index(typeid(Kill)); }
    std::string to_string() const override { return "Kill"; }
};

struct Ping : public Message {
    Ping(int seq) : sequence_(seq) {}
    std::type_index type() const override { return std::type_index(typeid(Ping)); }
    std::string to_string() const override { return "Ping(" + std::to_string(sequence_) + ")"; }
    int sequence() const { return sequence_; }

private:
    int sequence_;
};

struct Pong : public Message {
    Pong(int seq) : sequence_(seq) {}
    std::type_index type() const override { return std::type_index(typeid(Pong)); }
    std::string to_string() const override { return "Pong(" + std::to_string(sequence_) + ")"; }
    int sequence() const { return sequence_; }

private:
    int sequence_;
};

// Actor reference (like Erlang pid or Akka ActorRef)
class ActorRef {
public:
    ActorRef(ActorSystem* system, const std::string& path)
        : system_(system), path_(path), stopped_(false) {}

    virtual ~ActorRef() = default;

    const std::string& path() const { return path_; }
    ActorSystem* system() const { return system_; }

    bool is_terminated() const { return stopped_; }

    // Message sending
    void tell(std::unique_ptr<Message> message) {
        if (!stopped_) {
            message->set_sender(this);
            deliver_message(std::move(message));
        }
    }

    template<typename T>
    void tell(const T& data) {
        tell(std::make_unique<TypedMessage<T>>(data));
    }

    // Convenience operators
    void operator!(std::unique_ptr<Message> message) {
        tell(std::move(message));
    }

    template<typename T>
    void operator<<(const T& data) {
        tell(data);
    }

protected:
    virtual void deliver_message(std::unique_ptr<Message> message) = 0;

    ActorSystem* system_;
    std::string path_;
    std::atomic<bool> stopped_;
};

// Actor base class
class Actor {
public:
    Actor(ActorRef* self) : self_(self) {}
    virtual ~Actor() = default;

    ActorRef* self() const { return self_; }

    // Message handling
    virtual void receive(std::unique_ptr<Message> message) = 0;

    // Lifecycle hooks
    virtual void pre_start() {}
    virtual void post_stop() {}
    virtual void pre_restart() {}
    virtual void post_restart() {}

    // Actor utilities
    template<typename T>
    void become(std::function<void(std::unique_ptr<Message>)> handler) {
        // Simplified: just store the handler
        current_handler_ = handler;
    }

    void unbecome() {
        current_handler_ = nullptr;
    }

protected:
    ActorRef* self_;
    std::function<void(std::unique_ptr<Message>)> current_handler_;
};

// Local actor reference
class LocalActorRef : public ActorRef {
public:
    LocalActorRef(ActorSystem* system, const std::string& path,
                  std::unique_ptr<Actor> actor)
        : ActorRef(system, path), actor_(std::move(actor)) {
        actor_->pre_start();
    }

    ~LocalActorRef() override {
        stop();
    }

    void stop() {
        if (!stopped_) {
            stopped_ = true;
            actor_->post_stop();
        }
    }

protected:
    void deliver_message(std::unique_ptr<Message> message) override {
        if (stopped_) return;

        // Check for system messages
        if (message->type() == std::type_index(typeid(PoisonPill))) {
            stop();
            return;
        }

        // Deliver to actor
        if (actor_->current_handler_) {
            actor_->current_handler_(std::move(message));
        } else {
            actor_->receive(std::move(message));
        }
    }

private:
    std::unique_ptr<Actor> actor_;
};

// Actor system (like Erlang node or Akka ActorSystem)
class ActorSystem {
public:
    ActorSystem(const std::string& name) : name_(name), running_(true) {
        // Start system dispatcher thread
        dispatcher_thread_ = std::thread([this]() { dispatch_loop(); });
    }

    ~ActorSystem() {
        shutdown();
        if (dispatcher_thread_.joinable()) {
            dispatcher_thread_.join();
        }
    }

    const std::string& name() const { return name_; }

    // Actor creation
    template<typename ActorType, typename... Args>
    ActorRef* create_actor(const std::string& name, Args&&... args) {
        std::string path = name_ + "/" + name;
        auto actor = std::make_unique<ActorType>(std::forward<Args>(args)...);
        auto actor_ref = std::make_unique<LocalActorRef>(this, path, std::move(actor));

        ActorRef* ref = actor_ref.get();
        {
            std::unique_lock<std::mutex> lock(actors_mutex_);
            actors_[path] = std::move(actor_ref);
        }

        return ref;
    }

    // Actor lookup
    ActorRef* find_actor(const std::string& path) {
        std::unique_lock<std::mutex> lock(actors_mutex_);
        auto it = actors_.find(path);
        return it != actors_.end() ? it->second.get() : nullptr;
    }

    // System shutdown
    void shutdown() {
        running_ = false;

        std::unique_lock<std::mutex> lock(actors_mutex_);
        for (auto& pair : actors_) {
            pair.second->stop();
        }
        actors_.clear();
    }

    // Internal message delivery
    void deliver_message(ActorRef* target, std::unique_ptr<Message> message) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        message_queue_.push({target, std::move(message)});
        queue_cv_.notify_one();
    }

private:
    void dispatch_loop() {
        while (running_) {
            std::unique_lock<std::mutex> lock(queue_mutex_);

            // Wait for messages
            queue_cv_.wait(lock, [this]() {
                return !message_queue_.empty() || !running_;
            });

            if (!running_) break;

            // Process all pending messages
            while (!message_queue_.empty()) {
                auto [target, message] = std::move(message_queue_.front());
                message_queue_.pop();

                lock.unlock();

                // Deliver message
                target->deliver_message(std::move(message));

                lock.lock();
            }
        }
    }

    std::string name_;
    std::atomic<bool> running_;
    std::thread dispatcher_thread_;

    std::unordered_map<std::string, std::unique_ptr<LocalActorRef>> actors_;
    std::mutex actors_mutex_;

    std::queue<std::pair<ActorRef*, std::unique_ptr<Message>>> message_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
};

// Helper for creating typed messages
template<typename T>
std::unique_ptr<Message> make_message(const T& data) {
    return std::make_unique<TypedMessage<T>>(data);
}

// Pattern matching utilities
class PatternMatch {
public:
    template<typename T>
    static bool match(const Message& message, std::function<void(const T&)> handler) {
        if (message.type() == std::type_index(typeid(T))) {
            const auto& typed_msg = static_cast<const TypedMessage<T>&>(message);
            handler(typed_msg.data());
            return true;
        }
        return false;
    }

    template<typename T>
    static bool match(std::unique_ptr<Message>& message, std::function<void(const T&)> handler) {
        if (message->type() == std::type_index(typeid(T))) {
            const auto& typed_msg = static_cast<const TypedMessage<T>&>(*message);
            handler(typed_msg.data());
            return true;
        }
        return false;
    }
};

// Receive block (like Akka's receive)
class ReceiveBuilder {
public:
    template<typename T>
    ReceiveBuilder& match(std::function<void(const T&)> handler) {
        matchers_.push_back([handler](std::unique_ptr<Message>& message) {
            return PatternMatch::match<T>(message, handler);
        });
        return *this;
    }

    void handle(std::unique_ptr<Message> message) {
        for (auto& matcher : matchers_) {
            if (matcher(message)) {
                return; // First match wins
            }
        }
        // No match - could log or handle unhandled message
    }

private:
    std::vector<std::function<bool(std::unique_ptr<Message>&)>> matchers_;
};

// Example actors

// Ping-pong actor
class PingPongActor : public Actor {
public:
    PingPongActor(ActorRef* self, int max_pings)
        : Actor(self), max_pings_(max_pings), ping_count_(0) {}

    void receive(std::unique_ptr<Message> message) override {
        if (PatternMatch::match<Ping>(*message, [this](const Ping& ping) {
            std::cout << self()->path() << " received ping " << ping.sequence() << "\n";

            ping_count_++;
            if (ping_count_ < max_pings_) {
                // Send pong back to sender
                message->sender()->tell(make_message(Pong(ping.sequence())));
            } else {
                std::cout << self()->path() << " finished after " << ping_count_ << " pings\n";
            }
        })) return;

        if (PatternMatch::match<Pong>(*message, [this](const Pong& pong) {
            std::cout << self()->path() << " received pong " << pong.sequence() << "\n";
        })) return;
    }

private:
    int max_pings_;
    int ping_count_;
};

// Calculator actor
class CalculatorActor : public Actor {
public:
    CalculatorActor(ActorRef* self) : Actor(self) {}

    void receive(std::unique_ptr<Message> message) override {
        struct Add { int a, b; };
        struct Subtract { int a, b; };
        struct Multiply { int a, b; };
        struct Divide { int a, b; };

        if (PatternMatch::match<Add>(*message, [this](const Add& op) {
            int result = op.a + op.b;
            std::cout << op.a << " + " << op.b << " = " << result << "\n";
            if (message->sender()) {
                message->sender()->tell(result);
            }
        })) return;

        if (PatternMatch::match<Subtract>(*message, [this](const Subtract& op) {
            int result = op.a - op.b;
            std::cout << op.a << " - " << op.b << " = " << result << "\n";
            if (message->sender()) {
                message->sender()->tell(result);
            }
        })) return;

        if (PatternMatch::match<Multiply>(*message, [this](const Multiply& op) {
            int result = op.a * op.b;
            std::cout << op.a << " * " << op.b << " = " << result << "\n";
            if (message->sender()) {
                message->sender()->tell(result);
            }
        })) return;

        if (PatternMatch::match<Divide>(*message, [this](const Divide& op) {
            if (op.b != 0) {
                int result = op.a / op.b;
                std::cout << op.a << " / " << op.b << " = " << result << "\n";
                if (message->sender()) {
                    message->sender()->tell(result);
                }
            } else {
                std::cout << "Division by zero!\n";
            }
        })) return;
    }
};

// Supervisor actor
class SupervisorActor : public Actor {
public:
    SupervisorActor(ActorRef* self, ActorSystem* system)
        : Actor(self), system_(system), restart_count_(0) {}

    void receive(std::unique_ptr<Message> message) override {
        struct CreateWorker { std::string name; };
        struct WorkerFailed { std::string name; };

        if (PatternMatch::match<CreateWorker>(*message, [this](const CreateWorker& cmd) {
            auto worker = system_->create_actor<WorkerActor>(cmd.name, self());
            workers_[cmd.name] = worker;
            std::cout << "Supervisor created worker: " << cmd.name << "\n";
        })) return;

        if (PatternMatch::match<WorkerFailed>(*message, [this](const WorkerFailed& failure) {
            std::cout << "Supervisor handling failure of worker: " << failure.name << "\n";

            restart_count_++;
            if (restart_count_ < 3) {
                // Restart worker
                auto worker = system_->create_actor<WorkerActor>(failure.name, self());
                workers_[failure.name] = worker;
                std::cout << "Supervisor restarted worker: " << failure.name << "\n";
            } else {
                std::cout << "Supervisor giving up on worker: " << failure.name << "\n";
            }
        })) return;
    }

private:
    ActorSystem* system_;
    std::unordered_map<std::string, ActorRef*> workers_;
    int restart_count_;
};

// Worker actor that can fail
class WorkerActor : public Actor {
public:
    WorkerActor(ActorRef* self, ActorRef* supervisor)
        : Actor(self), supervisor_(supervisor), work_count_(0) {}

    void receive(std::unique_ptr<Message> message) override {
        struct DoWork { int task_id; };

        if (PatternMatch::match<DoWork>(*message, [this](const DoWork& work) {
            work_count_++;
            std::cout << self()->path() << " processing task " << work.task_id << "\n";

            // Simulate occasional failure
            if (work_count_ % 5 == 0) {
                std::cout << self()->path() << " failed on task " << work.task_id << "!\n";
                supervisor_->tell(make_message(WorkerFailed{self()->path()}));
                return;
            }

            // Send completion back to sender
            if (message->sender()) {
                message->sender()->tell("Task " + std::to_string(work.task_id) + " completed");
            }
        })) return;
    }

private:
    ActorRef* supervisor_;
    int work_count_;
};

// Router actor (load balancer)
class RouterActor : public Actor {
public:
    RouterActor(ActorRef* self) : Actor(self), next_worker_(0) {}

    void add_worker(ActorRef* worker) {
        workers_.push_back(worker);
    }

    void receive(std::unique_ptr<Message> message) override {
        if (workers_.empty()) {
            std::cout << "Router: No workers available\n";
            return;
        }

        // Round-robin load balancing
        ActorRef* worker = workers_[next_worker_++ % workers_.size()];

        // Forward message to worker
        worker->tell(std::move(message));
    }

private:
    std::vector<ActorRef*> workers_;
    size_t next_worker_;
};

// Demo application
int main() {
    std::cout << "Erlang/Akka-style Actor Event Loop Demo\n";
    std::cout << "=======================================\n\n";

    ActorSystem system("demo-system");

    // 1. Basic ping-pong example
    std::cout << "1. Ping-pong actors:\n";

    auto pinger = system.create_actor<PingPongActor>("pinger", 3);
    auto ponger = system.create_actor<PingPongActor>("ponger", 3);

    // Start ping-pong
    pinger->tell(make_message(Ping(1)));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // 2. Calculator example
    std::cout << "\n2. Calculator actor:\n";

    auto calculator = system.create_actor<CalculatorActor>("calculator");

    struct { int a, b; } operations[] = {
        {10, 5}, {20, 4}, {15, 3}, {100, 7}
    };

    for (auto& op : operations) {
        calculator->tell(make_message(decltype(CalculatorActor().receive(nullptr))::Add{op.a, op.b}));
        calculator->tell(make_message(decltype(CalculatorActor().receive(nullptr))::Subtract{op.a, op.b}));
        calculator->tell(make_message(decltype(CalculatorActor().receive(nullptr))::Multiply{op.a, op.b}));
        calculator->tell(make_message(decltype(CalculatorActor().receive(nullptr))::Divide{op.a, op.b}));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 3. Supervisor hierarchy
    std::cout << "\n3. Supervisor and workers:\n";

    auto supervisor = system.create_actor<SupervisorActor>("supervisor", &system);

    // Create workers through supervisor
    supervisor->tell(make_message(typename SupervisorActor::CreateWorker{"worker1"}));
    supervisor->tell(make_message(typename SupervisorActor::CreateWorker{"worker2"}));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send work to workers (they will fail periodically)
    for (int i = 1; i <= 12; ++i) {
        supervisor->tell(make_message(typename WorkerActor::DoWork{i}));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // 4. Router/load balancer
    std::cout << "\n4. Router/load balancer:\n";

    auto router = system.create_actor<RouterActor>("router");

    // Add calculator as a worker to the router
    static_cast<RouterActor*>(dynamic_cast<LocalActorRef*>(router)->actor_.get())->add_worker(calculator);

    // Send requests through router
    for (int i = 0; i < 5; ++i) {
        router->tell(make_message(decltype(CalculatorActor().receive(nullptr))::Add{i * 10, i}));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 5. Actor lifecycle
    std::cout << "\n5. Actor lifecycle:\n";

    auto temp_actor = system.create_actor<PingPongActor>("temp", 1);
    temp_actor->tell(make_message(Ping(100)));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send poison pill to stop actor
    temp_actor->tell(std::make_unique<PoisonPill>());
    std::cout << "Sent PoisonPill to temp actor\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 6. Pattern matching with ReceiveBuilder
    std::cout << "\n6. Advanced pattern matching:\n";

    class PatternActor : public Actor {
    public:
        PatternActor(ActorRef* self) : Actor(self) {}

        void receive(std::unique_ptr<Message> message) override {
            ReceiveBuilder()
                .match<std::string>([this](const std::string& str) {
                    std::cout << "PatternActor received string: " << str << "\n";
                })
                .match<int>([this](const int& num) {
                    std::cout << "PatternActor received int: " << num << "\n";
                })
                .handle(std::move(message));
        }
    };

    auto pattern_actor = system.create_actor<PatternActor>("pattern");
    pattern_actor->tell(std::string("Hello, World!"));
    pattern_actor->tell(42);
    pattern_actor->tell(3.14); // Won't match any pattern

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "\nShutting down actor system...\n";
    system.shutdown();

    std::cout << "\nDemo completed!\n";

    return 0;
}

/*
 * Key Features Demonstrated:
 *
 * 1. Actor Model Fundamentals:
 *    - Encapsulated state and behavior
 *    - Message-passing communication
 *    - No shared mutable state
 *    - Location transparency
 *
 * 2. Pattern Matching:
 *    - Type-safe message dispatch
 *    - Receive blocks with multiple patterns
 *    - Flexible message handling
 *
 * 3. Fault Tolerance:
 *    - Supervisor hierarchies
 *    - "Let it crash" philosophy
 *    - Actor restart capabilities
 *    - Failure isolation
 *
 * 4. Lifecycle Management:
 *    - Actor creation and destruction
 *    - Poison pill for graceful shutdown
 *    - Pre/post hooks for lifecycle events
 *
 * 5. Load Balancing:
 *    - Router actors for distribution
 *    - Round-robin load balancing
 *    - Dynamic worker management
 *
 * 6. Advanced Patterns:
 *    - Become/unbecome for state changes
 *    - Typed message system
 *    - Actor references and paths
 *
 * Real-World Applications:
 * - Erlang/OTP (WhatsApp, RabbitMQ, CouchDB)
 * - Akka framework (Lightbend, Play Framework)
 * - Microsoft Orleans (Azure services, Halo)
 * - Actor-based game servers (MMORPGs)
 * - IoT device management systems
 * - Financial trading platforms
 * - Telecom switching systems
 */
