# Event Loop Pattern Recognition Guide

## 🎯 **Decision Tree for Event Loop Selection**

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        EVENT LOOP DECISION TREE                          │
│                      "Choose Your Event Loop Weapon"                     │
└─────────────────────────────────────────────────────────────────────────┘

1. What is your primary event source?
   ├─── User Input (Mouse/Keyboard) ──► GUI Event Loop (Qt/GTK/WPF)
   ├─── Terminal Input ───────────────► TUI Event Loop (ncurses/curses)
   ├─── Network Sockets ──────────────► Network Event Loop (epoll/kqueue)
   ├─── Timers/Timeouts ─────────────► Timer Event Loop (boost::asio)
   ├─── Async Tasks/Concurrency ─────► Async Event Loop (libdispatch)
   └─── Message Passing ─────────────► Message Queue Loop (actor model)

2. What performance requirements?
   ├─── Low Latency (<1ms) ──────────► Network Event Loop (epoll)
   ├─── High Throughput ────────────► Async Event Loop (coroutines)
   ├─── Deterministic Timing ───────► Timer Event Loop (priority queue)
   └─── Real-time Constraints ──────► Game Loop (fixed timestep)

3. What concurrency model?
   ├─── Single Threaded ────────────► Simple Event Loop (select/poll)
   ├─── Multi-threaded ─────────────► Thread Pool + Event Loop
   ├─── Actor-based ────────────────► Message Queue Event Loop
   └─── Coroutine-based ───────────► Async Event Loop

4. What platform constraints?
   ├─── Linux ─────────────────────► epoll-based Network Loop
   ├─── macOS ─────────────────────► kqueue-based Network Loop
   ├─── Windows ───────────────────► IOCP-based Network Loop
   ├─── Cross-platform ───────────► libevent/boost::asio
   └─── Embedded ─────────────────► Polling Event Loop

5. What memory/CPU constraints?
   ├─── Low Memory ───────────────► Callback-based Event Loop
   ├─── Low CPU ──────────────────► Polling Event Loop
   ├─── High Performance ─────────► Completion-based Event Loop
   └─── Real-time Critical ───────► Preemptive Event Loop
```

## 📊 **Performance Characteristics**

| Event Loop Type | Best For | Time Complexity | Space Complexity | Latency | Throughput |
|----------------|----------|-----------------|------------------|---------|------------|
| **GUI Event Loop** | Desktop Apps | O(1) dispatch | O(n) events | ~10-50ms | 1000-5000 EPS |
| **TUI Event Loop** | Terminal Apps | O(1) dispatch | O(1) state | ~1-10ms | 100-1000 EPS |
| **Network Event Loop** | Servers | O(log n) | O(n) sockets | <1ms | 10k-1M EPS |
| **Async Event Loop** | Concurrent Apps | O(1) schedule | O(n) tasks | ~1ms | 100k+ TPS |
| **Timer Event Loop** | Scheduled Tasks | O(log n) | O(n) timers | <1ms | 10k-100k TPS |
| **Message Queue Loop** | Distributed Systems | O(1) enqueue | O(n) messages | ~1-5ms | 50k-500k MPS |

**EPS**: Events Per Second, **TPS**: Tasks Per Second, **MPS**: Messages Per Second

## 🎨 **Pattern Variants by Domain**

### **GUI Applications** 🖥️
```cpp
// Qt-style GUI Event Loop
class GUIEventLoop {
    std::queue<Event> event_queue_;
    std::vector<Widget*> widgets_;

    void run() {
        while (running_) {
            // Process system events (mouse, keyboard, paint)
            process_system_events();

            // Update UI state
            update_widgets();

            // Render if needed
            if (needs_redraw_) render();

            // Sleep to prevent 100% CPU
            std::this_thread::sleep_for(16ms); // ~60 FPS
        }
    }
};
```

### **Terminal Applications** 🖥️
```cpp
// ncurses-style TUI Event Loop
class TUIEventLoop {
    std::unordered_map<int, std::function<void()>> key_handlers_;

    void run() {
        while (running_) {
            // Non-blocking input check
            int ch = getch();
            if (ch != ERR) {
                auto handler = key_handlers_.find(ch);
                if (handler != key_handlers_.end()) {
                    handler->second();
                }
            }

            // Update display
            refresh();

            // Small delay to prevent busy waiting
            std::this_thread::sleep_for(10ms);
        }
    }
};
```

### **Network Servers** 🌐
```cpp
// epoll-based Network Event Loop
class NetworkEventLoop {
    int epoll_fd_;
    std::unordered_map<int, Connection*> connections_;

