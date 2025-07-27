# Network Performance Optimization Guide

## 🚀 Introduction

This guide covers **production-grade performance optimization techniques** for network applications. These are the same techniques used by companies like Google, Netflix, and Cloudflare to handle millions of requests per second.

## 📊 Performance Fundamentals

### Understanding Network Performance Metrics

```cpp
// Key metrics to monitor in production
struct NetworkMetrics {
    // Latency metrics (P50, P95, P99)
    LatencyHistogram request_latency;
    LatencyHistogram response_latency;
    LatencyHistogram connection_latency;
    
    // Throughput metrics
    std::atomic<uint64_t> requests_per_second{0};
    std::atomic<uint64_t> bytes_per_second{0};
    std::atomic<uint64_t> connections_per_second{0};
    
    // Error metrics
    std::atomic<uint64_t> connection_errors{0};
    std::atomic<uint64_t> timeout_errors{0};
    std::atomic<uint64_t> parse_errors{0};
    
    // Resource utilization
    std::atomic<size_t> active_connections{0};
    std::atomic<size_t> memory_usage_bytes{0};
    std::atomic<double> cpu_utilization{0.0};
};
```

**Critical Performance Insights:**

1. **Latency vs Throughput**: Often inversely related - optimize for your use case
2. **Memory bandwidth**: Often the bottleneck in high-performance systems
3. **CPU cache efficiency**: L1 cache miss costs ~4 cycles, L3 miss costs ~300 cycles
4. **Network round-trips**: Single round-trip can cost 1ms+ on internet

## 🔧 Memory Optimization Techniques

### 1. Zero-Copy I/O

Eliminate unnecessary memory copies between user space and kernel:

```cpp
// Traditional approach (2 copies)
class TraditionalSocket {
public:
    ssize_t send_file(int fd, const std::string& file_path) {
        // Copy 1: file -> user buffer
        std::ifstream file(file_path, std::ios::binary);
        std::vector<char> buffer(65536);
        file.read(buffer.data(), buffer.size());
        
        // Copy 2: user buffer -> kernel socket buffer
        return send(fd, buffer.data(), file.gcount(), 0);
    }
};

// Zero-copy approach (0 copies)
class ZeroCopySocket {
public:
    ssize_t send_file(int socket_fd, const std::string& file_path) {
        int file_fd = open(file_path.c_str(), O_RDONLY);
        if (file_fd < 0) return -1;
        
        struct stat file_stat;
        fstat(file_fd, &file_stat);
        
        // Direct kernel-to-kernel transfer
        off_t offset = 0;
        ssize_t result = sendfile(socket_fd, file_fd, &offset, file_stat.st_size);
        
        close(file_fd);
        return result;
    }
};

// Vectored I/O for multiple buffers
class ScatterGatherIO {
public:
    ssize_t send_multiple_buffers(int socket_fd, 
                                 const std::vector<std::span<const uint8_t>>& buffers) {
        std::vector<struct iovec> iov;
        iov.reserve(buffers.size());
        
        for (const auto& buffer : buffers) {
            iov.push_back({
                .iov_base = const_cast<uint8_t*>(buffer.data()),
                .iov_len = buffer.size()
            });
        }
        
        struct msghdr msg{};
        msg.msg_iov = iov.data();
        msg.msg_iovlen = iov.size();
        
        return sendmsg(socket_fd, &msg, MSG_NOSIGNAL);
    }
};
```

**Performance Impact**: Zero-copy can improve throughput by 2-5x for large file transfers.

### 2. Memory Pool Allocation

Reduce allocation overhead and improve cache locality:

