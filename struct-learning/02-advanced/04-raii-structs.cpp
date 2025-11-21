/*
 * =============================================================================
 * Advanced Struct Techniques: RAII with Structs - Advanced Resource Management
 * Production-Grade RAII Patterns for Top-Tier Companies
 * =============================================================================
 *
 * This file demonstrates advanced RAII techniques including:
 * - File handle management
 * - Memory buffer management
 * - Lock guards and mutexes
 * - Network socket management
 * - Database connection management
 * - Custom deleter patterns
 * - Exception safety guarantees
 *
 * Author: System Engineering Team
 * Version: 2.0
 * Last Modified: 2024-01-15
 *
 * =============================================================================
 */

#include <iostream>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <memory>
#include <mutex>
#include <fstream>
#include <vector>
#include <functional>

// =============================================================================
// FILE HANDLE RAII (GOOGLE-STYLE)
// =============================================================================

struct FileHandle {
    FILE* fp;
    const char* path;
    
    explicit FileHandle(const char* path, const char* mode) 
        : fp(std::fopen(path, mode)), path(path) {
        if (!fp) {
            throw std::runtime_error(std::string("Failed to open file: ") + path);
        }
    }
    
    ~FileHandle() {
        if (fp) {
            std::fclose(fp);
        }
    }
    
    // Non-copyable
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
    
    // Movable
    FileHandle(FileHandle&& other) noexcept 
        : fp(other.fp), path(other.path) {
        other.fp = nullptr;
        other.path = nullptr;
    }
    
    FileHandle& operator=(FileHandle&& other) noexcept {
        if (this != &other) {
            if (fp) std::fclose(fp);
            fp = other.fp;
            path = other.path;
            other.fp = nullptr;
            other.path = nullptr;
        }
        return *this;
    }
    
    FILE* get() const { return fp; }
    bool valid() const { return fp != nullptr; }
    
    size_t write(const void* data, size_t size, size_t count) {
        if (!fp) throw std::runtime_error("File handle is invalid");
        return std::fwrite(data, size, count, fp);
    }
    
    size_t read(void* data, size_t size, size_t count) {
        if (!fp) throw std::runtime_error("File handle is invalid");
        return std::fread(data, size, count, fp);
    }
};

// =============================================================================
// MEMORY BUFFER RAII (UBER-STYLE)
// =============================================================================

struct Buffer {
    char* data;
    size_t size;
    
    explicit Buffer(size_t n) 
        : data(static_cast<char*>(std::malloc(n))), size(n) {
        if (!data) {
            throw std::bad_alloc();
        }
        std::memset(data, 0, n);
    }
    
    Buffer(size_t n, char fill_value) 
        : data(static_cast<char*>(std::malloc(n))), size(n) {
        if (!data) {
            throw std::bad_alloc();
        }
        std::memset(data, fill_value, n);
    }
    
    ~Buffer() {
        std::free(data);
    }
    
    // Non-copyable
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    
    // Movable
    Buffer(Buffer&& other) noexcept 
        : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
    }
    
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            std::free(data);
            data = other.data;
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
    
    char* get() { return data; }
    const char* get() const { return data; }
    size_t get_size() const { return size; }
    
    void resize(size_t new_size) {
        char* new_data = static_cast<char*>(std::realloc(data, new_size));
        if (!new_data) {
            throw std::bad_alloc();
        }
        data = new_data;
        size = new_size;
    }
};

// =============================================================================
// LOCK GUARD RAII (BLOOMBERG-STYLE)
// =============================================================================

class MutexGuard {
private:
    std::mutex* mutex_;
    bool locked_;
    
public:
    explicit MutexGuard(std::mutex& m) 
        : mutex_(&m), locked_(true) {
        mutex_->lock();
    }
    
    ~MutexGuard() {
        if (locked_ && mutex_) {
            mutex_->unlock();
        }
    }
    
    // Non-copyable
    MutexGuard(const MutexGuard&) = delete;
    MutexGuard& operator=(const MutexGuard&) = delete;
    
    // Movable
    MutexGuard(MutexGuard&& other) noexcept 
        : mutex_(other.mutex_), locked_(other.locked_) {
        other.mutex_ = nullptr;
        other.locked_ = false;
    }
    
    void unlock() {
        if (locked_ && mutex_) {
            mutex_->unlock();
            locked_ = false;
        }
    }
    
    bool is_locked() const { return locked_; }
};

// =============================================================================
// NETWORK SOCKET RAII (AMAZON-STYLE)
// =============================================================================

class SocketHandle {
private:
    int fd_;
    bool connected_;
    
public:
    explicit SocketHandle(int fd) 
        : fd_(fd), connected_(fd >= 0) {}
    
    ~SocketHandle() {
        if (fd_ >= 0) {
            // In production: use proper close() system call
            // close(fd_);
            fd_ = -1;
            connected_ = false;
        }
    }
    