    void run() {
        while (running_) {
            struct epoll_event events[MAX_EVENTS];

            int num_events = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);

            for (int i = 0; i < num_events; ++i) {
                Connection* conn = connections_[events[i].data.fd];

                if (events[i].events & EPOLLIN) {
                    conn->handle_read();
                }
                if (events[i].events & EPOLLOUT) {
                    conn->handle_write();
                }
            }
        }
    }
};
```

### **Async Programming** ⚡
```cpp
// libdispatch/GCD-style Async Event Loop
class AsyncEventLoop {
    std::queue<std::function<void()>> task_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;

    void run() {
        while (running_) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                queue_cv_.wait(lock, [this] {
                    return !task_queue_.empty() || !running_;
                });

                if (!running_) break;

                task = std::move(task_queue_.front());
                task_queue_.pop();
            }

            // Execute task
            task();
        }
    }
};
```

### **Timer Systems** ⏰
```cpp
// Timer wheel/Event Timer
class TimerEventLoop {
    using TimerCallback = std::function<void()>;

    struct Timer {
        std::chrono::steady_clock::time_point deadline;
        TimerCallback callback;
        bool periodic;
        std::chrono::milliseconds interval;
    };

    std::priority_queue<Timer, std::vector<Timer>,
                        decltype([](const Timer& a, const Timer& b) {
                            return a.deadline > b.deadline;
                        })> timers_;

    void run() {
        while (running_) {
            auto now = std::chrono::steady_clock::now();

            while (!timers_.empty() &&
                   timers_.top().deadline <= now) {

                Timer timer = timers_.top();
                timers_.pop();

                timer.callback();

                if (timer.periodic) {
                    timer.deadline += timer.interval;
                    timers_.push(timer);
                }
            }

            if (!timers_.empty()) {
                auto sleep_time = timers_.top().deadline - now;
                std::this_thread::sleep_for(sleep_time);
            }
        }
    }
};
```

## 🏆 **Real-World Production Examples**

### **GUI Frameworks**
- **Qt Event Loop**: Cross-platform GUI applications
- **GTK Main Loop**: GNOME desktop applications
- **WPF Dispatcher**: .NET Windows applications
- **Swing EDT**: Java desktop applications

### **TUI Applications**
- **vim**: Terminal text editor event loop
- **htop**: System monitor with keyboard handling
- **tmux**: Terminal multiplexer event system
- **gdb**: Debugger command loop

### **Network Servers**
- **nginx**: epoll-based HTTP server
- **Redis**: Event-driven in-memory database
- **Node.js**: libuv event loop
- **Apache**: Multi-threaded event handling

### **Async Frameworks**
- **libdispatch (GCD)**: Apple's Grand Central Dispatch
- **boost::asio**: C++ async I/O framework
- **tokio**: Rust async runtime
- **asyncio**: Python async framework

### **Timer Systems**
- **Linux kernel timer wheel**: O(1) timer management
- **boost::asio deadline_timer**: Network timeout handling
- **JavaScript setTimeout/setInterval**: Browser timer APIs
- **cron**: System scheduler

### **Message Queues**
- **Erlang OTP**: Actor model message passing
- **Akka**: Scala actor framework
- **ZeroMQ**: Message queue library
- **RabbitMQ**: AMQP message broker

## 🎯 **Key Design Principles**

### **1. Event Source Abstraction**
```cpp
class EventSource {
public:
    virtual ~EventSource() = default;
    virtual bool has_events() = 0;
    virtual Event get_next_event() = 0;
};

class EventLoop {
    std::vector<std::unique_ptr<EventSource>> sources_;

    void run() {
        while (running_) {
            for (auto& source : sources_) {
                if (source->has_events()) {
                    Event event = source->get_next_event();
                    dispatch_event(event);
                }
            }
        }
    }
};
```

### **2. Callback Registration**
```cpp
class EventLoop {
    std::unordered_map<EventType, std::vector<EventCallback>> handlers_;

public:
    using EventCallback = std::function<void(const Event&)>;

    void register_handler(EventType type, EventCallback callback) {
        handlers_[type].push_back(callback);
    }

    void dispatch_event(const Event& event) {
        auto it = handlers_.find(event.type);
        if (it != handlers_.end()) {
            for (auto& callback : it->second) {
                callback(event);
            }
        }
    }
};
```

### **3. Thread Safety**
```cpp
class ThreadSafeEventLoop {
    std::queue<Event> event_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::atomic<bool> running_{true};

    void post_event(Event event) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            event_queue_.push(event);
        }
        queue_cv_.notify_one();
    }

    void run() {
        while (running_) {
            Event event;

            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                queue_cv_.wait(lock, [this] {
                    return !event_queue_.empty() || !running_;
                });

                if (!running_) break;

                event = event_queue_.front();
                event_queue_.pop();
            }

            process_event(event);
        }
    }
};
```

## 🚀 **Advanced Patterns**

### **Priority Event Queues**
```cpp
enum class Priority { LOW, NORMAL, HIGH, CRITICAL };

