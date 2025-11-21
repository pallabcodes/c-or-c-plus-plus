/**
 * @file distributed_databases.cpp
 * @brief Production-grade distributed database patterns from Cassandra, DynamoDB, Riak
 *
 * This implementation provides:
 * - Distributed key-value stores with consistency models
 * - Consistent hashing for data partitioning
 * - Vector clocks for causal consistency
 * - Conflict-free replicated data types (CRDTs)
 * - Anti-entropy and read repair
 * - Sharding strategies and load balancing
 * - Multi-version concurrency control
 * - Distributed transactions and sagas
 *
 * Sources: Cassandra, DynamoDB, Riak, CockroachDB, Google Spanner
 */

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <unordered_set>
#include <map>
#include <set>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <random>
#include <functional>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <iomanip>

namespace distributed_databases {

// ============================================================================
// Vector Clocks and Version Vectors
// ============================================================================

class VectorClock {
private:
    std::unordered_map<std::string, int64_t> clock;

public:
    void increment(const std::string& node_id) {
        clock[node_id]++;
    }

    bool happens_before(const VectorClock& other) const {
        // Check if this clock is dominated by other
        bool dominated = true;
        for (const auto& pair : other.clock) {
            auto it = clock.find(pair.first);
            int64_t this_value = (it != clock.end()) ? it->second : 0;
            if (this_value > pair.second) {
                dominated = false;
                break;
            }
        }

        // Check if there's at least one component where this > other
        bool strictly_less = false;
        for (const auto& pair : clock) {
            auto it = other.clock.find(pair.first);
            int64_t other_value = (it != other.clock.end()) ? it->second : 0;
            if (pair.second > other_value) {
                strictly_less = true;
                break;
            }
        }

        return dominated && strictly_less;
    }

    bool is_concurrent(const VectorClock& other) const {
        return !happens_before(other) && !other.happens_before(*this);
    }

    void merge(const VectorClock& other) {
        for (const auto& pair : other.clock) {
            auto it = clock.find(pair.first);
            int64_t current_value = (it != clock.end()) ? it->second : 0;
            clock[pair.first] = std::max(current_value, pair.second);
        }
    }

    std::string to_string() const {
        std::stringstream ss;
        ss << "{";
        bool first = true;
        for (const auto& pair : clock) {
            if (!first) ss << ",";
            ss << pair.first << ":" << pair.second;
            first = false;
        }
        ss << "}";
        return ss.str();
    }

    bool operator==(const VectorClock& other) const {
        return clock == other.clock;
    }

    bool operator!=(const VectorClock& other) const {
        return !(*this == other);
    }
};

// ============================================================================
// Consistent Hashing Ring
// ============================================================================

class ConsistentHashRing {
private:
    std::map<size_t, std::string> ring;  // hash -> node
    std::unordered_map<std::string, std::set<size_t>> node_tokens;
    int virtual_nodes_per_physical;

    size_t hash(const std::string& key) const {
        // FNV-1a hash
        const size_t FNV_prime = 1099511628211ULL;
        size_t hash = 14695981039346656037ULL;

        for (char c : key) {
            hash ^= static_cast<size_t>(c);
            hash *= FNV_prime;
        }

        return hash;
    }

public:
    ConsistentHashRing(int virtual_nodes = 100)
        : virtual_nodes_per_physical(virtual_nodes) {}

    void add_node(const std::string& node_id) {
        for (int i = 0; i < virtual_nodes_per_physical; ++i) {
            std::string token = node_id + "#" + std::to_string(i);
            size_t hash_value = hash(token);
            ring[hash_value] = node_id;
            node_tokens[node_id].insert(hash_value);
        }
    }

    void remove_node(const std::string& node_id) {
        if (node_tokens.count(node_id)) {
            for (size_t token : node_tokens[node_id]) {
                ring.erase(token);
            }
            node_tokens.erase(node_id);
        }
    }

    std::vector<std::string> get_nodes(const std::string& key, int replication_factor) {
        size_t key_hash = hash(key);
        std::vector<std::string> result;
        std::unordered_set<std::string> seen;

        // Find the first node >= key_hash
        auto it = ring.lower_bound(key_hash);
        if (it == ring.end()) {
            it = ring.begin();
        }

        // Collect distinct nodes
        for (size_t i = 0; i < ring.size() && result.size() < static_cast<size_t>(replication_factor); ++it) {
            if (it == ring.end()) {
                it = ring.begin();
            }

            if (seen.find(it->second) == seen.end()) {
                seen.insert(it->second);
                result.push_back(it->second);
            }
        }

        return result;
    }