```cpp
// High-performance memory pool
template<size_t BlockSize, size_t PoolSize>
class MemoryPool {
private:
    struct Block {
        alignas(std::max_align_t) uint8_t data[BlockSize];
        Block* next;
    };
    
    Block memory_[PoolSize];
    Block* free_list_;
    std::atomic<size_t> allocated_count_{0};
    std::mutex mutex_;

public:
    MemoryPool() {
        // Initialize free list
        free_list_ = &memory_[0];
        for (size_t i = 0; i < PoolSize - 1; ++i) {
            memory_[i].next = &memory_[i + 1];
        }
        memory_[PoolSize - 1].next = nullptr;
    }
    
    void* allocate() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!free_list_) {
            return nullptr;  // Pool exhausted
        }
        
        Block* block = free_list_;
        free_list_ = block->next;
        allocated_count_++;
        
        return block->data;
    }
    
    void deallocate(void* ptr) {
        if (!ptr) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        Block* block = reinterpret_cast<Block*>(
            static_cast<uint8_t*>(ptr) - offsetof(Block, data));
        
        block->next = free_list_;
        free_list_ = block;
        allocated_count_--;
    }
    
    size_t allocated_count() const { return allocated_count_; }
    bool is_exhausted() const { return free_list_ == nullptr; }
};

// Usage example for HTTP requests
class HttpRequestPool {
private:
    MemoryPool<sizeof(HttpRequest), 10000> pool_;
    
public:
    std::unique_ptr<HttpRequest> allocate_request() {
        void* memory = pool_.allocate();
        if (!memory) {
            throw std::bad_alloc();
        }
        
        return std::unique_ptr<HttpRequest>(
            new(memory) HttpRequest(),
            [this](HttpRequest* req) {
                req->~HttpRequest();
                pool_.deallocate(req);
            }
        );
    }
};
```

**Performance Impact**: Memory pools can reduce allocation latency by 10-100x.

### 3. Cache-Aligned Data Structures

Optimize for CPU cache line size (typically 64 bytes):

```cpp
// Cache-aligned connection structure
struct alignas(64) Connection {
    // Hot data (frequently accessed) - first cache line
    int socket_fd;
    ConnectionState state;
    uint64_t last_activity_ns;
    uint32_t bytes_received;
    uint32_t bytes_sent;
    
    // Padding to cache line boundary
    char padding1[64 - (sizeof(int) + sizeof(ConnectionState) + 
                       sizeof(uint64_t) + 2*sizeof(uint32_t))];
    
    // Cold data (less frequently accessed) - second cache line  
    std::string peer_address;
    std::chrono::steady_clock::time_point created_at;
    HttpRequestParser parser;
    
    char padding2[64]; // Ensure next object starts on cache line
};

// Lock-free data structures for high concurrency
class LockFreeQueue {
private:
    struct Node {
        std::atomic<void*> data{nullptr};
        std::atomic<Node*> next{nullptr};
    };
    
    alignas(64) std::atomic<Node*> head_;
    alignas(64) std::atomic<Node*> tail_;
    
public:
    void enqueue(void* item) {
        Node* new_node = new Node;
        new_node->data.store(item);
        
        Node* prev_tail = tail_.exchange(new_node);
        prev_tail->next.store(new_node);
    }
    
    void* dequeue() {
        Node* head = head_.load();
        Node* next = head->next.load();
        
        if (next == nullptr) {
            return nullptr;  // Empty queue
        }
        
        void* data = next->data.load();
        head_.store(next);
        delete head;
        
        return data;
    }
};
```

## ⚡ I/O Optimization Techniques

### 1. Event-Driven Architecture with epoll

Scale to thousands of concurrent connections:

