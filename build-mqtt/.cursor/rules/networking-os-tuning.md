# Networking and OS Tuning

## Socket Options
* TCP_NODELAY, SO_REUSEPORT, TCP_KEEPIDLE/INTVL/CNT
* Adequate socket buffers per workload characteristics

## Limits
* rlimits for file descriptors; thread pinning on NUMA

## Kernel Parameters
* net.core.somaxconn, net.ipv4.ip_local_port_range, backlog sizes

## IO Models
* epoll/kqueue edge vs. level triggered guidance

## Testing
* Validate tuning under load; watch drops, retransmits, and latency tails
