# Distributed Systems Pattern Recognition Guide

## 🌐 **Decision Tree for Distributed System Pattern Selection**

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    DISTRIBUTED SYSTEMS PATTERN DECISION TREE            │
│                  "Choose Your Distribution Architecture"                │
└─────────────────────────────────────────────────────────────────────────┘

1. What is your consistency requirement?
   ├─── Strong Consistency ─────────────────► Atomic operations, linearizability
   ├─── Eventual Consistency ───────────────► BASE properties, conflict resolution
   ├─── Causal Consistency ─────────────────► Vector clocks, causal ordering
   ├─── Weak Consistency ───────────────────► Best effort, optimistic updates
   └─── No Consistency ─────────────────────► Fire-and-forget messaging

2. What is your fault tolerance model?
   ├─── Fail-Stop ──────────────────────────► Crash fault tolerance
   ├─── Fail-Recovery ─────────────────────► State machine replication
   ├─── Fail-Arbitrary (Byzantine) ────────► Byzantine fault tolerance
   ├─── Fail-Silent ───────────────────────► Silent failure detection
   └─── Fail-Probabilistic ────────────────► Probabilistic guarantees

3. What is your data model?
   ├─── Key-Value ──────────────────────────► Consistent hashing, replication
   ├─── Relational ─────────────────────────► Distributed transactions, sharding
   ├─── Document ───────────────────────────► Eventual consistency, versioning
   ├─── Graph ──────────────────────────────► Distributed graph algorithms
   ├─── Time-Series ───────────────────────► Retention policies, compression
   └─── Streaming ──────────────────────────► Event ordering, exactly-once

4. What is your communication model?
   ├─── Synchronous ───────────────────────► RPC, request-response
   ├─── Asynchronous ──────────────────────► Message queues, pub-sub
   ├─── Batch ─────────────────────────────► Bulk transfers, ETL pipelines
   ├─── Streaming ─────────────────────────► Real-time data flow
   └─── Hybrid ────────────────────────────► Adaptive communication

5. What is your scaling requirement?
   ├─── Read-Heavy ────────────────────────► Read replicas, caching layers
   ├─── Write-Heavy ───────────────────────► Sharding, leader election
   ├─── Balanced ──────────────────────────► Load balancing, partitioning
   ├─── Burst ─────────────────────────────► Auto-scaling, elastic resources
   └─── Predictable ───────────────────────► Capacity planning, quotas

6. What is your latency requirement?
   ├─── Real-Time (<10ms) ─────────────────► In-memory, local caches
   ├─── Interactive (<100ms) ─────────────► CDN, edge computing
   ├─── Batch (<1s) ───────────────────────► Background processing
   ├─── Offline (>1s) ─────────────────────► Asynchronous processing
   └─── Best Effort ───────────────────────► Fire-and-forget

7. What is your network reliability?
   ├─── Reliable ──────────────────────────► TCP, acknowledgments, retries
   ├─── Unreliable ────────────────────────► UDP, best-effort delivery
   ├─── Partially Reliable ───────────────► Custom reliability layers
   ├─── WAN ───────────────────────────────► WAN optimization, compression
   └─── Lossy ─────────────────────────────► FEC, error correction

8. What is your deployment model?
   ├─── Cloud-Native ──────────────────────► Microservices, containers
   ├─── Hybrid ───────────────────────────► Multi-cloud, edge deployment
   ├─── On-Premises ───────────────────────► Traditional data centers
   ├─── Edge ──────────────────────────────► IoT, mobile edge computing
   └─── Multi-Region ─────────────────────► Global distribution, geo-replication
