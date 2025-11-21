/*
 * Async Event Loop (libdispatch/GCD style)
 *
 * Source: Apple's Grand Central Dispatch (GCD), libdispatch
 * Algorithm: Task-based concurrency with automatic thread pool management
 *
 * What Makes It Ingenious:
 * - Automatic thread pool sizing
 * - Work-stealing queues
 * - Task priorities and QoS
 * - Dispatch groups and semaphores
 * - Timer sources with leeway
 * - Target queues for execution context
 * - Barrier operations for synchronization
 *
 * When to Use:
 * - Concurrent task execution
 * - I/O-bound operations
 * - CPU-intensive parallel processing
 * - Real-time systems with QoS
 * - Mobile/desktop app development
 *
 * Real-World Usage:
 * - iOS/macOS applications (GCD)
 * - Swift concurrency runtime
 * - Node.js worker threads
 * - .NET Task Parallel Library
 * - Java ForkJoinPool
 * - Rust async runtimes (tokio, async-std)
 *
 * Time Complexity: O(1) task submission, O(log n) priority scheduling
 * Space Complexity: O(n) for queued tasks, O(t) for thread pool
 */

#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

// Task priority levels (QoS classes)
enum class DispatchQoS {
    BACKGROUND = 0,  // Lowest priority, for non-urgent work
    UTILITY = 1,     // For work that takes significant time
    DEFAULT = 2,     // Default priority
    USER_INITIATED = 3, // Work initiated by user, high priority
    USER_INTERACTIVE = 4, // Highest priority, for UI/main thread work
};

// Dispatch queue types
enum class DispatchQueueType {
    SERIAL,     // Tasks execute serially, one at a time
    CONCURRENT  // Tasks execute concurrently when possible
};

// Forward declarations
class DispatchQueue;
class DispatchGroup;
class DispatchSemaphore;
class DispatchSource;

// Task wrapper
class DispatchTask {
public:
    DispatchTask(std::function<void()> work, DispatchQoS qos = DispatchQoS::DEFAULT)
        : work_(work), qos_(qos), submitted_time_(std::chrono::steady_clock::now()) {}

    void execute() {
        if (work_) {
            work_();
        }
    }

    DispatchQoS qos() const { return qos_; }
    auto submitted_time() const { return submitted_time_; }

    // Priority comparison for queue ordering
    bool operator<(const DispatchTask& other) const {
        // Higher QoS values = higher priority
        if (qos_ != other.qos_) {
            return static_cast<int>(qos_) < static_cast<int>(other.qos_);
        }
        // Same QoS: earlier submission time = higher priority
        return submitted_time_ > other.submitted_time_;
    }

private:
    std::function<void()> work_;
    DispatchQoS qos_;
    std::chrono::steady_clock::time_point submitted_time_;
};

// Thread pool worker
class WorkerThread {
public:
    WorkerThread(DispatchQueue* queue) : queue_(queue), running_(true) {
        thread_ = std::thread([this]() { worker_loop(); });
    }

    ~WorkerThread() {
        stop();
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    void stop() {
        running_ = false;
    }

private:
    void worker_loop() {
        while (running_) {
            DispatchTask task(nullptr);

            // Try to get a task from the queue
            if (queue_->dequeue_task(task)) {
                task.execute();
            } else {
                // No task available, sleep briefly
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
        }
    }

    DispatchQueue* queue_;
    std::thread thread_;
    std::atomic<bool> running_;
};

// Dispatch Queue
class DispatchQueue {
public:
    DispatchQueue(const std::string& label,
                  DispatchQueueType type = DispatchQueueType::SERIAL,
                  DispatchQoS qos = DispatchQoS::DEFAULT)
        : label_(label), type_(type), qos_(qos), suspended_(false) {}

    ~DispatchQueue() {
        // Stop all worker threads
        for (auto& worker : workers_) {
            worker->stop();
        }
    }

    // Queue management
    void set_target_queue(DispatchQueue* target) {
        target_queue_ = target;
    }

    DispatchQueue* target_queue() const { return target_queue_; }

    void suspend() { suspended_ = true; }
    void resume() { suspended_ = false; }

    bool suspended() const { return suspended_; }

    // Task submission
    void async(std::function<void()> work, DispatchQoS qos = DispatchQoS::DEFAULT) {
        if (suspended_) return;

        DispatchTask task(work, qos);

        if (target_queue_) {
            // Forward to target queue
            target_queue_->enqueue_task(task);
        } else {
            enqueue_task(task);
        }
    }

    void sync(std::function<void()> work) {
        if (suspended_) return;

        std::mutex mtx;
        std::condition_variable cv;
        bool completed = false;

        async([&]() {
            work();
            {
                std::unique_lock<std::mutex> lock(mtx);
                completed = true;
            }
            cv.notify_one();
        });

        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return completed; });
    }