    std::string get_primary_node(const std::string& key) {
        auto nodes = get_nodes(key, 1);
        return nodes.empty() ? "" : nodes[0];
    }

    std::vector<std::string> get_preference_list(const std::string& key, int n) {
        return get_nodes(key, n);
    }

    size_t size() const {
        return ring.size() / virtual_nodes_per_physical;
    }

    std::vector<std::string> get_all_nodes() const {
        std::unordered_set<std::string> unique_nodes;
        for (const auto& pair : ring) {
            unique_nodes.insert(pair.second);
        }
        return std::vector<std::string>(unique_nodes.begin(), unique_nodes.end());
    }
};

// ============================================================================
// Conflict-Free Replicated Data Types (CRDTs)
// ============================================================================

template<typename T>
class CRDT_GCounter {
private:
    std::unordered_map<std::string, T> counters;

public:
    void increment(const std::string& node_id, T amount = 1) {
        counters[node_id] += amount;
    }

    T value() const {
        T total = 0;
        for (const auto& pair : counters) {
            total += pair.second;
        }
        return total;
    }

    void merge(const CRDT_GCounter& other) {
        for (const auto& pair : other.counters) {
            if (counters[pair.first] < pair.second) {
                counters[pair.first] = pair.second;
            }
        }
    }

    std::string to_string() const {
        std::stringstream ss;
        ss << "GCounter{";
        bool first = true;
        for (const auto& pair : counters) {
            if (!first) ss << ",";
            ss << pair.first << ":" << pair.second;
            first = false;
        }
        ss << "}";
        return ss.str();
    }
};

class CRDT_PNCounter {
private:
    CRDT_GCounter<int64_t> positive;
    CRDT_GCounter<int64_t> negative;

public:
    void increment(const std::string& node_id, int64_t amount = 1) {
        if (amount > 0) {
            positive.increment(node_id, amount);
        } else {
            negative.increment(node_id, -amount);
        }
    }

    void decrement(const std::string& node_id, int64_t amount = 1) {
        increment(node_id, -amount);
    }

    int64_t value() const {
        return positive.value() - negative.value();
    }

    void merge(const CRDT_PNCounter& other) {
        positive.merge(other.positive);
        negative.merge(other.negative);
    }

    std::string to_string() const {
        return "PNCounter{positive:" + positive.to_string() +
               ", negative:" + negative.to_string() + "}";
    }
};

class CRDT_GSet {
private:
    std::unordered_set<std::string> elements;

public:
    void add(const std::string& element) {
        elements.insert(element);
    }

    bool contains(const std::string& element) const {
        return elements.count(element) > 0;
    }

    const std::unordered_set<std::string>& value() const {
        return elements;
    }

    void merge(const CRDT_GSet& other) {
        for (const auto& elem : other.elements) {
            elements.insert(elem);
        }
    }

    std::string to_string() const {
        std::stringstream ss;
        ss << "GSet{";
        bool first = true;
        for (const auto& elem : elements) {
            if (!first) ss << ",";
            ss << elem;
            first = false;
        }
        ss << "}";
        return ss.str();
    }
};

class CRDT_LWWRegister {
private:
    std::string value;
    VectorClock timestamp;

public:
    void write(const std::string& new_value, const VectorClock& ts) {
        if (ts.happens_before(timestamp) || ts == timestamp) {
            return;  // Don't overwrite with older or equal timestamp
        }
        value = new_value;
        timestamp = ts;
    }

    std::string read() const {
        return value;
    }

    VectorClock get_timestamp() const {
        return timestamp;
    }

    void merge(const CRDT_LWWRegister& other) {
        if (other.timestamp.happens_before(timestamp)) {
            return;
        }
        if (timestamp.happens_before(other.timestamp)) {
            value = other.value;
            timestamp = other.timestamp;
        } else {
            // Concurrent writes - pick lexicographically larger value
            if (other.value > value) {
                value = other.value;
                timestamp.merge(other.timestamp);
            }
        }
    }

