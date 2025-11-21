/*
 * Creational Pattern: Singleton
 *
 * Demonstrates the Singleton pattern with thread-safe lazy initialization
 * using C++11 static local variable initialization (Meyers' Singleton).
 */
#include <iostream>
#include <mutex>
#include <string>
#include <cassert>

// Thread-safety: Thread-safe (uses static local variable, C++11 guarantees)
// Ownership: Owns single instance (static lifetime)
// Invariants: Only one instance exists
// Failure modes: None (static initialization is exception-safe)
class PrinterService {
private:
    std::string mode_;

    // Thread-safety: Thread-safe (private constructor)
    // Ownership: Constructs instance
    // Invariants: None
    // Failure modes: None
    PrinterService() : mode_("GrayScale") {}

public:
    // Delete copy constructor and assignment operator
    PrinterService(const PrinterService&) = delete;
    PrinterService& operator=(const PrinterService&) = delete;

    // Thread-safety: Thread-safe (C++11 static local initialization is thread-safe)
    // Ownership: Returns reference to static instance
    // Invariants: Always returns same instance
    // Failure modes: None
    static PrinterService& getInstance() {
        static PrinterService instance;
        return instance;
    }

    // Thread-safety: Not thread-safe (reads mode_ without synchronization)
    // Ownership: Returns copy of mode_
    // Invariants: None
    // Failure modes: Undefined behavior if newMode is empty
    // Note: getInstance() is thread-safe, but method calls are not synchronized.
    // For thread-safe method calls, see threadsafe/creational/singleton_threadsafe.cpp
    std::string getPrinterStatus() const {
        return mode_;
    }

    // Thread-safety: Not thread-safe (modifies mode_ without synchronization)
    // Ownership: Takes ownership of newMode parameter (copies)
    // Invariants: newMode must be non-empty
    // Failure modes: Undefined behavior if newMode is empty
    // Note: getInstance() is thread-safe, but method calls are not synchronized.
    // For thread-safe method calls, see threadsafe/creational/singleton_threadsafe.cpp
    void setMode(const std::string& newMode) {
        assert(!newMode.empty() && "Mode must be non-empty");
        mode_ = newMode;
        std::cout << "Mode changed to " << mode_ << std::endl;
    }
};

int main() {
    PrinterService& worker1 = PrinterService::getInstance();
    PrinterService& worker2 = PrinterService::getInstance();

    worker1.setMode("Color");
    worker2.setMode("Grayscale");

    std::cout << worker1.getPrinterStatus() << std::endl;
    std::cout << worker2.getPrinterStatus() << std::endl;

    // Verify both references point to the same instance
    std::cout << "Same instance: " << (&worker1 == &worker2 ? "Yes" : "No") << std::endl;

    return 0;
}
