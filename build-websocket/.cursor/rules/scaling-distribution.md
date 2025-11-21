# Scaling and Distribution Standards

## Overview
Scaling and distribution are critical for production grade WebSocket servers handling millions of concurrent connections across multiple nodes. This document defines standards for scaling WebSocket servers including load balancing, pub/sub, and horizontal sharding.

## Load Balancing

### Sticky Routing
* **Definition**: Route connections to same server
* **Methods**: Hash based, cookie based, consistent hashing
* **Benefits**: Enables session affinity
* **Rationale**: Sticky routing enables stateful connections

### Layer 4 Load Balancing
* **Definition**: TCP level load balancing
* **Methods**: IP hash, round robin
* **Use cases**: Simple connection distribution
* **Rationale**: L4 load balancing enables simple distribution

### Layer 7 Load Balancing
* **Definition**: HTTP/WebSocket level load balancing
* **Methods**: HTTP header based routing
* **Use cases**: Advanced routing, SSL termination
* **Rationale**: L7 load balancing enables advanced routing

### Example Sticky Routing
```cpp
class StickyRouter {
public:
    ServerId route_connection(const ConnectionRequest& request) {
        // Hash based routing
        auto hash = std::hash<std::string>{}(request.client_ip());
        auto server_id = servers_[hash % servers_.size()];
        return server_id;
    }
    
    ServerId route_reconnect(const std::string& session_token) {
        // Session token based routing
        auto session = session_store_.get(session_token);
        if (session) {
            return session->server_id;
        }
        return route_connection(ConnectionRequest{});
    }
    
private:
    std::vector<ServerId> servers_;
    SessionStore session_store_;
};
```

## Pub/Sub Backbone

### Message Broker
* **Definition**: Centralized message broker
* **Options**: Redis, NATS, Kafka
* **Purpose**: Cross node message delivery
* **Rationale**: Message broker enables distributed messaging

### Channel/Room Model
* **Definition**: Logical grouping of connections
* **Channels**: Topics for message routing
* **Subscriptions**: Connection subscriptions to channels
* **Rationale**: Channel model enables message routing

### Example Pub/Sub Integration
```cpp
class PubSubManager {
public:
    void subscribe(WebSocketConnection& conn, const std::string& channel) {
        // Local subscription
        local_subscriptions_[channel].insert(conn.id());
        
        // Remote subscription via broker
        broker_->subscribe(channel, server_id_);
    }
    
    void publish(const std::string& channel, const std::string& message) {
        // Publish to local subscribers
        auto it = local_subscriptions_.find(channel);
        if (it != local_subscriptions_.end()) {
            for (auto conn_id : it->second) {
                auto conn = get_connection(conn_id);
                if (conn) {
                    conn->send_text(message);
                }
            }
        }
        
        // Publish to remote subscribers via broker
        broker_->publish(channel, message);
    }
    
    void handle_broker_message(const std::string& channel, 
                               const std::string& message) {
        // Handle message from broker (remote node)
        auto it = local_subscriptions_.find(channel);
        if (it != local_subscriptions_.end()) {
            for (auto conn_id : it->second) {
                auto conn = get_connection(conn_id);
                if (conn) {
                    conn->send_text(message);
                }
            }
        }
    }
    
private:
    std::unordered_map<std::string, std::set<ConnectionId>> local_subscriptions_;
    std::shared_ptr<MessageBroker> broker_;
    ServerId server_id_;
};
```

## Horizontal Sharding

### Sharding Strategy
* **Definition**: Partition connections across servers
* **Methods**: Hash based, range based, directory based
* **Benefits**: Enables horizontal scaling
* **Rationale**: Sharding enables scale

### Channel Sharding
* **Definition**: Shard by channel/room
* **Benefits**: Localizes channel operations
* **Implementation**: Hash channel name to server
* **Rationale**: Channel sharding enables efficient routing

### Connection Sharding
* **Definition**: Shard by connection ID
* **Benefits**: Even distribution
* **Implementation**: Hash connection ID to server
* **Rationale**: Connection sharding enables even distribution

### Example Sharding
```cpp
class ShardManager {
public:
    ServerId get_shard_for_channel(const std::string& channel) {
        auto hash = std::hash<std::string>{}(channel);
        return servers_[hash % servers_.size()];
    }
    
    ServerId get_shard_for_connection(ConnectionId conn_id) {
        auto hash = std::hash<ConnectionId>{}(conn_id);
        return servers_[hash % servers_.size()];
    }
    
    bool is_local_shard(ServerId server_id) {
        return server_id == local_server_id_;
    }
    
private:
    std::vector<ServerId> servers_;
    ServerId local_server_id_;
};
```

## Presence Service

### Definition
* **Presence**: Track who is online in channels
* **Purpose**: Enable presence features
* **Implementation**: Track connection channel memberships
* **Rationale**: Presence enables user awareness

### Implementation
* **Local presence**: Track local connections
* **Distributed presence**: Share presence across nodes
* **Presence updates**: Notify on join/leave
* **Rationale**: Implementation enables presence features

## Delivery Semantics

### At Most Once
* **Definition**: Fire and forget delivery
* **Use cases**: Non critical messages
* **Benefits**: Simple, low overhead
* **Rationale**: At most once enables simple delivery

### At Least Once
* **Definition**: Guaranteed delivery with acknowledgments
* **Use cases**: Critical messages
* **Benefits**: Reliability
* **Rationale**: At least once enables reliable delivery

### Implementation
* **Ack/nack**: Acknowledgment mechanisms
* **Redelivery**: Redelivery on timeout
* **Deduplication**: Message deduplication
* **Rationale**: Implementation enables delivery semantics

## Implementation Standards

### Correctness
* **Routing correctness**: Ensure correct routing
* **Message delivery**: Ensure message delivery
* **Consistency**: Ensure consistency
* **Rationale**: Correctness is critical

### Performance
* **Efficient routing**: Optimize routing
* **Minimize latency**: Minimize cross node latency
* **Rationale**: Performance is critical

## Testing Requirements

### Unit Tests
* **Routing tests**: Test routing logic
* **Pub/sub tests**: Test pub/sub functionality
* **Sharding tests**: Test sharding logic
* **Presence tests**: Test presence tracking
* **Rationale**: Comprehensive testing ensures correctness

## Research Papers and References

### Scaling and Distribution
* "Distributed Systems" research papers
* "Load Balancing" research papers
* Scaling guides

## Implementation Checklist

- [ ] Understand load balancing
- [ ] Implement sticky routing
- [ ] Implement pub/sub backbone
- [ ] Implement horizontal sharding
- [ ] Implement presence service
- [ ] Write comprehensive unit tests
- [ ] Document scaling architecture