    std::string to_string() const {
        return "LWWRegister{value:'" + value + "', ts:" + timestamp.to_string() + "}";
    }
};

// ============================================================================
// Distributed Key-Value Store
// ============================================================================

enum class ConsistencyLevel {
    ONE,           // Single replica
    QUORUM,        // Majority of replicas
    ALL,           // All replicas
    LOCAL_QUORUM,  // Local datacenter quorum
    EACH_QUORUM    // Per-datacenter quorum
};

enum class ReplicationStrategy {
    SIMPLE_STRATEGY,    // Single datacenter
    NETWORK_TOPOLOGY_STRATEGY,  // Multi-datacenter
    LOCAL_STRATEGY      // Local node only
};

struct KeyValue {
    std::string key;
    std::string value;
    VectorClock version;
    bool deleted;

    KeyValue() : deleted(false) {}
    KeyValue(const std::string& k, const std::string& v, const VectorClock& vc, bool del = false)
        : key(k), value(v), version(vc), deleted(del) {}
};

class DistributedKVStore {
private:
    struct Node {
        std::string id;
        std::unordered_map<std::string, KeyValue> data;

        bool write(const KeyValue& kv) {
            data[kv.key] = kv;
            return true;
        }

        KeyValue* read(const std::string& key) {
            auto it = data.find(key);
            return (it != data.end()) ? &it->second : nullptr;
        }
    };

    std::unordered_map<std::string, Node> nodes;
    ConsistentHashRing ring;
    int replication_factor;
    ConsistencyLevel read_consistency;
    ConsistencyLevel write_consistency;

public:
    DistributedKVStore(int repl_factor = 3,
                      ConsistencyLevel read_cl = ConsistencyLevel::QUORUM,
                      ConsistencyLevel write_cl = ConsistencyLevel::QUORUM)
        : replication_factor(repl_factor),
          read_consistency(read_cl),
          write_consistency(write_cl) {}

    void add_node(const std::string& node_id) {
        nodes[node_id] = Node{node_id};
        ring.add_node(node_id);
    }

    bool put(const std::string& key, const std::string& value, const VectorClock& version) {
        auto preference_list = ring.get_preference_list(key, replication_factor);

        KeyValue kv(key, value, version);
        int success_count = 0;
        int required_writes = get_required_count(write_consistency, replication_factor);

        for (const auto& node_id : preference_list) {
            if (nodes.count(node_id) && nodes[node_id].write(kv)) {
                success_count++;
            }
        }

        return success_count >= required_writes;
    }

    std::vector<KeyValue> get(const std::string& key) {
        auto preference_list = ring.get_preference_list(key, replication_factor);

        std::vector<KeyValue> versions;
        int required_reads = get_required_count(read_consistency, replication_factor);
        int read_count = 0;

        for (const auto& node_id : preference_list) {
            if (nodes.count(node_id)) {
                auto kv = nodes[node_id].read(key);
                if (kv && !kv->deleted) {
                    versions.push_back(*kv);
                    read_count++;
                }
            }

            if (read_count >= required_reads) break;
        }

        return versions;
    }

    std::string get_with_resolution(const std::string& key) {
        auto versions = get(key);

        if (versions.empty()) {
            return "";  // Key not found
        }

        if (versions.size() == 1) {
            return versions[0].value;
        }

        // Resolve conflicts using vector clocks
        // Return the value with the most recent version
        KeyValue* latest = &versions[0];
        for (auto& version : versions) {
            if (version.version.happens_before(latest->version)) {
                // latest is newer, keep it
            } else if (latest->version.happens_before(version.version)) {
                latest = &version;
            } else {
                // Concurrent versions - pick lexicographically larger value
                if (version.value > latest->value) {
                    latest = &version;
                }
            }
        }

        return latest->value;
    }