    // Non-copyable
    SocketHandle(const SocketHandle&) = delete;
    SocketHandle& operator=(const SocketHandle&) = delete;
    
    // Movable
    SocketHandle(SocketHandle&& other) noexcept 
        : fd_(other.fd_), connected_(other.connected_) {
        other.fd_ = -1;
        other.connected_ = false;
    }
    
    SocketHandle& operator=(SocketHandle&& other) noexcept {
        if (this != &other) {
            if (fd_ >= 0) {
                // close(fd_);
            }
            fd_ = other.fd_;
            connected_ = other.connected_;
            other.fd_ = -1;
            other.connected_ = false;
        }
        return *this;
    }
    
    int get() const { return fd_; }
    bool is_connected() const { return connected_ && fd_ >= 0; }
    
    void disconnect() {
        if (fd_ >= 0) {
            // close(fd_);
            fd_ = -1;
            connected_ = false;
        }
    }
};

// =============================================================================
// DATABASE CONNECTION RAII (PAYPAL-STYLE)
// =============================================================================

class DatabaseConnection {
private:
    void* connection_;  // Opaque pointer (would be actual DB connection)
    bool active_;
    
public:
    explicit DatabaseConnection(const char* connection_string) 
        : connection_(nullptr), active_(false) {
        // In production: initialize actual database connection
        // connection_ = db_connect(connection_string);
        connection_ = reinterpret_cast<void*>(0x12345678);  // Dummy
        active_ = true;
    }
    
    ~DatabaseConnection() {
        if (connection_ && active_) {
            // In production: db_disconnect(connection_);
            connection_ = nullptr;
            active_ = false;
        }
    }
    
    // Non-copyable
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;
    
    // Movable
    DatabaseConnection(DatabaseConnection&& other) noexcept 
        : connection_(other.connection_), active_(other.active_) {
        other.connection_ = nullptr;
        other.active_ = false;
    }
    
    DatabaseConnection& operator=(DatabaseConnection&& other) noexcept {
        if (this != &other) {
            if (connection_ && active_) {
                // db_disconnect(connection_);
            }
            connection_ = other.connection_;
            active_ = other.active_;
            other.connection_ = nullptr;
            other.active_ = false;
        }
        return *this;
    }
    
    void* get() const { return connection_; }
    bool is_active() const { return active_ && connection_ != nullptr; }
    
    void close() {
        if (connection_ && active_) {
            // db_disconnect(connection_);
            connection_ = nullptr;
            active_ = false;
        }
    }
};

// =============================================================================
// CUSTOM DELETER PATTERN (STRIPE-STYLE)
// =============================================================================

template<typename T, typename Deleter = std::default_delete<T>>
class UniqueResource {
private:
    T* resource_;
    Deleter deleter_;
    
public:
    explicit UniqueResource(T* resource, Deleter deleter = Deleter{}) 
        : resource_(resource), deleter_(deleter) {}
    
    ~UniqueResource() {
        if (resource_) {
            deleter_(resource_);
        }
    }
    
    // Non-copyable
    UniqueResource(const UniqueResource&) = delete;
    UniqueResource& operator=(const UniqueResource&) = delete;
    
    // Movable
    UniqueResource(UniqueResource&& other) noexcept 
        : resource_(other.resource_), deleter_(std::move(other.deleter_)) {
        other.resource_ = nullptr;
    }
    
    UniqueResource& operator=(UniqueResource&& other) noexcept {
        if (this != &other) {
            if (resource_) {
                deleter_(resource_);
            }
            resource_ = other.resource_;
            deleter_ = std::move(other.deleter_);
            other.resource_ = nullptr;
        }
        return *this;
    }
    
    T* get() const { return resource_; }
    T* release() {
        T* temp = resource_;
        resource_ = nullptr;
        return temp;
    }
    
    void reset(T* new_resource = nullptr) {
        if (resource_) {
            deleter_(resource_);
        }
        resource_ = new_resource;
    }
};

// Custom deleter for FILE*
struct FileDeleter {
    void operator()(FILE* fp) const {
        if (fp) {
            std::fclose(fp);
        }
    }
};

// =============================================================================
// EXCEPTION-SAFE RESOURCE POOL (GOD-MODDED)
// =============================================================================

template<typename Resource>
class ResourcePool {
private:
    std::vector<std::unique_ptr<Resource>> pool_;
    std::mutex mutex_;
    size_t max_size_;
    
public:
    explicit ResourcePool(size_t max_size = 10) 
        : max_size_(max_size) {}
    
    std::unique_ptr<Resource> acquire() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!pool_.empty()) {
            auto resource = std::move(pool_.back());
            pool_.pop_back();
            return resource;
        }
        
        // Create new resource if pool is empty
        return std::make_unique<Resource>();
    }
    
    void release(std::unique_ptr<Resource> resource) {
        if (!resource) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (pool_.size() < max_size_) {
            pool_.push_back(std::move(resource));
        }
        // Otherwise, resource is destroyed automatically
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return pool_.size();
    }
};

