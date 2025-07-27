#pragma once

#include <string>
#include <string_view>
#include <chrono>
#include <format>
#include <mutex>
#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <sstream>
#include <iomanip>
#include <queue>
#include <functional>
#include <condition_variable>
#include <unordered_map>
#include <expected>

namespace utils {

// ================================================================================================
// Logging System
// ================================================================================================

/**
 * Log levels for filtering messages
 */
enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5
};

/**
 * High-performance logger with configurable output and formatting
 */
class Logger {
public:
    /**
     * Get the global logger instance
     */
    static Logger& instance();
    
    /**
     * Set minimum log level
     */
    void set_level(LogLevel level) { level_ = level; }
    
    /**
     * Set log output file (use empty string for stdout)
     */
    void set_output_file(const std::string& filename);
    
    /**
     * Enable/disable timestamps in log messages
     */
    void set_include_timestamp(bool include) { include_timestamp_ = include; }
    
    /**
     * Enable/disable thread ID in log messages
     */
    void set_include_thread_id(bool include) { include_thread_id_ = include; }
    
    /**
     * Log a message with specified level
     */
    template<typename... Args>
    void log(LogLevel level, std::string_view format, Args&&... args) {
        if (level < level_) return;
        
        std::string message;
        
        // Add timestamp if enabled
        if (include_timestamp_) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;
            
            auto tm = *std::localtime(&time_t);
            message += std::format("[{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}.{:03d}] ",
                                 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                                 tm.tm_hour, tm.tm_min, tm.tm_sec, ms.count());
        }
        
        // Add log level
        message += std::format("[{}] ", level_to_string(level));
        
        // Add thread ID if enabled
        if (include_thread_id_) {
            std::ostringstream oss;
            oss << std::this_thread::get_id();
            message += std::format("[{}] ", oss.str());
        }
        
        // Add formatted message
        if constexpr (sizeof...(args) > 0) {
            message += std::vformat(format, std::make_format_args(args...));
        } else {
            message += format;
        }
        
        message += '\n';
        
        // Thread-safe output
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (output_file_.is_open()) {
                output_file_ << message;
                output_file_.flush();
            } else {
                std::cout << message;
                std::cout.flush();
            }
        }
    }

private:
    Logger() = default;
    
    std::string_view level_to_string(LogLevel level) const {
        switch (level) {
            case LogLevel::TRACE: return "TRACE";
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO:  return "INFO";
            case LogLevel::WARN:  return "WARN";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }
    
    std::atomic<LogLevel> level_{LogLevel::INFO};
    std::atomic<bool> include_timestamp_{true};
    std::atomic<bool> include_thread_id_{false};
    std::ofstream output_file_;
    std::mutex mutex_;
};

// Convenience logging macros
#define LOG_TRACE(...) utils::Logger::instance().log(utils::LogLevel::TRACE, __VA_ARGS__)
#define LOG_DEBUG(...) utils::Logger::instance().log(utils::LogLevel::DEBUG, __VA_ARGS__)
#define LOG_INFO(...)  utils::Logger::instance().log(utils::LogLevel::INFO, __VA_ARGS__)
#define LOG_WARN(...)  utils::Logger::instance().log(utils::LogLevel::WARN, __VA_ARGS__)
#define LOG_ERROR(...) utils::Logger::instance().log(utils::LogLevel::ERROR, __VA_ARGS__)
#define LOG_FATAL(...) utils::Logger::instance().log(utils::LogLevel::FATAL, __VA_ARGS__)

// Convenience functions for backward compatibility
template<typename... Args>
void log_info(std::string_view format, Args&&... args) {
    Logger::instance().log(LogLevel::INFO, format, std::forward<Args>(args)...);
}

template<typename... Args>
void log_error(std::string_view format, Args&&... args) {
    Logger::instance().log(LogLevel::ERROR, format, std::forward<Args>(args)...);
}

template<typename... Args>
void log_debug(std::string_view format, Args&&... args) {
    Logger::instance().log(LogLevel::DEBUG, format, std::forward<Args>(args)...);
}

// ================================================================================================
// Thread Pool
// ================================================================================================

/**
 * Simple thread pool for async task execution
 */
class ThreadPool {
public:
    /**
     * Create thread pool with specified number of worker threads
     */
    explicit ThreadPool(size_t num_threads);
    
    /**
     * Destructor waits for all tasks to complete
     */
    ~ThreadPool();
    
    /**
     * Submit a task for execution
     */
    template<typename F>
    void submit(F&& task) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stopping_) return;
            
            tasks_.emplace(std::forward<F>(task));
        }
        condition_.notify_one();
    }
    
    /**
     * Get number of worker threads
     */
    size_t size() const { return workers_.size(); }
    
    /**
     * Get number of pending tasks
     */
    size_t pending_tasks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return tasks_.size();
    }

private:
    void worker_loop();
    
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic<bool> stopping_{false};
};

// ================================================================================================
// JSON Parser (Minimal Implementation)
// ================================================================================================

/**
 * Simple JSON value representation
 */
class JsonValue {
public:
    enum class Type {
        NULL_VALUE,
        BOOLEAN,
        NUMBER,
        STRING,
        ARRAY,
        OBJECT
    };
    
    JsonValue() : type_(Type::NULL_VALUE) {}
    JsonValue(bool value) : type_(Type::BOOLEAN), bool_value_(value) {}
    JsonValue(double value) : type_(Type::NUMBER), number_value_(value) {}
    JsonValue(std::string value) : type_(Type::STRING), string_value_(std::move(value)) {}
    
    Type type() const { return type_; }
    