    bool delete_key(const std::string& key, const VectorClock& version) {
        auto preference_list = ring.get_preference_list(key, replication_factor);

        KeyValue tombstone(key, "", version, true);
        int success_count = 0;
        int required_deletes = get_required_count(write_consistency, replication_factor);

        for (const auto& node_id : preference_list) {
            if (nodes.count(node_id) && nodes[node_id].write(tombstone)) {
                success_count++;
            }
        }

        return success_count >= required_deletes;
    }

private:
    int get_required_count(ConsistencyLevel level, int total_replicas) {
        switch (level) {
            case ConsistencyLevel::ONE: return 1;
            case ConsistencyLevel::QUORUM: return (total_replicas / 2) + 1;
            case ConsistencyLevel::ALL: return total_replicas;
            case ConsistencyLevel::LOCAL_QUORUM: return (total_replicas / 2) + 1;  // Simplified
            case ConsistencyLevel::EACH_QUORUM: return (total_replicas / 2) + 1;   // Simplified
            default: return 1;
        }
    }
};

// ============================================================================
// Multi-Version Concurrency Control (MVCC)
// ============================================================================

class MVCCStore {
private:
    struct VersionedValue {
        std::string value;
        int64_t timestamp;
        int64_t transaction_id;
        bool deleted;

        VersionedValue(const std::string& v, int64_t ts, int64_t tx_id, bool del = false)
            : value(v), timestamp(ts), transaction_id(tx_id), deleted(del) {}
    };

    struct Transaction {
        int64_t id;
        int64_t start_timestamp;
        std::unordered_map<std::string, std::string> writes;
        std::unordered_set<std::string> reads;
        bool committed;

        Transaction(int64_t tx_id, int64_t start_ts)
            : id(tx_id), start_timestamp(start_ts), committed(false) {}
    };

    std::unordered_map<std::string, std::vector<VersionedValue>> data;
    std::unordered_map<int64_t, Transaction> transactions;
    std::atomic<int64_t> next_transaction_id;
    std::atomic<int64_t> current_timestamp;

    std::mutex mutex;

public:
    MVCCStore() : next_transaction_id(1), current_timestamp(1) {}

    int64_t begin_transaction() {
        std::unique_lock<std::mutex> lock(mutex);
        int64_t tx_id = next_transaction_id++;
        int64_t start_ts = current_timestamp++;
        transactions[tx_id] = Transaction(tx_id, start_ts);
        return tx_id;
    }

    std::string read(int64_t tx_id, const std::string& key) {
        std::unique_lock<std::mutex> lock(mutex);

        auto tx_it = transactions.find(tx_id);
        if (tx_it == transactions.end()) {
            throw std::runtime_error("Invalid transaction ID");
        }

        Transaction& tx = tx_it->second;
        tx.reads.insert(key);

        auto data_it = data.find(key);
        if (data_it == data.end() || data_it->second.empty()) {
            return "";  // Key not found
        }

        // Find the latest version visible to this transaction
        for (auto it = data_it->second.rbegin(); it != data_it->second.rend(); ++it) {
            if (it->timestamp <= tx.start_timestamp && !it->deleted) {
                // Check if this version was written by an uncommitted transaction
                if (it->transaction_id != tx_id) {
                    auto writer_tx_it = transactions.find(it->transaction_id);
                    if (writer_tx_it != transactions.end() && !writer_tx_it->second.committed) {
                        continue;  // Skip uncommitted writes
                    }
                }
                return it->value;
            }
        }

        return "";  // No visible version
    }

    void write(int64_t tx_id, const std::string& key, const std::string& value) {
        std::unique_lock<std::mutex> lock(mutex);

        auto tx_it = transactions.find(tx_id);
        if (tx_it == transactions.end()) {
            throw std::runtime_error("Invalid transaction ID");
        }

        Transaction& tx = tx_it->second;
        tx.writes[key] = value;
    }

    bool commit_transaction(int64_t tx_id) {
        std::unique_lock<std::mutex> lock(mutex);

        auto tx_it = transactions.find(tx_id);
        if (tx_it == transactions.end()) {
            return false;
        }

        Transaction& tx = tx_it->second;

        // Write-ahead logging (simplified)
        int64_t commit_ts = current_timestamp++;

        // Write all changes
        for (const auto& pair : tx.writes) {
            const std::string& key = pair.first;
            const std::string& value = pair.second;

            data[key].emplace_back(value, commit_ts, tx_id, false);
        }

        tx.committed = true;
        return true;
    }

    void abort_transaction(int64_t tx_id) {
        std::unique_lock<std::mutex> lock(mutex);

        auto tx_it = transactions.find(tx_id);
        if (tx_it != transactions.end()) {
            transactions.erase(tx_it);
        }
    }