```

## 📊 **Performance Characteristics**

| Distributed Pattern | Consistency | Latency | Throughput | Scalability | Fault Tolerance |
|---------------------|-------------|---------|------------|-------------|-----------------|
| **Consensus (Raft)** | Strong | Medium | Medium | High | High |
| **Eventual Consistency** | Weak | Low | Very High | Very High | High |
| **Message Queues** | At-least-once | Variable | High | High | Medium |
| **Service Mesh** | Variable | Low | High | High | High |
| **Distributed Cache** | Weak | Very Low | Very High | High | Medium |
| **Load Balancer** | N/A | Low | High | High | Medium |
| **Circuit Breaker** | N/A | Medium | Variable | Medium | High |
| **Saga Pattern** | Eventual | High | Low | Medium | High |

## 🎯 **Pattern Variants by Distributed System Layer**

### **Consensus Patterns** 🏛️
```cpp
// Raft Consensus Algorithm
class RaftConsensus {
    enum class NodeState { FOLLOWER, CANDIDATE, LEADER };

    struct LogEntry {
        int term;
        int index;
        std::string command;
        bool committed;
    };

    NodeState state;
    int current_term;
    int voted_for;
    std::vector<LogEntry> log;
    int commit_index;
    int last_applied;

    // Leader election
    void start_election() {
        state = NodeState::CANDIDATE;
        current_term++;
        voted_for = node_id;
        vote_count = 1;

        // Send RequestVote RPCs
        for (auto& peer : peers) {
            send_request_vote(peer, current_term, last_log_index, last_log_term);
        }
    }

    // Log replication
    void replicate_log() {
        for (auto& peer : peers) {
            if (next_index[peer] <= log.size()) {
                send_append_entries(peer, current_term, log[next_index[peer]-1]);
            }
        }
    }
};
```

### **Distributed Databases Patterns** 🗄️
```cpp
// Distributed Key-Value Store
class DistributedKVStore {
    struct KeyValue {
        std::string key;
        std::string value;
        VectorClock version;
        bool deleted;
    };

    // Consistent hashing for partitioning
    ConsistentHashRing ring;

    // Replication and quorum
    int replication_factor;
    QuorumConfig quorum;

    std::string get(const std::string& key) {
        auto nodes = ring.get_nodes(key, replication_factor);

        // Read from quorum
        std::vector<KeyValue> versions;
        for (auto& node : nodes) {
            auto kv = node.read(key);
            if (kv) versions.push_back(*kv);
        }

        // Resolve conflicts using vector clocks
        return resolve_conflicts(versions);
    }

    bool put(const std::string& key, const std::string& value) {
        auto nodes = ring.get_nodes(key, replication_factor);

        // Write to quorum
        VectorClock new_version = get_current_version(key) + 1;
        KeyValue kv{key, value, new_version, false};

        int success_count = 0;
        for (auto& node : nodes) {
            if (node.write(kv)) success_count++;
        }

        return success_count >= quorum.write_quorum;
    }
};
```

### **Message Passing Patterns** 📨
```cpp
// Publish-Subscribe Messaging
class PubSubSystem {
    struct Topic {
        std::string name;
        std::vector<Subscriber*> subscribers;
        std::queue<Message> message_queue;
        std::unordered_map<std::string, size_t> consumer_offsets;
    };

    std::unordered_map<std::string, Topic> topics;

    void publish(const std::string& topic_name, const Message& message) {
        auto& topic = topics[topic_name];

        // Store message durably
        topic.message_queue.push(message);
        persist_message(topic_name, message);

        // Deliver to subscribers
        for (auto* subscriber : topic.subscribers) {
            deliver_message(subscriber, message);
        }
    }

    void subscribe(const std::string& topic_name, Subscriber* subscriber) {
        auto& topic = topics[topic_name];
        topic.subscribers.push_back(subscriber);

        // Send recent messages based on subscriber's offset
        size_t start_offset = topic.consumer_offsets[subscriber->id];
        replay_messages(subscriber, topic, start_offset);
    }
};
```

### **Fault Tolerance Patterns** 🛡️
```cpp
// Circuit Breaker Pattern
class CircuitBreaker {
    enum class State { CLOSED, OPEN, HALF_OPEN };

    State state;
    int failure_count;
    int success_count;
    std::chrono::steady_clock::time_point last_failure_time;
    std::chrono::milliseconds timeout;