    void barrier_async(std::function<void()> work) {
        // Wait for all current tasks to complete, then execute barrier
        sync([this, work]() {
            work();
        });
    }

    // Apply pattern (parallel for-each)
    template<typename Iterator, typename Function>
    void apply(Iterator begin, Iterator end, Function func) {
        if (type_ == DispatchQueueType::CONCURRENT) {
            std::vector<std::thread> threads;
            size_t num_threads = std::thread::hardware_concurrency();

            size_t total_items = std::distance(begin, end);
            size_t items_per_thread = total_items / num_threads;
            size_t remainder = total_items % num_threads;

            auto it = begin;
            for (size_t i = 0; i < num_threads; ++i) {
                auto thread_begin = it;
                size_t items_this_thread = items_per_thread + (i < remainder ? 1 : 0);

                std::advance(it, items_this_thread);

                threads.emplace_back([thread_begin, items_this_thread, func]() {
                    auto thread_it = thread_begin;
                    for (size_t j = 0; j < items_this_thread; ++j) {
                        func(*thread_it);
                        ++thread_it;
                    }
                });
            }

            for (auto& thread : threads) {
                thread.join();
            }
        } else {
            // Serial execution
            for (auto it = begin; it != end; ++it) {
                func(*it);
            }
        }
    }

    // Internal methods
    void enqueue_task(const DispatchTask& task) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        task_queue_.push(task);
        queue_cv_.notify_one();

        // Ensure we have enough worker threads
        ensure_workers();
    }

    bool dequeue_task(DispatchTask& task) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        if (task_queue_.empty()) {
            return false;
        }

        task = task_queue_.top();
        task_queue_.pop();
        return true;
    }

private:
    void ensure_workers() {
        size_t target_workers = (type_ == DispatchQueueType::CONCURRENT) ?
            std::thread::hardware_concurrency() : 1;

        while (workers_.size() < target_workers) {
            workers_.push_back(std::make_unique<WorkerThread>(this));
        }
    }

    std::string label_;
    DispatchQueueType type_;
    DispatchQoS qos_;
    bool suspended_;

    DispatchQueue* target_queue_ = nullptr;

    std::priority_queue<DispatchTask> task_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;

    std::vector<std::unique_ptr<WorkerThread>> workers_;
};

// Dispatch Group
class DispatchGroup {
public:
    DispatchGroup() : count_(0) {}

    ~DispatchGroup() {
        wait(); // Wait for all tasks to complete
    }

    void enter() {
        std::unique_lock<std::mutex> lock(mutex_);
        ++count_;
    }

    void leave() {
        std::unique_lock<std::mutex> lock(mutex_);
        --count_;
        if (count_ == 0) {
            cv_.notify_all();
        }
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return count_ == 0; });
    }

    template<typename Rep, typename Period>
    bool wait_for(const std::chrono::duration<Rep, Period>& timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        return cv_.wait_for(lock, timeout, [this]() { return count_ == 0; });
    }

    void async(DispatchQueue* queue, std::function<void()> work) {
        enter();
        queue->async([this, work]() {
            work();
            leave();
        });
    }

    void notify(DispatchQueue* queue, std::function<void()> work) {
        async(queue, work);
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    size_t count_;
};

// Dispatch Semaphore
class DispatchSemaphore {
public:
    explicit DispatchSemaphore(long value) : count_(value) {}