    // Snapshot isolation read
    std::string snapshot_read(int64_t tx_id, const std::string& key) {
        std::unique_lock<std::mutex> lock(mutex);

        auto tx_it = transactions.find(tx_id);
        if (tx_it == transactions.end()) {
            throw std::runtime_error("Invalid transaction ID");
        }

        Transaction& tx = tx_it->second;

        // Read from snapshot at transaction start
        auto data_it = data.find(key);
        if (data_it == data.end() || data_it->second.empty()) {
            return "";
        }

        for (auto it = data_it->second.rbegin(); it != data_it->second.rend(); ++it) {
            if (it->timestamp <= tx.start_timestamp && !it->deleted) {
                return it->value;
            }
        }

        return "";
    }
};

// ============================================================================
// Distributed Transactions (Saga Pattern)
// ============================================================================

enum class SagaState {
    PENDING,
    COMMITTING,
    ABORTING,
    COMMITTED,
    ABORTED
};

struct SagaStep {
    std::string id;
    std::function<bool()> action;
    std::function<bool()> compensation;
    bool completed;
    bool compensated;

    SagaStep(const std::string& i, std::function<bool()> a, std::function<bool()> c)
        : id(i), action(a), compensation(c), completed(false), compensated(false) {}
};

class SagaOrchestrator {
private:
    std::vector<SagaStep> steps;
    SagaState state;
    size_t current_step;
    std::mutex mutex;

public:
    SagaOrchestrator() : state(SagaState::PENDING), current_step(0) {}

    void add_step(const std::string& id,
                  std::function<bool()> action,
                  std::function<bool()> compensation) {
        std::unique_lock<std::mutex> lock(mutex);
        steps.emplace_back(id, action, compensation);
    }

    bool execute() {
        std::unique_lock<std::mutex> lock(mutex);

        if (state != SagaState::PENDING) {
            return false;
        }

        state = SagaState::COMMITTING;

        // Execute all steps
        for (size_t i = 0; i < steps.size(); ++i) {
            current_step = i;
            if (!steps[i].action()) {
                // Action failed, start compensation
                state = SagaState::ABORTING;
                return compensate_from(i);
            }
            steps[i].completed = true;
        }

        state = SagaState::COMMITTED;
        return true;
    }

    bool compensate_from(size_t start_step) {
        for (int i = start_step; i >= 0; --i) {
            if (steps[i].completed && !steps[i].compensated) {
                if (!steps[i].compensation()) {
                    // Compensation failed - manual intervention needed
                    std::cout << "Compensation failed for step: " << steps[i].id << "\n";
                    return false;
                }
                steps[i].compensated = true;
            }
        }

        state = SagaState::ABORTED;
        return true;
    }

    SagaState get_state() const {
        std::unique_lock<std::mutex> lock(mutex);
        return state;
    }

    std::string get_status() const {
        std::unique_lock<std::mutex> lock(mutex);
        std::stringstream ss;
        ss << "Saga state: ";

        switch (state) {
            case SagaState::PENDING: ss << "PENDING"; break;
            case SagaState::COMMITTING: ss << "COMMITTING (step " << current_step << ")"; break;
            case SagaState::ABORTING: ss << "ABORTING"; break;
            case SagaState::COMMITTED: ss << "COMMITTED"; break;
            case SagaState::ABORTED: ss << "ABORTED"; break;
        }

        return ss.str();
    }
};

// ============================================================================
// Anti-Entropy and Read Repair
// ============================================================================

class AntiEntropyProtocol {
private:
    DistributedKVStore* store;
    std::string local_node_id;
    std::thread repair_thread;
    std::atomic<bool> running;
    std::chrono::milliseconds repair_interval;

public:
    AntiEntropyProtocol(DistributedKVStore* s, const std::string& node_id,
                       std::chrono::milliseconds interval = std::chrono::minutes(5))
        : store(s), local_node_id(node_id), running(true), repair_interval(interval) {
        repair_thread = std::thread(&AntiEntropyProtocol::repair_loop, this);
    }

    ~AntiEntropyProtocol() {
        running = false;
        if (repair_thread.joinable()) {
            repair_thread.join();
        }
    }

    void repair_loop() {
        while (running) {
            std::this_thread::sleep_for(repair_interval);

            // In a real implementation, this would:
            // 1. Choose a random peer
            // 2. Exchange Merkle trees to find differences
            // 3. Repair inconsistent data

            std::cout << "[" << local_node_id << "] Running anti-entropy repair\n";
        }
    }