```cpp
// Production-grade epoll event loop
class EpollEventLoop {
private:
    int epoll_fd_;
    std::unordered_map<int, std::unique_ptr<EventHandler>> handlers_;
    std::atomic<bool> running_{false};
    
    // Performance tuning
    static constexpr int MAX_EVENTS = 1024;
    static constexpr int EPOLL_TIMEOUT_MS = 1;  // Low latency
    
public:
    class EventHandler {
    public:
        virtual ~EventHandler() = default;
        virtual bool handle_read() = 0;
        virtual bool handle_write() = 0;
        virtual bool handle_error() = 0;
    };
    
    EpollEventLoop() {
        epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
        if (epoll_fd_ < 0) {
            throw std::system_error(errno, std::system_category(), "epoll_create1");
        }
    }
    
    void add_handler(int fd, std::unique_ptr<EventHandler> handler, uint32_t events) {
        struct epoll_event event{};
        event.events = events | EPOLLET;  // Edge-triggered for performance
        event.data.fd = fd;
        
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) < 0) {
            throw std::system_error(errno, std::system_category(), "epoll_ctl ADD");
        }
        
        handlers_[fd] = std::move(handler);
    }
    
    void run() {
        running_ = true;
        std::array<epoll_event, MAX_EVENTS> events;
        
        while (running_) {
            int event_count = epoll_wait(epoll_fd_, events.data(), MAX_EVENTS, EPOLL_TIMEOUT_MS);
            
            if (event_count < 0) {
                if (errno == EINTR) continue;
                throw std::system_error(errno, std::system_category(), "epoll_wait");
            }
            
            // Process events
            for (int i = 0; i < event_count; ++i) {
                handle_event(events[i]);
            }
            
            // Optional: yield to other threads if no events
            if (event_count == 0) {
                std::this_thread::yield();
            }
        }
    }

private:
    void handle_event(const epoll_event& event) {
        int fd = event.data.fd;
        auto it = handlers_.find(fd);
        if (it == handlers_.end()) return;
        
        auto& handler = it->second;
        bool keep_alive = true;
        
        // Handle events in priority order
        if (event.events & (EPOLLHUP | EPOLLERR)) {
            keep_alive = handler->handle_error();
        } else {
            if (event.events & EPOLLIN) {
                keep_alive = handler->handle_read();
            }
            if (keep_alive && (event.events & EPOLLOUT)) {
                keep_alive = handler->handle_write();
            }
        }
        
        if (!keep_alive) {
            remove_handler(fd);
        }
    }
    
    void remove_handler(int fd) {
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
        handlers_.erase(fd);
    }
};
```

**Performance Impact**: Event-driven I/O can handle 10K+ concurrent connections on a single thread.

### 2. Optimized Buffer Management

Minimize allocations and copies:

```cpp
// Ring buffer for efficient I/O
class RingBuffer {
private:
    std::vector<uint8_t> buffer_;
    size_t read_pos_ = 0;
    size_t write_pos_ = 0;
    size_t size_;
    
public:
    explicit RingBuffer(size_t size) : buffer_(size), size_(size) {}
    
    // Write data to buffer
    size_t write(const uint8_t* data, size_t length) {
        size_t available = available_write();
        size_t to_write = std::min(length, available);
        
        if (to_write == 0) return 0;
        
        // Handle wrap-around
        size_t first_chunk = std::min(to_write, size_ - write_pos_);
        std::memcpy(buffer_.data() + write_pos_, data, first_chunk);
        
        if (first_chunk < to_write) {
            std::memcpy(buffer_.data(), data + first_chunk, to_write - first_chunk);
        }
        
        write_pos_ = (write_pos_ + to_write) % size_;
        return to_write;
    }
    
    // Read data from buffer
    size_t read(uint8_t* data, size_t length) {
        size_t available = available_read();
        size_t to_read = std::min(length, available);
        
        if (to_read == 0) return 0;
        
        // Handle wrap-around
        size_t first_chunk = std::min(to_read, size_ - read_pos_);
        std::memcpy(data, buffer_.data() + read_pos_, first_chunk);
        
        if (first_chunk < to_read) {
            std::memcpy(data + first_chunk, buffer_.data(), to_read - first_chunk);
        }
        
        read_pos_ = (read_pos_ + to_read) % size_;
        return to_read;
    }
    
    // Zero-copy access to contiguous data
    std::span<const uint8_t> peek() const {
        if (read_pos_ <= write_pos_) {
            return std::span<const uint8_t>(buffer_.data() + read_pos_, 
                                          write_pos_ - read_pos_);
        } else {
            return std::span<const uint8_t>(buffer_.data() + read_pos_, 
                                          size_ - read_pos_);
        }
    }
    
    void consume(size_t bytes) {
        read_pos_ = (read_pos_ + bytes) % size_;
    }
    
    size_t available_read() const {
        return (write_pos_ + size_ - read_pos_) % size_;
    }
    
    size_t available_write() const {
        return (read_pos_ + size_ - write_pos_ - 1) % size_;
    }
};

// Chain of buffers for large messages
class BufferChain {
private:
    struct BufferNode {
        std::vector<uint8_t> data;
        size_t offset = 0;
        std::unique_ptr<BufferNode> next;
    };
    
    std::unique_ptr<BufferNode> head_;
    BufferNode* tail_ = nullptr;
    size_t total_size_ = 0;

public:
    void append(std::vector<uint8_t> data) {
        auto node = std::make_unique<BufferNode>();
        node->data = std::move(data);
        
        if (!head_) {
            head_ = std::move(node);
            tail_ = head_.get();
        } else {
            tail_->next = std::move(node);
            tail_ = tail_->next.get();
        }
        
        total_size_ += tail_->data.size();
    }
    
    size_t send_to_socket(int socket_fd) {
        size_t total_sent = 0;
        
        while (head_) {
            auto& current = *head_;
            size_t remaining = current.data.size() - current.offset;
            
            ssize_t sent = send(socket_fd, 
                              current.data.data() + current.offset,
                              remaining, MSG_NOSIGNAL);
            
            if (sent <= 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break;  // Would block
                }
                throw std::system_error(errno, std::system_category(), "send");
            }
            
            current.offset += sent;
            total_sent += sent;
            total_size_ -= sent;
            
            // Remove completed buffer
            if (current.offset >= current.data.size()) {
                head_ = std::move(current.next);
                if (!head_) {
                    tail_ = nullptr;
                }
            }
        }
        
        return total_sent;
    }
    
    bool empty() const { return !head_; }
    size_t size() const { return total_size_; }
};
```