// =============================================================================
// DEMONSTRATION FUNCTIONS
// =============================================================================

void demonstrate_file_handle() {
    std::cout << "\n=== FILE HANDLE RAII ===" << std::endl;
    
    const char* path = "./_raii_demo.tmp";
    
    {
        FileHandle fh(path, "wb");
        Buffer buf(64);
        std::strcpy(buf.get(), "hello raii");
        fh.write(buf.get(), 1, std::strlen(buf.get()));
        std::cout << "File written successfully" << std::endl;
    }  // Resources released automatically
    
    {
        FileHandle fh(path, "rb");
        Buffer buf(64);
        fh.read(buf.get(), 1, 64);
        std::cout << "File read: " << buf.get() << std::endl;
    }
    
    std::remove(path);
}

void demonstrate_buffer_management() {
    std::cout << "\n=== MEMORY BUFFER RAII ===" << std::endl;
    
    Buffer buf1(1024);
    Buffer buf2(512, 0xFF);
    
    std::strcpy(buf1.get(), "Test buffer");
    std::cout << "Buffer 1: " << buf1.get() << std::endl;
    std::cout << "Buffer 1 size: " << buf1.get_size() << " bytes" << std::endl;
    
    buf1.resize(2048);
    std::cout << "After resize: " << buf1.get_size() << " bytes" << std::endl;
}

void demonstrate_lock_guard() {
    std::cout << "\n=== LOCK GUARD RAII ===" << std::endl;
    
    std::mutex mtx;
    
    {
        MutexGuard guard(mtx);
        std::cout << "Lock acquired: " << guard.is_locked() << std::endl;
        // Critical section
    }  // Lock released automatically
    
    std::cout << "Lock released automatically" << std::endl;
}

void demonstrate_socket_handle() {
    std::cout << "\n=== SOCKET HANDLE RAII ===" << std::endl;
    
    SocketHandle socket(42);  // Dummy file descriptor
    std::cout << "Socket created: " << socket.is_connected() << std::endl;
    std::cout << "Socket FD: " << socket.get() << std::endl;
    
    socket.disconnect();
    std::cout << "After disconnect: " << socket.is_connected() << std::endl;
}

void demonstrate_database_connection() {
    std::cout << "\n=== DATABASE CONNECTION RAII ===" << std::endl;
    
    DatabaseConnection db("postgresql://localhost/db");
    std::cout << "Database connected: " << db.is_active() << std::endl;
    
    db.close();
    std::cout << "After close: " << db.is_active() << std::endl;
}

void demonstrate_custom_deleter() {
    std::cout << "\n=== CUSTOM DELETER PATTERN ===" << std::endl;
    
    FILE* fp = std::fopen("./_deleter_demo.tmp", "w");
    UniqueResource<FILE, FileDeleter> file_resource(fp);
    
    std::cout << "File resource created" << std::endl;
    std::fprintf(file_resource.get(), "Test");
    
    // File closed automatically by custom deleter
    std::remove("./_deleter_demo.tmp");
}

void demonstrate_resource_pool() {
    std::cout << "\n=== RESOURCE POOL RAII ===" << std::endl;
    
    ResourcePool<Buffer> pool(5);
    
    auto buf1 = pool.acquire();
    auto buf2 = pool.acquire();
    
    std::cout << "Pool size after acquire: " << pool.size() << std::endl;
    
    pool.release(std::move(buf1));
    pool.release(std::move(buf2));
    
    std::cout << "Pool size after release: " << pool.size() << std::endl;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "=== GOD-MODDED ADVANCED RAII STRUCTS ===" << std::endl;
    std::cout << "Demonstrating production-grade RAII patterns" << std::endl;
    
    try {
        demonstrate_file_handle();
        demonstrate_buffer_management();
        demonstrate_lock_guard();
        demonstrate_socket_handle();
        demonstrate_database_connection();
        demonstrate_custom_deleter();
        demonstrate_resource_pool();
        
        std::cout << "\n=== RAII STRUCTS COMPLETED SUCCESSFULLY ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

// =============================================================================
// COMPILATION NOTES
// =============================================================================
/*
 * Compile with:
 *   g++ -std=c++17 -O2 -Wall -Wextra -pthread -o raii_structs 04-raii-structs.cpp
 *   clang++ -std=c++17 -O2 -Wall -Wextra -pthread -o raii_structs 04-raii-structs.cpp
 *
 * Advanced RAII techniques:
 *   - File handle management
 *   - Memory buffer management
 *   - Lock guards and mutexes
 *   - Network socket management
 *   - Database connection management
 *   - Custom deleter patterns
 *   - Resource pooling
 */