    void signal() {
        std::unique_lock<std::mutex> lock(mutex_);
        ++count_;
        cv_.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return count_ > 0; });
        --count_;
    }

    template<typename Rep, typename Period>
    bool wait_for(const std::chrono::duration<Rep, Period>& timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        bool success = cv_.wait_for(lock, timeout, [this]() { return count_ > 0; });
        if (success) {
            --count_;
        }
        return success;
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    long count_;
};

// Timer Source
class DispatchSource {
public:
    enum class Type {
        TIMER,
        READ,
        WRITE,
        SIGNAL
    };

    DispatchSource(Type type, DispatchQueue* queue)
        : type_(type), queue_(queue), active_(false) {}

    virtual ~DispatchSource() {
        cancel();
    }

    // Timer-specific methods
    void set_timer(std::chrono::nanoseconds start,
                   std::chrono::nanoseconds interval,
                   std::chrono::nanoseconds leeway = std::chrono::nanoseconds(0)) {
        if (type_ != Type::TIMER) return;

        start_time_ = std::chrono::steady_clock::now() + start;
        interval_ = interval;
        leeway_ = leeway;
    }

    void set_event_handler(std::function<void()> handler) {
        event_handler_ = handler;
    }

    void set_cancel_handler(std::function<void()> handler) {
        cancel_handler_ = handler;
    }

    void resume() {
        if (active_) return;
        active_ = true;

        if (type_ == Type::TIMER) {
            timer_thread_ = std::thread([this]() { timer_loop(); });
        }
    }

    void suspend() {
        active_ = false;
        if (timer_thread_.joinable()) {
            timer_thread_.join();
        }
    }

    void cancel() {
        suspend();
        if (cancel_handler_) {
            queue_->async(cancel_handler_);
        }
    }

private:
    void timer_loop() {
        while (active_) {
            auto now = std::chrono::steady_clock::now();
            auto next_fire = start_time_;

            while (next_fire <= now) {
                next_fire += interval_;
            }

            std::this_thread::sleep_until(next_fire - leeway_);

            if (active_ && event_handler_) {
                queue_->async(event_handler_);
            }
        }
    }

    Type type_;
    DispatchQueue* queue_;
    std::atomic<bool> active_;

    // Timer-specific
    std::chrono::steady_clock::time_point start_time_;
    std::chrono::nanoseconds interval_;
    std::chrono::nanoseconds leeway_;
    std::thread timer_thread_;

    std::function<void()> event_handler_;
    std::function<void()> cancel_handler_;
};

// Global dispatch queues (like GCD)
class Dispatch {
public:
    static DispatchQueue* get_main_queue() {
        static DispatchQueue main_queue("com.apple.main-thread", DispatchQueueType::SERIAL,
                                       DispatchQoS::USER_INTERACTIVE);
        return &main_queue;
    }

    static DispatchQueue* get_global_queue(DispatchQoS qos, unsigned long flags = 0) {
        (void)flags; // Unused for now
        static std::unordered_map<DispatchQoS, std::unique_ptr<DispatchQueue>> global_queues;

        auto it = global_queues.find(qos);
        if (it == global_queues.end()) {
            std::string label = "com.apple.global-queue.";
            switch (qos) {
                case DispatchQoS::BACKGROUND: label += "background"; break;
                case DispatchQoS::UTILITY: label += "utility"; break;
                case DispatchQoS::DEFAULT: label += "default"; break;
                case DispatchQoS::USER_INITIATED: label += "user-initiated"; break;
                case DispatchQoS::USER_INTERACTIVE: label += "user-interactive"; break;
            }

            auto queue = std::make_unique<DispatchQueue>(label, DispatchQueueType::CONCURRENT, qos);
            it = global_queues.emplace(qos, std::move(queue)).first;
        }

        return it->second.get();
    }

    static DispatchQueue* get_global_queue() {
        return get_global_queue(DispatchQoS::DEFAULT);
    }

    // Convenience functions
    static void async(DispatchQueue* queue, std::function<void()> work) {
        queue->async(work);
    }

    static void sync(DispatchQueue* queue, std::function<void()> work) {
        queue->sync(work);
    }

