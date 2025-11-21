/*
 * Object-Oriented Programming: Thread-Safe Employee and Student
 *
 * Demonstrates thread-safe implementation of inheritance hierarchy with
 * proper synchronization for concurrent access.
 */
#include <iostream>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <cassert>

// Thread-safety: Thread-safe (abstract interface)
// Ownership: Abstract base class, does not own derived objects
// Invariants: None
// Failure modes: None
class ThreadSafeAbstractEmployee {
public:
    virtual ~ThreadSafeAbstractEmployee() = default;

    // Thread-safety: Thread-safe (must be implemented thread-safely)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    virtual void askForPermission() = 0;
};

// Thread-safety: Thread-safe (uses shared_mutex for read-write operations)
// Ownership: Owns string members
// Invariants: age_ > 0, rollNo_ > 0
// Failure modes: Undefined behavior if invariants violated
class ThreadSafeStudent : public ThreadSafeAbstractEmployee {
private:
    mutable std::shared_mutex mutex_;  // Mutable for const methods
    std::string name_;
    std::string address_;
    int rollNo_;
    std::string dept_;
    int age_;

public:
    // Thread-safety: Thread-safe (constructor initializes members)
    // Ownership: Takes ownership of string parameters (copies)
    // Invariants: age > 0, rollNo > 0
    // Failure modes: Undefined behavior if age <= 0 or rollNo <= 0
    ThreadSafeStudent(const std::string& name, const std::string& address, int rollNo,
                      const std::string& dept, int age)
        : name_(name), address_(address), rollNo_(rollNo), dept_(dept), age_(age) {
        assert(age > 0 && "Age must be positive");
        assert(rollNo > 0 && "Roll number must be positive");
    }

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void askForPermission() override {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        if (age_ > 30) {
            std::cout << "[Thread " << std::this_thread::get_id() << "] "
                      << "Getting promoted" << std::endl;
        } else {
            std::cout << "[Thread " << std::this_thread::get_id() << "] "
                      << "Negotiating" << std::endl;
        }
    }

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void introduceYourself() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << "Hello, My name is " << name_ << std::endl;
    }

    // Thread-safety: Thread-safe (locks mutex exclusively)
    // Ownership: Takes ownership of name parameter (copies)
    // Invariants: name must be non-empty
    // Failure modes: Undefined behavior if name is empty
    void setName(const std::string& name) {
        assert(!name.empty() && "Name must be non-empty");
        std::unique_lock<std::shared_mutex> lock(mutex_);
        name_ = name;
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
};

// Thread-safety: Thread-safe (uses shared_mutex inherited from Student)
// Ownership: Owns string members
// Invariants: Inherits Student invariants
// Failure modes: Inherits Student failure modes
class ThreadSafeDeveloper : public ThreadSafeStudent {
private:
    mutable std::mutex devMutex_;  // Additional mutex for developer-specific state
    std::string favoriteProgrammingLang_;

public:
    // Thread-safety: Thread-safe (constructor initializes members)
    // Ownership: Takes ownership of string parameters (copies)
    // Invariants: Inherits Student invariants
    // Failure modes: Inherits Student failure modes
    ThreadSafeDeveloper(const std::string& name, const std::string& address, int rollNo,
                        const std::string& dept, const std::string& favoriteProgrammingLang, int age)
        : ThreadSafeStudent(name, address, rollNo, dept, age),
          favoriteProgrammingLang_(favoriteProgrammingLang) {}

    // Thread-safety: Thread-safe (locks mutex)
    // Ownership: None
    // Invariants: None
    // Failure modes: None
    void fixBug() const {
        std::lock_guard<std::mutex> lock(devMutex_);
        std::cout << "[Thread " << std::this_thread::get_id() << "] "
                  << getName() << " fixed the bug using " << favoriteProgrammingLang_ << std::endl;
    }

    // Thread-safety: Thread-safe (locks mutex for shared read)
    // Ownership: Returns copy of language
    // Invariants: None
    // Failure modes: None
    std::string getFavoriteLanguage() const {
        std::lock_guard<std::mutex> lock(devMutex_);
        return favoriteProgrammingLang_;
    }

    // Thread-safety: Thread-safe (locks mutex exclusively)
    // Ownership: Takes ownership of lang parameter (copies)
    // Invariants: lang must be non-empty
    // Failure modes: Undefined behavior if lang is empty
    void setFavoriteLanguage(const std::string& lang) {
        assert(!lang.empty() && "Language must be non-empty");
        std::lock_guard<std::mutex> lock(devMutex_);
        favoriteProgrammingLang_ = lang;
    }
};

void accessStudentThread(const ThreadSafeStudent& student, int threadId) {
    for (int i = 0; i < 3; ++i) {
        student.introduceYourself();
        student.askForPermission();
        std::string name = student.getName();
        int age = student.getAge();
        std::cout << "[Thread " << threadId << "] Read: " << name << ", " << age << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void modifyStudentThread(ThreadSafeStudent& student, int threadId) {
    student.setName("Updated_" + std::to_string(threadId));
}

void developerOperationsThread(ThreadSafeDeveloper& developer, int threadId) {
    for (int i = 0; i < 3; ++i) {
        developer.fixBug();
        std::string lang = developer.getFavoriteLanguage();
        std::cout << "[Thread " << threadId << "] Language: " << lang << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main() {
    ThreadSafeStudent student("John", "Boston", 30, "Wrestling", 29);

    // Reader threads
    std::vector<std::thread> readerThreads;
    for (int i = 0; i < 2; ++i) {
        readerThreads.emplace_back(accessStudentThread, std::cref(student), i);
    }

    // Writer thread
    std::thread writerThread(modifyStudentThread, std::ref(student), 99);

    // Wait for writer
    writerThread.join();

    // Wait for readers
    for (auto& t : readerThreads) {
        t.join();
    }

    // Test developer
    ThreadSafeDeveloper developer("Johnson", "UK", 40, "Engineering", "C++", 35);

    std::vector<std::thread> devThreads;
    for (int i = 0; i < 2; ++i) {
        devThreads.emplace_back(developerOperationsThread, std::ref(developer), i);
    }

    for (auto& t : devThreads) {
        t.join();
    }

    return 0;
}