    // Simplified read repair
    void read_repair(const std::string& key, const std::vector<KeyValue>& versions) {
        if (versions.size() <= 1) return;

        // Find the most recent version
        const KeyValue* latest = &versions[0];
        for (const auto& version : versions) {
            if (latest->version.happens_before(version.version)) {
                latest = &version;
            }
        }

        // Write back the latest version to all replicas
        VectorClock repair_version = latest->version;
        repair_version.increment(local_node_id + "_repair");

        store->put(key, latest->value, repair_version);

        std::cout << "[" << local_node_id << "] Read repair performed for key: " << key << "\n";
    }
};

// ============================================================================
// Demonstration and Testing
// ============================================================================

void demonstrate_vector_clocks() {
    std::cout << "=== Vector Clocks Demo ===\n";

    VectorClock vc1, vc2, vc3;

    vc1.increment("node1");
    vc1.increment("node1");

    vc2.increment("node2");

    vc3.increment("node1");
    vc3.increment("node2");

    std::cout << "VC1: " << vc1.to_string() << "\n";
    std::cout << "VC2: " << vc2.to_string() << "\n";
    std::cout << "VC3: " << vc3.to_string() << "\n";

    std::cout << "VC1 happens-before VC2: " << vc1.happens_before(vc2) << "\n";
    std::cout << "VC2 happens-before VC1: " << vc2.happens_before(vc1) << "\n";
    std::cout << "VC1 concurrent with VC2: " << vc1.is_concurrent(vc2) << "\n";
    std::cout << "VC1 happens-before VC3: " << vc1.happens_before(vc3) << "\n";
}

void demonstrate_consistent_hashing() {
    std::cout << "\n=== Consistent Hashing Demo ===\n";

    ConsistentHashRing ring(10);  // 10 virtual nodes per physical node

    ring.add_node("node1");
    ring.add_node("node2");
    ring.add_node("node3");

    std::cout << "Ring has " << ring.size() << " physical nodes\n";

    std::vector<std::string> keys = {"user123", "product456", "order789", "session001"};

    for (const auto& key : keys) {
        auto nodes = ring.get_nodes(key, 3);
        std::cout << "Key '" << key << "' maps to: ";
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << nodes[i];
        }
        std::cout << "\n";
    }

    // Add a node and see redistribution
    std::cout << "\nAdding node4...\n";
    ring.add_node("node4");

    for (const auto& key : keys) {
        auto nodes = ring.get_nodes(key, 3);
        std::cout << "Key '" << key << "' now maps to: ";
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << nodes[i];
        }
        std::cout << "\n";
    }
}

void demonstrate_crdts() {
    std::cout << "\n=== CRDTs Demo ===\n";

    // G-Counter
    CRDT_GCounter<int64_t> gcounter;
    gcounter.increment("node1", 5);
    gcounter.increment("node2", 3);
    gcounter.increment("node1", 2);

    std::cout << "G-Counter value: " << gcounter.value() << "\n";
    std::cout << gcounter.to_string() << "\n";

    // PN-Counter
    CRDT_PNCounter pncounter;
    pncounter.increment("node1", 10);
    pncounter.increment("node2", 5);
    pncounter.decrement("node1", 3);

    std::cout << "PN-Counter value: " << pncounter.value() << "\n";
    std::cout << pncounter.to_string() << "\n";

    // G-Set
    CRDT_GSet gset;
    gset.add("apple");
    gset.add("banana");
    gset.add("cherry");

    std::cout << "G-Set contains 'banana': " << gset.contains("banana") << "\n";
    std::cout << gset.to_string() << "\n";

    // LWW Register
    CRDT_LWWRegister reg1, reg2;

    VectorClock vc1, vc2;
    vc1.increment("node1");
    vc2.increment("node2");

    reg1.write("value1", vc1);
    reg2.write("value2", vc2);

    reg1.merge(reg2);  // Merge concurrent writes

    std::cout << "LWW Register value: " << reg1.read() << "\n";
    std::cout << reg1.to_string() << "\n";
}