    bool call(std::function<bool()> operation) {
        switch (state) {
            case State::CLOSED:
                if (!operation()) {
                    failure_count++;
                    if (failure_count >= failure_threshold) {
                        state = State::OPEN;
                        last_failure_time = std::chrono::steady_clock::now();
                    }
                    return false;
                }
                failure_count = 0;
                return true;

            case State::OPEN:
                if (std::chrono::steady_clock::now() - last_failure_time > timeout) {
                    state = State::HALF_OPEN;
                    success_count = 0;
                }
                return false;  // Fail fast

            case State::HALF_OPEN:
                if (operation()) {
                    success_count++;
                    if (success_count >= success_threshold) {
                        state = State::CLOSED;
                        failure_count = 0;
                    }
                    return true;
                } else {
                    state = State::OPEN;
                    last_failure_time = std::chrono::steady_clock::now();
                    return false;
                }
        }
        return false;
    }
};
```

### **Distributed Coordination Patterns** 🎭
```cpp
// Distributed Lock Service
class DistributedLockService {
    struct LockRequest {
        std::string resource;
        std::string owner;
        int sequence_number;
        LockMode mode;
    };

    std::unordered_map<std::string, std::vector<LockRequest>> lock_queues;
    std::unordered_map<std::string, LockRequest> held_locks;

    bool acquire_lock(const std::string& resource, const std::string& owner,
                     LockMode mode, int timeout_ms) {

        LockRequest request{resource, owner, get_sequence_number(), mode};

        // Check if lock can be granted immediately
        if (can_grant_lock(request)) {
            grant_lock(request);
            return true;
        }

        // Queue the request
        lock_queues[resource].push_back(request);

        // Wait with timeout
        return wait_for_lock(request, timeout_ms);
    }

    void release_lock(const std::string& resource, const std::string& owner) {
        auto it = held_locks.find(resource);
        if (it != held_locks.end() && it->second.owner == owner) {
            held_locks.erase(it);

            // Grant next waiting request
            auto& queue = lock_queues[resource];
            if (!queue.empty()) {
                auto next_request = queue.front();
                queue.erase(queue.begin());

                if (can_grant_lock(next_request)) {
                    grant_lock(next_request);
                }
            }
        }
    }
};
```

## 🏆 **Real-World Production Examples**

### **Consensus Algorithms**
- **Raft**: etcd, Consul, CockroachDB
- **Paxos**: Google Chubby, Apache ZooKeeper, Boxwood
- **ZAB**: Apache ZooKeeper, HBase
- **Viewstamped Replication**: Azure Storage, DynamoDB
- **Byzantine Paxos**: Stellar, Hyperledger Fabric

### **Distributed Databases**
- **Key-Value**: Redis Cluster, etcd, Consul
- **Document**: MongoDB, CouchDB, Cassandra
- **Relational**: CockroachDB, TiDB, YugabyteDB
- **NewSQL**: Google Spanner, Aurora, PlanetScale
- **Time-Series**: InfluxDB, TimescaleDB, Prometheus

### **Message Systems**
- **Queues**: RabbitMQ, ActiveMQ, SQS
- **Streams**: Apache Kafka, Redpanda, Pulsar
- **Pub-Sub**: Google Pub/Sub, AWS SNS, NATS
- **Event Streaming**: Apache Flink, Spark Streaming, Kafka Streams

### **Fault Tolerance**
- **Leader Election**: ZooKeeper, etcd, Consul
- **Failure Detection**: SWIM, Phi Accrual, Heartbeat
- **Replication**: Multi-Paxos, Chain Replication, Primary-Backup
- **Recovery**: State Machine Replication, Log Shipping, Snapshot

### **Service Coordination**
- **Service Discovery**: Eureka, ZooKeeper, Consul
- **Configuration**: Apollo, Spring Config, etcd
- **Distributed Locks**: ZooKeeper, etcd, Redis
- **Leader Election**: ZooKeeper, Kubernetes, etcd

### **Microservices Patterns**
- **Service Mesh**: Istio, Linkerd, Consul Connect
- **API Gateway**: Kong, Traefik, Ambassador
- **Saga Orchestration**: Eventuate, Temporal, Conductor
- **Circuit Breaker**: Hystrix, Resilience4j, Sentinel

### **Data Pipeline Patterns**
- **ETL**: Apache Airflow, Prefect, Dagster
- **Stream Processing**: Apache Kafka Streams, Apache Beam, Flink
- **Batch Processing**: Apache Spark, Apache Flink, Presto
- **Real-time Analytics**: Druid, Pinot, ClickHouse

### **Load Balancing**
- **DNS**: Route 53, Cloudflare, PowerDNS
- **L4 Load Balancing**: HAProxy, NGINX, Envoy
- **L7 Load Balancing**: Traefik, Istio, AWS ALB
- **Service Mesh**: Istio, Linkerd, Consul

### **Caching Patterns**
- **Distributed Cache**: Redis Cluster, Memcached, Hazelcast
- **CDN**: Cloudflare, Akamai, Fastly
- **Edge Computing**: Cloudflare Workers, Lambda@Edge, Fastly Compute

## ⚡ **Advanced Distributed Patterns**

### **1. CRDTs (Conflict-Free Replicated Data Types)**
```cpp
template<typename T>
class CRDT_GCounter {
private:
    std::unordered_map<std::string, T> counters;  // Per-replica counters

public:
    void increment(const std::string& replica_id, T amount = 1) {
        counters[replica_id] += amount;
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
};
```

### **2. Vector Clocks and Causal Consistency**
```cpp
class VectorClock {
private:
    std::unordered_map<std::string, int> clock;

public:
    void increment(const std::string& node_id) {
        clock[node_id]++;
    }

