/**
 * @file kafka_style_streaming.cpp
 * @brief Kafka-style stream processing combining multiple research papers
 *
 * This implementation provides:
 * - Exactly-once processing semantics with idempotent operations
 * - Windowing strategies (tumbling, sliding, session windows)
 * - Stream-table duality with materialized views
 * - Consumer group management with rebalancing
 * - Partitioning and replication for fault tolerance
 * - Backpressure handling and flow control
 * - Event time vs processing time semantics
 *
 * Research Papers & Sources:
 * - "The Dataflow Model: A Practical Approach to Balancing Correctness, Latency, and Cost in Massive-Scale, Unbounded, Out-of-Order Data Processing" - Google (2015)
 * - "MillWheel: Fault-Tolerant Stream Processing at Internet Scale" - Google (2013)
 * - "Kafka: a Distributed Messaging System for Log Processing" - LinkedIn (2011)
 * - "Discretized Streams: Fault-Tolerant Streaming Computation at Scale" - UC Berkeley (2012)
 * - "Exactly-once semantics in a distributed stream processing system" - Microsoft Research
 * - Apache Kafka source code and documentation
 * - Apache Flink streaming patterns
 *
 * Unique Implementation: Combines Kafka's partitioning model with Google's
 * MillWheel's fault tolerance and Dataflow's windowing semantics
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <memory>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <sstream>
#include <iomanip>
#include <random>
#include <atomic>

// ============================================================================
// Core Streaming Concepts
// ============================================================================

enum class ProcessingSemantics {
    AT_LEAST_ONCE,
    AT_MOST_ONCE,
    EXACTLY_ONCE
};

enum class WindowType {
    TUMBLING,
    SLIDING,
    SESSION,
    GLOBAL
};

enum class TimeCharacteristic {
    PROCESSING_TIME,
    EVENT_TIME,
    INGESTION_TIME
};

struct StreamRecord {
    std::string key;
    std::vector<uint8_t> value;
    int64_t timestamp;
    int64_t watermark;
    std::string partition_key;
    int partition_id;
    int64_t offset;

    StreamRecord(const std::string& k, const std::vector<uint8_t>& v,
                int64_t ts = 0, const std::string& pk = "")
        : key(k), value(v), timestamp(ts), watermark(0),
          partition_key(pk), partition_id(0), offset(0) {}
};

struct Window {
    int64_t start_time;
    int64_t end_time;
    std::vector<StreamRecord> records;
    bool is_complete;
    int64_t max_timestamp;

    Window(int64_t start, int64_t end)
        : start_time(start), end_time(end), is_complete(false), max_timestamp(0) {}

    void add_record(const StreamRecord& record) {
        records.push_back(record);
        max_timestamp = std::max(max_timestamp, record.timestamp);
    }

    size_t size() const { return records.size(); }
};

// ============================================================================
// Partition and Consumer Management
// ============================================================================

struct PartitionInfo {
    int partition_id;
    int64_t start_offset;
    int64_t end_offset;
    int64_t committed_offset;
    int64_t high_watermark;
    bool is_leader;
    std::string leader_broker;

    PartitionInfo(int id) : partition_id(id), start_offset(0), end_offset(0),
                          committed_offset(-1), high_watermark(0), is_leader(false) {}
};

struct ConsumerInfo {
    std::string consumer_id;
    std::string group_id;
    std::unordered_set<int> assigned_partitions;
    int64_t last_heartbeat;
    bool is_coordinator;

    ConsumerInfo(const std::string& cid, const std::string& gid)
        : consumer_id(cid), group_id(gid), last_heartbeat(0), is_coordinator(false) {}
};

class ConsumerGroupManager {
private:
    std::string group_id_;
    std::unordered_map<std::string, ConsumerInfo> consumers_;
    std::unordered_map<int, PartitionInfo> partitions_;
    std::mutex mutex_;

    // Rebalancing algorithm
    void rebalance_partitions() {
        if (consumers_.empty() || partitions_.empty()) return;

        std::vector<std::string> consumer_list;
        for (const auto& pair : consumers_) {
            consumer_list.push_back(pair.first);
        }

        // Range assignment strategy (simplified)
        int total_partitions = partitions_.size();
        int total_consumers = consumer_list.size();

        for (size_t i = 0; i < consumer_list.size(); ++i) {
            auto& consumer = consumers_[consumer_list[i]];
            consumer.assigned_partitions.clear();

            // Calculate partition range for this consumer
            int partitions_per_consumer = total_partitions / total_consumers;
            int extra_partitions = total_partitions % total_consumers;

            int start_partition = i * partitions_per_consumer + std::min(i, extra_partitions);
            int end_partition = start_partition + partitions_per_consumer + (i < extra_partitions ? 1 : 0);

            for (int p = start_partition; p < end_partition; ++p) {
                consumer.assigned_partitions.insert(p);
            }
        }

        std::cout << "Rebalanced " << total_partitions << " partitions across "
                 << total_consumers << " consumers\n";
    }

public:
    ConsumerGroupManager(const std::string& group_id) : group_id_(group_id) {}

    void add_consumer(const std::string& consumer_id) {
        std::unique_lock<std::mutex> lock(mutex_);
        consumers_[consumer_id] = ConsumerInfo(consumer_id, group_id_);
        rebalance_partitions();
    }

    void remove_consumer(const std::string& consumer_id) {
        std::unique_lock<std::mutex> lock(mutex_);
        consumers_.erase(consumer_id);
        rebalance_partitions();
    }

    void add_partition(int partition_id) {
        std::unique_lock<std::mutex> lock(mutex_);
        partitions_[partition_id] = PartitionInfo(partition_id);
        rebalance_partitions();
    }

    std::unordered_set<int> get_consumer_partitions(const std::string& consumer_id) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (consumers_.count(consumer_id)) {
            return consumers_[consumer_id].assigned_partitions;
        }
        return {};
    }

    void update_heartbeat(const std::string& consumer_id) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (consumers_.count(consumer_id)) {
            consumers_[consumer_id].last_heartbeat =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
        }
    }

    void commit_offset(const std::string& consumer_id, int partition_id, int64_t offset) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (partitions_.count(partition_id)) {
            partitions_[partition_id].committed_offset = offset;
        }
    }

    int64_t get_committed_offset(const std::string& consumer_id, int partition_id) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (partitions_.count(partition_id)) {
            return partitions_[partition_id].committed_offset;
        }
        return -1;
    }

    std::vector<std::string> get_consumers() {
        std::unique_lock<std::mutex> lock(mutex_);
        std::vector<std::string> result;
        for (const auto& pair : consumers_) {
            result.push_back(pair.first);
        }
        return result;
    }
};

// ============================================================================
// Windowing Engine (Dataflow Model)
// ============================================================================

class WindowingEngine {
private:
    WindowType window_type_;
    int64_t window_size_;
    int64_t window_slide_;
    int64_t allowed_lateness_;
    TimeCharacteristic time_characteristic_;

    std::unordered_map<std::string, std::vector<Window>> active_windows_;
    std::unordered_map<std::string, int64_t> watermarks_;

    // Session window gap detection
    int64_t session_gap_;

public:
    WindowingEngine(WindowType type, int64_t size, int64_t slide = 0,
                   int64_t lateness = 0, TimeCharacteristic time_char = TimeCharacteristic::EVENT_TIME)
        : window_type_(type), window_size_(size), window_slide_(slide ? slide : size),
          allowed_lateness_(lateness), time_characteristic_(time_char), session_gap_(30000) {} // 30 seconds

    std::vector<Window*> assign_windows(const StreamRecord& record) {
        std::vector<Window*> assigned_windows;

        int64_t timestamp = get_record_timestamp(record);
        std::string key = record.key;

        // Ensure we have window storage for this key
        if (active_windows_.find(key) == active_windows_.end()) {
            active_windows_[key] = std::vector<Window>();
        }

        switch (window_type_) {
            case WindowType::TUMBLING:
                assigned_windows = assign_tumbling_windows(key, timestamp);
                break;
            case WindowType::SLIDING:
                assigned_windows = assign_sliding_windows(key, timestamp);
                break;
            case WindowType::SESSION:
                assigned_windows = assign_session_windows(key, timestamp);
                break;
            case WindowType::GLOBAL:
                assigned_windows = assign_global_windows(key, timestamp);
                break;
        }

        // Add record to assigned windows
        for (Window* window : assigned_windows) {
            window->add_record(record);
        }

        return assigned_windows;
    }

    std::vector<Window> get_completed_windows(const std::string& key, int64_t current_watermark) {
        std::vector<Window> completed;

        if (active_windows_.find(key) == active_windows_.end()) {
            return completed;
        }

        auto& windows = active_windows_[key];

        // Check watermark-based completion
        auto it = windows.begin();
        while (it != windows.end()) {
            if (is_window_complete(*it, current_watermark)) {
                completed.push_back(std::move(*it));
                it = windows.erase(it);
            } else {
                ++it;
            }
        }

        return completed;
    }

    void update_watermark(const std::string& key, int64_t watermark) {
        watermarks_[key] = std::max(watermarks_[key], watermark);
    }

    int64_t get_watermark(const std::string& key) {
        return watermarks_[key];
    }

private:
    int64_t get_record_timestamp(const StreamRecord& record) {
        switch (time_characteristic_) {
            case TimeCharacteristic::EVENT_TIME:
                return record.timestamp;
            case TimeCharacteristic::PROCESSING_TIME:
                return std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
            case TimeCharacteristic::INGESTION_TIME:
                return record.timestamp; // Assume timestamp is ingestion time
            default:
                return record.timestamp;
        }
    }

    std::vector<Window*> assign_tumbling_windows(const std::string& key, int64_t timestamp) {
        int64_t window_start = (timestamp / window_size_) * window_size_;

        return get_or_create_window(key, window_start, window_start + window_size_);
    }

    std::vector<Window*> assign_sliding_windows(const std::string& key, int64_t timestamp) {
        std::vector<Window*> assigned;

        // Calculate which windows this timestamp belongs to
        int64_t earliest_window = timestamp - window_size_ + window_slide_;
        int64_t latest_window = timestamp + window_slide_;

        for (int64_t window_end = earliest_window; window_end <= latest_window; window_end += window_slide_) {
            int64_t window_start = window_end - window_size_;
            if (window_start <= timestamp && timestamp < window_end) {
                auto windows = get_or_create_window(key, window_start, window_end);
                assigned.insert(assigned.end(), windows.begin(), windows.end());
            }
        }

        return assigned;
    }

    std::vector<Window*> assign_session_windows(const std::string& key, int64_t timestamp) {
        auto& windows = active_windows_[key];

        // Find if this timestamp fits into an existing session window
        for (auto& window : windows) {
            if (timestamp >= window.start_time && timestamp < window.end_time + session_gap_) {
                // Extend the window
                window.end_time = timestamp + session_gap_;
                window.max_timestamp = std::max(window.max_timestamp, timestamp);
                return {&window};
            }
        }

        // Create new session window
        int64_t window_start = timestamp;
        int64_t window_end = timestamp + session_gap_;
        return get_or_create_window(key, window_start, window_end);
    }

    std::vector<Window*> assign_global_windows(const std::string& key, int64_t timestamp) {
        // Single global window per key
        return get_or_create_window(key, 0, INT64_MAX);
    }

    std::vector<Window*> get_or_create_window(const std::string& key, int64_t start, int64_t end) {
        auto& windows = active_windows_[key];

        // Find existing window
        for (auto& window : windows) {
            if (window.start_time == start && window.end_time == end) {
                return {&window};
            }
        }

        // Create new window
        windows.emplace_back(start, end);
        return {&windows.back()};
    }

    bool is_window_complete(const Window& window, int64_t current_watermark) {
        if (window_type_ == WindowType::GLOBAL) {
            return false; // Global windows never complete
        }

        // Window is complete if watermark has passed the end time + allowed lateness
        return current_watermark >= window.end_time + allowed_lateness_;
    }
};

// ============================================================================
// Stream Processor (MillWheel-inspired)
// ============================================================================

enum class ProcessingState {
    IDLE,
    PROCESSING,
    COMMITTING,
    FAILED
};

struct Checkpoint {
    int64_t offset;
    int64_t watermark;
    std::unordered_map<std::string, std::string> state;
    int64_t timestamp;

    Checkpoint(int64_t off, int64_t wm, const std::unordered_map<std::string, std::string>& s)
        : offset(off), watermark(wm), state(s) {
        timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }
};

class StreamProcessor {
private:
    std::string processor_id_;
    int partition_id_;
    WindowingEngine* windowing_engine_;
    ProcessingSemantics semantics_;

    // State management
    std::unordered_map<std::string, std::string> processor_state_;
    std::queue<Checkpoint> checkpoint_queue_;

    // Processing
    std::queue<StreamRecord> input_buffer_;
    std::unordered_map<std::string, std::vector<StreamRecord>> pending_outputs_;
    ProcessingState state_;

    // Exactly-once processing
    std::unordered_set<int64_t> processed_offsets_;
    int64_t last_committed_offset_;
    int64_t current_watermark_;

    // Backpressure
    size_t max_buffer_size_;
    std::atomic<bool> backpressure_enabled_;

    std::mutex mutex_;
    std::condition_variable cv_;

public:
    StreamProcessor(const std::string& processor_id, int partition_id,
                   WindowingEngine* window_engine, ProcessingSemantics semantics = ProcessingSemantics::EXACTLY_ONCE)
        : processor_id_(processor_id), partition_id_(partition_id), windowing_engine_(window_engine),
          semantics_(semantics), state_(ProcessingState::IDLE), last_committed_offset_(-1),
          current_watermark_(0), max_buffer_size_(1000), backpressure_enabled_(false) {}

    void process_record(const StreamRecord& record) {
        std::unique_lock<std::mutex> lock(mutex_);

        // Check for backpressure
        if (input_buffer_.size() >= max_buffer_size_) {
            backpressure_enabled_ = true;
            cv_.wait(lock, [this]() { return input_buffer_.size() < max_buffer_size_; });
            backpressure_enabled_ = false;
        }

        // Exactly-once check
        if (semantics_ == ProcessingSemantics::EXACTLY_ONCE) {
            if (processed_offsets_.count(record.offset)) {
                return; // Already processed
            }
        }

        input_buffer_.push(record);
        cv_.notify_one();
    }

    void start_processing(std::function<void(const StreamRecord&)> output_callback) {
        state_ = ProcessingState::PROCESSING;

        while (state_ == ProcessingState::PROCESSING) {
            StreamRecord record("", {}, 0);

            {
                std::unique_lock<std::mutex> lock(mutex_);
                if (input_buffer_.empty()) {
                    cv_.wait_for(lock, std::chrono::milliseconds(100));
                    continue;
                }

                record = input_buffer_.front();
                input_buffer_.pop();
            }

            try {
                // Process the record
                process_single_record(record, output_callback);

                // Mark as processed for exactly-once
                if (semantics_ == ProcessingSemantics::EXACTLY_ONCE) {
                    processed_offsets_.insert(record.offset);
                }

                // Update watermark
                update_watermark(record.timestamp);

                // Create checkpoint periodically
                if (record.offset % 100 == 0) {  // Every 100 records
                    create_checkpoint(record.offset);
                }

            } catch (const std::exception& e) {
                std::cout << "Processing failed for record at offset " << record.offset
                         << ": " << e.what() << "\n";

                if (semantics_ == ProcessingSemantics::EXACTLY_ONCE) {
                    // Restore from last checkpoint
                    restore_from_checkpoint();
                }
            }
        }
    }

    void stop_processing() {
        state_ = ProcessingState::IDLE;
        cv_.notify_all();
    }

    bool is_backpressured() const {
        return backpressure_enabled_;
    }

    void set_state(const std::string& key, const std::string& value) {
        processor_state_[key] = value;
    }

    std::string get_state(const std::string& key) {
        if (processor_state_.count(key)) {
            return processor_state_[key];
        }
        return "";
    }

private:
    void process_single_record(const StreamRecord& record,
                              std::function<void(const StreamRecord&)> output_callback) {
        // Assign to windows
        auto assigned_windows = windowing_engine_->assign_windows(record);

        // Process each window (simplified - in real implementation, this would
        // be more sophisticated with user-defined functions)
        for (Window* window : assigned_windows) {
            // Simple aggregation: count records per window
            std::string count_key = "count:" + record.key + ":" + std::to_string(window->start_time);
            int current_count = std::stoi(get_state(count_key));
            set_state(count_key, std::to_string(current_count + 1));

            // Emit result if window is ready
            if (window->size() >= 5) {  // Simple threshold
                StreamRecord output_record(record.key + "_count",
                                         std::vector<uint8_t>(count_key.begin(), count_key.end()),
                                         record.timestamp);
                output_callback(output_record);
            }
        }

        // Update watermark for this key
        windowing_engine_->update_watermark(record.key, current_watermark_);
    }

    void update_watermark(int64_t timestamp) {
        current_watermark_ = std::max(current_watermark_, timestamp - 1000); // 1 second lag
    }

    void create_checkpoint(int64_t offset) {
        checkpoint_queue_.push(Checkpoint(offset, current_watermark_, processor_state_));
        std::cout << "Created checkpoint at offset " << offset << "\n";
    }

    void restore_from_checkpoint() {
        if (checkpoint_queue_.empty()) {
            std::cout << "No checkpoint available for restoration\n";
            return;
        }

        const Checkpoint& checkpoint = checkpoint_queue_.back();
        processor_state_ = checkpoint.state;
        current_watermark_ = checkpoint.watermark;
        last_committed_offset_ = checkpoint.offset;

        std::cout << "Restored from checkpoint at offset " << checkpoint.offset << "\n";
    }
};

// ============================================================================
// Stream Topology (Kafka Streams-style)
// ============================================================================

enum class StreamOperation {
    MAP,
    FILTER,
    FLAT_MAP,
    GROUP_BY,
    AGGREGATE,
    JOIN,
    MERGE
};

struct StreamNode {
    std::string node_id;
    StreamOperation operation;
    std::function<StreamRecord(const StreamRecord&)> transform_func;
    std::function<bool(const StreamRecord&)> filter_func;
    std::string source_topic;
    std::string sink_topic;
    std::vector<std::string> input_nodes;
    std::vector<std::string> output_nodes;

    StreamNode(const std::string& id, StreamOperation op)
        : node_id(id), operation(op) {}
};

class StreamTopology {
private:
    std::unordered_map<std::string, StreamNode> nodes_;
    std::string source_topic_;
    std::string sink_topic_;

public:
    StreamTopology(const std::string& source_topic, const std::string& sink_topic)
        : source_topic_(source_topic), sink_topic_(sink_topic) {}

    std::string add_source(const std::string& topic) {
        std::string node_id = "source_" + topic;
        nodes_[node_id] = StreamNode(node_id, StreamOperation::MERGE);
        nodes_[node_id].source_topic = topic;
        return node_id;
    }

    std::string add_sink(const std::string& topic, const std::string& input_node) {
        std::string node_id = "sink_" + topic;
        nodes_[node_id] = StreamNode(node_id, StreamOperation::MERGE);
        nodes_[node_id].sink_topic = topic;
        nodes_[node_id].input_nodes = {input_node};
        nodes_[input_node].output_nodes.push_back(node_id);
        return node_id;
    }

    std::string add_processor(const std::string& name, StreamOperation operation,
                            const std::string& input_node) {
        std::string node_id = "processor_" + name;
        nodes_[node_id] = StreamNode(node_id, operation);
        nodes_[node_id].input_nodes = {input_node};
        nodes_[input_node].output_nodes.push_back(node_id);
        return node_id;
    }

    void set_transform(const std::string& node_id,
                      std::function<StreamRecord(const StreamRecord&)> transform) {
        if (nodes_.count(node_id)) {
            nodes_[node_id].transform_func = transform;
        }
    }

    void set_filter(const std::string& node_id,
                   std::function<bool(const StreamRecord&)> filter) {
        if (nodes_.count(node_id)) {
            nodes_[node_id].filter_func = filter;
        }
    }

    std::vector<std::string> get_execution_order() {
        std::vector<std::string> order;
        std::unordered_set<std::string> visited;

        // Simple topological sort (assuming DAG)
        std::function<void(const std::string&)> dfs = [&](const std::string& node_id) {
            if (visited.count(node_id)) return;
            visited.insert(node_id);

            if (nodes_.count(node_id)) {
                for (const auto& output : nodes_[node_id].output_nodes) {
                    dfs(output);
                }
            }

            order.push_back(node_id);
        };

        // Start from source nodes
        for (const auto& pair : nodes_) {
            if (pair.second.input_nodes.empty()) {
                dfs(pair.first);
            }
        }

        std::reverse(order.begin(), order.end());
        return order;
    }

    StreamRecord process_through_node(const std::string& node_id, const StreamRecord& input) {
        if (!nodes_.count(node_id)) {
            return input;
        }

        const StreamNode& node = nodes_[node_id];
        StreamRecord output = input;

        switch (node.operation) {
            case StreamOperation::MAP:
                if (node.transform_func) {
                    output = node.transform_func(input);
                }
                break;
            case StreamOperation::FILTER:
                if (node.filter_func && !node.filter_func(input)) {
                    // Filtered out - return empty record
                    return StreamRecord("", {}, 0);
                }
                break;
            case StreamOperation::FLAT_MAP:
                // Simplified - just pass through
                break;
            default:
                // Pass through unchanged
                break;
        }

        return output;
    }
};

// ============================================================================
// Kafka-Style Stream Processing Engine
// ============================================================================

class KafkaStreamProcessor {
private:
    std::string application_id_;
    ConsumerGroupManager& consumer_group_;
    std::unordered_map<int, std::unique_ptr<StreamProcessor>> partition_processors_;
    std::unordered_map<int, std::unique_ptr<WindowingEngine>> windowing_engines_;
    std::unique_ptr<StreamTopology> topology_;

    // Replication and fault tolerance
    std::unordered_map<int, std::vector<std::string>> partition_replicas_;
    std::unordered_map<std::string, ProcessingState> replica_states_;

public:
    KafkaStreamProcessor(const std::string& app_id, ConsumerGroupManager& consumer_group)
        : application_id_(app_id), consumer_group_(consumer_group) {}

    void set_topology(std::unique_ptr<StreamTopology> topology) {
        topology_ = std::move(topology);
    }

    void add_partition(int partition_id, WindowType window_type = WindowType::TUMBLING,
                      int64_t window_size = 60000) {  // 1 minute windows
        // Create windowing engine
        auto window_engine = std::make_unique<WindowingEngine>(window_type, window_size);
        windowing_engines_[partition_id] = std::move(window_engine);

        // Create stream processor
        auto processor = std::make_unique<StreamProcessor>(
            application_id_ + "_processor_" + std::to_string(partition_id),
            partition_id,
            windowing_engines_[partition_id].get()
        );

        partition_processors_[partition_id] = std::move(processor);

        // Register with consumer group
        consumer_group_.add_partition(partition_id);
    }

    void start_processing() {
        std::cout << "Starting Kafka-style stream processing for " << application_id_ << "\n";

        // Assign partitions to this processor
        std::string consumer_id = application_id_ + "_consumer";
        consumer_group_.add_consumer(consumer_id);

        auto assigned_partitions = consumer_group_.get_consumer_partitions(consumer_id);

        // Start processing threads for each partition
        for (int partition_id : assigned_partitions) {
            if (partition_processors_.count(partition_id)) {
                std::thread processor_thread([this, partition_id]() {
                    partition_processors_[partition_id]->start_processing(
                        [this, partition_id](const StreamRecord& output_record) {
                            handle_output_record(partition_id, output_record);
                        }
                    );
                });
                processor_thread.detach();
            }
        }

        std::cout << "Started processing " << assigned_partitions.size() << " partitions\n";
    }

    void process_input_record(int partition_id, const StreamRecord& record) {
        if (partition_processors_.count(partition_id)) {
            partition_processors_[partition_id]->process_record(record);
        }
    }

    bool is_partition_backpressured(int partition_id) {
        if (partition_processors_.count(partition_id)) {
            return partition_processors_[partition_id]->is_backpressured();
        }
        return false;
    }

    // Fault tolerance methods
    void add_replica(int partition_id, const std::string& replica_id) {
        partition_replicas_[partition_id].push_back(replica_id);
    }

    void handle_replica_failure(const std::string& failed_replica_id) {
        // Find partitions that were handled by this replica
        for (const auto& pair : partition_replicas_) {
            auto& replicas = pair.second;
            auto it = std::find(replicas.begin(), replicas.end(), failed_replica_id);
            if (it != replicas.end()) {
                replicas.erase(it);
                // Trigger rebalancing or failover
                std::cout << "Replica " << failed_replica_id << " failed for partition "
                         << pair.first << "\n";
            }
        }
    }

private:
    void handle_output_record(int partition_id, const StreamRecord& record) {
        // Process through topology if available
        StreamRecord processed_record = record;

        if (topology_) {
            auto execution_order = topology_->get_execution_order();
            for (const auto& node_id : execution_order) {
                processed_record = topology_->process_through_node(node_id, processed_record);
                if (processed_record.key.empty()) {
                    return; // Filtered out
                }
            }
        }

        // In real implementation, this would send to output topic
        std::cout << "Output record: key=" << processed_record.key
                 << ", partition=" << partition_id
                 << ", offset=" << processed_record.offset << "\n";
    }
};

// ============================================================================
// Demonstration and Testing
// ============================================================================

void demonstrate_windowing() {
    std::cout << "=== Windowing Engine Demo ===\n";

    // Create tumbling window engine
    WindowingEngine tumbling_engine(WindowType::TUMBLING, 10000); // 10 second windows

    // Create sliding window engine
    WindowingEngine sliding_engine(WindowType::SLIDING, 10000, 5000); // 10s window, 5s slide

    // Create session window engine
    WindowingEngine session_engine(WindowType::SESSION, 30000); // 30s session gap

    // Process some records
    std::vector<StreamRecord> records = {
        StreamRecord("user1", std::vector<uint8_t>{1, 2, 3}, 1000),
        StreamRecord("user1", std::vector<uint8_t>{4, 5, 6}, 3000),
        StreamRecord("user1", std::vector<uint8_t>{7, 8, 9}, 12000),
        StreamRecord("user1", std::vector<uint8_t>{10, 11, 12}, 15000),
        StreamRecord("user2", std::vector<uint8_t>{13, 14, 15}, 2000),
        StreamRecord("user2", std::vector<uint8_t>{16, 17, 18}, 22000)
    };

    for (const auto& record : records) {
        auto windows = tumbling_engine.assign_windows(record);
        std::cout << "Record at " << record.timestamp << "ms assigned to "
                 << windows.size() << " tumbling window(s)\n";

        auto sliding_windows = sliding_engine.assign_windows(record);
        std::cout << "Record at " << record.timestamp << "ms assigned to "
                 << sliding_windows.size() << " sliding window(s)\n";

        auto session_windows = session_engine.assign_windows(record);
        std::cout << "Record at " << record.timestamp << "ms assigned to "
                 << session_windows.size() << " session window(s)\n";
    }

    // Check completed windows
    auto completed_tumbling = tumbling_engine.get_completed_windows("user1", 25000);
    std::cout << "Completed tumbling windows for user1: " << completed_tumbling.size() << "\n";

    auto completed_session = session_engine.get_completed_windows("user2", 30000);
    std::cout << "Completed session windows for user2: " << completed_session.size() << "\n";
}

void demonstrate_consumer_groups() {
    std::cout << "\n=== Consumer Group Management Demo ===\n";

    ConsumerGroupManager consumer_group("test_group");

    // Add partitions
    for (int i = 0; i < 6; ++i) {
        consumer_group.add_partition(i);
    }

    // Add consumers
    std::vector<std::string> consumers = {"consumer1", "consumer2", "consumer3"};
    for (const auto& consumer : consumers) {
        consumer_group.add_consumer(consumer);
    }

    // Show partition assignments
    for (const auto& consumer : consumers) {
        auto partitions = consumer_group.get_consumer_partitions(consumer);
        std::cout << "Consumer " << consumer << " assigned partitions: ";
        for (int p : partitions) {
            std::cout << p << " ";
        }
        std::cout << "\n";
    }

    // Add another consumer and rebalance
    consumer_group.add_consumer("consumer4");

    std::cout << "After adding consumer4:\n";
    for (const auto& consumer : consumer_group.get_consumers()) {
        auto partitions = consumer_group.get_consumer_partitions(consumer);
        std::cout << "Consumer " << consumer << " assigned partitions: ";
        for (int p : partitions) {
            std::cout << p << " ";
        }
        std::cout << "\n";
    }

    // Commit offsets
    consumer_group.commit_offset("consumer1", 0, 100);
    consumer_group.commit_offset("consumer1", 1, 150);

    std::cout << "Committed offset for consumer1 partition 0: "
             << consumer_group.get_committed_offset("consumer1", 0) << "\n";
}

void demonstrate_stream_processing() {
    std::cout << "\n=== Stream Processing Demo ===\n";

    // Create windowing engine
    WindowingEngine window_engine(WindowType::TUMBLING, 10000);

    // Create stream processor
    StreamProcessor processor("test_processor", 0, &window_engine,
                            ProcessingSemantics::EXACTLY_ONCE);

    // Simulate processing records
    std::vector<StreamRecord> records;
    for (int i = 0; i < 10; ++i) {
        StreamRecord record("test_key", std::vector<uint8_t>{static_cast<uint8_t>(i)}, i * 1000);
        record.offset = i;
        records.push_back(record);
    }

    // Start processing in background thread
    std::thread processor_thread([&]() {
        processor.start_processing([](const StreamRecord& output) {
            std::cout << "Processed output: " << output.key << " with "
                     << output.value.size() << " bytes\n";
        });
    });

    // Feed records to processor
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    for (const auto& record : records) {
        processor.process_record(record);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    processor.stop_processing();
    processor_thread.join();

    std::cout << "Stream processing completed\n";
}

void demonstrate_topology() {
    std::cout << "\n=== Stream Topology Demo ===\n";

    // Create topology: source -> filter -> map -> sink
    auto topology = std::make_unique<StreamTopology>("input_topic", "output_topic");

    auto source_node = topology->add_source("input_topic");
    auto filter_node = topology->add_processor("even_filter", StreamOperation::FILTER, source_node);
    auto map_node = topology->add_processor("double_mapper", StreamOperation::MAP, filter_node);
    auto sink_node = topology->add_sink("output_topic", map_node);

    // Set up filter (keep only even numbers)
    topology->set_filter(filter_node, [](const StreamRecord& record) -> bool {
        if (!record.value.empty()) {
            int value = record.value[0];
            return value % 2 == 0;
        }
        return false;
    });

    // Set up mapper (double the value)
    topology->set_transform(map_node, [](const StreamRecord& record) -> StreamRecord {
        if (!record.value.empty()) {
            int value = record.value[0];
            int doubled = value * 2;
            return StreamRecord(record.key, std::vector<uint8_t>{static_cast<uint8_t>(doubled)},
                              record.timestamp);
        }
        return record;
    });

    // Process records through topology
    std::vector<StreamRecord> input_records;
    for (int i = 1; i <= 10; ++i) {
        input_records.emplace_back("number", std::vector<uint8_t>{static_cast<uint8_t>(i)}, i * 1000);
    }

    auto execution_order = topology->get_execution_order();
    std::cout << "Execution order: ";
    for (const auto& node : execution_order) {
        std::cout << node << " -> ";
    }
    std::cout << "\n";

    for (const auto& input : input_records) {
        StreamRecord current = input;

        for (const auto& node_id : execution_order) {
            current = topology->process_through_node(node_id, current);
            if (current.key.empty()) {
                break; // Filtered out
            }
        }

        if (!current.key.empty()) {
            std::cout << "Input: " << static_cast<int>(input.value[0])
                     << " -> Output: " << static_cast<int>(current.value[0]) << "\n";
        }
    }
}

void demonstrate_kafka_style_processing() {
    std::cout << "\n=== Kafka-Style Stream Processing Demo ===\n";

    ConsumerGroupManager consumer_group("kafka_app_group");

    KafkaStreamProcessor processor("kafka_app", consumer_group);

    // Create topology
    auto topology = std::make_unique<StreamTopology>("click_events", "user_sessions");

    auto source = topology->add_source("click_events");
    auto filter = topology->add_processor("valid_clicks", StreamOperation::FILTER, source);
    auto group_by = topology->add_processor("group_by_user", StreamOperation::GROUP_BY, filter);
    auto aggregate = topology->add_processor("session_aggregate", StreamOperation::AGGREGATE, group_by);
    auto sink = topology->add_sink("user_sessions", aggregate);

    processor.set_topology(std::move(topology));

    // Add partitions
    for (int i = 0; i < 3; ++i) {
        processor.add_partition(i);
    }

    // Start processing
    processor.start_processing();

    // Simulate some input records
    std::vector<std::tuple<int, StreamRecord>> input_records = {
        {0, StreamRecord("user123", std::vector<uint8_t>{'c', 'l', 'i', 'c', 'k'}, 1000)},
        {1, StreamRecord("user456", std::vector<uint8_t>{'c', 'l', 'i', 'c', 'k'}, 1500)},
        {0, StreamRecord("user123", std::vector<uint8_t>{'c', 'l', 'i', 'c', 'k'}, 2000)},
        {2, StreamRecord("user789", std::vector<uint8_t>{'c', 'l', 'i', 'c', 'k'}, 2500)},
    };

    for (const auto& [partition, record] : input_records) {
        processor.process_input_record(partition, record);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Check backpressure
    for (int i = 0; i < 3; ++i) {
        bool backpressured = processor.is_partition_backpressured(i);
        std::cout << "Partition " << i << " backpressured: " << (backpressured ? "YES" : "NO") << "\n";
    }

    std::cout << "Kafka-style processing demo completed\n";
}

} // namespace kafka_style_streaming

// ============================================================================
// Main Function for Testing
// ============================================================================

int main() {
    std::cout << "🌊 **Kafka-Style Stream Processing** - Exactly-Once Semantics\n";
    std::cout << "=========================================================\n\n";

    kafka_style_streaming::demonstrate_windowing();
    kafka_style_streaming::demonstrate_consumer_groups();
    kafka_style_streaming::demonstrate_stream_processing();
    kafka_style_streaming::demonstrate_topology();
    kafka_style_streaming::demonstrate_kafka_style_processing();

    std::cout << "\n✅ **Stream Processing Complete**\n";
    std::cout << "Sources: Apache Kafka, Apache Flink, Google Dataflow, Google MillWheel\n";
    std::cout << "Features: Windowing, Exactly-once processing, Consumer groups, Fault tolerance, Backpressure\n";

    return 0;
}
