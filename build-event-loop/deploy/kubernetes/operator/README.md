# Cyclone Kubernetes Operator

**Enterprise-Grade Kubernetes Management for Cyclone Applications**

The Cyclone Operator provides advanced Kubernetes-native management for Cyclone applications, enabling automated deployment, scaling, monitoring, and optimization of 2M+ RPS services.

## 🚀 Features

### Intelligent Application Management
- **Automated Deployment**: Single CRD deploys complete Cyclone stack
- **Performance Optimization**: Automatic tuning based on workload patterns
- **Health Management**: Comprehensive health checks and self-healing
- **Configuration Management**: Hot-reload configuration without restarts

### Advanced Scaling & Performance
- **Intelligent Autoscaling**: RPS-based and CPU-based scaling decisions
- **NUMA-Aware Scheduling**: Optimal core placement for performance
- **Resource Optimization**: Dynamic resource allocation based on metrics
- **Circuit Breaker Integration**: Automatic fault tolerance management

### Enterprise Observability
- **Prometheus Integration**: Native metrics collection and export
- **Structured Logging**: Kubernetes-native log aggregation
- **Distributed Tracing**: Request tracing across microservices
- **Performance Analytics**: Real-time RPS, latency, and throughput monitoring

### Production Reliability
- **Pod Disruption Budgets**: Guaranteed availability during updates
- **Rolling Updates**: Zero-downtime deployments with health checks
- **Security Contexts**: Least-privilege container execution
- **Network Policies**: Secure inter-service communication

## 📦 Installation

### Install Operator Lifecycle Manager (OLM)

```bash
# Install OLM (if not already installed)
kubectl apply -f https://github.com/operator-framework/operator-lifecycle-manager/releases/download/v0.23.1/crds.yaml
kubectl apply -f https://github.com/operator-framework/operator-lifecycle-manager/releases/download/v0.23.1/olm.yaml
```

### Install Cyclone Operator

```bash
# Create namespace
kubectl create namespace cyclone-system

# Install CRDs
kubectl apply -f deploy/kubernetes/operator/crds/

# Install operator
kubectl apply -f deploy/kubernetes/operator/cyclone-operator.yaml

# Verify installation
kubectl get pods -n cyclone-system
kubectl get crd | grep cyclone
```

## 🚀 Deploying Cyclone Applications

### Simple Deployment

```bash
# Deploy a basic Cyclone web application
kubectl apply -f - <<EOF
apiVersion: cyclone.dev/v1
kind: CycloneApp
metadata:
  name: my-cyclone-app
spec:
  version: "2.0.0"
  replicas: 3
  targetRPS: 1000000
  networking:
    enableRDMA: true
    enableDPDK: true
    enableXDP: true
EOF
```

### High-Performance API Deployment

```bash
# Deploy with full enterprise features
kubectl apply -f deploy/kubernetes/operator/examples/high-performance-api.yaml
```

### Custom Configuration

```yaml
apiVersion: cyclone.dev/v1
kind: CycloneApp
metadata:
  name: custom-cyclone-app
spec:
  version: "2.0.0"
  replicas: 5
  image: "my-registry/cyclone-web:v2.0.0"
  targetRPS: 2000000
  maxConnections: 500000

  networking:
    enableRDMA: true
    enableDPDK: true
    enableXDP: true
    zeroCopy: true

  resources:
    requests:
      cpu: "2000m"
      memory: "2Gi"
    limits:
      cpu: "4000m"
      memory: "4Gi"

  tls:
    enabled: true
    certificateSecret: "my-tls-secret"

  autoscaling:
    enabled: true
    minReplicas: 3
    maxReplicas: 50
    targetCPUUtilizationPercentage: 70
    targetRPS: 150000

  circuitBreaker:
    enabled: true
    failureThreshold: 10
    successThreshold: 5
    timeoutSeconds: 300
```

## 📊 Monitoring & Observability

### Prometheus Metrics

The operator automatically exposes comprehensive metrics:

```prometheus
# Application metrics
cyclone_app_requests_total{app="my-cyclone-app"} 1500000
cyclone_app_request_duration_seconds{quantile="0.95"} 0.005
cyclone_app_active_connections 25000

# Networking metrics
cyclone_rdma_operations_total 5000000
cyclone_dpdk_packets_processed_total 10000000
cyclone_xdp_packets_filtered_total 500000

# System metrics
cyclone_cpu_utilization 0.75
cyclone_memory_utilization 0.60
cyclone_network_throughput_gbps 25.5
```

### Grafana Dashboards

Pre-built dashboards are available:

```bash
# Install Cyclone dashboards
kubectl apply -f deploy/kubernetes/monitoring/dashboards/
```

### Custom Metrics

Add custom metrics to your Cyclone applications:

```rust
use cyclone::metrics::Counter;

// In your application
let requests = Counter::new("custom_requests_total");
metrics.register_counter("custom_requests", requests);

// Increment in request handlers
requests.increment();
```

## 🔧 Configuration Management

### Hot Configuration Reload

Update configuration without restarting:

```bash
# Update ConfigMap
kubectl patch configmap cyclone-api-config --type merge -p '{
  "data": {
    "config.toml": "... new config ..."
  }
}'

# Operator automatically reloads configuration
```

### Environment-Specific Configurations