    bool happens_before(const VectorClock& other) const {
        bool less_or_equal = true;
        bool strictly_less = false;

        for (const auto& pair : other.clock) {
            auto it = clock.find(pair.first);
            int this_value = (it != clock.end()) ? it->second : 0;

            if (this_value > pair.second) {
                less_or_equal = false;
                break;
            }
            if (this_value < pair.second) {
                strictly_less = true;
            }
        }

        // Check for components only in this clock
        for (const auto& pair : clock) {
            if (other.clock.find(pair.first) == other.clock.end() && pair.second > 0) {
                strictly_less = true;
            }
        }

        return less_or_equal && strictly_less;
    }

    void merge(const VectorClock& other) {
        for (const auto& pair : other.clock) {
            if (clock[pair.first] < pair.second) {
                clock[pair.first] = pair.second;
            }
        }
    }
};
```

### **3. Consistent Hashing**
```cpp
class ConsistentHashRing {
private:
    std::map<size_t, std::string> ring;  // hash -> node
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
    void add_node(const std::string& node_id) {
        for (int i = 0; i < virtual_nodes_per_physical; ++i) {
            std::string virtual_node = node_id + "#" + std::to_string(i);
            size_t hash_value = hash(virtual_node);
            ring[hash_value] = node_id;
        }
    }

    void remove_node(const std::string& node_id) {
        for (int i = 0; i < virtual_nodes_per_physical; ++i) {
            std::string virtual_node = node_id + "#" + std::to_string(i);
            size_t hash_value = hash(virtual_node);
            ring.erase(hash_value);
        }
    }