## 🧵 Concurrency Optimization

### 1. Thread Pool with Work Stealing

Maximize CPU utilization:

```cpp
// High-performance thread pool with work stealing
class WorkStealingThreadPool {
private:
    struct alignas(64) WorkerThread {
        std::thread thread;
        std::deque<std::function<void()>> queue;
        std::mutex mutex;
        std::condition_variable condition;
        std::atomic<bool> stop{false};
        
        // Statistics
        std::atomic<uint64_t> tasks_executed{0};
        std::atomic<uint64_t> tasks_stolen{0};
    };
    
    std::vector<std::unique_ptr<WorkerThread>> workers_;
    std::atomic<size_t> next_worker_{0};
    
public:
    explicit WorkStealingThreadPool(size_t num_threads) {
        workers_.reserve(num_threads);
        
        for (size_t i = 0; i < num_threads; ++i) {
            auto worker = std::make_unique<WorkerThread>();
            
            worker->thread = std::thread([this, i, worker_ptr = worker.get()]() {
                worker_loop(i, worker_ptr);
            });
            
            workers_.push_back(std::move(worker));
        }
    }
    
    ~WorkStealingThreadPool() {
        // Signal all workers to stop
        for (auto& worker : workers_) {
            worker->stop = true;
            worker->condition.notify_one();
        }
        
        // Wait for all threads to finish
        for (auto& worker : workers_) {
            if (worker->thread.joinable()) {
                worker->thread.join();
            }
        }
    }
    
    template<typename F>
    void submit(F&& task) {
        // Round-robin task distribution
        size_t worker_index = next_worker_.fetch_add(1) % workers_.size();
        auto& worker = workers_[worker_index];
        
        {
            std::lock_guard<std::mutex> lock(worker->mutex);
            worker->queue.emplace_back(std::forward<F>(task));
        }
        
        worker->condition.notify_one();
    }

private:
    void worker_loop(size_t worker_id, WorkerThread* worker) {
        while (!worker->stop) {
            std::function<void()> task;
            
            // Try to get task from own queue
            {
                std::unique_lock<std::mutex> lock(worker->mutex);
                
                if (worker->queue.empty()) {
                    // Try to steal work from other workers
                    if (!steal_work(worker_id, task)) {
                        // No work available, wait
                        worker->condition.wait(lock, [worker]() {
                            return worker->stop || !worker->queue.empty();
                        });
                        
                        if (worker->stop) break;
                    }
                }
                
                if (!task && !worker->queue.empty()) {
                    task = std::move(worker->queue.front());
                    worker->queue.pop_front();
                }
            }
            
            if (task) {
                try {
                    task();
                    worker->tasks_executed++;
                } catch (const std::exception& e) {
                    // Log error and continue
                    utils::log_error("Task execution error: {}", e.what());
                }
            }
        }
    }
    
    bool steal_work(size_t thief_id, std::function<void()>& stolen_task) {
        // Try to steal from each other worker
        for (size_t i = 1; i < workers_.size(); ++i) {
            size_t victim_id = (thief_id + i) % workers_.size();
            auto& victim = workers_[victim_id];
            
            std::lock_guard<std::mutex> lock(victim->mutex);
            if (!victim->queue.empty()) {
                // Steal from back (LIFO for better cache locality)
                stolen_task = std::move(victim->queue.back());
                victim->queue.pop_back();
                
                workers_[thief_id]->tasks_stolen++;
                return true;
            }
        }
        
        return false;
    }
};
```

