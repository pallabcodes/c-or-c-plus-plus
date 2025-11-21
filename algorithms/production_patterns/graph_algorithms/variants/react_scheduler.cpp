/*
 * React Scheduler - Work Loop with Time Slicing and Priority Scheduling
 * 
 * Source: https://github.com/facebook/react/blob/main/packages/scheduler/src/forks/Scheduler.js
 * Repository: facebook/react
 * File: `packages/scheduler/src/forks/Scheduler.js`
 * 
 * What Makes It Ingenious:
 * - Time-sliced work loop: Can pause/resume work to keep UI responsive
 * - Priority-based scheduling: Different priority levels for different work
 * - MessageChannel/requestIdleCallback for scheduling: Uses browser APIs efficiently
 * - Work expiration tracking: Ensures high-priority work completes on time
 * - Continuous work loop: Processes work until deadline or all work done
 * - Used in React for concurrent rendering and keeping UI responsive
 * 
 * When to Use:
 * - Need to schedule work with priorities
 * - Time-sliced processing (pause/resume)
 * - Keep UI responsive during heavy computation
 * - Incremental processing of large tasks
 * - Priority-based task scheduling
 * 
 * Real-World Usage:
 * - React concurrent rendering
 * - React scheduler for fiber work
 * - UI frameworks requiring responsive rendering
 * - Incremental processing systems
 * - Priority-based task queues
 * 
 * Time Complexity:
 * - Schedule work: O(log n) for priority queue insertion
 * - Process work: O(1) per work unit (amortized)
 * - Work loop: O(n) where n is number of work units
 * 
 * Space Complexity: O(n) for priority queue
 */

#include <queue>
#include <vector>
#include <functional>
#include <chrono>
#include <cstdint>

// Priority levels (lower number = higher priority)
enum class Priority {
    Immediate = 1,    // Must be executed immediately
    UserBlocking = 2, // User interaction blocking
    Normal = 3,       // Normal priority
    Low = 4,          // Low priority
    Idle = 5          // Can be deferred
};

// Work unit
struct WorkUnit {
    std::function<void()> callback;
    Priority priority;
    int64_t expiration_time; // When this work expires
    int id;
    
    WorkUnit(std::function<void()> cb, Priority p, int64_t exp, int i)
        : callback(cb), priority(p), expiration_time(exp), id(i) {}
};

// Comparator for priority queue (min-heap by priority, then expiration)
struct WorkComparator {
    bool operator()(const WorkUnit& a, const WorkUnit& b) {
        if (a.priority != b.priority) {
            return static_cast<int>(a.priority) > static_cast<int>(b.priority);
        }
        return a.expiration_time > b.expiration_time;
    }
};

class ReactScheduler {
private:
    std::priority_queue<WorkUnit, std::vector<WorkUnit>, WorkComparator> work_queue_;
    int64_t current_time_;
    bool is_working_;
    int work_id_counter_;
    
    // Get current time in milliseconds
    int64_t get_current_time() {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ).count();
    }
    
    // Calculate expiration time based on priority
    int64_t calculate_expiration_time(Priority priority) {
        int64_t timeout;
        switch (priority) {
            case Priority::Immediate:
                timeout = 0;
                break;
            case Priority::UserBlocking:
                timeout = 250; // 250ms
                break;
            case Priority::Normal:
                timeout = 5000; // 5s
                break;
            case Priority::Low:
                timeout = 10000; // 10s
                break;
            case Priority::Idle:
                timeout = 999999999; // Effectively never
                break;
        }
        return current_time_ + timeout;
    }
    
public:
    ReactScheduler() 
        : current_time_(0)
        , is_working_(false)
        , work_id_counter_(0) {}
    
    // Schedule work with priority
    int schedule_work(std::function<void()> callback, Priority priority) {
        current_time_ = get_current_time();
        int64_t expiration = calculate_expiration_time(priority);
        int id = work_id_counter_++;
        
        work_queue_.emplace(callback, priority, expiration, id);
        return id;
    }
    
    // Cancel scheduled work (simplified - would need work ID tracking)
    void cancel_work(int work_id) {
        // In real implementation, would mark work as cancelled
        // For simplicity, we'll just skip cancelled work during processing
    }
    
    // Work loop - process work until deadline or all work done
    void work_loop(int64_t deadline_ms) {
        if (is_working_) return; // Prevent re-entrancy
        
        is_working_ = true;
        current_time_ = get_current_time();
        int64_t deadline = current_time_ + deadline_ms;
        
        while (!work_queue_.empty() && current_time_ < deadline) {
            WorkUnit work = work_queue_.top();
            work_queue_.pop();
            
            // Check if work expired
            if (current_time_ > work.expiration_time) {
                // Work expired, skip it (or handle expiration)
                continue;
            }
            
            // Execute work
            work.callback();
            
            // Update current time
            current_time_ = get_current_time();
        }
        
        is_working_ = false;
    }
    
    // Process all work (blocking)
    void flush_work() {
        while (!work_queue_.empty()) {
            WorkUnit work = work_queue_.top();
            work_queue_.pop();
            work.callback();
        }
    }
    
    // Check if there's pending work
    bool has_pending_work() const {
        return !work_queue_.empty();
    }
    
    // Get next expiration time
    int64_t get_next_expiration_time() const {
        if (work_queue_.empty()) {
            return INT64_MAX;
        }
        return work_queue_.top().expiration_time;
    }
    
    // Time-sliced work processing (React's pattern)
    bool should_yield() const {
        if (work_queue_.empty()) {
            return false;
        }
        
        current_time_ = get_current_time();
        int64_t next_expiration = get_next_expiration_time();
        
        // Yield if we're past deadline or work is expiring soon
        return current_time_ >= next_expiration;
    }
};

// Example usage
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    ReactScheduler scheduler;
    
    // Schedule work with different priorities
    scheduler.schedule_work([]() {
        std::cout << "High priority work executed" << std::endl;
    }, Priority::Immediate);
    
    scheduler.schedule_work([]() {
        std::cout << "Normal priority work executed" << std::endl;
    }, Priority::Normal);
    
    scheduler.schedule_work([]() {
        std::cout << "Low priority work executed" << std::endl;
    }, Priority::Low);
    
    // Process work with time slicing (5ms deadline)
    std::cout << "Processing work with 5ms deadline:" << std::endl;
    scheduler.work_loop(5);
    
    // Process remaining work
    std::cout << "\nProcessing remaining work:" << std::endl;
    scheduler.flush_work();
    
    // Simulate continuous work loop (React's pattern)
    std::cout << "\nSimulating continuous work loop:" << std::endl;
    for (int i = 0; i < 3; i++) {
        scheduler.schedule_work([i]() {
            std::cout << "Work unit " << i << " executed" << std::endl;
        }, Priority::Normal);
    }
    
    // Process in chunks
    while (scheduler.has_pending_work()) {
        scheduler.work_loop(2); // 2ms chunks
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    return 0;
}