    std::vector<std::string> get_nodes(const std::string& key, int count) {
        size_t key_hash = hash(key);
        std::vector<std::string> result;

        auto it = ring.lower_bound(key_hash);
        if (it == ring.end()) {
            it = ring.begin();
        }

        // Collect 'count' distinct nodes
        std::unordered_set<std::string> seen;
        for (; result.size() < static_cast<size_t>(count) && seen.size() < ring.size(); ++it) {
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
};
```

### **4. Gossip Protocol**
```cpp
class GossipProtocol {
private:
    struct GossipMessage {
        std::string sender;
        std::unordered_map<std::string, NodeState> states;
        std::chrono::steady_clock::time_point timestamp;
    };

    struct NodeState {
        enum class Status { ALIVE, SUSPECT, DEAD };
        Status status;
        int incarnation;
        std::chrono::steady_clock::time_point last_update;
    };

    std::string node_id;
    std::unordered_map<std::string, NodeState> membership_list;
    std::vector<std::string> seeds;

    void gossip() {
        // Select random nodes to gossip with
        std::vector<std::string> targets = select_gossip_targets();

        GossipMessage message;
        message.sender = node_id;
        message.states = membership_list;
        message.timestamp = std::chrono::steady_clock::now();

        for (const auto& target : targets) {
            send_gossip_message(target, message);
        }
    }

    void handle_gossip_message(const GossipMessage& message) {
        // Merge received states with local states
        for (const auto& pair : message.states) {
            const std::string& node = pair.first;
            const NodeState& remote_state = pair.second;

            auto it = membership_list.find(node);
            if (it == membership_list.end()) {
                // New node
                membership_list[node] = remote_state;
            } else {
                NodeState& local_state = it->second;

                // Resolve conflicts using incarnation numbers
                if (remote_state.incarnation > local_state.incarnation) {
                    local_state = remote_state;
                } else if (remote_state.incarnation == local_state.incarnation) {
                    // Use timestamps to break ties
                    if (remote_state.last_update > local_state.last_update) {
                        local_state = remote_state;
                    }
                }
            }
        }
    }

    std::vector<std::string> select_gossip_targets() {
        // Select sqrt(n) random nodes
        size_t target_count = std::sqrt(membership_list.size());
        std::vector<std::string> candidates;

        for (const auto& pair : membership_list) {
            if (pair.first != node_id) {
                candidates.push_back(pair.first);
            }
        }

        // Random selection
        std::random_shuffle(candidates.begin(), candidates.end());
        if (candidates.size() > target_count) {
            candidates.resize(target_count);
        }

        return candidates;
    }
};
```

### **5. Saga Pattern for Distributed Transactions**
```cpp
class SagaOrchestrator {
private:
    struct SagaStep {
        std::string id;
        std::function<bool()> action;
        std::function<bool()> compensation;
        bool completed;
        bool compensated;
    };

    std::vector<SagaStep> steps;
    size_t current_step;
    bool compensating;

public:
    void add_step(const std::string& id,
                  std::function<bool()> action,
                  std::function<bool()> compensation) {
        steps.push_back({id, action, compensation, false, false});
    }

    bool execute() {
        current_step = 0;
        compensating = false;

        while (current_step < steps.size()) {
            if (!compensating) {
                // Execute next step
                if (!steps[current_step].action()) {
                    // Action failed, start compensation
                    compensating = true;
                    current_step--;  // Retry current step? No, compensate previous
                } else {
                    steps[current_step].completed = true;
                    current_step++;
                }
            } else {
                // Compensate previous step
                if (current_step >= 0 && steps[current_step].completed) {
                    if (steps[current_step].compensation()) {
                        steps[current_step].compensated = true;
                        current_step--;
                    } else {
                        // Compensation failed - manual intervention needed
                        return false;
                    }
                } else {
                    current_step--;
                }
            }
        }

        return !compensating;  // Success if not compensating
    }

    SagaStatus get_status() const {
        if (compensating) return SagaStatus::COMPENSATING;
        if (current_step >= steps.size()) return SagaStatus::COMPLETED;
        return SagaStatus::IN_PROGRESS;
    }
};
```

## 📚 **Further Reading**

- **"Designing Data-Intensive Applications"** - Martin Kleppmann
- **"Distributed Systems: Principles and Paradigms"** - Tanenbaum, Van Steen
- **"Consensus: Bridging Theory and Practice"** - Lynch, Malkhi, Spiegelman
- **"Distributed Computing: Fundamentals, Simulations and Advanced Topics"** - Attiya, Welch
- **"Microservices Patterns"** - Richardson
- **"Streaming Systems"** - Akidau, Chernyak, Lax
- **"The Art of Scalability"** - Abbott, Fisher

---

*"Distributed systems are not about building bigger monoliths, but orchestrating smart parts that dance in harmony across the network."* 🌐⚡