### 2. Lock-Free Data Structures

Eliminate lock contention:

```cpp
// Lock-free MPSC (Multiple Producer Single Consumer) queue
template<typename T>
class LockFreeMPSCQueue {
private:
    struct Node {
        std::atomic<T*> data{nullptr};
        std::atomic<Node*> next{nullptr};
    };
    
    alignas(64) std::atomic<Node*> head_;  // Consumer end
    alignas(64) std::atomic<Node*> tail_;  // Producer end

public:
    LockFreeMPSCQueue() {
        Node* dummy = new Node;
        head_.store(dummy);
        tail_.store(dummy);
    }
    
    ~LockFreeMPSCQueue() {
        while (Node* old_head = head_.load()) {
            head_.store(old_head->next);
            delete old_head;
        }
    }
    
    void enqueue(T item) {
        Node* new_node = new Node;
        T* data = new T(std::move(item));
        new_node->data.store(data);
        
        // Atomically update tail
        Node* prev_tail = tail_.exchange(new_node);
        prev_tail->next.store(new_node);
    }
    
    bool dequeue(T& result) {
        Node* head = head_.load();
        Node* next = head->next.load();
        
        if (next == nullptr) {
            return false;  // Queue is empty
        }
        
        T* data = next->data.load();
        if (data == nullptr) {
            return false;  // No data yet
        }
        
        result = *data;
        delete data;
        
        head_.store(next);
        delete head;
        
        return true;
    }
};
```

## 🌐 Protocol-Specific Optimizations

### 1. HTTP/1.1 Optimizations

