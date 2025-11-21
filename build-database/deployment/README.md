# AuroraDB Deployment Guide

This directory contains production-ready deployment configurations for AuroraDB, including Docker, Kubernetes, and CLI tools.

## 🚀 Quick Start

### Using Docker Compose (Development)

```bash
# Start AuroraDB with monitoring stack
docker-compose -f deployment/docker/docker-compose.yml up -d

# Check status
curl http://localhost:8080/health

# Connect using CLI
cargo run --bin aurora-cli -- --help
```

### Using Kubernetes (Production)

```bash
# Deploy to Kubernetes
kubectl apply -f deployment/kubernetes/

# Check deployment status
kubectl get pods -n aurora-db
kubectl get services -n aurora-db

# Scale the cluster
kubectl scale statefulset aurora-db-replicas --replicas=5 -n aurora-db
```

## 📁 Directory Structure

```
deployment/
├── docker/                    # Docker configurations
│   ├── Dockerfile            # Multi-stage production build
│   ├── docker-compose.yml    # Development environment
│   └── haproxy.cfg          # Load balancer config
├── kubernetes/              # Kubernetes manifests
│   └── aurora-deployment.yaml # Complete cluster deployment
├── cli/                     # Command-line interface
│   ├── src/
│   │   ├── main.rs         # CLI entry point
│   │   ├── commands.rs     # Command implementations
│   │   ├── client.rs       # HTTP client
│   │   └── output.rs       # Output formatting
│   └── Cargo.toml          # CLI dependencies
└── config/                  # Configuration files
    └── production.toml     # Production configuration
```

## 🐳 Docker Deployment

### Building the Image

```bash
# Build production image
docker build -f deployment/docker/Dockerfile -t aurora-db:latest .

# Build debug image
docker build -f deployment/docker/Dockerfile --target debug -t aurora-db:debug .
```

### Environment Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `AURORA_NODE_ROLE` | Node role (primary/replica) | primary |
| `AURORA_CLUSTER_SIZE` | Cluster size | 3 |
| `AURORA_JIT_ENABLED` | Enable JIT compilation | true |
| `AURORA_BUFFER_POOL_SIZE` | Buffer pool size | 2GB |
| `AURORA_MAX_CONNECTIONS` | Max connections | 1000 |

### Docker Compose Services

- **aurora-primary**: Main database node
- **aurora-replica**: Read replica
- **aurora-lb**: HAProxy load balancer
- **prometheus**: Metrics collection
- **grafana**: Monitoring dashboard

## ☸️ Kubernetes Deployment

### Prerequisites

```bash
# Install Helm (optional)
curl https://get.helm.sh/helm-v3.12.0-linux-amd64.tar.gz | tar xz
sudo mv linux-amd64/helm /usr/local/bin/

# Create namespace
kubectl create namespace aurora-db
```

### Deploying

```bash
# Deploy AuroraDB cluster
kubectl apply -f deployment/kubernetes/aurora-deployment.yaml

# Wait for rollout
kubectl rollout status statefulset/aurora-db-primary -n aurora-db

# Check cluster status
kubectl get pods -n aurora-db
```

### Scaling

```bash
# Scale replicas
kubectl scale statefulset aurora-db-replicas --replicas=5 -n aurora-db

# Scale primary (usually 1)
kubectl scale statefulset aurora-db-primary --replicas=1 -n aurora-db
```

### Monitoring

```bash
# Port forward Grafana
kubectl port-forward svc/grafana 3000:3000 -n aurora-db

# Access Grafana at http://localhost:3000 (admin/aurora)
```

## 🖥️ CLI Tool

### Installation

```bash
# Build CLI
cd deployment/cli
cargo build --release

# Install globally
sudo cp target/release/aurora-cli /usr/local/bin/
```

### Usage Examples

```bash
# Connect to database
aurora-cli -h localhost -p 5432 -U aurora -d aurora

# Get status
aurora-cli status

# Execute query
aurora-cli query "SELECT * FROM users LIMIT 10"

# List tables
aurora-cli tables

# Show table schema
aurora-cli schema users

# Create table
aurora-cli create-table --name products --columns "id:INTEGER,name:VARCHAR(100),price:DECIMAL"

# Get metrics
aurora-cli metrics

# Cluster management
aurora-cli cluster status
aurora-cli cluster nodes

# JIT management
aurora-cli jit status
aurora-cli jit cache

# Backup and restore
aurora-cli backup --output /backup/aurora-$(date +%Y%m%d).sql
aurora-cli restore --input /backup/aurora-20231201.sql
```

### CLI Options

```
Usage: aurora-cli [OPTIONS] [COMMAND]

Commands:
  status         Show database status and health
  query          Execute SQL query
  tables         List database tables
  schema         Show table schema
  create-table   Create a new table
  metrics        Show database metrics
  backup         Create database backup
  restore        Restore database from backup
  users          Manage database users
  cluster        Cluster management
  jit            JIT compilation management
  maintenance    Database maintenance
  help           Print this message or the help of the given subcommand(s)

Options:
  -h, --host <HOST>        Database host [default: localhost]
  -p, --port <PORT>        Database port [default: 5432]
  -U, --user <USER>        Database user [default: aurora]
  -W, --password           Prompt for password
  -d, --database <DB>      Database name [default: aurora]
  -f, --format <FORMAT>    Output format (table, json, csv) [default: table]
      --help               Print help
```