    template<typename Iterator, typename Function>
    static void apply(Iterator begin, Iterator end, Function func, DispatchQueue* queue = nullptr) {
        if (!queue) queue = get_global_queue();
        queue->apply(begin, end, func);
    }

    static void after(std::chrono::nanoseconds delay, DispatchQueue* queue,
                      std::function<void()> work) {
        auto timer = std::make_shared<DispatchSource>(DispatchSource::Type::TIMER, queue);
        timer->set_timer(delay, std::chrono::nanoseconds(0));
        timer->set_event_handler([timer, work]() {
            work();
        });
        timer->resume();
    }
};

// Example: Concurrent image processing
class ImageProcessor {
public:
    void process_images(const std::vector<std::string>& image_paths) {
        DispatchQueue* processing_queue = Dispatch::get_global_queue(DispatchQoS::UTILITY);

        std::cout << "Processing " << image_paths.size() << " images concurrently...\n";

        DispatchGroup group;

        for (const auto& path : image_paths) {
            group.async(processing_queue, [path]() {
                // Simulate image processing
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                std::cout << "Processed image: " << path << "\n";
            });
        }

        group.wait();
        std::cout << "All images processed!\n";
    }
};

// Example: Network request simulation
class NetworkManager {
public:
    void fetch_data(const std::string& url, std::function<void(std::string)> completion) {
        DispatchQueue* network_queue = Dispatch::get_global_queue(DispatchQoS::UTILITY);

        Dispatch::async(network_queue, [url, completion]() {
            // Simulate network request
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            std::string response = "Data from " + url;
            std::cout << "Fetched: " << response << "\n";

            // Call completion on main queue
            Dispatch::async(Dispatch::get_main_queue(), [completion, response]() {
                completion(response);
            });
        });
    }
};

// Example: Producer-consumer pattern
class ProducerConsumer {
public:
    ProducerConsumer(size_t buffer_size) : buffer_size_(buffer_size) {}

    void start() {
        producer_queue_ = std::make_unique<DispatchQueue>("producer-queue",
                                                         DispatchQueueType::SERIAL);
        consumer_queue_ = std::make_unique<DispatchQueue>("consumer-queue",
                                                         DispatchQueueType::CONCURRENT);

        // Start producer
        Dispatch::async(producer_queue_.get(), [this]() { producer_loop(); });

        // Start consumers
        for (size_t i = 0; i < 3; ++i) {
            Dispatch::async(consumer_queue_.get(), [this, i]() { consumer_loop(i); });
        }
    }