```cpp
// Optimized HTTP header parsing
class FastHttpHeaderParser {
private:
    // Pre-computed hash table for common headers
    static constexpr std::array<uint64_t, 32> COMMON_HEADERS = {
        hash("content-length"), hash("content-type"), hash("connection"),
        hash("host"), hash("user-agent"), hash("accept"), /* ... */
    };
    
    // SIMD-optimized header parsing
    bool parse_header_line_fast(const char* line, size_t length,
                               std::string_view& name, std::string_view& value) {
        // Use SIMD instructions to find colon separator
        const char* colon = find_colon_simd(line, length);
        if (!colon) return false;
        
        size_t name_len = colon - line;
        name = std::string_view(line, name_len);
        
        // Skip colon and whitespace
        const char* value_start = colon + 1;
        while (value_start < line + length && *value_start == ' ') {
            value_start++;
        }
        
        value = std::string_view(value_start, line + length - value_start);
        return true;
    }
    
    const char* find_colon_simd(const char* data, size_t length) {
        // Use AVX2 instructions for fast colon search
        #ifdef __AVX2__
        const __m256i colon_vec = _mm256_set1_epi8(':');
        
        for (size_t i = 0; i + 32 <= length; i += 32) {
            __m256i chunk = _mm256_loadu_si256(
                reinterpret_cast<const __m256i*>(data + i));
            __m256i result = _mm256_cmpeq_epi8(chunk, colon_vec);
            
            uint32_t mask = _mm256_movemask_epi8(result);
            if (mask != 0) {
                return data + i + __builtin_ctz(mask);
            }
        }
        #endif
        
        // Fallback to scalar search
        return static_cast<const char*>(memchr(data, ':', length));
    }
    
    static constexpr uint64_t hash(const char* str) {
        // Compile-time string hashing
        uint64_t hash = 14695981039346656037ULL;
        while (*str) {
            hash ^= static_cast<uint8_t>(*str++);
            hash *= 1099511628211ULL;
        }
        return hash;
    }
};

// Connection pooling for HTTP clients
class HttpConnectionPool {
private:
    struct PooledConnection {
        Socket socket;
        std::chrono::steady_clock::time_point last_used;
        uint32_t request_count = 0;
    };
    
    std::unordered_map<std::string, std::queue<PooledConnection>> pools_;
    std::mutex mutex_;
    
    static constexpr auto MAX_IDLE_TIME = std::chrono::minutes(5);
    static constexpr uint32_t MAX_REQUESTS_PER_CONNECTION = 1000;

public:
    std::optional<Socket> get_connection(const std::string& host, uint16_t port) {
        std::string key = host + ":" + std::to_string(port);
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto& pool = pools_[key];
        
        // Remove stale connections
        while (!pool.empty()) {
            auto& conn = pool.front();
            auto age = std::chrono::steady_clock::now() - conn.last_used;
            
            if (age > MAX_IDLE_TIME || conn.request_count >= MAX_REQUESTS_PER_CONNECTION) {
                pool.pop();
            } else {
                // Reuse connection
                Socket socket = std::move(conn.socket);
                pool.pop();
                return socket;
            }
        }
        
        return std::nullopt;  // No reusable connection
    }
    
    void return_connection(const std::string& host, uint16_t port, 
                          Socket socket, uint32_t request_count) {
        std::string key = host + ":" + std::to_string(port);
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto& pool = pools_[key];
        
        pool.push({
            .socket = std::move(socket),
            .last_used = std::chrono::steady_clock::now(),
            .request_count = request_count
        });
    }
};
```

### 2. WebSocket Optimizations