```yaml
apiVersion: cyclone.dev/v1
kind: CycloneApp
metadata:
  name: production-api
spec:
  # Production-optimized settings
  targetRPS: 2000000
  replicas: 10
  resources:
    limits:
      cpu: "8000m"
      memory: "8Gi"
---
apiVersion: cyclone.dev/v1
kind: CycloneApp
metadata:
  name: staging-api
spec:
  # Development settings
  targetRPS: 100000
  replicas: 2
  resources:
    limits:
      cpu: "1000m"
      memory: "1Gi"
```

## 🚨 Troubleshooting

### Check Operator Status

```bash
# Check operator health
kubectl get pods -n cyclone-system
kubectl logs -n cyclone-system deployment/cyclone-operator

# Check CRD status
kubectl get crd cycloneapps.cyclone.dev
kubectl describe crd cycloneapps.cyclone.dev
```

### Application Debugging

```bash
# Check application status
kubectl get cycloneapps
kubectl describe cycloneapp my-cyclone-app

# View application logs
kubectl logs -l app=my-cyclone-app

# Check metrics
kubectl port-forward svc/cyclone-api-service 9090:9090
curl http://localhost:9090/metrics
```

### Performance Issues

```bash
# Check resource utilization
kubectl top pods -l app=my-cyclone-app

# Analyze network performance
kubectl exec -it cyclone-pod -- cyclone profile network

# Check for scaling events
kubectl describe hpa cyclone-api-hpa
```

## 🔒 Security

### Pod Security Standards

```yaml
# Enforce restricted security context
apiVersion: cyclone.dev/v1
kind: CycloneApp
metadata:
  name: secure-app
spec:
  securityContext:
    runAsNonRoot: true
    runAsUser: 65534
    allowPrivilegeEscalation: false
    capabilities:
      drop: ["ALL"]
    seccompProfile:
      type: "RuntimeDefault"
```

### Network Policies

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: cyclone-api-policy
spec:
  podSelector:
    matchLabels:
      app: cyclone-api
  policyTypes:
  - Ingress
  - Egress
  ingress:
  - from:
    - namespaceSelector:
        matchLabels:
          name: ingress-nginx
    ports:
    - protocol: TCP
      port: 3000
```

## 📈 Performance Tuning

### CPU Optimization

```yaml
apiVersion: cyclone.dev/v1
kind: CycloneApp
metadata:
  name: optimized-app
spec:
  affinity:
    nodeAffinity:
      requiredDuringSchedulingIgnoredDuringExecution:
        nodeSelectorTerms:
        - matchExpressions:
          - key: kubernetes.io/os
            operator: In
            values:
            - linux
          - key: cyclone.dev/rdma-capable
            operator: In
            values:
            - "true"
  tolerations:
  - key: "cyclone.dev/high-performance"
    operator: "Equal"
    value: "true"
    effect: "NoSchedule"
```

### Memory Optimization

```yaml
spec:
  resources:
    requests:
      memory: "2Gi"
      hugepages-2Mi: "1Gi"
    limits:
      memory: "4Gi"
      hugepages-2Mi: "2Gi"
  securityContext:
    hugepages:
    - mountPath: /dev/hugepages
      mountPropagation: HostToContainer
```

### Network Optimization

```yaml
spec:
  networking:
    enableRDMA: true
    enableDPDK: true
    enableXDP: true
  hostNetwork: true  # For maximum performance (use carefully)
  dnsPolicy: ClusterFirstWithHostNet
```

## 🔄 Upgrades & Rollbacks

### Rolling Updates

```bash
# Update application version
kubectl patch cycloneapp my-app --type merge -p '{
  "spec": {
    "version": "2.1.0",
    "image": "cyclone/cyclone-web:v2.1.0"
  }
}'

# Monitor rollout
kubectl rollout status deployment/my-app
```

### Rollbacks

```bash
# Rollback to previous version
kubectl rollout undo deployment/my-app

# Or rollback via operator
kubectl patch cycloneapp my-app --type merge -p '{
  "spec": {
    "version": "2.0.0",
    "image": "cyclone/cyclone-web:v2.0.0"
  }
}'
```

## 📚 API Reference

### CycloneApp Spec

| Field | Type | Description | Default |
|-------|------|-------------|---------|
| `version` | string | Cyclone version | "latest" |
| `replicas` | int | Number of replicas | 3 |
| `targetRPS` | int | Target RPS for optimization | 2000000 |
| `networking.enableRDMA` | bool | Enable RDMA acceleration | true |
| `networking.enableDPDK` | bool | Enable DPDK processing | true |
| `networking.enableXDP` | bool | Enable XDP protection | true |
| `autoscaling.enabled` | bool | Enable HPA | true |
| `circuitBreaker.enabled` | bool | Enable circuit breaker | true |

### Status Fields

| Field | Description |
|-------|-------------|
| `phase` | Current deployment phase |
| `replicas` | Current replica count |
| `performance.currentRPS` | Current RPS achieved |
| `performance.latencyP95` | 95th percentile latency |
| `networking.activeConnections` | Active connections |

## 🤝 Contributing

See [CONTRIBUTING.md](../../CONTRIBUTING.md) for development guidelines.

## 📄 License

Licensed under MIT License. See [LICENSE](../../LICENSE) for details.
