/*
 * Structural Pattern: Adapter
 *
 * Demonstrates the Adapter pattern for converting the interface of one class
 * to another interface that clients expect.
 */
#include <iostream>
#include <string>

// Thread-safety: Not thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class JsonLogger {
public:
    virtual ~JsonLogger() = default;

    // Thread-safety: Not thread-safe (may modify state)
    // Ownership: Borrows message (read-only)
    // Invariants: message must be non-empty
    // Failure modes: Undefined behavior if message is empty
    virtual void logMessage(const std::string& message) = 0;
};

// Thread-safety: Not thread-safe (may modify state via output)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: None
class XmlLogger {
public:
    // Thread-safety: Not thread-safe (may modify state via output)
    // Ownership: Borrows xmlMessage (read-only)
    // Invariants: xmlMessage must be non-empty
    // Failure modes: Undefined behavior if xmlMessage is empty
    void log(const std::string& xmlMessage) {
        std::cout << xmlMessage << std::endl;
    }
};

// Thread-safety: Not thread-safe (mutable state)
// Ownership: Owns XmlLogger instance
// Invariants: xmlLogger_ must be valid
// Failure modes: Undefined behavior if xmlLogger_ is invalid
class LoggerAdapter : public JsonLogger {
private:
    XmlLogger xmlLogger_;

public:
    // Thread-safety: Not thread-safe (constructor copies xmlLogger)
    // Ownership: Takes ownership of xmlLogger (copies)
    // Invariants: None
    // Failure modes: None
    explicit LoggerAdapter(const XmlLogger& xmlLogger) : xmlLogger_(xmlLogger) {}

    // Thread-safety: Not thread-safe (calls xmlLogger_.log)
    // Ownership: Borrows message (read-only)
    // Invariants: message must be non-empty
    // Failure modes: Undefined behavior if message is empty
    void logMessage(const std::string& message) override {
        xmlLogger_.log(message);
    }
};

int main() {
    LoggerAdapter logger{XmlLogger()};
    logger.logMessage("<message>hello</message>");
    return 0;
}