struct PrioritizedEvent {
    Event event;
    Priority priority;
    std::chrono::steady_clock::time_point timestamp;
};

class PriorityEventLoop {
    std::priority_queue<PrioritizedEvent,
                       std::vector<PrioritizedEvent>,
                       decltype([](const PrioritizedEvent& a,
                                  const PrioritizedEvent& b) {
                           return a.priority < b.priority;
                       })> event_queue_;
};
```

### **Event Filtering & Transformation**
```cpp
class EventFilter {
public:
    virtual bool accept(const Event& event) = 0;
    virtual Event transform(const Event& event) = 0;
};

class FilteredEventLoop : public EventLoop {
    std::vector<std::unique_ptr<EventFilter>> filters_;

    void process_event(Event event) {
        for (auto& filter : filters_) {
            if (!filter->accept(event)) return;

            event = filter->transform(event);
        }

        EventLoop::process_event(event);
    }
};
```

### **Event Loop Composition**
```cpp
class CompositeEventLoop {
    std::vector<std::unique_ptr<EventLoop>> loops_;
    std::thread::id main_thread_;

public:
    void add_event_loop(std::unique_ptr<EventLoop> loop) {
        loops_.push_back(std::move(loop));
    }

    void run() {
        main_thread_ = std::this_thread::get_id();

        std::vector<std::thread> threads;
        for (size_t i = 1; i < loops_.size(); ++i) {
            threads.emplace_back([this, i]() {
                loops_[i]->run();
            });
        }

        loops_[0]->run(); // Main loop runs in main thread

        for (auto& thread : threads) {
            thread.join();
        }
    }
};
```

## ⚡ **Performance Optimizations**

### **1. Event Batching**
```cpp
class BatchedEventLoop {
    std::vector<Event> event_batch_;
    size_t batch_size_ = 64;

    void process_batch() {
        if (event_batch_.empty()) return;

        // Sort by type for better cache locality
        std::sort(event_batch_.begin(), event_batch_.end(),
                 [](const Event& a, const Event& b) {
                     return a.type < b.type;
                 });

        for (const auto& event : event_batch_) {
            process_event(event);
        }

        event_batch_.clear();
    }
};
```

### **2. Lock-Free Event Queues**
```cpp
class LockFreeEventLoop {
    moodycamel::ConcurrentQueue<Event> event_queue_;

    void post_event(Event event) {
        event_queue_.enqueue(event);
    }

    void run() {
        while (running_) {
            Event event;
            if (event_queue_.try_dequeue(event)) {
                process_event(event);
            } else {
                std::this_thread::sleep_for(1ms);
            }
        }
    }
};
```

### **3. CPU Cache Optimization**
```cpp
struct alignas(64) CacheAlignedEvent {
    EventType type;
    uint64_t timestamp;
    std::array<char, 56> data; // Pad to cache line
};

class CacheOptimizedEventLoop {
    std::vector<CacheAlignedEvent> event_buffer_;
    size_t read_index_ = 0;
    size_t write_index_ = 0;
};
```

## 🎯 **Common Pitfalls & Solutions**

### **1. Event Loop Starvation**
**Problem**: High-priority events starve low-priority ones
**Solution**: Priority aging, round-robin scheduling

### **2. Memory Leaks in Callbacks**
**Problem**: Lambda captures cause circular references
**Solution**: Weak pointers, explicit cleanup

### **3. Thread Safety Issues**
**Problem**: Concurrent access to event handlers
**Solution**: Handler registration/deregistration queues

### **4. Busy Waiting**
**Problem**: 100% CPU usage when no events
**Solution**: Blocking waits with timeouts

### **5. Event Ordering**
**Problem**: Events processed out of logical order
**Solution**: Sequence numbers, dependency tracking

## 🚀 **Future Directions**

### **1. Coroutine Integration**
```cpp
class CoroutineEventLoop {
    std::vector<std::coroutine_handle<>> coroutines_;

    void schedule_coroutine(std::coroutine_handle<> coro) {
        coroutines_.push_back(coro);
    }
};
```

### **2. SIMD Event Processing**
```cpp
class SIMD_EventLoop {
    __m256i event_types_; // Process 8 events simultaneously
    __m256d event_data_;
};
```

### **3. GPU-Accelerated Events**
```cpp
class GPU_EventLoop {
    cudaStream_t stream_;
    Event* gpu_events_;
};
```

## 📚 **Further Reading**

- **"Computer Systems: A Programmer's Perspective"** - Event-driven programming
- **"Unix Network Programming"** - Network event loops
- **"Game Programming Patterns"** - Game loop patterns
- **"Beautiful Code"** - Event loop implementations
- **Linux Kernel Source** - epoll implementation
- **Node.js/libuv** - High-performance event loops

---

*"Event loops are the beating heart of modern software systems - master them and you master concurrency itself."*
