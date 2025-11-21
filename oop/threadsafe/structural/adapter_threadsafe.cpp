/*
 * Structural Pattern: Thread-Safe Adapter
 *
 * Demonstrates thread-safe Adapter pattern implementation with proper
 * synchronization for concurrent logging operations.
 */
#include <iostream>
#include <string>
#include <mutex>
#include <thread>
#include <vector>
#include <cassert>

// Thread-safety: Thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class ThreadSafeJsonLogger {
public:
    virtual ~ThreadSafeJsonLogger() = default;

    // Thread-safety: Thread-safe (must be implemented thread-safely)
    // Ownership: Borrows message (read-only)
    // Invariants: message must be non-empty
    // Failure modes: Undefined behavior if message is empty
    virtual void logMessage(const std::string& message) = 0;
};

// Thread-safety: Thread-safe (uses mutex for output synchronization)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: None
class ThreadSafeXmlLogger {
private:
    mutable std::mutex mutex_;  // Mutable for const methods

public:
    // Thread-safety: Thread-safe (locks mutex for output)
    // Ownership: Borrows xmlMessage (read-only)
    // Invariants: xmlMessage must be non-empty
    // Failure modes: Undefined behavior if xmlMessage is empty
    void log(const std::string& xmlMessage) {
        assert(!xmlMessage.empty() && "Message must be non-empty");
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << xmlMessage << std::endl;
    }
};

// Thread-safety: Thread-safe (uses mutex for adapter operations)
// Ownership: Owns XmlLogger instance
// Invariants: xmlLogger_ must be valid
// Failure modes: Undefined behavior if xmlLogger_ is invalid
class ThreadSafeLoggerAdapter : public ThreadSafeJsonLogger {
private:
    ThreadSafeXmlLogger xmlLogger_;
    mutable std::mutex adapterMutex_;  // Additional protection if needed

public:
    // Thread-safety: Thread-safe (constructor copies xmlLogger)
    // Ownership: Takes ownership of xmlLogger (copies)
    // Invariants: None
    // Failure modes: None
    explicit ThreadSafeLoggerAdapter(const ThreadSafeXmlLogger& xmlLogger)
        : xmlLogger_(xmlLogger) {}

    // Thread-safety: Thread-safe (locks mutex, calls thread-safe log)
    // Ownership: Borrows message (read-only)
    // Invariants: message must be non-empty
    // Failure modes: Undefined behavior if message is empty
    void logMessage(const std::string& message) override {
        assert(!message.empty() && "Message must be non-empty");
        std::lock_guard<std::mutex> lock(adapterMutex_);
        xmlLogger_.log(message);
    }
};

void logMessagesThread(ThreadSafeJsonLogger& logger, int threadId) {
    for (int i = 0; i < 5; ++i) {
        std::string message = "<message>Thread_" + std::to_string(threadId) +
                              "_Message_" + std::to_string(i) + "</message>";
        logger.logMessage(message);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    ThreadSafeXmlLogger xmlLogger;
    ThreadSafeLoggerAdapter adapter(xmlLogger);

    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(logMessagesThread, std::ref(adapter), i);
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}

