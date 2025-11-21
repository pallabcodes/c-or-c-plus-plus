/*
 * Linux I/O Interval Coalescing
 *
 * Source: Linux kernel I/O scheduler and block layer
 * Repository: https://github.com/torvalds/linux
 * Files: block/blk-merge.c, block/elevator.c, include/linux/blkdev.h
 * Algorithm: Adjacent I/O request merging for disk optimization
 *
 * What Makes It Ingenious:
 * - Adjacent merging: Only merge physically adjacent requests
 * - Elevator algorithm integration: Reorders requests for efficiency
 * - Deadline scheduling: Prevents starvation of old requests
 * - Plugging/unplugging: Batch request submission
 * - Used in Linux I/O schedulers (CFQ, deadline, noop)
 * - Minimizes disk head movement and improves throughput
 *
 * When to Use:
 * - Disk I/O optimization
 * - File system operations
 * - Storage subsystem management
 * - Network packet coalescing
 * - Batch processing systems
 * - Resource scheduling with physical constraints
 *
 * Real-World Usage:
 * - Linux kernel I/O schedulers
 * - Database storage engines
 * - File system drivers
 * - Network protocol stacks
 * - Storage area networks (SAN)
 * - SSD flash translation layers
 *
 * Time Complexity: O(n) for merging operations
 * Space Complexity: O(1) extra space beyond request storage
 */

#include <vector>
#include <deque>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdint>

// I/O request representation (simplified from Linux kernel)
struct IORequest {
    uint64_t sector_start;    // Starting sector number
    uint32_t sector_count;    // Number of sectors
    bool is_read;            // Read or write operation
    uint64_t timestamp;       // Request arrival time
    int priority;            // Request priority
    void* data;              // User data pointer

    IORequest(uint64_t start, uint32_t count, bool read = true,
              uint64_t time = 0, int prio = 0)
        : sector_start(start), sector_count(count), is_read(read),
          timestamp(time), priority(prio), data(nullptr) {}

    uint64_t sector_end() const {
        return sector_start + sector_count;
    }

    bool overlaps(const IORequest& other) const {
        return sector_start < other.sector_end() &&
               other.sector_start < sector_end();
    }

    bool adjacent(const IORequest& other) const {
        return sector_end() == other.sector_start ||
               other.sector_end() == sector_start;
    }

    // Check if requests can be merged
    bool can_merge(const IORequest& other) const {
        // Must be same operation type and adjacent/overlapping
        return is_read == other.is_read &&
               (overlaps(other) || adjacent(other));
    }

    // Merge two requests (assumes they can be merged)
    IORequest merge(const IORequest& other) const {
        uint64_t new_start = std::min(sector_start, other.sector_start);
        uint64_t new_end = std::max(sector_end(), other.sector_end());
        uint32_t new_count = new_end - new_start;

        // Use earliest timestamp and higher priority
        uint64_t new_time = std::min(timestamp, other.timestamp);
        int new_prio = std::max(priority, other.priority);

        return IORequest(new_start, new_count, is_read, new_time, new_prio);
    }

    void print() const {
        std::cout << (is_read ? "READ" : "WRITE")
                  << " [" << sector_start << ", " << sector_end() << "]"
                  << " sectors: " << sector_count
                  << " prio: " << priority;
    }
};

// Linux-style I/O scheduler with interval coalescing
class LinuxIOScheduler {
private:
    std::deque<IORequest> request_queue_;
    bool plugged_;           // Plugging state for batching
    uint64_t last_submit_time_;
    size_t max_queue_depth_;
    uint64_t current_time_;   // Simulated time

    // Deadline parameters (simplified)
    uint64_t read_expire_time_;
    uint64_t write_expire_time_;

    // Statistics
    size_t total_requests_;
    size_t merged_requests_;
    size_t submitted_requests_;

public:
    LinuxIOScheduler(size_t max_depth = 128)
        : plugged_(false), last_submit_time_(0), max_queue_depth_(max_depth),
          current_time_(0), read_expire_time_(5000), write_expire_time_(10000),
          total_requests_(0), merged_requests_(0), submitted_requests_(0) {}

    // Add I/O request to scheduler
    void submit_request(const IORequest& request) {
        IORequest req = request;
        req.timestamp = current_time_;
        total_requests_++;

        // Try to merge with existing requests
        if (try_merge_request(req)) {
            merged_requests_++;
            return;
        }

        // Add to queue
        request_queue_.push_back(req);

        // Sort by sector for better merging opportunities
        sort_queue_by_sector();

        // Check if we should unplug (submit requests)
        if (should_unplug()) {
            unplug();
        }
    }

    // Try to merge request with existing ones
    bool try_merge_request(IORequest& new_req) {
        // Look for mergeable requests (adjacent or overlapping)
        for (auto it = request_queue_.begin(); it != request_queue_.end(); ++it) {
            if (it->can_merge(new_req)) {
                // Merge the requests
                *it = it->merge(new_req);
                return true;
            }
        }
        return false;
    }

    // Sort queue by sector number (for better merging)
    void sort_queue_by_sector() {
        std::sort(request_queue_.begin(), request_queue_.end(),
                 [](const IORequest& a, const IORequest& b) {
                     return a.sector_start < b.sector_start;
                 });
    }

