/*
 * Structural Pattern: Thread-Safe Facade
 *
 * Demonstrates thread-safe Facade pattern implementation with proper
 * synchronization for concurrent subsystem operations.
 */
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <cassert>

// Thread-safety: Thread-safe (uses shared_mutex for read-write operations)
// Ownership: Owns no resources
// Invariants: temperature must be in valid range (-50 to 50)
// Failure modes: Undefined behavior if invariants violated
class ThreadSafeSmartHomeSubSystem {
public:
    enum class Brightness {
        UNKNOWN,
        BRIGHT,
        DIM
    };

    enum class Service {
        UNKNOWN,
        HULU,
        NETFLIX,
        HBO
    };

private:
    mutable std::shared_mutex mutex_;  // Mutable for const methods
    Brightness brightness_;
    int temperature_;
    bool isSecurityArmed_;
    Service streamingService_;

public:
    // Thread-safety: Thread-safe (constructor initializes members)
    // Ownership: Initializes members
    // Invariants: temperature must be in valid range
    // Failure modes: None
    ThreadSafeSmartHomeSubSystem()
        : brightness_(Brightness::UNKNOWN),
          temperature_(19),
          isSecurityArmed_(false),
          streamingService_(Service::UNKNOWN) {}

    // Thread-safety: Thread-safe (locks mutex exclusively)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void setBrightness(Brightness brightness) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        brightness_ = brightness;
    }

    // Thread-safety: Thread-safe (locks mutex exclusively)
    // Ownership: None
    // Invariants: temperature must be in valid range
    // Failure modes: Undefined behavior if temperature out of range
    void setTemperature(int temperature) {
        assert(temperature >= -50 && temperature <= 50 && "Temperature out of range");
        std::unique_lock<std::shared_mutex> lock(mutex_);
        temperature_ = temperature;
    }

    // Thread-safety: Thread-safe (locks mutex exclusively)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void setIsSecurityArmed(bool isSecurityArmed) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        isSecurityArmed_ = isSecurityArmed;
    }

    // Thread-safety: Thread-safe (locks mutex exclusively)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void setStreamingService(Service streamingService) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        streamingService_ = streamingService;
    }

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: Returns copy of enum value
    // Invariants: None
    // Failure modes: None
    Brightness getBrightness() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return brightness_;
    }

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: Returns copy of temperature
    // Invariants: None
    // Failure modes: None
    int getTemperature() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return temperature_;
    }

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: Returns copy of security status
    // Invariants: None
    // Failure modes: None
    bool isSecurityArmed() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return isSecurityArmed_;
    }

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: Returns copy of service
    // Invariants: None
    // Failure modes: None
    Service getStreamingService() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return streamingService_;
    }
};

// Thread-safety: Thread-safe (uses mutex for facade operations)
// Ownership: Borrows smartHome reference
// Invariants: smartHome must be valid
// Failure modes: Undefined behavior if smartHome is invalid
class ThreadSafeSmartHomeFacade {
private:
    ThreadSafeSmartHomeSubSystem& smartHome_;
    mutable std::mutex facadeMutex_;  // Protects facade operations

public:
    // Thread-safety: Thread-safe (constructor stores reference)
    // Ownership: Borrows smartHome reference
    // Invariants: smartHome must be valid
    // Failure modes: Undefined behavior if smartHome is invalid
    explicit ThreadSafeSmartHomeFacade(ThreadSafeSmartHomeSubSystem& smartHome)
        : smartHome_(smartHome) {}

    // Thread-safety: Thread-safe (locks mutex, calls thread-safe methods)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void setMovieMode() {
        std::lock_guard<std::mutex> lock(facadeMutex_);
        smartHome_.setBrightness(ThreadSafeSmartHomeSubSystem::Brightness::DIM);
        smartHome_.setTemperature(21);
        smartHome_.setIsSecurityArmed(false);
        smartHome_.setStreamingService(ThreadSafeSmartHomeSubSystem::Service::NETFLIX);
        std::cout << "[Thread " << std::this_thread::get_id() << "] Movie mode set" << std::endl;
    }

    // Thread-safety: Thread-safe (locks mutex, calls thread-safe methods)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void setFocusMode() {
        std::lock_guard<std::mutex> lock(facadeMutex_);
        smartHome_.setBrightness(ThreadSafeSmartHomeSubSystem::Brightness::BRIGHT);
        smartHome_.setTemperature(22);
        smartHome_.setIsSecurityArmed(true);
        smartHome_.setStreamingService(ThreadSafeSmartHomeSubSystem::Service::UNKNOWN);
        std::cout << "[Thread " << std::this_thread::get_id() << "] Focus mode set" << std::endl;
    }

    // Thread-safety: Thread-safe (locks mutex for read)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void displayStatus() const {
        std::lock_guard<std::mutex> lock(facadeMutex_);
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "Brightness: " << static_cast<int>(smartHome_.getBrightness())
                  << ", Temperature: " << smartHome_.getTemperature()
                  << ", Security: " << (smartHome_.isSecurityArmed() ? "Armed" : "Disarmed")
                  << std::endl;
    }
};

void setModesThread(ThreadSafeSmartHomeFacade& facade, int threadId) {
    if (threadId % 2 == 0) {
        facade.setMovieMode();
    } else {
        facade.setFocusMode();
    }
    facade.displayStatus();
}

void readStatusThread(const ThreadSafeSmartHomeFacade& facade, int threadId) {
    for (int i = 0; i < 3; ++i) {
        facade.displayStatus();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main() {
    ThreadSafeSmartHomeSubSystem smartHome;
    ThreadSafeSmartHomeFacade facade(smartHome);

    std::vector<std::thread> writerThreads;
    for (int i = 0; i < 2; ++i) {
        writerThreads.emplace_back(setModesThread, std::ref(facade), i);
    }

    std::vector<std::thread> readerThreads;
    for (int i = 0; i < 2; ++i) {
        readerThreads.emplace_back(readStatusThread, std::cref(facade), i);
    }

    for (auto& t : writerThreads) {
        t.join();
    }
    for (auto& t : readerThreads) {
        t.join();
    }

    return 0;
}

