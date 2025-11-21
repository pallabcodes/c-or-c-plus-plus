/*
 * Object-Oriented Programming: Thread-Safe Structs and Interfaces
 *
 * Demonstrates thread-safe struct and interface implementation with proper
 * synchronization for concurrent access.
 */
#include <iostream>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <cassert>

// Thread-safety: Thread-safe (uses mutex for state protection)
// Ownership: Owns string member
// Invariants: Name must be non-empty, Age > 0
// Failure modes: Undefined behavior if invariants violated
class ThreadSafePerson {
private:
    mutable std::shared_mutex mutex_;  // Mutable for const methods
    std::string name_;
    int age_;

public:
    // Thread-safety: Thread-safe (constructor initializes members)
    // Ownership: Takes ownership of name (copies)
    // Invariants: name must be non-empty, age > 0
    // Failure modes: Undefined behavior if invariants violated
    ThreadSafePerson(const std::string& name, int age) : name_(name), age_(age) {
        assert(!name.empty() && "Name must be non-empty");
        assert(age > 0 && "Age must be positive");
    }

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: Returns copy of name
    // Invariants: None
    // Failure modes: None
    std::string getName() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return name_;
    }

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: Returns copy of age
    // Invariants: None
    // Failure modes: None
    int getAge() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return age_;
    }

    // Thread-safety: Thread-safe (locks mutex exclusively)
    // Ownership: Takes ownership of name (copies)
    // Invariants: name must be non-empty
    // Failure modes: Undefined behavior if name is empty
    void setName(const std::string& name) {
        assert(!name.empty() && "Name must be non-empty");
        std::unique_lock<std::shared_mutex> lock(mutex_);
        name_ = name;
    }

    // Thread-safety: Thread-safe (locks mutex exclusively)
    // Ownership: None
    // Invariants: age > 0
    // Failure modes: Undefined behavior if age <= 0
    void setAge(int age) {
        assert(age > 0 && "Age must be positive");
        std::unique_lock<std::shared_mutex> lock(mutex_);
        age_ = age;
    }

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: None
    // Invariants: Name must be non-empty, Age > 0
    // Failure modes: Undefined behavior if Name is empty or Age <= 0
    void Introduce() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        assert(!name_.empty() && "Name must be non-empty");
        assert(age_ > 0 && "Age must be positive");
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "Hello, my name is " << name_ << " and I'm " << age_
                  << " years old." << std::endl;
    }
};

// Thread-safety: Thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class ThreadSafeISpeaker {
public:
    virtual ~ThreadSafeISpeaker() = default;

    // Thread-safety: Thread-safe (must be implemented thread-safely)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void Speak() = 0;
};

// Thread-safety: Thread-safe (uses mutex for output synchronization)
// Ownership: Owns no resources
// Invariants: None
// Failure modes: None
class ThreadSafeSpeaker : public ThreadSafeISpeaker {
private:
    mutable std::mutex mutex_;  // Mutable for const methods
    int speakCount_;  // Example mutable state

public:
    // Thread-safety: Thread-safe (constructor initializes state)
    // Ownership: Initializes speakCount_ to 0
    // Invariants: None
    // Failure modes: None
    ThreadSafeSpeaker() : speakCount_(0) {}

    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void Speak() override {
        std::lock_guard<std::mutex> lock(mutex_);
        ++speakCount_;
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "Speaking... (count: " << speakCount_ << ")" << std::endl;
    }

    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: Returns copy of speakCount_
    // Invariants: None
    // Failure modes: None
    int getSpeakCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return speakCount_;
    }
};

void accessPersonThread(const ThreadSafePerson& person, int threadId) {
    for (int i = 0; i < 3; ++i) {
        person.Introduce();
        std::string name = person.getName();
        int age = person.getAge();
        std::cout << "[Thread " << threadId << "] Read: " << name << ", " << age << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void modifyPersonThread(ThreadSafePerson& person, int threadId) {
    person.setName("Updated_" + std::to_string(threadId));
    person.setAge(25 + threadId);
}

void speakThread(ThreadSafeISpeaker& speaker, int threadId) {
    for (int i = 0; i < 3; ++i) {
        speaker.Speak();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    ThreadSafePerson person("John Doe", 30);

    // Reader threads
    std::vector<std::thread> readerThreads;
    for (int i = 0; i < 2; ++i) {
        readerThreads.emplace_back(accessPersonThread, std::cref(person), i);
    }

    // Writer thread
    std::thread writerThread(modifyPersonThread, std::ref(person), 99);

    // Wait for writer
    writerThread.join();

    // Wait for readers
    for (auto& t : readerThreads) {
        t.join();
    }

    // Test speaker
    ThreadSafeSpeaker speaker;
    std::vector<std::thread> speakerThreads;
    for (int i = 0; i < 3; ++i) {
        speakerThreads.emplace_back(speakThread, std::ref(speaker), i);
    }

    for (auto& t : speakerThreads) {
        t.join();
    }

    std::cout << "Total speak count: " << speaker.getSpeakCount() << std::endl;

    return 0;
}