    // Check if deadline has expired (should unplug)
    bool deadline_expired() const {
        if (request_queue_.empty()) return false;

        uint64_t oldest_time = UINT64_MAX;
        for (const auto& req : request_queue_) {
            oldest_time = std::min(oldest_time, req.timestamp);
        }

        uint64_t age = current_time_ - oldest_time;
        uint64_t expire_time = request_queue_.front().is_read ?
                              read_expire_time_ : write_expire_time_;

        return age >= expire_time;
    }

    // Check if queue is full
    bool queue_full() const {
        return request_queue_.size() >= max_queue_depth_;
    }

    // Determine if we should unplug (submit requests)
    bool should_unplug() const {
        return plugged_ && (deadline_expired() || queue_full());
    }

    // Submit all queued requests to disk
    void unplug() {
        plugged_ = false;

        if (request_queue_.empty()) return;

        std::cout << "Submitting " << request_queue_.size() << " I/O requests:" << std::endl;

        // Simulate elevator algorithm: sort by sector for optimal disk access
        std::sort(request_queue_.begin(), request_queue_.end(),
                 [](const IORequest& a, const IORequest& b) {
                     if (a.is_read != b.is_read) return a.is_read; // Reads first
                     return a.sector_start < b.sector_start;      // Then by sector
                 });

        for (const auto& req : request_queue_) {
            std::cout << "  ";
            req.print();
            std::cout << std::endl;
            submitted_requests_++;
        }

        // Simulate I/O time
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        request_queue_.clear();
        last_submit_time_ = current_time_;
    }

    // Plug the scheduler (batch mode)
    void plug() {
        plugged_ = true;
    }

    // Advance simulated time
    void advance_time(uint64_t delta) {
        current_time_ += delta;

        // Check for deadline expiry
        if (should_unplug()) {
            unplug();
        }
    }

    // Get statistics
    void print_statistics() const {
        std::cout << "I/O Scheduler Statistics:" << std::endl;
        std::cout << "  Total requests: " << total_requests_ << std::endl;
        std::cout << "  Merged requests: " << merged_requests_ << std::endl;
        std::cout << "  Submitted requests: " << submitted_requests_ << std::endl;
        std::cout << "  Merge ratio: " << (total_requests_ > 0 ?
                    (double)merged_requests_ / total_requests_ * 100 : 0) << "%" << std::endl;
        std::cout << "  Current queue depth: " << request_queue_.size() << std::endl;
        std::cout << "  Plugged: " << (plugged_ ? "yes" : "no") << std::endl;
    }

    // Get current queue (for inspection)
    const std::deque<IORequest>& get_queue() const {
        return request_queue_;
    }
};

// Linux block device I/O coalescing
class LinuxBlockDevice {
private:
    LinuxIOScheduler scheduler_;
    uint64_t total_sectors_;
    uint64_t current_time_;

public:
    LinuxBlockDevice(uint64_t sectors = 1000000)
        : total_sectors_(sectors), current_time_(0) {
        // Start in plugged mode for batching
        scheduler_.plug();
    }

    // Submit I/O request
    void submit_io(uint64_t sector_start, uint32_t sector_count,
                   bool is_read = true, int priority = 0) {
        if (sector_start + sector_count > total_sectors_) {
            std::cerr << "I/O request out of bounds!" << std::endl;
            return;
        }

        IORequest req(sector_start, sector_count, is_read, current_time_, priority);
        scheduler_.submit_request(req);
    }

    // Flush pending requests
    void flush() {
        scheduler_.unplug();
    }

    // Simulate time passing
    void advance_time(uint64_t milliseconds) {
        current_time_ += milliseconds;
        scheduler_.advance_time(milliseconds);
    }

    // Print device statistics
    void print_stats() {
        std::cout << "Block Device Statistics:" << std::endl;
        std::cout << "  Total sectors: " << total_sectors_ << std::endl;
        std::cout << "  Current time: " << current_time_ << "ms" << std::endl;
        scheduler_.print_statistics();
    }

    LinuxIOScheduler& get_scheduler() { return scheduler_; }
};

// Example usage demonstrating Linux I/O coalescing
int main() {
    std::cout << "Linux I/O Interval Coalescing Demonstration:" << std::endl;

    LinuxBlockDevice device;

    // Simulate a workload with adjacent and overlapping requests
    std::cout << "Submitting I/O requests..." << std::endl;

    // Adjacent reads (should be merged)
    device.submit_io(1000, 64, true, 1);
    device.submit_io(1064, 32, true, 1);  // Adjacent to first
    device.submit_io(1024, 16, true, 1);  // Overlaps with first two

    // Non-adjacent reads (won't be merged)
    device.submit_io(2000, 128, true, 2);

    // Writes (different from reads, won't merge with reads)
    device.submit_io(1500, 64, false, 3);
    device.submit_io(1564, 32, false, 3); // Adjacent write

    // Advance time to trigger deadline
    device.advance_time(6000); // This should trigger unplug due to deadline

    std::cout << "\nAfter deadline expiry:" << std::endl;
    device.print_stats();

    // Submit more requests
    device.submit_io(3000, 256, true, 4);
    device.submit_io(3256, 128, true, 4); // Adjacent

    // Manually flush
    device.flush();

    std::cout << "\nFinal statistics:" << std::endl;
    device.print_stats();

    return 0;
}