```cpp
// Optimized WebSocket frame parsing
class FastWebSocketFrameParser {
private:
    // Pre-allocated buffers to avoid allocations
    std::vector<uint8_t> frame_buffer_;
    std::vector<uint8_t> payload_buffer_;
    
public:
    ParseResult parse_frame_optimized(const uint8_t* data, size_t length) {
        if (length < 2) {
            return ParseError::NEED_MORE_DATA;
        }
        
        // Fast path for small text frames (most common)
        if (data[0] == 0x81 && (data[1] & 0x80) && (data[1] & 0x7F) < 126) {
            return parse_small_masked_text_frame(data, length);
        }
        
        // General parsing for other frame types
        return parse_frame_general(data, length);
    }

private:
    ParseResult parse_small_masked_text_frame(const uint8_t* data, size_t length) {
        uint8_t payload_len = data[1] & 0x7F;
        
        if (length < 6 + payload_len) {
            return ParseError::NEED_MORE_DATA;
        }
        
        // Extract masking key
        uint32_t mask = *reinterpret_cast<const uint32_t*>(data + 2);
        
        // Unmask payload efficiently
        const uint8_t* masked_payload = data + 6;
        
        if (payload_buffer_.size() < payload_len) {
            payload_buffer_.resize(payload_len);
        }
        
        // SIMD unmasking for better performance
        unmask_payload_simd(masked_payload, payload_buffer_.data(), payload_len, mask);
        
        WebSocketFrame frame{
            .fin = true,
            .opcode = WebSocketFrame::Opcode::TEXT,
            .masked = true,
            .payload = std::vector<uint8_t>(payload_buffer_.begin(), 
                                          payload_buffer_.begin() + payload_len)
        };
        
        return std::make_pair(std::move(frame), 6 + payload_len);
    }
    
    void unmask_payload_simd(const uint8_t* src, uint8_t* dst, 
                           size_t length, uint32_t mask) {
        #ifdef __AVX2__
        // Broadcast mask to all lanes
        __m256i mask_vec = _mm256_set1_epi32(mask);
        
        size_t simd_length = length & ~31;  // Round down to 32-byte boundary
        
        for (size_t i = 0; i < simd_length; i += 32) {
            __m256i data_vec = _mm256_loadu_si256(
                reinterpret_cast<const __m256i*>(src + i));
            __m256i result = _mm256_xor_si256(data_vec, mask_vec);
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst + i), result);
        }
        
        // Handle remaining bytes
        for (size_t i = simd_length; i < length; ++i) {
            dst[i] = src[i] ^ (mask >> ((i % 4) * 8));
        }
        #else
        // Fallback scalar implementation
        for (size_t i = 0; i < length; ++i) {
            dst[i] = src[i] ^ (mask >> ((i % 4) * 8));
        }
        #endif
    }
};

// WebSocket message compression (RFC 7692)
class WebSocketDeflateExtension {
private:
    z_stream deflate_stream_{};
    z_stream inflate_stream_{};
    bool initialized_ = false;
    
public:
    bool initialize() {
        if (initialized_) return true;
        
        // Initialize deflate stream
        deflate_stream_.zalloc = Z_NULL;
        deflate_stream_.zfree = Z_NULL;
        deflate_stream_.opaque = Z_NULL;
        
        if (deflateInit2(&deflate_stream_, Z_DEFAULT_COMPRESSION,
                        Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
            return false;
        }
        
        // Initialize inflate stream
        inflate_stream_.zalloc = Z_NULL;
        inflate_stream_.zfree = Z_NULL;
        inflate_stream_.opaque = Z_NULL;
        
        if (inflateInit2(&inflate_stream_, -15) != Z_OK) {
            deflateEnd(&deflate_stream_);
            return false;
        }
        
        initialized_ = true;
        return true;
    }
    
    std::vector<uint8_t> compress(const std::vector<uint8_t>& data) {
        if (!initialized_) return data;
        
        std::vector<uint8_t> compressed;
        compressed.resize(data.size() + 16);  // Overhead estimate
        
        deflate_stream_.next_in = const_cast<uint8_t*>(data.data());
        deflate_stream_.avail_in = data.size();
        deflate_stream_.next_out = compressed.data();
        deflate_stream_.avail_out = compressed.size();
        
        int result = deflate(&deflate_stream_, Z_SYNC_FLUSH);
        if (result != Z_OK) {
            return data;  // Compression failed, return original
        }
        
        compressed.resize(compressed.size() - deflate_stream_.avail_out);
        
        // Remove trailing 00 00 FF FF for per-message deflate
        if (compressed.size() >= 4) {
            compressed.resize(compressed.size() - 4);
        }
        
        return compressed;
    }
};
```

## 📊 Performance Monitoring