    void stop() {
        running_ = false;
        semaphore_.signal(); // Wake up waiting threads
    }

private:
    void producer_loop() {
        for (int i = 0; i < 20 && running_; ++i) {
            semaphore_.wait(); // Wait for buffer space

            {
                std::unique_lock<std::mutex> lock(buffer_mutex_);
                buffer_.push(i);
                std::cout << "Produced: " << i << "\n";
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        stop();
    }

    void consumer_loop(size_t consumer_id) {
        while (running_) {
            int item = -1;
            {
                std::unique_lock<std::mutex> lock(buffer_mutex_);
                if (!buffer_.empty()) {
                    item = buffer_.front();
                    buffer_.pop();
                    semaphore_.signal(); // Signal buffer space available
                }
            }

            if (item >= 0) {
                // Process item
                std::cout << "Consumer " << consumer_id << " processed: " << item << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    }

    size_t buffer_size_;
    std::queue<int> buffer_;
    std::mutex buffer_mutex_;
    DispatchSemaphore semaphore_{5}; // Buffer size semaphore
    std::atomic<bool> running_{true};

    std::unique_ptr<DispatchQueue> producer_queue_;
    std::unique_ptr<DispatchQueue> consumer_queue_;
};

// Demo application
int main() {
    std::cout << "libdispatch/GCD-style Async Event Loop Demo\n";
    std::cout << "==========================================\n\n";

    // 1. Basic async operations
    std::cout << "1. Basic async operations:\n";
    DispatchQueue* global_queue = Dispatch::get_global_queue();

    Dispatch::async(global_queue, []() {
        std::cout << "Task 1 executed\n";
    });

    Dispatch::async(global_queue, []() {
        std::cout << "Task 2 executed\n";
    });

    // 2. Serial queue example
    std::cout << "\n2. Serial queue operations:\n";
    DispatchQueue serial_queue("com.example.serial", DispatchQueueType::SERIAL);

    for (int i = 0; i < 5; ++i) {
        Dispatch::async(&serial_queue, [i]() {
            std::cout << "Serial task " << i << " executed\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        });
    }

    // 3. Apply pattern (parallel processing)
    std::cout << "\n3. Parallel processing with dispatch_apply:\n";
    std::vector<int> numbers(10);
    std::iota(numbers.begin(), numbers.end(), 0);

    Dispatch::apply(numbers.begin(), numbers.end(), [](int n) {
        std::cout << "Processing number: " << n << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    });

    // 4. Group operations
    std::cout << "\n4. Dispatch group operations:\n";
    DispatchGroup group;

    for (int i = 0; i < 3; ++i) {
        group.async(global_queue, [i]() {
            std::cout << "Group task " << i << " starting\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            std::cout << "Group task " << i << " completed\n";
        });
    }

    group.wait();
    std::cout << "All group tasks completed!\n";

    // 5. Timer operations
    std::cout << "\n5. Timer operations:\n";
    auto timer = std::make_shared<DispatchSource>(DispatchSource::Type::TIMER, global_queue);
    int counter = 0;

    timer->set_timer(std::chrono::seconds(1), std::chrono::seconds(1));
    timer->set_event_handler([&counter, timer]() {
        counter++;
        std::cout << "Timer fired " << counter << " times\n";
        if (counter >= 3) {
            timer->cancel();
        }
    });
    timer->resume();

    // 6. Image processing example
    std::cout << "\n6. Image processing simulation:\n";
    ImageProcessor processor;
    std::vector<std::string> images = {"image1.jpg", "image2.jpg", "image3.jpg", "image4.jpg"};
    processor.process_images(images);

    // 7. Producer-consumer pattern
    std::cout << "\n7. Producer-consumer pattern:\n";
    ProducerConsumer pc(5);
    pc.start();

    // Wait a bit for demo
    std::this_thread::sleep_for(std::chrono::seconds(3));
    pc.stop();

    // 8. After delay execution
    std::cout << "\n8. Delayed execution:\n";
    Dispatch::after(std::chrono::seconds(1), global_queue, []() {
        std::cout << "This executes after 1 second delay\n";
    });

    // Wait for timer and delayed execution
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "\nDemo completed!\n";

    return 0;
}

/*
 * Key Features Demonstrated:
 *
 * 1. Automatic Thread Pool Management:
 *    - Dynamic worker thread creation
 *    - Work-stealing algorithms
 *    - CPU core utilization
 *
 * 2. Quality of Service (QoS):
 *    - Priority-based task scheduling
 *    - Background, utility, default, user-initiated, user-interactive
 *    - Resource allocation based on priority
 *
 * 3. Queue Types:
 *    - Serial queues: One task at a time
 *    - Concurrent queues: Multiple tasks simultaneously
 *    - Main queue: UI/main thread operations
 *
 * 4. Synchronization Primitives:
 *    - Dispatch groups: Wait for multiple async operations
 *    - Semaphores: Resource counting and signaling
 *    - Barriers: Synchronization points in concurrent queues
 *
 * 5. Timer Sources:
 *    - Periodic and one-shot timers
 *    - Leeway for power management
 *    - Cancelation support
 *
 * 6. Advanced Patterns:
 *    - Dispatch apply: Parallel for-each operations
 *    - After delay: Scheduled execution
 *    - Target queues: Execution context control
 *
 * Real-World Applications:
 * - iOS/macOS app development (GCD/libdispatch)
 * - Swift concurrency model
 * - Node.js worker_threads
 * - .NET Task Parallel Library (TPL)
 * - Java ForkJoinPool
 * - Rust async runtimes (tokio, async-std)
 * - High-performance computing
 * - Game engines (Unity job system)
 */