## ⚙️ Configuration

### Production Configuration

The `config/production.toml` file contains all production settings:

```toml
[database]
max_connections = 1000
buffer_pool_size = "2GB"

[jit]
enabled = true
optimization_level = "aggressive"
cache_size_mb = 512

[cluster]
mode = "cluster"
replication_factor = 3
```

### Environment Variables

Override configuration with environment variables:

```bash
export AURORA_MAX_CONNECTIONS=2000
export AURORA_JIT_ENABLED=false
export AURORA_LOG_LEVEL=debug
```

## 📊 Monitoring

### Metrics Endpoints

- **Health Check**: `GET /health`
- **Metrics**: `GET /metrics` (Prometheus format)
- **Status**: `GET /api/status` (JSON)

### Key Metrics

- `aurora_connections_active`: Active connections
- `aurora_queries_total`: Total queries executed
- `aurora_jit_compilations_total`: JIT compilations
- `aurora_memory_usage_bytes`: Memory usage
- `aurora_disk_usage_bytes`: Disk usage

### Grafana Dashboards

Pre-configured dashboards for:
- Database performance
- Query latency
- Resource utilization
- Cluster health
- JIT compilation metrics

## 🔒 Security

### TLS Configuration

```toml
[security]
enable_ssl = true
ssl_cert_file = "/etc/ssl/certs/aurora.crt"
ssl_key_file = "/etc/ssl/private/aurora.key"
tls_min_version = "TLSv1.2"
```

### Authentication

```bash
# Create admin user
aurora-cli users create --username admin --password $(openssl rand -base64 32)

# Enable audit logging
aurora-cli query "ALTER SYSTEM SET enable_audit_log = true"
```

## 🔧 Troubleshooting

### Common Issues

1. **Connection Refused**
   ```bash
   # Check if AuroraDB is running
   docker ps | grep aurora

   # Check logs
   docker logs aurora-primary
   ```

2. **High Memory Usage**
   ```bash
   # Check buffer pool settings
   aurora-cli metrics | grep buffer_pool

   # Adjust configuration
   export AURORA_BUFFER_POOL_SIZE=1GB
   ```

3. **Slow Queries**
   ```bash
   # Check JIT status
   aurora-cli jit status

   # Enable JIT if disabled
   aurora-cli query "ALTER SYSTEM SET jit_enabled = true"
   ```

### Performance Tuning

```bash
# Increase buffer pool
export AURORA_BUFFER_POOL_SIZE=4GB

# Enable aggressive JIT
export AURORA_JIT_OPTIMIZATION_LEVEL=aggressive

# Tune connection pooling
export AURORA_MAX_CONNECTIONS=2000
```

## 📈 Scaling

### Horizontal Scaling

```bash
# Add more replicas
kubectl scale statefulset aurora-db-replicas --replicas=10 -n aurora-db

# Use read/write splitting
# Primary: aurora-db-service (write)
# Replicas: aurora-db-read-service (read)
```

### Vertical Scaling

```bash
# Increase CPU/memory
kubectl patch statefulset aurora-db-primary -n aurora-db \
  --type='json' \
  -p='[{"op": "replace", "path": "/spec/template/spec/containers/0/resources/requests/memory", "value": "4Gi"}]'
```

## 🔄 Backup & Recovery

### Automated Backups

```bash
# Enable automated backups
aurora-cli query "ALTER SYSTEM SET backup_enabled = true"
aurora-cli query "ALTER SYSTEM SET backup_interval_hours = 6"
```

### Manual Backup

```bash
# Create backup
aurora-cli backup --output /backup/aurora-$(date +%Y%m%d_%H%M%S).sql

# Restore from backup
aurora-cli restore --input /backup/aurora-20231201_120000.sql
```

## 🆘 Support

### Logs

```bash
# Docker logs
docker logs aurora-primary

# Kubernetes logs
kubectl logs -f deployment/aurora-db-primary -n aurora-db

# CLI logs
aurora-cli query "SELECT * FROM aurora_logs ORDER BY timestamp DESC LIMIT 100"
```

### Health Checks

```bash
# Quick health check
curl http://localhost:8080/health

# Detailed status
aurora-cli status

# Cluster health
aurora-cli cluster status
```

---

## 🎯 UNIQUENESS Features Enabled

This deployment enables AuroraDB's UNIQUENESS features:

- **🚀 JIT Performance**: LLVM-based query compilation with SIMD acceleration
- **🔄 Smart Caching**: Intelligent query result caching with dependency tracking
- **📊 Advanced Monitoring**: Real-time metrics and performance profiling
- **☸️ Cloud-Native**: Kubernetes-native deployment with auto-scaling
- **🔒 Enterprise Security**: TLS encryption, RBAC, and audit logging
- **📈 Auto-Scaling**: Horizontal scaling with load balancing
- **💾 Intelligent Storage**: Hybrid B+ tree/LSM with compression
- **🔍 Vector Search**: Built-in similarity search capabilities

**Ready to deploy AuroraDB and experience breakthrough database performance!** 🚀✨