    bool as_bool() const { return bool_value_; }
    double as_number() const { return number_value_; }
    const std::string& as_string() const { return string_value_; }
    
    // Array operations
    void push_back(JsonValue value) {
        if (type_ != Type::ARRAY) {
            type_ = Type::ARRAY;
            array_value_.clear();
        }
        array_value_.push_back(std::move(value));
    }
    
    const std::vector<JsonValue>& as_array() const { return array_value_; }
    
    // Object operations
    void set(const std::string& key, JsonValue value) {
        if (type_ != Type::OBJECT) {
            type_ = Type::OBJECT;
            object_value_.clear();
        }
        object_value_[key] = std::move(value);
    }
    
    const JsonValue& get(const std::string& key) const {
        static JsonValue null_value;
        auto it = object_value_.find(key);
        return it != object_value_.end() ? it->second : null_value;
    }
    
    const std::unordered_map<std::string, JsonValue>& as_object() const { 
        return object_value_; 
    }
    
    /**
     * Serialize to JSON string
     */
    std::string to_string() const;

private:
    Type type_;
    bool bool_value_ = false;
    double number_value_ = 0.0;
    std::string string_value_;
    std::vector<JsonValue> array_value_;
    std::unordered_map<std::string, JsonValue> object_value_;
};

/**
 * Simple JSON parser
 */
class JsonParser {
public:
    /**
     * Parse JSON from string
     */
    static std::expected<JsonValue, std::string> parse(std::string_view json);

private:
    JsonParser(std::string_view json) : json_(json), pos_(0) {}
    
    JsonValue parse_value();
    JsonValue parse_object();
    JsonValue parse_array();
    JsonValue parse_string();
    JsonValue parse_number();
    JsonValue parse_literal();
    
    void skip_whitespace();
    char peek() const;
    char consume();
    std::string parse_string_content();
    
    std::string_view json_;
    size_t pos_;
};

// ================================================================================================
// Performance Utilities
// ================================================================================================

/**
 * High-resolution timer for performance measurement
 */
class Timer {
public:
    Timer() : start_time_(std::chrono::high_resolution_clock::now()) {}
    
    /**
     * Reset the timer
     */
    void reset() {
        start_time_ = std::chrono::high_resolution_clock::now();
    }
    
    /**
     * Get elapsed time in milliseconds
     */
    double elapsed_ms() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start_time_);
        return duration.count() / 1000.0;
    }
    
    /**
     * Get elapsed time in microseconds
     */
    uint64_t elapsed_us() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start_time_);
        return duration.count();
    }
    
    /**
     * Get elapsed time in nanoseconds
     */
    uint64_t elapsed_ns() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_time_);
        return duration.count();
    }

private:
    std::chrono::high_resolution_clock::time_point start_time_;
};

/**
 * RAII-style scoped timer that logs elapsed time
 */
class ScopedTimer {
public:
    explicit ScopedTimer(std::string name) : name_(std::move(name)) {}
    
    ~ScopedTimer() {
        LOG_DEBUG("{} took {:.3f}ms", name_, timer_.elapsed_ms());
    }

private:
    std::string name_;
    Timer timer_;
};

#define SCOPED_TIMER(name) utils::ScopedTimer _timer(name)

/**
 * Latency histogram for performance monitoring
 */
class LatencyHistogram {
public:
    LatencyHistogram() = default;
    
    /**
     * Record a latency measurement
     */
    void record(std::chrono::nanoseconds latency) {
        std::lock_guard<std::mutex> lock(mutex_);
        samples_.push_back(latency.count());
        total_samples_++;
        total_latency_ += latency.count();
    }
    
    /**
     * Get percentile value (0.0 to 1.0)
     */
    double percentile(double p) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (samples_.empty()) return 0.0;
        
        auto sorted = samples_;
        std::sort(sorted.begin(), sorted.end());
        
        size_t index = static_cast<size_t>(p * (sorted.size() - 1));
        return static_cast<double>(sorted[index]);
    }
    
    /**
     * Get average latency
     */
    double average() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (total_samples_ == 0) return 0.0;
        return static_cast<double>(total_latency_) / total_samples_;
    }
    
    /**
     * Get sample count
     */
    size_t count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return total_samples_;
    }
    
    /**
     * Clear all samples
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        samples_.clear();
        total_samples_ = 0;
        total_latency_ = 0;
    }

private:
    mutable std::mutex mutex_;
    std::vector<uint64_t> samples_;
    uint64_t total_samples_ = 0;
    uint64_t total_latency_ = 0;
};

// ================================================================================================
// String Utilities
// ================================================================================================

/**
 * Trim whitespace from string
 */
std::string trim(std::string_view str);

/**
 * Split string by delimiter
 */
std::vector<std::string> split(std::string_view str, char delimiter);

/**
 * Join strings with delimiter
 */
std::string join(const std::vector<std::string>& parts, std::string_view delimiter);

/**
 * Convert string to lowercase
 */
std::string to_lower(std::string_view str);

/**
 * Convert string to uppercase
 */
std::string to_upper(std::string_view str);

/**
 * Check if string starts with prefix
 */
bool starts_with(std::string_view str, std::string_view prefix);

/**
 * Check if string ends with suffix
 */
bool ends_with(std::string_view str, std::string_view suffix);

// ================================================================================================
// Random Utilities
// ================================================================================================

/**
 * Generate random string of specified length
 */
std::string random_string(size_t length, std::string_view charset = 
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

/**
 * Generate random integer in range [min, max]
 */
int random_int(int min, int max);

/**
 * Generate random UUID (version 4)
 */
std::string random_uuid();

} // namespace utils