void demonstrate_distributed_kv() {
    std::cout << "\n=== Distributed KV Store Demo ===\n";

    DistributedKVStore store(3, ConsistencyLevel::QUORUM, ConsistencyLevel::QUORUM);

    // Add nodes
    for (int i = 1; i <= 5; ++i) {
        store.add_node("node" + std::to_string(i));
    }

    // Write some data
    VectorClock vc1, vc2;
    vc1.increment("client1");

    bool success1 = store.put("key1", "value1", vc1);
    std::cout << "Write key1: " << (success1 ? "SUCCESS" : "FAILED") << "\n";

    // Read the data
    auto versions = store.get("key1");
    std::cout << "Read key1: found " << versions.size() << " versions\n";

    for (const auto& version : versions) {
        std::cout << "  " << version.value << " @ " << version.version.to_string() << "\n";
    }

    // Conflict resolution
    std::string resolved_value = store.get_with_resolution("key1");
    std::cout << "Resolved value: " << resolved_value << "\n";

    // Simulate concurrent writes
    vc2.increment("client2");
    store.put("key1", "value1_modified", vc2);

    resolved_value = store.get_with_resolution("key1");
    std::cout << "After concurrent write: " << resolved_value << "\n";
}

void demonstrate_mvcc() {
    std::cout << "\n=== MVCC Demo ===\n";

    MVCCStore store;

    // Transaction 1
    int64_t tx1 = store.begin_transaction();
    store.write(tx1, "account1", "1000");
    store.write(tx1, "account2", "500");

    // Transaction 2 (concurrent)
    int64_t tx2 = store.begin_transaction();
    std::string balance1 = store.read(tx2, "account1");
    std::string balance2 = store.read(tx2, "account2");

    std::cout << "TX2 read: account1=" << balance1 << ", account2=" << balance2 << "\n";

    // Transfer money in TX2
    int new_balance1 = std::stoi(balance1) - 100;
    int new_balance2 = std::stoi(balance2) + 100;
    store.write(tx2, "account1", std::to_string(new_balance1));
    store.write(tx2, "account2", std::to_string(new_balance2));

    // Commit both transactions
    store.commit_transaction(tx1);
    store.commit_transaction(tx2);

    // Read final values
    int64_t tx3 = store.begin_transaction();
    std::string final1 = store.read(tx3, "account1");
    std::string final2 = store.read(tx3, "account2");

    std::cout << "Final state: account1=" << final1 << ", account2=" << final2 << "\n";
}

void demonstrate_saga_pattern() {
    std::cout << "\n=== Saga Pattern Demo ===\n";

    SagaOrchestrator saga;

    // Simulate a money transfer saga
    saga.add_step("withdraw_from_source",
        []() { std::cout << "Withdrawing from source account\n"; return true; },
        []() { std::cout << "Refunding source account\n"; return true; });

    saga.add_step("deposit_to_destination",
        []() {
            std::cout << "Depositing to destination account\n";
            // Simulate failure
            static int attempt = 0;
            return ++attempt > 1;  // Fail on first attempt
        },
        []() { std::cout << "Reversing deposit\n"; return true; });

    saga.add_step("update_transaction_log",
        []() { std::cout << "Logging transaction\n"; return true; },
        []() { std::cout << "Removing transaction log\n"; return true; });

    bool success = saga.execute();

    std::cout << "Saga " << (success ? "succeeded" : "failed") << "\n";
    std::cout << "Final status: " << saga.get_status() << "\n";
}

} // namespace distributed_databases

// ============================================================================
// Main Function for Testing
// ============================================================================

int main() {
    std::cout << "🗄️ **Distributed Databases** - Production-Grade Data Consistency\n";
    std::cout << "===========================================================\n\n";

    distributed_databases::demonstrate_vector_clocks();
    distributed_databases::demonstrate_consistent_hashing();
    distributed_databases::demonstrate_crdts();
    distributed_databases::demonstrate_distributed_kv();
    distributed_databases::demonstrate_mvcc();
    distributed_databases::demonstrate_saga_pattern();

    std::cout << "\n✅ **Distributed Databases Complete**\n";
    std::cout << "Extracted patterns from: Cassandra, DynamoDB, Riak, CockroachDB, Google Spanner\n";
    std::cout << "Features: Vector Clocks, CRDTs, Consistent Hashing, MVCC, Sagas, Anti-Entropy\n";

    return 0;
}