```cpp
// Real-time performance metrics
class PerformanceMonitor {
private:
    struct Metrics {
        std::atomic<uint64_t> requests_processed{0};
        std::atomic<uint64_t> bytes_transferred{0};
        std::atomic<uint64_t> connections_established{0};
        std::atomic<uint64_t> errors_encountered{0};
        
        LatencyHistogram request_latency;
        LatencyHistogram connection_latency;
        
        std::chrono::steady_clock::time_point start_time;
    } metrics_;
    
    std::thread monitoring_thread_;
    std::atomic<bool> running_{false};

public:
    PerformanceMonitor() {
        metrics_.start_time = std::chrono::steady_clock::now();
        
        running_ = true;
        monitoring_thread_ = std::thread([this]() {
            monitoring_loop();
        });
    }
    
    ~PerformanceMonitor() {
        running_ = false;
        if (monitoring_thread_.joinable()) {
            monitoring_thread_.join();
        }
    }
    
    void record_request(std::chrono::nanoseconds latency, size_t bytes) {
        metrics_.requests_processed++;
        metrics_.bytes_transferred += bytes;
        metrics_.request_latency.record(latency);
    }
    
    void record_connection(std::chrono::nanoseconds setup_time) {
        metrics_.connections_established++;
        metrics_.connection_latency.record(setup_time);
    }
    
    void record_error() {
        metrics_.errors_encountered++;
    }

private:
    void monitoring_loop() {
        auto last_report = std::chrono::steady_clock::now();
        uint64_t last_requests = 0;
        uint64_t last_bytes = 0;
        
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                now - last_report).count();
            
            uint64_t current_requests = metrics_.requests_processed.load();
            uint64_t current_bytes = metrics_.bytes_transferred.load();
            
            double rps = static_cast<double>(current_requests - last_requests) / duration;
            double mbps = static_cast<double>(current_bytes - last_bytes) * 8.0 / 
                         (duration * 1024 * 1024);
            
            utils::log_info("Performance: {:.2f} req/s, {:.2f} Mbps, "
                          "P50 latency: {:.2f}ms, P99 latency: {:.2f}ms",
                          rps, mbps,
                          metrics_.request_latency.percentile(0.5) / 1e6,
                          metrics_.request_latency.percentile(0.99) / 1e6);
            
            last_report = now;
            last_requests = current_requests;
            last_bytes = current_bytes;
        }
    }
};
```

## 🎯 Real-World Performance Targets

### Production Performance Benchmarks

Based on industry standards:

| Metric | Good | Great | Exceptional |
|--------|------|-------|-------------|
| **HTTP Latency (P99)** | <100ms | <10ms | <1ms |
| **WebSocket Latency** | <50ms | <5ms | <1ms |
| **Throughput** | 1K RPS | 10K RPS | 100K+ RPS |
| **Concurrent Connections** | 1K | 10K | 100K+ |
| **Memory per Connection** | <10KB | <1KB | <500B |
| **CPU per Connection** | <1% | <0.1% | <0.01% |

### Optimization Checklist

✅ **Memory Optimizations**
- [ ] Zero-copy I/O where possible
- [ ] Memory pools for frequent allocations
- [ ] Cache-aligned data structures
- [ ] SIMD optimizations for data processing

✅ **I/O Optimizations**
- [ ] Event-driven architecture (epoll/kqueue)
- [ ] Non-blocking sockets
- [ ] Vectored I/O for multiple buffers
- [ ] Connection pooling and reuse

✅ **Concurrency Optimizations**
- [ ] Lock-free data structures
- [ ] Work-stealing thread pool
- [ ] Atomic operations instead of locks
- [ ] Thread-local storage for hot paths

✅ **Protocol Optimizations**
- [ ] HTTP keep-alive connections
- [ ] WebSocket compression
- [ ] Efficient header parsing
- [ ] Binary protocol where applicable

✅ **Monitoring & Profiling**
- [ ] Real-time performance metrics
- [ ] Latency histograms (P50, P95, P99)
- [ ] Memory usage tracking
- [ ] CPU profiling with flamegraphs

## 🏆 Advanced Techniques

### Kernel Bypass Networking

For extreme performance requirements:

```cpp
// DPDK-style user-space networking (conceptual)
class UserSpaceNetworking {
private:
    // Memory-mapped network buffers
    void* rx_ring_buffer_;
    void* tx_ring_buffer_;
    
public:
    // Bypass kernel for packet processing
    bool poll_packets() {
        // Direct access to NIC ring buffers
        // Process packets in user space
        // Avoid syscall overhead
    }
};
```

### CPU Affinity and NUMA Optimization

```cpp
// Pin threads to specific CPU cores
void optimize_cpu_affinity() {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);  // Pin to CPU 0
    
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

// NUMA-aware memory allocation
void* allocate_numa_local(size_t size) {
    int node = numa_node_of_cpu(sched_getcpu());
    return numa_alloc_onnode(size, node);
}
```

These optimizations represent the cutting edge of network performance engineering, as used in the most demanding production environments.

---

*Next: [Complete Implementation Guide](../examples/)*
