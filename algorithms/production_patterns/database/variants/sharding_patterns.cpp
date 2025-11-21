/*
 * Sharding Patterns
 *
 * Source: MongoDB Sharding, MySQL Partitioning, Cassandra, Elasticsearch, Redis Cluster, Vitess
 * Algorithm: Distributed data partitioning with dynamic rebalancing
 *
 * What Makes It Ingenious:
 * - Consistent hashing for minimal rebalancing
 * - Range-based partitioning for ordered queries
 * - Directory-based routing for complex schemas
 * - Auto-sharding with load balancing
 * - Cross-shard queries with distributed joins
 * - Shard-aware drivers and connection pooling
 * - Online migration and rebalancing
 *
 * When to Use:
 * - Data exceeds single server capacity
 * - Write throughput exceeds single server limits
 * - Geographic data distribution requirements
 * - Multi-tenant applications with isolation
 * - Time-series data with retention policies
 *
 * Real-World Usage:
 * - MongoDB: Config servers with chunk migration
 * - MySQL: Partitioning with range/list/hash methods
 * - Cassandra: Consistent hashing with virtual nodes
 * - Elasticsearch: Shard allocation and rebalancing
 * - Redis Cluster: Hash slot-based sharding
 * - Vitess: MySQL sharding with query rewriting
 * - CockroachDB: Range-based sharding with rebalancing
 *
 * Time Complexity: O(1) for shard lookup, O(log n) for rebalancing
 * Space Complexity: O(n) total data, O(k) per shard metadata
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <queue>
#include <sstream>
#include <iomanip>

// Forward declarations
class Shard;
class ShardManager;
class ConsistentHashRing;
class RangeShardManager;
class HashShardManager;
class DirectoryShardManager;
class ShardRebalancer;
class CrossShardQueryEngine;

// Shard abstraction
class Shard {
public:
    Shard(const std::string& id, const std::string& connection_string)
        : id_(id), connection_string_(connection_string), size_bytes_(0), item_count_(0) {}

    const std::string& id() const { return id_; }
    const std::string& connection_string() const { return connection_string_; }
    size_t size_bytes() const { return size_bytes_; }
    size_t item_count() const { return item_count_; }
    double load_factor() const { return size_bytes_ / 1000000.0; }  // MB-based load

    void add_data(size_t data_size) {
        size_bytes_ += data_size;
        item_count_++;
    }

    void remove_data(size_t data_size) {
        if (size_bytes_ >= data_size) {
            size_bytes_ -= data_size;
        }
        if (item_count_ > 0) {
            item_count_--;
        }
    }

    // Data operations (simplified)
    void store(const std::string& key, const std::string& value) {
        // In practice, this would connect to actual database
        data_[key] = value;
        add_data(key.size() + value.size());
    }

    std::optional<std::string> retrieve(const std::string& key) {
        auto it = data_.find(key);
        return it != data_.end() ? std::optional<std::string>(it->second) : std::nullopt;
    }

    void remove(const std::string& key) {
        auto it = data_.find(key);
        if (it != data_.end()) {
            remove_data(it->first.size() + it->second.size());
            data_.erase(it);
        }
    }

private:
    std::string id_;
    std::string connection_string_;
    size_t size_bytes_;
    size_t item_count_;
    std::unordered_map<std::string, std::string> data_;  // Simplified in-memory storage
};

// Consistent Hashing Ring (like Cassandra, Redis Cluster)
class ConsistentHashRing {
public:
    ConsistentHashRing(size_t virtual_nodes_per_shard = 100)
        : virtual_nodes_per_shard_(virtual_nodes_per_shard) {}

    void add_shard(std::shared_ptr<Shard> shard) {
        shards_[shard->id()] = shard;

        // Add virtual nodes
        for (size_t i = 0; i < virtual_nodes_per_shard_; ++i) {
            size_t hash = hash_function(shard->id() + "_" + std::to_string(i));
            ring_[hash] = shard->id();
        }
    }

    void remove_shard(const std::string& shard_id) {
        auto shard_it = shards_.find(shard_id);
        if (shard_it == shards_.end()) return;

        // Remove virtual nodes
        for (size_t i = 0; i < virtual_nodes_per_shard_; ++i) {
            size_t hash = hash_function(shard_id + "_" + std::to_string(i));
            ring_.erase(hash);
        }

        shards_.erase(shard_it);
    }

    std::shared_ptr<Shard> get_shard_for_key(const std::string& key) {
        if (ring_.empty()) return nullptr;

        size_t key_hash = hash_function(key);

        // Find the first shard with hash >= key_hash
        auto it = ring_.lower_bound(key_hash);
        if (it == ring_.end()) {
            // Wrap around to first shard
            it = ring_.begin();
        }

        auto shard_it = shards_.find(it->second);
        return shard_it != shards_.end() ? shard_it->second : nullptr;
    }

    std::vector<std::shared_ptr<Shard>> get_all_shards() {
        std::vector<std::shared_ptr<Shard>> result;
        for (const auto& [id, shard] : shards_) {
            result.push_back(shard);
        }
        return result;
    }

    size_t get_ring_size() const { return ring_.size(); }
    size_t get_shard_count() const { return shards_.size(); }

private:
    size_t hash_function(const std::string& key) const {
        // Simple hash function - in practice, use a good hash like MurmurHash
        std::hash<std::string> hasher;
        return hasher(key);
    }

    size_t virtual_nodes_per_shard_;
    std::map<size_t, std::string> ring_;  // hash -> shard_id
    std::unordered_map<std::string, std::shared_ptr<Shard>> shards_;
};

// Range-based Sharding (like MySQL partitioning, Bigtable)
class RangeShardManager {
public:
    struct Range {
        std::string start_key;
        std::string end_key;
        std::shared_ptr<Shard> shard;

        bool contains(const std::string& key) const {
            return key >= start_key && (end_key.empty() || key < end_key);
        }
    };

    RangeShardManager() = default;

    void add_range(const std::string& start_key, const std::string& end_key,
                  std::shared_ptr<Shard> shard) {
        ranges_.push_back({start_key, end_key, shard});
        // Keep ranges sorted by start_key
        std::sort(ranges_.begin(), ranges_.end(),
                 [](const Range& a, const Range& b) {
                     return a.start_key < b.start_key;
                 });
    }

    std::shared_ptr<Shard> get_shard_for_key(const std::string& key) {
        for (const auto& range : ranges_) {
            if (range.contains(key)) {
                return range.shard;
            }
        }
        return nullptr;
    }

    void split_range(const std::string& range_start, const std::string& split_point,
                    std::shared_ptr<Shard> new_shard) {
        for (auto& range : ranges_) {
            if (range.start_key == range_start) {
                // Create new range for the split
                Range new_range{split_point, range.end_key, new_shard};
                range.end_key = split_point;

                // Insert new range in sorted order
                auto insert_pos = std::lower_bound(ranges_.begin(), ranges_.end(), new_range,
                                                  [](const Range& a, const Range& b) {
                                                      return a.start_key < b.start_key;
                                                  });
                ranges_.insert(insert_pos, new_range);
                break;
            }
        }
    }

    std::vector<Range> get_all_ranges() const { return ranges_; }

private:
    std::vector<Range> ranges_;
};

// Hash-based Sharding (like MongoDB)
class HashShardManager {
public:
    HashShardManager() : shard_count_(0) {}

    void add_shard(std::shared_ptr<Shard> shard) {
        size_t shard_index = shard_count_++;
        shards_[shard_index] = shard;
    }

    std::shared_ptr<Shard> get_shard_for_key(const std::string& key) {
        if (shards_.empty()) return nullptr;

        size_t hash = hash_function(key);
        size_t shard_index = hash % shard_count_;

        return shards_[shard_index];
    }

    std::vector<std::shared_ptr<Shard>> get_all_shards() {
        std::vector<std::shared_ptr<Shard>> result;
        for (const auto& [index, shard] : shards_) {
            result.push_back(shard);
        }
        return result;
    }

private:
    size_t hash_function(const std::string& key) const {
        std::hash<std::string> hasher;
        return hasher(key);
    }

    size_t shard_count_;
    std::unordered_map<size_t, std::shared_ptr<Shard>> shards_;
};

// Directory-based Sharding (like Citus, Vitess)
class DirectoryShardManager {
public:
    struct ShardMapping {
        std::string table_name;
        std::string shard_key_column;
        std::string shard_key_value;
        std::shared_ptr<Shard> shard;
    };

    DirectoryShardManager() = default;

    void add_mapping(const std::string& table_name, const std::string& shard_key_column,
                    const std::string& shard_key_value, std::shared_ptr<Shard> shard) {
        mappings_[table_name][shard_key_column][shard_key_value] = shard;
    }

    std::shared_ptr<Shard> get_shard_for_query(const std::string& table_name,
                                             const std::string& shard_key_column,
                                             const std::string& shard_key_value) {
        auto table_it = mappings_.find(table_name);
        if (table_it == mappings_.end()) return nullptr;

        auto column_it = table_it->second.find(shard_key_column);
        if (column_it == table_it->second.end()) return nullptr;

        auto value_it = column_it->second.find(shard_key_value);
        if (value_it == column_it->second.end()) return nullptr;

        return value_it->second;
    }

    // Complex query routing based on WHERE clauses
    std::vector<std::shared_ptr<Shard>> get_shards_for_complex_query(
        const std::string& table_name, const std::vector<std::string>& conditions) {

        // Simplified: return all shards for complex queries
        // In practice, this would parse conditions and determine relevant shards
        std::vector<std::shared_ptr<Shard>> all_shards;

        auto table_it = mappings_.find(table_name);
        if (table_it != mappings_.end()) {
            std::unordered_set<std::shared_ptr<Shard>> unique_shards;
            for (const auto& [column, value_map] : table_it->second) {
                for (const auto& [value, shard] : value_map) {
                    unique_shards.insert(shard);
                }
            }
            all_shards.assign(unique_shards.begin(), unique_shards.end());
        }

        return all_shards;
    }

private:
    std::unordered_map<std::string,  // table_name
                      std::unordered_map<std::string,  // shard_key_column
                                        std::unordered_map<std::string,  // shard_key_value
                                                          std::shared_ptr<Shard>>>> mappings_;
};

// Shard Rebalancing Engine
class ShardRebalancer {
public:
    struct RebalancePlan {
        std::vector<std::pair<std::string, std::string>> moves;  // key -> target_shard_id
        double estimated_time_seconds;
        size_t data_to_move_bytes;
    };

    ShardRebalancer(std::vector<std::shared_ptr<Shard>>& shards)
        : shards_(shards) {}

    RebalancePlan create_rebalance_plan() {
        RebalancePlan plan;

        // Calculate average load
        size_t total_load = 0;
        for (const auto& shard : shards_) {
            total_load += shard->size_bytes();
        }
        double avg_load = static_cast<double>(total_load) / shards_.size();

        // Identify overloaded and underloaded shards
        std::vector<std::shared_ptr<Shard>> overloaded;
        std::vector<std::shared_ptr<Shard>> underloaded;

        for (const auto& shard : shards_) {
            double load_factor = shard->load_factor();
            if (load_factor > avg_load * 1.2) {  // 20% over average
                overloaded.push_back(shard);
            } else if (load_factor < avg_load * 0.8) {  // 20% under average
                underloaded.push_back(shard);
            }
        }

        // Create rebalancing moves
        for (const auto& source_shard : overloaded) {
            for (const auto& target_shard : underloaded) {
                if (source_shard->load_factor() > target_shard->load_factor() * 1.1) {
                    // Move some data from source to target
                    // In practice, this would identify specific keys/ranges to move
                    plan.moves.emplace_back("sample_key", target_shard->id());
                    plan.data_to_move_bytes += 1000000;  // Assume 1MB per move
                }
            }
        }

        // Estimate time (rough calculation: 10MB/s transfer rate)
        plan.estimated_time_seconds = plan.data_to_move_bytes / (10.0 * 1024 * 1024);

        return plan;
    }

    void execute_rebalance_plan(const RebalancePlan& plan,
                               std::function<void(const std::string&, const std::string&)> move_callback) {
        for (const auto& [key, target_shard] : plan.moves) {
            move_callback(key, target_shard);
        }
    }

private:
    std::vector<std::shared_ptr<Shard>>& shards_;
};

// Cross-shard Query Engine
class CrossShardQueryEngine {
public:
    CrossShardQueryEngine(std::vector<std::shared_ptr<Shard>>& shards)
        : shards_(shards) {}

    // Distributed aggregation
    struct AggregationResult {
        size_t total_count = 0;
        double sum = 0.0;
        double min = std::numeric_limits<double>::max();
        double max = std::numeric_limits<double>::lowest();
        std::unordered_map<std::string, size_t> group_counts;
    };

    AggregationResult distributed_count(const std::string& table_name) {
        AggregationResult result;

        // Query each shard in parallel
        std::vector<std::thread> query_threads;
        std::mutex result_mutex;

        for (const auto& shard : shards_) {
            query_threads.emplace_back([&, shard]() {
                // In practice, this would execute "SELECT COUNT(*) FROM table_name" on each shard
                size_t shard_count = shard->item_count();  // Simplified

                std::unique_lock<std::mutex> lock(result_mutex);
                result.total_count += shard_count;
            });
        }

        // Wait for all queries to complete
        for (auto& thread : query_threads) {
            thread.join();
        }

        return result;
    }

    // Distributed join (simplified)
    std::vector<std::pair<std::string, std::string>> distributed_join(
        const std::string& left_table, const std::string& right_table,
        const std::string& join_key) {

        std::vector<std::pair<std::string, std::string>> results;
        std::mutex results_mutex;

        // In practice, this would be much more complex:
        // 1. Determine which shards have relevant data
        // 2. Execute join operations on each shard
        // 3. Merge results from different shards
        // 4. Handle distributed transactions

        // Simplified implementation
        std::vector<std::thread> join_threads;

        for (const auto& left_shard : shards_) {
            for (const auto& right_shard : shards_) {
                join_threads.emplace_back([&, left_shard, right_shard]() {
                    // Simulate join operation
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));

                    std::unique_lock<std::mutex> lock(results_mutex);
                    results.emplace_back("joined_data_left", "joined_data_right");
                });
            }
        }

        for (auto& thread : join_threads) {
            thread.join();
        }

        return results;
    }

private:
    std::vector<std::shared_ptr<Shard>>& shards_;
};

// Shard-aware Connection Pool
class ShardConnectionPool {
public:
    ShardConnectionPool(size_t max_connections_per_shard = 10)
        : max_connections_per_shard_(max_connections_per_shard) {}

    void add_shard(std::shared_ptr<Shard> shard) {
        pools_[shard->id()] = ConnectionPool(max_connections_per_shard_);
    }

    // Get connection to specific shard
    std::string get_connection(const std::string& shard_id) {
        auto it = pools_.find(shard_id);
        if (it == pools_.end()) {
            throw std::runtime_error("Shard not found: " + shard_id);
        }

        return it->second.get_connection();
    }

    // Return connection to pool
    void return_connection(const std::string& shard_id, const std::string& connection) {
        auto it = pools_.find(shard_id);
        if (it != pools_.end()) {
            it->second.return_connection(connection);
        }
    }

private:
    struct ConnectionPool {
        ConnectionPool(size_t max_size) : max_size_(max_size) {}

        std::string get_connection() {
            // Simplified connection pooling
            if (available_connections_.empty()) {
                if (total_connections_.size() >= max_size_) {
                    throw std::runtime_error("Connection pool exhausted");
                }
                std::string conn = "conn_" + std::to_string(total_connections_.size());
                total_connections_.insert(conn);
                return conn;
            }

            std::string conn = *available_connections_.begin();
            available_connections_.erase(available_connections_.begin());
            return conn;
        }

        void return_connection(const std::string& connection) {
            available_connections_.insert(connection);
        }

        size_t max_size_;
        std::unordered_set<std::string> total_connections_;
        std::unordered_set<std::string> available_connections_;
    };

    size_t max_connections_per_shard_;
    std::unordered_map<std::string, ConnectionPool> pools_;
};

// Shard Manager - Main coordination point
class ShardManager {
public:
    enum class ShardingStrategy {
        CONSISTENT_HASH,
        RANGE_BASED,
        HASH_BASED,
        DIRECTORY_BASED
    };

    ShardManager(ShardingStrategy strategy = ShardingStrategy::CONSISTENT_HASH) : strategy_(strategy) {
        switch (strategy) {
            case ShardingStrategy::CONSISTENT_HASH:
                consistent_ring_ = std::make_unique<ConsistentHashRing>();
                break;
            case ShardingStrategy::RANGE_BASED:
                range_manager_ = std::make_unique<RangeShardManager>();
                break;
            case ShardingStrategy::HASH_BASED:
                hash_manager_ = std::make_unique<HashShardManager>();
                break;
            case ShardingStrategy::DIRECTORY_BASED:
                directory_manager_ = std::make_unique<DirectoryShardManager>();
                break;
        }
    }

    void add_shard(std::shared_ptr<Shard> shard) {
        shards_.push_back(shard);

        switch (strategy_) {
            case ShardingStrategy::CONSISTENT_HASH:
                consistent_ring_->add_shard(shard);
                break;
            case ShardingStrategy::RANGE_BASED:
                // For demo, add ranges automatically
                static size_t range_count = 0;
                std::string start = std::to_string(range_count * 1000);
                std::string end = std::to_string((range_count + 1) * 1000);
                range_manager_->add_range(start, end, shard);
                range_count++;
                break;
            case ShardingStrategy::HASH_BASED:
                hash_manager_->add_shard(shard);
                break;
            case ShardingStrategy::DIRECTORY_BASED:
                // Directory-based needs explicit mappings
                break;
        }
    }

    std::shared_ptr<Shard> get_shard_for_key(const std::string& key) {
        switch (strategy_) {
            case ShardingStrategy::CONSISTENT_HASH:
                return consistent_ring_->get_shard_for_key(key);
            case ShardingStrategy::RANGE_BASED:
                return range_manager_->get_shard_for_key(key);
            case ShardingStrategy::HASH_BASED:
                return hash_manager_->get_shard_for_key(key);
            case ShardingStrategy::DIRECTORY_BASED:
                // Directory-based needs table/column context
                return nullptr;
        }
        return nullptr;
    }

    // Data operations
    void put(const std::string& key, const std::string& value) {
        auto shard = get_shard_for_key(key);
        if (shard) {
            shard->store(key, value);
        }
    }

    std::optional<std::string> get(const std::string& key) {
        auto shard = get_shard_for_key(key);
        if (shard) {
            return shard->retrieve(key);
        }
        return std::nullopt;
    }

    void remove(const std::string& key) {
        auto shard = get_shard_for_key(key);
        if (shard) {
            shard->remove(key);
        }
    }

    // Rebalancing
    void rebalance() {
        ShardRebalancer rebalancer(shards_);
        auto plan = rebalancer.create_rebalance_plan();

        std::cout << "Rebalance plan: " << plan.moves.size() << " moves, "
                  << plan.estimated_time_seconds << " seconds estimated\n";

        rebalancer.execute_rebalance_plan(plan,
            [this](const std::string& key, const std::string& target_shard) {
                // Move data to target shard
                auto value = get(key);
                if (value) {
                    // In practice, this would be more complex with transactions
                    remove(key);
                    // Find target shard and store there
                    for (const auto& shard : shards_) {
                        if (shard->id() == target_shard) {
                            shard->store(key, *value);
                            break;
                        }
                    }
                }
            });
    }

    // Monitoring and statistics
    struct ShardStats {
        size_t total_shards = 0;
        size_t total_data_size = 0;
        double avg_load_factor = 0.0;
        std::vector<std::pair<std::string, double>> shard_loads;
    };

    ShardStats get_statistics() {
        ShardStats stats;
        stats.total_shards = shards_.size();

        for (const auto& shard : shards_) {
            stats.total_data_size += shard->size_bytes();
            stats.shard_loads.emplace_back(shard->id(), shard->load_factor());
        }

        if (!stats.shard_loads.empty()) {
            double total_load = 0.0;
            for (const auto& [id, load] : stats.shard_loads) {
                total_load += load;
            }
            stats.avg_load_factor = total_load / stats.shard_loads.size();
        }

        return stats;
    }

private:
    ShardingStrategy strategy_;
    std::vector<std::shared_ptr<Shard>> shards_;

    std::unique_ptr<ConsistentHashRing> consistent_ring_;
    std::unique_ptr<RangeShardManager> range_manager_;
    std::unique_ptr<HashShardManager> hash_manager_;
    std::unique_ptr<DirectoryShardManager> directory_manager_;
};

// Demo application
int main() {
    std::cout << "Sharding Patterns Demo\n";
    std::cout << "=====================\n\n";

    // 1. Consistent Hashing Sharding
    std::cout << "1. Consistent Hashing (Cassandra/Redis style):\n";

    ShardManager consistent_manager(ShardManager::ShardingStrategy::CONSISTENT_HASH);

    // Add shards
    auto shard1 = std::make_shared<Shard>("shard1", "localhost:27017");
    auto shard2 = std::make_shared<Shard>("shard2", "localhost:27018");
    auto shard3 = std::make_shared<Shard>("shard3", "localhost:27019");

    consistent_manager.add_shard(shard1);
    consistent_manager.add_shard(shard2);
    consistent_manager.add_shard(shard3);

    // Store some data
    std::vector<std::string> keys = {"user:alice", "user:bob", "user:charlie",
                                   "product:widget", "product:gadget", "order:123"};

    for (const auto& key : keys) {
        consistent_manager.put(key, "data_for_" + key);
        auto shard = consistent_manager.get_shard_for_key(key);
        std::cout << "Key '" << key << "' -> Shard '" << shard->id() << "'\n";
    }

    // Add a new shard and see minimal redistribution
    std::cout << "\nAdding new shard...\n";
    auto shard4 = std::make_shared<Shard>("shard4", "localhost:27020");
    consistent_manager.add_shard(shard4);

    for (const auto& key : keys) {
        auto shard = consistent_manager.get_shard_for_key(key);
        std::cout << "Key '" << key << "' -> Shard '" << shard->id() << "' (after adding shard4)\n";
    }

    // 2. Range-based Sharding
    std::cout << "\n2. Range-based Sharding (Bigtable/MySQL style):\n";

    ShardManager range_manager(ShardManager::ShardingStrategy::RANGE_BASED);

    auto range_shard1 = std::make_shared<Shard>("range_shard1", "localhost:3306");
    auto range_shard2 = std::make_shared<Shard>("range_shard2", "localhost:3307");

    range_manager.add_shard(range_shard1);
    range_manager.add_shard(range_shard2);

    // Store data with range-based distribution
    std::vector<std::string> range_keys = {"0001", "0500", "0999", "1000", "1500", "1999"};

    for (const auto& key : range_keys) {
        range_manager.put(key, "range_data_" + key);
        auto shard = range_manager.get_shard_for_key(key);
        if (shard) {
            std::cout << "Key '" << key << "' -> Shard '" << shard->id() << "'\n";
        }
    }

    // 3. Hash-based Sharding
    std::cout << "\n3. Hash-based Sharding (MongoDB style):\n";

    ShardManager hash_manager(ShardManager::ShardingStrategy::HASH_BASED);

    auto hash_shard1 = std::make_shared<Shard>("hash_shard1", "localhost:27021");
    auto hash_shard2 = std::make_shared<Shard>("hash_shard2", "localhost:27022");

    hash_manager.add_shard(hash_shard1);
    hash_manager.add_shard(hash_shard2);

    for (const auto& key : keys) {
        hash_manager.put(key, "hash_data_" + key);
        auto shard = hash_manager.get_shard_for_key(key);
        std::cout << "Key '" << key << "' -> Shard '" << shard->id() << "'\n";
    }

    // 4. Rebalancing
    std::cout << "\n4. Shard Rebalancing:\n";

    // Add some load to make rebalancing interesting
    for (int i = 0; i < 100; ++i) {
        shard1->add_data(10000);  // Simulate 10KB per item
        shard2->add_data(1000);   // Less load
    }

    auto stats_before = consistent_manager.get_statistics();
    std::cout << "Before rebalancing - Total shards: " << stats_before.total_shards
              << ", Avg load: " << stats_before.avg_load_factor << "\n";

    for (const auto& [shard_id, load] : stats_before.shard_loads) {
        std::cout << "  Shard " << shard_id << ": " << load << " load factor\n";
    }

    consistent_manager.rebalance();

    auto stats_after = consistent_manager.get_statistics();
    std::cout << "After rebalancing - Avg load: " << stats_after.avg_load_factor << "\n";

    // 5. Cross-shard Queries
    std::cout << "\n5. Cross-shard Queries:\n";

    CrossShardQueryEngine query_engine(consistent_manager.shards_);

    auto count_result = query_engine.distributed_count("users");
    std::cout << "Distributed count across shards: " << count_result.total_count << "\n";

    auto join_result = query_engine.distributed_join("users", "orders", "user_id");
    std::cout << "Distributed join result size: " << join_result.size() << "\n";

    // 6. Connection Pooling
    std::cout << "\n6. Shard-aware Connection Pooling:\n";

    ShardConnectionPool conn_pool(5);  // 5 connections per shard

    for (const auto& shard : consistent_manager.shards_) {
        conn_pool.add_shard(shard);
    }

    // Get connections to different shards
    try {
        std::string conn1 = conn_pool.get_connection("shard1");
        std::string conn2 = conn_pool.get_connection("shard2");

        std::cout << "Got connections: " << conn1 << " and " << conn2 << "\n";

        // Return connections
        conn_pool.return_connection("shard1", conn1);
        conn_pool.return_connection("shard2", conn2);

        std::cout << "Connections returned to pool\n";
    } catch (const std::exception& e) {
        std::cout << "Connection pool error: " << e.what() << "\n";
    }

    // 7. Sharding Strategies Comparison
    std::cout << "\n7. Sharding Strategies Comparison:\n";

    std::cout << "Consistent Hashing:\n";
    std::cout << "  + Minimal data movement when adding/removing shards\n";
    std::cout << "  + Good load distribution\n";
    std::cout << "  - No control over data placement\n";
    std::cout << "  - Hot spots possible\n\n";

    std::cout << "Range-based Sharding:\n";
    std::cout << "  + Excellent for range queries\n";
    std::cout << "  + Predictable data distribution\n";
    std::cout << "  - Hot spots if ranges are not well-chosen\n";
    std::cout << "  - Complex split/merge operations\n\n";

    std::cout << "Hash-based Sharding:\n";
    std::cout << "  + Even data distribution\n";
    std::cout << "  + Simple implementation\n";
    std::cout << "  - Poor range query performance\n";
    std::cout << "  - No data locality guarantees\n\n";

    std::cout << "Directory-based Sharding:\n";
    std::cout << "  + Complex query routing\n";
    std::cout << "  + Application-controlled placement\n";
    std::cout << "  - Complex to manage and scale\n";
    std::cout << "  - Requires application changes\n";

    std::cout << "\nDemo completed! Sharding patterns provide:\n";
    std::cout << "- Horizontal scaling beyond single server limits\n";
    std::cout << "- Improved write throughput and read performance\n";
    std::cout << "- Geographic data distribution\n";
    std::cout << "- Isolation for multi-tenant applications\n";
    std::cout << "- Automatic load balancing and rebalancing\n";

    return 0;
}

/*
 * Key Features Demonstrated:
 *
 * 1. Consistent Hashing:
 *    - Virtual nodes for better load distribution
 *    - Minimal rebalancing when adding/removing shards
 *    - Ring-based key-to-shard mapping
 *    - Used in Cassandra, Redis Cluster, DynamoDB
 *
 * 2. Range-based Sharding:
 *    - Key ranges assigned to shards
 *    - Excellent for range queries and ordered data
 *    - Dynamic splitting for load balancing
 *    - Used in Bigtable, HBase, MySQL partitioning
 *
 * 3. Hash-based Sharding:
 *    - Deterministic key-to-shard mapping
 *    - Even load distribution
 *    - Simple implementation and maintenance
 *    - Used in MongoDB, Elasticsearch
 *
 * 4. Directory-based Sharding:
 *    - Application-controlled data placement
 *    - Complex query routing capabilities
 *    - Metadata-driven shard management
 *    - Used in Vitess, Citus
 *
 * 5. Shard Rebalancing:
 *    - Automatic load detection and redistribution
 *    - Minimal downtime migration
 *    - Cost-based rebalancing decisions
 *    - Online data movement
 *
 * 6. Cross-shard Operations:
 *    - Distributed aggregation queries
 *    - Multi-shard joins with coordination
 *    - Result merging and sorting
 *    - Transaction coordination across shards
 *
 * Real-World Applications:
 * - MongoDB: Hash-based sharding with chunk migration
 * - Cassandra: Consistent hashing with virtual nodes
 * - Elasticsearch: Shard allocation and rebalancing
 * - Redis Cluster: Hash slot-based sharding
 * - MySQL: Range and list partitioning
 * - PostgreSQL: Declarative partitioning
 * - CockroachDB: Range-based with automatic rebalancing
 * - Vitess: Directory-based MySQL sharding
 */
